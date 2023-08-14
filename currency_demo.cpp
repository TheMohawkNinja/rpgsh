#include <stdio.h>
#include <map>
#include <string>

class RPGSH_CURRENCYSYSTEM;

class RPGSH_CURRENCY
{
	public:
		RPGSH_CURRENCYSYSTEM* ParentCurrencySystem = nullptr;
		std::string Name = "";
		int Quantity = 0;
		std::string ConversionDown = "";
		std::string ConversionUp = "";

	RPGSH_CURRENCY(){}
	RPGSH_CURRENCY(std::string _Name, int _Quantity, std::string _ConversionDown, std::string _ConversionUp)
	{
		Name = _Name;
		Quantity = _Quantity;
		ConversionDown = _ConversionDown;
		ConversionUp = _ConversionUp;
	}

	void AttachToCurrencySystem(RPGSH_CURRENCYSYSTEM* _CurrencySystem);

	RPGSH_CURRENCY& operator -= (const int b);
};
class RPGSH_CURRENCYSYSTEM
{
	public:
		std::map<std::string, RPGSH_CURRENCY> Currency;

	RPGSH_CURRENCY& operator [] (const std::string b)
	{
		return Currency[b];
	}

	//Iterator type for the class
	using iterator = typename std::map<std::string, RPGSH_CURRENCY>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_CURRENCYSYSTEM){}"
	iterator begin() const
	{
		return Currency.begin();
	}
	iterator end() const
	{
		return Currency.end();
	}

	RPGSH_CURRENCYSYSTEM(){}

	int MakeChange(RPGSH_CURRENCY* c);
	bool HasEquivalentTo(int Quantity, std::string Denomination);
	void print()
	{
		#define PAD_OFFSET 5

		unsigned int NameLength = 0;
		unsigned int LongestName = 0;

		//Get longest currency name and quantity as references for spacing
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

//Links the instance of RPGSH_CURRENCY to the instance of RPGSH_CURRENCYSYSTEM
//Allows the instance of RPGSH_CURRENCYSYSTEM to detect changes in the attached RGPSH_CURRENCY
//Also automatically adds the Currency to the map of Currencies within the CurrencySystem
void RPGSH_CURRENCY::AttachToCurrencySystem(RPGSH_CURRENCYSYSTEM* _CurrencySystem)
{
	ParentCurrencySystem = _CurrencySystem;
	ParentCurrencySystem->Currency[Name] = *this;
}
RPGSH_CURRENCY& RPGSH_CURRENCY::operator -= (const int b)
{
	fprintf(stdout,"Attempting to debit %d %s\n",b,Name.c_str());

	int ret = 0;
	if(Quantity-b < 0 && ParentCurrencySystem && ParentCurrencySystem->HasEquivalentTo(b,Name))
	{
		Quantity -= b;
		ParentCurrencySystem->MakeChange(this);
	}
	else if(Quantity-b < 0 && ParentCurrencySystem)
	{
		fprintf(stderr,"Insufficient funds!\n");
	}
	else if(Quantity-b >= 0)
	{
		Quantity -= b;
	}

	return *this;
}
bool RPGSH_CURRENCYSYSTEM::HasEquivalentTo(int Quantity, std::string Denomination)
{
	RPGSH_CURRENCY c = Currency[Denomination];

	int total = c.Quantity;
	int totalFactor = 1;
	fprintf(stdout,"total = %d, number of %s = %d, totalFactor = %d\n",total,c.Name.c_str(),c.Quantity,totalFactor);

	while(true)
	{
		c = Currency[c.ConversionUp];
		int space = c.ConversionDown.find(" ");
		int length = c.ConversionDown.length();

		totalFactor *= std::stoi(c.ConversionDown.substr(0,space));
		total += c.Quantity * totalFactor;
		fprintf(stdout,"total = %d, number of %s = %d, totalFactor = %d\n",total,c.Name.c_str(),c.Quantity,totalFactor);

		if(c.ConversionUp == "")
		{
			break;
		}
	}

	fprintf(stdout,"Checking if %d >= %d\n",total,Quantity);
	return total >= Quantity;
}
int RPGSH_CURRENCYSYSTEM::MakeChange(RPGSH_CURRENCY* c)
{
	std::string NextHighest = c->ConversionUp;
	int ret = 0;
	int space = Currency[NextHighest].ConversionDown.find(" ");
	int ConversionFactor = std::stoi(Currency[NextHighest].ConversionDown.substr(0,space));

	//Number of Currency[NextHighest] that will need to be converted to current currency
	int ChangeCount = ((abs(c->Quantity))/ConversionFactor)+1;
	fprintf(stdout,"c->Quantity = %d, ConversionFactor = %d\n",c->Quantity,ConversionFactor);

	//If the player can't make change with the next highest up currency, try the currency after that
	if(Currency[NextHighest].Quantity < ChangeCount)
	{
		MakeChange(&Currency[NextHighest]);
	}
	c->Quantity += ConversionFactor*ChangeCount;
	Currency[NextHighest] -= ChangeCount;

	return ret;
}

int main()
{
	#define PLATINUM	"Platinum"
	#define GOLD		"Gold"
	#define SILVER		"Silver"
	#define COPPER		"Copper"

	RPGSH_CURRENCYSYSTEM C;
	RPGSH_CURRENCY(PLATINUM,10,	"10 "+std::string(GOLD),	"").AttachToCurrencySystem(&C);
	RPGSH_CURRENCY(GOLD,	10,	"10 "+std::string(SILVER),	PLATINUM).AttachToCurrencySystem(&C);
	RPGSH_CURRENCY(SILVER,	10,	"10 "+std::string(COPPER),	GOLD).AttachToCurrencySystem(&C);
	RPGSH_CURRENCY(COPPER,	10,	"",				SILVER).AttachToCurrencySystem(&C);

	fprintf(stdout,"Starting amount\n");
	C.print();
	fprintf(stdout,"\n");

	C[GOLD] -= 10;
	C.print();
	fprintf(stdout,"\n");

	C[SILVER] -= 25;
	C.print();
	fprintf(stdout,"\n");

	C[COPPER] -= 1223;
	C.print();
	fprintf(stdout,"\n");

	C[COPPER] -= 9999;
	C.print();
	fprintf(stdout,"\n");

	C[COPPER] -= 8637;
	C.print();
	fprintf(stdout,"\n");

	return 0;
}
