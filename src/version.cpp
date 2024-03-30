#include <cstring>
#include <stdio.h>
#include "../headers/define.h"

int main(int argc, char** argv)
{
	if(!strcmp(argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"Prints version information.\n");
		return 0;
	}
	fprintf(stdout,"Version %d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);

	return 0;
}
