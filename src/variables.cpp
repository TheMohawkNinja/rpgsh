#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/var.h"

Character c = Character(false);
Campaign m = Campaign();
Shell s = Shell();

template<typename T>
void check_key(Scope scope, std::string key)
{
	for(const auto& [k,v] : scope.getDatamap<T>())
	{
		if(k == key)
		{
			fprintf(stdout,"%s\n",scope.getStr<T>(key).c_str());
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
void append_output(std::map<std::string,std::string> map, std::string key, std::string* output)
{
	for(const auto& [k,v] : map)
	{
		// Get root variable if it exists
		if(("/"+k.substr(0,key.length()-1)+"/") == key)
			output->append(k + DS + v + DS);

		// Get all subkeys
		if("/"+k.substr(0,key.length()-1) == key)
			output->append(k + DS + v + DS);
	}
}
int main(int argc, char** argv)
{
	check_print_app_description(argv,"Not meant for direct call by user. Implicitly called when modifying variables.");

	if(std::string(argv[1]).length() == 1)// If the user only enters the scope sigil
	{
		output(Error,"No variable type specified.");
		exit(-1);
	}

	std::string arg(argv[1]);
	std::string key = arg.substr(arg.find("/"),arg.length()-(arg.find("/")));
	Scope scope;

	// Check scope sigil
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

	if(argc == 2)// If the user just submits a variable...
	{
		unsigned int space = 0;

		if(key[key.length()-1] != '/')// ...and the last character isn't a '/', just print value
		{
			switch(argv[1][1])
			{
				case CURRENCY_SIGIL:
					fprintf(stdout,"%s\n",scope.getStr<Currency>(key).c_str());
					exit(0);
				case CURRENCYSYSTEM_SIGIL:
					fprintf(stdout,"%s\n",scope.getStr<CurrencySystem>(key).c_str());
					exit(0);
				case DICE_SIGIL:
					fprintf(stdout,"%s\n",scope.getStr<Dice>(key).c_str());
					exit(0);
				case VAR_SIGIL:
					fprintf(stdout,"%s\n",scope.getStr<Var>(key).c_str());
					exit(0);
				case WALLET_SIGIL:
					fprintf(stdout,"%s\n",scope.getStr<Wallet>(key).c_str());
					exit(0);
				case '/':
					// If a type specifier is not specified, check for match.
					// Specifically, check in order of what is most to least likely
					check_key<Var>(scope,key);
					check_key<Dice>(scope,key);
					check_key<Wallet>(scope,key);
					check_key<Currency>(scope,key);
					check_key<CurrencySystem>(scope,key);

					// If we get here, there was no match, so just output nothing and exit like we would for any other non-existent variable
					exit(0);
				default:
					output(Error,"Unknown type specifier \'%c\' in \"%s\"",arg[1],arg.c_str());
					exit(-1);
			}
		}
		else// ...and the last character is a '/', print a list of keys and constructors
		{
			int space = 0;
			std::map<std::string,std::string> c_map;
			std::map<std::string,std::string> cs_map;
			std::map<std::string,std::string> d_map;
			std::map<std::string,std::string> v_map;
			std::map<std::string,std::string> w_map;

			// When printing entire containers, treat type sigil as a filter
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

			// Create output string from map
			std::string output = "";
			append_output(c_map,key,&output);
			append_output(cs_map,key,&output);
			append_output(d_map,key,&output);
			append_output(v_map,key,&output);
			append_output(w_map,key,&output);

			// Cut off the extraneous DS
			output = output.substr(0,output.length()-DS.length());
			fprintf(stdout,"%s\n",output.c_str());
		}
	}
	else if(argc == 3)// Unary operators
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
	else// Binary operators
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
