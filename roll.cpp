#include "classes.h"

int main(int argc, char** argv)
{
	bool only_total = false;
	bool only_rolls = false;
	unsigned int repeat = 1;
	std::string dice_str, current_arg;
	DNDSH_CHARACTER c = DNDSH_CHARACTER();//TODO This will eventually load the character defined in argv[1]

	for(int arg=2; arg<argc; arg++)
	{
		current_arg = std::string(argv[arg]);
		if(current_arg.substr(0,1) == "-")//Assume anything starting with a "-" is a non-dice parameter
		{
			if(current_arg == "-t" || current_arg == "--test")
			{
				fprintf(stdout,"Initiating roll test...\n\n");
				DNDSH_DICE dice = DNDSH_DICE();
				dice.test();
				return 0;
			}
			else if(current_arg == "-r" || current_arg == "--repeat")
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
			else if(current_arg == "--only-rolls")
			{
				only_rolls = true;
			}
			else if(current_arg == "--only-total")
			{
				only_total = true;
			}
			else if(current_arg == "-?" || current_arg == "--help")
			{
				fprintf(stdout,"USAGE:\n");
				fprintf(stdout,"\troll [%sc%s]d%sf%s[+%sm%s|-%sm%s] [-r %sn%s] [--only-rolls|--only-total]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\troll [-t|--test]\n");
				fprintf(stdout,"\troll [-?|--help]\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"\t%sc%s			Optional. The number of dice being rolled. Set to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t%sf%s			The number of faces on each die being rolled.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t%sm%s			Optional. The modifier for the total roll.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t-r %sn%s|--repeat %sn%s		Optional. Repeat the roll %sn%s times. Set to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t--only-rolls		Only display dice rolls.\n");
				fprintf(stdout,"\t--only-total		Only display total of the roll.\n");
				fprintf(stdout,"\t-t|--test		Rolls 100000d20 and displays results with percent deviation from perfect distribution.\n");
				fprintf(stdout,"\t-h|--help		Displays help text.\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"EXAMPLES:\n");
				fprintf(stdout,"\troll 4d8+3			Rolls an 8-sided die 4 times, then adds 3 to the result.\n");
				fprintf(stdout,"\troll 2d10+5 --only-total	Rolls a 10-sided die twice, then adds 5 to the result. Only displays the total of the result.\n");
				fprintf(stdout,"\troll --test			Initiates a roll test.\n");
				fprintf(stdout,"\troll -?				Displays this help text.\n");
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

	DNDSH_DICE dice = DNDSH_DICE(dice_str,only_rolls,only_total);

	for(int i=0; i<repeat; i++)
	{
		dice.roll();
	}
}
