#include <cstring>
#include <stdio.h>
#include "../headers/define.h"
#include "../headers/functions.h"

int main(int argc, char** argv)
{
	check_print_app_description(argv,"Prints version information.");

	fprintf(stdout,"Version %d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);

	return 0;
}
