#include "../headers/dice.h"
#include "../headers/functions.h"

int main(int argc, char** argv)
{
	bool only_total = false;
	bool only_rolls = false;
	bool is_list = false;
	unsigned int count = 0;
	unsigned int repeat = 1;
	std::string dice_str, current_arg;
	std::string count_expr = "";

	for(int arg=1; arg<argc; arg++)
	{
		current_arg = std::string(argv[arg]);
		if(current_arg.substr(0,1) == "-")//Assume anything starting with a "-" is a non-dice parameter
		{
			if(current_arg == "-t" || current_arg == "--test")
			{
				fprintf(stdout,"Initiating roll test...\n\n");
				RPGSH_DICE dice = RPGSH_DICE();
				dice.test();
				return 0;
			}
			else if(current_arg == "-r" || current_arg == "--repeat")
			{
				if(!argv[arg+1])
				{
					output(Error,"No repeat value specified.");
					return -1;
				}
				else
				{
					try
					{
						repeat = std::stoi(argv[arg+1]);

						if(repeat < 1)
						{
							output(Error,"Repeat value must be greater than 0.");
							return -1;
						}
					}
					catch(...)
					{
						output(Error,"Can't parse repeat value. \"%s\" is not a number.",argv[arg+1]);
						return -1;
					}
				}
				arg++;
			}
			else if(current_arg == "-l" || current_arg == "--list")
			{
				if(!argv[arg+1])
				{
					output(Error,"No path specified.");
					return -1;
				}
				dice_str = argv[arg+1];
				is_list = true;
				arg++;
			}
			else if(current_arg == "-c" || current_arg == "--count")
			{
				if(!argv[arg+1])
				{
					output(Error,"No count expression specified.");
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
						output(Error,"Invalid count expression in argument \"%s\".",argv[arg+1]);
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
								output(Error,"Invalid count amount \"%s\".",count_str.c_str());
								return -1;
							}
						}
						count = std::stoi(count_str);
					}
					catch(...)
					{
						output(Error,"Invalid count amount \"%s\".",count_str.c_str());
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
				fprintf(stdout,"\troll [%sc%s]d%sf%s[+%sm%s|-%sm%s] [-r %sn%s] [--only-rolls|--only-total]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\troll [-l|--list] %slist%s [-r %sn%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\troll [-t|--test]\n");
				fprintf(stdout,"\troll [-?|--help]\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"\t%sc%s			The number of dice being rolled.\n\t\t\t\tSet to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t%sf%s			The number of faces on each die being rolled.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t%sm%s			The modifier for the total roll.\n",TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t-r %sn%s|--repeat %sn%s		Repeat the roll %sn%s times.\n\t\t\t\tSet to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t-l %slist%s|--list %slist%s	Rolls a die with custom faces sourced from %slist%s (newline delimited).\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t\t\t\tIf %slist%s does not start with a \'/\', \'./\', or \'../\' roll will check the \"%s\" directory.\n",TEXT_ITALIC,TEXT_NORMAL,dice_lists_dir.c_str());
				fprintf(stdout,"\t\t\t\tOtherwise it will check the path specified by the user.\n");
				fprintf(stdout,"\t-c %sexpr%s|--count %sexpr%s	Count how many dice rolls meet a criteria defined by a boolean expression (=,<,>,<=,>=,!=) and a numerical value.\n\t\t\t\tIf the boolean expression is omitted, it is assumed to be \"=\".\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
				fprintf(stdout,"\t--only-rolls		Only display dice rolls.\n");
				fprintf(stdout,"\t--only-total		Only display total of the roll.\n");
				fprintf(stdout,"\t-t|--test		Rolls 100000d20 and displays results with percent deviation from perfect distribution.\n");
				fprintf(stdout,"\t-h|--help		Displays help text.\n");
				fprintf(stdout,"\n");
				fprintf(stdout,"EXAMPLES:\n");
				fprintf(stdout,"\troll 4d8+3			Rolls an 8-sided die 4 times, then adds 3 to the result.\n");
				fprintf(stdout,"\troll 3d6+2 -r 5			Rolls a 6-sided die 3 times, then adds 2 to the result. Repeats 5 times.\n");
				fprintf(stdout,"\troll -l chaos-burst-2.0		Rolls a dice with faces derived from the lines in the file \".%s\".\n",(dice_lists_dir+"chaos-burts-2.0").c_str());
				fprintf(stdout,"\troll 10d20 -c >=15		Rolls a 20-sided die 10 times and counts the number of rolls greater than or equal to 15.\n");
				fprintf(stdout,"\troll 2d10-5 --only-total	Rolls a 10-sided die twice, then subtracts 5 to the result. Only displays the total of the result.\n");
				fprintf(stdout,"\troll --test			Initiates a roll test.\n");
				fprintf(stdout,"\troll -?				Displays this help text.\n");
				return 0;
			}
			else
			{
				output(Warning,"Unknown option \"%s\".",current_arg.c_str());
			}
		}
		else
		{
			if(!is_list)
			{
				dice_str = current_arg;
			}
		}
	}

	RPGSH_DICE dice = RPGSH_DICE(dice_str,only_rolls,only_total,is_list,count_expr,count);

	if(is_list)
	{
		for(int i=0; i<repeat; i++)
		{
			fprintf(stdout,"%s",(repeat > 1 && i > 0) ? "\n" : "");
			dice.roll();
		}
	}

	for(int i=0; i<repeat && dice.Quantity>0; i++)
	{
		fprintf(stdout,"%s",(repeat > 1 && i > 0) ? "\n" : "");
		dice.roll();
	}

	return 0;
}
