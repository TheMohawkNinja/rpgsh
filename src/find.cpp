#include <string>
#include <vector>
#include <regex>
#include "../headers/functions.h"

int main(int argc, char** argv)
{

	if(argc > 2)
		output(Warning,"find only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Searches rpgsh applications, campaigns, characters, and variables for matches to a pattern.");
	chkFlagModifyVariables(argv,true);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tfind [[%sOPTIONS%s]|%spattern%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%spattern%s\tPrint all rpgsh applications, campaigns, characters, and variables that match %spattern%s",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);

		return 0;
	}

	std::string p = std::string(argv[1]);
	std::regex pattern(p);
	Character c = Character();
	Campaign m = Campaign();
	Shell s = Shell();

	printHeader("Camapigns");
	std::vector<std::string> campaigns = getDirectoryListing(campaigns_dir);
	for(const auto& campaign : campaigns)
	{
		std::sregex_iterator it(campaign.begin(),campaign.end(),pattern);
		std::sregex_iterator it_end;

		if(it != it_end)
			fprintf(stdout,"%s\n",campaign.c_str());
	}
	fprintf(stdout,"\n");

	printHeader("Characters");
	std::string m_dir = left(m.getDatasource(),rfindu(m.getDatasource(),'/'));
	std::vector<std::string> characters = getDirectoryListing(m_dir+"/characters");
	for(const auto& character : campaigns)
	{
		std::sregex_iterator it(character.begin(),character.end(),pattern);
		std::sregex_iterator it_end;

		if(it != it_end)
			fprintf(stdout,"%s\n",character.c_str());
	}
	fprintf(stdout,"\n");

	return 0;
}
