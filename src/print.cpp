#include <cstring>
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

void print_header(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(long unsigned int i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
}
template <typename T>
void get_longest_key_from_datamap(datamap<T> m, unsigned int* p_current_longest_len)
{
	for(auto& [k,v] : m)
	{
		//Get longest key length, omitting hidden variables
		if(k[0] != '.' && k.length() > (*p_current_longest_len))
			(*p_current_longest_len) = k.length();
	}
}
const char* print_value(std::string value)
{
	if(value != "") return value.c_str();
	else return empty_str.c_str();
}
void print_data(Scope scope)
{
	unsigned long var_len = scope.getDatamap<Var>().size();
	unsigned long dice_len = scope.getDatamap<Dice>().size();
	unsigned long wallet_len = scope.getDatamap<Wallet>().size();
	unsigned long currency_len = scope.getDatamap<Currency>().size();
	unsigned long i=0;
	for(auto& [k,v] : scope.getDatamap<Var>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		i++;
		fprintf(stdout,"%s%sVar%s%s",TEXT_BOLD,VAR_COLOR,TEXT_NORMAL,addSpaces(COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sValue:  %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Value));
		if(i < var_len-1 || dice_len || wallet_len || currency_len) fprintf(stdout,"\n");
	}
	i=0;
	for(auto& [k,v] : scope.getDatamap<Dice>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		i++;
		fprintf(stdout,"%s%sDice%s%s",TEXT_BOLD,DICE_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		if(v.List != "")
		{
			fprintf(stdout,"%sList:         %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.List));
		}
		else
		{
			fprintf(stdout,"%sQuantity:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(std::to_string(v.Quantity)));
			fprintf(stdout,"%sFaces:        %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(std::to_string(v.Faces)));
			fprintf(stdout,"%sModifier:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(std::to_string(v.Modifier)));
		}
		if(i < dice_len || wallet_len || currency_len) fprintf(stdout,"\n");
	}
	i=0;
	for(auto& [k,v] : scope.getDatamap<Wallet>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		i++;

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
			fprintf(stdout,"%s\n",print_value(std::to_string(q)));
		}
		if(i < wallet_len || currency_len) fprintf(stdout,"\n");
	}
	i=0;
	for(auto& [k,v] : scope.getDatamap<Currency>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		i++;
		fprintf(stdout,"%s%sCurrency%s%s",TEXT_BOLD,CURRENCY_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sSystem:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.System));
		fprintf(stdout,"%sName:             %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Name));
		fprintf(stdout,"%sSmallerAmount:    %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(std::to_string(v.SmallerAmount)));
		fprintf(stdout,"%sSmaller:          %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Smaller));
		fprintf(stdout,"%sLarger:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Larger));
		if(i < currency_len) fprintf(stdout,"\n");
	}
}
void print_character_variables()
{
	Character c = Character(false);
	std::string sigil(1,CHARACTER_SIGIL);
	print_header("("+sigil+") "+c.getName());
	print_data(c);
}
void print_campaign_variables()
{
	confirmCampaignVariablesFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR);
	print_header("("+sigil+") "+left(m_name,m_name.length()-1));// Omit trailing '/'
	print_data(m);
}
void print_shell_variables()
{
	Shell s = Shell();
	std::string sigil(1,SHELL_SIGIL);
	print_header("("+sigil+") "+"Shell");
	print_data(s);
}
int main(int argc, char** argv)
{
	check_print_app_description(argv,"Pretty prints variables, variable sets, and scopes.");

	if(argc > 2)
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);

	if(argc == 1 || !strcmp(argv[1],"--all"))
	{
		print_shell_variables();
		fprintf(stdout,"\n");
		print_campaign_variables();
		fprintf(stdout,"\n");
		print_character_variables();
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
		print_data(variable);
	}
	else if(looksLikeSet(std::string(argv[1])))
	{
		std::string set_string = std::string(argv[1]);
		Scope set;

		while(findu(set_string,DS) != std::string::npos)
		{
			std::string set_key = left(set_string,findu(set_string,DS));
			set_string = right(set_string,findu(set_string,set_key)+set_key.length()+DS.length());
			std::string set_value = left(set_string,findu(set_string,DS));

			switch(set_value[0])
			{
				case VAR_SIGIL:
					set.set<Var>(set_key,set_value);
					break;
				case DICE_SIGIL:
					set.set<Dice>(set_key,set_value);
					break;
				case WALLET_SIGIL:
					set.set<Wallet>(set_key,set_value);
					break;
				case CURRENCY_SIGIL:
					set.set<Currency>(set_key,set_value);
					break;
			}

			if(findu(set_string,DS) != std::string::npos)
				set_string = right(set_string,findu(set_string,set_value)+set_value.length()+DS.length());
			else
				break;
		}

		print_data(set);
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
