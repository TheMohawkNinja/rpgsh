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
#include <stdexcept>
#include "text.h"

//Variable character definitions
#define SHELL_VAR		'$'
#define CHAR_VAR		'%'

//Built-in variables
#define CURRENT_CHAR_SHELL_VAR	".CURRENT_CHAR"
#define CHAR_NAME_ATTR		".NAME"

//Setting names
#define PADDING			"padding"
#define DEFAULT_GAME		"default_game"

//Games
#define GAME_DND1E		"dnd1e"		//Dungeons and Dragons, 1st Edition (AD&D)
#define GAME_DND2E		"dnd2e"		//Dungeons and Dragons, 2nd Edition
#define GAME_DND3E		"dnd3e"		//Dungeons and Dragons, 3rd Edition
#define GAME_DND3_5E		"dnd3.5e"	//Dungeons and Dragons, 3.5 Edition
#define GAME_DND4E		"dnd4e"		//Dungeons and Dragons, 4th Edition
#define GAME_DND5E		"dnd5e"		//Dungeons and Dragons, 5th Edition
#define GAME_PATHFINDER		"pathfinder"	//Pathfinder

const char* random_seed_path = "/dev/random";
const char* backup_random_seed_path = "/dev/urandom";
std::string user = getlogin();
std::string root_path = "/home/"+user+"/.rpgsh/";
std::string shell_vars_path = root_path+".shell";
std::string config_path = root_path+".config";

enum output_level
{
	Info,
	Warning,
	Error
};
void output(output_level level, const char* format, ...)
{
	FILE* stream;
	std::string prefix = "";
	std::string color;
	va_list args;
	va_start(args, format);

	switch(level)
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

bool stob(std::string s)
{
	if(!strcasecmp(s.c_str(),"true"))
	{
		return true;
	}
	else if(!strcasecmp(s.c_str(),"false"))
	{
		return false;
	}
	else
	{
		throw std::invalid_argument("Parameter for stob() was not \'true\' or \'false\'.");
		return false;
	}
}

void check_root_path()
{
	if(!std::filesystem::exists(root_path.c_str()))
	{
		output(Info,"Root rpgsh directory not found, creating directory at \'%s\'.",root_path.c_str());
		std::filesystem::create_directory(root_path);
	}
}

class RPGSH_CONFIG
{
	#define COMMENT '#'

	// Configuration file definition:
	//
	// Name=Value
	// Ignore '#' for use as comments
	public:
		std::string default_value = "";
		std::map<std::string,std::string> setting;

	private:
	std::string get_config_item(std::string s)
	{
		if(s.find("=")!=std::string::npos)
		{
			return s.substr(0,s.find("="));
		}
		else//No '='
		{
			output(Warning,"No \'=\' found for config item \'%s\', ignoring...",s.c_str());
			return default_value;
		}
	}
	std::string get_config_value(std::string s)
	{
		if(s.find("=")!=std::string::npos)
		{
			if(s.find("=") == s.length()-1)//Nothing after '='
			{
				//If the setting exists, return default value for setting, otherwise return default_value
				if(auto search = setting.find(s); search != setting.end())
				{
					output(Warning,"Found blank config setting \'%s\'",setting[s].c_str());
					return setting[s];
				}
				else
				{
					output(Warning,"Unknown config setting \'%s\', ignoring...",s.c_str());
					return default_value;
				}
			}
			else//Found value
			{
				return s.substr(s.find("=")+1,s.length()-(s.find("=")+1));
			}
		}
		else//No '='
		{
			return default_value;
		}
	}

	public:
	RPGSH_CONFIG()
	{
		check_root_path();

		// Set defaults
		setting[PADDING]	=	"true";
		setting[DEFAULT_GAME]	=	GAME_DND5E;

		// Create default config file if one does not exist
		if(!std::filesystem::exists(config_path.c_str()))
		{
			output(Info,"Config file not found, creating default at \'%s\'.",config_path.c_str());
			std::ofstream fs(config_path.c_str());
			fs<<COMMENT<<" Places a newline character before and after command output.\n";
			fs<<COMMENT<<" Default: "<<setting[PADDING]<<"\n";
			fs<<PADDING<<"="<<setting[PADDING]<<"\n";
			fs<<"\n";
			fs<<COMMENT<<" Sets the default game for use by RPGSH.\n";
			fs<<COMMENT<<" Will change things like the character sheet used for the \'print\' command, and what attributes will be created for new characters when using default settings.\n";
			fs<<COMMENT<<" Default: "<<setting[DEFAULT_GAME]<<"\n";
			fs<<DEFAULT_GAME<<"="<<setting[DEFAULT_GAME]<<"\n";
			fs.close();
		}
		std::ifstream fs(config_path.c_str());
		while(!fs.eof())
		{
			std::string data;
			std::getline(fs,data);
			if(data.length() && data[0] != COMMENT)
			{
				setting[get_config_item(data)] = get_config_value(data);
			}
		}
		fs.close();
	}
};

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

				fs.open(random_seed_path);
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
				output(Error,"Unable to open file \"%s\"",dice.c_str());
				exit(-1);
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
			float percent = 100-(((float)result_quantity[i]/((float)Quantity/(float)Faces))*100);
			fprintf(stdout,"\t│ # of %d's: %s%d (%s%s%.2f%%%s from perfect) │\n",(i+1),(i<9)?" ":"",result_quantity[i],(percent>=0)?plus.c_str():"",TEXT_WHITE,percent,TEXT_NORMAL);
		}
		fprintf(stdout,"\t└───────────────────────────────────────┘\n");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Cannot subtract two non-numerical values from each other.");
			}
			else
			{
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Cannot multiply two non-numerical values from each other.");
			}
			else
			{
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Cannot divide two non-numerical values from each other.");
			}
			else
			{
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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
				output(Error,"Ambiguous operation between numerical and non-numerical values.");
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

std::string get_shell_var(std::string var)
{
	RPGSH_VAR v = RPGSH_VAR();
	std::ifstream ifs(shell_vars_path.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(v.DataSeparator)) == var)
		{
			return data.substr(data.find(v.DataSeparator)+v.DataSeparator.length(),
				data.length()-(data.find(v.DataSeparator)+v.DataSeparator.length()));
		}
	}
	ifs.close();
	return "";
}
void set_shell_var(std::string var,std::string value)
{
	RPGSH_VAR v = RPGSH_VAR();
	std::ifstream ifs(shell_vars_path.c_str());
	std::filesystem::remove((shell_vars_path+".bak").c_str());
	std::ofstream ofs((shell_vars_path+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(v.DataSeparator)) == var)
		{
			ofs<<var + v.DataSeparator + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue)
	{
		ofs<<var + v.DataSeparator + value + "\n";
	}
	ifs.close();
	ofs.close();
	std::filesystem::remove(shell_vars_path.c_str());
	std::filesystem::rename((shell_vars_path+".bak").c_str(),shell_vars_path.c_str());
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

	RPGSH_CHAR()
	{
		Attr["Name"]			=	"<NO_NAME>";
		Attr[CHAR_NAME_ATTR]		=	"Name";
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

	std::string Name()
	{
		return std::string(Attr[std::string(Attr[CHAR_NAME_ATTR])]);
	}
	void save()
	{
		std::string char_path = root_path+Name()+".char";

		check_root_path();
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
	void load(std::string character, bool load_bak)
	{
		std::string char_path = root_path+character+".char"+((load_bak)?".bak":"");

		check_root_path();
		std::ifstream fs(char_path.c_str());
		if(!fs.good())
		{
			//This should only apply if 'character' gets set to something that isn't a valid character
			//Generate default character and set that as the current character
			RPGSH_CHAR *dummy = new RPGSH_CHAR();
			dummy->save();
			char_path = root_path+Name()+".char"+((load_bak)?".bak":"");
			delete dummy;
			fs.open(char_path.c_str());
		}
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
	void set_as_current()
	{
		set_shell_var(CURRENT_CHAR_SHELL_VAR,Name());
	}
	void update_Name(std::string new_name_attr)
	{
		Attr[CHAR_NAME_ATTR] = new_name_attr;
		set_as_current();
		save();
	}
};
