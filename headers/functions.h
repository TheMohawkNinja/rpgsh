#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include "output.h"
#include "obj.h"
#include "define.h"

bool stob(std::string s);

void confirmShellVarsFile();

void printBadOpAndThrow(std::string bad_op);

void padding();

void run_rpgsh_prog(std::string args);

std::string get_shell_var(std::string var);
void set_shell_var(std::string var,std::string value);

std::string get_campaign_var(std::string var);
void set_campaign_var(std::string var,std::string value);

std::map<std::string,std::string> load_vars_from_file(std::string path);
/*std::string get_var(std::string var)
{
	rpgsh_char c = rpgsh_char();
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
void save_obj_to_file(std::string path, T obj, std::string obj_id)
{
	std::ofstream fs(path, std::ios::app);
	if(!fs.good())
	{
		output(Error,"Unable to open \'%s\' for writing",path.c_str());
		exit(-1);
	}

	for(const auto& [key,value] : obj)
	{
		//Template/Character file format definition
		std::string data =	obj_id+
					DS+
					key+
					DS+
					std::string(value)+
					"\n";
		fs<<data;
	}
	fs.close();
}

template <typename T>
RPGSH_OBJ<T> load_obj_from_file(std::string path, std::string var_id)
{
	RPGSH_OBJ<T> obj;

	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \'%s\'",path.c_str());
		exit(-1);
	}

	std::ifstream fs(path);
	if(!fs.good())
	{
		output(Error,"Unable to open \'%s\' for reading",path.c_str());
		exit(-1);
	}

	while(!fs.eof())
	{
		std::string data = "";
		std::string var = "";
		std::string key = "";
		T value;

		std::getline(fs,data);
		if(data != "" && data[0] != COMMENT && data.substr(0,data.find(DS)) == var_id)
		{
			var = data.substr(0,data.find(DS));
			key = data.substr(data.find(var)+var.length()+DS.length(),
					 (data.rfind(DS)-(data.find(var)+var.length()+DS.length())));

			//NOTE: This requires that the constructor T(std::string) exists!
			value = T(data.substr(data.rfind(DS)+DS.length(),
				 (data.length()-data.rfind(DS)+DS.length())));
			obj[key] = value;
		}
	}
	fs.close();

	return obj;
}
