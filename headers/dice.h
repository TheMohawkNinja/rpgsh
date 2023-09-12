#pragma once

#include <climits>
#include <string.h>
#include "functions.h"

class RPGSH_DICE
{
	private:
		bool just_show_rolls = false;
		bool just_show_total = false;
		bool is_list = false;
		unsigned int count = 0;
		unsigned int total_count = 0;
		std::string dice_list = "";
		std::string Quantity_str = "";
		std::string Faces_str = "";
		std::string count_expr = "";
		std::vector<int> result_quantity;

	int get_value(std::string d, std::string value, int start, std::string terminator, bool allow_sign, bool required)
	{
		std::string value_str = "";
		if(terminator != "")
		{
			for(int i=start; d.substr(i,1)!=terminator; i++)
			{
				try
				{
					if(!allow_sign)
					{
						if(d.substr(i,1) == "+" || d.substr(i,1) == "-")
						{
							break;
						}
						std::stoi(d.substr(i,1));
						value_str += d.substr(i,1);
					}
					else
					{
						if(d.substr(i,1) != "+" && d.substr(i,1) != "-")
						{
							std::stoi(d.substr(i,1));
						}
						value_str += d.substr(i,1);
					}
				}
				catch(...)
				{
					output(Error,"Unable to get dice %s. \"%s\" is not a number.",value.c_str(),d.substr(i,1).c_str());
					exit(-1);
				}
			}

			try
			{
				std::stoi(value_str);
				return std::stoi(value_str);
			}
			catch(...)
			{
				output(Error,"Unable to get dice %s. \"%s\" exceeds the maximum size of %d.",value.c_str(),value_str.c_str(),INT_MAX);
				exit(-1);
			}
		}
		else
		{
			if(start == d.length())
			{
				if(required)
				{
					output(Error,"Unable to get dice %s. No %s specified.",value.c_str(),value.c_str());
					exit(-1);
				}
				return 0;
			}
			for(int i=start; i<d.length(); i++)
			{
				try
				{
					if(!allow_sign)
					{
						if(d.substr(i,1) == "+" || d.substr(i,1) == "-")
						{
							break;
						}
						std::stoi(d.substr(i,1));
						value_str += d.substr(i,1);
					}
					else
					{
						if(d.substr(i,1) != "+" && d.substr(i,1) != "-")
						{
							std::stoi(d.substr(i,1));
						}
						value_str += d.substr(i,1);
					}
				}
				catch(...)
				{
					output(Error,"Unable to get dice %s. \"%s\" is not a number.",value.c_str(),d.substr(i,1).c_str());
					exit(-1);
				}
			}

			try
			{
				std::stoi(value_str);
				return std::stoi(value_str);
			}
			catch(...)
			{
				output(Error,"Unable to get dice %s. \"%s\" exceeds the maximum size of %d.",value.c_str(),value_str.c_str(),INT_MAX);
				exit(-1);
			}
		}
	}

	public:
		unsigned int	Quantity	=	0;
		unsigned int	Faces		=	0;
			 int	Modifier	=	0;

		explicit operator std::string() const
		{
			return (std::to_string(Quantity)+"d"+std::to_string(Faces));
		}

	RPGSH_DICE(){}
	RPGSH_DICE(std::string dice_str)
	{
		if(dice_str.substr(0,1) != "d")
		{
			Quantity = get_value(dice_str,"quantity",0,"d",false,true);
			Faces = get_value(dice_str,"faces",dice_str.find("d",0) + 1,"",false,true);
			Modifier = get_value(dice_str,"modifier",dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
		}
		else
		{
			Quantity = 1;
			Faces = get_value(dice_str,"faces",dice_str.find("d",0) + 1,"",false,true);
			Modifier = get_value(dice_str,"modifier",dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
		}
	}
	RPGSH_DICE(unsigned int _Quantity, unsigned int _Faces, int _Modifier)
	{
		Quantity = _Quantity;
		Faces = _Faces;
		Modifier = _Modifier;
		just_show_total = true;
	}
	RPGSH_DICE(std::string dice_str, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count)
	{
		if(!_is_list)
		{
			if(dice_str.substr(0,1) != "d")
			{
				Quantity = get_value(dice_str,"quantity",0,"d",false,true);
				Faces = get_value(dice_str,"faces",dice_str.find("d",0) + 1,"",false,true);
				Modifier = get_value(dice_str,"modifier",dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
			}
			else
			{
				Quantity = 1;
				Faces = get_value(dice_str,"faces",dice_str.find("d",0) + 1,"",false,true);
				Modifier = get_value(dice_str,"modifier",dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
			}
		}
		else
		{
			dice_list = dice_str;
		}

		just_show_rolls = _just_show_rolls;
		just_show_total = _just_show_total;
		is_list = _is_list;
		count_expr = _count_expr;
		count = _count;
	}
	RPGSH_DICE(unsigned int _Quantity, unsigned int _Faces, int _Modifier, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count)
	{
		Quantity = _Quantity;
		Faces = _Faces;
		Modifier = _Modifier;
		just_show_rolls = _just_show_rolls;
		just_show_total = _just_show_total;
		is_list = _is_list;
		count_expr = _count_expr;
		count = _count;
	}

	std::string dice()
	{
		std::string ret = "";
		if(Quantity)
		{
			ret += std::to_string(Quantity);
		}
		ret += "d"+std::to_string(Faces);
		if(Modifier)
		{
			ret += (Modifier>0) ? "+" : "";
			ret += Modifier;
		}
		return ret;
	}
	void roll()
	{
		if(!std::filesystem::exists(random_seed_path))
		{
			if(std::filesystem::exists(backup_random_seed_path))
			{
				random_seed_path = backup_random_seed_path;
			}
			else
			{
				output(Error,"Random seed path file doesn't exist at either \"%s\" or \"%s\". If system corruption is not suspected, please report your system's RNG/pRNG file to the RPGSH Github issues page.",random_seed_path,backup_random_seed_path);
			}
		}
		if(Quantity > 0 && Faces > 0)
		{
			for(int i=0; i<Faces; i++)
			{
				result_quantity.push_back(0);
			}

			if(!just_show_rolls && !just_show_total)
			{
				fprintf(stdout,"Rolling a %s%d%s-sided die %s%d%s time(s) with a modifier of %s%d%s...\n\n",TEXT_WHITE,Faces,TEXT_NORMAL,TEXT_WHITE,Quantity,TEXT_NORMAL,TEXT_WHITE,Modifier,TEXT_NORMAL);
			}

			std::ifstream fs(random_seed_path);
			std::string data, seed;
			int result;

			while(seed.length() < Quantity)
			{
				std::getline(fs,data);
				seed += data;
			}

			int total = 0;
			std::string color;

			for(int i=0; i<Quantity; i++)
			{
				std::srand((int)seed[i] * (int)seed[i] - i);
				std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
				result = std::rand() % Faces + 1;
				result_quantity[result-1]++;
				total += result;

				if(count_expr != "")
				{
					if(count_expr == "=" && result == count){total_count++;}
					if(count_expr == "<" && result < count){total_count++;}
					if(count_expr == ">" && result > count){total_count++;}
					if(count_expr == "<=" && result <= count){total_count++;}
					if(count_expr == ">=" && result >= count){total_count++;}
					if(count_expr == "!=" && result != count){total_count++;}
				}

				if(result == 1)
				{
					color = TEXT_RED;
				}
				else if(result == Faces)
				{
					color = TEXT_GREEN;
				}
				else
				{
					color = TEXT_WHITE;
				}

				if(just_show_rolls)
				{
					fprintf(stdout,"%s%d%s\n",color.c_str(),result,TEXT_NORMAL);
				}
				else if(!just_show_total)
				{
					fprintf(stdout,"Roll");
					for(int space=std::to_string(Quantity).length(); space>=std::to_string(i+1).length(); space--)
					{
						fprintf(stdout," ");
					}
					fprintf(stdout,"%d:",(i+1));
					for(int space=std::to_string(Faces).length(); space>=std::to_string(result).length(); space--)
					{
						fprintf(stdout," ");
					}
					fprintf(stdout,"%s%d%s\n",color.c_str(),result,TEXT_NORMAL);
				}
			}
			if(!just_show_rolls && !just_show_total)
			{
				fprintf(stdout,"\n");
				fprintf(stdout,"Natural total:\t%s%d%s\n",TEXT_WHITE,total,TEXT_NORMAL);

				if(Modifier != 0)
				{
					fprintf(stdout,"Modifier:\t%s%d%s\n",TEXT_WHITE,Modifier,TEXT_NORMAL);
					total += Modifier;
				}
				fprintf(stdout,"Total:\t\t%s%d%s\n",TEXT_WHITE,total,TEXT_NORMAL);

				if(count_expr != "")
				{
					fprintf(stdout,"\n");
					fprintf(stdout,"Rolls %s%d:\t%s%d%s\n",count_expr.c_str(),count,TEXT_WHITE,total_count,TEXT_NORMAL);
				}
			}
			else if(just_show_total)
			{
				total += Modifier;
				fprintf(stdout,"%d\n",total);
			}
			fs.close();
		}
		else if(is_list)
		{
			std::string list_line = "";
			std::string list_path = "";
			std::vector<std::string> values;
			std::ifstream fs;

			if(dice_list.substr(0,1) != "/" && dice_list.substr(0,2) != "./" && dice_list.substr(0,3) != "../")//If not specifying path, assume it's something in the dice-lists folder
			{
				list_path = dice_lists_dir+dice_list;
			}
			else//Otherwise, user is specifying a path outside of the dice-lists folder, so just go with what the user inputted
			{
				list_path = dice_list;
			}

			if(std::filesystem::exists(list_path))
			{
				fs.open(list_path);
				if(!fs.good())
				{
					output(Error,"Unable to open dice list \"%s\".",list_path.c_str());
				}
			}
			else
			{
				output(Error,"Dice list \"%s\" does not exist.",list_path.c_str());
				exit(-1);
			}

			while(std::getline(fs,list_line))//Get values from dice list, making sure to ignore blank space and comments
			{
				if(list_line != "" && list_line[0] != COMMENT)
				{
					values.push_back(list_line);
					list_line = "";
				}
			}
			fs.close();

			fs.open(random_seed_path);
			std::string data, seed;
			int result;
			do
			{
				std::getline(fs,data);
			}while((int)data[0] == 0); //Prevents crashing on first seed when (int)seed[0] == 0
			fs.close();

			seed += data;
			std::srand((int)seed[0] * (int)seed[seed.length()-1]);
			std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
			result = std::rand() % values.size();

			if(!just_show_total && !just_show_rolls)
			{
				fprintf(stdout,"[Roll result: %s%d%s]\n",TEXT_WHITE,result,TEXT_NORMAL);
			}
			if(!just_show_rolls)
			{
				fprintf(stdout,"%s\n",values[result].c_str());
			}
			else
			{
				fprintf(stdout,"%d\n",result);
			}
		}
	}
	void test()
	{
		std::string plus = TEXT_WHITE;
		plus += "+";
		Quantity = 100000;
		Faces = 20;
		roll();
		fprintf(stdout,"\n");
		fprintf(stdout,"\t┌───────────────────────────────────────┐\n");
		fprintf(stdout,"\t│ %s%s%sTEST RESULTS:%s                         │\n",TEXT_ITALIC,TEXT_BOLD,TEXT_YELLOW,TEXT_NORMAL);
		fprintf(stdout,"\t│                                       │\n");
		for(int i=0; i<Faces; i++)
		{
			float percent = ((float)result_quantity[i]/((float)Quantity/(float)Faces))*100-100;
			fprintf(stdout,"\t│ # of %d's: %s%d (%s%s%.2f%%%s from perfect) │\n",(i+1),(i<9)?" ":"",result_quantity[i],(percent>=0)?plus.c_str():"",TEXT_WHITE,percent,TEXT_NORMAL);
		}
		fprintf(stdout,"\t└───────────────────────────────────────┘\n");
	}
};
