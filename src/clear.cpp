#include <strings.h>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(warning,"clear only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Clears the screen.");
	chkFlagPreserveVariables(argv,none);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tclear [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tClears the screen.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	fprintf(stdout,CLEAR_SCREEN);
	return 0;
}
