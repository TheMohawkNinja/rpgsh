#include <cmath>//floor()
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/text.h"
#include "../headers/var.h"

datamap<Currency> getCurrencySystem(std::wstring system)
{
	datamap<Currency> denominations;
	for(const auto& [k,v] : getDatamapFromAllScopes<Currency>())
		if(!stringcasecmp(v.System,system)) denominations[v.Name] = v;

	return denominations;
}

void Currency::tryParseCurrencySystem(std::wstring* str)
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
void Currency::tryParseName(std::wstring* str)
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
void Currency::tryParseSmallerAmount(std::wstring* str)
{
	try
	{
		long unsigned int comma_pos = findu(*str,',');
		std::wstring SmallerAmountStr = left(*str,comma_pos);
		*str = str->substr(comma_pos+1,str->length()-(comma_pos+1));

		//Allow an empty SmallerAmount to imply 0
		if(SmallerAmountStr == L"") SmallerAmountStr = L"0";

		SmallerAmount = std::stoi(SmallerAmountStr);
	}
	catch(std::runtime_error& e)
	{
		throw e.what();
	}
}
void Currency::tryParseSmaller(std::wstring* str)
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
void Currency::tryParseLarger(std::wstring* str)
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
Currency::Currency(std::wstring str)
{
	//FORMAT
	//@c/MyCurrency = c{CurrencySystem,Name,SmallerAmount,Smaller,Larger}

	//Make sure explicit constructor is formatted correctly
	if(str.length() < 7 ||
	   str[1] != '{' ||
	   str[0] != CURRENCY_SIGIL ||
	   findu(str,'}') == std::wstring::npos ||
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
Currency::Currency(std::wstring _Name, int _SmallerAmount, std::wstring _Smaller, std::wstring _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}
Currency::Currency(std::wstring _System, std::wstring _Name, int _SmallerAmount, std::wstring _Smaller, std::wstring _Larger)
{
	System = _System;
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}

Currency::operator std::wstring() const
{
	if(System != L"")
		return std::wstring(1,CURRENCY_SIGIL)+L"{"+
		       System+L","+
		       Name+L","+
		       std::to_wstring(SmallerAmount)+L","+
		       Smaller+L","+
		       Larger+L"}";
	else
		return std::wstring(1,CURRENCY_SIGIL)+L"{"+
		       Name+L","+
		       std::to_wstring(SmallerAmount)+L","+
		       Smaller+L","+
		       Larger+L"}";
}
Currency::operator bool() const
{
	return Smaller != L"";
}
const wchar_t* Currency::c_str() const
{
	return std::wstring(*this).c_str();
}
Currency& Currency::operator = ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Currency& Currency::operator = ([[maybe_unused]] const std::wstring b)
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
Currency& Currency::operator += (const std::wstring b)
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
	Wallet lhs = Wallet(std::wstring(1,WALLET_SIGIL)+L"{"+std::wstring(*this)+L",1}");
	Wallet rhs = b;

	return (lhs + rhs);
}
Wallet Currency::operator += (const Currency b)
{
	Currency lhs = *this;
	return Wallet(L"w{"+std::wstring(lhs)+L","+L"1"+L","+std::wstring(b)+L",1}");
}
Currency& Currency::operator -= (const int b)
{
	SmallerAmount -= b;
	return *this;
}
Currency& Currency::operator -= ([[maybe_unused]] const std::wstring b)
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
	return Wallet(L"w{"+std::wstring(lhs)+L","+std::to_wstring(b)+L"}");
}
Wallet Currency::operator *= ([[maybe_unused]] const std::wstring b)
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
Currency& Currency::operator /= ([[maybe_unused]] const std::wstring b)
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
Currency& Currency::operator ^= ([[maybe_unused]] const std::wstring b)
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
Currency& Currency::operator %= ([[maybe_unused]] const std::wstring b)
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
Currency Currency::operator + (const std::wstring b)
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
Currency Currency::operator - (const std::wstring b)
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
Wallet Currency::operator * (const std::wstring b)
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
Currency Currency::operator / (const std::wstring b)
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
Currency Currency::operator ^ (const std::wstring b)
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
Currency Currency::operator % (const std::wstring b)
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
bool Currency::operator == ([[maybe_unused]] const std::wstring& b) const
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
bool Currency::operator < ([[maybe_unused]] const std::wstring& b) const
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
	if(stringcasecmp(System,b.System))
		return false;
	else if(!stringcasecmp(b.Larger,Name))
		return true;
	else if(!stringcasecmp(b.Smaller,Name))
		return false;
	else if(!stringcasecmp(b.Name,Name))
		return false;
	else
		return true;

	return (Name < b.Name);
}
bool Currency::operator > ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Currency::operator > ([[maybe_unused]] const std::wstring& b) const
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
	if((System == L""  && b.System != L"") || (b.System == L"" && System != L""))
		return false;
	if(System == L"" && b.System == L"")
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
bool Currency::operator <= (const std::wstring& b) const
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
bool Currency::operator >= (const std::wstring& b) const
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
bool Currency::operator != (const std::wstring& b) const
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
bool Currency::operator && (const std::wstring b)
{
	return bool(*this) && (b != L"" && stringcasecmp(b,L"false"));
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
bool Currency::operator || (const std::wstring b)
{
	return bool(*this) || (b != L"" && stringcasecmp(b,L"false"));
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
	int total = get(c);
	int totalFactor = 1;

	while(c.Larger != L"")
	{
		c = getCurrencySystem(c.System)[c.Larger];

		totalFactor *= c.SmallerAmount;
		total += get(c) * totalFactor;
	}

	return total >= q;
}
unsigned int Wallet::getEquivalentValueInLowestDenomination(std::wstring system)
{
	Currency current_denomination = Currency();
	unsigned int total = 0;
	unsigned int totalFactor = 1;

	//Get smallest Currency
	for(const auto& [k,v] : getCurrencySystem(system))
	{
		if(v.Smaller != L"") continue;
		current_denomination = v;
		break;
	}

	//Get total
	while(true)
	{
		datamap<Currency> s = getCurrencySystem(current_denomination.System);
		if(current_denomination.SmallerAmount > 0)
			totalFactor *= current_denomination.SmallerAmount;

		if(get(current_denomination))
			total += get(current_denomination) * totalFactor;

		if(current_denomination.Larger == L"") break;
		current_denomination = s[current_denomination.Larger];
	}

	return total;
}
void Wallet::print()
{
	unsigned int NameLength = 0;
	unsigned int LongestName = 0;

	//Get longest currency name as reference for spacing
	for(const auto& [key,value] : Monies)
	{
		NameLength = key.Name.length();
		if(NameLength > LongestName)
		{
			LongestName = NameLength;
		}
	}

	//Print formatted list of currencies and the currency values
	for(const auto& [key,value] : Monies)
	{
		if(value > 0)
		{
			unsigned int QuantityLength = std::to_string(value).length();
			unsigned int PadLength = (LongestName - key.Name.length()) +
						 QuantityLength +
						 COLUMN_PADDING;
			fprintf(stdout,"%ls%s%*d%s\n",key.Name.c_str(),TEXT_WHITE,PadLength,value,TEXT_NORMAL);
		}
	}
}
void Wallet::FloatQuantityToIntCurrency(Currency c, float q)
{
	int totalFactor = 1;

	while(c.Smaller != L"")
	{
		totalFactor *= c.SmallerAmount;
		c = getCurrencySystem(c.System)[c.Smaller];
	}

	*this += Money{c,(int)(q*totalFactor)};
}
bool Wallet::containsCurrency(std::wstring currency_str)
{
	for(const auto& m : Monies)
	{
		if(!stringcasecmp(currency_str,m.c.Name))
		{
			return true;
			break;
		}
	}
	return false;
}
Currency Wallet::getExistingCurrency(std::wstring currency_str)
{
	for(const auto& m : Monies)
	{
		if(!stringcasecmp(currency_str,m.c.Name))
		{
			return m.c;
			break;
		}
	}
	return Currency();
}

Wallet::Wallet(){}
Wallet::Wallet(const Wallet& b)
{
	Monies = b.Monies;
	transaction = b.transaction;
}
Wallet::Wallet(const Money m)
{
	set(m.c,m.q);
}
Wallet::Wallet(std::wstring str)
{
	//FORMAT
	//@w/MyWallet = w{currency_1,quantity_1,currency_2,quantity_2,...,currency_n,quantity_n}

	datamap<Currency> currencies = getDatamapFromAllScopes<Currency>();
	Currency currency = Currency();

	//Get map of currency names
	datamap<std::wstring> currencynames;
	for(auto& [k,v] : currencies)
		currencynames[k] = v.Name;

	if(str[1] != '{')
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

	if(str[0] == CURRENCY_SIGIL)// Create wallet from currency explicit constructor
	{
		*this = Wallet(std::wstring(1,WALLET_SIGIL)+L"{"+str+L",1}");
		return;
	}

	if(str[0] != WALLET_SIGIL)
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

	//Cut off the initial "w{" to make parsing consistent
	str = str.substr(2,str.length()-2);

	if(str == L"}") return; // Empty constructor, so don't continue parsing

	//Main parsing loop
	while(true)
	{
		char delimiter = ',';

		if(findu(str,delimiter) == std::wstring::npos)
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

		//Create currency object to be added to wallet
		std::wstring c(1,CURRENCY_SIGIL);
		const long unsigned int c_pos = findu(str,c+L"{");
		if(c_pos == std::wstring::npos)
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
		std::wstring currency_str = str.substr(c_pos,findu(str,'}')+1-c_pos);
		const long unsigned int c_str_ln = currency_str.length();
		if(c_str_ln == std::wstring::npos)
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
		currency = Currency(currency_str);
		str = right(str,c_str_ln+1);

		//If we're at the end of the constructor
		if(findu(str,delimiter) == std::wstring::npos) delimiter = '}';
		const long unsigned int next_delimiter = findu(str,delimiter);

		//Parse quantity string and try to add it to the wallet
		std::wstring quantity_str = left(str,next_delimiter);
		try
		{
			set(currency,std::stoi(quantity_str));
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

Wallet::operator std::wstring() const
{
	//@w/MyWallet = {<currency_1>,<quantity_1>,<currency_2>,<quantity_2>,...,<currency_n>,<quantity_n>}

	std::wstring ret = std::wstring(1,WALLET_SIGIL)+L"{";
	for(const auto& m : Monies)
		ret += std::wstring(m.c)+L","+std::to_wstring(m.q)+L",";

	//Cut final ',' and add terminating '}'
	if(findu(ret,',') != std::wstring::npos)
		ret = left(ret,ret.length()-1);
	ret += L"}";

	return ret;
}
Wallet::operator bool() const
{
	for(const auto& m : Monies)
		if(m.q > 0) return true;

	return false;
}
const wchar_t* Wallet::c_str() const
{
	return std::wstring(*this).c_str();
}

void Wallet::sort()
{
	//Sort first by system alphabetically
	for(long unsigned int i=0; i<Monies.size(); i++)
	{
		if(i && Monies[i].c.System < Monies[i-1].c.System)
		{
			Money tmp = Monies[i-1];
			Monies[i-1] = Monies[i];
			Monies[i] = tmp;
			i=0;
		}
	}

	//Find where each system starts
	typedef std::pair<std::wstring,long unsigned int> sys_index;
	std::vector<sys_index> systems;
	for(long unsigned int i=0; i<Monies.size(); i++)
		if(!systems.size() || Monies[i].c.System != systems[systems.size()-1].first)
			systems.push_back(sys_index(Monies[i].c.System,i));

	//Main sorting algorithm
	for(long unsigned int system=0; system<systems.size(); system++)
	{
		sys_index s = systems[system];
		for(long unsigned int i=s.second; i<Monies.size()-1 && Monies[i].c.System == s.first; i++)
		{
			if(i > s.second && Monies[i].c.Larger == L"")
			{
				Money tmp = Monies[s.second];
				Monies[s.second] = Monies[i];
				Monies[i] = tmp;
				i=s.second;
			}
			else if(i > s.second && Monies[i-1].c.Larger == Monies[i].c.Name)
			{
				Money tmp = Monies[i-1];
				Monies[i-1] = Monies[i];
				Monies[i] = tmp;
				i=s.second;
			}
			else if(system < systems.size()-1 && Monies[i+1].c.System == s.first && Monies[i+1].c.Smaller == Monies[i].c.Name)
			{
				Money tmp = Monies[i+1];
				Monies[i+1] = Monies[i];
				Monies[i] = tmp;
				i=s.second;
			}
			else if(system == systems.size()-1 && i < Monies.size()-1 && Monies[i+1].c.Smaller == Monies[i].c.Name)
			{
				Money tmp = Monies[i+1];
				Monies[i+1] = Monies[i];
				Monies[i] = tmp;
				i=s.second;
			}
			else if(system < systems.size()-1 && Monies[i+1].c.System == s.first && Monies[i].c.Smaller == L"")
			{
				Money tmp = Monies[systems[system+1].second-1];
				Monies[systems[system+1].second-1] = Monies[i];
				Monies[i] = tmp;
				i=s.second;
			}
			else if(system == systems.size()-1 && i < Monies.size()-1 && Monies[i].c.Smaller == L"")
			{
				Money tmp = Monies.back();
				Monies.back() = Monies[i];
				Monies[i] = tmp;
				i=s.second;
			}
		}
	}
}

int Wallet::get(const Currency c)
{
	for(const auto& m : Monies)
		if(c == m.c) return m.q;

	return 0;
}
void Wallet::set(const Currency c, const int q)
{
	for(auto& m : Monies)
	{
		if(c != m.c) continue;
		m.q = q;
		sort();
		return;
	}
	Monies.push_back(Money{c,q});
	sort();
}

Wallet& Wallet::operator = ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator = ([[maybe_unused]] const std::wstring b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator = (const Money b)
{
	for(auto& m : Monies)
		m.q = 0;
	set(b.c,b.q);
	sort();
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
	Monies = b.Monies;
	transaction = b.transaction;

	return *this;
}
Wallet& Wallet::operator = (const Currency b)
{
	Monies.clear();
	set(b,1);

	return *this;
}
Wallet& Wallet::operator += ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator += ([[maybe_unused]] const std::wstring b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator += (const Money b)
{
	set(b.c,get(b.c)+b.q);
	datamap<Currency> s = getCurrencySystem(b.c.System);

	if(b.c.System != L"" && s[b.c.Larger].SmallerAmount <= get(b.c))
		TradeUp(b.c.System,this);

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
Wallet& Wallet::operator += (const Wallet b)
{
	for(const auto& m : b.Monies)
		*this += Money{m.c,m.q};
	return *this;
}
Wallet& Wallet::operator += (const Currency b)
{
	*this += Money{b,1};
	return *this;
}
Wallet& Wallet::operator -= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator -= ([[maybe_unused]] const std::wstring b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator -= (const Money b)
{
	transaction = b;

	if(get(b.c)-b.q < 0 && b.c.System != L"" && HasEffectivelyAtLeast(b.q,b.c))
	{
		set(b.c,get(b.c)-b.q);
		MakeChange(b.c,this);
	}
	else if(get(b.c)-b.q < 0 && b.c.System != L"")//TODO: Maybe create option to allow users to go into debt?
	{
		output(Error,"Insufficient funds!");
	}
	else if(get(b.c)-b.q >= 0)
	{
		set(b.c,get(b.c)-b.q);
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
	for(const auto& m : b.Monies)
	{
		if(!HasEffectivelyAtLeast(m.q,m.c))
		{
			output(Error,"Insufficient funds!");
			return *this;
		}
	}

	for(const auto& m : b.Monies)
		*this -= Money{m.c,m.q};
	return *this;
}
Wallet& Wallet::operator -= (const Currency b)
{
	*this -= Money{b,1};
	return *this;
}
Wallet& Wallet::operator *= (const int b)
{
	for(const auto& m : Monies)
		if(b > 1) *this += Money{m.c,(m.q*(b-1))};//Avoid zeroing out wallet if user factors by 1

	return *this;
}
Wallet& Wallet::operator *= ([[maybe_unused]] const std::wstring b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator *= ([[maybe_unused]] const Money b)
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

	for(auto& m : Monies)
	{
		float quotient = ((float)m.q/b);
		unsigned int _floor = floor(quotient);

		if(_floor) change[m.c] = (quotient-_floor);
		set(m.c,_floor);
	}

	//Convert everything to the right of the decimal point to the closest whole currencies
	//Will necessarily be a bit lossy just like real finances when dealing with repeating decimals and the like
	for(const auto& [c,q] : change)
		FloatQuantityToIntCurrency(c,q);

	return *this;
}
Wallet& Wallet::operator /= ([[maybe_unused]] const std::wstring b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Wallet& Wallet::operator /= ([[maybe_unused]] const Money b)
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
Wallet& Wallet::operator ^= ([[maybe_unused]] const std::wstring b)
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
	for(auto& m : Monies) m.q %= b;

	return *this;
}
Wallet& Wallet::operator %= ([[maybe_unused]] const std::wstring b)
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
Wallet Wallet::operator + (const std::wstring b)
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
Wallet Wallet::operator - (const std::wstring b)
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
Wallet Wallet::operator * (const std::wstring b)
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
Wallet Wallet::operator / (const std::wstring b)
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
Wallet Wallet::operator ^ (const std::wstring b)
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
Wallet Wallet::operator % (const std::wstring b)
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
bool Wallet::operator == ([[maybe_unused]] const std::wstring& b) const
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
	for(const auto& m : Monies)
	{
		for(const auto& bm : b.Monies)
		{
			if(m.c == bm.c && m.q == bm.q)
			{
				containsCurrency = true;
				break;
			}
			else if(m.c == bm.c && m.q != bm.q)//Differing like-currency amounts
			{
				return false;
			}
		}
		if(!containsCurrency) return false;//*this contains currency b doesn't have
	}

	containsCurrency = false;
	for(const auto& bm : b.Monies)
	{
		for(const auto& m : Monies)
		{
			if(bm.c == m.c && bm.q == m.q)
			{
				containsCurrency = true;
				break;
			}
			else if(bm.c == m.c && bm.q != m.q)//Differing like-currency amounts
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
	Wallet w = Wallet(std::wstring(1,WALLET_SIGIL)+L"{"+std::wstring(b)+L",1}");
	return getWalletValue(*this) == getWalletValue(w);
}
bool Wallet::operator < ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Wallet::operator < ([[maybe_unused]] const std::wstring& b) const
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
	Wallet w = Wallet(std::wstring(1,WALLET_SIGIL)+L"{"+std::wstring(b)+L",1}");
	return getWalletValue(*this) < getWalletValue(w);
}
bool Wallet::operator > ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Wallet::operator > ([[maybe_unused]] const std::wstring& b) const
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
	Wallet w = Wallet(std::wstring(1,WALLET_SIGIL)+L"{"+std::wstring(b)+L",1}");
	return getWalletValue(*this) > getWalletValue(w);
}
bool Wallet::operator <= (const int& b) const
{
	return *this < b || *this == b;
}
bool Wallet::operator <= (const std::wstring& b) const
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
bool Wallet::operator >= (const std::wstring& b) const
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
bool Wallet::operator != (const std::wstring& b) const
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
bool Wallet::operator && (const std::wstring b)
{
	return bool(*this) && (b != L"" && stringcasecmp(b,L"false"));
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
bool Wallet::operator || (const std::wstring b)
{
	return bool(*this) || (b != L"" && stringcasecmp(b,L"false"));
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
	for(const auto& m : Monies)
	{
		if(m.c.System == L"" || m.c.Smaller == L"")
		{
			*this += Money{m.c,1};
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
	//Subtract 1 of the smallest currency
	for(const auto& m : Monies)
	{
		if(m.c.System == L"" || m.c.Smaller == L"")
		{
			*this -= Money{m.c,1};
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
	std::wstring NextHighest = c.Larger;
	int ConversionFactor = s[NextHighest].SmallerAmount;
	int transactionAmount = (*w).transaction.q;

	//Number of Currency[NextHighest] that will need to be converted to current currency
	int ChangeCount = (abs(((*w).get(c)))/ConversionFactor);

	//Only break an extra higher denomination currency if we really need to
	//e.g. If I owe 13 dimes, but I have 1 dollar and 5 dimes, don't try to break 2 dollars and claim I don't have enough money
	int MinimumAmountToBreak = ChangeCount*ConversionFactor;
	int PreTransactionQuantity = transactionAmount+(*w).get(c);
	if((MinimumAmountToBreak+PreTransactionQuantity) < transactionAmount)
		ChangeCount++;

	//Prevent unneccesary subtraction
	//Check if the player has to make change with the currency after that to complete transaction
	if(ChangeCount == 0)
		return;
	else if((*w).get(s[NextHighest]) < ChangeCount)
		MakeChange(s[NextHighest],w);

	//Make change by breaking the larger denomination into the smaller denomination
	(*w).set(c,((*w).get(c)+(ConversionFactor*ChangeCount)));
	(*w) -= Money{s[NextHighest],ChangeCount};
}
void TradeUp(std::wstring system, Wallet* w)
{
	datamap<Currency> s = getCurrencySystem(system);
	for(const auto& m : (*w).Monies)
	{
		if(m.c.System == system && m.c.Larger != L"" && m.q >= s[m.c.Larger].SmallerAmount)
		{
			//Make use of the trucation of the divison return value to int to simplify the math
			int ConversionFactor = s[m.c.Larger].SmallerAmount;
			int AmountTradable = ((*w).get(m.c)/ConversionFactor);
			(*w).set(s[m.c.Larger],(*w).get(s[m.c.Larger])+AmountTradable);
			(*w).set(m.c,(*w).get(m.c)-(AmountTradable*ConversionFactor));
		}
	}
}
