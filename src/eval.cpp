#include <climits>
#include <string.h>
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"
#include "../headers/var.h"

enum Action
{
	Read,
	Write,
	SetAdd,
	SetRemove,
	SetAddA,
	SetRemoveA
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
	if(p_vi->variable.back() != '/')// If the last character isn't a '/', just handle value
	{
		if(p_vi->type != '/' && !isTypeSigil(p_vi->type))
		{
			output(Error,"Unknown type specifier \'%c\' in \"%s\"",p_vi->type,p_vi->variable.c_str());
			exit(-1);
		}

		if(p_vi->evalType == VAR_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Var>(p_vi->key);
			if(action == Read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Var>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Var>(p_vi->key,Var(value));
			else if(action == Read && keyExists && !stringcasecmp(p_vi->property,"Value"))
				return p_vi->scope.getProperty<Var>(p_vi->key,p_vi->property);
			else if(action == Write && !stringcasecmp(p_vi->property,"Value"))
				p_vi->scope.setProperty<Var,std::string>(p_vi->key,p_vi->property,Var(value).Value);
		}
		else if(p_vi->evalType == DICE_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Dice>(p_vi->key);
			if(action == Read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Dice>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Dice>(p_vi->key,Dice(value));
			else if(action == Read && keyExists &&
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
		else if(p_vi->evalType == WALLET_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Wallet>(p_vi->key);
			if(action == Read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Wallet>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Wallet>(p_vi->key,Wallet(value));
			else if(action == Read && keyExists && p_vi->scope.get<Wallet>(p_vi->key).containsCurrency(p_vi->property))
				return p_vi->scope.getProperty<Wallet>(p_vi->key,p_vi->property);
			else if(action == Write)
				p_vi->scope.setProperty<Wallet,int>(p_vi->key,p_vi->property,Wallet(value).Money[Currency(p_vi->property)]);
		}
		else if(p_vi->evalType == CURRENCY_SIGIL)
		{
			bool keyExists = p_vi->scope.keyExists<Currency>(p_vi->key);
			if(action == Read && keyExists && p_vi->property == "")
				return p_vi->scope.getStr<Currency>(p_vi->key);
			else if(action == Write && p_vi->property == "")
				p_vi->scope.set<Currency>(p_vi->key,Currency(value));
			else if(action == Read && keyExists &&
				(!stringcasecmp(p_vi->property,"CurrencySystem") ||
				 !stringcasecmp(p_vi->property,"Name") ||
				 !stringcasecmp(p_vi->property,"SmallerAmount") ||
				 !stringcasecmp(p_vi->property,"Smaller") ||
				 !stringcasecmp(p_vi->property,"Larger")))
				return p_vi->scope.getProperty<Currency>(p_vi->key,p_vi->property);
			else if(action == Write && !stringcasecmp(p_vi->property,"CurrencySystem"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).System);
			else if(action == Write && !stringcasecmp(p_vi->property,"Name"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).Name);
			else if(action == Write && !stringcasecmp(p_vi->property,"Smaller"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).Smaller);
			else if(action == Write && !stringcasecmp(p_vi->property,"Larger"))
				p_vi->scope.setProperty<Currency,std::string>(p_vi->key,p_vi->property,Currency(value).Larger);
			else if(action == Write && !stringcasecmp(p_vi->property,"SmallerAmount"))
				p_vi->scope.setProperty<Currency,int>(p_vi->key,p_vi->property,Currency(value).SmallerAmount);
		}

		// Should only execute if action == Write
		p_vi->scope.save();
		return "";
	}
	else if(action == Read)// If the last character is a '/' and we are just printing the value, print a list of keys and constructors
	{
		return getSetStr(*p_vi);
	}
	else if(action == SetAdd || action == SetAddA ||
		action == SetRemove || action == SetRemoveA)// Perform write operation on variable set
	{
		struct RemovedKey
		{
			bool isRemoved = false;
			char type = 0;
		};

		//Make sure we don't remove keys that aren't part of the set being altered
		if(action == SetRemove || action == SetRemoveA)
		{
			for(const auto& [rhs_k,rhs_v] : getSet(value))
			{
				bool keyFound = false;
				for(const auto& [lhs_k,lhs_v] : getSet(getSetStr(*p_vi)))
				{
					if(lhs_k == rhs_k)
					{
						keyFound = true;
						break;
					}
				}
				if(!keyFound)
				{
					output(Error,"\"%s\" is not contained within the LHS set.",rhs_k.c_str());
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
					   (action == SetAdd || action == SetAddA))
						p_vi->scope.set<Var>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == VAR_SIGIL)
						rk = {p_vi->scope.remove<Var>(k),VAR_SIGIL};
					break;
				case DICE_SIGIL:
					if((p_vi->type == '/' || p_vi->type == DICE_SIGIL) &&
					   (action == SetAdd || action == SetAddA))
						p_vi->scope.set<Dice>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == DICE_SIGIL)
						rk = {p_vi->scope.remove<Dice>(k),DICE_SIGIL};
					break;
				case WALLET_SIGIL:
					if((p_vi->type == '/' || p_vi->type == WALLET_SIGIL) &&
					   (action == SetAdd || action == SetAddA))
						p_vi->scope.set<Wallet>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == WALLET_SIGIL)
						rk = {p_vi->scope.remove<Wallet>(k),WALLET_SIGIL};
					break;
				case CURRENCY_SIGIL:
					if((p_vi->type == '/' || p_vi->type == CURRENCY_SIGIL) &&
					   (action == SetAdd || action == SetAddA))
						p_vi->scope.set<Currency>(p_vi->key+k,v);
					else if(p_vi->type == '/' || p_vi->type == CURRENCY_SIGIL)
						rk = {p_vi->scope.remove<Currency>(k),CURRENCY_SIGIL};
					break;
			}

			if(action == SetAddA)
				output(Info,"Set \"%c%c/%s\" to \"%s\"",p_vi->scope.sigil,p_vi->evalType,(p_vi->key+k).c_str(),v.c_str());
			else if(action == SetRemoveA && rk.isRemoved)
				output(Warning,"Removed \"%c%c/%s\"",p_vi->scope.sigil,rk.type,k.c_str());
		}

		if (action == SetAdd || action == SetRemove) return getSetStr(*p_vi);
		else if(action == SetAddA || action == SetRemoveA) p_vi->scope.save();
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
	for(long unsigned int precedence=0; precedence<operations.size(); precedence++)
	{
		int op_match = findInVect<std::string>(operations[precedence],op);

		try{(void)TL(lhs);}
		catch(const std::runtime_error& e)
		{
			if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
				output(Error,"Invalid LHS explicit constructor: \"%s\"",lhs.c_str());
			else
				output(Error,"Error constructing LHS: %s",e.what());
		}
		if(rhs != "")
		{
			try{(void)TR(rhs);}
			catch(const std::runtime_error& e)
			{
				if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
					output(Error,"Invalid RHS explicit constructor: \"%s\"",rhs.c_str());
				else
					output(Error,"Error constructing RHS: %s",e.what());
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
				output(Error,"Invalid operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else if(e.what() == std::string(E_UNKNOWN_OPERATION))
				output(Error,"Unknown operation: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else if(e.what() == std::string(E_INSUFFICIENT_FUNDS))
				output(Error,"Insufficient funds: \"%s %s %s\"",lhs.c_str(),op.c_str(),rhs.c_str());
			else
				output(Error,"Error during operation: \"%s %s %s\" (%s)",lhs.c_str(),op.c_str(),rhs.c_str(),e.what());

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
		else if(std::is_same_v<TL,Currency> && left(v[rhs_pos],2) == std::string(1,CURRENCY_SIGIL)+"{" && v[op_pos] == OP_ADD)
			return escapeSpaces(std::string(Currency(v[lhs_pos]) + Currency(v[rhs_pos])));
		else if(std::is_same_v<TL,Currency> && left(v[rhs_pos],2) == std::string(1,WALLET_SIGIL)+"{" && v[op_pos] == OP_ADD)
			return escapeSpaces(std::string(Currency(v[lhs_pos]) + Wallet(v[rhs_pos])));
	}
	catch(const std::runtime_error& e)
	{
		if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
			output(Error,"Invalid RHS explicit constructor: %s\n",v[rhs_pos].c_str());
		else
			output(Error,"Error during operation: \"%s %s %s\" (%s)",v[lhs_pos].c_str(),v[op_pos].c_str(),v[rhs_pos].c_str(),e.what());

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
		(void)doAction(vi, Write, result);

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

	chkPrntAppDesc(argv,"Prints values and evaluates operations. Implicitly called when modifying variables.");

	if(argc == 1)
	{
		output(Error,"eval expects at least one argument.");
		exit(-1);
	}

	if(isRequestingHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\teval [%sOPTIONS%s | %sOPERATION%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		fprintf(stdout,"\nOPERATION:\n");
		fprintf(stdout,"\tOne operand and at least one operator, seperated by spaces, which may be wrapped in parentheses.\n");
		fprintf(stdout,"\n\t\te.g. \"2 + 2\", \"@/HitPoints/Current = (16 + 2 - 5)\", \"@/CoinPurse >= @[Store]/Sword/Cost\", \"@/Age ++\"\n");
		fprintf(stdout,"\n\tCurrently supported operators are as follows:\n\n");
		fprintf(stdout,"\t\t Unary  Arithmetic       Assignment          Relational    Boolean\n");
		fprintf(stdout,"\t\t┌─────┐┌───────────┐┌───────────────────┐┌───────────────┐┌───────┐\n");
		fprintf(stdout,"\t\t│%s %s││%s %s %s %s %s %s││%s %s %s %s %s %s %s││%s %s %s %s %s %s││ %s %s │\n",OP_INC,OP_DEC,OP_ADD,OP_SUB,OP_MUL,OP_DIV,OP_EXP,OP_MOD,OP_ASSIGN,OP_ADDA,OP_SUBA,OP_MULA,OP_DIVA,OP_EXPA,OP_MODA,OP_EQ,OP_NE,OP_LT,OP_GT,OP_LE,OP_GE,OP_AND,OP_OR);
		fprintf(stdout,"\t\t└─────┘└───────────┘└───────────────────┘└───────────────┘└───────┘\n");
		fprintf(stdout,"\n\tFor in-depth information on operations, please consult docs/rpgsh-userguide.pdf.\n");

		return 0;
	}

	if(findu(std::string(argv[1]),'/') == std::string::npos)// If the user only enters the scope sigil
	{
		output(Error,"Expected at least one '/' delimiter.");
		exit(-1);
	}

	VariableInfo vi;

	if(strlen(argv[1]) > 1 && (isTypeSigil(argv[1][1]) || argv[1][1] == '/' || argv[1][1] == '['))
		vi = parseVariable(std::string(argv[1]));

	if(argc == 2)// Print data if all the user enters is a variable
	{
		std::string str = doAction(&vi, Read, "");
		if(str != "") fprintf(stdout,"%s\n",str.c_str());
	}
	else if(vi.variable.back() != '/')// Perform operation on variable
	{
		std::string old_value = getAppOutput(vi.variable)[0];
		std::vector<std::string> args;
		long unsigned int open_paren_ctr = 0;
		long unsigned int close_paren_ctr = 0;

		// Need to know final operation to determine whether we print to screen or not
		std::string final_op = std::string(argv[2]);
		if(final_op[0] == '(')
			final_op = right(final_op,1);
		if(final_op.back() == ')' && !isEscaped(final_op,final_op.length()-1))
			final_op = left(final_op,final_op.length()-1);

		// Generate vector of operators and operands, while also determining number of parentheses
		for(int i=1; i<argc; i++)
		{
			std::string arg = std::string(argv[i]);
			long unsigned int open_paren = findu(arg,'(');
			long unsigned int close_paren = rfindu(arg,')');
			open_paren_ctr += countu(arg,'(');
			close_paren_ctr += countu(arg,')');

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
				case '/':
					try{args[0] = vi.scope.getProperty<Var>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<Dice>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<Wallet>(vi.key,vi.property);break;}
					catch(...){}
					try{args[0] = vi.scope.getProperty<Currency>(vi.key,vi.property);break;}
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
				case '/':
					if(vi.scope.keyExists<Var>(vi.key))
					{args[0] = vi.scope.getStr<Var>(vi.key);break;}
					else if(vi.scope.keyExists<Dice>(vi.key))
					{args[0] = vi.scope.getStr<Dice>(vi.key);break;}
					else if(vi.scope.keyExists<Wallet>(vi.key))
					{args[0] = vi.scope.getStr<Wallet>(vi.key);break;}
					else if(vi.scope.keyExists<Currency>(vi.key))
					{args[0] = vi.scope.getStr<Currency>(vi.key);break;}
					else{args[0] = "";}
			}
		}

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

		// Wrap everything in parenthesis just to make below code simpler
		args[0] = '('+args[0];
		args.back() += ')';

		// PEMDAS
		for(int start=0; start<args_size-1; start++)
		{
			if(findu(args[start],'(') != std::string::npos)
			{
				for(int end=start; end<args_size; end++)
				{
					if(args[end].back() == ')' && !isEscaped(args[end],args[end].length()-1))
					{
						// Strip parenthesis off args to ensure good parsing
						std::string rparens = right(args[end],findu(args[end],')')+1);
						args[start] = right(args[start],rfindu(args[start],'(')+1);
						args[end] = left(args[end],findu(args[end],')'));

						// Vectors for each operation type are in order of operator precedence
						// INT_MAX as a maximum for op_pos should be fine given that would be a massively complex equation.
						int op_pos = INT_MAX;
						for(const auto& precedence : operations)
						{
							for(const auto& op : precedence)
							{
								int found_op = findInVect<std::string>(args,op,start);
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
						else if(op_pos == INT_MAX)
						{
							output(Error,"Unknown operator \"%s\"",args[start+1].c_str());
							exit(-1);
						}

						start = -1;
						break;
					}
					else if(findu(args[end],'(') != std::string::npos && end>start)// Nested '('
					{
						start = end - 1;
						break;
					}
				}
			}

			// Check if parenthesis still exist, and re-wrap if needed. Restart loop to go back through PEMDAS if needed
			if(args.size() > 1 && args[0][0] != '(')
				args[0] = '(' + args[0];
			if(args.size() > 1 && args.back().back() != ')')
				args.back() += ')';
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

		// Print result
		std::string new_value = getAppOutput(vi.variable)[0];
		if(findInVect<std::string>(assignOps,final_op) == -1 && findInVect<std::string>(unaryOps,final_op) == -1)
			fprintf(stdout,"%s\n",args[0].c_str());
		else if(old_value == "")
			output(Info,"%c%c/%s has been initialized to %s",
			       vi.scope.sigil,vi.evalType,vi.key.c_str(),new_value.c_str());
		else if(old_value != new_value)
			output(Info,"%c%c/%s has changed from %s to %s",
			       vi.scope.sigil,vi.evalType,vi.key.c_str(),old_value.c_str(),new_value.c_str());
		else
			output(Info,"%c%c/%s is unchanged, current value: %s",
			       vi.scope.sigil,vi.evalType,vi.key.c_str(),old_value.c_str());
	}
	else// Perform operation on set
	{
		if(argc != 4 ||
		   (strcasecmp(argv[2],OP_ADD) && strcasecmp(argv[2],OP_SUB) &&
		    strcasecmp(argv[2],OP_ADDA) && strcasecmp(argv[2],OP_SUBA)))
		{
			output(Error,"Variable set modification only supports the following operators: %s, %s, %s, %s",
			       OP_ADD,OP_SUB,OP_ADDA,OP_SUBA);
			return -1;
		}

		std::string rhs = std::string(argv[3]);

		if(!looksLikeSet(rhs))
		{
			output(Error,"Variable set modifications can only be performed with other variable sets.");
			return -1;
		}

		if(!strcasecmp(argv[2],OP_ADD))
			fprintf(stdout,"%s\n",doAction(&vi, SetAdd, rhs).c_str());
		else if(!strcasecmp(argv[2],OP_ADDA))
			(void)doAction(&vi, SetAddA, rhs);
		else if(!strcasecmp(argv[2],OP_SUB))
			fprintf(stdout,"%s\n",doAction(&vi, SetRemove, rhs).c_str());
		else if(!strcasecmp(argv[2],OP_SUBA))
			(void)doAction(&vi, SetRemoveA, rhs);
	}
	return 0;
}
