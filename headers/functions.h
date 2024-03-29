#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "output.h"
#include "obj.h"
#include "define.h"

bool stob(std::string s);

void confirmShellVarsFile();

void confirmCampaignVarsFile();

std::vector<std::string> getDirectoryListing(std::string path);

void printBadOpAndThrow(std::string bad_op);

void padding();

void run_rpgsh_prog(std::string args, bool redirect_output);

std::string get_shell_var(std::string var);
void set_shell_var(std::string var,std::string value);

std::string get_campaign_var(std::string var);
void set_campaign_var(std::string var,std::string value);

std::map<std::string,std::string> load_vars_from_file(std::string path);

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
