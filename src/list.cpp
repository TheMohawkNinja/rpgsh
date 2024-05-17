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
void print_map(T m, char scope_sigil)
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

		fprintf(stdout,"%c%s",scope_sigil,k.c_str());
		for(int i=k.length(); i<key_max_len+5; i++)
			fprintf(stdout," ");
		fprintf(stdout,"%s\n",v.c_str());
	}
}
void print_player_attrs()
{
	Character c = Character(false);
	std::string sigil(1,CHARACTER_SIGIL);
	print_header("("+sigil+") "+c.getStr<Var>(c.getStr<Var>(std::string(CHAR_NAME_ATTR))));
	print_map<datamap<Var>>(c.getDatamap<Var>(),CHARACTER_SIGIL);
}
void print_campaign_vars()
{
	confirmCampaignVarsFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR);
	print_header("("+sigil+") "+m_name.substr(0,m_name.length()-1));// Omit trailing '/'
	print_map<datamap<Var>>(m.getDatamap<Var>(),CAMPAIGN_SIGIL);
}
void print_shell_vars()
{
	Shell s = Shell();
	std::string sigil(1,SHELL_SIGIL);
	print_header("("+sigil+") "+"Shell");
	print_map<datamap<Var>>(s.getDatamap<Var>(),SHELL_SIGIL);
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
