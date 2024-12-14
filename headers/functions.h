#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "define.h"
#include "output.h"
#include "scope.h"
#include "var.h"

struct VariableInfo
{
	Scope scope = Scope();
	std::string variable = "";
	char type = '\0';
	char evalType = '\0';
	std::string key = "";
	std::string property = "";
};

std::string btos(bool b);

bool stob(std::string s);

bool isScopeSigil(char c);

bool isTypeSigil(char c);

bool looksLikeSet(std::string s);

bool isEscaped(std::string str, int pos);

int stringcasecmp(std::string a, std::string b);

int findInStrVect(std::vector<std::string> v, std::string str, unsigned int start);

std::string left(std::string str, int n);

std::string right(std::string str, int n);

std::string addSpaces(int n);

std::string mergeQuotes(std::string str);

std::string escapeSpaces(std::string str);

void confirmEnvVariablesFile();

void confirmShellVariablesFile();

void confirmCampaignVariablesFile();

std::vector<std::string> getDirectoryListing(std::string path);
std::string getLikeFileName(std::string chk_file,std::string chk_dir,bool is_dir,std::string xref);
void loadXRef(std::string* arg, Scope* p_scope);
VariableInfo parseVariable(std::string v);// Derive information about variable from string

void printBadOpAndThrow(std::string bad_op);

void padding();

int run_rpgsh_prog(std::string args, bool redirect_output);

std::vector<std::string> get_prog_output(std::string prog);

void check_print_app_description(char** _argv, std::string description);

std::string get_env_variable(std::string v);
void set_env_variable(std::string v, std::string value);

CurrencySystem findMatchingCurrencySystem(std::string str);

unsigned int getWalletValue(Wallet w);

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map);
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput);

std::string getSet(VariableInfo vi);

template <typename TL, typename TR>
bool approxEquals(TL lhs, TR rhs);

template <typename T>
void sort(std::vector<T>* v)
{
	for(long unsigned int i=0; i<v->size()-1; i++)
	{
		for(long unsigned int j=i+1; j<v->size(); j++)
		{
			if((*v)[j] < (*v)[i])
			{
				std::string tmp = (*v)[i];
				(*v)[i]=(*v)[j];
				(*v)[j]=tmp;
			}
		}
	}
}

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
datamap<T> getDatamapFromAllScopes(char type)//TODO: Remove "type", infer from T
{
	confirmEnvVariablesFile();
	datamap<T> ret;

	std::string character = get_env_variable(CURRENT_CHAR_SHELL_VAR);
	std::string campaign = get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_campaign_path = current_campaign_dir+variable_file_name;
	std::string current_character_path = current_campaign_dir+"characters/"+character+".char";

	for(const auto& [k,v] : load_obj_from_file<T>(current_character_path,type))
		ret[k] = v;

	for(const auto& [k,v] : load_obj_from_file<T>(current_campaign_path,type))
		ret[k] = v;

	for(const auto& [k,v] : load_obj_from_file<T>(shell_variables_path,type))
		ret[k] = v;

	return ret;
}
