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
	int period = key.find('.');
	int r_period = key.rfind('.');
	int slash = key.find('/');
	if(period == (int)std::string::npos)//If any periods are before the last '/', print the explicit constructor
	{
		key = right(key,key.find('/')+1);
		fprintf(stdout,"%s\n",scope.getStr<T>(key).c_str());
		exit(0);
	}
	else//Otherwise, print the requested property
	{
		std::string property = key.substr(r_period+1,key.length()-(r_period+1));
		key = key.substr(slash+1,r_period-(slash+1));
		fprintf(stdout,"%s\n",scope.getProperty<T>(key,property).c_str());
		exit(0);
	}
}
template<typename T>
void get_map(Scope scope, std::map<std::string,std::string>* p_map)
{
	for(const auto& [k,v] : scope.getDatamap<T>())
		(*p_map)[k] = scope.getStr<T>(k);
}
void append_output(std::map<std::string,std::string> map, std::string key, std::string* pOutput)
{
	for(const auto& [k,v] : map)
	{
		//Get root variable if it exists
		if(!stringcasecmp(("/"+left(k,key.length()-1)+"/"),key))
			pOutput->append(k + DS + v + DS);

		//Get all subkeys
		if(!stringcasecmp(("/"+left(k,key.length()-1)),key))
			pOutput->append(k + DS + v + DS);
	}
}
std::string getLikeFileName(std::string chk_file,std::string chk_dir,bool is_dir,std::string xref)
{
	for(const auto& entry : getDirectoryListing(chk_dir))
	{
		if(is_dir &&
		   !stringcasecmp(entry,chk_file) &&
		   std::filesystem::is_directory(campaigns_dir+entry))
			return entry;
		else if(!is_dir &&
			!stringcasecmp(entry,chk_file) &&
			std::filesystem::is_regular_file(chk_dir+entry))
			return entry;
	}
	output(Error,"Invalid xref \"%s\".",xref.c_str());
	exit(-1);
}
std::string load_external_reference(std::string arg, Scope* p_scope)
{
	//Ending square bracket not found
	if(arg.find(']') == std::string::npos)
	{
		output(Error,"No terminating \']\' found for xref.");
		exit(-1);
	}

	//Get string between the square brackets
	std::string xref = arg.substr(arg.find('[')+1,arg.find(']')-(arg.find('[')+1));

	//Actually load the xref into the scope
	std::vector<std::string> campaigns;
	std::string xref_dir = campaigns_dir+
			       get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
			       "characters/";
	std::string xref_char = xref;
	std::string campaign = "";
	switch(arg[0])
	{
		case CHARACTER_SIGIL:
			if(xref.find('/') != std::string::npos)//Attempting to get a character from another campaign
			{
				campaign = left(xref,xref.find('/'));
				xref_char = right(xref,xref.find('/')+1);
				xref_dir = campaigns_dir+
					   getLikeFileName(campaign,campaigns_dir,true,xref)+
					   "/characters/";
			}

			p_scope->load(xref_dir+
				      getLikeFileName(xref_char+".char",xref_dir,false,xref));
			break;
		case CAMPAIGN_SIGIL:
			p_scope->load(campaigns_dir+
				      getLikeFileName(xref,campaigns_dir,true,xref)+
				      "/"+
				      variable_file_name);
			break;
		case SHELL_SIGIL:
			output(Error,"Cannot use xref with shell scope.");
			exit(-1);
	}

	//Remove external reference string so we can continue to use the current arg under the new context
	arg = arg[0]+left(arg,arg.find('[')-1)+right(arg,arg.find(']')+1);
	return arg;
}
bool isUnaryOp(std::string op)
{
	return (op == "++" ||
		op == "--");
}
bool isArithmeticOp(std::string op)
{
	return (op == "+" ||
		op == "-" ||
		op == "*" ||
		op == "/" ||
		op == "^" ||//TODO document
		op == "%");//TODO document
}
bool isAssignmentOp(std::string op)
{
	return (op == "==" ||
		op == "+=" ||
		op == "-=" ||
		op == "*=" ||
		op == "/=" ||
		op == "^=");//TODO document
}
bool isRelationalOp(std::string op)//TODO document
{
	return (op == "="  ||
		op == "<"  ||
		op == ">"  ||
		op == "<=" ||
		op == ">=" ||
		op == "!=");
}
template<typename T>
T performModOp(Scope scope, std::string key, std::string property, std::string op, std::string rhs_str)//Unary, Arithmetic, and Assignment
{
	T old_value = scope.get<T>(key);
	T new_value = T();

	if(op == "++")
	{
		new_value = old_value;
		try{new_value++;}
		catch(...){output(Error,"Invalid operation: \"%s ++\"",old_value.c_str());}
	}
	else if(op == "--")
	{
		new_value = old_value;
		try{new_value--;}
		catch(...){output(Error,"Invalid operation: \"%s --\"",old_value.c_str());}
	}

	if(new_value != old_value)
		output(Info,"\"%s\" has changed from \"%s\" to \"%s\"",key.c_str(),std::string(old_value).c_str(),std::string(new_value).c_str());

	return new_value;
}

int main(int argc, char** argv)
{
	check_print_app_description(argv,"Not meant for direct call by user. Implicitly called when modifying variables.");

	if(std::string(argv[1]).find('/') == std::string::npos)//If the user only enters the scope sigil
	{
		output(Error,"Expected at least one '/' delimiter.");
		exit(-1);
	}

	Scope scope;
	std::string variable(argv[1]);
	std::string chk_str = "";

	//Check if user is requesting a character xref from a different campaign.
	if((int)variable.find(']') > (int)variable.find('/'))
		chk_str = right(variable,variable.find(']')+1);
	else
		chk_str = right(variable,variable.find('/'));

	//Check scope sigil
	switch(variable[0])
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

	//Check for external references
	if(variable[1] == '[')
		variable = load_external_reference(variable,&scope);

	if(argc == 2)//If the user just submits a variable...
	{
		std::string key = "";
		key = right(chk_str,chk_str.find('/')+1);
		if(chk_str.find('.') != std::string::npos)
			key = left(key,key.find('.'));

		if(chk_str[chk_str.length()-1] != '/')//...and the last character isn't a '/', just print value
		{
			switch(variable[1])
			{
				case CURRENCY_SIGIL:
					if(scope.keyExists<Currency>(key))
						print_requested_data<Currency>(scope,chk_str);
					break;
				case CURRENCYSYSTEM_SIGIL:
					if(scope.keyExists<CurrencySystem>(key))
						print_requested_data<CurrencySystem>(scope,chk_str);
					break;
				case DICE_SIGIL:
					if(scope.keyExists<Dice>(key))
						print_requested_data<Dice>(scope,chk_str);
					break;
				case VAR_SIGIL:
					if(scope.keyExists<Var>(key))
						print_requested_data<Var>(scope,chk_str);
					break;
				case WALLET_SIGIL:
					if(scope.keyExists<Wallet>(key))
						print_requested_data<Wallet>(scope,chk_str);
					break;
				case '/':
					//If a type specifier is not specified, check for match.
					//Specifically, check in order of what is most to least likely
					if(scope.keyExists<Var>(key))
						print_requested_data<Var>(scope,chk_str);
					else if(scope.keyExists<Dice>(key))
						print_requested_data<Dice>(scope,chk_str);
					else if(scope.keyExists<Wallet>(key))
						print_requested_data<Wallet>(scope,chk_str);
					else if(scope.keyExists<Currency>(key))
						print_requested_data<Currency>(scope,chk_str);
					else if(scope.keyExists<CurrencySystem>(key))
						print_requested_data<CurrencySystem>(scope,chk_str);
					break;
				default:
					output(Error,"Unknown type specifier \'%c\' in \"%s\"",variable[1],variable.c_str());
					exit(-1);
			}
		}
		else//...and the last character is a '/', print a list of keys and constructors
		{
			std::map<std::string,std::string> c_map;
			std::map<std::string,std::string> cs_map;
			std::map<std::string,std::string> d_map;
			std::map<std::string,std::string> v_map;
			std::map<std::string,std::string> w_map;

			//When printing entire containers, treat type sigil as a filter
			switch(variable[1])
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
					output(Error,"Unknown type specifier \'%c\' in \"%s\"",variable[1],variable.c_str());
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
			output = left(output,output.length()-DS.length());
			fprintf(stdout,"%s\n",output.c_str());
		}
	}
	else if(argc == 3)//Unary operators
	{
		std::string op(argv[2]);
		std::string key = "";
		std::string property = "";

		if((int)variable.find('.') < (int)variable.rfind('/'))
		{
			key = right(variable,variable.find('/')+1);
		}
		else
		{
			key = right(variable,variable.find('/')+1);
			key = left(key,key.find('.'));
			property = right(variable,variable.find('.')+1);
		}
		if(isUnaryOp(op))
		{
			switch(variable[1])
			{
				case CURRENCY_SIGIL:
					scope.set<Currency>(key,performModOp<Currency>(scope, key, property, op, ""));
				break;
				case CURRENCYSYSTEM_SIGIL:
					scope.set<CurrencySystem>(key,performModOp<CurrencySystem>(scope, key, property, op, ""));
				break;
				case DICE_SIGIL:
					scope.set<Dice>(key,performModOp<Dice>(scope, key, property, op, ""));
				break;
				case VAR_SIGIL:
					scope.set<Var>(key,performModOp<Var>(scope, key, property, op, ""));
				break;
				case WALLET_SIGIL:
					scope.set<Wallet>(key,performModOp<Wallet>(scope, key, property, op, ""));
				break;
				case '/':
					if(scope.keyExists<Var>(key))
						scope.set<Var>(key,performModOp<Var>(scope, key, property, op, ""));
					else if(scope.keyExists<Dice>(key))
						scope.set<Dice>(key,performModOp<Dice>(scope, key, property, op, ""));
					else if(scope.keyExists<Wallet>(key))
						scope.set<Wallet>(key,performModOp<Wallet>(scope, key, property, op, ""));
					else if(scope.keyExists<Currency>(key))
						scope.set<Currency>(key,performModOp<Currency>(scope, key, property, op, ""));
					else if(scope.keyExists<CurrencySystem>(key))
						scope.set<CurrencySystem>(key,performModOp<CurrencySystem>(scope, key, property, op, ""));
				break;
			}
		}
		scope.save();
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
