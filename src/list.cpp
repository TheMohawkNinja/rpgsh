#include <cstring>
#include "../headers/char.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

void print_header(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(int i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
}
template <typename T>
void print_map(T m, char Scope_sigil)
{
	unsigned int key_max_len = 0;
	for(auto& [k,v] : m)
	{
		if(k[0] != '.' && k.length() > key_max_len)
			key_max_len = k.length();
	}
	for(auto& [k,v] : m)
	{
		if(k[0] == '.')
			continue;

		fprintf(stdout,"%c%s",Scope_sigil,k.c_str());
		for(int i=k.length(); i<key_max_len+5; i++)
			fprintf(stdout," ");
		fprintf(stdout,"%s\n",v.c_str());
	}
}
void print_player_attrs()
{
	Character c = Character(false);
	std::string sigil(1,CHAR_SIGIL);
	print_header("("+sigil+") "+c.getStr<Var>(c.getStr<Var>(std::string(CHAR_NAME_ATTR))));
	print_map<datamap<Var>>(c.getDatamap<Var>(),CHAR_SIGIL);
}
void print_campaign_vars()
{
	confirmCampaignVarsFile();
	std::map<std::string,std::string> vars;
	std::string current_campaign = get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR);
	std::string campaign_vars_file = campaigns_dir +
					current_campaign +
					".vars";
	current_campaign = current_campaign.substr(0,current_campaign.length()-1);
	std::string sigil(1,CAMPAIGN_SIGIL);// Omit trailing '/'
	print_header("("+sigil+") "+current_campaign);
	vars = load_vars_from_file(campaign_vars_file);
	print_map<std::map<std::string,std::string>>(vars,CAMPAIGN_SIGIL);
}
void print_shell_vars()
{
	confirmShellVarsFile();
	std::map<std::string,std::string> vars;
	std::string sigil(1,SHELL_SIGIL);
	print_header("("+sigil+") "+"Shell");
	vars = load_vars_from_file(shell_vars_file);
	print_map<std::map<std::string,std::string>>(vars,SHELL_SIGIL);
}
int main(int argc, char** argv)
{
	check_print_app_description(argv,"Prints one or all variable Scopes.");

	if(argc > 2)
	{
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);
	}

	if(argc == 1 || !strcmp(argv[1],"--all"))// Print everything
	{
		print_shell_vars();
		fprintf(stdout,"\n");
		print_campaign_vars();
		fprintf(stdout,"\n");
		print_player_attrs();
	}
	else if(!strcmp(argv[1],"--character"))
		print_player_attrs();
	else if(!strcmp(argv[1],"--campaign"))
		print_campaign_vars();
	else if(!strcmp(argv[1],"--shell"))
		print_shell_vars();
	else if(!strcmp(argv[1],"--help") || !strcmp(argv[1],"-?"))
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
