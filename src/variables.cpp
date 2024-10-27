#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/var.h"

Character c = Character(false);
Campaign m = Campaign();
Shell s = Shell();

std::vector<std::string> unaryOps = {"++","--"};
std::vector<std::string> arithOps = {"^","*","/","%","+","-"};
std::vector<std::string> assignOps = {"=","^=","*=","/=","%=","+=","-="};
std::vector<std::string> relationOps = {"==","~=","<",">","<=",">=","!=","~="};

void parseVariable(std::string v, std::string* pKey, std::string* pProperty)//Derive key and property values from the variable string
{
	(*pKey) = right(v,v.find('/')+1);
	if((int)v.find('.') > (int)v.rfind('/'))
	{
		(*pKey) = left((*pKey),pKey->find('.'));
		(*pProperty) = right(v,v.find('.')+1);
	}
}
template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map)
{
	for(const auto& [k,v] : scope.getDatamap<T>())
		(*p_map)[k] = scope.getStr<T>(k);
}
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput)
{
	for(const auto& [k,v] : map)
	{
		//Get root variable if it exists
		if(!stringcasecmp((left(k,key.length())+"/"),key))
			pOutput->append(k + DS + v + DS);

		//Get all subkeys
		if(!stringcasecmp((left(k,key.length())),key))
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
std::string loadXRef(std::string arg, Scope* p_scope)
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
template <typename T>
void tryGetProperty(Scope scope, std::string type_str, std::string key, std::string property, std::string* pOld_property)
{
	if(property == "") return;

	try
	{
		(*pOld_property) = scope.getProperty<T>(key,property);
	}
	catch(...)
	{
		output(Error,"\"%s\" is not a valid %s property.",property.c_str(),type_str.c_str());
		exit(-1);
	}
}
template<typename TL, typename TR>
TL doArithOps(TL lhs, std::string op, TR rhs)
{
	if(op == "^")
	{
		try{lhs = lhs^rhs;}
		catch(...){throw;}
	}
	else if(op == "*")
	{
		try{lhs = lhs*rhs;}
		catch(...){throw;}
	}
	else if(op == "/")
	{
		try{lhs = lhs/rhs;}
		catch(...){throw;}
	}
	else if(op == "%")
	{
		try{lhs = lhs%rhs;}
		catch(...){throw;}
	}
	else if(op == "+")
	{
		try{lhs = lhs+rhs;}
		catch(...){throw;}
	}
	else if(op == "-")
	{
		try{lhs = lhs-rhs;}
		catch(...){throw;}
	}
	else // We should only get here if I forgot to code an operation
	{
		throw std::runtime_error(E_UNKNOWN_OPERATION);
	}

	return lhs;
}
template<typename TL, typename TR>
std::string getResult(std::string lhs, std::string op, std::string rhs)
{
	//TODO: Handle other operators (and incorrect ones)?
	if(findInStrVect(arithOps,op,0) > -1)
	{
		try
		{
			return std::string(doArithOps<TL,TR>(TL(lhs),op,TR(rhs)));
		}
		catch(const std::runtime_error& e)
		{
			if(e.what() == std::string(E_INVALID_OPERATION))
				output(Error,"Invalid operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else if(e.what() == std::string(E_UNKNOWN_OPERATION))
				output(Error,"Unknown operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
		}
	}
	/*else if(findInStrVect(relationOps,op) > -1)
		return btos(doRelOp<TL,TR>(TL(lhs),op,TR(rhs)));*/

	return ""; //Suppress -Wreturn-type
}
void parseOperandsAndDoOp(std::vector<std::string>* v, unsigned int lhs_pos, unsigned int op_pos, unsigned int rhs_pos)
{
	//TODO Handle quote-wrapped strings to force string interpretation

	std::string result = "";

	fprintf(stdout,"Performing operation: %s %s %s\n",(*v)[lhs_pos].c_str(),(*v)[op_pos].c_str(),(*v)[rhs_pos].c_str());

	if(left((*v)[lhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
	{
		if(left((*v)[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
			result = getResult<Var,Var>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
			result = getResult<Var,Dice>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
			result = getResult<Var,Wallet>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
			result = getResult<Var,Currency>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
			result = getResult<Var,CurrencySystem>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else
		{
			try
			{
				int v_rhs = std::stoi((*v)[rhs_pos]);
				if(findInStrVect(arithOps,(*v)[op_pos],0) > -1)
					result = std::string(doArithOps<Var,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));
				/*else if(isRelationalOp((*v)[op_pos]))
					result = btos(doRelOp<Var,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));*/
			}
			catch(...)
			{
				result = getResult<Var,std::string>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
			}
		}
	}
	else if(left((*v)[lhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
	{
		if(left((*v)[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
			result = getResult<Dice,Var>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
			result = getResult<Dice,Dice>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
			result = getResult<Dice,Wallet>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
			result = getResult<Dice,Currency>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
			result = getResult<Dice,CurrencySystem>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else
		{
			try
			{
				int v_rhs = std::stoi((*v)[rhs_pos]);
				if(findInStrVect(arithOps,(*v)[op_pos],0) > -1)
					result = std::string(doArithOps<Dice,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));
				/*else if(isRelationalOp((*v)[op_pos]))
					result = btos(doRelOp<Dice,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));*/
			}
			catch(...)
			{
				result = getResult<Dice,std::string>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
			}
		}
	}
	else if(left((*v)[lhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
	{
		if(left((*v)[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
			result = getResult<Wallet,Var>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
			result = getResult<Wallet,Dice>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
			result = getResult<Wallet,Wallet>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
			result = getResult<Wallet,Currency>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
			result = getResult<Wallet,CurrencySystem>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else
		{
			try
			{
				int v_rhs = std::stoi((*v)[rhs_pos]);
				if(findInStrVect(arithOps,(*v)[op_pos],0) > -1)
					result = std::string(doArithOps<Wallet,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));
				/*else if(isRelationalOp((*v)[op_pos]))
					result = btos(doRelOp<Wallet,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));*/
			}
			catch(...)
			{
				result = getResult<Wallet,std::string>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
			}
		}
	}
	else if(left((*v)[lhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
	{
		if(left((*v)[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
			result = getResult<Currency,Var>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
			result = getResult<Currency,Dice>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
			result = getResult<Currency,Wallet>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
			result = getResult<Currency,Currency>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
			result = getResult<Currency,CurrencySystem>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else
		{
			try
			{
				int v_rhs = std::stoi((*v)[rhs_pos]);
				if(findInStrVect(arithOps,(*v)[op_pos],0) > -1)
					result = std::string(doArithOps<Currency,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));
				/*else if(isRelationalOp((*v)[op_pos]))
					result = btos(doRelOp<Currency,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));*/
			}
			catch(...)
			{
				result = getResult<Currency,std::string>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
			}
		}
	}
	else if(left((*v)[lhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
	{
		if(left((*v)[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
			result = getResult<CurrencySystem,Var>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
			result = getResult<CurrencySystem,Dice>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
			result = getResult<CurrencySystem,Wallet>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
			result = getResult<CurrencySystem,Currency>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
			result = getResult<CurrencySystem,CurrencySystem>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else
		{
			try
			{
				int v_rhs = std::stoi((*v)[rhs_pos]);
				if(findInStrVect(arithOps,(*v)[op_pos],0) > -1)
					result = std::string(doArithOps<CurrencySystem,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));
				/*else if(isRelationalOp((*v)[op_pos]))
					result = btos(doRelOp<CurrencySystem,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));*/
			}
			catch(...)
			{
				result = getResult<CurrencySystem,std::string>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
			}
		}
	}
	else//Treat non-explicit constructors as Var
	{
		if(left((*v)[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
			result = getResult<Var,Var>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
			result = getResult<Var,Dice>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
			result = getResult<Var,Wallet>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
			result = getResult<Var,Currency>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else if(left((*v)[rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
			result = getResult<Var,CurrencySystem>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
		else
		{
			try
			{
				int v_rhs = std::stoi((*v)[rhs_pos]);
				if(findInStrVect(arithOps,(*v)[op_pos],0) > -1)
					result = std::string(doArithOps<Var,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));
				/*else if(isRelationalOp((*v)[op_pos]))
					result = btos(doRelOp<Var,int>((*v)[lhs_pos],(*v)[op_pos],v_rhs));*/
			}
			catch(...)
			{
				result = getResult<Var,std::string>((*v)[lhs_pos],(*v)[op_pos],(*v)[rhs_pos]);
			}
		}
	}

	fprintf(stdout,"Result: \"%s\"\n",result.c_str());

	//Replace operators and operands with result
	v->erase(v->begin()+rhs_pos);
	v->erase(v->begin()+op_pos);
	v->erase(v->begin()+lhs_pos);
	v->insert(v->begin()+lhs_pos,result);

	for(const auto& s : (*v))
		fprintf(stdout,"\ts: \"%s\"\n",s.c_str());
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
	std::string scope_str = "";

	//Check scope sigil
	switch(variable[0])
	{
		case CHARACTER_SIGIL:
			scope = c;
			scope_str = "Character";
			break;
		case CAMPAIGN_SIGIL:
			scope = m;
			scope_str = "Campaign";
			break;
		case SHELL_SIGIL:
			scope = s;
			scope_str = "Shell";
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",argv[1][0]);
			exit(-1);
	}

	//Check for external references
	if(variable[1] == '[')
		variable = loadXRef(variable,&scope);

	std::string key = "";
	std::string property = "";
	parseVariable(variable,&key,&property);

	if(argc == 2)//If the user just submits a variable...
	{
		if(variable[variable.length()-1] != '/')//...and the last character isn't a '/', just print value
		{
			if((variable[1] == '/' || variable[1] == VAR_SIGIL) && scope.keyExists<Var>(key))
			{
				if(property == "")
					fprintf(stdout,"%s\n",scope.getStr<Var>(key).c_str());
				else if(!stringcasecmp(property,"Value"))
					fprintf(stdout,"%s\n",scope.getProperty<Var>(key,property).c_str());
			}
			else if((variable[1] == '/' || variable[1] == DICE_SIGIL) && scope.keyExists<Dice>(key))
			{
				if(property == "")
					fprintf(stdout,"%s\n",scope.getStr<Dice>(key).c_str());
				else if((!stringcasecmp(property,"Quantity") ||
				         !stringcasecmp(property,"Faces") ||
				         !stringcasecmp(property,"Modifier") ||
				         !stringcasecmp(property,"List")))
					fprintf(stdout,"%s\n",scope.getProperty<Dice>(key,property).c_str());
			}
			else if((variable[1] == '/' || variable[1] == WALLET_SIGIL) && scope.keyExists<Wallet>(key))
			{
				if(property == "")
					fprintf(stdout,"%s\n",scope.getStr<Wallet>(key).c_str());
				else if(scope.get<Wallet>(key).containsCurrency(property))
					fprintf(stdout,"%s\n",scope.getProperty<Wallet>(key,property).c_str());
			}
			else if((variable[1] == '/' || variable[1] == CURRENCY_SIGIL) && scope.keyExists<Currency>(key))
			{
				if(property == "")
					fprintf(stdout,"%s\n",scope.getStr<Currency>(key).c_str());
				else if((!stringcasecmp(property,"CurrencySystem") ||
					 !stringcasecmp(property,"Name") ||
					 !stringcasecmp(property,"SmallerAmount") ||
					 !stringcasecmp(property,"Smaller") ||
					 !stringcasecmp(property,"Larger")))
					fprintf(stdout,"%s\n",scope.getProperty<Currency>(key,property).c_str());
			}
			else if((variable[1] == '/' || variable[1] == CURRENCYSYSTEM_SIGIL) && scope.keyExists<CurrencySystem>(key))
			{
				if(property == "")
					fprintf(stdout,"%s\n",scope.getStr<CurrencySystem>(key).c_str());
				else if(!stringcasecmp(property,"Name"))
					fprintf(stdout,"%s\n",scope.getProperty<CurrencySystem>(key,property).c_str());
			}
			else if(variable[1] != '/' &&
				variable[1] != VAR_SIGIL &&
				variable[1] != DICE_SIGIL &&
				variable[1] != WALLET_SIGIL &&
				variable[1] != CURRENCY_SIGIL &&
				variable[1] != CURRENCYSYSTEM_SIGIL)
			{
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
					appendMap<Currency>(scope,&c_map);
					break;
				case CURRENCYSYSTEM_SIGIL:
					appendMap<CurrencySystem>(scope,&cs_map);
					break;
				case DICE_SIGIL:
					appendMap<Dice>(scope,&d_map);
					break;
				case VAR_SIGIL:
					appendMap<Var>(scope,&v_map);
					break;
				case WALLET_SIGIL:
					appendMap<Wallet>(scope,&w_map);
					break;
				case '/':
					appendMap<Currency>(scope,&c_map);
					appendMap<CurrencySystem>(scope,&cs_map);
					appendMap<Dice>(scope,&d_map);
					appendMap<Var>(scope,&v_map);
					appendMap<Wallet>(scope,&w_map);
					break;
				default:
					output(Error,"Unknown type specifier \'%c\' in \"%s\"",variable[1],variable.c_str());
					exit(-1);
			}

			//Create output string from map
			std::string output = "";
			appendOutput(c_map,key,&output);
			appendOutput(cs_map,key,&output);
			appendOutput(d_map,key,&output);
			appendOutput(v_map,key,&output);
			appendOutput(w_map,key,&output);

			//Cut off the extraneous DS
			output = left(output,output.length()-DS.length());
			fprintf(stdout,"%s\n",output.c_str());
		}
	}
	else if(argc == 3)//Unary operators
	{
		std::string op(argv[2]);
		std::string type_str = "";
		std::string old_value = "";
		std::string new_value = "";

		if(findInStrVect(unaryOps,op,0) > -1)
		{
			//TODO: May have to ensure that properties get the right evaluated data type
			std::string old_property = "";

			if(scope.keyExists<Var>(key) && (variable[1] == '/' || variable[1] == VAR_SIGIL))
			{
				Var old_var = scope.get<Var>(key);
				old_value = std::string(old_var);
				type_str = "Var";

				tryGetProperty<Var>(scope, type_str, key, property, &old_property);

				if(property == "")
					scope.set<Var>(key,doArithOps<Var,int>(old_var,op,0));
				else if(!stringcasecmp(property,"Value"))
					scope.setProperty<Var,std::string>(key,property,doArithOps<Var,int>(old_property,op,0).Value);

				new_value = scope.getStr<Var>(key);
			}
			else if(scope.keyExists<Dice>(key) && (variable[1] == '/' || variable[1] == DICE_SIGIL))
			{
				Dice old_dice = scope.get<Dice>(key);
				old_value = std::string(old_dice);
				type_str = "Dice";

				tryGetProperty<Dice>(scope, type_str, key, property, &old_property);

				if(property == "")
					scope.set<Dice>(key,doArithOps<Dice,int>(old_dice,op,0));
				else if((!stringcasecmp(property,"Quantity") ||
					 !stringcasecmp(property,"Faces") ||
					 !stringcasecmp(property,"Modifier")))
					scope.setProperty<Dice,int>(key,property,std::stoi(doArithOps<Var,int>(old_property,op,0).Value));
				else if(!stringcasecmp(property,"List"))
					scope.setProperty<Dice,std::string>(key,property,doArithOps<Var,int>(old_property,op,0).Value);

				new_value = scope.getStr<Dice>(key);
			}
			else if(scope.keyExists<Wallet>(key) && (variable[1] == '/' || variable[1] == WALLET_SIGIL))
			{
				Wallet old_wallet = scope.get<Wallet>(key);
				old_value = std::string(old_wallet);
				type_str = "Wallet";

				tryGetProperty<Wallet>(scope, type_str, key, property, &old_property);

				if(property == "")
					scope.set<Wallet>(key,doArithOps<Wallet,int>(old_wallet,op,0));
				else if(scope.get<Wallet>(key).containsCurrency(property))
					scope.setProperty<Wallet,int>(key,property,std::stoi(doArithOps<Var,int>(old_property,op,0).Value));

				new_value = scope.getStr<Wallet>(key);
			}
			else if(scope.keyExists<Currency>(key) && (variable[1] == '/' || variable[1] == CURRENCY_SIGIL))
			{
				Currency old_currency = scope.get<Currency>(key);
				old_value = std::string(old_currency);
				type_str = "Currency";

				tryGetProperty<Currency>(scope, type_str, key, property, &old_property);

				if(property == "")
					scope.set<Currency>(key,doArithOps<Currency,int>(old_currency,op,0));
				else if(!stringcasecmp(property,"CurrencySystem"))
					scope.setProperty<Currency,std::shared_ptr<CurrencySystem>>(key,property,std::make_shared<CurrencySystem>(CurrencySystem(doArithOps<Var,int>(old_property,op,0).Value)));
				else if(!stringcasecmp(property,"SmallerAmount"))
					scope.setProperty<Currency,int>(key,property,std::stoi(doArithOps<Var,int>(old_property,op,0).Value));
				else if(!stringcasecmp(property,"Smaller") ||
					!stringcasecmp(property,"Larger"))
					scope.setProperty<Currency,std::string>(key,property,doArithOps<Var,int>(old_property,op,0).Value);

				new_value = scope.getStr<Currency>(key);
			}
			else if(scope.keyExists<CurrencySystem>(key) && (variable[1] == '/' || variable[1] == CURRENCYSYSTEM_SIGIL))
			{
				CurrencySystem old_currencysystem = scope.get<CurrencySystem>(key);
				old_value = std::string(old_currencysystem);
				type_str = "CurrencySystem";

				tryGetProperty<CurrencySystem>(scope, type_str, key, property, &old_property);

				if(property == "")
					scope.set<CurrencySystem>(key,doArithOps<CurrencySystem,int>(old_currencysystem,op,0));
				else if(!stringcasecmp(property,"Name"))
					scope.setProperty<CurrencySystem,std::string>(key,property,doArithOps<Var,int>(old_property,op,0).Value);

				new_value = scope.getStr<CurrencySystem>(key);
			}
			else if(variable[1] != '/')
			{
				output(Error,"Unknown type specifier \'%c\' in \"%s\"",variable[1],variable.c_str());
				exit(-1);
			}

			if(new_value != old_value)
				output(Info,"%s %s \"%s\" has changed from \"%s\" to \"%s\"",scope_str.c_str(),
											     type_str.c_str(),
											     key.c_str(),
											     old_value.c_str(),
											     new_value.c_str());
		}
		else
		{
			output(Error,"Invalid unary operator \"%s\"",op.c_str());
		}

		scope.save();
	}
	else//Binary operators
	{
		std::vector<std::string> args;
		unsigned int open_paren_ctr = 0;
		unsigned int close_paren_ctr = 0;
		for(int i=1; i<argc; i++)
		{
			std::string arg = std::string(argv[i]);
			long unsigned int open_paren = arg.find('(');
			long unsigned int close_paren = arg.rfind(')');

			for(const auto& c : arg)
			{
				if(c == '(') open_paren_ctr++;
				else if (c == ')') close_paren_ctr++;
			}

			if(open_paren != std::string::npos && close_paren != std::string::npos)
				args.push_back(arg.substr(open_paren,close_paren+1-open_paren));
			else if(open_paren != std::string::npos)
				args.push_back(arg.substr(open_paren,arg.length()-open_paren));
			else if(close_paren != std::string::npos)
				args.push_back(arg.substr(0,close_paren+1));
			else
				args.push_back(arg);
		}

		int args_size = args.size();

		//Wrap everything in parenthesis just to make below code simpler
		args[0] = '('+args[0];
		args[args_size-1] += ')';

		for(const auto& arg : args)
			fprintf(stdout,"Arg: %s\n",arg.c_str());

		if(open_paren_ctr > close_paren_ctr)
		{
			output(Error,"Missing close parenthesis to match open parenthesis.");
			exit(-1);
		}
		else if(open_paren_ctr < close_paren_ctr)
		{
			output(Error,"Missing open parenthesis to match close parenthesis.");
			exit(-1);
		}

		//PEMDAS
		for(int start=0; start<args_size-1; start++)
		{
			fprintf(stdout,"Checking args[%d]: %s\n",start,args[start].c_str());
			if(args[start].find('(') != std::string::npos)
			{
				for(int end=start; end<args_size; end++)
				{
					fprintf(stdout,"\tChecking args[%d]: %s\n",end,args[end].c_str());
					if(args[end][args[end].length()-1] == ')')
					{
						//Strip parenthesis off args to ensure good parsing
						fprintf(stdout,"\targs[start] = %s\n",args[start].c_str());
						fprintf(stdout,"\targs[end] = %s\n",args[end].c_str());
						std::string rparens = right(args[end],args[end].find(')')+1);
						args[start] = right(args[start],args[start].rfind('(')+1);
						args[end] = left(args[end],args[end].find(')'));
						fprintf(stdout,"\targs[start] = %s\n",args[start].c_str());
						fprintf(stdout,"\targs[end] = %s\n",args[end].c_str());

						//Vector is in order of operator precedence
						for(const auto& arithOp : arithOps)
						{
							int op_pos = findInStrVect(args,arithOp,start);
							fprintf(stdout,"\top_pos (%s) = %d\n",arithOp.c_str(),op_pos);

							if(op_pos > start && op_pos < end)
							{
								parseOperandsAndDoOp(&args,op_pos-1,op_pos,op_pos+1);
								args[start]+=rparens;//Maintain end parens
							}
						}
						fprintf(stdout,"\n");

						start = 1;
						break;
					}
					else if(args[end].find('(') != std::string::npos && end>start)//Nested '('
					{
						start = end-1;
						break;
					}
				}
			}

			//Check if parenthesis still exist. If so, restart loop to go back through PEMDAS
			if(start == args_size-1)
			{
				for(const auto& arg : args)
				{
					if(arg[0] == '(')
					{
						start = 1;
						break;
					}
				}
			}
		}
	}
	return 0;
}
