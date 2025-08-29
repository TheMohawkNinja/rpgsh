#include <strings.h>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(warning,"sleep only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Pauses for the specified period of seconds.");
	chkFlagPreserveVariables(argv,none);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tsleep [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tSleep for 1 second.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%sn%s\t\tSleep for %sn%s seconds.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	if(argc == 1)
	{
		sleep(1);
		return 0;
	}

	int seconds = 0;
	try
	{
		seconds = std::stoi(argv[1]);
	}
	catch(...)
	{
		output(error,"Unable to parse \'%s\" as a valid integer.",argv[1]);
		return -1;
	}

	sleep(seconds);

	return 0;
}
