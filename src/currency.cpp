#include <cmath>//floor()
#include <strings.h>//strcasecmp()
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/text.h"
#include "../headers/var.h"

void currency_t::tryParseCurrencySystem(datamap<currencysystem_t> currencysystems, std::string* str, std::string fullstr)
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

	for(const auto& [k,v] : currencysystems)
	{
		if(!strcasecmp(v.Name.c_str(),cs_match.c_str()))
		{
			//Can't just call function with &v for some reason
			currencysystem_t cs = v;
			AttachToCurrencySystem(&cs);
		}
	}
	if(!System)
	{
		output(Error,"No currency system matches \"%s\".",cs_match.c_str());
		exit(-1);
	}
}
void currency_t::tryParseName(std::string* str, std::string fullstr)
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
}
void currency_t::tryParseSmallerAmount(std::string* str, std::string fullstr)
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
void currency_t::tryParseSmaller(std::string* str, std::string fullstr)
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
void currency_t::tryParseLarger(std::string* str, std::string fullstr)
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

currency_t::currency_t(){}
currency_t::currency_t(std::string str)
{
	//FORMAT
	//@c/MyCurrency = {Name,SmallerAmount,Smaller,Larger}
	//@c/MyCurrency = {CurrencySystem,Name,SmallerAmount,Smaller,Larger}

	datamap<currencysystem_t> currencysystems = getDatamapFromAllScopes<currencysystem_t>(CURRENCYSYSTEM_SIGIL);
	std::string fullstr = str;

	//Get number of commas to determine if we are involving a currencysystem_t
	int commas = 0;
	for(const auto& c : str)
	{
		if(c == ',')
			commas++;
	}

	if(commas < 3)
	{
		output(Error,"Too few arguments in currency definition.");
		exit(-1);
	}
	else if(commas > 4)
	{
		output(Error,"Too many arguments in currency definition.");
		exit(-1);
	}

	//Make sure starting character is '{'
	if(str[0] != '{')
	{
		output(Error,"Expected starting \'{\' at beginning of currency definition.");
		exit(-1);
	}

	//Nix the first character to make future substrings more intuitive
	str = str.substr(1,str.length()-1);

	//Name
	//CurrencySystem
	if(commas == 3)
		tryParseName(&str, fullstr);
	else
		tryParseCurrencySystem(currencysystems, &str, fullstr);

	//SmallerAmount
	//Name
	if(commas == 3)
		tryParseSmallerAmount(&str, fullstr);
	else
		tryParseName(&str, fullstr);

	//Smaller
	//SmallerAmount
	if(commas == 3)
		tryParseSmaller(&str, fullstr);
	else
		tryParseSmallerAmount(&str, fullstr);

	//Larger
	//Smaller
	if(commas == 3)
		tryParseLarger(&str, fullstr);
	else
		tryParseSmaller(&str, fullstr);

	//<N/A>
	//Larger
	if(commas == 4)
		tryParseLarger(&str, fullstr);
}
currency_t::currency_t(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}
currency_t::currency_t(currencysystem_t* _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;

	AttachToCurrencySystem(_CS);
}

bool currency_t::operator == (const currency_t& b) const
{
	return (System == b.System &&
		Name == b.Name &&
		Smaller == b.Smaller &&
		SmallerAmount == b.SmallerAmount &&
		Larger == b.Larger);
}

currency_t& currencysystem_t::operator [] (const std::string b)
{
	return Denomination[b];
}

//Beginning and end iterators.
//So I can use "for(const auto& [key,val] : currencysystem_t){}"
std::map<std::string, currency_t>::const_iterator currencysystem_t::begin() const
{
	return Denomination.begin();
}
std::map<std::string, currency_t>::const_iterator currencysystem_t::end() const
{
	return Denomination.end();
}

currencysystem_t::currencysystem_t(){}
currencysystem_t::currencysystem_t(std::string str)
{
	Name = str;
}

bool wallet_t::HasEffectivelyAtLeast(int q, currency_t c)
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
void wallet_t::print(int tab)
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
void wallet_t::FloatQuantityToIntCurrency(currency_t c, float q)
{
	int totalFactor = 1;

	while(c.Smaller != "")
	{
		totalFactor *= c.SmallerAmount;
		c = c.System->Denomination[c.Smaller];
	}

	*this += money_t(c,(int)(q*totalFactor));
}

int& wallet_t::operator [] (const currency_t b)
{
	return Money[b];
}

//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : wallet_t){}"
std::map<currency_t, int>::const_iterator wallet_t::begin() const
{
	return Money.begin();
}
std::map<currency_t, int>::const_iterator wallet_t::end() const
{
	return Money.end();
}

wallet_t::wallet_t(){}
wallet_t::wallet_t(const money_t m)
{
	currency_t c = m.first;
	int q = m.second;

	Money[c] = q;
}
wallet_t::wallet_t(std::string str)
{
	//FORMAT
	//@w/MyWallet = {<currency_1> <quantity_1>,<currency_2> <quantity_2>,...,<currency_n> <quantity_n>}

	datamap<currency_t> currencies = getDatamapFromAllScopes<currency_t>(WALLET_SIGIL);

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

wallet_t& wallet_t::operator = (const unsigned int b)
{
	printBadOpAndThrow("Wallet = "+std::to_string(b));
	return *this;
}
wallet_t& wallet_t::operator = (const money_t b)
{
	for(const auto& [c,q] : Money)
	{
		Money[c] = 0;
	}
	Money[b.first] = b.second;
	return *this;
}
wallet_t& wallet_t::operator = (const dice_t b)
{
	printBadOpAndThrow("Wallet = "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator = (const var_t b)
{
	printBadOpAndThrow("Wallet = "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator -= (const unsigned int b)
{
	printBadOpAndThrow("Wallet -= "+std::to_string(b));
	return *this;
}
wallet_t& wallet_t::operator -= (const money_t b)
{
	currency_t c = b.first;
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
wallet_t& wallet_t::operator -= (const wallet_t b)
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
wallet_t& wallet_t::operator -= (const dice_t b)
{
	printBadOpAndThrow("Wallet -= "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator -= (const var_t b)
{
	printBadOpAndThrow("Wallet -= "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator += (const unsigned int b)
{
	printBadOpAndThrow("Wallet += "+std::to_string(b));
	return *this;
}
wallet_t& wallet_t::operator += (const money_t b)
{
	currency_t c = b.first;
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
wallet_t& wallet_t::operator += (const wallet_t b)
{
	for(const auto& [c,q] : b)
	{
		*this += money_t(c,q);
	}
	return *this;
}
wallet_t& wallet_t::operator += (const dice_t b)
{
	printBadOpAndThrow("Wallet += "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator += (const var_t b)
{
	printBadOpAndThrow("Wallet += "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator *= (const unsigned int b)
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
wallet_t& wallet_t::operator *= (const money_t b)
{
	std::string c = b.first.Name;
	std::string q = std::to_string(b.second);
	printBadOpAndThrow("Wallet *= "+q+" "+c);
	return *this;
}
wallet_t& wallet_t::operator *= (const wallet_t b)
{
	printBadOpAndThrow("Wallet *= Wallet");
	return *this;
}
wallet_t& wallet_t::operator *= (const dice_t b)
{
	printBadOpAndThrow("Wallet *= "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator *= (const var_t b)
{
	printBadOpAndThrow("Wallet *= "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator /= (const unsigned int b)
{
	output(Info,"Decreasing wallet value by a factor of %d",b);
	if(b == 0)
	{
		output(Error,"Attempted to divide wallet value by 0!\n");
		return *this;
	}

	std::map<currency_t,float> change;

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
wallet_t& wallet_t::operator /= (const money_t b)
{
	std::string c = b.first.Name;
	std::string q = std::to_string(b.second);
	printBadOpAndThrow("Wallet /= "+q+" "+c);
	return *this;
}
wallet_t& wallet_t::operator /= (const wallet_t b)
{
	printBadOpAndThrow("Wallet /= Wallet");
	return *this;
}
wallet_t& wallet_t::operator /= (const dice_t b)
{
	printBadOpAndThrow("Wallet /= "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator /= (const var_t b)
{
	printBadOpAndThrow("Wallet /= "+std::string(b));
	return *this;
}
wallet_t& wallet_t::operator ++ ()
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
wallet_t& wallet_t::operator ++ (int)
{
	return ++(*this);
}
wallet_t& wallet_t::operator -- ()
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
wallet_t& wallet_t::operator -- (int)
{
	return --(*this);
}

bool wallet_t::operator != (const std::string b)//TODO: May need to revisit this for a possible wallet_t(std::string) constructor
{
	return true;
}
//Links the instance of currency_t to the instance of currencysystem_t
//Allows the instance of currencysystem_t to detect changes in the attached RGPSH_CURRENCY
//Also automatically adds the Currency to the map of Currencies within the CurrencySystem
void currency_t::AttachToCurrencySystem(currencysystem_t* _CurrencySystem)
{
	System = _CurrencySystem;
	System->Denomination[Name] = *this;
}

//Required for compilation due to the use of currency_t as a key for an std::map in the wallet_t class
//Orders std::map from highest to lowest denomination
//If you use more than one currency_t, you will need to declare currencysystem_t for each of them
//Even if they are the only currency_t in the system. Otherwise, operators don't give expected behavior
bool currency_t::operator < (const currency_t& b) const
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
			//Needs to be here, otherwise wallet_t.print() doesn't work right
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

void currencysystem_t::MakeChange(currency_t c, wallet_t* w)
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
void currencysystem_t::TradeUp(currencysystem_t* S, wallet_t* w)
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
