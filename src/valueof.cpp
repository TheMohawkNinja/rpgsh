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
		Currency smallest;
		for(const auto& [c,q] : w_in)
		{
			if(stringcasecmp(c.Name,currency)) continue;
			smallest = c;
			break;
		}

		//sdea = Smallest Denomination Equivalent Amount
		unsigned int sdea = getEquivalentValueInLowestDenomination(smallest.System);
	}

	return 0;
}
