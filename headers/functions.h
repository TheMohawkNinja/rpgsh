#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdarg>
#include "define.h"
#include "output.h"
#include "var.h"

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
