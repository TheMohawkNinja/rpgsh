#include <string>
#include <cstring>
#include "../headers/configuration.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(warning,"help only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Prints this helpful list of applications and what they do.");
	chkFlagPreserveVariables(argv,none);

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
	Configuration cfg = Configuration();
	long unsigned longest_name_ln = 0;
	long unsigned longest_alias_ln = 0;
	std::string appname;
	for(long unsigned i=0; i<applications.size(); i++)
	{
		std::string app = applications[i];
		appname = right(app,prefix.length());
		if(appname.length() > longest_name_ln)
			longest_name_ln = appname.length();

		std::string aliases = "";
		for(const auto& [k,v] : getSet(cfg.setting[ALIASES]))
			if(!stringcasecmp(v,appname)) aliases += k;

		if(aliases.length() > longest_alias_ln)
			longest_alias_ln = aliases.length();
	}

	//Print results
	std::string name_header_str = "Name";
	std::string aliases_header_str = "Alias(es)";
	unsigned char name_spacing = (longest_name_ln > name_header_str.length() ? longest_name_ln-name_header_str.length() : longest_name_ln);
	unsigned char alias_spacing = (longest_alias_ln > aliases_header_str.length() ? longest_alias_ln-aliases_header_str.length() : longest_alias_ln);
	fprintf(stdout,"%s%s%s%s%s%s%s%sDescription\n",
		TEXT_BOLD,
		TEXT_GREEN,
		name_header_str.c_str(),
		TEXT_MAGENTA,
		addSpaces(name_spacing+COLUMN_PADDING).c_str(),
		aliases_header_str.c_str(),
		TEXT_NORMAL,
		addSpaces(alias_spacing+COLUMN_PADDING).c_str());
	fprintf(stdout,"%s%s",TEXT_BOLD,TEXT_WHITE);
	for(long unsigned i=0; i<longest_name_ln+longest_alias_ln+(2*COLUMN_PADDING)+25; i++)//25 is arbitrary, just looks fine
		fprintf(stdout,"─");
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
		fprintf(stdout,"%s%s%s%s%s",TEXT_BOLD,TEXT_MAGENTA,addSpaces(name_spacing-appname.length()+COLUMN_PADDING+name_header_str.length()).c_str(),aliases.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%s",addSpaces(alias_spacing-aliases.length()+COLUMN_PADDING+aliases_header_str.length()).c_str());

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
				fprintf(stdout,"%s%s\n",addSpaces(longest_name_ln+COLUMN_PADDING).c_str(),line.c_str());
		}
	}

	if(print_app_help) fprintf(stdout,"\b"); //Remove extraneous newline

	return 0;
}
