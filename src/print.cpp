#include <cstring>
#include <regex>
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

void printHeader(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(long unsigned int i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
}
template <typename T>
void getLongestKey(datamap<T> m, unsigned int* p_current_longest_len)
{
	for(auto& [k,v] : m)
	{
		//Get longest key length, omitting hidden variables
		if(k[0] != '.' && k.length() > (*p_current_longest_len))
			(*p_current_longest_len) = k.length();
	}
}
void printData(Scope scope)
{
	for(auto& [k,v] : scope.getDatamap<Var>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sVar%s%s",TEXT_BOLD,VAR_COLOR,TEXT_NORMAL,addSpaces(COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sValue:  %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Value));
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Dice>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sDice%s%s",TEXT_BOLD,DICE_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		if(v.List != "")
		{
			fprintf(stdout,"%sList:         %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.List));
		}
		else
		{
			fprintf(stdout,"%sQuantity:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(std::to_string(v.Quantity)));
			fprintf(stdout,"%sFaces:        %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(std::to_string(v.Faces)));
			fprintf(stdout,"%sModifier:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(std::to_string(v.Modifier)));
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Wallet>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;

		//Get longest currency name
		long unsigned int longest_cur = 0;
		for(auto& [c,q] : v)
		{
			if(!q) continue;
			if(c.Name.length() > longest_cur)
				longest_cur = c.Name.length();
		}

		fprintf(stdout,"%s%sWallet%s%s",TEXT_BOLD,WALLET_COLOR,TEXT_NORMAL,addSpaces(longest_cur+COLUMN_PADDING-5).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);

		//Print values
		for(auto& [c,q] : v)
		{
			if(!q) continue;

			fprintf(stdout,"%s%s:%s%s",TEXT_ITALIC,c.Name.c_str(),TEXT_NORMAL,addSpaces(longest_cur-c.Name.length()+COLUMN_PADDING).c_str());
			fprintf(stdout,"%s\n",makePretty(std::to_string(q)));
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Currency>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sCurrency%s%s",TEXT_BOLD,CURRENCY_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sSystem:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.System));
		fprintf(stdout,"%sName:             %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Name));
		fprintf(stdout,"%sSmallerAmount:    %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(std::to_string(v.SmallerAmount)));
		fprintf(stdout,"%sSmaller:          %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Smaller));
		fprintf(stdout,"%sLarger:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Larger));
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\b");//Remove extraneous newline
}
void printCharacterVariables()
{
	Character c = Character();
	std::string sigil(1,CHARACTER_SIGIL);
	printHeader("("+sigil+") "+c.getName());
	printData(c);
}
void printCampaignVariables()
{
	confirmCampaignVariablesFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = getEnvVariable(ENV_CURRENT_CAMPAIGN);
	printHeader("("+sigil+") "+left(m_name,m_name.length()-1));// Omit trailing '/'
	printData(m);
}
void printShellVariables()
{
	Shell s = Shell();
	std::string sigil(1,SHELL_SIGIL);
	printHeader("("+sigil+") "+"Shell");
	printData(s);
}
int main(int argc, char** argv)
{
	chkFlagAppDesc(argv,"Pretty prints variables, variable sets, and scopes.");
	chkFlagModifyVariables(argv,false);

	if(argc > 2)
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);

	if(argc == 1)
	{
		printShellVariables();
		fprintf(stdout,"\n");
		printCampaignVariables();
		fprintf(stdout,"\n");
		printCharacterVariables();
	}
	else if(isTypeSigil(argv[1][0]) && argv[1][1] == '{')
	{
		Scope variable;
		std::string v_str = std::string(argv[1]);

		try
		{
			switch(argv[1][0])
			{
				case(VAR_SIGIL):
					variable.set<Var>("",Var(v_str));
					break;
				case(DICE_SIGIL):
					variable.set<Dice>("",Dice(v_str));
					break;
				case(WALLET_SIGIL):
					variable.set<Wallet>("",Wallet(v_str));
					break;
				case(CURRENCY_SIGIL):
					variable.set<Currency>("",Currency(v_str));
					break;
			}
		}
		catch(const std::runtime_error& e)
		{
			output(Error,"Unable to pretty-print explicit constructor \"%s\": %s",v_str.c_str(),e.what());
			exit(-1);
		}
		printData(variable);
	}
	else if(looksLikeSet(std::string(argv[1])))
	{
		Scope set;

		for(const auto& [k,v] : getSet(std::string(argv[1])))
		{
			switch(v[0])
			{
				case VAR_SIGIL:
					set.set<Var>(k,v);
					break;
				case DICE_SIGIL:
					set.set<Dice>(k,v);
					break;
				case WALLET_SIGIL:
					set.set<Wallet>(k,v);
					break;
				case CURRENCY_SIGIL:
					set.set<Currency>(k,v);
					break;
			}
		}

		printData(set);
	}
	else if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tprint\n");
		fprintf(stdout,"\tprint [%svariable%s | %svariable set%s] [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints all variables in all scopes.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
	}
	else
	{
		output(Error,"Unknown option \"%s\".",argv[1]);
		exit(-1);
	}
}
