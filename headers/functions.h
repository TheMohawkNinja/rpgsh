#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "output.h"
#include "define.h"
#include "var.h"

bool stob(std::string s);

void confirmEnvVariablesFile();

void confirmShellVarsFile();

void confirmCampaignVarsFile();

std::vector<std::string> getDirectoryListing(std::string path);

void printBadOpAndThrow(std::string bad_op);

void padding();

void run_rpgsh_prog(std::string args, bool redirect_output);

std::vector<std::string> get_prog_output(std::string prog);

std::string get_env_variable(std::string v);
void set_env_variable(std::string v, std::string value);

std::string get_shell_var(std::string var);
void set_shell_var(std::string var,std::string value);

std::string get_campaign_var(std::string var);
void set_campaign_var(std::string var,std::string value);

std::map<std::string,std::string> load_vars_from_file(std::string path);

template <typename T>
void save_obj_to_file(std::string path, T obj, char obj_id)
{
	std::ofstream fs(path, std::ios::app);
	if(!fs.good())
	{
		output(Error,"Unable to open \"%s\" for writing",path.c_str());
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
datamap<T> load_obj_from_file(std::string path, char var_id)
{
	datamap<T> obj;

	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \"%s\"",path.c_str());
		exit(-1);
	}

	std::ifstream fs(path);
	if(!fs.good())
	{
		output(Error,"Unable to open \"%s\" for reading",path.c_str());
		exit(-1);
	}

	while(!fs.eof())
	{
		std::string data = "";
		char type = '\0';
		std::string key = "";
		T value;

		std::getline(fs,data);
		type = data[0];
		if(data != "" && type != COMMENT && type == var_id)
		{
			key = data.substr(1+DS.length(),
					 (data.rfind(DS)-(1+DS.length())));

			//NOTE: This requires that the constructor T(std::string) exists!
			value = T(data.substr(data.rfind(DS)+DS.length(),
				 (data.length()-data.rfind(DS)+DS.length())));
			obj[key] = value;
		}
	}
	fs.close();

	return obj;
}

template <typename T>
datamap<T> getDatamapFromAllScopes(char var_id)
{
	datamap<T> ret;

	std::string character = get_shell_var(CURRENT_CHAR_SHELL_VAR);
	std::string campaign = get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_campaign_file = current_campaign_dir+campaigns_dir+".vars";
	std::string current_character_file = current_campaign_dir+"characters/"+character+".char";

	for(const auto& [k,v] : load_obj_from_file<T>(current_character_file,var_id))
		ret[k] = v;

	for(const auto& [k,v] : load_obj_from_file<T>(current_campaign_file,var_id))
		ret[k] = v;

	for(const auto& [k,v] : load_obj_from_file<T>(shell_vars_file,var_id))
		ret[k] = v;

	return ret;
}
