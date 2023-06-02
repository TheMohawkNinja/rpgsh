#include "classes.h"

int main(int argc, char** argv)
{
	unsigned int repeat = 1;
	std::string dice_str, current_arg;
	DNDSH_CHARACTER c = DNDSH_CHARACTER();//TODO This will eventually load the character defined in argv[1]

	for(int arg=2; arg<argc; arg++)
	{
		current_arg = std::string(argv[arg]);
		if(current_arg.substr(0,1) == "-")//Assume anything starting with a "-" is a non-dice parameter
		{
			if(current_arg == "-t")
			{
				fprintf(stdout,"Initiating roll test...\n\n");
				DNDSH_DICE dice = DNDSH_DICE();
				dice.test();
				return 0;
			}
			else if(current_arg == "-r")
			{
				if(!argv[arg+1])
				{
					fprintf(stderr,"%s%sERROR: No repeat value specified.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
					return -1;
				}
				else
				{
					try
					{
						repeat = std::strtol(argv[arg+1],NULL,10);
						arg++;
	
						if(repeat < 1)
						{
							fprintf(stderr,"%s%sERROR: Repeat value must be greater than 0.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
							return -1;
						}
					}
					catch(...)
					{
						fprintf(stderr,"%s%sERROR: Can't parse repeat value. \"%s\" is not a number.%s\n",TEXT_RED,TEXT_BOLD,argv[arg+1],TEXT_NORMAL);
						return -1;
					}
				}
			}
			else if(current_arg == "-h")
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
			else
			{
				fprintf(stderr,"%s%sWARNING: Unknown option \"%s\".%s\n",TEXT_YELLOW,TEXT_BOLD,current_arg.c_str(),TEXT_NORMAL);
			}
		}
		else
		{
			dice_str = current_arg;
		}
	
	}

	DNDSH_DICE dice = DNDSH_DICE(dice_str);

	for(int i=0; i<repeat; i++)
	{
		dice.roll();
	}
}
