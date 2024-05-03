#include <cmath>//floor()
#include <strings.h>//strcasecmp()
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/text.h"
#include "../headers/var.h"

void Currency::tryParseCurrencySystem(datamap<CurrencySystem> currencysystems, std::string* str, std::string fullstr)
{
	std::string cs_match = "";
	std::string str_temp = "";

	try
	{
		cs_match = str->substr(0,str->find(","));
		str_temp = str->substr(str->find(",")+1,
				       str->length()-str->find(",")+1);
		str = &str_temp;
	}
	catch(...)
	{
		output(Error,"Unable to parse currencysystem from \"%s\".",fullstr.c_str());
		exit(-1);
	}

	if(cs_match == "")
	{
		output(Error,"CurrencySystem cannot be empty.");
		exit(-1);
	}

	for(const auto& [k,v] : currencysystems)
	{
		if(!strcasecmp(v.Name.c_str(),cs_match.c_str()))
		{
			//Can't just call function with &v for some reason
			CurrencySystem cs = v;
			AttachToCurrencySystem(&cs);
		}
	}
	if(!System)
	{
		output(Error,"No currency system matches \"%s\".",cs_match.c_str());
		exit(-1);
	}
}
void Currency::tryParseName(std::string* str, std::string fullstr)
{
	std::string str_temp = "";

	try
	{
		Name = str->substr(0,str->find(","));
		str_temp = str->substr(str->find(",")+1,
				       str->length()-str->find(",")+1);
		str = &str_temp;
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
	std::string str_temp = "";

	try
	{
		SmallerAmountStr = str->substr(0,str->find(","));
		str_temp = str->substr(str->find(",")+1,
				       str->length()-str->find(",")+1);
		str = &str_temp;
	}
	catch(...)
	{
		output(Error,"Unable to parse currency SmallerAmount from \"%s\".",fullstr.c_str());
		exit(-1);
	}

	try
	{
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
	std::string str_temp = "";

	try
	{
		Smaller = str->substr(0,str->find(","));
		str_temp = str->substr(str->find(",")+1,
				       str->length()-str->find(",")+1);
		str = &str_temp;
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
		output(Error,"Invalid argument count in currency explicit constructor. There should be exactly five arguments.");
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
Currency::Currency(CurrencySystem* _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
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
	{
		return "{" + System->Name + "," +
			     Name + "," +
			     std::to_string(SmallerAmount) + "," +
			     Smaller + "," +
			     Larger + "}";
	}
	else
	{
		return "{" + Name + "," +
			     std::to_string(SmallerAmount) + "," +
			     Smaller + "," +
			     Larger + "}";
	}
}
bool Currency::operator == (const Currency& b) const
{
	return (System == b.System &&
		Name == b.Name &&
		Smaller == b.Smaller &&
		SmallerAmount == b.SmallerAmount &&
		Larger == b.Larger);
}
//TODO Currency < operator
CurrencySystem::operator std::string() const
{
	return Name;
}
Currency& CurrencySystem::operator [] (const std::string b)
{
	return Denomination[b];
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
	Name = str;
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
	//@w/MyWallet = {<currency_1> <quantity_1>,<currency_2> <quantity_2>,...,<currency_n> <quantity_n>}

	datamap<Currency> currencies = getDatamapFromAllScopes<Currency>(WALLET_SIGIL);

	//Get map of currency names
	datamap<std::string> currencynames;
	for(auto& [k,v] : currencies)
		currencynames[k] = v.Name;

	if(str[0] != '{')
	{
		output(Error,"Expected starting \'{\' at beginning of wallet definition.");
		exit(-1);
	}

	str = str.substr(1,str.length()-1);

	//Get currency amounts
	while(true)
	{
		std::string cur_str = str.substr(0,str.find(" "));
		char delimiter = ',';

		if(str.find(",") == std::string::npos)
			delimiter = '}';

		if(str.find(delimiter) == std::string::npos)
		{
			output(Error,"Unable to parse currency for wallet, missing \'%c\'",delimiter);
			exit(-1);
		}

		std::string quant_str = str.substr(str.find(" ")+1,
						   str.find(delimiter)-(str.find(" ")+1));

		try
		{
			Money[currencynames[cur_str]] += std::stoi(quant_str);
		}
		catch(...)
		{
			output(Error,"Unable to add the currency \"%s\" in the quantity \"%s\" to the wallet.",cur_str.c_str(),quant_str.c_str());
		}
	}
}

Wallet& Wallet::operator = (const unsigned int b)
{
	printBadOpAndThrow("Wallet = "+std::to_string(b));
	return *this;
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
Wallet& Wallet::operator = (const Dice b)
{
	printBadOpAndThrow("Wallet = "+std::string(b));
	return *this;
}
Wallet& Wallet::operator = (const Var b)
{
	printBadOpAndThrow("Wallet = "+std::string(b));
	return *this;
}
Wallet& Wallet::operator -= (const unsigned int b)
{
	printBadOpAndThrow("Wallet -= "+std::to_string(b));
	return *this;
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
	{
		*this -= money_t(c,q);
	}
	return *this;
}
Wallet& Wallet::operator -= (const Dice b)
{
	printBadOpAndThrow("Wallet -= "+std::string(b));
	return *this;
}
Wallet& Wallet::operator -= (const Var b)
{
	printBadOpAndThrow("Wallet -= "+std::string(b));
	return *this;
}
Wallet& Wallet::operator += (const unsigned int b)
{
	printBadOpAndThrow("Wallet += "+std::to_string(b));
	return *this;
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
Wallet& Wallet::operator += (const Wallet b)
{
	for(const auto& [c,q] : b)
	{
		*this += money_t(c,q);
	}
	return *this;
}
Wallet& Wallet::operator += (const Dice b)
{
	printBadOpAndThrow("Wallet += "+std::string(b));
	return *this;
}
Wallet& Wallet::operator += (const Var b)
{
	printBadOpAndThrow("Wallet += "+std::string(b));
	return *this;
}
Wallet& Wallet::operator *= (const unsigned int b)
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
Wallet& Wallet::operator *= (const money_t b)
{
	std::string c = b.first.Name;
	std::string q = std::to_string(b.second);
	printBadOpAndThrow("Wallet *= "+q+" "+c);
	return *this;
}
Wallet& Wallet::operator *= (const Wallet b)
{
	printBadOpAndThrow("Wallet *= Wallet");
	return *this;
}
Wallet& Wallet::operator *= (const Dice b)
{
	printBadOpAndThrow("Wallet *= "+std::string(b));
	return *this;
}
Wallet& Wallet::operator *= (const Var b)
{
	printBadOpAndThrow("Wallet *= "+std::string(b));
	return *this;
}
Wallet& Wallet::operator /= (const unsigned int b)
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
Wallet& Wallet::operator /= (const money_t b)
{
	std::string c = b.first.Name;
	std::string q = std::to_string(b.second);
	printBadOpAndThrow("Wallet /= "+q+" "+c);
	return *this;
}
Wallet& Wallet::operator /= (const Wallet b)
{
	printBadOpAndThrow("Wallet /= Wallet");
	return *this;
}
Wallet& Wallet::operator /= (const Dice b)
{
	printBadOpAndThrow("Wallet /= "+std::string(b));
	return *this;
}
Wallet& Wallet::operator /= (const Var b)
{
	printBadOpAndThrow("Wallet /= "+std::string(b));
	return *this;
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

Wallet::operator std::string() const
{
	//@w/MyWallet = {<currency_1> <quantity_1>,<currency_2> <quantity_2>,...,<currency_n> <quantity_n>}

	std::string ret = "{";
	for(const auto& [k,v] : Money)
		ret += k.Name + " " + std::to_string(v) + ",";

	//Cut final ','
	ret = ret.substr(0,ret.length()-1);

	ret += "}";

	return ret;
}
bool Wallet::operator != (const std::string b)//TODO: May need to revisit this for a possible Wallet(std::string) constructor
{
	return true;
}
//Links the instance of Currency to the instance of CurrencySystem
//Allows the instance of CurrencySystem to detect changes in the attached RGPSH_CURRENCY
//Also automatically adds the Currency to the map of Currencies within the CurrencySystem
void Currency::AttachToCurrencySystem(CurrencySystem* _CurrencySystem)
{
	System = _CurrencySystem;
	System->Denomination[Name] = *this;
}

//Required for compilation due to the use of Currency as a key for an std::map in the Wallet class
//Orders std::map from highest to lowest denomination
//If you use more than one Currency, you will need to declare CurrencySystem for each of them
//Even if they are the only Currency in the system. Otherwise, operators don't give expected behavior
bool Currency::operator < (const Currency& b) const
{
	if(!System && b.System || !b.System && System)
	{
		return false;
	}
	if(!System && !b.System)
	{
		if(b.Larger == Name)
		{
			return true;
		}
		else if(b.Smaller == Name)
		{
			return false;
		}
		else if(b.Name == Name)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	for(const auto& [key,value] : *System)
	{
		if(System != b.System)
		{
			//Needs to be here, otherwise Wallet.print() doesn't work right
			//and operations try doing cross-system exchanges that don't work.
			//Probably due to some under-the-hood nonsense with std::map
			continue;
		}
		else if(b.Larger == Name)
		{
			return true;
		}
		else if(b.Smaller == Name)
		{
			return false;
		}
		else if(b.Name == Name)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return (Name < b.Name);
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
void CurrencySystem::TradeUp(CurrencySystem* S, Wallet* w)
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
