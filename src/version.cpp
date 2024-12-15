#include <cstring>
#include <stdio.h>
#include "../headers/define.h"
#include "../headers/functions.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"version only expects 0 or 1 arguments, ignoring all other arguments.");

	check_print_app_description(argv,"Prints version information.");

	if(argv[1] && (!strcmp(argv[1],"-M") || !strcmp(argv[1],"--major")))
	{
		fprintf(stdout,"%d\n",MAJOR_VERSION);
		return 0;
	}
	else if(argv[1] && (!strcmp(argv[1],"-m") || !strcmp(argv[1],"--minor")))
	{
		fprintf(stdout,"%d\n",MINOR_VERSION);
		return 0;
	}
	else if(argv[1] && (!strcasecmp(argv[1],"-p") || !strcasecmp(argv[1],"--patch")))
	{
		fprintf(stdout,"%d\n",PATCH_VERSION);
		return 0;
	}
	else if(argv[1] && (!strcasecmp(argv[1],"-v") || !strcasecmp(argv[1],"--version")))
	{
		fprintf(stdout,"%d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);
		return 0;
	}
	else if(argv[1] && (!strcasecmp(argv[1],"-?") || !strcasecmp(argv[1],"--help")))
	{
		fprintf(stdout,"Prints version information.\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tversion\t\t\tPrints version information.\n");
		fprintf(stdout,"\tversion [-M|--major]\tPrints major version.\n");
		fprintf(stdout,"\tversion [-m|--minor]\tPrints minor version.\n");
		fprintf(stdout,"\tversion [-p|--patch]\tPrints patch version.\n");
		fprintf(stdout,"\tversion [-v|--version]\tPrints version information sans \"Version \".\n");
		fprintf(stdout,"\tversion [-?|--help]\tPrints this help text.\n");
		return 0;
	}

	fprintf(stdout,"Version %d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);
	return 0;
}
