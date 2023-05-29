#include "classes.h"

int main(int argc, char** argv)
{
	DNDSH_DICE dice = DNDSH_DICE(argv[2]);
	dice.roll();
}
