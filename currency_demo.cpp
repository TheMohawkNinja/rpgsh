#include <stdio.h>
#include <map>
#include <string>

class RPGSH_CURRENCYSYSTEM;

class RPGSH_CURRENCY
{
	public:
		RPGSH_CURRENCYSYSTEM* System = nullptr;
		std::string Name = "";
		int Quantity = 0;
		int SmallerAmount = 0;
		std::string Smaller = "";
		std::string Larger = "";

	void AttachToCurrencySystem(RPGSH_CURRENCYSYSTEM* _CurrencySystem);

	RPGSH_CURRENCY(){}
	RPGSH_CURRENCY(std::string _Name, int _Quantity, int _SmallerAmount, std::string _Smaller, std::string _Larger)
	{
		Name = _Name;
		Quantity = _Quantity;
		Smaller = _Smaller;
		SmallerAmount = _SmallerAmount;
		Larger = _Larger;
	}
	RPGSH_CURRENCY(RPGSH_CURRENCYSYSTEM* _CS, std::string _Name, int _Quantity, int _SmallerAmount, std::string _Smaller, std::string _Larger)
	{
		Name = _Name;
		Quantity = _Quantity;
		Smaller = _Smaller;
		SmallerAmount = _SmallerAmount;
		Larger = _Larger;

		AttachToCurrencySystem(_CS);
	}

	RPGSH_CURRENCY& operator -= (const int b);
	bool operator < (const RPGSH_CURRENCY& b) const;
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

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_CURRENCYSYSTEM){}"
	iterator begin() const
	{
		return Denomination.begin();
	}
	iterator end() const
	{
		return Denomination.end();
	}

	RPGSH_CURRENCYSYSTEM(){}

	void MakeChange(RPGSH_CURRENCY* c);
	bool HasEquivalentTo(int Quantity, std::string Denomination);
	void print()
	{
		#define PAD_OFFSET 5

		unsigned int NameLength = 0;
		unsigned int LongestName = 0;

		//Get longest currency name as reference for spacing
		for(const auto& [key,value] : *this)
		{
			NameLength = key.length();
			if(NameLength > LongestName)
			{
				LongestName = NameLength;
			}
		}

		//Print formatted list of currencies and the currency values
		for(const auto& [key,value] : *this)
		{
			unsigned int QuantityLength = std::to_string(value.Quantity).length();
			unsigned int PadLength = (LongestName - key.length()) +
						 QuantityLength +
						 PAD_OFFSET;
			fprintf(stdout,"%s\e[97m%*d\e[0m\n",key.c_str(),PadLength,value.Quantity);
		}
	}
};
typedef std::pair<RPGSH_CURRENCY, int> money;
class RPGSH_WALLET
{
	std::map<RPGSH_CURRENCY, int> Money;

	int& operator [] (const RPGSH_CURRENCY b)
	{
		return Money[b];
	}

	//Iterator type for the class
	using iterator = typename std::map<RPGSH_CURRENCY, int>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_CURRENCYSYSTEM){}"
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

		fprintf(stdout,"Attempting to debit %d %s\n",q,d.Name.c_str());
		d.System->transaction.Name = d.Name;
		d.System->transaction.Quantity = q;

		if(Money[d]-q < 0 && d.System && d.System->HasEquivalentTo(q,d.Name))
		{
			Money[d] -= q;
			d.System->MakeChange(&d);
		}
		else if(Money[d]-q < 0 && d.System)
		{
			fprintf(stderr,"Insufficient funds!\n");
		}
		else if(Money[d]-q >= 0)
		{
			Money[d] -= q;
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

//Currency comparison operators order by position in hierarchy (e.g. Dollar > Dime == true)
bool RPGSH_CURRENCY::operator < (const RPGSH_CURRENCY& b) const
{
	for(const auto& [key,value] : *System)
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
	return (Name < b.Name);
}
RPGSH_CURRENCY& RPGSH_CURRENCY::operator -= (const int b)
{
	fprintf(stdout,"Attempting to debit %d %s\n",b,Name.c_str());
	System->transaction.Name = Name;
	System->transaction.Quantity = b;

	int ret = 0;
	if(Quantity-b < 0 && System && System->HasEquivalentTo(b,Name))
	{
		Quantity -= b;
		System->MakeChange(this);
	}
	else if(Quantity-b < 0 && System)
	{
		fprintf(stderr,"Insufficient funds!\n");
	}
	else if(Quantity-b >= 0)
	{
		Quantity -= b;
	}

	return *this;
}

bool RPGSH_CURRENCYSYSTEM::HasEquivalentTo(int Quantity, std::string D)
{
	RPGSH_CURRENCY c = Denomination[D];

	int total = c.Quantity;
	int totalFactor = 1;

	while(c.Larger != "")
	{
		c = Denomination[c.Larger];

		totalFactor *= c.SmallerAmount;
		total += c.Quantity * totalFactor;
	}

	return total >= Quantity;
}
void RPGSH_CURRENCYSYSTEM::MakeChange(RPGSH_CURRENCY* c)
{
	std::string NextHighest = c->Larger;
	int ConversionFactor = Denomination[NextHighest].SmallerAmount;

	//Number of Currency[NextHighest] that will need to be converted to current currency
	int ChangeCount = (abs(c->Quantity)/ConversionFactor);

	//Only break an extra higher denomination currency if we really need to
	//e.g. If I owe 13 dimes, but I have 1 dollar and 5 dimes, don't try to break 2 dollars and claim I don't have enough money
	int MinimumAmountToBreak = ChangeCount*ConversionFactor;
	int PreTransactionQuantity = transaction.Quantity+c->Quantity;
	if((MinimumAmountToBreak + PreTransactionQuantity) < transaction.Quantity)
	{
		//Fairly certain this is uneeded
		//This is implicitly true on the first pass and *shouldn't(?)* be needed on subsequent passes
		//if(c->Name == transaction.Name)
		ChangeCount++;
	}

	//Prevent unneccesary subtraction and check if the player has to make change with the currency after that to complete transation
	if(ChangeCount == 0)
	{
		return;
	}
	else if(Denomination[NextHighest].Quantity < ChangeCount)
	{
		MakeChange(&Denomination[NextHighest]);
	}

	//Make change by breaking the larger denomination into the smaller denomination
	c->Quantity += ConversionFactor*ChangeCount;
	Denomination[NextHighest] -= ChangeCount;
}

int main()
{
	#define PLATINUM	"Platinum"
	#define GOLD		"Gold"
	#define SILVER		"Silver"
	#define COPPER		"Copper"

	RPGSH_CURRENCYSYSTEM CS;
	RPGSH_CURRENCY(&CS, PLATINUM,	10,	10,	std::string(GOLD),	"");
	RPGSH_CURRENCY(&CS, GOLD,	10,	10,	std::string(SILVER),	PLATINUM);
	RPGSH_CURRENCY(&CS, SILVER,	10,	10,	std::string(COPPER),	GOLD);
	RPGSH_CURRENCY(&CS, COPPER,	10,	0,	"",			SILVER);

	fprintf(stdout,"Starting amount\n");
	CS.print();
	fprintf(stdout,"\n");

	CS[GOLD] -= 10;
	CS.print();
	fprintf(stdout,"\n");

	CS[SILVER] -= 25;
	CS.print();
	fprintf(stdout,"\n");

	CS[COPPER] -= 1223;
	CS.print();
	fprintf(stdout,"\n");

	CS[COPPER] -= 9999;
	CS.print();
	fprintf(stdout,"\n");

	CS[COPPER] -= 8637;
	CS.print();
	fprintf(stdout,"\n");

	return 0;
}
