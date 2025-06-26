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
		fprintf(stdout,"\t%swallet%s %scurrency%s\tPretty prints the equivalent value of %swallet%s in %scurrency%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\t\tPrints this help text\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	Wallet w_in, w_out;
	bool hasWallet = false;
	Currency currency;
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
		else if(argv[i][0] == CURRENCY_SIGIL && argv[i][1] == '{')
		{
			try
			{
				currency = Currency(std::string(argv[i]));
			}
			catch(const std::runtime_error& e)
			{
				output(Error,"Unable to construct a Currency from \"%s\": %s",argv[i],e.what());
				return -1;
			}
		}
		else if(argv[i][0] == '-')
		{
			output(Warning,"Unknown option \"%s\"",argv[i]);
		}
	}

	if(!hasWallet)
	{
		output(Error,"valueof expects a wallet for an argument.");
		return -1;
	}
	if(currency == Currency())
	{
		for(const auto& line : getAppOutput("print "+std::string(w_in)).output)
			fprintf(stdout,"%s\n",line.c_str());
		fprintf(stdout,"\b");
		return 0;
	}
	else if(!w_in.containsCurrency(currency.Name))
	{
		output(Error,"Currency \"%s\" is not contained within the inputted Wallet.",currency.c_str());
		return -1;
	}
	else
	{
		//Get smallest currency in system
		Currency smallest;
		for(const auto& [k,v] : getCurrencySystem(currency.System))
		{
			if(v.Smaller != "") continue;
			smallest = v;
			break;
		}

		//sdea = Smallest Denomination Equivalent Amount
		int sdea = w_in.getEquivalentValueInLowestDenomination(smallest.System);
		w_out = Wallet(Money{smallest,sdea});

		//Give output wallet the keys of all currencies in the system
		datamap<Currency> s = getCurrencySystem(smallest.System);
		for(const auto& [k,v] : s)
			if(!w_out.get(v)) w_out.set(v,0);

		for(long unsigned int i=0; i<w_out.Monies.size(); i++)
		{
			Money m = w_out.Monies[i];
			if(m.c.Larger != "" && m.c.Larger != currency.Larger && m.q >= s[m.c.Larger].SmallerAmount)
			{
				//Make use of the trucation of the divison return value to int to simplify the math
				int ConversionFactor = s[m.c.Larger].SmallerAmount;
				int AmountTradable = (w_out.get(m.c)/ConversionFactor);
				w_out.set(s[m.c.Larger],w_out.get(s[m.c.Larger])+AmountTradable);
				w_out.set(m.c,w_out.get(m.c)-(AmountTradable*ConversionFactor));
				i=0;
			}
		}

		for(long unsigned int i=0; i<w_out.Monies.size()-1; i++)
		{
			if(w_out.Monies[i].q) break;
			w_out.Monies.erase(w_out.Monies.begin()+i);
			i--;
		}
		fprintf(stdout,"%d\n",w_out.Monies[0].q);
		if(w_out.Monies.size() > 1)
		{
			fprintf(stdout,"\nRemainder: ");
			for(long unsigned int i=1; i<w_out.Monies.size(); i++)
				fprintf(stdout,"%d %s, ",w_out.Monies[i].q,w_out.Monies[i].c.Name.c_str());
			fprintf(stdout,"\b\b%s\n",CLEAR_TO_LINE_END);
		}
	}

	return 0;
}
