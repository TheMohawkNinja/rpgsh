#include <string>
#include <string.h>
#include <vector>
#include <cctype>
#include <climits>
#include <fstream>
#include "text.h"

class DNDSH_DICE
{
	private:
		bool just_show_rolls = false;
		bool just_show_total = false;
		bool is_list = false;
		unsigned int count = 0;
		unsigned int total_count = 0;
		std::string dice = "";
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
						fprintf(stderr,"%s%sERROR: Unable to get dice %s. \"%s\" is not a number.%s\n",TEXT_BOLD,TEXT_RED,value.c_str(),d.substr(i,1).c_str(),TEXT_NORMAL);
						return 0;
					}
				}

				try
				{
					std::stoi(value_str);
					return std::stoi(value_str);
				}
				catch(...)
				{
					fprintf(stderr,"%s%sERROR: Unable to get dice %s. \"%s\" exceeds the maximum size of %d.%s\n",TEXT_BOLD,TEXT_RED,value.c_str(),value_str.c_str(),INT_MAX,TEXT_NORMAL);
					return 0;
				}
			}
			else
			{
				if(start == d.length())
				{
					if(required)
					{
						fprintf(stderr,"%s%sERROR: Unable to get dice %s. No %s specified.%s\n",TEXT_BOLD,TEXT_RED,value.c_str(),value.c_str(),TEXT_NORMAL);
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
						fprintf(stderr,"%s%sERROR: Unable to get dice %s. \"%s\" is not a number.%s\n",TEXT_BOLD,TEXT_RED,value.c_str(),d.substr(i,1).c_str(),TEXT_NORMAL);
						return 0;
					}
				}

				try
				{
					std::stoi(value_str);
					return std::stoi(value_str);
				}
				catch(...)
				{
					fprintf(stderr,"%s%sERROR: Unable to get dice %s. \"%s\" exceeds the maximum size of %d.%s\n",TEXT_BOLD,TEXT_RED,value.c_str(),value_str.c_str(),INT_MAX,TEXT_NORMAL);
					return 0;
				}
			}
		}
	public:
		unsigned int	Quantity		=	0;
		unsigned int	Faces		=	0;
			 int	Modifier	=	0;

	DNDSH_DICE(){}
	DNDSH_DICE(unsigned int _quantity, unsigned int _faces, int _modifier)
	{
		Quantity = _quantity;
		Faces = _faces;
		Modifier = _modifier;
		just_show_total = true;
	}
	DNDSH_DICE(std::string _dice, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count)
	{
		dice = _dice;
		just_show_rolls = _just_show_rolls;
		just_show_total = _just_show_total;
		is_list = _is_list;
		count_expr = _count_expr;
		count = _count;

		if(!is_list)
		{
			if(dice.substr(0,1) != "d")
			{
				Quantity = get_value(dice,"quantity",0,"d",false,true);
				Faces = get_value(dice,"faces",dice.find("d",0) + 1,"",false,true);
				Modifier = get_value(dice,"modifier",dice.find(std::to_string(Faces),dice.find("d",0)) + std::to_string(Faces).length(),"",true,false);
			}
			else
			{
				Quantity = 1;
				Faces = get_value(dice,"faces",dice.find("d",0) + 1,"",false,true);
				Modifier = get_value(dice,"modifier",dice.find(std::to_string(Faces),dice.find("d",0)) + std::to_string(Faces).length(),"",true,false);
			}
		}
	}

	void roll()
	{
		if(Quantity > 0 && Faces > 0)
		{
			for(int i=0; i<Faces; i++)
			{
				result_quantity.push_back(0);
			}

			if(!just_show_rolls && !just_show_total)
			{
				fprintf(stdout,"Rolling an %s%d%s-sided die %s%d%s time(s) with a modifier of %s%d%s...\n",TEXT_WHITE,Faces,TEXT_NORMAL,TEXT_WHITE,Quantity,TEXT_NORMAL,TEXT_WHITE,Modifier,TEXT_NORMAL);
			}

			std::ifstream fs("/dev/random");//Probably safe to assume this file exists?
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
			try
			{
				std::string tmp = "";
				std::vector<std::string> values;
				std::ifstream fs(dice);

				while(std::getline(fs,tmp))
				{
					values.push_back(tmp);
					tmp = "";
				}
				fs.close();

				fs.open("/dev/random");//Probably safe to assume this file exists?
				std::string data, seed;
				int result;
				std::getline(fs,data);
				fs.close();

				seed += data;
				std::srand((int)seed[0] * (int)seed[seed.length()-1]);
				std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
				fprintf(stdout,"%s\n",values[std::rand() % values.size()].c_str());
			}
			catch(...)
			{
				fprintf(stderr,"%s%sERROR: Unable to open file \"%s\"\n",TEXT_RED,TEXT_ITALIC,dice.c_str(),TEXT_NORMAL);
				exit(-1);
			}
		}
	}
	void test()
	{
		Quantity = 100000;
		Faces = 20;
		roll();
		fprintf(stdout,"\n");
		for(int i=0; i<Faces; i++)
		{
			float percent = 100-(((float)result_quantity[i]/((float)Quantity/(float)Faces))*100);
			fprintf(stdout,"# of %d's: %d (%.2f%% from perfect)\n",(i+1),result_quantity[i],percent);
		}
	}
};

class DNDSH_CURRENCY
{
	public:
		unsigned int 	Copper		=	0;
		unsigned int	Silver		=	0;
		unsigned int	Electrum	=	0;
		unsigned int	Gold		=	0;
		unsigned int	Platinum	=	0;
	DNDSH_CURRENCY(){}
	DNDSH_CURRENCY( unsigned int _Copper,
			unsigned int _Silver,
			unsigned int _Electrum,
			unsigned int _Gold,
			unsigned int _Platinum)
			{
				Copper = _Copper;
				Silver = _Silver;
				Electrum = _Electrum;
				Gold = _Gold;
				Platinum = _Platinum;
			}
};

class DNDSH_ITEM
{
	public:
		std::string	Name		=	"<NO_NAME>";
		std::string	Rarity		=	"<NO_RARITY>";
		DNDSH_CURRENCY	Value		=	DNDSH_CURRENCY();
		std::string	Type		=	"<NO_TYPE>";
		unsigned int	Weight		=	0;
		std::string	Description	=	"<NO_DESCRIPTION>";
		DNDSH_DICE	Damage		=	DNDSH_DICE();
		bool		Equipped	=	false;
	DNDSH_ITEM(){}
	DNDSH_ITEM(	std::string	_Name,
			std::string	_Rarity,
			DNDSH_CURRENCY	_Value,
			std::string	_Type,
			unsigned int	_Weight,
			std::string	_Description,
			DNDSH_DICE	_Damage,
			bool		_Equipped)
			{
				Name = _Name;
				Rarity = _Rarity;
				Value = _Value;
				Type = _Type;
				Weight = _Weight;
				Description = _Description;
				Damage = _Damage;
				Equipped = _Equipped;
			}

};

class DNDSH_SPELL
{
	public:
		std::string	Name		=	"<NO_NAME>";
		unsigned char	Level		=	0;
		std::string	School		=	"<NO_SCHOOL>";
		std::string	Components	=	"<NO_COMPONENTS>";
		std::string	CastingTime	=	"<NO_CASTING_TIME>";
		std::string	Range		=	"<NO_RANGE>";
		std::string	Duration	=	"<NO_DURATION>";

};

class DNDSH_CHAR_ATTR
{
	public:
		std::string Value = "";

		DNDSH_CHAR_ATTR operator + (const DNDSH_CHAR_ATTR &Other)
		{
			bool Value_is_int = true;
			bool Other_is_int = true;
			try
			{
				std::stoi(Value);
			}
			catch(...)
			{
				Value_is_int = false;
			}
			try
			{
				std::stoi(Other);
			}
			catch(...)
			{
				Other_is_int = false;
			}

			if(Value_is_int && Other_is_int)
			{
				Value = std::to_string((std::stoi(Value) + std::stoi(Other)));
			}
			else if(!Value_is_int && !Other_is_int)
			{
				Value = Value + Other;
			}
			else
			{
				fprintf(stderr,"%s%sERROR: Ambiguous operation between numerical and non-numerical value.%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
			}
		}

	DNDSH_CHAR_ATTR(){}
	DNDSH_CHAR_ATTR(std::string _value)
	{
		Value = _value;
	}
};

class DNDSH_CHARACTER
{
	public:
		//std::vector<
		char				Name[32]		=	"<NO_NAME>";
		std::string			Class			=	"<NO_CLASS>";
		unsigned char			Level			=	1;
		std::string			Background		=	"<NO_BACKGROUND>";
		std::string			Player			=	"<NO_PLAYER>";
		std::string			Race			=	"<NO_RACE>";
		std::string			Alignment		=	"<NO_ALIGNMENT>";
		unsigned int			XP			=	0;
		bool				Inspiration		=	false;
		unsigned char			Proficiency		=	0;
		unsigned char			AC			=	0;
		unsigned char			Inititiative		=	0;
		unsigned char			Speed			=	0;

		unsigned char			HP			=	0;
		unsigned char			MaxHP			=	0;
		unsigned char			TempHP			=	0;
		DNDSH_DICE			CurrentHitDice		=	DNDSH_DICE();
		DNDSH_DICE			TotalHitDice		=	DNDSH_DICE();
		unsigned char			FailedDeathSaves	=	0;
		unsigned char			SucceededDeathSaves	=	0;

		unsigned char			Str			=	0;
		unsigned char			Dex			=	0;
		unsigned char			Con			=	0;
		unsigned char			Int			=	0;
		unsigned char			Wis			=	0;
		unsigned char			Cha			=	0;

		std::string			PersonalityTraits	=	"<NO_PERSONALITY_TRAITS>";
		std::string			Ideals			=	"<NO_IDEALS>";
		std::string			Bonds			=	"<NO_BONDS>";
		std::string			Flaws			=	"<NO_FLAWS>";
		std::string			FeaturesAndTraits	=	"<NO_FEATURES_AND_TRAITS>";
		DNDSH_CURRENCY			Currency		=	DNDSH_CURRENCY();

		unsigned int			Age			=	0;
		unsigned int			Height			=	0;
		unsigned int			Weight			=	0;
		std::string			EyeColor		=	"<NO_EYE_COLOR>";
		std::string			SkinColor		=	"<NO_SKIN_COLOR>";
		std::string			Hair			=	"<NO_HAIR>";

		std::string			Appearance		=	"<NO_APPEARANCE>";
		std::string			Allies			=	"<NO_ALLIES_OR_ORGANIZATIONS>";
		std::string			Backstory		=	"<NO_BACKSTORY>";
		std::string			Treasure		=	"<NO_TREASURE>";

		std::string			SpellcastingAbility	=	"<NO_SPELLCASTING_ABILITY>";
		unsigned char			SpellSaveDC		=	0;
		unsigned char			SpellAttackBonus	=	0;
		std::vector<DNDSH_SPELL>	Spellbook		=	{};
};
