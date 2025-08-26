#include <string.h>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(warning,"import only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Runs an rpgsh script.");
	chkFlagPreserveVariables(argv,none);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\timport [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%sscript%s\t\tRuns the script %sscript%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	if(argc == 1)
	{
		output(error,"No rpgsh script specified.");
		return -1;
	}

	return runScript(std::string(argv[1]));
}
