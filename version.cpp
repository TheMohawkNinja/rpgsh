#include <stdio.h>

int main()
{
	const unsigned char major = 0;
	const unsigned char minor = 5;
	const unsigned char patch = 7;
	fprintf(stdout,"Version %d.%d.%d\n",major,minor,patch);

	return 0;
}
