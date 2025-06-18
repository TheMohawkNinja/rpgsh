#pragma once

#include <climits>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "define.h"
#include "scope.h"
#include "text.h"
#include "var.h"

enum OutputLevel
{
	Info,
	Warning,
	Error
};
struct VariableInfo
{
	Scope scope = Scope();
	std::wstring variable = L"";
	std::wstring xref = L"";
	char type = '\0';
	char evalType = '\0';
	std::wstring key = L"";
	std::wstring property = L"";
};
struct GetAppOutputInfo
{
	int status = 0;
	std::vector<std::wstring> output;
};
struct MCStr
{
	std::wstring m, c;
};

std::wstring defaultPrompt(Character c);

void output(OutputLevel level, const char* format, ...);

std::wstring makePretty(std::wstring value);
std::wstring stripFormatting(std::wstring str);

std::string btos(bool b);
std::wstring btows(bool b);
bool stob(std::string s);
bool wstob(std::wstring s);

bool isScopeSigil(char c);
bool isTypeSigil(char c);

bool looksLikeSet(std::wstring s);
bool looksLikeVariable(std::wstring s);

bool isEscaped(std::wstring str, long unsigned int pos);

int stringcasecmp(std::string a, std::string b);
int stringcasecmp(std::wstring a, std::wstring b);

std::vector<std::wstring> split(std::wstring str, char delimiter);

template <typename T>
long long int findInVect(std::vector<T> v, T match, long unsigned int start=0)
{
	for(long unsigned int i=start; i<v.size(); i++)
		if(v[i] == match) return i;

	return -1;
}

long unsigned int findu(std::string str, std::string match, long unsigned int start=0);
long unsigned int findu(std::string str, char ch, long unsigned int start=0);
long unsigned int rfindu(std::string str, std::string match, long unsigned int start=UINT_MAX);
long unsigned int rfindu(std::string str, char ch, long unsigned int start=UINT_MAX);
long unsigned int findu(std::wstring str, std::wstring match, long unsigned int start=0);
long unsigned int findu(std::wstring str, char ch, long unsigned int start=0);
long unsigned int rfindu(std::wstring str, std::wstring match, long unsigned int start=UINT_MAX);
long unsigned int rfindu(std::wstring str, char ch, long unsigned int start=UINT_MAX);

std::string left(std::string str, int n);
std::wstring left(std::wstring str, int n);
std::string right(std::string str, int n);
std::wstring right(std::wstring str, int n);

long unsigned int countu(std::wstring str, char ch);

std::wstring addSpaces(unsigned int n);

std::wstring mergeQuotes(std::wstring str);

std::wstring escapeSpaces(std::wstring str);

void confirmEnvVariablesFile();

void confirmShellVariablesFile();

void confirmCampaignVariablesFile();

void confirmHistoryFile();

std::vector<std::wstring> getDirectoryListing(std::wstring path);
std::wstring getLikeFileName(std::wstring chk_file,std::wstring chk_dir,bool is_dir,std::wstring xref);
void loadXRef(std::wstring* arg, Scope* p_scope);
VariableInfo parseVariable(std::wstring v);// Derive information about variable from string

MCStr parseMCStr(std::wstring s);

void printBadOpAndThrow(std::wstring bad_op);

void padding();

int runApp(std::wstring args, bool redirect_output);
GetAppOutputInfo getAppOutput(std::wstring prog);

void chkFlagAppDesc(char** _argv, std::wstring description);
void chkFlagModifyVariables(char** _argv, bool canModify=false);
bool chkFlagHelp(char** _argv);

std::wstring getEnvVariable(std::wstring v);
void setEnvVariable(std::wstring v, std::wstring value);

unsigned int getWalletValue(Wallet w);

template<typename T>
void appendMap(Scope scope, std::map<std::wstring,std::wstring>* p_map);
void appendOutput(std::map<std::wstring,std::wstring> map, std::wstring key, std::wstring* pOutput);

std::map<std::wstring,std::wstring> getSet(std::wstring set_str);
std::wstring getSetStr(VariableInfo vi);

template <typename T>
void sort(std::vector<T>* v)
{
	for(long unsigned int i=0; i<v->size()-1; i++)
	{
		for(long unsigned int j=i+1; j<v->size(); j++)
		{
			if((*v)[j] < (*v)[i])
			{
				std::wstring tmp = (*v)[i];
				(*v)[i]=(*v)[j];
				(*v)[j]=tmp;
			}
		}
	}
}

template <typename T>
datamap<T> getDatamapFromAllScopes();
