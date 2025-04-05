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
void printData(Scope scope, Var depth=-1, bool raw=false)
{
	for(auto& [k,v] : scope.getDatamap<Var>())
	{
		if(k[0] == '.' || (depth>=0 && depth<countu(k,'/')+1)) continue;
		if(!raw)
		{
			fprintf(stdout,"%s%sVar%s%s",TEXT_BOLD,VAR_COLOR,TEXT_NORMAL,addSpaces(COLUMN_PADDING).c_str());
			fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
			fprintf(stdout,"%sValue:  %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Value));
		}
		else
		{
			fprintf(stdout,"%s\n",makePretty(v.Value));
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Dice>())
	{
		if((k[0] == '.') || (depth>=0 && depth<countu(k,'/')+1)) continue;
		if(!raw)
		{
			fprintf(stdout,"%s%sDice%s%s",TEXT_BOLD,DICE_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
			fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);

			if(v.List != "")
			{
				fprintf(stdout,"%sList:         %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.List));
			}
			else
			{
				fprintf(stdout,"%sQuantity:     %s%u\n",TEXT_ITALIC,TEXT_NORMAL,v.Quantity);
				fprintf(stdout,"%sFaces:        %s%u\n",TEXT_ITALIC,TEXT_NORMAL,v.Faces);
				fprintf(stdout,"%sModifier:     %s%d\n",TEXT_ITALIC,TEXT_NORMAL,v.Modifier);
			}
		}
		else
		{
			if(v.List != "")
				fprintf(stdout,"%s\n",makePretty(v.List));
			else
				fprintf(stdout,"%ud%u%s%d\n",v.Quantity,v.Faces,v.Modifier>=0 ? "+" : "",v.Modifier);
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Wallet>())
	{
		if((k[0] == '.') || (depth>=0 && depth<countu(k,'/')+1)) continue;

		//Get longest currency name
		long unsigned int longest_cur = 0;
		for(auto& [c,q] : v)
		{
			if(!q) continue;
			if(c.Name.length() > longest_cur)
				longest_cur = c.Name.length();
		}
		if(!raw)
		{
			fprintf(stdout,"%s%sWallet%s%s",TEXT_BOLD,WALLET_COLOR,TEXT_NORMAL,addSpaces(longest_cur+COLUMN_PADDING-5).c_str());
			fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		}

		//Print values
		for(auto& [c,q] : v)
		{
			if(!q) continue;
			fprintf(stdout,"%s%s:%s%s",TEXT_ITALIC,c.Name.c_str(),TEXT_NORMAL,addSpaces(longest_cur-c.Name.length()+COLUMN_PADDING).c_str());
			fprintf(stdout,"%d\n",q);
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Currency>())
	{
		if((k[0] == '.') || (depth>=0 && depth<countu(k,'/')+1)) continue;
		if(!raw)
		{
			fprintf(stdout,"%s%sCurrency%s%s",TEXT_BOLD,CURRENCY_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
			fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		}
		fprintf(stdout,"%sSystem:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.System));
		fprintf(stdout,"%sName:             %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Name));
		fprintf(stdout,"%sSmallerAmount:    %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(std::to_string(v.SmallerAmount)));
		fprintf(stdout,"%sSmaller:          %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Smaller));
		fprintf(stdout,"%sLarger:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,makePretty(v.Larger));
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\b");//Remove extraneous newline
}
void printCharacterVariables(Var depth=-1, bool raw=false)
{
	Character c = Character();
	std::string sigil(1,CHARACTER_SIGIL);
	if(!raw) printHeader("("+sigil+") "+c.getName());
	printData(c, depth, raw);
}
void printCampaignVariables(Var depth=-1, bool raw=false)
{
	confirmCampaignVariablesFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = getEnvVariable(ENV_CURRENT_CAMPAIGN);
	if(!raw) printHeader("("+sigil+") "+left(m_name,m_name.length()-1));// Omit trailing '/'
	printData(m, depth, raw);
}
void printShellVariables(Var depth=-1, bool raw=false)
{
	Shell s = Shell();
	std::string sigil(1,SHELL_SIGIL);
	if(!raw) printHeader("("+sigil+") "+"Shell");
	printData(s, depth, raw);
}
int main(int argc, char** argv)//TODO: Crashing when trying to print single variable
{
	chkFlagAppDesc(argv,"Pretty prints variables, variable sets, and scopes.");
	chkFlagModifyVariables(argv,false);

	if(argc > 4)
		output(Warning,"Print expects no more than four arguments. All args past \"%s\" will be ignored.",argv[1]);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tprint\n");
		fprintf(stdout,"\tprint [%svariable%s | %svariable set%s] [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPretty prints all variables in all scopes.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-d\t\t\tPretty prints all variables in all scopes up to a depth (number of blocks of text separated by \'/\' in the key) of %sn%s.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-d %sn%s\t\tPretty prints data in the set with keys up to a depth of %sn%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-r\t\t\tOnly pretty print the data, no headers.\n");
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}


	Scope data;
	Var depth = -1;
	bool raw = false;
	for(int i=1; i<argc; i++)
	{
		std::string arg_str = std::string(argv[i]);
		fprintf(stdout,"argv[%d] = %s\n",i,argv[i]);

		if(isTypeSigil(argv[i][0]) && argv[i][1] == '{')
		{
			fprintf(stdout,"Looks like explicit constructor.\n");
			try
			{
				switch(argv[i][0])
				{
					case(VAR_SIGIL):
						data.set<Var>("",Var(arg_str));
						break;
					case(DICE_SIGIL):
						data.set<Dice>("",Dice(arg_str));
						break;
					case(WALLET_SIGIL):
						data.set<Wallet>("",Wallet(arg_str));
						break;
					case(CURRENCY_SIGIL):
						data.set<Currency>("",Currency(arg_str));
						break;
				}
			}
			catch(const std::runtime_error& e)
			{
				output(Error,"Unable to pretty-print explicit constructor \"%s\": %s",arg_str.c_str(),e.what());
				return -1;
			}
		}
		else if(looksLikeSet(arg_str))
		{
			for(const auto& [k,v] : getSet(arg_str))
			{
				switch(v[0])
				{
					case VAR_SIGIL:
						data.set<Var>(k,v);
						break;
					case DICE_SIGIL:
						data.set<Dice>(k,v);
						break;
					case WALLET_SIGIL:
						data.set<Wallet>(k,v);
						break;
					case CURRENCY_SIGIL:
						data.set<Currency>(k,v);
						break;
				}
			}
		}
		else if(arg_str == "-d")
		{
			if(i == argc)
			{
				output(Error,"Missing value for print depth.");
				return -1;
			}

			if(i < argc-1)
			{
				depth = Var(std::string(argv[i+1]));
				i++;
				if(!depth.isInt())
				{
					output(Error,"Invalid value \'%s\" for depth.",depth.c_str());
					return -1;
				}
			}
		}
		else if(arg_str == "-r")
		{
			raw = true;
		}
		else
		{
			output(Error,"Unknown option \"%s\".",argv[i]);
			return -1;
		}
	}

	if(data.isEmpty())
	{
		printShellVariables(depth,raw);
		fprintf(stdout,"\n");
		printCampaignVariables(depth,raw);
		fprintf(stdout,"\n");
		printCharacterVariables(depth,raw);
		return 0;
	}
	printData(data, depth, raw);
}
