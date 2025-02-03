#include <cmath>//floor()
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/text.h"
#include "../headers/var.h"

datamap<Currency> getCurrencySystem(std::string system)
{
	datamap<Currency> denominations;
	for(const auto& [k,v] : getDatamapFromAllScopes<Currency>())
		if(!stringcasecmp(v.System,system)) denominations[v.Name] = v;
	return denominations;
}

void Currency::tryParseCurrencySystem(std::string* str)
{
	try
	{
		long unsigned int comma_pos = findu(*str,',');
		System = left(*str,comma_pos);
		*str = str->substr(comma_pos+1,str->length()-(comma_pos+1));
	}
	catch(const std::runtime_error& e)
	{
		throw e.what();
	}
}
void Currency::tryParseName(std::string* str)
{
	try
	{
		long unsigned int comma_pos = findu(*str,',');
		Name = left(*str,comma_pos);
		*str = str->substr(comma_pos+1,str->length()-(comma_pos+1));
	}
	catch(const std::runtime_error& e)
	{
		throw e.what();
	}
}
void Currency::tryParseSmallerAmount(std::string* str)
{
	try
	{
		long unsigned int comma_pos = findu(*str,',');
		std::string SmallerAmountStr = left(*str,comma_pos);
		*str = str->substr(comma_pos+1,str->length()-(comma_pos+1));

		//Allow an empty SmallerAmount to imply 0
		if(SmallerAmountStr == "") SmallerAmountStr = "0";

		SmallerAmount = std::stoi(SmallerAmountStr);
	}
	catch(std::runtime_error& e)
	{
		throw e.what();
	}
}
void Currency::tryParseSmaller(std::string* str)
{
	try
	{
		long unsigned int comma_pos = findu(*str,',');
		Smaller = left(*str,comma_pos);
		*str = str->substr(comma_pos+1,str->length()-(comma_pos+1));
	}
	catch(std::runtime_error& e)
	{
		throw e.what();
	}
}
void Currency::tryParseLarger(std::string* str)
{
	try
	{
		Larger = left(*str,findu(*str,'}'));
	}
	catch(std::runtime_error& e)
	{
		throw e.what();
	}
}

Currency::Currency(){}
Currency::Currency(const Currency& b)
{
	System = b.System;
	Name = b.Name;
	SmallerAmount = b.SmallerAmount;
	Smaller = b.Smaller;
	Larger = b.Larger;
}
Currency::Currency(std::string str)
{
	//FORMAT
	//@c/MyCurrency = c{CurrencySystem,Name,SmallerAmount,Smaller,Larger}

	//Make sure explicit constructor is formatted correctly
	if(str.length() < 7 ||
	   str[1] != '{' ||
	   str[0] != CURRENCY_SIGIL ||
	   findu(str,'}') == std::string::npos ||
	   countu(str,',') > 4 ||
	   countu(str,',') < 3) throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

	//Nix the first two characters to make future substrings more intuitive
	str = str.substr(2,str.length()-2);

	//Parse everything
	tryParseCurrencySystem(&str);
	tryParseName(&str);
	tryParseSmallerAmount(&str);
	tryParseSmaller(&str);
	tryParseLarger(&str);
}
Currency::Currency(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}
Currency::Currency(std::string _System, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	System = _System;
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}

Currency::operator std::string() const
{
	if(System != "")
		return std::string(1,CURRENCY_SIGIL)+"{"+
		       System+","+
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
Currency::operator bool() const
{
	return Smaller != "";
}
const char* Currency::c_str() const
{
	return std::string(*this).c_str();
}
Currency& Currency::operator = ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator = ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator = (const Var b)
{
	try{return *this = std::stoi(b.Value);}
	catch(...){return *this = b.Value;}
}
Currency& Currency::operator = ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator = ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator = (const Currency b)
{
	System = b.System;
	Name = b.Name;
	SmallerAmount = b.SmallerAmount;
	Smaller = b.Smaller;
	Larger = b.Larger;

	return *this;
}
Currency& Currency::operator += (const int b)
{
	SmallerAmount += b;
	return *this;
}
Currency& Currency::operator += (const std::string b)
{
	Name = mergeQuotes(Name + b);
	return *this;
}
Currency& Currency::operator += (const Var b)
{
	try{return *this += std::stoi(b.Value);}
	catch(...){return *this += b.Value;}
}
Currency& Currency::operator += ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet Currency::operator += (const Wallet b)
{
	Wallet lhs = Wallet(std::string(1,WALLET_SIGIL)+"{"+std::string(*this)+",1}");
	Wallet rhs = b;

	return (lhs + rhs);
}
Wallet Currency::operator += (const Currency b)
{
	Currency lhs = *this;
	return Wallet("w{"+std::string(lhs)+","+"1"+","+std::string(b)+",1}");
}
Currency& Currency::operator -= (const int b)
{
	SmallerAmount -= b;
	return *this;
}
Currency& Currency::operator -= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator -= (const Var b)
{
	try{return *this -= std::stoi(b.Value);}
	catch(...){return *this -= b.Value;}
}
Currency& Currency::operator -= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator -= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator -= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet Currency::operator *= (const int b)
{
	Currency lhs = *this;
	return Wallet("w{"+std::string(lhs)+","+std::to_string(b)+"}");
}
Wallet Currency::operator *= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet Currency::operator *= (const Var b)
{
	try{return *this *= std::stoi(b.Value);}
	catch(...){return *this *= b.Value;}
}
Currency& Currency::operator *= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator *= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator *= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator /= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator /= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator /= (const Var b)
{
	try{return *this /= std::stoi(b.Value);}
	catch(...){return *this /= b.Value;}
}
Currency& Currency::operator /= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator /= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator /= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator ^= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator ^= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator ^= (const Var b)
{
	try{return *this ^= std::stoi(b.Value);}
	catch(...){return *this ^= b.Value;}
}
Currency& Currency::operator ^= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator ^= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator ^= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator %= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator %= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator %= (const Var b)
{
	try{return *this %= std::stoi(b.Value);}
	catch(...){return *this %= b.Value;}
}
Currency& Currency::operator %= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator %= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator %= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency Currency::operator + (const int b)
{
	return *this += b;
}
Currency Currency::operator + (const std::string b)
{
	return *this += b;
}
Currency Currency::operator + (const Var b)
{
	return *this += b;
}
Currency Currency::operator + (const Dice b)
{
	return *this += b;
}
Wallet Currency::operator + (const Wallet b)
{
	return *this += b;
}
Wallet Currency::operator + (const Currency b)
{
	return *this += b;
}
Currency Currency::operator - (const int b)
{
	return *this -= b;
}
Currency Currency::operator - (const std::string b)
{
	return *this -= b;
}
Currency Currency::operator - (const Var b)
{
	return *this -= b;
}
Currency Currency::operator - (const Dice b)
{
	return *this -= b;
}
Currency Currency::operator - (const Wallet b)
{
	return *this -= b;
}
Currency Currency::operator - (const Currency b)
{
	return *this -= b;
}
Wallet Currency::operator * (const int b)
{
	return *this *= b;
}
Wallet Currency::operator * (const std::string b)
{
	return *this *= b;
}
Wallet Currency::operator * (const Var b)
{
	return *this *= b;
}
Currency Currency::operator * (const Dice b)
{
	return *this *= b;
}
Currency Currency::operator * (const Wallet b)
{
	return *this *= b;
}
Currency Currency::operator * (const Currency b)
{
	return *this *= b;
}
Currency Currency::operator / (const int b)
{
	return *this /= b;
}
Currency Currency::operator / (const std::string b)
{
	return *this /= b;
}
Currency Currency::operator / (const Var b)
{
	return *this /= b;
}
Currency Currency::operator / (const Dice b)
{
	return *this /= b;
}
Currency Currency::operator / (const Wallet b)
{
	return *this /= b;
}
Currency Currency::operator / (const Currency b)
{
	return *this /= b;
}
Currency Currency::operator ^ (const int b)
{
	return *this ^= b;
}
Currency Currency::operator ^ (const std::string b)
{
	return *this ^= b;
}
Currency Currency::operator ^ (const Var b)
{
	return *this ^= b;
}
Currency Currency::operator ^ (const Dice b)
{
	return *this ^= b;
}
Currency Currency::operator ^ (const Wallet b)
{
	return *this ^= b;
}
Currency Currency::operator ^ (const Currency b)
{
	return *this ^= b;
}
Currency Currency::operator % (const int b)
{
	return *this %= b;
}
Currency Currency::operator % (const std::string b)
{
	return *this %= b;
}
Currency Currency::operator % (const Var b)
{
	return *this %= b;
}
Currency Currency::operator % (const Dice b)
{
	return *this %= b;
}
Currency Currency::operator % (const Wallet b)
{
	return *this %= b;
}
Currency Currency::operator % (const Currency b)
{
	return *this %= b;
}
bool Currency::operator == ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator == ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Currency::operator == (const Var& b) const
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
bool Currency::operator == (const Currency& b) const
{
	return (System == b.System &&
		Name == b.Name &&
		Smaller == b.Smaller &&
		SmallerAmount == b.SmallerAmount &&
		Larger == b.Larger);
}
bool Currency::operator < ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator < ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Currency::operator < (const Var& b) const
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
bool Currency::operator < (const Currency& b) const//Orders std::map with Currency key
{
	if((System == ""  && b.System != "") || (b.System == "" && System != ""))
		return false;
	if(System == "" && b.System == "")
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

	//No idea why this needs to be in a loop, but it doesn't work right otherwise
	while(true)
	{
		if(System != b.System)
			continue;
		else if(b.Larger == Name)
			return true;
		else if(b.Smaller == Name)
			return false;
		else if(b.Name == Name)
			return false;
		else
			return true;
	}
	return (Name < b.Name);
}
bool Currency::operator > ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Currency::operator > (const Var& b) const
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
bool Currency::operator > (const Currency& b) const
{
	if((System == ""  && b.System != "") || (b.System == "" && System != ""))
		return false;
	if(System == "" && b.System == "")
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

	while(true)
	{
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
	}
	return (Name > b.Name);
}
bool Currency::operator <= (const int& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= (const std::string& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= (const Var& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= (const Dice& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= (const Wallet& b) const
{
	return (*this < b || *this == b);
}
bool Currency::operator <= (const Currency& b) const//Orders std::map with Currency key
{
	return (*this < b || *this == b);
}
bool Currency::operator >= (const int& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= (const std::string& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= (const Var& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= (const Dice& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= (const Wallet& b) const
{
	return (*this > b || *this == b);
}
bool Currency::operator >= (const Currency& b) const//Orders std::map with Currency key
{
	return (*this > b || *this == b);
}
bool Currency::operator != (const int& b) const
{
	return !(*this == b);
}
bool Currency::operator != (const std::string& b) const
{
	return !(*this == b);
}
bool Currency::operator != (const Var& b) const
{
	return !(*this == b);
}
bool Currency::operator != (const Dice& b) const
{
	return !(*this == b);
}
bool Currency::operator != (const Wallet& b) const
{
	return !(*this == b);
}
bool Currency::operator != (const Currency& b) const
{
	return !(*this == b);
}
bool Currency::operator && (const int b)
{
	return bool(*this) && (b != 0);
}
bool Currency::operator && (const std::string b)
{
	return bool(*this) && (b != "" && stringcasecmp(b,"false"));
}
bool Currency::operator && (const Var b)
{
	return bool(*this) && bool(b);
}
bool Currency::operator && (const Dice b)
{
	return bool(*this) && bool(b);
}
bool Currency::operator && (const Wallet b)
{
	return bool(*this) && bool(b);
}
bool Currency::operator && (const Currency b)
{
	return bool(*this) && bool(b);
}
bool Currency::operator || (const int b)
{
	return bool(*this) || (b != 0);
}
bool Currency::operator || (const std::string b)
{
	return bool(*this) || (b != "" && stringcasecmp(b,"false"));
}
bool Currency::operator || (const Var b)
{
	return bool(*this) || bool(b);
}
bool Currency::operator || (const Dice b)
{
	return bool(*this) || bool(b);
}
bool Currency::operator || (const Wallet b)
{
	return bool(*this) || bool(b);
}
bool Currency::operator || (const Currency b)
{
	return bool(*this) || bool(b);
}
Currency& Currency::operator ++ (int)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator -- (int)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}

bool Wallet::HasEffectivelyAtLeast(int q, Currency c)
{
	int total = Money[c];
	int totalFactor = 1;

	while(c.Larger != "")
	{
		c = getCurrencySystem(c.System)[c.Larger];

		totalFactor *= c.SmallerAmount;
		total += Money[c] * totalFactor;
	}

	return total >= q;
}
unsigned int Wallet::getEquivalentValueInLowestDenomination(std::string system)
{
	Currency current_denomination = Currency();
	unsigned int total = 0;
	unsigned int totalFactor = 1;

	//Get smallest Currency
	for(const auto& [k,v] : getCurrencySystem(system))
	{
		if(v.Smaller != "") continue;

		current_denomination = v;
		break;
	}

	//Get total
	while(current_denomination.Larger != "")
	{
		datamap<Currency> s = getCurrencySystem(current_denomination.System);
		if(current_denomination.SmallerAmount > 0)
			totalFactor *= current_denomination.SmallerAmount;

		if(Money[current_denomination])
			total += Money[current_denomination] * totalFactor;

		current_denomination = s[current_denomination.Larger];
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
		c = getCurrencySystem(c.System)[c.Smaller];
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
Wallet::Wallet(const Wallet& b)
{
	Money = b.Money;
	transaction = b.transaction;
}
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

	datamap<Currency> currencies = getDatamapFromAllScopes<Currency>();
	Currency currency = Currency();

	//Get map of currency names
	datamap<std::string> currencynames;
	for(auto& [k,v] : currencies)
		currencynames[k] = v.Name;

	if(str[1] != '{')
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

	if(str[0] == CURRENCY_SIGIL)// Create wallet from currency explicit constructor
	{
		*this = Wallet(std::string(1,WALLET_SIGIL)+"{"+str+",1}");
		return;
	}

	if(str[0] != WALLET_SIGIL)
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

	//Cut off the initial "w{" to make parsing consistent
	str = str.substr(2,str.length()-2);

	if(str == "}") return; // Empty constructor, so don't continue parsing

	//Main parsing loop
	while(true)
	{
		char delimiter = ',';

		if(findu(str,delimiter) == std::string::npos)
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

		//Create currency object to be added to wallet
		std::string c(1,CURRENCY_SIGIL);
		int c_pos = findu(str,c+"{");
		std::string currency_str = str.substr(c_pos,findu(str,'}')+1-c_pos);
		int c_str_ln = currency_str.length();
		currency = Currency(currency_str);
		str = right(str,c_str_ln+1);

		//If we're at the end of the constructor
		if(findu(str,delimiter) == std::string::npos) delimiter = '}';
		const long unsigned int next_delimiter = findu(str,delimiter);

		//Parse quantity string and try to add it to the wallet
		std::string quantity_str = left(str,next_delimiter);
		try
		{
			Money[currency] += std::stoi(quantity_str);
		}
		catch(std::runtime_error& e)
		{
			throw e.what();
		}

		if(delimiter == '}') break;

		//Remove what we just worked on
		str = right(str,findu(str,',')+1);
	}
}

Wallet::operator std::string() const
{
	//@w/MyWallet = {<currency_1>,<quantity_1>,<currency_2>,<quantity_2>,...,<currency_n>,<quantity_n>}

	std::string ret = std::string(1,WALLET_SIGIL)+"{";
	for(const auto& [c,q] : Money)
		ret += std::string(c) + "," + std::to_string(q) + ",";

	//Cut final ',' and add terminating '}'
	if(findu(ret,',') != std::string::npos)
		ret = left(ret,ret.length()-1);
	ret += "}";

	return ret;
}
Wallet::operator bool() const
{
	for(const auto& [c,q] : Money)
		if(q > 0) return true;

	return false;
}
const char* Wallet::c_str() const
{
	return std::string(*this).c_str();
}
Wallet& Wallet::operator = ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator = ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
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
Wallet& Wallet::operator = ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator = (const Wallet b)
{
	Money = b.Money;
	transaction = b.transaction;

	return *this;
}
Wallet& Wallet::operator = (const Currency b)
{
	Money.clear();
	Money[b] = 1;

	return *this;
}
Wallet& Wallet::operator += ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator += ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator += (const money_t b)
{
	Currency c = b.first;
	int q = b.second;

	Money[c] += q;
	datamap<Currency> s = getCurrencySystem(c.System);

	if(c.System != "" && s[c.Larger].SmallerAmount <= Money[c])
		TradeUp(c.System,this);

	return *this;
}
Wallet& Wallet::operator += (const Var b)
{
	Wallet lhs = *this;
	try{return lhs += std::stoi(b.Value);}
	catch(...){return lhs += b.Value;}
}
Wallet& Wallet::operator += ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator += ([[maybe_unused]] const Wallet b)
{
	for(const auto& [c,q] : b)
		*this += money_t(c,q);
	return *this;
}
Wallet& Wallet::operator += (const Currency b)
{
	*this += money_t(b,1);
	return *this;
}
Wallet& Wallet::operator -= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator -= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator -= (const money_t b)
{
	Currency c = b.first;
	int q = b.second;

	transaction = b;

	if(Money[c]-q < 0 && c.System != "" && HasEffectivelyAtLeast(q,c))
	{
		Money[c] -= q;
		MakeChange(c,this);
	}
	else if(Money[c]-q < 0 && c.System != "")//TODO: Maybe create option to allow users to go into debt?
	{
		output(Error,"Insufficient funds!");
	}
	else if(Money[c]-q >= 0)
	{
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
Wallet& Wallet::operator -= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
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

	for([[maybe_unused]] const auto& [c,q] : b)
		*this -= money_t(c,q);
	return *this;
}
Wallet& Wallet::operator -= (const Currency b)
{
	*this -= money_t(b,1);
	return *this;
}
Wallet& Wallet::operator *= (const int b)
{
	for(const auto& [c,q] : *this)
		if(b > 1) *this += money_t(c,(q*(b-1)));//Avoid zeroing out wallet if user factors by 1

	return *this;
}
Wallet& Wallet::operator *= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator *= ([[maybe_unused]] const money_t b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator *= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator *= (const Var b)
{
	try{return *this *= std::stoi(b.Value);}
	catch(...){return *this *= b.Value;}
}
Wallet& Wallet::operator *= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator *= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator /= (const int b)
{
	if(!b) throw std::runtime_error(E_INVALID_OPERATION);

	std::map<Currency,float> change;

	for(const auto& [c,q] : *this)
	{
		float quotient = ((float)q/b);
		unsigned int _floor = floor(quotient);

		if(_floor) change[c] = (quotient-_floor);
		Money[c] = _floor;
	}

	//Convert everything to the right of the decimal point to the closest whole currencies
	//Will necessarily be a bit lossy just like real finances when dealing with repeating decimals and the like
	for(const auto& [c,q] : change)
		FloatQuantityToIntCurrency(c,q);

	return *this;
}
Wallet& Wallet::operator /= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator /= ([[maybe_unused]] const money_t b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator /= (const Var b)
{
	try{return *this /= std::stoi(b.Value);}
	catch(...){return *this /= b.Value;}
}
Wallet& Wallet::operator /= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator /= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator /= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator ^= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator ^= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator ^= (const Var b)
{
	try{return *this ^= std::stoi(b.Value);}
	catch(...){return *this ^= b.Value;}
}
Wallet& Wallet::operator ^= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator ^= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator ^= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator %= (const int b)
{
	for(auto& [c,q] : Money) q %= b;

	return *this;
}
Wallet& Wallet::operator %= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator %= (const Var b)
{
	try{return *this %= std::stoi(b.Value);}
	catch(...){return *this %= b.Value;}
}
Wallet& Wallet::operator %= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator %= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator %= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet Wallet::operator + (const int b)
{
	return *this += b;
}
Wallet Wallet::operator + (const std::string b)
{
	return *this += b;
}
Wallet Wallet::operator + (const Var b)
{
	return *this += b;
}
Wallet Wallet::operator + (const Dice b)
{
	return *this += b;
}
Wallet Wallet::operator + (const Wallet b)
{
	return *this += b;
}
Wallet Wallet::operator + (const Currency b)
{
	return *this += b;
}
Wallet Wallet::operator - (const int b)
{
	return *this -= b;
}
Wallet Wallet::operator - (const std::string b)
{
	return *this -= b;
}
Wallet Wallet::operator - (const Var b)
{
	return *this -= b;
}
Wallet Wallet::operator - (const Dice b)
{
	return *this -= b;
}
Wallet Wallet::operator - (const Wallet b)
{
	return *this -= b;
}
Wallet Wallet::operator - (const Currency b)
{
	return *this -= b;
}
Wallet Wallet::operator * (const int b)
{
	return *this *= b;
}
Wallet Wallet::operator * (const std::string b)
{
	return *this *= b;
}
Wallet Wallet::operator * (const Var b)
{
	return *this *= b;
}
Wallet Wallet::operator * (const Dice b)
{
	return *this *= b;
}
Wallet Wallet::operator * (const Wallet b)
{
	return *this *= b;
}
Wallet Wallet::operator * (const Currency b)
{
	return *this *= b;
}
Wallet Wallet::operator / (const int b)
{
	return *this /= b;
}
Wallet Wallet::operator / (const std::string b)
{
	return *this /= b;
}
Wallet Wallet::operator / (const Var b)
{
	return *this /= b;
}
Wallet Wallet::operator / (const Dice b)
{
	return *this /= b;
}
Wallet Wallet::operator / (const Wallet b)
{
	return *this /= b;
}
Wallet Wallet::operator / (const Currency b)
{
	return *this /= b;
}
Wallet Wallet::operator ^ (const int b)
{
	return *this ^= b;
}
Wallet Wallet::operator ^ (const std::string b)
{
	return *this ^= b;
}
Wallet Wallet::operator ^ (const Var b)
{
	return *this ^= b;
}
Wallet Wallet::operator ^ (const Dice b)
{
	return *this ^= b;
}
Wallet Wallet::operator ^ (const Wallet b)
{
	return *this ^= b;
}
Wallet Wallet::operator ^ (const Currency b)
{
	return *this ^= b;
}
Wallet Wallet::operator % (const int b)
{
	return *this %= b;
}
Wallet Wallet::operator % (const std::string b)
{
	return *this %= b;
}
Wallet Wallet::operator % (const Var b)
{
	return *this %= b;
}
Wallet Wallet::operator % (const Dice b)
{
	return *this %= b;
}
Wallet Wallet::operator % (const Wallet b)
{
	return *this %= b;
}
Wallet Wallet::operator % (const Currency b)
{
	return *this %= b;
}
bool Wallet::operator == ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Wallet::operator == ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Wallet::operator == (const Var& b) const
{
	try{return *this == std::stoi(b.Value);}
	catch(...){return *this == b.Value;}
}
bool Wallet::operator == ([[maybe_unused]] const Dice& b) const
{
	return false;
}
bool Wallet::operator == (const Wallet& b) const
{
	bool containsCurrency = false;
	for([[maybe_unused]] const auto& [c,q] : Money)
	{
		for([[maybe_unused]] const auto& [b_c,b_q] : b.Money)
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
	for([[maybe_unused]] const auto& [b_c,b_q] : b.Money)
	{
		for([[maybe_unused]] const auto& [c,q] : Money)
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
	Wallet w = Wallet(std::string(1,WALLET_SIGIL)+"{"+std::string(b)+",1}");
	return getWalletValue(*this) == getWalletValue(w);
}
bool Wallet::operator < ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Wallet::operator < ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Wallet::operator < (const Var& b) const
{
	try{return *this < std::stoi(b.Value);}
	catch(...){return *this < b.Value;}
}
bool Wallet::operator < ([[maybe_unused]] const Dice& b) const
{
	return false;
}
bool Wallet::operator < (const Wallet& b) const
{
	return getWalletValue(*this) < getWalletValue(b);
}
bool Wallet::operator < (const Currency& b) const
{
	Wallet w = Wallet(std::string(1,WALLET_SIGIL)+"{"+std::string(b)+",1}");
	return getWalletValue(*this) < getWalletValue(w);
}
bool Wallet::operator > ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Wallet::operator > ([[maybe_unused]] const std::string& b) const
{
	return false;
}
bool Wallet::operator > (const Var& b) const
{
	try{return *this > std::stoi(b.Value);}
	catch(...){return *this > b.Value;}
}
bool Wallet::operator > ([[maybe_unused]] const Dice& b) const
{
	return false;
}
bool Wallet::operator > (const Wallet& b) const
{
	return getWalletValue(*this) > getWalletValue(b);
}
bool Wallet::operator > ([[maybe_unused]] const Currency& b) const
{
	Wallet w = Wallet(std::string(1,WALLET_SIGIL)+"{"+std::string(b)+",1}");
	return getWalletValue(*this) > getWalletValue(w);
}
bool Wallet::operator <= (const int& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator <= (const std::string& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator <= (const Var& b) const
{
	try{return *this <= std::stoi(b.Value);}
	catch(...){return *this <= b.Value;}
}
bool Wallet::operator <= (const Dice& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator <= (const Wallet& b) const
{
	return getWalletValue(*this) <= getWalletValue(b);
}
bool Wallet::operator <= (const Currency& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator >= (const int& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator >= (const std::string& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator >= (const Var& b) const
{
	try{return *this >= std::stoi(b.Value);}
	catch(...){return *this >= b.Value;}
}
bool Wallet::operator >= (const Dice& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator >= (const Wallet& b) const
{
	return getWalletValue(*this) >= getWalletValue(b);
}
bool Wallet::operator >= (const Currency& b) const
{
	return *this < b || *this == b;
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
bool Wallet::operator && (const int b)
{
	return bool(*this) && (b != 0);
}
bool Wallet::operator && (const std::string b)
{
	return bool(*this) && (b != "" && stringcasecmp(b,"false"));
}
bool Wallet::operator && (const Var b)
{
	return bool(*this) && bool(b);
}
bool Wallet::operator && (const Dice b)
{
	return bool(*this) && bool(b);
}
bool Wallet::operator && (const Wallet b)
{
	return bool(*this) && bool(b);
}
bool Wallet::operator && (const Currency b)
{
	return bool(*this) && bool(b);
}
bool Wallet::operator || (const int b)
{
	return bool(*this) || (b != 0);
}
bool Wallet::operator || (const std::string b)
{
	return bool(*this) || (b != "" && stringcasecmp(b,"false"));
}
bool Wallet::operator || (const Var b)
{
	return bool(*this) || bool(b);
}
bool Wallet::operator || (const Dice b)
{
	return bool(*this) || bool(b);
}
bool Wallet::operator || (const Wallet b)
{
	return bool(*this) || bool(b);
}
bool Wallet::operator || (const Currency b)
{
	return bool(*this) || bool(b);
}
Wallet& Wallet::operator ++ ()
{
	//Add 1 of the smallest currency
	for(const auto& [c,q] : Money)
	{
		if(c.System == "" || c.Smaller == "")
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
		if(c.System == "" || c.Smaller == "")
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

void MakeChange(Currency c, Wallet* w)
{
	datamap<Currency> s = getCurrencySystem(c.System);
	std::string NextHighest = c.Larger;
	int ConversionFactor = s[NextHighest].SmallerAmount;
	int transactionAmount = (*w).transaction.second;

	//Number of Currency[NextHighest] that will need to be converted to current currency
	int ChangeCount = (abs(((*w)[c]))/ConversionFactor);

	//Only break an extra higher denomination currency if we really need to
	//e.g. If I owe 13 dimes, but I have 1 dollar and 5 dimes, don't try to break 2 dollars and claim I don't have enough money
	int MinimumAmountToBreak = ChangeCount*ConversionFactor;
	int PreTransactionQuantity = transactionAmount+(*w)[c];
	if((MinimumAmountToBreak + PreTransactionQuantity) < transactionAmount)
		ChangeCount++;

	//Prevent unneccesary subtraction
	//Check if the player has to make change with the currency after that to complete transaction
	if(ChangeCount == 0)
		return;
	else if((*w)[s[NextHighest]] < ChangeCount)
		MakeChange(s[NextHighest],w);

	//Make change by breaking the larger denomination into the smaller denomination
	(*w)[c] += ConversionFactor*ChangeCount;
	(*w) -= money_t(s[NextHighest],ChangeCount);
}
void TradeUp(std::string system, Wallet* w)
{
	datamap<Currency> s = getCurrencySystem(system);
	for(const auto& [c,q] : (*w).Money)
	{
		if(c.System == system && c.Larger != "" && q >= s[c.Larger].SmallerAmount)
		{
			//Make use of the trucation of the divison return value to int to simplify the math
			int ConversionFactor = s[c.Larger].SmallerAmount;
			int AmountTradable = ((*w)[c] / ConversionFactor);
			(*w)[s[c.Larger]] += AmountTradable;
			(*w)[c] -= (AmountTradable * ConversionFactor);
		}
	}
}
