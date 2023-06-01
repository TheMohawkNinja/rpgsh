#include "classes.h"

int main(int argc, char** argv)
{
	if(std::string(argv[2]) == "-t" || std::string(argv[2]) == "--test")
	{
		fprintf(stdout,"Initiating roll test...\n\n");
		DNDSH_DICE dice = DNDSH_DICE();
		dice.test();
	}
	else
	{
		DNDSH_DICE dice = DNDSH_DICE(argv[2]);
		dice.roll();
	}
}
