#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include <stdarg.h>
#include "headers/output.h"

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
typedef std::pair<RPGSH_CURRENCY, int> money;
class RPGSH_WALLET
{
	public:
		std::map<RPGSH_CURRENCY, int> Money;
		std::pair<RPGSH_CURRENCY,int> transaction;

	bool HasEffectivelyAtLeast(int q, RPGSH_CURRENCY _d)
	{
		RPGSH_CURRENCY d = _d;

		int total = Money[d];
		int totalFactor = 1;

		while(d.Larger != "")
		{
			d = d.System->Denomination[d.Larger];

			totalFactor *= d.SmallerAmount;
			total += Money[d] * totalFactor;
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

	RPGSH_WALLET& operator -= (const money m)
	{
		RPGSH_CURRENCY d = m.first;
		int q = m.second;

		output(Info,"Attempting to debit %d %s",q,d.Name.c_str());
		transaction = m;

		if(Money[d]-q < 0 && d.System && HasEffectivelyAtLeast(q,d))
		{
			Money[d] -= q;
			d.System->MakeChange(d, this);
		}
		else if(Money[d]-q < 0 && d.System)
		{
			output(Error,"Insufficient funds!");
		}
		else if(Money[d]-q >= 0)
		{
			Money[d] -= q;
		}
		return *this;
	}
	RPGSH_WALLET& operator += (const money m)
	{
		RPGSH_CURRENCY d = m.first;
		int q = m.second;

		output(Info,"Crediting %d %s",q,d.Name.c_str());
		transaction = m;

		Money[d] += q;

		if(d.System && (d.System->Denomination[d.Larger].SmallerAmount < Money[d]))
		{
			d.System->TradeUp(d.System,this);
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
	(*w) -= money(Denomination[NextHighest],ChangeCount);
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
		q > S->Denomination[c.Larger].SmallerAmount)
		{
			//Make use of the trucation of the divison return value to int to simplify the math
			int ConversionFactor = S->Denomination[c.Larger].SmallerAmount;
			int AmountTradable = ((*w)[c] / ConversionFactor);
			(*w)[S->Denomination[c.Larger]] += AmountTradable;
			(*w)[c] -= (AmountTradable * ConversionFactor);
		}
	}
}

int main()
{
	#define PLATINUM	"Platinum"
	#define GOLD		"Gold"
	#define SILVER		"Silver"
	#define COPPER		"Copper"

	#define DOLLAR		"Dollar"
	#define DIME		"Dime"
	#define NICKEL		"Nickel"
	#define PENNY		"Penny"

	RPGSH_CURRENCYSYSTEM DND;
	RPGSH_CURRENCY Platinum	= RPGSH_CURRENCY(&DND, PLATINUM,10, GOLD,	"");
	RPGSH_CURRENCY Gold	= RPGSH_CURRENCY(&DND, GOLD,	10, SILVER,	PLATINUM);
	RPGSH_CURRENCY Silver	= RPGSH_CURRENCY(&DND, SILVER,	10, COPPER,	GOLD);
	RPGSH_CURRENCY Copper	= RPGSH_CURRENCY(&DND, COPPER,	0,  "",		SILVER);

	RPGSH_CURRENCYSYSTEM US;
	RPGSH_CURRENCY Dollar	= RPGSH_CURRENCY(&US, DOLLAR,	10, DIME,	"");
	RPGSH_CURRENCY Dime	= RPGSH_CURRENCY(&US, DIME,	2,  NICKEL,	DOLLAR);
	RPGSH_CURRENCY Nickel	= RPGSH_CURRENCY(&US, NICKEL,	5,  PENNY,	DIME);
	RPGSH_CURRENCY Penny	= RPGSH_CURRENCY(&US, PENNY,	0,  "",		NICKEL);

	RPGSH_WALLET W;
	W[Platinum] =	10;
	W[Gold] =	10;
	W[Silver] =	10;
	W[Copper] =	10;

	W[Dollar] =	10;
	W[Dime] =	10;
	W[Nickel] =	10;
	W[Penny] =	10;

	fprintf(stdout,"Starting amount\n");
	W.print();
	fprintf(stdout,"\n");

	W -= money(Gold,10);
	W.print();
	fprintf(stdout,"\n");

	W -= money(Silver,25);
	W.print();
	fprintf(stdout,"\n");

	W -= money(Copper,1223);
	W.print();
	fprintf(stdout,"\n");

	W -= money(Copper,9999);
	W.print();
	fprintf(stdout,"\n");

	W -= money(Copper,8637);
	W.print();
	fprintf(stdout,"\n");

	W += money(Copper,1572);
	W.print();
	fprintf(stdout,"\n");

	W -= money(Penny,420);
	W.print();
	fprintf(stdout,"\n");

	W += money(Penny,397);
	W.print();
	fprintf(stdout,"\n");

	return 0;
}