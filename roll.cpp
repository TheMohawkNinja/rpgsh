#include "classes.h"
#include <getopt.h>
//#include "text.h"

int main(int argc, char** argv)
{
	unsigned int repeat = 1;
	std::string dice_str;
	DNDSH_CHARACTER c = DNDSH_CHARACTER();//TODO This will eventually load the character defined in argv[1]

	const option long_opts[] = {
		{"test",	no_argument,		nullptr,	't'},
		{"repeat",	required_argument,	nullptr,	'r'},
		{"help",	no_argument,		nullptr,	'h'},
		{0,		0,			0,		0}};

	const auto opt = getopt_long(argc, argv, "trh", long_opts, nullptr);

	while(true)
	{
		fprintf(stdout,"opt = %d\n",opt);
		if(opt == -1){break;}

		switch(opt)
		{
			case 't':
			{
				fprintf(stdout,"Initiating roll test...\n\n");
				DNDSH_DICE dice = DNDSH_DICE();
				dice.test();
				return 0;
			}
			case 'r':
			{
				if(optind == argc)
				{
					fprintf(stderr,"%s%sERROR: No repeat value specified.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
					return -1;
				}
				else
				{
					try
					{
						fprintf(stdout,"argv[%d] = \"%d\"\n",(optind+1),std::strtol(argv[optind+1],NULL,10));
						repeat = std::strtol(argv[optind+1],NULL,10);
						break;

						if(repeat < 1)
						{
							fprintf(stderr,"%s%sERROR: Repeat value must be greater than 0.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
							return -1;
						}
					}
					catch(...)
					{
						fprintf(stderr,"%s%sERROR: Can't parse repeat value. \"%s\" is not a number.%s\n",TEXT_RED,TEXT_BOLD,argv[optind],TEXT_NORMAL);
						return -1;
					}
				}
			}
			case 'h':
			{
				fprintf(stdout,"USAGE:\n");
				fprintf(stdout,"\troll [%sc%s]d%sf%s[+%sm%s|-%sm%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\troll [-t|--test]\n");
				fprintf(stdout,"\troll [-h|--help]\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"%sc%s		Optional. The number of dice being rolled. Dice count set to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"%sf%s		The number of faces on each die being rolled.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"%sm%s		Optional. The modifier for the total roll.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"-t|--test	Rolls 100000d20 and displays results showing rolls per face, along with the percent deviation from perfect distribution.\n");
				fprintf(stdout,"-h|--help	Displays help text.\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"EXAMPLES:\n");
				fprintf(stdout,"\troll 4d8+3	Rolls an 8-sided die 4 times, then adds 3 to the result.\n");
				fprintf(stdout,"\troll -t		Initiates a roll test.\n");
				fprintf(stdout,"\troll --help	Displays this help text.\n");
				return 0;
			}
			default:
				dice_str = argv[optind];
				break;
		}
	}

	DNDSH_DICE dice = DNDSH_DICE(dice_str);

	for(int i=0; i<repeat; i++)
	{
		dice.roll();
	}
}
