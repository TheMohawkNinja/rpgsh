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
const char* printValue(std::string value)
{
	if(value == "") return empty_str.c_str();

	std::map<std::string,char> common_esc_char = {
	{"\\\\a",'\a'}, //Bell
	{"\\\\b",'\b'}, //Backspace
	{"\\\\f",'\f'}, //Form feed
	{"\\\\n",'\n'}, //New line
	{"\\\\r",'\r'}, //Carriage return
	{"\\\\t",'\t'}, //Horizontal tab
	{"\\\\v",'\v'}};//Vertical tab

	for(const auto& [k,v] : common_esc_char)
		value = std::regex_replace(value,std::regex(k),std::string(1,v));

	for(long unsigned int i=0; i<value.length(); i++)
		if(value[i] == '\\') value.erase(value.begin()+i);

	return value.c_str();
}
void printData(Scope scope)
{
	for(auto& [k,v] : scope.getDatamap<Var>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sVar%s%s",TEXT_BOLD,VAR_COLOR,TEXT_NORMAL,addSpaces(COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sValue:  %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Value));
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
			fprintf(stdout,"%sList:         %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.List));
		}
		else
		{
			fprintf(stdout,"%sQuantity:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.Quantity)));
			fprintf(stdout,"%sFaces:        %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.Faces)));
			fprintf(stdout,"%sModifier:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.Modifier)));
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
			fprintf(stdout,"%s\n",printValue(std::to_string(q)));
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Currency>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sCurrency%s%s",TEXT_BOLD,CURRENCY_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sSystem:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.System));
		fprintf(stdout,"%sName:             %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Name));
		fprintf(stdout,"%sSmallerAmount:    %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.SmallerAmount)));
		fprintf(stdout,"%sSmaller:          %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Smaller));
		fprintf(stdout,"%sLarger:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Larger));
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\b");//Remove extraneous newline
}
void printCharacterVariables()
{
	Character c = Character(false);
	std::string sigil(1,CHARACTER_SIGIL);
	printHeader("("+sigil+") "+c.getName());
	printData(c);
}
void printCampaignVariables()
{
	confirmCampaignVariablesFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = getEnvVariable(CURRENT_CAMPAIGN_SHELL_VAR);
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
	chkPrntAppDesc(argv,"Pretty prints variables, variable sets, and scopes.");

	if(argc > 2)
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);

	if(argc == 1 || !strcmp(argv[1],"--all"))
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
		printData(variable);
	}
	else if(looksLikeSet(std::string(argv[1])))
	{
		std::string set_string = std::string(argv[1]);
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
	else if(!strcmp(argv[1],"-?") || !strcmp(argv[1],"--help"))
	{
		fprintf(stdout,"Pretty prints variables, variable sets, and scopes.\n\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tprint\t\t\tPrints all variables in all scopes.\n");
		fprintf(stdout,"\tprint %svariable%s\t\tPretty prints information about variable.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tprint %svariable set%s\tPretty prints information about all variables in the set.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tprint %soption%s\t\tSee %sOPTIONS.%s\n\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"OPTIONS:\n");
		fprintf(stdout,"\t--all\t\t\tEquivalent to entering no arguments.\n");
		fprintf(stdout,"\t-? | --all\t\tPrints this help text.\n");
	}
	else
	{
		output(Error,"Unknown option \"%s\".",argv[1]);
		exit(-1);
	}
}
