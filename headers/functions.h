#pragma once

#include <climits>
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

bool isEscaped(std::string str, long unsigned int pos);

int stringcasecmp(std::string a, std::string b);

int findInStrVect(std::vector<std::string> v, std::string str, unsigned int start=0);

long unsigned int findu(std::string str, std::string match, long unsigned int start=0);
long unsigned int findu(std::string str, char ch, long unsigned int start=0);
long unsigned int rfindu(std::string str, std::string match, long unsigned int start=UINT_MAX);
long unsigned int rfindu(std::string str, char ch, long unsigned int start=UINT_MAX);

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

unsigned int getWalletValue(Wallet w);

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map);
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput);

std::string getSet(VariableInfo vi);

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
