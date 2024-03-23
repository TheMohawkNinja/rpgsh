#include <string.h>
#include <map>
#include <vector>
#include <cstdarg>
#include "../headers/functions.h"
#include "../headers/dice.h"

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

void printBadOpAndThrow(std::string bad_op)
{
	output(Error,("Invalid operation: "+bad_op).c_str());
	throw;
	return;
}

std::string get_shell_var(std::string var)
{
	std::ifstream ifs(shell_vars_file.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == var)
		{
			return data.substr(data.find(DS)+DS.length(),
				data.length()-(data.find(DS)+DS.length()));
		}
	}
	ifs.close();
	return "";
}
void set_shell_var(std::string var,std::string value)
{
	std::ifstream ifs(shell_vars_file.c_str());
	std::filesystem::remove((shell_vars_file+".bak").c_str());
	std::ofstream ofs((shell_vars_file+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "" || data == "\n")// Prevents writing blank lines back to file
			continue;
		if(data.substr(0,data.find(DS)) == var)
		{
			ofs<<var + DS + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue)
	{
		ofs<<var + DS + value + "\n";
	}
	ifs.close();
	ofs.close();
	std::filesystem::remove(shell_vars_file.c_str());
	std::filesystem::rename((shell_vars_file+".bak").c_str(),shell_vars_file.c_str());
}

std::string get_campaign_var(std::string var)
{
	std::string campaign_vars_file = campaigns_dir +
					get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
					".vars";
	std::ifstream ifs(campaign_vars_file.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == var)
		{
			return data.substr(data.find(DS)+DS.length(),
				data.length()-(data.find(DS)+DS.length()));
		}
	}
	ifs.close();
	return "";
}
void set_campaign_var(std::string var,std::string value)
{
	std::string campaign_vars_file = campaigns_dir +
					get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
					".vars";
	std::ifstream ifs(campaign_vars_file.c_str());
	std::filesystem::remove((campaign_vars_file+".bak").c_str());
	std::ofstream ofs((campaign_vars_file+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "" || data == "\n")// Prevents writing blank lines back to file
			continue;
		if(data.substr(0,data.find(DS)) == var)
		{
			ofs<<var + DS + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue)
	{
		ofs<<var + DS + value + "\n";
	}
	ifs.close();
	ofs.close();
	std::filesystem::remove(campaign_vars_file.c_str());
	std::filesystem::rename((campaign_vars_file+".bak").c_str(),campaign_vars_file.c_str());
}

std::map<std::string,std::string> load_vars_from_file(std::string path)
{
	std::map<std::string,std::string> vars;

	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \'%s\'",path.c_str());
		exit(-1);
	}

	std::ifstream ifs(path);
	if(!ifs.good())
	{
		output(Error,"Unable to open \'%s\' for reading",path.c_str());
		exit(-1);
	}
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "")
			break;
		std::string var_name = data.substr(0,data.find(DS));
		std::string var_value = data.substr(var_name.length()+DS.length(),
						data.length()-(var_name.length()+DS.length()));
		vars[var_name] = var_value;
	}
	ifs.close();

	return vars;
}
/*std::string get_var(std::string var)
{
	RPGSH_CHAR c = RPGSH_CHAR();
	std::string ret = "";
	std::string path = "";
	std::map<std::string,std::string> m;
	char last_char = var[var.length()-1];

	switch(var[0])
	{
		case CHAR_SIGIL:
			c.load(get_shell_var(CURRENT_CHAR_SHELL_VAR),false);
			switch(last_char)
			{
				case '/':
				ret += std::string(c.Attr[var.substr(0,var.length()-1)]);
				break;

				default:
				return std::string(c.Attr[var]);
			}

			// Convert character sheet to std::map<std::string,std::string>
			// TODO Will need to add currency, currency system, and wallet
			for(const auto& [k,v] : c.Attr)
				m[k] = std::string(v);
			for(const auto& [k,v] : c.Dice)
				m[k] = std::string(v);
			break;
		case CAMPAIGN_SIGIL:
			switch(last_char)
			{
				case '/':
				try
				{
					ret += get_campaign_var(var.substr(0,var.length()-1));
				}
				catch(...){}
				break;

				default:
				return get_campaign_var(var);
			}
			path = campaigns_dir +
				get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
				".vars";
			m = load_vars_from_file(path);
			break;
		case SHELL_SIGIL:
			switch(last_char)
			{
				case '/':
				try
				{
					ret += get_shell_var(var.substr(0,var.length()-1));
				}
				catch(...){}
				break;

				default:
				return get_shell_var(var);
			}
			path = shell_vars_file;
			m = load_vars_from_file(path);
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",last_char);
			exit(-1);
	}
	return ret;
}*/

template <typename T>
void save_obj_to_file(std::string path, RPGSH_OBJ<RPGSH_DICE> obj, std::string obj_id);
void save_obj_to_file(std::string path, RPGSH_OBJ<RPGSH_VAR> obj, std::string obj_id);

template <typename T>
RPGSH_OBJ<RPGSH_DICE> load_obj_from_file(std::string path, std::string var_id);
RPGSH_OBJ<RPGSH_VAR> load_obj_from_file(std::string path, std::string var_id);
