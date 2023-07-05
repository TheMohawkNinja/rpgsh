#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <cctype>
#include <climits>
#include <fstream>
#include <filesystem>
#include <cstdarg>
#include <unistd.h>
#include "text.h"

#define SHELL_VAR '$'
#define CHAR_VAR  '%'

std::string user = getlogin();
std::string base_path = "/home/"+user+"/.rpgsh/";
std::string shell_vars_path = base_path+".shell";

enum RPGSH_OUTPUT_TYPE
{
	Info,
	Warning,
	Error
};
void RPGSH_OUTPUT(RPGSH_OUTPUT_TYPE type, const char* format, ...)
{
	FILE* stream;
	std::string prefix = "";
	std::string color;
	va_list args;
	va_start(args, format);

	switch(type)
	{
		case Info:
			stream = stdout;
			prefix = "%s%s[%sINFO%s]%s    ";
			color = TEXT_CYAN;
			break;
		case Warning:
			stream = stderr;
			prefix = "%s%s[%sWARNING%s]%s ";
			color = TEXT_YELLOW;
			break;
		case Error:
			stream = stderr;
			prefix = "%s%s[%sERROR%s]%s   ";
			color = TEXT_RED;
			break;
	}
	fprintf(stream,prefix.c_str(),TEXT_BOLD,TEXT_WHITE,color.c_str(),TEXT_WHITE,color.c_str());
	vfprintf(stream,format,args);
	fprintf(stream,"%s\n",TEXT_NORMAL);
}

class RPGSH_DICE
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

	public:
		unsigned int	Quantity		=	0;
		unsigned int	Faces		=	0;
			 int	Modifier	=	0;

	RPGSH_DICE(){}
	RPGSH_DICE(unsigned int _quantity, unsigned int _faces, int _modifier)
	{
		Quantity = _quantity;
		Faces = _faces;
		Modifier = _modifier;
		just_show_total = true;
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
				RPGSH_OUTPUT(Error,"Unable to open file \"%s\"",dice.c_str());
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

class RPGSH_CURRENCY
{
	public:
		unsigned int 	Copper		=	0;
		unsigned int	Silver		=	0;
		unsigned int	Electrum	=	0;
		unsigned int	Gold		=	0;
		unsigned int	Platinum	=	0;
	RPGSH_CURRENCY(){}
	RPGSH_CURRENCY( unsigned int _Copper,
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

class RPGSH_ITEM
{
	public:
		std::string	Name		=	"<NO_NAME>";
		std::string	Rarity		=	"<NO_RARITY>";
		RPGSH_CURRENCY	Value		=	RPGSH_CURRENCY();
		std::string	Type		=	"<NO_TYPE>";
		unsigned int	Weight		=	0;
		std::string	Description	=	"<NO_DESCRIPTION>";
		RPGSH_DICE	Damage		=	RPGSH_DICE();
		bool		Equipped	=	false;
	RPGSH_ITEM(){}
	RPGSH_ITEM(	std::string	_Name,
			std::string	_Rarity,
			RPGSH_CURRENCY	_Value,
			std::string	_Type,
			unsigned int	_Weight,
			std::string	_Description,
			RPGSH_DICE	_Damage,
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

class RPGSH_SPELL
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

class RPGSH_VAR
{
	public:
		std::string Value = "";
		std::string DataSeparator = "::";

		explicit operator std::string() const
		{
			return Value;
		}
		explicit operator int() const
		{
			return std::stoi(Value);
		}
		RPGSH_VAR& operator = (const RPGSH_VAR b)
		{
			Value = b.Value;
			return *this;
		}
		RPGSH_VAR& operator = (const std::string b)
		{
			Value = b;
			return *this;
		}
		RPGSH_VAR& operator = (const int b)
		{
			Value = std::to_string(b);
			return *this;
		}
		RPGSH_VAR& operator += (const RPGSH_VAR b)
		{
			bool a_is_num = true;
			bool b_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}
			try{std::stoi(b.Value);}
			catch(...){b_is_num = false;}

			if(a_is_num && b_is_num)
			{
				Value = std::to_string((std::stoi(Value) + std::stoi(b.Value)));
			}
			else if(!a_is_num && !b_is_num)
			{
				Value = Value + b.Value;
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator += (const std::string b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(!a_is_num)
			{
				Value = Value + b;
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator += (const int b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(a_is_num)
			{
				Value = std::to_string((std::stoi(Value) + b));
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator -= (const RPGSH_VAR b)
		{
			bool a_is_num = true;
			bool b_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}
			try{std::stoi(b.Value);}
			catch(...){b_is_num = false;}

			if(a_is_num && b_is_num)
			{
				Value = std::to_string((std::stoi(Value) - std::stoi(b.Value)));
			}
			else if(!a_is_num && !b_is_num)
			{
				RPGSH_OUTPUT(Error,"Cannot subtract two non-numerical values from each other.");
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator -= (const int b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(a_is_num)
			{
				Value = std::to_string((std::stoi(Value) - b));
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator *= (const RPGSH_VAR b)
		{
			bool a_is_num = true;
			bool b_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}
			try{std::stoi(b.Value);}
			catch(...){b_is_num = false;}

			if(a_is_num && b_is_num)
			{
				Value = std::to_string((std::stoi(Value) * std::stoi(b.Value)));
			}
			else if(!a_is_num && !b_is_num)
			{
				RPGSH_OUTPUT(Error,"Cannot multiply two non-numerical values from each other.");
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator *= (const int b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(a_is_num)
			{
				Value = std::to_string((std::stoi(Value) * b));
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator /= (const RPGSH_VAR b)
		{
			bool a_is_num = true;
			bool b_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}
			try{std::stoi(b.Value);}
			catch(...){b_is_num = false;}

			if(a_is_num && b_is_num)
			{
				Value = std::to_string((std::stoi(Value) / std::stoi(b.Value)));
			}
			else if(!a_is_num && !b_is_num)
			{
				RPGSH_OUTPUT(Error,"Cannot divide two non-numerical values from each other.");
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator /= (const int b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(a_is_num)
			{
				Value = std::to_string((std::stoi(Value) / b));
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator ++ (const int b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(a_is_num)
			{
				Value = std::to_string((std::stoi(Value) + 1));
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}
		RPGSH_VAR& operator -- (const int b)
		{
			bool a_is_num = true;

			try{std::stoi(Value);}
			catch(...){a_is_num = false;}

			if(a_is_num)
			{
				Value = std::to_string((std::stoi(Value) - 1));
			}
			else
			{
				RPGSH_OUTPUT(Error,"Ambiguous operation between numerical and non-numerical values.");
			}
			return *this;
		}

	RPGSH_VAR(){}
	RPGSH_VAR(std::string _value)
	{
		Value = _value;
	}
	RPGSH_VAR(int _value)
	{
		Value = std::to_string(_value);
	}

	const char* c_str()
	{
		return Value.c_str();
	}
};
RPGSH_VAR operator + (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result += b;
	return result;
}
RPGSH_VAR operator + (const RPGSH_VAR a, const std::string b)
{
	RPGSH_VAR result = a;
	result += b;
	return result;
}
RPGSH_VAR operator + (const std::string a, const RPGSH_VAR b)
{
	RPGSH_VAR result = RPGSH_VAR(a);
	result += b;
	return result;
}
RPGSH_VAR operator + (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result += b;
	return result;
}
RPGSH_VAR operator + (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result += b;
	return result;
}
RPGSH_VAR operator - (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result -= b;
	return result;
}
RPGSH_VAR operator - (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result -= b;
	return result;
}
RPGSH_VAR operator - (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result -= b;
	return result;
}
RPGSH_VAR operator * (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result *= b;
	return result;
}
RPGSH_VAR operator * (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result *= b;
	return result;
}
RPGSH_VAR operator * (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result *= b;
	return result;
}
RPGSH_VAR operator / (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result /= b;
	return result;
}
RPGSH_VAR operator / (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result /= b;
	return result;
}
RPGSH_VAR operator / (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result /= b;
	return result;
}

class RPGSH_CHAR
{
	private:
		RPGSH_VAR var = RPGSH_VAR();
	public:
		std::string AttributeDesignator		=	"Attr";
		std::string SpellDesignator		=	"Spell";
		std::string DiceDesignator		=	"Dice";
		std::string CurrencyDesignator		=	"Currency";

		std::map<std::string, RPGSH_VAR> Attr;
		RPGSH_DICE		CurrentHitDice	=	RPGSH_DICE();
		RPGSH_DICE		TotalHitDice	=	RPGSH_DICE();
		RPGSH_CURRENCY		Currency	=	RPGSH_CURRENCY();
		std::vector<RPGSH_SPELL>Spellbook	=	{};

		std::string current_char_path = base_path+".current";

	RPGSH_CHAR()
	{
		Attr["Name"]			=	"<NO_NAME>";
		Attr["Class"]			=	"<NO_CLASS>";
		Attr["Level"]			=	1;
		Attr["Background"]		=	"<NO_BACKGROUND>";
		Attr["Player"]			=	"<NO_PLAYER>";
		Attr["Race"]			=	"<NO_RACE>";
		Attr["Alignment"]		=	"<NO_ALIGNMENT>";
		Attr["XP"]			=	0;
		Attr["Inspiration"]		=	"false";
		Attr["Proficiency"]		=	0;
		Attr["AC"]			=	0;
		Attr["Initiative"]		=	0;
		Attr["Speed"]			=	0;
		Attr["HP"]			=	0;
		Attr["MaxHP"]			=	0;
		Attr["TempHP"]			=	0;
		Attr["FailedDeathSaves"]	=	0;
		Attr["SucceededDeathSaves"]	=	0;
		Attr["Str"]			=	0;
		Attr["Dex"]			=	0;
		Attr["Con"]			=	0;
		Attr["Int"]			=	0;
		Attr["Wis"]			=	0;
		Attr["Cha"]			=	0;
		Attr["PersonalityTraits"]	=	"<NO_PERSONALITY_TRAITS>";
		Attr["Ideals"]			=	"<NO_IDEALS>";
		Attr["Bonds"]			=	"<NO_BONDS>";
		Attr["Flaws"]			=	"<NO_FLAWS>";
		Attr["FeaturesAndTraits"]	=	"<NO_FEATURES_AND_TRAITS>";
		Attr["Age"]			=	0;
		Attr["Height"]			=	0;
		Attr["Weight"]			=	0;
		Attr["EyeColor"]		=	"<NO_EYE_COLOR>";
		Attr["SkinColor"]		=	"<NO_SKIN_COLOR>";
		Attr["Hair"]			=	"<NO_HAIR>";
		Attr["Appearance"]		=	"<NO_APPEARANCE>";
		Attr["Allies"]			=	"<NO_ALLIES_OR_ORGANIZATIONS>";
		Attr["Backstory"]		=	"<NO_BACKSTORY>";
		Attr["Treasure"]		=	"<NO_TREASURE>";
		Attr["SpellcastingAbility"]	=	"<NO_SPELLCASTING_ABILITY>";
		Attr["SpellSaveDC"]		=	0;
		Attr["SpellAttackBonus"]	=	0;
	}

	void set_current()
	{
		if(std::filesystem::exists(current_char_path.c_str()))
		{
			std::filesystem::remove(current_char_path.c_str());
		}
		std::ofstream fs(current_char_path.c_str());
		fs<<std::string(Attr["Name"]);
		fs.close();
	}
	void save()
	{
		std::string char_path = base_path+std::string(Attr["Name"])+".char";

		if(!std::filesystem::exists(base_path.c_str()))
		{
			RPGSH_OUTPUT(Info,"Main rpgsh user directory not found at \"%s\", creating directory...",base_path.c_str());
			std::filesystem::create_directory(base_path);
		}
		if(std::filesystem::exists(char_path.c_str()))
		{
			std::filesystem::rename(char_path.c_str(),(char_path+".bak").c_str());
		}

		std::ofstream fs((char_path).c_str());
		for(const auto& [key,value] : Attr)
		{
			//Character file format definition
			std::string data =	AttributeDesignator+
						var.DataSeparator+
						key+
						var.DataSeparator+
						std::string(value)+
						"\n";
			fs<<data;
		}
		fs.close();
	}
	void load(bool load_bak)
	{
		std::ifstream fs((current_char_path).c_str());
		std::string current_char;
		std::getline(fs,current_char);
		fs.close();

		std::string char_path = base_path+current_char+".char"+((load_bak)?".bak":"");

		fs.open(char_path.c_str());
		while(!fs.eof())
		{
			std::string data = "";
			std::getline(fs,data);

			if(data.substr(0,data.find(var.DataSeparator)) == AttributeDesignator)//TODO Complete loading code
			{
				data = data.substr(data.find(var.DataSeparator)+var.DataSeparator.length(),
							(data.length() - (data.find(var.DataSeparator))));
				Attr[data.substr(0,data.find(var.DataSeparator))] = data.substr(data.find(var.DataSeparator)+var.DataSeparator.length(),
												(data.length() - (data.find(var.DataSeparator))));
			}
		}
		fs.close();
	}
};
