#include <climits>
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/var.h"

enum Action
{
	Read,
	Write
};

struct VariableInfo
{
	Scope scope = Scope();
	std::string variable = "";
	std::string key = "";
	std::string property = "";
};

Character c = Character(false);
Campaign m = Campaign();
Shell s = Shell();

// Operator precedence, later string vectors = lower precedence
// https://en.cppreference.com/w/cpp/language/operator_precedence
std::vector<std::vector<std::string>> operations = {	{OP_INC,OP_DEC},
							{OP_MUL,OP_DIV,OP_EXP,OP_MOD},
							{OP_ADD,OP_SUB},
							{OP_LT,OP_LE,OP_GT,OP_GE},
							{OP_EQ,OP_NE,OP_AEQ},
							{OP_AND},
							{OP_OR},
							{OP_ASSIGN,OP_ADDA,OP_SUBA,OP_MULA,OP_DIVA,OP_EXPA,OP_MODA}};

// Vectors for readability
std::vector<std::string> modOps;
std::vector<std::string> unaryOps;
std::vector<std::string> boolOps;
std::vector<std::string> assignOps;

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
void loadXRef(std::string* arg, Scope* p_scope)
{
	// Ending square bracket not found
	if(arg->find(']') == std::string::npos)
	{
		output(Error,"No terminating \']\' found for xref.");
		exit(-1);
	}

	// Get string between the square brackets
	std::string xref = arg->substr(arg->find('[')+1,arg->find(']')-(arg->find('[')+1));

	// Actually load the xref into the scope
	std::vector<std::string> campaigns;
	std::string xref_dir = campaigns_dir+
			       get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
			       "characters/";
	std::string xref_char = xref;
	std::string campaign = "";
	switch((*arg)[0])
	{
		case CHARACTER_SIGIL:
			if(xref.find('/') != std::string::npos)// Attempting to get a character from another campaign
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

	// Remove external reference string so we can continue to use the current arg under the new context
	(*arg) = (*arg)[0]+left((*arg),arg->find('[')-1)+right((*arg),arg->find(']')+1);
}
VariableInfo parseVariable(std::string v)// Derive information about variable from string
{
	VariableInfo vi;

	// Check scope sigil
	switch(v[0])
	{
		case CHARACTER_SIGIL:
			vi.scope = c;
			break;
		case CAMPAIGN_SIGIL:
			vi.scope = m;
			break;
		case SHELL_SIGIL:
			vi.scope = s;
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",v[0]);
			exit(-1);
	}

	// Check for external references
	if(v[1] == '[') loadXRef(&v,&vi.scope);

	vi.variable = v;
	vi.key = right(v,v.find('/')+1);
	if((int)v.find('.') > (int)v.rfind('/'))
	{
		vi.key = left(vi.key,vi.key.find('.'));
		vi.property = right(v,v.find('.')+1);
	}

	return vi;
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
		// Get root variable if it exists
		if(!stringcasecmp((left(k,key.length())+"/"),key))
			pOutput->append(k + DS + v + DS);

		// Get all subkeys
		if(!stringcasecmp((left(k,key.length())),key))
			pOutput->append(k + DS + v + DS);
	}
}
std::string readOrWriteDataOnScope(VariableInfo* p_vi, Action action, std::string value)
{
	if(p_vi->variable[p_vi->variable.length()-1] != '/')// If the last character isn't a '/', just handle value
	{
		if(p_vi->variable[1] == '/' || p_vi->variable[1] == VAR_SIGIL)
		{
			if(action == Read && !p_vi->scope.keyExists<Var>(p_vi->key))
				return "";
			else if(action == Read && p_vi->property == "")
				return p_vi->scope.getStr<Var>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Var>(p_vi->key,Var(value));
			else if(action == Read && !stringcasecmp(p_vi->property,"Value"))
				return p_vi->scope.getProperty<Var>(p_vi->key,p_vi->property);
			else if(action == Write && !stringcasecmp(p_vi->property,"Value"))
				p_vi->scope.setProperty<Var,std::string>(p_vi->key,p_vi->property,Var(value).Value);
		}
		else if(p_vi->variable[1] == '/' || p_vi->variable[1] == DICE_SIGIL)
		{
			if(action == Read && !p_vi->scope.keyExists<Dice>(p_vi->key))
				return "";
			else if(action == Read && p_vi->property == "")
				return p_vi->scope.getStr<Dice>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Dice>(p_vi->key,Dice(value));
			else if(action == Read &&
				(!stringcasecmp(p_vi->property,"Quantity") ||
			         !stringcasecmp(p_vi->property,"Faces") ||
			         !stringcasecmp(p_vi->property,"Modifier") ||
				 !stringcasecmp(p_vi->property,"List")))
				return p_vi->scope.getProperty<Dice>(p_vi->key,p_vi->property);
			else if(action == Write && !stringcasecmp(p_vi->property,"Quantity"))
				p_vi->scope.setProperty<Dice,int>(p_vi->key,p_vi->property,Dice(value).Quantity);
			else if(action == Write && !stringcasecmp(p_vi->property,"Faces"))
				p_vi->scope.setProperty<Dice,int>(p_vi->key,p_vi->property,Dice(value).Faces);
			else if(action == Write && !stringcasecmp(p_vi->property,"Modifier"))
				p_vi->scope.setProperty<Dice,int>(p_vi->key,p_vi->property,Dice(value).Modifier);
			else if(action == Write && !stringcasecmp(p_vi->property,"List"))
				p_vi->scope.setProperty<Dice,std::string>(p_vi->key,p_vi->property,Dice(value).List);
		}
		else if(p_vi->variable[1] == '/' || p_vi->variable[1] == WALLET_SIGIL)
		{
			if(action == Read && !p_vi->scope.keyExists<Wallet>(p_vi->key))
				return "";
			else if(action == Read && p_vi->property == "")
				return p_vi->scope.getStr<Wallet>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Wallet>(p_vi->key,Wallet(value));
			else if(action == Read && p_vi->scope.get<Wallet>(p_vi->key).containsCurrency(p_vi->property))
				return p_vi->scope.getProperty<Wallet>(p_vi->key,p_vi->property);
			else if(action == Write)
				p_vi->scope.setProperty<Wallet,int>(p_vi->key,p_vi->property,Wallet(value).Money[Currency(p_vi->property)]);
		}
		else if(p_vi->variable[1] == '/' || p_vi->variable[1] == CURRENCY_SIGIL)
		{
			if(action == Read && !p_vi->scope.keyExists<Currency>(p_vi->key))
				return "";
			else if(action == Read && p_vi->property == "")
				return p_vi->scope.getStr<Currency>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Currency>(p_vi->key,Currency(value));
			else if(action == Read &&
				(!stringcasecmp(p_vi->property,"CurrencySystem") ||
				 !stringcasecmp(p_vi->property,"Name") ||
				 !stringcasecmp(p_vi->property,"SmallerAmount") ||
				 !stringcasecmp(p_vi->property,"Smaller") ||
				 !stringcasecmp(p_vi->property,"Larger")))
				return p_vi->scope.getProperty<Currency>(p_vi->key,p_vi->property);
			else if(action == Write && !stringcasecmp(p_vi->property,"CurrencySystem"))
				p_vi->scope.setProperty<Currency,std::shared_ptr<CurrencySystem>>(p_vi->key,p_vi->property,Currency(value).System);
			else if(action == Write && !stringcasecmp(p_vi->property,"Name"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).Name);
			else if(action == Write && !stringcasecmp(p_vi->property,"Smaller"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).Smaller);
			else if(action == Write && !stringcasecmp(p_vi->property,"Larger"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).Larger);
			else if(action == Write && !stringcasecmp(p_vi->property,"SmallerAmount"))
				p_vi->scope.setProperty<Currency,int>(p_vi->key,p_vi->property,Currency(value).SmallerAmount);
		}
		else if(p_vi->variable[1] == '/' || p_vi->variable[1] == CURRENCYSYSTEM_SIGIL)
		{
			if(action == Read && !p_vi->scope.keyExists<CurrencySystem>(p_vi->key))
				return "";
			else if(action == Read && p_vi->property == "")
				return p_vi->scope.getStr<CurrencySystem>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<CurrencySystem>(p_vi->key,CurrencySystem(value));
			else if(action == Read && !stringcasecmp(p_vi->property,"Name"))
				return p_vi->scope.getProperty<CurrencySystem>(p_vi->key,p_vi->property);
			else if(action == Write && !stringcasecmp(p_vi->property,"Name"))
				p_vi->scope.setProperty<CurrencySystem,std::string>(p_vi->key,p_vi->property,CurrencySystem(value).Name);
		}
		else if(p_vi->variable[1] != '/' && !isTypeSigil(p_vi->variable[1]))
		{
			output(Error,"Unknown type specifier \'%c\' in \"%s\"",p_vi->variable[1],p_vi->variable.c_str());
			exit(-1);
		}
	}
	else// ...and the last character is a '/', print a list of p_vi->keys and constructors
	{
		std::map<std::string,std::string> c_map;
		std::map<std::string,std::string> cs_map;
		std::map<std::string,std::string> d_map;
		std::map<std::string,std::string> v_map;
		std::map<std::string,std::string> w_map;

		// When printing entire containers, treat type sigil as a filter
		switch(p_vi->variable[1])
		{
			case CURRENCY_SIGIL:
				appendMap<Currency>(p_vi->scope,&c_map);
				break;
			case CURRENCYSYSTEM_SIGIL:
				appendMap<CurrencySystem>(p_vi->scope,&cs_map);
				break;
			case DICE_SIGIL:
				appendMap<Dice>(p_vi->scope,&d_map);
				break;
			case VAR_SIGIL:
				appendMap<Var>(p_vi->scope,&v_map);
				break;
			case WALLET_SIGIL:
				appendMap<Wallet>(p_vi->scope,&w_map);
				break;
			case '/':
				appendMap<Currency>(p_vi->scope,&c_map);
				appendMap<CurrencySystem>(p_vi->scope,&cs_map);
				appendMap<Dice>(p_vi->scope,&d_map);
				appendMap<Var>(p_vi->scope,&v_map);
				appendMap<Wallet>(p_vi->scope,&w_map);
				break;
			default:
				output(Error,"Unknown type specifier \'%c\' in \"%s\"",p_vi->variable[1],p_vi->variable.c_str());
				exit(-1);
		}

		// Create output string from map
		std::string output = "";
		appendOutput(c_map,p_vi->key,&output);
		appendOutput(cs_map,p_vi->key,&output);
		appendOutput(d_map,p_vi->key,&output);
		appendOutput(v_map,p_vi->key,&output);
		appendOutput(w_map,p_vi->key,&output);

		// Cut off the extraneous DS
		output = left(output,output.length()-DS.length());
		return output;
	}

	if(action == Write) p_vi->scope.save();

	return ""; // Supress -Wreturn-type
}
template<typename TL, typename TR>
TL doModOp(TL lhs, std::string op, TR rhs)
{
	if(op == OP_INC)
	{
		try{lhs++;}
		catch(...){throw;}
	}
	else if(op == OP_DEC)
	{
		try{lhs--;}
		catch(...){throw;}
	}
	else if(op == OP_MUL)
	{
		try{lhs = lhs * rhs;}
		catch(...){throw;}
	}
	else if(op == OP_DIV)
	{
		try{lhs = lhs / rhs;}
		catch(...){throw;}
	}
	else if(op == OP_EXP)
	{
		try{lhs = lhs ^ rhs;}
		catch(...){throw;}
	}
	else if(op == OP_MOD)
	{
		try{lhs = lhs % rhs;}
		catch(...){throw;}
	}
	else if(op == OP_ADD)
	{
		try{lhs = lhs + rhs;}
		catch(...){throw;}
	}
	else if(op == OP_SUB)
	{
		try{lhs = lhs - rhs;}
		catch(...){throw;}
	}
	else if(op == OP_ASSIGN)
	{
		try{lhs = rhs;}
		catch(...){throw;}
	}
	else if(op == OP_MULA)
	{
		try{lhs *= rhs;}
		catch(...){throw;}
	}
	else if(op == OP_DIVA)
	{
		try{lhs /= rhs;}
		catch(...){throw;}
	}
	else if(op == OP_EXPA)
	{
		try{lhs ^= rhs;}
		catch(...){throw;}
	}
	else if(op == OP_MODA)
	{
		try{lhs %= rhs;}
		catch(...){throw;}
	}
	else if(op == OP_ADDA)
	{
		try{lhs += rhs;}
		catch(...){throw;}
	}
	else if(op == OP_SUBA)
	{
		try{lhs -= rhs;}
		catch(...){throw;}
	}
	else// We should only get here if I forgot to code an operation
	{
		throw std::runtime_error(E_UNKNOWN_OPERATION);
	}

	return lhs;
}
template<typename TL, typename TR>
bool doBoolOp(TL lhs, std::string op, TR rhs)
{
	if(op == OP_LT)
	{
		try{return lhs < rhs;}
		catch(...){throw;}
	}
	else if(op == OP_LE)
	{
		try{return lhs <= rhs;}
		catch(...){throw;}
	}
	else if(op == OP_GT)
	{
		try{return lhs > rhs;}
		catch(...){throw;}
	}
	else if(op == OP_GE)
	{
		try{return lhs >= rhs;}
		catch(...){throw;}
	}
	else if(op == OP_EQ)
	{
		try{return lhs == rhs;}
		catch(...){throw;}
	}
	else if(op == OP_NE)
	{
		try{return lhs != rhs;}
		catch(...){throw;}
	}
	else if(op == OP_AEQ)
	{
		try{return approxEquals<TL,TR>(lhs,rhs);}
		catch(...){throw;}
	}
	else if(op == OP_AND)
	{
		try{return lhs && rhs;}
		catch(...){throw;}
	}
	else if(op == OP_OR)
	{
		try{return lhs || rhs;}
		catch(...){throw;}
	}
	else// We should only get here if I forgot to code an operation
	{
		throw std::runtime_error(E_UNKNOWN_OPERATION);
	}
}
template<typename TL, typename TR>
std::string getResult(std::string lhs, std::string op, std::string rhs)
{
	for(long unsigned int precedence=0; precedence<operations.size(); precedence++)
	{
		int op_match = findInStrVect(operations[precedence],op,0);
		try
		{
			if(op_match > -1 && (precedence < 3 || precedence == 7))// Mod operators
				return std::string(doModOp<TL,TR>(TL(lhs),op,TR(rhs)));
			else if(op_match > -1 && (precedence >= 3 && precedence < 7))// Boolean operators
				return btos(doBoolOp<TL,TR>(TL(lhs),op,TR(rhs)));
		}
		catch(const std::runtime_error& e)
		{
			if(e.what() == std::string(E_INVALID_OPERATION))
				output(Error,"Invalid operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else if(e.what() == std::string(E_UNKNOWN_OPERATION))
				output(Error,"Unknown operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else
				output(Error,"Error during operation: \"%s %s %s\" (%s)",lhs.c_str(),op.c_str(),rhs.c_str(),e.what());

			exit(-1);
		}
	}

	return "";// Supress -Wreturn-type
}
template<typename TL>
std::string parseRHSAndDoOp(VariableInfo** vi, std::vector<std::string>** v, unsigned int* lhs_pos, unsigned int* op_pos, unsigned int* rhs_pos)
{
	if((*rhs_pos) == UINT_MAX)// Unary operators
	{
		if((*vi)->property == "")
			return getResult<TL,Var>((*vi)->scope.getStr<TL>((*vi)->key),(**v)[*op_pos],"");
		else
			return getResult<TL,Var>((*vi)->scope.getProperty<TL>((*vi)->key,(*vi)->property),(**v)[*op_pos],"");
	}

	if(left((**v)[*rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
		return getResult<TL,Var>((**v)[*lhs_pos],(**v)[*op_pos],(**v)[*rhs_pos]);
	else if(left((**v)[*rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
		return getResult<TL,Dice>((**v)[*lhs_pos],(**v)[*op_pos],(**v)[*rhs_pos]);
	else if(left((**v)[*rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
		return getResult<TL,Wallet>((**v)[*lhs_pos],(**v)[*op_pos],(**v)[*rhs_pos]);
	else if(left((**v)[*rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
		return getResult<TL,Currency>((**v)[*lhs_pos],(**v)[*op_pos],(**v)[*rhs_pos]);
	else if(left((**v)[*rhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
		return getResult<TL,CurrencySystem>((**v)[*lhs_pos],(**v)[*op_pos],(**v)[*rhs_pos]);
	else
		return getResult<TL,Var>((**v)[*lhs_pos],(**v)[*op_pos],(**v)[*rhs_pos]);

	return "";// Supress -Wreturn-type
}
void parseLHSAndDoOp(VariableInfo* vi, std::vector<std::string>* v, unsigned int lhs_pos, unsigned int op_pos, unsigned int rhs_pos)
{
	// TODO Handle quote-wrapped strings to force string interpretation

	std::string result = "";

	if(rhs_pos >= v->size())
		rhs_pos = UINT_MAX;// Unary operators. MAX_BUFFER is way less than UINT_MAX, so this is okay

	if(left((*v)[lhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
		result = parseRHSAndDoOp<Var>(&vi, &v, &lhs_pos, &op_pos, &rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
		result = parseRHSAndDoOp<Dice>(&vi, &v, &lhs_pos, &op_pos, &rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
		result = parseRHSAndDoOp<Wallet>(&vi, &v, &lhs_pos, &op_pos, &rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
		result = parseRHSAndDoOp<Currency>(&vi, &v, &lhs_pos, &op_pos, &rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,CURRENCYSYSTEM_SIGIL)+"{")
		result = parseRHSAndDoOp<CurrencySystem>(&vi, &v, &lhs_pos, &op_pos, &rhs_pos);
	else// Treat non-explicit constructors as Var
		result = parseRHSAndDoOp<Var>(&vi, &v, &lhs_pos, &op_pos, &rhs_pos);

	// If the first arg is a variable and we are assigning it, we'll need to save the result
	if(lhs_pos == 0 && vi->key != "" &&
	  (findInStrVect(assignOps,(*v)[op_pos],0) > -1 || findInStrVect(unaryOps,(*v)[op_pos],0) > -1))
		(void)readOrWriteDataOnScope(vi, Write, result);

	// Replace operators and operands with result
	v->erase(v->begin()+rhs_pos);
	v->erase(v->begin()+op_pos);
	v->erase(v->begin()+lhs_pos);
	v->insert(v->begin()+lhs_pos,result);
}

int main(int argc, char** argv)
{
	// Initialize operator group vectors
	modOps.insert(modOps.end(),operations[0].begin(),operations[0].end());
	modOps.insert(modOps.end(),operations[1].begin(),operations[1].end());
	modOps.insert(modOps.end(),operations[2].begin(),operations[2].end());
	modOps.insert(modOps.end(),operations[5].begin(),operations[5].end());
	unaryOps.insert(unaryOps.end(),operations[0].begin(),operations[0].end());
	boolOps.insert(boolOps.end(),operations[3].begin(),operations[3].end());
	boolOps.insert(boolOps.end(),operations[4].begin(),operations[4].end());
	boolOps.insert(boolOps.end(),operations[5].begin(),operations[5].end());
	boolOps.insert(boolOps.end(),operations[6].begin(),operations[6].end());
	assignOps.insert(assignOps.end(),operations[7].begin(),operations[7].end());

	check_print_app_description(argv,"Not meant for direct call by user. Implicitly called when modifying variables.");

	if(std::string(argv[1]).find('/') == std::string::npos)// If the user only enters the scope sigil
	{
		output(Error,"Expected at least one '/' delimiter.");
		exit(-1);
	}

	VariableInfo vi;
	std::string variable(argv[1]);
	std::string scope_str = "";

	if(isScopeSigil(variable[0]) && (isTypeSigil(variable[1]) || variable[1] == '/' || variable[1] == '['))
		vi = parseVariable(variable);

	if(argc == 2)// Print data if all the user enters is a variable
	{
		std::string str = readOrWriteDataOnScope(&vi, Read, "");
		if(str != "") fprintf(stdout,"%s\n",str.c_str());
	}
	else// Perform operation
	{
		std::vector<std::string> args;
		unsigned int open_paren_ctr = 0;
		unsigned int close_paren_ctr = 0;

		// Need to know final operation to determine whether we print to screen or not
		std::string final_op = std::string(argv[2]);
		if(final_op[0] == '(')
			final_op = right(final_op,1);
		if(final_op[final_op.length()-1] == ')')
			final_op = left(final_op,final_op.length()-1);

		// Generate vector of operators and operands, while also determining number of parentheses
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

		// Replace first arg with value if it's a variable
		if(vi.key != "" && vi.property != "")
		{
			switch(args[0][1])
			{
				case VAR_SIGIL:
					args[0] = vi.scope.getProperty<Var>(vi.key,vi.property);
					break;
				case DICE_SIGIL:
					args[0] = vi.scope.getProperty<Dice>(vi.key,vi.property);
					break;
				case WALLET_SIGIL:
					args[0] = vi.scope.getProperty<Wallet>(vi.key,vi.property);
					break;
				case CURRENCY_SIGIL:
					args[0] = vi.scope.getProperty<Currency>(vi.key,vi.property);
					break;
				case CURRENCYSYSTEM_SIGIL:
					args[0] = vi.scope.getProperty<CurrencySystem>(vi.key,vi.property);
					break;
				case '/':
					try{args[0] = vi.scope.getProperty<Var>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<Dice>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<Wallet>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<Currency>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<CurrencySystem>(vi.key,vi.property);break;}
					catch(...){}
					output(Error,"%s is not a valid property of %s.",vi.property.c_str(),vi.key.c_str());
					exit(-1);
			}
		}
		else if(vi.key != "")
		{
			switch(args[0][1])
			{
				case VAR_SIGIL:
					args[0] = vi.scope.getStr<Var>(vi.key);
					break;
				case DICE_SIGIL:
					args[0] = vi.scope.getStr<Dice>(vi.key);
					break;
				case WALLET_SIGIL:
					args[0] = vi.scope.getStr<Wallet>(vi.key);
					break;
				case CURRENCY_SIGIL:
					args[0] = vi.scope.getStr<Currency>(vi.key);
					break;
				case CURRENCYSYSTEM_SIGIL:
					args[0] = vi.scope.getStr<CurrencySystem>(vi.key);
					break;
				case '/':
					if(vi.scope.keyExists<Var>(vi.key))
					{args[0] = vi.scope.getStr<Var>(vi.key);break;}
					else if(vi.scope.keyExists<Dice>(vi.key))
					{args[0] = vi.scope.getStr<Dice>(vi.key);break;}
					else if(vi.scope.keyExists<Wallet>(vi.key))
					{args[0] = vi.scope.getStr<Wallet>(vi.key);break;}
					else if(vi.scope.keyExists<Currency>(vi.key))
					{args[0] = vi.scope.getStr<Currency>(vi.key);break;}
					else if(vi.scope.keyExists<CurrencySystem>(vi.key))
					{args[0] = vi.scope.getStr<CurrencySystem>(vi.key);break;}
					else{args[0] = "";}
			}
		}

		// Wrap everything in parenthesis just to make below code simpler
		args[0] = '('+args[0];
		args[args_size-1] += ')';

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

		// PEMDAS
		for(int start=0; start<args_size-1; start++)
		{
			if(args[start].find('(') != std::string::npos)
			{
				for(int end=start; end<args_size; end++)
				{
					if(args[end][args[end].length()-1] == ')')
					{
						// Strip parenthesis off args to ensure good parsing
						std::string rparens = right(args[end],args[end].find(')')+1);
						args[start] = right(args[start],args[start].rfind('(')+1);
						args[end] = left(args[end],args[end].find(')'));

						// Vectors for each operation type are in order of operator precedence
						int op_pos = INT_MAX;
						for(const auto& precedence : operations)
						{
							for(const auto& op : precedence)
							{
								int found_op = findInStrVect(args,op,start);
								if(found_op > -1 && found_op < op_pos)
									op_pos = found_op;
							}
							if(op_pos < INT_MAX) break;
						}
						if(op_pos > start && op_pos <= end)
						{
							parseLHSAndDoOp(&vi,&args,op_pos-1,op_pos,op_pos+1);
							args[start] += rparens;// Maintain end parens
						}

						start = -1;
						break;
					}
					else if(args[end].find('(') != std::string::npos && end>start)// Nested '('
					{
						start = end - 1;
						break;
					}
				}
			}

			// Check if parenthesis still exist, and re-wrap if needed. Restart loop to go back through PEMDAS if needed
			if(args.size() > 1 && args[0][0] != '(')
				args[0] = '(' + args[0];
			if(args.size() > 1 && args[args.size()-1][args[args.size()-1].length()-1] != ')')
				args[args.size()-1] += ')';
			if(start == args_size-1)
			{
				for(const auto& arg : args)
				{
					if(arg[0] == '(')
					{
						start = -1;
						break;
					}
				}
			}
		}

		// Print result if we aren't writing to a variable
		if(findInStrVect(assignOps,final_op,0) == -1 && findInStrVect(unaryOps,final_op,0) == -1)
			fprintf(stdout,"%s\n",args[0].c_str());
	}
	return 0;
}
