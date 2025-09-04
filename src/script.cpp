#include <string.h>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	chkFlagAppDesc(argv,"Runs an rpgsh script.");
	chkFlagPreserveVariables(argv,none);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tscript [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%sscript%s %sargs%s\t\tRuns the script %sscript%s with the option args %sargs%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-l\t\tPrints all of the available scripts in the default folder.\n");
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	if(argc == 1)
	{
		output(error,"No rpgsh script specified.");
		return -1;
	}
	else if(!strcmp(argv[1],"-l"))
	{
		std::vector<std::string> scripts = getDirectoryListing(scripts_dir);
		for(const auto& script : scripts)
			if(!std::filesystem::is_directory(scripts_dir+"/"+script)) fprintf(stdout,"%s\n",script.c_str());
		return 0;
	}

	std::vector<std::string> args;
	args.push_back(std::to_string(argc-2));
	for(int i=2; i<argc; i++) args.push_back(std::string(argv[i]));
	return runScript(std::string(argv[1]),args);
}
