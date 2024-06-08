#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/var.h"

Character c = Character(false);
Campaign m = Campaign();
Shell s = Shell();

template<typename T>
void print_requested_data(Scope scope, std::string key)
{
	//Directly evaulating the methods doesn't work for some reason, so we have to use interim variables, weird...
	int period = key.find(".");
	int slash = key.rfind("/");
	if(period < slash)//If any periods are before the last '/', print the explicit constructor
	{
		key = key.substr(1,key.length()-1);
		fprintf(stdout,"%s\n",scope.getStr<T>(key).c_str());
	}
	else//Otherwise, print the requested property
	{
		std::string property = key.substr(key.rfind(".")+1,
						  key.length()-(key.rfind(".")+1));
		key = key.substr(1,key.rfind(".")-1);
		fprintf(stdout,"%s\n",scope.getProperty<T>(key,property).c_str());
	}
}
template<typename T>
void check_key(Scope scope, std::string key)
{
	int period = key.find(".");
	int slash = key.rfind("/");
	for(const auto& [k,v] : scope.getDatamap<T>())
	{
		std::string chk_str = "";
		if(period < slash)
			chk_str = key.substr(1,key.length()-1);
		else
			chk_str = key.substr(1,key.rfind(".")-1);

		if(!stringcasecmp(chk_str,k))
		{
			chk_str = "/"+k;
			if(period > slash)
				chk_str+=key.substr(period,key.length()-period);
			print_requested_data<T>(scope,chk_str);
			exit(0);
		}
	}
}
template<typename T>
void get_map(Scope scope, std::map<std::string,std::string>* p_map)
{
	for(const auto& [k,v] : scope.getDatamap<T>())
		(*p_map)[k] = scope.getStr<T>(k);
}
void append_output(std::map<std::string,std::string> map, std::string key, std::string* p_output)
{
	for(const auto& [k,v] : map)
	{
		//Get root variable if it exists
		if(("/"+k.substr(0,key.length()-1)+"/") == key)
			p_output->append(k + DS + v + DS);

		//Get all subkeys
		if("/"+k.substr(0,key.length()-1) == key)
			p_output->append(k + DS + v + DS);
	}
}
std::string load_external_reference(std::string arg, Scope* p_scope)
{
	//Ending square bracket not found
	if(arg.find(']') == std::string::npos)
	{
		output(Error,"No terminating \']\' found for external reference.");
		exit(-1);
	}

	//Get string between the square brackets
	std::string xref = arg.substr(arg.find('[')+1,
				      arg.find(']')-(arg.find('[')+1));

	//Current campaign character directory
	std::string cur_camp_char_dir = campaigns_dir + get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR) + "characters/";

	//Actually load the xref into the scope
	std::vector<std::string> dir_list;
	std::string file = "";
	std::string xref_path = "";
	switch(arg[0])
	{
		case CHARACTER_SIGIL:
			dir_list = getDirectoryListing(cur_camp_char_dir);
			for(int i=0; i<dir_list.size(); i++)
			{
				file = dir_list[i].substr(0,dir_list[i].rfind("."));
				xref_path = cur_camp_char_dir+dir_list[i];
				if(!stringcasecmp(file,xref) &&
				   std::filesystem::is_regular_file(xref_path))
				{
					p_scope->load(xref_path);
					break;
				}
				else if(i == dir_list.size()-1)
				{
					output(Error,"Invalid xref \"%s\".",xref.c_str());
					exit(-1);
				}
			}
			break;
		case CAMPAIGN_SIGIL:
			dir_list = getDirectoryListing(campaigns_dir);
			for(int i=0; i<dir_list.size(); i++)
			{
				xref_path = campaigns_dir+dir_list[i];
				if(!stringcasecmp(dir_list[i],xref) &&
				   std::filesystem::is_directory(xref_path))
				{
					p_scope->load(xref_path+"/"+variable_file_name);
					break;
				}
				else if(i == dir_list.size()-1)
				{
					output(Error,"Invalid xref \"%s\".",xref.c_str());
					exit(-1);
				}
			}
			break;
		case SHELL_SIGIL:
			output(Error,"Cannot use an external reference with shell scope sigil.");
			exit(-1);
	}

	//Remove external reference string so we can continue to use the current arg under the new context
	arg = arg[0]+
	      arg.substr(0,arg.find('[')-1)+
	      arg.substr(arg.find(']')+1,arg.length()-(arg.find(']')+1));
	return arg;
}

int main(int argc, char** argv)
{
	check_print_app_description(argv,"Not meant for direct call by user. Implicitly called when modifying variables.");

	if(std::string(argv[1]).find("/") == std::string::npos)//If the user only enters the scope sigil
	{
		output(Error,"Expected at least one '/' delimiter.");
		exit(-1);
	}

	std::string arg(argv[1]);
	std::string key = arg.substr(arg.find("/"),arg.length()-(arg.find("/")));
	Scope scope;

	//Check scope sigil
	switch(argv[1][0])
	{
		case CHARACTER_SIGIL:
			scope = c;
			break;
		case CAMPAIGN_SIGIL:
			scope = m;
			break;
		case SHELL_SIGIL:
			scope = s;
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",argv[1][0]);
			exit(-1);
	}

	if(argc == 2)//If the user just submits a variable...
	{
		unsigned int space = 0;

		if(key[key.length()-1] != '/')//...and the last character isn't a '/', just print value
		{
			//Check for external references
			if(arg[1] == '[')
				arg = load_external_reference(arg,&scope);

			switch(arg[1])
			{
				case CURRENCY_SIGIL:
					print_requested_data<Currency>(scope,key);
					exit(0);
				case CURRENCYSYSTEM_SIGIL:
					print_requested_data<CurrencySystem>(scope,key);
					exit(0);
				case DICE_SIGIL:
					print_requested_data<Dice>(scope,key);
					exit(0);
				case VAR_SIGIL:
					print_requested_data<Var>(scope,key);
					exit(0);
				case WALLET_SIGIL:
					print_requested_data<Wallet>(scope,key);
					exit(0);
				case '/':
					//If a type specifier is not specified, check for match.
					//Specifically, check in order of what is most to least likely
					check_key<Var>(scope,key);
					check_key<Dice>(scope,key);
					check_key<Wallet>(scope,key);
					check_key<Currency>(scope,key);
					check_key<CurrencySystem>(scope,key);

					//If we get here, there was no match, so just output nothing and exit like we would for any other non-existent variable
					exit(0);
				default:
					output(Error,"Unknown type specifier \'%c\' in \"%s\"",arg[1],arg.c_str());
					exit(-1);
			}
		}
		else//...and the last character is a '/', print a list of keys and constructors
		{
			int space = 0;
			std::map<std::string,std::string> c_map;
			std::map<std::string,std::string> cs_map;
			std::map<std::string,std::string> d_map;
			std::map<std::string,std::string> v_map;
			std::map<std::string,std::string> w_map;

			//Check for external references
			if(arg[1] == '[')
				arg = load_external_reference(arg,&scope);

			//When printing entire containers, treat type sigil as a filter
			switch(arg[1])
			{
				case CURRENCY_SIGIL:
					get_map<Currency>(scope,&c_map);
					break;
				case CURRENCYSYSTEM_SIGIL:
					get_map<CurrencySystem>(scope,&cs_map);
					break;
				case DICE_SIGIL:
					get_map<Dice>(scope,&d_map);
					break;
				case VAR_SIGIL:
					get_map<Var>(scope,&v_map);
					break;
				case WALLET_SIGIL:
					get_map<Wallet>(scope,&w_map);
					break;
				case '/':
					get_map<Currency>(scope,&c_map);
					get_map<CurrencySystem>(scope,&cs_map);
					get_map<Dice>(scope,&d_map);
					get_map<Var>(scope,&v_map);
					get_map<Wallet>(scope,&w_map);
					break;
				default:
					output(Error,"Unknown type specifier \'%c\' in \"%s\"",arg[1],arg.c_str());
					exit(-1);
			}

			//Create output string from map
			std::string output = "";
			append_output(c_map,key,&output);
			append_output(cs_map,key,&output);
			append_output(d_map,key,&output);
			append_output(v_map,key,&output);
			append_output(w_map,key,&output);

			//Cut off the extraneous DS
			output = output.substr(0,output.length()-DS.length());
			fprintf(stdout,"%s\n",output.c_str());
		}
	}
	else if(argc == 3)//Unary operators
	{
		/*if(is_operator(std::string(argv[2])))
		{
			if(std::string(argv[2]) == "++")
			{
				set_variable(key,old_value,old_value+1,argv[1][0]);
				return 0;
			}
			else if(std::string(argv[2]) == "--")
			{
				set_variable(key,old_value,old_value-1,argv[1][0]);
				return 0;
			}
			else
			{
				output(Error,"Expected new value after \'%s\'.",argv[argc-1]);
				exit(-1);
			}
		}
		else
		{
			output(Error,"Expected operator before \'%s\'.",argv[argc-1]);
			exit(-1);
		}*/
	}
	else//Binary operators
	{
		/*Var new_value = std::string(argv[3]);
		for(int i=4; i<argc; i++)
		{
			if(i%2 == 1 && is_operator(argv[i]))
			{
				output(Error,"Expected non-operator new value at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 == 0 && !is_operator(argv[i]))
			{
				output(Error,"Expected operator at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 == 0)
			{
				Var current_operand = std::string(argv[i+1]);
				if(!strcmp(argv[i],"+"))
				{
					new_value += current_operand;
				}
				else if(!strcmp(argv[i],"-"))
				{
					new_value -= current_operand;
				}
				else if(!strcmp(argv[i],"*"))
				{
					new_value *= current_operand;
				}
				else if(!strcmp(argv[i],"/"))
				{
					new_value /= current_operand;
				}
				else
				{
					output(Error,"Invalid operator \'%s\'.",argv[i]);
					exit(-1);
				}
			}
		}

		if(!strcmp(argv[2],"="))
		{
			set_variable(key,old_value,new_value,argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"+="))
		{
			set_variable(key,old_value,std::string(old_value+new_value),argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"-="))
		{
			set_variable(key,old_value,std::string(old_value-new_value),argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"*="))
		{
			set_variable(key,old_value,std::string(old_value*new_value),argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"/="))
		{
			set_variable(key,old_value,std::string(old_value/new_value),argv[1][0]);
			return 0;
		}
		else
		{
			output(Error,"Invalid operator \'%s\'.",argv[2]);
			exit(-1);
		}*/
	}
	return 0;
}
