#include <cstring>
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

//TODO print functions should include all datatypes

void print_header(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(int i=0; i<s.length()+2; i++)
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
void print_spaces(unsigned int end)
{
	for(int i=0; i<end; i++)
		fprintf(stdout," ");
}
const char* print_value(std::string value)
{
	if(value != "")
		return value.c_str();
	else
		return empty_str.c_str();
}
void print_data(Scope scope)
{
	for(auto& [k,v] : scope.getDatamap<CurrencySystem>())
	{
		//Skip hidden variables
		if(k[0] == '.')
			continue;

		fprintf(stdout,"%s%sCurrencySystem%s",TEXT_BOLD,CURRENCYSYSTEM_COLOR,TEXT_NORMAL);
		print_spaces(COLUMN_PADDING);
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sName:              %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Name));
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Currency>())
	{
		//Skip hidden variables
		if(k[0] == '.')
			continue;

		fprintf(stdout,"%s%sCurrency%s",TEXT_BOLD,CURRENCY_COLOR,TEXT_NORMAL);
		print_spaces(2*COLUMN_PADDING);
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sSystem:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.System->Name));
		fprintf(stdout,"%sName:             %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Name));
		fprintf(stdout,"%sSmallerAmount:    %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(std::to_string(v.SmallerAmount)));
		fprintf(stdout,"%sSmaller:          %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Smaller));
		fprintf(stdout,"%sLarger:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Larger));
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Dice>())
	{
		//Skip hidden variables
		if(k[0] == '.')
			continue;

		fprintf(stdout,"%s%sDice%s",TEXT_BOLD,DICE_COLOR,TEXT_NORMAL);
		print_spaces(2*COLUMN_PADDING);
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
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Var>())
	{
		//Skip hidden variables
		if(k[0] == '.')
			continue;

		fprintf(stdout,"%s%sVar%s",TEXT_BOLD,VAR_COLOR,TEXT_NORMAL);
		print_spaces(COLUMN_PADDING);
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sValue:  %s%s\n",TEXT_ITALIC,TEXT_NORMAL,print_value(v.Value));
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Wallet>())
	{
		//Skip hidden variables
		if(k[0] == '.')
			continue;

		//Get longest currency name
		int longest_cur = 0;
		for(auto& [c,q] : v)
		{
			if(c.Name.length() > longest_cur)
				longest_cur = c.Name.length();
		}

		fprintf(stdout,"%s%sWallet%s",TEXT_BOLD,WALLET_COLOR,TEXT_NORMAL);
		print_spaces(longest_cur+COLUMN_PADDING-5);//6 = length of "Wallet" + ':' in printed values below
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);

		//Print values
		for(auto& [c,q] : v)
		{
			fprintf(stdout,"%s%s:%s",TEXT_ITALIC,c.Name.c_str(),TEXT_NORMAL);
			print_spaces(longest_cur-c.Name.length()+COLUMN_PADDING);
			fprintf(stdout,"%s\n",print_value(std::to_string(q)));
		}

		fprintf(stdout,"\n");
	}
}
void print_player_attrs()
{
	Character c = Character(false);
	std::string sigil(1,CHARACTER_SIGIL);
	print_header("("+sigil+") "+c.getName());
	print_data(c);
}
void print_campaign_vars()
{
	confirmCampaignVarsFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR);
	print_header("("+sigil+") "+m_name.substr(0,m_name.length()-1));// Omit trailing '/'
	print_data(m);
}
void print_shell_vars()
{
	Shell s = Shell();
	std::string sigil(1,SHELL_SIGIL);
	print_header("("+sigil+") "+"Shell");
	print_data(s);
}
int main(int argc, char** argv)
{
	check_print_app_description(argv,"Prints one or all variable scopes.");

	if(argc > 2)
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);

	if(argc == 1 || !strcmp(argv[1],"--all"))// Print everything
	{
		print_shell_vars();
		fprintf(stdout,"\n");
		print_campaign_vars();
		fprintf(stdout,"\n");
		print_player_attrs();
	}
	else if(!strcmp(argv[1],"-c") || !strcmp(argv[1],"--character"))
	{
		print_player_attrs();
	}
	else if(!strcmp(argv[1],"-m") || !strcmp(argv[1],"--campaign"))
	{
		print_campaign_vars();
	}
	else if(!strcmp(argv[1],"-s") || !strcmp(argv[1],"--shell"))
	{
		print_shell_vars();
	}
	else if(!strcmp(argv[1],"-?") || !strcmp(argv[1],"--help"))
	{
		fprintf(stdout,"Prints all variables contained in one or all Scopes\n\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tlist %soption%s\n\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"OPTIONS:\n");
		fprintf(stdout,"\t%snone%s | --all\tPrints all variables in all Scopes.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t--character\tPrints all character attributes.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t--campaign\tPrints all current campaign variables.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t--shell\t\tPrints all shell variables.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-? | --all\tPrints this help text.\n",TEXT_ITALIC,TEXT_NORMAL);
	}
	else
	{
		output(Error,"Unknown option \"%s\".",argv[1]);
		exit(-1);
	}
}
