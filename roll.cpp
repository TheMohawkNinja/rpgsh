#include <filesystem>
#include "classes.h"

int main(int argc, char** argv)
{
	bool only_total = false;
	bool only_rolls = false;
	bool is_list = false;
	unsigned int count = 0;
	unsigned int repeat = 1;
	std::string dice_str, current_arg;
	std::string count_expr = "";

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
						repeat = std::stoi(argv[arg+1],NULL,10);
	
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
				arg++;
			}
			else if(current_arg == "-l" || current_arg == "--list")
			{
				if(!argv[arg+1])
				{
					fprintf(stderr,"%s%sERROR: No path specified.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
					return -1;
				}
				else if(std::filesystem::exists(argv[arg+1]))
				{
					dice_str = argv[arg+1];
					is_list = true;
				}
				else
				{
					fprintf(stderr,"%s%sERROR: File \"%s\" not found.%s\n",TEXT_RED,TEXT_BOLD,argv[arg+1],TEXT_NORMAL);
					return -1;
				}
			}
			else if(current_arg == "-c" || current_arg == "--count")
			{
				if(!argv[arg+1])
				{
					fprintf(stderr,"%s%sERROR: No count expression specified.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
					return -1;
				}

				count_expr = "";
				std::string count_str = "";

				try
				{
					count_str = argv[arg+1];
					count = std::stoi(count_str);
					count_expr = "=";
				}
				catch(...)
				{
					count_str = "";
					std::string valid_expr[6] = {"<=", ">=", "!=", "=", "<", ">"};

					for(int i=0; valid_expr[i] != ""; i++)
					{
						if(std::string(argv[arg+1]).substr(0,valid_expr[i].length()) == valid_expr[i])
						{
							count_expr = valid_expr[i];
							break;
						}
					}

					if(count_expr == "")
					{
						fprintf(stderr,"%s%sERROR: Invalid count expression in argument \"%s\".%s\n",TEXT_RED,TEXT_BOLD,argv[arg+1],TEXT_NORMAL);
						return -1;
					}
				}

				if(count == 0)
				{
					try
					{
						for(int i=count_expr.length(); i<std::string(argv[arg+1]).length(); i++)
						{
							count_str += std::string(argv[arg+1]).substr(i,1);
							if(!std::isdigit(count_str[count_str.length()-1]))
							{
								fprintf(stderr,"%s%sERROR: Invalid count amount \"%s\".%s\n",TEXT_RED,TEXT_BOLD,count_str.c_str(),TEXT_NORMAL);
								return -1;
							}
						}
						count = std::stoi(count_str);
					}
					catch(...)
					{
						fprintf(stderr,"%s%sERROR: Invalid count amount \"%s\".%s\n",TEXT_RED,TEXT_BOLD,count_str.c_str(),TEXT_NORMAL);
						return -1;
					}
				}
				arg++;
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
				fprintf(stdout,"A simple dice rolling program.\n\n");
				fprintf(stdout,"USAGE:\n");
				fprintf(stdout,"\troll [%sc%s]d%sf%s[+%sm%s|-%sm%s] [-r %sn%s] [-l %spath%s] [--only-rolls|--only-total]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\troll [-t|--test]\n");
				fprintf(stdout,"\troll [-?|--help]\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"\t%sc%s			The number of dice being rolled.\n\t\t\t\tSet to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t%sf%s			The number of faces on each die being rolled.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t%sm%s			The modifier for the total roll.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t-r %sn%s|--repeat %sn%s		Repeat the roll %sn%s times.\n\t\t\t\tSet to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t-l %spath%s|--list %spath%s	Rolls a die with custom faces sourced from %spath%s (newline delimited).\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t-c %sexpr%s|--count %sexpr%s	Count how many dice rolls meet a criteria defined by a boolean expression (=,<,>,<=,>=,!=) and a numerical value.\n\t\t\t\tIf the boolean expression is omitted, it is assumed to be \"=\".\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t--only-rolls		Only display dice rolls.\n");
				fprintf(stdout,"\t--only-total		Only display total of the roll.\n");
				fprintf(stdout,"\t-t|--test		Rolls 100000d20 and displays results with percent deviation from perfect distribution.\n");
				fprintf(stdout,"\t-h|--help		Displays help text.\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"EXAMPLES:\n");
				fprintf(stdout,"\troll 4d8+3			Rolls an 8-sided die 4 times, then adds 3 to the result.\n");
				fprintf(stdout,"\troll 3d6+2 -r 5			Rolls a 6-sided die 3 times, then adds 2 to the result. Repeat this 5 times.\n");
				fprintf(stdout,"\troll -l ./mylist		Rolls a dice with faces derived from the lines in the file \"./mylist\".\n");
				fprintf(stdout,"\troll 10d20 -c >=15		Rolls a 20-sided die 10 times and counts the number of rolls greater than or equal to 15.\n");
				fprintf(stdout,"\troll 2d10-5 --only-total	Rolls a 10-sided die twice, then subtracts 5 to the result. Only displays the total of the result.\n");
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

	DNDSH_DICE dice = DNDSH_DICE(dice_str,only_rolls,only_total,is_list,count_expr,count);

	for(int i=0; i<repeat; i++)
	{
		fprintf(stdout,"%s",(repeat > 1 && i > 0) ? "\n" : "");
		dice.roll();
	}
}
