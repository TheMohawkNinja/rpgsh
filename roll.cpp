#include "classes.h"

int main(int argc, char** argv)
{
	for(int i=0; i<argc; i++)
	{
		fprintf(stdout,"Arg[%d]: %s\n",i,argv[i]);
	}
	DNDSH_DICE dice = DNDSH_DICE(argv[2]);
	dice.roll();
}
