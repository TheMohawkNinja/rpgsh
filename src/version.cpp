#include <stdio.h>
#include "../headers/define.h"

int main()
{
	fprintf(stdout,"Version %d.%d.%d\n",MAJOR_VERSION,MINOR_VERSION,PATCH_VERSION);

	return 0;
}
