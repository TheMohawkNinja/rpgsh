#include <strings.h>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"clear only expects 0 or 1 arguments, ignoring all other arguments.");

	chkPrntAppDesc(argv,"Clears the screen.");

	if(argv[1] && (!strcasecmp(argv[1],"-?") || !strcasecmp(argv[1],"--help")))
	{
		fprintf(stdout,"Clears the screen.\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tclear\t\t\tClears the screen.\n");
		fprintf(stdout,"\tclear [-?|--help]\tPrints this help text.\n");
		return 0;
	}

	fprintf(stdout,CLEAR_SCREEN);
	return 0;
}
