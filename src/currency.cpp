#include <cmath>//floor()
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/text.h"
#include "../headers/var.h"

void Currency::tryParseCurrencySystem(datamap<CurrencySystem> currencysystems, std::string* str, std::string fullstr)
{
	std::string cs_match = "";

	try
	{
		cs_match = str->substr(0,str->find(","));
		(*str) = str->substr(str->find(",")+1,
				     str->length()-str->find(",")+1);
	}
	catch(...)
	{
		output(Error,"Unable to parse currencysystem from \"%s\".",fullstr.c_str());
		exit(-1);
	}

	if(cs_match == "")
	{
		//Currency isn't part of a CurrencySystem, so just exit
		exit(-1);
	}

	for(const auto& [k,v] : currencysystems)
	{
		if(!stringcasecmp(v.Name,cs_match))
			AttachToCurrencySystem(std::make_shared<CurrencySystem>(v));
	}
	if(!System)
	{
		output(Error,"No currency system matches \"%s\".",cs_match.c_str());
		exit(-1);
	}
}
void Currency::tryParseName(std::string* str, std::string fullstr)
{
	try
	{
		Name = str->substr(0,str->find(","));
		(*str) = str->substr(str->find(",")+1,
				     str->length()-str->find(",")+1);
	}
	catch(...)
	{
		output(Error,"Unable to parse currency Name from \"%s\".",fullstr.c_str());
		exit(-1);
	}

	if(Name == "")
	{
		output(Error,"Currency Name cannot be empty.");
		exit(-1);
	}
}
void Currency::tryParseSmallerAmount(std::string* str, std::string fullstr)
{
	std::string SmallerAmountStr = "";

	try
	{
		SmallerAmountStr = str->substr(0,str->find(","));
		(*str) = str->substr(str->find(",")+1,
				     str->length()-str->find(",")+1);
	}
	catch(...)
	{
		output(Error,"Unable to parse currency SmallerAmount from \"%s\".",fullstr.c_str());
		exit(-1);
	}

	try
	{
		//Allow an empty SmallerAmount to imply 0
		if(SmallerAmountStr == "")
		{
			SmallerAmount = 0;
			return;
		}
		SmallerAmount = std::stoi(SmallerAmountStr);
	}
	catch(...)
	{
		output(Error,"\"%s\" is not an integer.",SmallerAmountStr.c_str());
		exit(-1);
	}
}
void Currency::tryParseSmaller(std::string* str, std::string fullstr)
{
	try
	{
		Smaller = str->substr(0,str->find(","));
		(*str) = str->substr(str->find(",")+1,
				     str->length()-str->find(",")+1);
	}
	catch(...)
	{
		output(Error,"Unable to parse currency Smaller from \"%s\".",fullstr.c_str());
		exit(-1);
	}
}
void Currency::tryParseLarger(std::string* str, std::string fullstr)
{
	if(str->find("}") == std::string::npos)
	{
		output(Error,"No ending \'}\' found in currency definition.");
	}

	try
	{
		Larger = str->substr(0,str->find("}"));
	}
	catch(...)
	{
		output(Error,"Unable to parse currency Larger from \"%s\".",fullstr.c_str());
		exit(-1);
	}
}

Currency::Currency(){}
Currency::Currency(std::string str)
{
	//FORMAT
	//@c/MyCurrency = c{CurrencySystem,Name,SmallerAmount,Smaller,Larger}

	datamap<CurrencySystem> currencysystems = getDatamapFromAllScopes<CurrencySystem>(CURRENCYSYSTEM_SIGIL);
	std::string fullstr = str;

	//Get number of commas to determine if we are involving a CurrencySystem
	int commas = 0;
	for(const auto& c : str)
	{
		if(c == ',')
			commas++;
	}

	//Make sure start of explicit constructor is formatted correctly
	if(str[1] != '{')
	{
		output(Error,"Missing or misplaced starting \'{\' in currency explicit constructor");
		exit(-1);
	}

	//Make sure first character is 'c'
	if(str[0] != CURRENCY_SIGIL)
	{
		output(Error,"Incorrect data type specifier sigil for currency definition.");
		exit(-1);
	}

	//Check for end of explicit constructor definition
	if(str.find("}") == std::string::npos)
	{
		output(Error,"Currency explicit constructor missing terminating \'}\'.");
		exit(-1);
	}

	//Do we at least have the correct number of args?
	if(commas != 4)
	{
		output(Error,"Invalid argument count in currency explicit constructor. There should be exactly four commas delimiters.");
		exit(-1);
	}

	//Nix the first two characters to make future substrings more intuitive
	str = str.substr(2,str.length()-2);

	//Parse everything
	tryParseCurrencySystem(currencysystems, &str, fullstr);
	tryParseName(&str, fullstr);
	tryParseSmallerAmount(&str, fullstr);
	tryParseSmaller(&str, fullstr);
	tryParseLarger(&str, fullstr);
}
Currency::Currency(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}
Currency::Currency(std::shared_ptr<CurrencySystem> _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;

	AttachToCurrencySystem(_CS);
}

Currency::operator std::string() const
{
	if(System)
		return std::string(1,CURRENCY_SIGIL)+"{"+
		       System->Name+","+
		       Name+","+
		       std::to_string(SmallerAmount)+","+
		       Smaller+","+
		       Larger+"}";
	else
		return std::string(1,CURRENCY_SIGIL)+"{"+
		       Name+","+
		       std::to_string(SmallerAmount)+","+
		       Smaller+","+
		       Larger+"}";
}
const char* Currency::c_str() const
{
	return std::string(*this).c_str();
}
Currency& Currency::operator = ([[maybe_unused]] const int b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator = ([[maybe_unused]] const std::string b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator = (const Var b)
{
	try{return *this = std::stoi(b.Value);}
	catch(...){return *this = b.Value;}
}
Currency& Currency::operator = ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator = ([[maybe_unused]] const Wallet b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator = (const Currency b)
{
	*this = b;
	return *this;
}
Currency& Currency::operator = ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator += (const int b)
{
	SmallerAmount += b;
	return *this;
}
Currency& Currency::operator += (const std::string b)
{
	Name += b;
	return *this;
}
Currency& Currency::operator += ([[maybe_unused]] const Var b)
{
	try{return *this + std::stoi(b.Value);}
	catch(...){return *this + b.Value;}
}
Currency& Currency::operator += ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Wallet& Currency::operator += ([[maybe_unused]] const Wallet b)
{
	b += *this;
	return b;
}
Wallet& Currency::operator += ([[maybe_unused]] const Currency b)
{
	return Wallet("w{"+std::string(*this)+","+"1"+","+std::string(b)+",1}");
}
Currency& Currency::operator += ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator -= ([[maybe_unused]] const int b)
{
	SmallerAmount -= b;
	return *this;
}
Currency& Currency::operator -= ([[maybe_unused]] const std::string b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator -= ([[maybe_unused]] const Var b)
{
	try{return *this + std::stoi(b.Value);}
	catch(...){return *this + b.Value;}
}
Currency& Currency::operator -= ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator -= ([[maybe_unused]] const Wallet b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator -= ([[maybe_unused]] const Currency b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator -= ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Wallet& Currency::operator *= ([[maybe_unused]] const int b)
{
	return Wallet("w{"+std::string(*this)+","+std::to_string(b)+"}");
}
Currency& Currency::operator *= ([[maybe_unused]] const std::string b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator *= ([[maybe_unused]] const Var b)
{
	try{return *this + std::stoi(b.Value);}
	catch(...){return *this + b.Value;}
}
Currency& Currency::operator *= ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator *= ([[maybe_unused]] const Wallet b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator *= ([[maybe_unused]] const Currency b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator *= ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator /= ([[maybe_unused]] const int b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator /= ([[maybe_unused]] const std::string b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator /= ([[maybe_unused]] const Var b)
{
	try{return *this + std::stoi(b.Value);}
	catch(...){return *this + b.Value;}
}
Currency& Currency::operator /= ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator /= ([[maybe_unused]] const Wallet b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator /= ([[maybe_unused]] const Currency b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator /= ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator ^= ([[maybe_unused]] const int b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator ^= ([[maybe_unused]] const std::string b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator ^= ([[maybe_unused]] const Var b)
{
	try{return *this ^= std::stoi(b.Value);}
	catch(...){return *this ^= b.Value;}
}
Currency& Currency::operator ^= ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator ^= ([[maybe_unused]] const Wallet b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator ^= ([[maybe_unused]] const Currency b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator ^= ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator %= ([[maybe_unused]] const int b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator %= ([[maybe_unused]] const std::string b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator %= ([[maybe_unused]] const Var b)
{
	try{return *this %= std::stoi(b.Value);}
	catch(...){return *this %= b.Value;}
}
Currency& Currency::operator %= ([[maybe_unused]] const Dice b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator %= ([[maybe_unused]] const Wallet b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator %= ([[maybe_unused]] const Currency b)
{
		throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator %= ([[maybe_unused]] const CurrencySystem b)
{
		throw std::runtime_error("invalid_operation");
}
Currency Currency::operator + ([[maybe_unused]] const int b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Currency Currency::operator + ([[maybe_unused]] const std::string b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Currency Currency::operator + ([[maybe_unused]] const Var b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Currency Currency::operator + ([[maybe_unused]] const Dice b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Wallet Currency::operator + ([[maybe_unused]] const Wallet b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Wallet Currency::operator + ([[maybe_unused]] const Currency b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Currency Currency::operator + ([[maybe_unused]] const CurrencySystem b)
{
	Currency lhs = *this;
	return (lhs += b);
}
Currency Currency::operator - ([[maybe_unused]] const int b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Currency Currency::operator - ([[maybe_unused]] const std::string b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Currency Currency::operator - ([[maybe_unused]] const Var b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Currency Currency::operator - ([[maybe_unused]] const Dice b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Currency Currency::operator - ([[maybe_unused]] const Wallet b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Currency Currency::operator - ([[maybe_unused]] const Currency b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Currency Currency::operator - ([[maybe_unused]] const CurrencySystem b)
{
	Currency lhs = *this;
	return (lhs -= b);
}
Wallet Currency::operator * ([[maybe_unused]] const int b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator * ([[maybe_unused]] const std::string b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator * ([[maybe_unused]] const Var b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator * ([[maybe_unused]] const Dice b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator * ([[maybe_unused]] const Wallet b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator * ([[maybe_unused]] const Currency b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator * ([[maybe_unused]] const CurrencySystem b)
{
	Currency lhs = *this;
	return (lhs *= b);
}
Currency Currency::operator / ([[maybe_unused]] const int b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator / ([[maybe_unused]] const std::string b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator / ([[maybe_unused]] const Var b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator / ([[maybe_unused]] const Dice b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator / ([[maybe_unused]] const Wallet b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator / ([[maybe_unused]] const Currency b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator / ([[maybe_unused]] const CurrencySystem b)
{
	Currency lhs = *this;
	return (lhs /= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const int b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const std::string b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const Var b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const Dice b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const Wallet b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const Currency b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator ^ ([[maybe_unused]] const CurrencySystem b)
{
	Currency lhs = *this;
	return (lhs ^= b);
}
Currency Currency::operator % ([[maybe_unused]] const int b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
Currency Currency::operator % ([[maybe_unused]] const std::string b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
Currency Currency::operator % ([[maybe_unused]] const Var b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
Currency Currency::operator % ([[maybe_unused]] const Dice b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
Currency Currency::operator % ([[maybe_unused]] const Wallet b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
Currency Currency::operator % ([[maybe_unused]] const Currency b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
Currency Currency::operator % ([[maybe_unused]] const CurrencySystem b)
{
	Currency lhs = *this;
	return (lhs %= b);
}
bool Currency::operator == ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator == ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Currency::operator == ([[maybe_unused]] const Var& b) const
{
	Currency lhs = *this;
	try{return lhs == std::stoi(b.Value);}
	catch(...){return lhs == b.Value;}
}
bool Currency::operator == ([[maybe_unused]] const Dice& b) const
{
	return false;
}
bool Currency::operator == ([[maybe_unused]] const Wallet& b) const
{
	return false;
}
bool Currency::operator == ([[maybe_unused]] const Currency& b) const
{
	return (System == b.System &&
		Name == b.Name &&
		Smaller == b.Smaller &&
		SmallerAmount == b.SmallerAmount &&
		Larger == b.Larger);
}
bool Currency::operator == ([[maybe_unused]] const CurrencySystem& b) const
{
	return false;
}
bool Currency::operator < ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator < ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Currency::operator < ([[maybe_unused]] const Var& b) const
{
	Currency lhs = *this;
	try{return lhs < std::stoi(b.Value);}
	catch(...){return lhs < b.Value;}
}
bool Currency::operator < ([[maybe_unused]] const Dice& b) const
{
	return false;
}
bool Currency::operator < ([[maybe_unused]] const Wallet& b) const
{
	return false;
}
bool Currency::operator < ([[maybe_unused]] const Currency& b) const//Orders std::map with Currency key
{
	if((!System && b.System) || (!b.System && System))
		return false;
	if(!System && !b.System)
	{
		if(b.Larger == Name)
			return true;
		else if(b.Smaller == Name)
			return false;
		else if(b.Name == Name)
			return false;
		else
			return true;
	}
	if(System != b.System)//May be source of some issues due to getting rid of apparently unneeded for loop
		return false;
	else if(b.Larger == Name)
		return true;
	else if(b.Smaller == Name)
		return false;
	else if(b.Name == Name)
		return false;
	else
		return true;

	return (Name < b.Name);
}
bool Currency::operator < ([[maybe_unused]] const CurrencySystem& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const Var& b) const
{
	Currency lhs = *this;
	try{return lhs > std::stoi(b.Value);}
	catch(...){return lhs > b.Value;}
}
bool Currency::operator > ([[maybe_unused]] const Dice& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const Wallet& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const Currency& b) const
{
	if((!System && b.System) || (!b.System && System))
		return false;
	if(!System && !b.System)
	{
		if(b.Larger == Name)
			return false;
		else if(b.Smaller == Name)
			return true;
		else if(b.Name == Name)
			return false;
		else
			return true;
	}
	if(System != b.System)//May be source of some issues due to getting rid of apparently unneeded for loop
		return false;
	else if(b.Larger == Name)
		return false;
	else if(b.Smaller == Name)
		return true;
	else if(b.Name == Name)
		return false;
	else
		return true;

	return (Name > b.Name);
}
bool Currency::operator > ([[maybe_unused]] const CurrencySystem& b) const
{
	return false;
}
bool Currency::operator <= ([[maybe_unused]] const int& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= ([[maybe_unused]] const std::string& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= ([[maybe_unused]] const Var& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= ([[maybe_unused]] const Dice& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= ([[maybe_unused]] const Wallet& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= ([[maybe_unused]] const Currency& b) const//Orders std::map with Currency key
{
	return (*this < b || *this == b);
}
bool Currency::operator <= ([[maybe_unused]] const CurrencySystem& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const int& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const std::string& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const Var& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const Dice& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const Wallet& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const Currency& b) const//Orders std::map with Currency key
{
	return (*this > b || *this == b);
}
bool Currency::operator >= ([[maybe_unused]] const CurrencySystem& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator != ([[maybe_unused]] const int& b) const
{
	return !(*this == b);
}
bool Currency::operator != ([[maybe_unused]] const std::string& b) const
{
	return !(*this == b);
}
bool Currency::operator != ([[maybe_unused]] const Var& b) const
{
	return !(*this == b);
}
bool Currency::operator != ([[maybe_unused]] const Dice& b) const
{
	return !(*this == b);
}
bool Currency::operator != ([[maybe_unused]] const Wallet& b) const
{
	return !(*this == b);
}
bool Currency::operator != ([[maybe_unused]] const Currency& b) const
{
	return !(*this == b);
}
bool Currency::operator != ([[maybe_unused]] const CurrencySystem& b) const
{
	return !(*this == b);
}
Currency& Currency::operator ++ (int)
{
	throw std::runtime_error("invalid_operation");
}
Currency& Currency::operator -- (int)
{
	throw std::runtime_error("invalid_operation");
}

CurrencySystem::operator std::string() const
{
	return std::string(1,CURRENCYSYSTEM_SIGIL)+"{"+Name+"}";
}
const char* CurrencySystem::c_str() const
{
	return std::string(*this).c_str();
}
Currency& CurrencySystem::operator [] (const std::string b)
{
	return Denomination[b];
}
CurrencySystem& CurrencySystem::operator = (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator = (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator = (const Var b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator = (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator = (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator = (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator = (const CurrencySystem b)
{
	Name = b.Name;
	Denomination = b.Denomination;
	transaction = b.transaction;

	return *this;
}
CurrencySystem& CurrencySystem::operator += (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator += (const std::string b)
{
	Name += b;
	return *this;
}
CurrencySystem& CurrencySystem::operator += (const Var b)
{
	try{*this += std::stoi(b.Value);}
	catch(...){*this += b.Value;}
}
CurrencySystem& CurrencySystem::operator += (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator += (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator += (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator += (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator -= (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator -= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator -= (const Var b)
{
	try{*this -= std::stoi(b.Value);}
	catch(...){*this -= b.Value;}

	return *this;
}
CurrencySystem& CurrencySystem::operator -= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator -= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator -= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator -= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator *= (const int b)
{
	try
	{
		Name = std::to_string(std::stoi(Name) * b);
	}
	catch(...)
	{
		if(b < 0)
			throw std::runtime_error("invalid_operation");
		else if(b == 0)
			Name = "";
		else
			for(int i=1; i<b; i++){Name += Name;}
	}

	return *this;
}
CurrencySystem& CurrencySystem::operator *= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator *= (const Var b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator *= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator *= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator *= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator *= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const Var b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator /= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const Var b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator ^= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const Var b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem& CurrencySystem::operator %= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
CurrencySystem CurrencySystem::operator + (const int b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator + (const std::string b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator + (const Var b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator + (const Dice b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator + (const Wallet b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator + (const Currency b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator + (const CurrencySystem b)
{
	CurrencySystem lhs = *this;
	return (lhs += b);
}
CurrencySystem CurrencySystem::operator - (const int b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator - (const std::string b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator - (const Var b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator - (const Dice b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator - (const Wallet b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator - (const Currency b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator - (const CurrencySystem b)
{
	CurrencySystem lhs = *this;
	return (lhs -= b);
}
CurrencySystem CurrencySystem::operator * (const int b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator * (const std::string b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator * (const Var b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator * (const Dice b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator * (const Wallet b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator * (const Currency b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator * (const CurrencySystem b)
{
	CurrencySystem lhs = *this;
	return (lhs *= b);
}
CurrencySystem CurrencySystem::operator / (const int b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator / (const std::string b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator / (const Var b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator / (const Dice b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator / (const Wallet b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator / (const Currency b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator / (const CurrencySystem b)
{
	CurrencySystem lhs = *this;
	return (lhs /= b);
}
CurrencySystem CurrencySystem::operator ^ (const int b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator ^ (const std::string b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator ^ (const Var b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator ^ (const Dice b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator ^ (const Wallet b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator ^ (const Currency b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator ^ (const CurrencySystem b)
{
	CurrencySystem lhs = *this;
	return (lhs ^= b);
}
CurrencySystem CurrencySystem::operator % (const int b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
CurrencySystem CurrencySystem::operator % (const std::string b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
CurrencySystem CurrencySystem::operator % (const Var b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
CurrencySystem CurrencySystem::operator % (const Dice b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
CurrencySystem CurrencySystem::operator % (const Wallet b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
CurrencySystem CurrencySystem::operator % (const Currency b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
CurrencySystem CurrencySystem::operator % (const CurrencySystem b)
{
	CurrencySystem lhs = *this;
	return (lhs %= b);
}
bool CurrencySystem::operator == (const int& b) const
{
	return false;
}
bool CurrencySystem::operator == (const std::string& b) const
{
	return false;
}
bool CurrencySystem::operator == (const Var& b) const
{
	return false;
}
bool CurrencySystem::operator == (const Dice& b) const
{
	return false;
}
bool CurrencySystem::operator == (const Wallet& b) const
{
	return false;
}
bool CurrencySystem::operator == (const Currency& b) const
{
	return false;
}
bool CurrencySystem::operator == (const CurrencySystem& b) const
{
	return Name == b.Name;
}
bool CurrencySystem::operator < (const int& b) const
{
	return false;
}
bool CurrencySystem::operator < (const std::string& b) const
{
	return false;
}
bool CurrencySystem::operator < (const Var& b) const
{
	return false;
}
bool CurrencySystem::operator < (const Dice& b) const
{
	return false;
}
bool CurrencySystem::operator < (const Wallet& b) const
{
	return false;
}
bool CurrencySystem::operator < (const Currency& b) const
{
	return false;
}
bool CurrencySystem::operator < (const CurrencySystem& b) const
{
	return false;
}
bool CurrencySystem::operator > (const int& b) const
{
	return false;
}
bool CurrencySystem::operator > (const std::string& b) const
{
	return false;
}
bool CurrencySystem::operator > (const Var& b) const
{
	return false;
}
bool CurrencySystem::operator > (const Dice& b) const
{
	return false;
}
bool CurrencySystem::operator > (const Wallet& b) const
{
	return false;
}
bool CurrencySystem::operator > (const Currency& b) const
{
	return false;
}
bool CurrencySystem::operator > (const CurrencySystem& b) const
{
	return false;
}
bool CurrencySystem::operator <= (const int& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator <= (const std::string& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator <= (const Var& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator <= (const Dice& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator <= (const Wallet& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator <= (const Currency& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator <= (const CurrencySystem& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator >= (const int& b) const
{
	return (*this > b || *this == b);
}
bool CurrencySystem::operator >= (const std::string& b) const
{
	return (*this > b || *this == b);
}
bool CurrencySystem::operator >= (const Var& b) const
{
	return (*this > b || *this == b);
}
bool CurrencySystem::operator >= (const Dice& b) const
{
	return (*this > b || *this == b);
}
bool CurrencySystem::operator >= (const Wallet& b) const
{
	return (*this > b || *this == b);
}
bool CurrencySystem::operator >= (const Currency& b) const
{
	return (*this > b || *this == b);
}
bool CurrencySystem::operator >= (const CurrencySystem& b) const
{
	return (*this < b || *this == b);
}
bool CurrencySystem::operator != (const int& b) const
{
	return !(*this == b);
}
bool CurrencySystem::operator != (const std::string& b) const
{
	return !(*this == b);
}
bool CurrencySystem::operator != (const Var& b) const
{
	return !(*this == b);
}
bool CurrencySystem::operator != (const Dice& b) const
{
	return !(*this == b);
}
bool CurrencySystem::operator != (const Wallet& b) const
{
	return !(*this == b);
}
bool CurrencySystem::operator != (const Currency& b) const
{
	return !(*this == b);
}
bool CurrencySystem::operator != (const CurrencySystem& b) const
{
	return !(*this == b);
}
CurrencySystem& CurrencySystem::operator ++ (int)
{
	throw std::runtime_error("invalid_operation");
	return *this;
}
CurrencySystem& CurrencySystem::operator -- (int)
{
	throw std::runtime_error("invalid_operation");
	return *this;
}

//Beginning and end iterators.
//So I can use "for(const auto& [key,val] : CurrencySystem){}"
std::map<std::string, Currency>::const_iterator CurrencySystem::begin() const
{
	return Denomination.begin();
}
std::map<std::string, Currency>::const_iterator CurrencySystem::end() const
{
	return Denomination.end();
}

CurrencySystem::CurrencySystem(){}
CurrencySystem::CurrencySystem(std::string str)
{
	std::string s(1,CURRENCYSYSTEM_SIGIL);

	//If second character isn't a '{', this isn't an explicit constructor, so just treat it as a normal value
	if(str[1] != '{')
	{
		Name = str;
		exit(-1);
	}

	//Make sure first character is 's'
	if(str[0] != CURRENCYSYSTEM_SIGIL)
	{
		output(Error,"Incorrect data type specifier sigil for currency system explicit constructor.");
		exit(-1);
	}

	//Check for end of explicit constructor definition
	if(str.find("}") == std::string::npos)
	{
		output(Error,"Missing terminating \'}\' for currency system explicit constructor.");
		exit(-1);
	}

	if(str.substr(0,2) == (s+"{") && str.find("}") != std::string::npos)// Explicit constructor
	{
		Name = str.substr(2,str.find("}")-2);
	}
}

bool Wallet::HasEffectivelyAtLeast(int q, Currency c)
{
	int total = Money[c];
	int totalFactor = 1;

	while(c.Larger != "")
	{
		c = c.System->Denomination[c.Larger];

		totalFactor *= c.SmallerAmount;
		total += Money[c] * totalFactor;
	}

	return total >= q;
}
unsigned int Wallet::getEquivalentValueInLowestDenomination(CurrencySystem system)
{
	Currency smallest = Currency();
	unsigned int total = 0;
	unsigned int totalFactor = 1;

	//Get smallest Currency
	for(const auto& c : system.Denomination)
	{
		if(c.Smaller == "")
		{
			smallest = c;
			break;
		}
	}

	//Get total
	while(c.Larger != "")
	{
		c = c.system.Denomination[c.Larger];

		totalFactor *= c.SmallerAmount;

		if(Money[c])
			total += Money[c] * totalFactor;
	}

	return total;
}
void Wallet::print()
{
	unsigned int NameLength = 0;
	unsigned int LongestName = 0;

	//Get longest currency name as reference for spacing
	for(const auto& [key,value] : Money)
	{
		NameLength = key.Name.length();
		if(NameLength > LongestName)
		{
			LongestName = NameLength;
		}
	}

	//Print formatted list of currencies and the currency values
	for(const auto& [key,value] : Money)
	{
		if(value > 0)
		{
			unsigned int QuantityLength = std::to_string(value).length();
			unsigned int PadLength = (LongestName - key.Name.length()) +
						 QuantityLength +
						 COLUMN_PADDING;
			fprintf(stdout,"%s%s%*d%s\n",key.Name.c_str(),TEXT_WHITE,PadLength,value,TEXT_NORMAL);
		}
	}
}
void Wallet::FloatQuantityToIntCurrency(Currency c, float q)
{
	int totalFactor = 1;

	while(c.Smaller != "")
	{
		totalFactor *= c.SmallerAmount;
		c = c.System->Denomination[c.Smaller];
	}

	*this += money_t(c,(int)(q*totalFactor));
}
bool Wallet::containsCurrency(std::string currency_str)
{
	for(const auto& [c,q] : *this)
	{
		if(!stringcasecmp(currency_str,c.Name))
		{
			return true;
			break;
		}
	}
	return false;
}
Currency Wallet::getExistingCurrency(std::string currency_str)
{
	for(const auto& [c,q] : *this)
	{
		if(!stringcasecmp(currency_str,c.Name))
		{
			return c;
			break;
		}
	}
	return Currency();
}

int& Wallet::operator [] (const Currency b)
{
	return Money[b];
}

//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : Wallet){}"
std::map<Currency, int>::const_iterator Wallet::begin() const
{
	return Money.begin();
}
std::map<Currency, int>::const_iterator Wallet::end() const
{
	return Money.end();
}

Wallet::Wallet(){}
Wallet::Wallet(const money_t m)
{
	Currency c = m.first;
	int q = m.second;

	Money[c] = q;
}
Wallet::Wallet(std::string str)
{
	//FORMAT
	//@w/MyWallet = w{currency_1,quantity_1,currency_2,quantity_2,...,currency_n,quantity_n}

	datamap<Currency> currencies = getDatamapFromAllScopes<Currency>(CURRENCY_SIGIL);
	Currency currency = Currency();

	//Get map of currency names
	datamap<std::string> currencynames;
	for(auto& [k,v] : currencies)
		currencynames[k] = v.Name;

	if(str[1] != '{')
	{
		output(Error,"Expected starting \'{\' at beginning of wallet explicit constructor.");
		exit(-1);
	}

	if(str[0] != WALLET_SIGIL)
	{
		output(Error,"Incorrect data type specifier sigil for wallet explicit constructor.");
		exit(-1);
	}

	//Cut off the initial "w{" to make parsing consistent
	str = str.substr(2,str.length()-2);

	//Main parsing loop
	while(true)
	{
		int str_ln = str.length();
		std::string delimiter = ",";

		if(str.find(delimiter) == std::string::npos)
		{
			output(Error,"Unable to parse currency for wallet, missing \'%s\'",delimiter.c_str());
			exit(-1);
		}

		//Create currency object to be added to wallet
		std::string c(1,CURRENCY_SIGIL);
		std::string currency_str = str.substr(str.find(c),str.find("}")+1-str.find(c));
		int c_str_ln = currency_str.length();
		currency = Currency(currency_str);

		//If we're at the end of the constructor
		const long unsigned int next_delimiter = str.find(delimiter,c_str_ln+1);
		if(next_delimiter == std::string::npos)
			delimiter = "}";

		//Parse quantity string
		std::string quantity_str = str.substr(c_str_ln+1,next_delimiter-c_str_ln-1);
		int q_str_ln = quantity_str.length();

		//Try to add money to the wallet
		try
		{
			Money[currency] += std::stoi(quantity_str);
		}
		catch(...)
		{
			output(Error,"Unable to add the currency \"%s\" in the quantity \"%s\" to the wallet.",currency.Name.c_str(),quantity_str.c_str());
			exit(-1);
		}

		//Remove what we just worked on
		int end_of_last_c = c_str_ln+q_str_ln+2;
		if(delimiter != "}")
			str = str.substr(end_of_last_c,str_ln-c_str_ln);
		else
			break;
	}
}

Wallet::operator std::string() const
{
	//@w/MyWallet = {<currency_1>,<quantity_1>,<currency_2>,<quantity_2>,...,<currency_n>,<quantity_n>}

	std::string ret = std::string(1,WALLET_SIGIL)+"{";
	for(const auto& [c,q] : Money)
		ret += std::string(c) + "," + std::to_string(q) + ",";

	//Cut final ',' and add terminating '}'
	ret = left(ret,ret.length()-1) + "}";

	return ret;
}
const char* Wallet::c_str() const
{
	return std::string(*this).c_str();
}

Wallet& Wallet::operator = (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator = (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator = (const money_t b)
{
	for(const auto& [c,q] : Money)
	{
		Money[c] = 0;
	}
	Money[b.first] = b.second;
	return *this;
}
Wallet& Wallet::operator = (const Var b)
{
	try{*this = std::stoi(b.Value);}
	catch(...){*this = b.Value;}

	return *this;
}
Wallet& Wallet::operator = (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator = (const Wallet b)
{
	*this = b;
	return *this;
}
Wallet& Wallet::operator = (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator = (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator += (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator += (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator += (const money_t b)
{
	Currency c = b.first;
	int q = b.second;

	if(q > 0)
	{
		output(Info,"Crediting %d %s",q,c.Name.c_str());
		Money[c] += q;

		if(c.System && (c.System->Denomination[c.Larger].SmallerAmount <= Money[c]))
		{
			c.System->TradeUp(c.System,this);
		}
	}
	return *this;
}
Wallet& Wallet::operator += (const Var b)
{
	Wallet lhs = *this;
	try{return lhs += std::stoi(b.Value);}
	catch(...){return lhs += b.Value;}
}
Wallet& Wallet::operator += (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator += (const Wallet b)
{
	for(const auto& [c,q] : b)
	{
		*this += money_t(c,q);
	}
	return *this;
}
Wallet& Wallet::operator += (const Currency b)
{
	Money[b]++;
	return *this;
}
Wallet& Wallet::operator += (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator -= (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator -= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator -= (const money_t b)
{
	Currency c = b.first;
	int q = b.second;

	transaction = b;

	if(Money[c]-q < 0 && c.System && HasEffectivelyAtLeast(q,c))
	{
		output(Info,"Debiting %d %s",q,c.Name.c_str());
		Money[c] -= q;
		c.System->MakeChange(c, this);
	}
	else if(Money[c]-q < 0 && c.System)
	{
		output(Error,"Insufficient funds!");
	}
	else if(Money[c]-q >= 0)
	{
		output(Info,"Debiting %d %s",q,c.Name.c_str());
		Money[c] -= q;
	}
	return *this;
}
Wallet& Wallet::operator -= (const Var b)
{
	Wallet lhs = *this;
	try{return lhs -= std::stoi(b.Value);}
	catch(...){return lhs -= b.Value;}
}
Wallet& Wallet::operator -= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator -= (const Wallet b)
{
	for(const auto& [c,q] : b)
	{
		if(!HasEffectivelyAtLeast(q,c))
		{
			output(Error,"Insufficient funds!");
			return *this;
		}
	}

	for(const auto& [c,q] : b)
		*this -= money_t(c,q);
	return *this;
}
Wallet& Wallet::operator += (const Currency b)
{
	if(Money[b]) Money[b]--;
	return *this;
}
Wallet& Wallet::operator += (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator *= (const int b)
{
	output(Info,"Increasing wallet value by a factor of %d",b);
	for(const auto& [c,q] : *this)
	{
		//Keep from printing pointless "Crediting..." messages
		//Avoid zeroing out wallet if you factor by 1
		if(q > 0 && b > 1)
		{
			*this += money_t(c,(q*(b-1)));
		}
	}
	return *this;
}
Wallet& Wallet::operator *= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator *= (const money_t b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator *= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator *= (const Var b)
{
	Wallet lhs = *this;
	try{return lhs *= std::stoi(b.Value);}
	catch(...){return lhs *= b.Value;}
}
Wallet& Wallet::operator *= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator *= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator *= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator /= (const int b)
{
	output(Info,"Decreasing wallet value by a factor of %d",b);
	if(b == 0)
	{
		output(Error,"Attempted to divide wallet value by 0!\n");
		return *this;
	}

	std::map<Currency,float> change;

	for(const auto& [c,q] : *this)
	{
		float quotient = ((float)q/b);
		unsigned int _floor = floor(quotient);

		if(_floor != 0)
		{
			change[c] = (quotient-_floor);
		}
		Money[c] = _floor;
	}

	//Convert everything to the right of the decimal point to the closest whole currencies
	//Will necessarily be a bit lossy just like real finances when dealing with repeating decimals and the like
	for(const auto& [c,q] : change)
	{
		FloatQuantityToIntCurrency(c,q);
	}
	return *this;
}
Wallet& Wallet::operator /= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator /= (const money_t b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator /= (const Var b)
{
	Wallet lhs = *this;
	try{return lhs /= std::stoi(b.Value);}
	catch(...){return lhs /= b.Value;}
}
Wallet& Wallet::operator /= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator /= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator /= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator /= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator ^= (const int b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator ^= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator ^= (const Var b)
{
	Wallet lhs = *this;
	try{return lhs ^= std::stoi(b.Value);}
	catch(...){return lhs ^= b.Value;}
}
Wallet& Wallet::operator ^= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator ^= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator ^= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator ^= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator %= (const int b)
{
	for(auto& [c,q] : Money)
		c %= b;

	return *this;
}
Wallet& Wallet::operator %= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator %= (const Var b)
{
	Wallet lhs = *this;
	try{return lhs %= std::stoi(b.Value);}
	catch(...){return lhs %= b.Value;}
}
Wallet& Wallet::operator %= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator %= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator %= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet& Wallet::operator %= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Wallet Wallet::operator + (const int b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator + (const std::string b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator + (const Var b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator + (const Dice b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator + (const Wallet b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator + (const Currency b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator + (const CurrencySystem b)
{
	Wallet lhs = *this;
	return (lhs += b);
}
Wallet Wallet::operator - (const int b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator - (const std::string b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator - (const Var b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator - (const Dice b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator - (const Wallet b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator - (const Currency b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator - (const CurrencySystem b)
{
	Wallet lhs = *this;
	return (lhs -= b);
}
Wallet Wallet::operator * (const int b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator * (const std::string b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator * (const Var b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator * (const Dice b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator * (const Wallet b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator * (const Currency b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator * (const CurrencySystem b)
{
	Wallet lhs = *this;
	return (lhs *= b);
}
Wallet Wallet::operator / (const int b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator / (const std::string b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator / (const Var b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator / (const Dice b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator / (const Wallet b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator / (const Currency b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator / (const CurrencySystem b)
{
	Wallet lhs = *this;
	return (lhs /= b);
}
Wallet Wallet::operator ^ (const int b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator ^ (const std::string b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator ^ (const Var b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator ^ (const Dice b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator ^ (const Wallet b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator ^ (const Currency b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator ^ (const CurrencySystem b)
{
	Wallet lhs = *this;
	return (lhs ^= b);
}
Wallet Wallet::operator % (const int b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
Wallet Wallet::operator % (const std::string b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
Wallet Wallet::operator % (const Var b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
Wallet Wallet::operator % (const Dice b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
Wallet Wallet::operator % (const Wallet b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
Wallet Wallet::operator % (const Currency b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
Wallet Wallet::operator % (const CurrencySystem b)
{
	Wallet lhs = *this;
	return (lhs %= b);
}
bool Wallet::operator == (const int& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator == (const std::string& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator == (const Var& b) const
{
	try{return *this == std::stoi(b.Value);}
	catch(...){return *this == b.Value;}
}
bool Wallet::operator == (const Dice& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator == (const Wallet& b) const
{
	bool containsCurrency = false;
	for(const auto& [c,q] : Money)
	{
		for(const auto& [b_c,b_q] : b.Money)
		{
			if(c == b_c && q == b_q)
			{
				containsCurrency = true;
				break;
			}
			else if(c == b_c && q != b_q)//Differing like-currency amounts
			{
				return false;
			}
		}
		if(!containsCurrency) return false;//*this contains currency b doesn't have
	}

	containsCurrency = false;
	for(const auto& [b_c,b_q] : b.Money)
	{
		for(const auto& [c,q] : Money)
		{
			if(b_c == c && b_q == q)
			{
				containsCurrency = true;
				break;
			}
			else if(b_c == c && b_q != q)//Differing like-currency amounts
			{
				return false;
			}
		}
		if(!containsCurrency) return false;//b contains currency *this doesn't have
	}

	return true;
}
bool Wallet::operator == (const Currency& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator == (const CurrencySystem& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator < (const int& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator < (const std::string& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator < (const Var& b) const
{
	try{return *this < std::stoi(b.Value);}
	catch(...){return *this < b.Value;}
}
bool Wallet::operator < (const Dice& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator < (const Wallet& b) const
{
	return getWalletValue(*this) < getWalletValue(b);
}
bool Wallet::operator < (const Currency& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator < (const CurrencySystem& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator > (const int& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator > (const std::string& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator > (const Var& b) const
{
	try{return *this > std::stoi(b.Value);}
	catch(...){return *this > b.Value;}
}
bool Wallet::operator > (const Dice& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator > (const Wallet& b) const
{
	return getWalletValue(*this) > getWalletValue(b);
}
bool Wallet::operator > (const Currency& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator > (const CurrencySystem& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator <= (const int& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator <= (const std::string& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator <= (const Var& b) const
{
	try{return *this <= std::stoi(b.Value);}
	catch(...){return *this <= b.Value;}
}
bool Wallet::operator <= (const Dice& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator <= (const Wallet& b) const
{
	return getWalletValue(*this) <= getWalletValue(b);
}
bool Wallet::operator <= (const Currency& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator <= ([[maybe_unused]] const CurrencySystem& b) const
{
	return false;
}
bool Wallet::operator >= (const int& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator >= (const std::string& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator >= (const Var& b) const
{
	try{return *this >= std::stoi(b.Value);}
	catch(...){return *this >= b.Value;}
}
bool Wallet::operator >= (const Dice& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator >= (const Wallet& b) const
{
	return getWalletValue(*this) >= getWalletValue(b);
}
bool Wallet::operator >= (const Currency& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator >= (const CurrencySystem& b) const
{
	(void)decltype(b)();
	return false;
}
bool Wallet::operator != (const int& b) const
{
	return !(*this == b);
}
bool Wallet::operator != (const std::string& b) const
{
	return !(*this == b);
}
bool Wallet::operator != (const Var& b) const
{
	return !(*this == b);
}
bool Wallet::operator != (const Dice& b) const
{
	return !(*this == b);
}
bool Wallet::operator != (const Wallet& b) const
{
	return !(*this == b);
}
bool Wallet::operator != (const Currency& b) const
{
	return !(*this == b);
}
bool Wallet::operator != (const CurrencySystem& b) const
{
	return !(*this == b);
}
Wallet& Wallet::operator ++ ()
{
	//Add 1 of the smallest currency
	for(const auto& [c,q] : Money)
	{
		if(!c.System || c.Smaller == "")
		{
			*this += money_t(c,1);
			break;
		}
	}
	return *this;
}
Wallet& Wallet::operator ++ (int)
{
	return ++(*this);
}
Wallet& Wallet::operator -- ()
{
	//Subtracy 1 of the smallest currency
	for(const auto& [c,q] : Money)
	{
		if(!c.System || c.Smaller == "")
		{
			*this -= money_t(c,1);
			break;
		}
	}
	return *this;
}
Wallet& Wallet::operator -- (int)
{
	return --(*this);
}

//Links the instance of Currency to the instance of CurrencySystem
//Allows the instance of CurrencySystem to detect changes in the attached Currency
//Also automatically adds the Currency to the map of Currencies within the CurrencySystem
void Currency::AttachToCurrencySystem(std::shared_ptr<CurrencySystem> _CurrencySystem)
{
	System = _CurrencySystem;
	System->Denomination[Name] = *this;
}

void CurrencySystem::MakeChange(Currency c, Wallet* w)
{
	std::string NextHighest = c.Larger;
	int ConversionFactor = Denomination[NextHighest].SmallerAmount;
	int transactionAmount = (*w).transaction.second;

	//Number of Currency[NextHighest] that will need to be converted to current currency
	int ChangeCount = (abs(((*w)[c]))/ConversionFactor);

	//Only break an extra higher denomination currency if we really need to
	//e.g. If I owe 13 dimes, but I have 1 dollar and 5 dimes, don't try to break 2 dollars and claim I don't have enough money
	int MinimumAmountToBreak = ChangeCount*ConversionFactor;
	int PreTransactionQuantity = transactionAmount+(*w)[c];
	if((MinimumAmountToBreak + PreTransactionQuantity) < transactionAmount)
	{
		ChangeCount++;
	}

	//Prevent unneccesary subtraction
	//Check if the player has to make change with the currency after that to complete transaction
	if(ChangeCount == 0)
	{
		return;
	}
	else if((*w)[Denomination[NextHighest]] < ChangeCount)
	{
		MakeChange(Denomination[NextHighest],w);
	}

	//Make change by breaking the larger denomination into the smaller denomination
	(*w)[c] += ConversionFactor*ChangeCount;
	(*w) -= money_t(Denomination[NextHighest],ChangeCount);
}
void CurrencySystem::TradeUp(std::shared_ptr<CurrencySystem> S, Wallet* w)
{
	for(auto i = (*w).Money.rbegin(); i != (*w).Money.rend(); ++i)
	{
		//'c' and 'q' for the currency and quantity respectively
		const auto& c = i->first;
		const auto& q = i->second;

		if(c.System == S &&
		c.Larger != "" &&
		q >= S->Denomination[c.Larger].SmallerAmount)
		{
			//Make use of the trucation of the divison return value to int to simplify the math
			int ConversionFactor = S->Denomination[c.Larger].SmallerAmount;
			int AmountTradable = ((*w)[c] / ConversionFactor);
			(*w)[S->Denomination[c.Larger]] += AmountTradable;
			(*w)[c] -= (AmountTradable * ConversionFactor);
		}
	}
}
