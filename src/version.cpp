#include <cstring>
#include <stdio.h>
#include "../headers/chkflag.h"
#include "../headers/define.h"
#include "../headers/text.h"
#include "../headers/functions.h"
#include "../headers/output.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"version only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Prints version information.");
	chkFlagModifyVariables(argv,false);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tversion [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints version information.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-M\t\tPrints major version.\n");
		fprintf(stdout,"\t-m\t\tPrints minor version.\n");
		fprintf(stdout,"\t-p\t\tPrints patch version.\n");
		fprintf(stdout,"\t-v\t\tPrints version information sans \"Version \".\n");
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	if(argv[1] && !strcmp(argv[1],"-M"))
		fprintf(stdout,"%d\n",MAJOR_VERSION);
	else if(argv[1] && !strcmp(argv[1],"-m"))
		fprintf(stdout,"%d\n",MINOR_VERSION);
	else if(argv[1] && !strcmp(argv[1],"-p"))
		fprintf(stdout,"%d\n",PATCH_VERSION);
	else if(argv[1] && !strcmp(argv[1],"-v"))
		fprintf(stdout,"%d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);
	else if(argv[1])
		output(Error,"Unknown option: %s\n",argv[1]);
	else
		fprintf(stdout,"Version %d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);

	return 0;
}
