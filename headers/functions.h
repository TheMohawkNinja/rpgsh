#pragma once

#include <climits>
#include <fstream>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "define.h"
#include "text.h"
#include "var.h"
#include "variable.h"

struct GetAppOutputInfo
{
	int status = 0;
	std::vector<std::string> output;
};

bool isScopeSigil(char c);
bool isTypeSigil(char c);

bool looksLikeSet(std::string s);
bool looksLikeVariable(std::string s);

template <typename T>
long long int findInVect(std::vector<T> v, T match, long unsigned int start=0)
{
	for(long unsigned int i=start; i<v.size(); i++)
		if(v[i] == match) return i;

	return -1;
}

void confirmEnvVariablesFile();
void confirmShellVariablesFile();
void confirmCampaignVariablesFile();
void confirmHistoryFile();

std::vector<std::string> getDirectoryListing(std::string path);
std::string getLikeFileName(std::string chk_file,std::string chk_dir,bool is_dir,std::string xref);

void printBadOpAndThrow(std::string bad_op);

void padding();

int runApp(std::string args, bool redirect_output);
GetAppOutputInfo getAppOutput(std::string prog);

void chkFlagAppDesc(char** _argv, std::string description);
void chkFlagModifyVariables(char** _argv, bool canModify=false);
bool chkFlagHelp(char** _argv);

std::string getEnvVariable(std::string v);
void setEnvVariable(std::string v, std::string value);

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map);
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput);

std::map<std::string,std::string> getSet(std::string set_str);
std::string getSetStr(VariableInfo vi);

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
datamap<T> getDatamapFromAllScopes();
