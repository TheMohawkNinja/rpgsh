#include <cstring>
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/text.h"

void test()
{
	std::string plus = std::string(TEXT_WHITE)+"+";
	unsigned int Quantity = 100000;
	unsigned int Faces = 20;
	fprintf(stdout,"Initiating roll test of %s%dd%d%s...\n",TEXT_WHITE,Quantity,Faces,TEXT_NORMAL);
	Dice dice = Dice(Quantity,Faces,0);
	RollResults results = dice.roll();
	std::vector<int> face_roll_ctr;
	for(unsigned int i=0; i<Faces; i++) face_roll_ctr.push_back(0);//Need to init vector before we can assign values
	for(const auto& r : results) face_roll_ctr[std::stoi(r.first)-1]++;
	fprintf(stdout,"\n");
	fprintf(stdout,"\t┌───────────────────────────────────────┐\n");
	fprintf(stdout,"\t│ %s%s%sTEST RESULTS:%s                         │\n",TEXT_ITALIC,TEXT_BOLD,TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"\t│                                       │\n");
	for(unsigned int i=0; i<Faces; i++)
	{
		float percent = ((float)face_roll_ctr[i]/((float)Quantity/(float)Faces))*100-100;
		fprintf(stdout,"\t│ # of %s%d's: %d (%s%s%.2f%%%s from perfect) │\n",(i<9)?" ":"",(i+1),face_roll_ctr[i],(percent>=0)?plus.c_str():"",TEXT_WHITE,percent,TEXT_NORMAL);
	}
	fprintf(stdout,"\t└───────────────────────────────────────┘\n");
}
int main(int argc, char** argv)
{
	bool only_total = false;
	bool only_rolls = false;
	unsigned int count = 0;
	unsigned int repeat = 1;
	std::string dice_str, current_arg;
	std::string count_expr = "";

	chkFlagAppDesc(argv,"A simple, yet featureful dice rolling program.");
	chkFlagPreserveVariables(argv,none);

	if(argc == 1)
	{
		output(error,"roll expects at least one argument.");
		exit(-1);
	}

	for(int arg=1; arg<argc; arg++)
	{
		current_arg = std::string(argv[arg]);
		if(chkFlagHelp(argv))
		{
			fprintf(stdout,"USAGE:\n");
			fprintf(stdout,"\troll %sdice%s [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
			fprintf(stdout,"\nOPTIONS:\n");
			fprintf(stdout,"\t-r %sn%s\t\tRepeat the roll %sn%s times. Set to 1 if omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
			fprintf(stdout,"\t-l\t\tPrints all of the available dice lists.\n");
			fprintf(stdout,"\t-c %sexpr n%s\tCount rolls meeting a criteria defined by a boolean expression %sexpr%s and a number %sn%s.\n\t\t\tExpressions supported: %s, %s, %s, %s, %s, %s\n\t\t\tIf %sexpr%s is omitted, it assumes \'%s\'.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,OP_EQ,OP_LT,OP_GT,OP_LE,OP_GE,OP_NE,TEXT_ITALIC,TEXT_NORMAL,OP_EQ);
			fprintf(stdout,"\t--only-rolls\tOnly display dice rolls.\n");
			fprintf(stdout,"\t--only-total\tOnly display total of the roll. When used with a list, only the list item is shown.\n");
			fprintf(stdout,"\t-t\t\tRolls 100000d20 and displays results with percent deviation from perfect distribution.\n");
			fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
			fprintf(stdout,"\nEXAMPLES:\n");
			fprintf(stdout,"\troll d20\t\tRolls a 20-sided die.\n");
			fprintf(stdout,"\troll 4d8+3\t\tRolls four 8-sided dice, then adds 3 to the result.\n");
			fprintf(stdout,"\troll 3d6+2 -r 5\t\tRolls three 6-sided dice and adds 2 to the result. Repeats 5 times.\n");
			fprintf(stdout,"\troll chaos-burst-2.0\tRolls a dice using values from \"%s\".\n",(dice_lists_dir+"chaos-burst-2.0").c_str());
			fprintf(stdout,"\troll -l\t\t\tPrints dice lists.\n");
			fprintf(stdout,"\troll 10d20 -c >= 15\tRolls ten 20-sided dice and counts the rolls greater than or equal to 15.\n");
			fprintf(stdout,"\troll d8 --only-total\tRolls an 8-sided die, only displaying the total of the result.\n");
			fprintf(stdout,"\troll -t\t\t\tInitiates a roll test.\n");
			fprintf(stdout,"\troll %s\t\t\tPrints help text.\n",FLAG_HELPSHORT);
			return 0;
		}
		else if(current_arg == "-t")
		{
			test();
			return 0;
		}
		else if(current_arg == "-r")
		{
			if(!argv[arg+1])
			{
				output(error,"No repeat value specified.");
				return -1;
			}
			else if(!Var(argv[arg+1]).isInt())
			{
				output(error,"Can't parse repeat value. \"%s\" is not a number.",argv[arg+1]);
				return -1;
			}
			else if(Var(argv[arg+1]) < 1)
			{
				output(error,"Repeat value must be greater than 0.");
				return -1;
			}

			repeat = std::stoi(argv[arg+1]);
			arg++;
		}
		else if(current_arg == "-l")
		{
			std::vector<std::string> lists = getDirectoryListing(dice_lists_dir);
			for(const auto& list : lists)
				fprintf(stdout,"%s\n",list.c_str());
			return 0;
		}
		else if(current_arg == "-c")
		{
			if(!argv[arg+1])
			{
				output(error,"No count expression specified.");
				return -1;
			}

			count_expr = "";
			std::string count_str = "";

			try
			{
				count_str = argv[arg+1];
				count = std::stoi(count_str);
				count_expr = OP_EQ;
				arg++;
				continue;
			}
			catch(...)
			{
				count_str = "";
				std::string valid_expr[6] = {OP_LE, OP_GE, OP_NE, OP_EQ, OP_LT, OP_GT};

				for(const auto& expr : valid_expr)
				{
					if(std::string(argv[arg+1]) == expr)
					{
						count_expr = expr;
						break;
					}
				}

				if(count_expr == "")
				{
					output(error,"Invalid count expression in argument \"%s\".",argv[arg+1]);
					return -1;
				}
			}

			if(count == 0)
			{
				if(!argv[arg+2])
				{
					output(error,"No count quantity specified.");
					return -1;
				}

				count_str = std::string(argv[arg+2]);
				if(!Var(count_str).isInt())
				{
					output(error,"Invalid count amount \"%s\".",count_str.c_str());
					return -1;
				}

				count = std::stoi(count_str);
			}

			arg+=2;
		}
		else if(current_arg == "--only-rolls")
		{
			only_rolls = true;
		}
		else if(current_arg == "--only-total")
		{
			only_total = true;
		}
		else
		{
			dice_str = current_arg;
		}
	}

	Dice dice;
	try
	{
		dice = Dice(dice_str);
	}
	catch(const std::runtime_error& e)
	{
		if(e.what() == std::string(E_INVALID_EXPLICIT_CONSTRUCTOR))
			output(error,"Invalid explicit dice constructor: \"%s\".",dice_str.c_str());
		else
			output(error,"Error when attempting to construct dice object with \"%s\": %s.",dice_str.c_str(),e.what());
		exit(-1);
	}

	if(!only_rolls && !only_total && dice.List == "")
		fprintf(stdout,"Rolling a %s%d%s-sided die %s%d%s time(s) with a modifier of %s%d%s...\n\n",TEXT_WHITE,dice.Faces,TEXT_NORMAL,TEXT_WHITE,dice.Quantity,TEXT_NORMAL,TEXT_WHITE,dice.Modifier,TEXT_NORMAL);
	else if(!only_rolls && !only_total)
		fprintf(stdout,"Getting %s from list \"%s%s%s\"...\n\n",repeat > 1 ? (std::string(TEXT_WHITE)+std::to_string(repeat)+std::string(TEXT_NORMAL)+" values").c_str() : "a value",TEXT_WHITE,dice.List.c_str(),TEXT_NORMAL);

	RollResults results;
	for(unsigned int i=0; i<repeat; i++)
	{
		try
		{
			results = dice.roll();
		}
		catch(const std::runtime_error& e)
		{
			if(e.what() == std::string(E_NO_PRNG_FILE))
				output(error,"Cannot seed pRNG for dice roll. \"%s\" and \"%s\" do not exist. If your system uses a different path for pRNG, please report your system's pRNG file path to the rpgsh GitHub issues page.",prng_path,backup_prng_path);
			else if(e.what() == std::string(E_FILE_NOT_FOUND))
				output(error,"List file not found at \"%s\".",(dice_lists_dir+dice.List).c_str());
			else if(e.what() == std::string(E_BAD_FS))
				output(error,"Unable to read list file at \"%s\".",(dice_lists_dir+dice.List).c_str());
			else
				output(error,"An error occured when attempting to roll \"%s\": %s.",dice_str.c_str(),e.what());
			exit(-1);
		}

		fprintf(stdout,"%s",(repeat > 1 && i > 0) ? "\n" : "");

		if(dice.List != "")
		{
			if(!only_total && !only_rolls)
				fprintf(stdout,"[Roll result: %s%ld%s]\n",TEXT_WHITE,std::stol(results[0].first),TEXT_NORMAL);
			if(!only_rolls)
				fprintf(stdout,"%s\n",results[0].second.c_str());
			else
				fprintf(stdout,"%ld\n",std::stol(results[0].first));

			continue;
		}

		long total = 0;
		long total_count = 0;
		long roll = 0;
		for(const auto& r : results)
		{
			std::string nat_roll = r.first;
			std::string mod_roll = r.second;
			roll++;
			total += std::stol(nat_roll);

			if(count_expr != "" &&
			  ((count_expr == OP_EQ && std::stol(mod_roll) == count) ||
			  (count_expr == OP_LT && std::stol(mod_roll) <  count) ||
			  (count_expr == OP_GT && std::stol(mod_roll) >  count) ||
			  (count_expr == OP_LE && std::stol(mod_roll) <= count) ||
			  (count_expr == OP_GE && std::stol(mod_roll) >= count) ||
			  (count_expr == OP_NE && std::stol(mod_roll) != count)))
				total_count++;

			std::string color;
			if     (std::stol(nat_roll) == 1)		color = TEXT_RED;
			else if(std::stol(nat_roll) == dice.Faces)	color = TEXT_GREEN;
			else						color = TEXT_WHITE;

			if(only_rolls)
			{
				fprintf(stdout,"%ld\n",std::stol(mod_roll));
			}
			else if(!only_total)
			{
				std::string q_str = std::to_string(dice.Quantity);
				std::string roll_str = std::to_string(roll);
				std::string f_str = std::to_string(dice.Faces);
				fprintf(stdout,"Roll");
				fprintf(stdout,"%s",addSpaces(q_str.length()-roll_str.length()+1).c_str());
				fprintf(stdout,"%ld: ",roll);
				fprintf(stdout,"%s%ld%s\n",color.c_str(),std::stol(nat_roll),TEXT_NORMAL);
			}
		}
		if(!only_rolls && !only_total)
		{
			fprintf(stdout,"\n");
			fprintf(stdout,"Natural total:\t%s%ld%s\n",TEXT_WHITE,total,TEXT_NORMAL);
			fprintf(stdout,"Modifier:\t%s%d%s\n",TEXT_WHITE,dice.Modifier,TEXT_NORMAL);
			fprintf(stdout,"Total:\t\t%s%ld%s\n",TEXT_WHITE,total+dice.Modifier,TEXT_NORMAL);

			if(count_expr != "")
			{
				fprintf(stdout,"\n");
				fprintf(stdout,"Rolls %s %d:\t%s%ld%s\n",count_expr.c_str(),count,TEXT_WHITE,total_count,TEXT_NORMAL);
			}
		}
		else if(only_total)
		{
			total += dice.Modifier;
			fprintf(stdout,"%ld\n",total);
		}
	}

	return 0;
}
