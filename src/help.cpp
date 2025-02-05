#include <cstring>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"help only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Prints this helpful list of applications and what they do.");
	chkFlagModifyVariables(argv,false);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\thelp [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints a list of applications and their descriptions.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-a\t\tPrints a list of applications along with their descriptions and help text.\n");
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	bool print_app_help = argv[1] && !strcasecmp(argv[1],"-a");

	//Generate list of apps from cache file
	std::vector<std::string> applications;
	std::ifstream ifs(rpgsh_programs_cache_path);
	std::string line = "";
	while(!ifs.eof())
	{
		getline(ifs,line);
		if(line.length()) applications.push_back(line);
	}
	ifs.close();

	//Determine longest app and alias names for column formatting
	Config cfg = Config();
	long unsigned int longestNameLength = 0;
	long unsigned int longestAliasLength = 0;
	std::string appname;
	for(long unsigned int i=0; i<applications.size(); i++)
	{
		std::string app = applications[i];
		appname = right(app,prefix.length());
		if(appname.length() > longestNameLength)
			longestNameLength = appname.length();

		std::string aliases = "";
		for(const auto& [k,v] : getSet(cfg.setting[ALIASES]))
			if(!stringcasecmp(v,appname)) aliases += k;

		if(aliases.length() > longestAliasLength)
			longestAliasLength = aliases.length();
	}

	//Print results
	fprintf(stdout,"%s%sName%s%sAlias Pattern%s%sDescription\n",TEXT_BOLD,TEXT_GREEN,TEXT_MAGENTA,addSpaces(longestNameLength-4+COLUMN_PADDING).c_str(),TEXT_NORMAL,addSpaces(longestAliasLength-13+COLUMN_PADDING).c_str());
	fprintf(stdout,"%s%s",TEXT_BOLD,TEXT_WHITE);
	for(unsigned long int i=0; i<longestNameLength+longestAliasLength+(2*COLUMN_PADDING)+25; i++) fprintf(stdout,"─");//25 is arbitrary, just looks fine
	fprintf(stdout,"%s\n",TEXT_NORMAL);
	for(const auto& app : applications)
	{
		appname = right(app,prefix.length());
		std::string appdescription = getAppOutput(appname+" "+FLAG_APPDESCRIPTION).output[0];

		//Print application name, aliases (if applicable), and the description
		fprintf(stdout,"%s%s%s%s",TEXT_BOLD,TEXT_GREEN,appname.c_str(),TEXT_NORMAL);
		std::string aliases = "";
		for(const auto& [k,v] : getSet(cfg.setting[ALIASES]))
			if(!stringcasecmp(v,appname)) aliases += (k+",");
		if(aliases != "") aliases = left(aliases,aliases.length()-1);
		fprintf(stdout,"%s%s%s%s%s",TEXT_BOLD,TEXT_MAGENTA,addSpaces(longestNameLength-appname.length()+COLUMN_PADDING).c_str(),aliases.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%s",addSpaces(longestAliasLength-aliases.length()+COLUMN_PADDING).c_str());

		//Description should really be just one line anyways
		if(appdescription != "")
			fprintf(stdout,"%s\n",appdescription.c_str());
		else
			fprintf(stdout,"%s\n",empty_str.c_str());

		if(print_app_help)
		{
			std::vector<std::string>apphelp = getAppOutput(appname+" --help").output;

			fprintf(stdout,"\n");
			for(const auto& line : apphelp)
				fprintf(stdout,"%s%s\n",addSpaces(longestNameLength+COLUMN_PADDING).c_str(),line.c_str());
		}
	}

	if(print_app_help) fprintf(stdout,"\b"); //Remove extraneous newline

	return 0;
}
