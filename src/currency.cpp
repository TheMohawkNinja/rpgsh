#include <cmath>//floor()
#include "../headers/currency.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/text.h"
#include "../headers/var.h"

RPGSH_CURRENCY::RPGSH_CURRENCY(){}
RPGSH_CURRENCY::RPGSH_CURRENCY(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;
}
RPGSH_CURRENCY::RPGSH_CURRENCY(RPGSH_CURRENCYSYSTEM* _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
{
	Name = _Name;
	Smaller = _Smaller;
	SmallerAmount = _SmallerAmount;
	Larger = _Larger;

	AttachToCurrencySystem(_CS);
}

bool RPGSH_CURRENCY::operator == (const RPGSH_CURRENCY& b) const
{
	return (System == b.System &&
		Name == b.Name &&
		Smaller == b.Smaller &&
		SmallerAmount == b.SmallerAmount &&
		Larger == Larger);
}

RPGSH_CURRENCY& RPGSH_CURRENCYSYSTEM::operator [] (const std::string b)
{
	return Denomination[b];
}

//Beginning and end iterators.
//So I can use "for(const auto& [key,val] : RPGSH_CURRENCYSYSTEM){}"
std::map<std::string, RPGSH_CURRENCY>::const_iterator RPGSH_CURRENCYSYSTEM::begin() const
{
	return Denomination.begin();
}
std::map<std::string, RPGSH_CURRENCY>::const_iterator RPGSH_CURRENCYSYSTEM::end() const
{
	return Denomination.end();
}

RPGSH_CURRENCYSYSTEM::RPGSH_CURRENCYSYSTEM(){}

bool RPGSH_WALLET::HasEffectivelyAtLeast(int q, RPGSH_CURRENCY c)
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
void RPGSH_WALLET::print(int tab)
{
	#define PAD_OFFSET 5

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
						 PAD_OFFSET;
			//tab_over(tab);
			fprintf(stdout,"\t%s%s%*d%s\n",key.Name.c_str(),TEXT_WHITE,PadLength,value,TEXT_NORMAL);
		}
	}
}
void RPGSH_WALLET::FloatQuantityToWholeCurrency(RPGSH_CURRENCY c, float q)
{
	int totalFactor = 1;

	while(c.Smaller != "")
	{
		totalFactor *= c.SmallerAmount;
		c = c.System->Denomination[c.Smaller];
	}

	*this += money_t(c,(int)(q*totalFactor));
}

int& RPGSH_WALLET::operator [] (const RPGSH_CURRENCY b)
{
	return Money[b];
}

//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_WALLET){}"
std::map<RPGSH_CURRENCY, int>::const_iterator RPGSH_WALLET::begin() const
{
	return Money.begin();
}
std::map<RPGSH_CURRENCY, int>::const_iterator RPGSH_WALLET::end() const
{
	return Money.end();
}

RPGSH_WALLET::RPGSH_WALLET(){}
RPGSH_WALLET::RPGSH_WALLET(const money_t m)
{
	RPGSH_CURRENCY c = m.first;
	int q = m.second;

	Money[c] = q;
}
RPGSH_WALLET::RPGSH_WALLET(std::string s)//TODO: Assumes a format "<quantity> <currency>"
{
	int s_space = s.find(" ");
	int s_ln = s.length();
	money_t m;
	try
	{
		std::string q = s.substr(0,s_space);
		m.second = std::stoi(q);
	}
	catch(...)
	{
		output(Error,"Invalid quantity in wallet constructor \'%s\'",s.c_str());
	}

	try
	{
		std::string c = s.substr(s_space+1,s_ln-s_space);
		m.first = RPGSH_CURRENCY();
	}
	catch(...)
	{
		output(Error,"Invalid currency in wallet constructor \'%s\'",s.c_str());
	}
}

RPGSH_WALLET& RPGSH_WALLET::operator = (const unsigned int b)
{
	printBadOpAndThrow("Wallet = "+std::to_string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator = (const money_t b)
{
	for(const auto& [c,q] : Money)
	{
		Money[c] = 0;
	}
	Money[b.first] = b.second;
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator = (const RPGSH_DICE b)
{
	printBadOpAndThrow("Wallet = "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator = (const RPGSH_VAR b)
{
	printBadOpAndThrow("Wallet = "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator -= (const unsigned int b)
{
	printBadOpAndThrow("Wallet -= "+std::to_string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator -= (const money_t b)
{
	RPGSH_CURRENCY c = b.first;
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
RPGSH_WALLET& RPGSH_WALLET::operator -= (const RPGSH_WALLET b)
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
RPGSH_WALLET& RPGSH_WALLET::operator -= (const RPGSH_DICE b)
{
	printBadOpAndThrow("Wallet -= "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator -= (const RPGSH_VAR b)
{
	printBadOpAndThrow("Wallet -= "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator += (const unsigned int b)
{
	printBadOpAndThrow("Wallet += "+std::to_string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator += (const money_t b)
{
	RPGSH_CURRENCY c = b.first;
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
RPGSH_WALLET& RPGSH_WALLET::operator += (const RPGSH_WALLET b)
{
	for(const auto& [c,q] : b)
	{
		*this += money_t(c,q);
	}
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator += (const RPGSH_DICE b)
{
	printBadOpAndThrow("Wallet += "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator += (const RPGSH_VAR b)
{
	printBadOpAndThrow("Wallet += "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator *= (const unsigned int b)
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
RPGSH_WALLET& RPGSH_WALLET::operator *= (const money_t b)
{
	std::string c = b.first.Name;
	std::string q = std::to_string(b.second);
	printBadOpAndThrow("Wallet *= "+q+" "+c);
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator *= (const RPGSH_WALLET b)
{
	printBadOpAndThrow("Wallet *= Wallet");
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator *= (const RPGSH_DICE b)
{
	printBadOpAndThrow("Wallet *= "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator *= (const RPGSH_VAR b)
{
	printBadOpAndThrow("Wallet *= "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator /= (const unsigned int b)
{
	output(Info,"Decreasing wallet value by a factor of %d",b);
	if(b == 0)
	{
		output(Error,"Attempted to divide wallet value by 0!\n");
		return *this;
	}

	std::map<RPGSH_CURRENCY,float> change;

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
		FloatQuantityToWholeCurrency(c,q);
	}
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator /= (const money_t b)
{
	std::string c = b.first.Name;
	std::string q = std::to_string(b.second);
	printBadOpAndThrow("Wallet /= "+q+" "+c);
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator /= (const RPGSH_WALLET b)
{
	printBadOpAndThrow("Wallet /= Wallet");
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator /= (const RPGSH_DICE b)
{
	printBadOpAndThrow("Wallet /= "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator /= (const RPGSH_VAR b)
{
	printBadOpAndThrow("Wallet /= "+std::string(b));
	return *this;
}
RPGSH_WALLET& RPGSH_WALLET::operator ++ ()
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
RPGSH_WALLET& RPGSH_WALLET::operator ++ (int)
{
	return ++(*this);
}
RPGSH_WALLET& RPGSH_WALLET::operator -- ()
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
RPGSH_WALLET& RPGSH_WALLET::operator -- (int)
{
	return --(*this);
}

bool RPGSH_WALLET::operator != (const std::string b)//TODO: May need to revisit this for a possible RPGSH_WALLET(std::string) constructor
{
	return true;
}
//Links the instance of RPGSH_CURRENCY to the instance of RPGSH_CURRENCYSYSTEM
//Allows the instance of RPGSH_CURRENCYSYSTEM to detect changes in the attached RGPSH_CURRENCY
//Also automatically adds the Currency to the map of Currencies within the CurrencySystem
void RPGSH_CURRENCY::AttachToCurrencySystem(RPGSH_CURRENCYSYSTEM* _CurrencySystem)
{
	System = _CurrencySystem;
	System->Denomination[Name] = *this;
}

//Required for compilation due to the use of RPGSH_CURRENCY as a key for an std::map in the RPGSH_WALLET class
//Orders std::map from highest to lowest denomination
//If you use more than one RPGSH_CURRENCY, you will need to declare RPGSH_CURRENCYSYSTEM for each of them
//Even if they are the only RPGSH_CURRENCY in the system. Otherwise, operators don't give expected behavior
bool RPGSH_CURRENCY::operator < (const RPGSH_CURRENCY& b) const
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
			//Needs to be here, otherwise RPGSH_WALLET.print() doesn't work right
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

void RPGSH_CURRENCYSYSTEM::MakeChange(RPGSH_CURRENCY c, RPGSH_WALLET* w)
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
void RPGSH_CURRENCYSYSTEM::TradeUp(RPGSH_CURRENCYSYSTEM* S, RPGSH_WALLET* w)
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
