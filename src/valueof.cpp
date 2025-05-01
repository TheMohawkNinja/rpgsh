#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(Error,"valueof expects at least 1 argument.");
		exit(-1);
	}
	if(argc > 3)
		output(Warning,"valueof expects no more than 2 arguments, ignoring all other arguments");

	chkFlagAppDesc(argv,"Returns the value of the given wallet in the requested currency.");
	chkFlagModifyVariables(argv,false);
	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tvalueof [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%swallet%s\t\tPretty prints the content of %swallet%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%swallet%s -%scurrency%s\tPretty prints the equivalent value of %swallet%s in %scurrency%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\t\tPrints this help text\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	Wallet w_in, w_out;
	bool hasWallet = false;
	std::string currency;
	for(int i=1; i<argc; i++)
	{
		if(argv[i][0] == WALLET_SIGIL && argv[i][1] == '{')
		{
			try
			{
				w_in = Wallet(std::string(argv[i]));
				hasWallet = true;
			}
			catch(const std::runtime_error& e)
			{
				output(Error,"Unable to construct a Wallet from \"%s\": %s",argv[i],e.what());
				return -1;
			}
		}
		else if(argv[i][0] == '-')
		{
			currency = right(std::string(argv[i]),1);
		}
	}

	if(!hasWallet)
	{
		output(Error,"valueof expects a wallet for an argument.");
		return -1;
	}
	if(currency == "")
	{
		for(const auto& line : getAppOutput("print "+std::string(w_in)).output)
			fprintf(stdout,"%s\n",line.c_str());
		fprintf(stdout,"\b");
		return 0;
	}
	else if(!w_in.containsCurrency(currency))
	{
		output(Error,"Currency \"%s\" is not contained within the inputted Wallet.",currency.c_str());
		return -1;
	}
	else
	{
		//Get smallest currency in system
		Currency smallest;
		for(const auto& [c,q] : w_in)
		{
			if(c.Smaller != "") continue;
			smallest = c;
			break;
		}

		//sdea = Smallest Denomination Equivalent Amount
		unsigned int sdea = w_in.getEquivalentValueInLowestDenomination(smallest.System);
		w_out = Wallet(money_t(smallest,sdea));

		//Give output wallet the keys of all currencies in the system
		datamap<Currency> s = getCurrencySystem(smallest.System);
		for(const auto& [k,v] : s)
		{
			if(!w_out[v]) w_out[v] = 1;
			fprintf(stdout,"Set %s to %d\n",v.Name.c_str(),w_out[v]);
		}

		for(const auto& [c,q] : w_out)
		{
			fprintf(stdout,"\tChecking %s\n",c.Name.c_str());
			if(c.System == smallest.System && c.Larger != "" && q >= s[c.Larger].SmallerAmount)
			{
				//Make use of the trucation of the divison return value to int to simplify the math
				int ConversionFactor = s[c.Larger].SmallerAmount;
				int AmountTradable = (w_out[c] / ConversionFactor);
				w_out[s[c.Larger]] += AmountTradable;
				w_out[c] -= (AmountTradable * ConversionFactor);
			}
		}

		for(const auto& line : getAppOutput("print "+std::string(w_out)).output)
			fprintf(stdout,"%s\n",line.c_str());
		fprintf(stdout,"\b");
	}

	return 0;
}
