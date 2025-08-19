#include <climits>
#include <string.h>
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"
#include "../headers/var.h"

#define OP_NOT_FOUND INT_MAX // INT_MAX as a maximum for op_pos should be fine given that would be a massively complex equation.

enum Action
{
	read,
	write,
	set_add,
	set_remove,
	set_add_a,
	set_remove_a
};

// Operator precedence, later string vectors = lower precedence
// https://en.cppreference.com/w/cpp/language/operator_precedence
std::vector<std::vector<std::string>> operations = {	{OP_INC,OP_DEC},
							{OP_MUL,OP_DIV,OP_EXP,OP_MOD},
							{OP_ADD,OP_SUB},
							{OP_LT,OP_LE,OP_GT,OP_GE},
							{OP_EQ,OP_NE},
							{OP_AND},
							{OP_OR},
							{OP_ASSIGN,OP_ADDA,OP_SUBA,OP_MULA,OP_DIVA,OP_EXPA,OP_MODA}};

// Vectors for readability
std::vector<std::string> modOps;
std::vector<std::string> unaryOps;
std::vector<std::string> boolOps;
std::vector<std::string> assignOps;

std::string doAction(VariableInfo* p_vi, Action action, std::string value)
{
	std::string v;
	if(isTypeSigil(value[0]) && value[1] == '{') v = value;
	else v = Var(value).Value;

	if(p_vi->variable.back() != '/')// If the last character isn't a '/', just handle v
	{
		if(p_vi->type != '/' && !isTypeSigil(p_vi->type))
		{
			output(error,"Unknown type specifier \'%c\' in \"%s\"",p_vi->type,p_vi->variable.c_str());
			exit(-1);
		}

		if(p_vi->evalType == VAR_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Var>(p_vi->key);
			if(action == read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Var>(p_vi->key);
			else if(action == write && p_vi->property == "")
				p_vi->scope.set<Var>(p_vi->key,Var(v));
			else if(action == read && keyExists && !stringcasecmp(p_vi->property,"Value"))
				return p_vi->scope.getProperty<Var>(p_vi->key,p_vi->property);
			else if(action == write && !stringcasecmp(p_vi->property,"Value"))
				p_vi->scope.setProperty<Var,std::string>(p_vi->key,p_vi->property,v);
		}
		else if(p_vi->evalType == DICE_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Dice>(p_vi->key);
			if(action == read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Dice>(p_vi->key);
			else if(action == write && p_vi->property == "")
				p_vi->scope.set<Dice>(p_vi->key,Dice(v));
			else if(action == read && keyExists &&
				(!stringcasecmp(p_vi->property,"Quantity") ||
			         !stringcasecmp(p_vi->property,"Faces") ||
			         !stringcasecmp(p_vi->property,"Modifier") ||
				 !stringcasecmp(p_vi->property,"List")))
				return p_vi->scope.getProperty<Dice>(p_vi->key,p_vi->property);
			else if(action == write && !stringcasecmp(p_vi->property,"Quantity"))
				p_vi->scope.setProperty<Dice,int>(p_vi->key,p_vi->property,std::stoi(v));
			else if(action == write && !stringcasecmp(p_vi->property,"Faces"))
				p_vi->scope.setProperty<Dice,int>(p_vi->key,p_vi->property,std::stoi(v));
			else if(action == write && !stringcasecmp(p_vi->property,"Modifier"))
				p_vi->scope.setProperty<Dice,int>(p_vi->key,p_vi->property,std::stoi(v));
			else if(action == write && !stringcasecmp(p_vi->property,"List"))
				p_vi->scope.setProperty<Dice,std::string>(p_vi->key,p_vi->property,v);
		}
		else if(p_vi->evalType == WALLET_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Wallet>(p_vi->key);
			if(action == read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Wallet>(p_vi->key);
			else if(action == write && p_vi->property == "")
				p_vi->scope.set<Wallet>(p_vi->key,Wallet(v));
			else if(action == read && keyExists && p_vi->scope.get<Wallet>(p_vi->key).containsCurrency(p_vi->property))
				return p_vi->scope.getProperty<Wallet>(p_vi->key,p_vi->property);
			else if(action == write)
				p_vi->scope.setProperty<Wallet,int>(p_vi->key,p_vi->property,std::stoi(v));
		}
		else if(p_vi->evalType == CURRENCY_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Currency>(p_vi->key);
			if(action == read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Currency>(p_vi->key);
			else if(action == write && p_vi->property == "")
				p_vi->scope.set<Currency>(p_vi->key,Currency(v));
			else if(action == read && keyExists &&
				(!stringcasecmp(p_vi->property,"CurrencySystem") ||
				 !stringcasecmp(p_vi->property,"Name") ||
				 !stringcasecmp(p_vi->property,"SmallerAmount") ||
				 !stringcasecmp(p_vi->property,"Smaller") ||
				 !stringcasecmp(p_vi->property,"Larger")))
				return p_vi->scope.getProperty<Currency>(p_vi->key,p_vi->property);
			else if(action == write && !stringcasecmp(p_vi->property,"CurrencySystem"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,v);
			else if(action == write && !stringcasecmp(p_vi->property,"Name"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,v);
			else if(action == write && !stringcasecmp(p_vi->property,"Smaller"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,v);
			else if(action == write && !stringcasecmp(p_vi->property,"Larger"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,v);
			else if(action == write && !stringcasecmp(p_vi->property,"SmallerAmount"))
				p_vi->scope.setProperty<Currency,int>(p_vi->key,p_vi->property,std::stoi(v));
		}

		// Should only execute if action == write
		p_vi->scope.save();
		return "";
	}
	else if(action == read)// If the last character is a '/' and we are just printing the value, print a list of keys and constructors
	{
		return getSetStr(*p_vi);
	}
	else if(action == set_add || action == set_add_a ||
		action == set_remove || action == set_remove_a)// Perform write operation on variable set
	{
		struct RemovedKey
		{
			bool isRemoved = false;
			char type = 0;
		};

		//Make sure we don't remove keys that aren't part of the set being altered
		if(action == set_remove || action == set_remove_a)
		{
			for(const auto& [rhs_k,rhs_v] : getSet(value))
			{
				bool keyFound = false;
				for(const auto& [lhs_k,lhs_v] : getSet(getSetStr(*p_vi)))
				{
					if(lhs_k != rhs_k) continue;
					keyFound = true;
					break;
				}
				if(!keyFound)
				{
					output(error,"\"%s\" is not contained within the LHS set.",rhs_k.c_str());
					exit(-1);
				}
			}
		}

		for(const auto& [k,v] : getSet(value))
		{
			RemovedKey rk;

			switch(v[0])
			{
				case VAR_SIGIL:
					if((p_vi->type == '/' || p_vi->type == VAR_SIGIL) &&
					   (action == set_add || action == set_add_a))
						p_vi->scope.set<Var>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == VAR_SIGIL)
						rk = {p_vi->scope.remove<Var>(k),VAR_SIGIL};
					break;
				case DICE_SIGIL:
					if((p_vi->type == '/' || p_vi->type == DICE_SIGIL) &&
					   (action == set_add || action == set_add_a))
						p_vi->scope.set<Dice>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == DICE_SIGIL)
						rk = {p_vi->scope.remove<Dice>(k),DICE_SIGIL};
					break;
				case WALLET_SIGIL:
					if((p_vi->type == '/' || p_vi->type == WALLET_SIGIL) &&
					   (action == set_add || action == set_add_a))
						p_vi->scope.set<Wallet>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == WALLET_SIGIL)
						rk = {p_vi->scope.remove<Wallet>(k),WALLET_SIGIL};
					break;
				case CURRENCY_SIGIL:
					if((p_vi->type == '/' || p_vi->type == CURRENCY_SIGIL) &&
					   (action == set_add || action == set_add_a))
						p_vi->scope.set<Currency>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == CURRENCY_SIGIL)
						rk = {p_vi->scope.remove<Currency>(k),CURRENCY_SIGIL};
					break;
			}

			std::string xref = (p_vi->xref != "" ? "["+p_vi->xref+"]" : "");
			if(action == set_add_a)
				output(info,"Set \"%c%s%c/%s\" to \"%s\"",p_vi->scope.sigil,xref.c_str(),p_vi->evalType,(p_vi->key+k).c_str(),v.c_str());
			else if(action == set_remove_a && rk.isRemoved)
				output(info,"Variable \"%c%s%c/%s\" (value: \"%s\") has been deleted.",p_vi->scope.sigil,xref.c_str(),rk.type,k.c_str(),v.c_str());
		}

		if (action == set_add || action == set_remove) return getSetStr(*p_vi);
		else if(action == set_add_a || action == set_remove_a) p_vi->scope.save();
	}

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
	for(long unsigned precedence=0; precedence<operations.size(); precedence++)
	{
		int op_match = findInVect<std::string>(operations[precedence],op);

		try{(void)TL(lhs);}
		catch(const std::runtime_error& e)
		{
			if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
				output(error,"Invalid LHS explicit constructor: \"%s\"",lhs.c_str());
			else
				output(error,"Error constructing LHS: %s",e.what());
		}
		if(rhs != "")
		{
			try{(void)TR(rhs);}
			catch(const std::runtime_error& e)
			{
				if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
					output(error,"Invalid RHS explicit constructor: \"%s\"",rhs.c_str());
				else
					output(error,"Error constructing RHS: %s",e.what());
			}
		}
		try
		{
			if(op_match > -1 && (precedence < 3 || precedence == 7))// Mod operators
				return escapeSpaces(std::string(doModOp<TL,TR>(TL(lhs),op,TR(rhs))));
			else if(op_match > -1 && (precedence >= 3 && precedence < 7))// Boolean operators
				return btos(doBoolOp<TL,TR>(TL(lhs),op,TR(rhs)));
		}
		catch(const std::runtime_error& e)
		{
			if(e.what() == std::string(E_INVALID_OPERATION))
				output(error,"Invalid operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else if(e.what() == std::string(E_UNKNOWN_OPERATION))
				output(error,"Unknown operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else if(e.what() == std::string(E_INSUFFICIENT_FUNDS))
				output(error,"Insufficient funds: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else
				output(error,"Error during operation: \"%s %s %s\" (%s)",lhs.c_str(),op.c_str(),rhs.c_str(),e.what());

			exit(-1);
		}
	}
	return "";// Supress -Wreturn-type
}
template<typename TL>
std::string parseRHSAndDoOp(std::vector<std::string> v, unsigned int lhs_pos, unsigned int op_pos, unsigned int rhs_pos)
{
	if(rhs_pos == UINT_MAX)// Unary operators
		return getResult<TL,Var>(v[lhs_pos],v[op_pos],"");

	// Exceptions to the "return type is always LHS"
	try
	{
		if(std::is_same_v<TL,Currency> && (left(v[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{" || v[rhs_pos][1] != '{') && Var(v[rhs_pos]).isInt() && v[op_pos] == OP_MUL)
			return escapeSpaces(std::string(Currency(v[lhs_pos]) * Var(v[rhs_pos])));
		else if(std::is_same_v<TL,Var> && left(v[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{" && Var(v[lhs_pos]).isInt() && v[op_pos] == OP_MUL)
			return escapeSpaces(std::string(Currency(v[rhs_pos]) * Var(v[lhs_pos])));
		else if(std::is_same_v<TL,Currency> && left(v[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{" && v[op_pos] == OP_ADD)
			return escapeSpaces(std::string(Currency(v[lhs_pos]) + Currency(v[rhs_pos])));
		else if(std::is_same_v<TL,Currency> && left(v[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{" && v[op_pos] == OP_ADD)
			return escapeSpaces(std::string(Currency(v[lhs_pos]) + Wallet(v[rhs_pos])));
	}
	catch(const std::runtime_error& e)
	{
		if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
			output(error,"Invalid RHS explicit constructor: %s\n",v[rhs_pos].c_str());
		else
			output(error,"Error during operation: \"%s %s %s\" (%s)",v[lhs_pos].c_str(),v[op_pos].c_str(),v[rhs_pos].c_str(),e.what());

		exit(-1);
	}

	if(left(v[rhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
		return getResult<TL,Var>(v[lhs_pos],v[op_pos],v[rhs_pos]);
	else if(left(v[rhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
		return getResult<TL,Dice>(v[lhs_pos],v[op_pos],v[rhs_pos]);
	else if(left(v[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
		return getResult<TL,Wallet>(v[lhs_pos],v[op_pos],v[rhs_pos]);
	else if(left(v[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
		return getResult<TL,Currency>(v[lhs_pos],v[op_pos],v[rhs_pos]);
	else if(std::is_same_v<TL,Dice> && !Var(v[rhs_pos]).isInt())
		return getResult<TL,Dice>(v[lhs_pos],v[op_pos],v[rhs_pos]);
	else
		return getResult<TL,Var>(v[lhs_pos],v[op_pos],v[rhs_pos]);

	return "";// Supress -Wreturn-type
}
void parseLHSAndDoOp(VariableInfo* vi, std::vector<std::string>* v, unsigned int lhs_pos, unsigned int op_pos, unsigned int rhs_pos)
{
	std::string result = "";

	if(rhs_pos >= v->size())
		rhs_pos = UINT_MAX;// Unary operators. MAX_BUFFER is way less than UINT_MAX, so this is okay.

	if(left((*v)[lhs_pos],2) == std::string(1,VAR_SIGIL)+"{")
		result = parseRHSAndDoOp<Var>(*v, lhs_pos, op_pos, rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,DICE_SIGIL)+"{")
		result = parseRHSAndDoOp<Dice>(*v, lhs_pos, op_pos, rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,WALLET_SIGIL)+"{")
		result = parseRHSAndDoOp<Wallet>(*v, lhs_pos, op_pos, rhs_pos);
	else if(left((*v)[lhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{")
		result = parseRHSAndDoOp<Currency>(*v, lhs_pos, op_pos, rhs_pos);
	else
		result = parseRHSAndDoOp<Var>(*v, lhs_pos, op_pos, rhs_pos);

	// If the first arg is a variable and we are assigning it, we'll need to save the result
	if(lhs_pos == 0 && vi->key != "" &&
	  (findInVect<std::string>(assignOps,(*v)[op_pos]) > -1 || findInVect<std::string>(unaryOps,(*v)[op_pos]) > -1))
		(void)doAction(vi, write, result);

	// Replace operators and operands with result
	if(rhs_pos < UINT_MAX)
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

	chkFlagAppDesc(argv,"Prints values and evaluates operations. Implicitly called when modifying variables.");
	chkFlagPreserveVariables(argv,first);

	if(argc == 1)
	{
		output(error,"eval expects at least one argument.");
		return -1;
	}

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\teval [%sOPTIONS%s | %sOPERATION%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		fprintf(stdout,"\nOPERATION:\n");
		fprintf(stdout,"\tOne operand and at least one operator, seperated by spaces, which may be wrapped in parentheses.\n");
		fprintf(stdout,"\n\t\te.g. \"2 + 2\", \"@v/HitPoints/Current = (16 + 2 - 5)\", \"@/CoinPurse >= @[Store]/Sword/Cost\", \"@/Age ++\"\n");
		fprintf(stdout,"\n\tCurrently supported operators are as follows:\n\n");
		fprintf(stdout,"\t\t Unary  Arithmetic       Assignment          Relational    Boolean\n");
		fprintf(stdout,"\t\t┌─────┐┌───────────┐┌───────────────────┐┌───────────────┐┌───────┐\n");
		fprintf(stdout,"\t\t│%s %s││%s %s %s %s %s %s││%s %s %s %s %s %s %s││%s %s %s %s %s %s││ %s %s │\n",OP_INC,OP_DEC,OP_ADD,OP_SUB,OP_MUL,OP_DIV,OP_EXP,OP_MOD,OP_ASSIGN,OP_ADDA,OP_SUBA,OP_MULA,OP_DIVA,OP_EXPA,OP_MODA,OP_EQ,OP_NE,OP_LT,OP_GT,OP_LE,OP_GE,OP_AND,OP_OR);
		fprintf(stdout,"\t\t└─────┘└───────────┘└───────────────────┘└───────────────┘└───────┘\n");
		fprintf(stdout,"\n\tFor in-depth information on operations, please consult docs/rpgsh-userguide.pdf.\n");

		return 0;
	}

	if(!looksLikeVariable(std::string(argv[1])) && argv[2] &&
	   findInVect<std::string>(assignOps,std::string(argv[2])) != -1)
	{
		output(error,"Values cannot be assigned to constants.");
		return -1;
	}

	VariableInfo vi;

	if(looksLikeVariable(std::string(argv[1])))
		vi = parseVariable(std::string(argv[1]));

	if(argc == 2)// Print data if all the user enters is a variable
	{
		if(vi.type == '\0')//argv[1] is a constant
		{
			try
			{
				Dice d = Dice(argv[1]);
				if(d.List == "")
					fprintf(stdout,"%s\n",d.c_str());
				else
					fprintf(stdout,"%s\n",Var(argv[1]).c_str());

				return 0;
			}
			catch(std::runtime_error& e)
			{
				if(!strcmp(e.what(),E_INVALID_EXPLICIT_CONSTRUCTOR))
					fprintf(stdout,"%s\n",Var(argv[1]).c_str());
				else
					output(error,"An error occured while attempting to see if %s was a valid Dice constructor: %s",argv[1],e.what());

				return 0;
			}
		}

		std::string str = doAction(&vi, read, "");
		if(str != "") fprintf(stdout,"%s\n",str.c_str());
	}
	else if(vi.variable == "" || vi.variable.back() != '/')// Perform operation on variable
	{
		struct LParenInfo
		{
			long unsigned pos = 0;
			int depth = 0;
		};

		std::vector<LParenInfo> lpi;
		std::string old_value;
		if(vi.variable != "") old_value = getAppOutput(vi.variable).output[0];

		std::vector<std::string> args;

		// Need to know final operation to determine whether we print to screen or not
		std::string final_op = std::string(argv[2]);
		if(final_op[0] == '(')
			final_op = right(final_op,1);
		if(final_op.back() == ')' && !isEscaped(final_op,final_op.length()-1))
			final_op = left(final_op,final_op.length()-1);

		// Generate vector of operators and operands, while also determining number of parentheses
		std::string arg_str;
		unsigned int lparen_ctr = 0;
		unsigned int rparen_ctr = 0;
		for(int i=1; i<argc; i++)
		{
			std::string arg = std::string(argv[i]);
			lparen_ctr += countu(arg,'(');
			rparen_ctr += countu(arg,')');

			args.push_back(arg);
		}

		// Replace first arg with value if it's a variable
		if(vi.key != "" && vi.property != "")
		{
			switch(vi.evalType)
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
			}
		}
		else if(vi.key != "")
		{
			switch(vi.evalType)
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
			}
		}
		else
		{
			std::string lparens, rparens;
			if(findu(args[0],'(') != std::string::npos)
			{
				lparens = left(args[0],rfindu(args[0],'(')+1);
				args[0] = right(args[0],rfindu(args[0],'(')+1);
			}
			if(findu(args[0],')') != std::string::npos)
			{
				rparens = right(args[0],findu(args[0],')')+1);
				args[0] = left(args[0],findu(args[0],')')+1);
			}

			args[0] = lparens+std::string(Var(args[0]))+rparens;
		}

		if(lparen_ctr > rparen_ctr)
		{
			output(error,"Missing close parenthesis to match open parenthesis.");
			return -1;
		}
		else if(lparen_ctr < rparen_ctr)
		{
			output(error,"Missing open parenthesis to match close parenthesis.");
			return -1;
		}

		// Wrap everything in parenthesis just to make below code simpler
		bool added_lparens = false;
		bool added_rparens = false;
		args[0] = '('+args[0];
		args.back() += ')';

		// PEMDAS
		long unsigned pos = 0;
		for(int start=0; start<(int)args.size()-1; start++)
		{
			arg_str = "";
			for(const auto& arg : args)
				arg_str += arg+" ";
			arg_str = left(arg_str,arg_str.length()-1);

			int depth = 0;
			lpi.clear();
			for(long unsigned i=0; i<arg_str.length(); i++)
			{
				if(arg_str[i] == '(')
				{
					lpi.push_back({i,depth});
					depth++;
				}
				else if(arg_str[i] == ')')
				{
					depth--;
				}
			}

			LParenInfo current_lpi = {0,0};
			for(const auto& i : lpi)
				if(i.depth > current_lpi.depth) current_lpi = i;

			if((long unsigned)start < args.size() && pos <= current_lpi.pos && (pos+args[start].length()) > current_lpi.pos)
			{
				for(int end=start; end < (int)args.size(); end++)
				{
					if(args[end].back() == ')' && !isEscaped(args[end],args[end].length()-1))
					{
						// Strip parenthesis off args to ensure good parsing
						std::string lparens = left(args[start],rfindu(args[start],'('));
						std::string rparens = right(args[end],findu(args[end],')')+1);
						args[start] = right(args[start],rfindu(args[start],'(')+1);
						args[end] = left(args[end],findu(args[end],')'));

						std::vector<std::string> current_ops;
						for(int i=start; i<=end; i++)
							current_ops.push_back(args[i]);

						// Vectors for each operation type are in order of operator precedence
						int op_pos = OP_NOT_FOUND;
						for(const auto& precedence : operations)
						{
							for(const auto& op : precedence)
							{
								int next_op = findInVect<std::string>(current_ops,op);
								if(next_op > -1 && next_op < op_pos)
									op_pos = next_op+start;
							}
							if(op_pos != OP_NOT_FOUND) break;
						}
						if(op_pos != OP_NOT_FOUND)
						{
							parseLHSAndDoOp(&vi,&args,op_pos-1,op_pos,op_pos+1);
							if(lparens[0] == '(')
							{
								args[start] = lparens+args[start];
								added_lparens = true;
							}
							args[start] += rparens;
							added_rparens = true;
						}
						else
						{
							output(error,"Unknown operator \"%s\"",args[op_pos].c_str());
							return -1;
						}

						pos = 0;
						start = -1;
						break;
					}
				}
			}
			if(start > -1)
				pos += args[start].length()+1;

			// Check if parenthesis still exist, and re-wrap if needed. Restart loop to go back through PEMDAS if needed
			if(args.size() > 1 && args[0][0] != '(')
				args[0] = '(' + args[0];
			if(args.size() > 1 && args.back().back() != ')')
				args.back() += ')';
			if(start == (int)args.size()-1)
			{
				for(const auto& arg : args)
				{
					if(arg[0] == '(')
					{
						pos = 0;
						start = -1;
						break;
					}
				}
			}
		}

		if(added_lparens) args[0] = right(args[0],rfindu(args[0],'(')+1);
		if(added_rparens) args[0] = left(args[0],findu(args[0],')'));

		// Print result
		if(findInVect<std::string>(assignOps,final_op) == -1 && findInVect<std::string>(unaryOps,final_op) == -1)
		{
			fprintf(stdout,"%s\n",args[0].c_str());
			return 0;
		}

		std::string new_value = getAppOutput(std::string(argv[1])).output[0];
		std::string xref = (vi.xref != "" ? "["+vi.xref+"]" : "");
		std::string p = vi.property;
		if(old_value == "")
			output(info,"%c%s%c/%s has been initialized to %s",
			       vi.scope.sigil,xref.c_str(),vi.evalType,vi.key.c_str(),new_value.c_str());
		else if(old_value != new_value)
			output(info,"%c%s%c/%s%s has changed from %s to %s",
			       vi.scope.sigil,xref.c_str(),vi.evalType,vi.key.c_str(),(p == "" ? "" : "."+p).c_str(),old_value.c_str(),new_value.c_str());
		else
			output(info,"%c%s%c/%s%s is unchanged, current value: %s",
			       vi.scope.sigil,xref.c_str(),vi.evalType,vi.key.c_str(),(p == "" ? "" : "."+p).c_str(),old_value.c_str());
	}
	else// Perform operation on set
	{
		if(argc != 4 ||
		   (strcasecmp(argv[2],OP_ADD) && strcasecmp(argv[2],OP_SUB) &&
		    strcasecmp(argv[2],OP_ADDA) && strcasecmp(argv[2],OP_SUBA)))
		{
			output(error,"Variable set modification only supports the following operators: %s, %s, %s, %s",
			       OP_ADD,OP_SUB,OP_ADDA,OP_SUBA);
			return -1;
		}

		std::string rhs = std::string(argv[3]);

		if(!looksLikeSet(rhs))
		{
			output(error,"Variable set modifications can only be performed with other variable sets.");
			return -1;
		}

		if(!strcasecmp(argv[2],OP_ADD))
			fprintf(stdout,"%s\n",doAction(&vi, set_add, rhs).c_str());
		else if(!strcasecmp(argv[2],OP_ADDA))
			(void)doAction(&vi, set_add_a, rhs);
		else if(!strcasecmp(argv[2],OP_SUB))
			fprintf(stdout,"%s\n",doAction(&vi, set_remove, rhs).c_str());
		else if(!strcasecmp(argv[2],OP_SUBA))
			(void)doAction(&vi, set_remove_a, rhs);
	}
	return 0;
}
