#pragma once

#include <map>
#include <string>
#include <stdarg.h>
#include <cmath>//floor()
#include "output.h"

class RPGSH_CURRENCYSYSTEM;
class RPGSH_WALLET;

class RPGSH_CURRENCY
{
	public:
		RPGSH_CURRENCYSYSTEM* System = nullptr;
		std::string Name = "";
		int SmallerAmount = 0;
		std::string Smaller = "";
		std::string Larger = "";

	void AttachToCurrencySystem(RPGSH_CURRENCYSYSTEM* _CurrencySystem);

	RPGSH_CURRENCY(){}
	RPGSH_CURRENCY(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
	{
		Name = _Name;
		Smaller = _Smaller;
		SmallerAmount = _SmallerAmount;
		Larger = _Larger;
	}
	RPGSH_CURRENCY(RPGSH_CURRENCYSYSTEM* _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger)
	{
		Name = _Name;
		Smaller = _Smaller;
		SmallerAmount = _SmallerAmount;
		Larger = _Larger;

		AttachToCurrencySystem(_CS);
	}

	RPGSH_CURRENCY& operator -= (const int b);
	bool operator < (const RPGSH_CURRENCY& b) const;
	bool operator == (const RPGSH_CURRENCY& b) const;
};
class RPGSH_CURRENCYSYSTEM
{
	public:
		std::map<std::string, RPGSH_CURRENCY> Denomination;
		RPGSH_CURRENCY transaction = RPGSH_CURRENCY();

	RPGSH_CURRENCY& operator [] (const std::string b)
	{
		return Denomination[b];
	}

	//Iterator type for the class
	using iterator = typename std::map<std::string, RPGSH_CURRENCY>::const_iterator;

	//Beginning and end iterators.
	//So I can use "for(const auto& [key,val] : RPGSH_CURRENCYSYSTEM){}"
	iterator begin() const
	{
		return Denomination.begin();
	}
	iterator end() const
	{
		return Denomination.end();
	}

	RPGSH_CURRENCYSYSTEM(){}

	void MakeChange(RPGSH_CURRENCY c, RPGSH_WALLET* w);
	void TradeUp(RPGSH_CURRENCYSYSTEM* S, RPGSH_WALLET* w);
	bool HasEquivalentTo(int Quantity, std::string Denomination);
};
typedef std::pair<RPGSH_CURRENCY, int> money_t;
class RPGSH_WALLET
{
	public:
		std::map<RPGSH_CURRENCY, int> Money;
		std::pair<RPGSH_CURRENCY,int> transaction;

	bool HasEffectivelyAtLeast(int q, RPGSH_CURRENCY c)
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
	void print()
	{
		#define PAD_OFFSET 5

		unsigned int NameLength = 0;
		unsigned int LongestName = 0;

		//Get longest currency name as reference for spacing
		for(const auto& [key,value] : *this)
		{
			NameLength = key.Name.length();
			if(NameLength > LongestName)
			{
				LongestName = NameLength;
			}
		}

		//Print formatted list of currencies and the currency values
		for(const auto& [key,value] : *this)
		{
			if(value > 0)
			{
				unsigned int QuantityLength = std::to_string(value).length();
				unsigned int PadLength = (LongestName - key.Name.length()) +
							 QuantityLength +
							 PAD_OFFSET;
				fprintf(stdout,"%s%s%*d%s\n",key.Name.c_str(),TEXT_WHITE,PadLength,value,TEXT_NORMAL);
			}
		}
	}
	void FloatQuantityToWholeCurrency(RPGSH_CURRENCY c, float q)
	{
		int totalFactor = 1;

		while(c.Smaller != "")
		{
			totalFactor *= c.SmallerAmount;
			c = c.System->Denomination[c.Smaller];
		}

		*this += money_t(c,(int)(q*totalFactor));
	}

	int& operator [] (const RPGSH_CURRENCY b)
	{
		return Money[b];
	}

	//Iterator for the class
	using iterator = typename std::map<RPGSH_CURRENCY, int>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_WALLET){}"
	iterator begin() const
	{
		return Money.begin();
	}
	iterator end() const
	{
		return Money.end();
	}

	RPGSH_WALLET& operator -= (const money_t m)
	{
		RPGSH_CURRENCY c = m.first;
		int q = m.second;

		output(Info,"Attempting to debit %d %s",q,c.Name.c_str());
		transaction = m;

		if(Money[c]-q < 0 && c.System && HasEffectivelyAtLeast(q,c))
		{
			Money[c] -= q;
			c.System->MakeChange(c, this);
		}
		else if(Money[c]-q < 0 && c.System)
		{
			output(Error,"Insufficient funds!");
		}
		else if(Money[c]-q >= 0)
		{
			Money[c] -= q;
		}
		return *this;
	}
	RPGSH_WALLET& operator -= (const RPGSH_WALLET w)
	{
		output(Info,"Attempting to debit the following:");
		RPGSH_WALLET _w = w;
		_w.print();

		for(const auto& [c,q] : w)
		{
			if(!HasEffectivelyAtLeast(q,c))
			{
				output(Error,"Insufficient funds!");
				return *this;
			}
		}

		for(const auto& [c,q] : w)
		{
			*this -= money_t(c,q);
		}
		return *this;
	}
	RPGSH_WALLET& operator += (const money_t m)
	{
		RPGSH_CURRENCY c = m.first;
		int q = m.second;

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
	RPGSH_WALLET& operator += (const RPGSH_WALLET w)
	{
		for(const auto& [c,q] : w)
		{
			*this += money_t(c,q);
		}
		return *this;
	}
	RPGSH_WALLET& operator *= (const unsigned int f)
	{
		output(Info,"Increasing wallet value by a factor of %d",f);
		for(const auto& [c,q] : *this)
		{
			//Keep from printing pointless "Crediting..." messages
			//Avoid zeroing out wallet if you factor by 1
			if(q > 0 && f > 1)
			{
				*this += money_t(c,(q*(f-1)));
			}
		}
		return *this;
	}
	RPGSH_WALLET& operator /= (const unsigned int f)
	{
		output(Info,"Decreasing wallet value by a factor of %d",f);
		if(f == 0)
		{
			output(Error,"Attempted to divide wallet value by 0!\n");
			return *this;
		}

		std::map<RPGSH_CURRENCY,float> change;

		for(const auto& [c,q] : *this)
		{
			float quotient = ((float)q/f);
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
};

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
bool RPGSH_CURRENCY::operator < (const RPGSH_CURRENCY& b) const
{
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
bool RPGSH_CURRENCY::operator == (const RPGSH_CURRENCY& b) const
{
	return  System == b.System &&
		Name == b.Name &&
		Smaller == b.Smaller &&
		SmallerAmount == b.SmallerAmount &&
		Larger == b.Larger;
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
