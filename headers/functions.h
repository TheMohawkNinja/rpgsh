#pragma once

#include <climits>
#include <fstream>
#include <filesystem>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <vector>
#include "define.h"
#include "scope.h"
#include "text.h"
#include "var.h"

enum OutputLevel
{
	info,
	warning,
	error
};
enum PreserveVariableLevel
{
	none,
	first,
	all
};
struct VariableInfo
{
	Scope scope = Scope();
	std::string variable = "";
	std::string xref = "";
	char type = '\0';
	char evalType = '\0';
	std::string key = "";
	std::string property = "";
};
struct GetAppOutputInfo
{
	int status = 0;
	std::vector<std::string> output;
};
struct MCStr
{
	std::string m, c;
};
struct ComplexCommandInfo
{
	std::string condition;
	std::vector<std::string> commands;
};

void printHeader(std::string s);

std::string defaultPrompt(Character c);

void output(OutputLevel level, const char* format, ...);

std::string handleBackslashEscSeqs(std::string str);
std::string makePretty(std::string str);
std::string stripFormatting(std::string str);

std::string btos(bool b);
std::string pvltos(PreserveVariableLevel pvl);
bool stob(std::string s);
PreserveVariableLevel stopvl(std::string s);

bool isScopeSigil(char c);
bool isTypeSigil(char c);

bool looksLikeSet(std::string s);
bool looksLikeVariable(std::string s);

bool isEscaped(std::string str, long unsigned pos);

int stringcasecmp(std::string a, std::string b);

std::vector<std::string> split(std::string str, char delimiter);

template <typename T>
long long findInVect(std::vector<T> v, T match, long unsigned start=0)
{
	for(long unsigned i=start; i<v.size(); i++)
		if(v[i] == match) return i;

	return -1;
}

long unsigned findu(std::string str, std::string match, long unsigned start=0);
long unsigned findu(std::string str, char ch, long unsigned start=0);
long unsigned rfindu(std::string str, std::string match, long unsigned start=UINT_MAX);
long unsigned rfindu(std::string str, char ch, long unsigned start=UINT_MAX);
long unsigned nfindu(std::string str, std::string match, long unsigned start=0);
long unsigned nfindu(std::string str, char ch, long unsigned start=0);

std::string left(std::string str, int n);
std::string right(std::string str, int n);

long unsigned countu(std::string str, char ch);

long unsigned getDisplayLength(std::string str);
int getCharLength(char c);

void addKeyToMatches(std::vector<std::string>** ppMatches, std::string k, std::string chk_str,
		     std::string key);
void addKeysToMatches(std::vector<std::string>* pMatches, Scope scope, std::string chk_str,
		      std::string key, char type);
void addPropertyToMatches(std::vector<std::string>** ppMatches, std::string chk_str,
			  std::string property,std::string property_str);
void addPropertiesToMatches(std::vector<std::string>* pMatches, Scope scope, std::string chk_str,
			    std::string k, std::string key, std::string property, char type);
void moveCursorBack(winsize w, int start, int end=0);
void moveCursorForward(winsize w, int start, int end);
void inputHandler(std::string* pInput, long unsigned offset=0);

std::string addSpaces(unsigned int n);

std::string mergeQuotes(std::string str);

std::string escapeSpaces(std::string str);
std::string escapeRegexChars(std::string str);

void confirmEnvVariablesFile();
void confirmShellVariablesFile();
void confirmCampaignVariablesFile();
void confirmHistoryFile();

std::vector<std::string> getDirectoryListing(std::string path);
std::string getLikeFileName(std::string chk_file, std::string chk_dir, bool is_dir, std::string xref);
void loadXRef(std::string* arg, Scope* p_scope);
VariableInfo parseVariable(std::string v);

MCStr parseMCStr(std::string s);

void printBadOpAndThrow(std::string bad_op);

void padding();

ComplexCommandInfo parseComplexCommandString(int argc, char** argv, bool hasCondition);
int replaceVariables(std::string* p_arg_str, PreserveVariableLevel pvl);
int runApp(std::string arg_str, bool redirect_output);
GetAppOutputInfo getAppOutput(std::string prog);
int runScript(std::string path);

void chkFlagAppDesc(char** _argv, std::string description);
void chkFlagPreserveVariables(char** _argv, PreserveVariableLevel pvl=none);
bool chkFlagHelp(char** _argv);

std::string getEnvVariable(std::string v);
void setEnvVariable(std::string v, std::string value);

unsigned int getWalletValue(Wallet w);

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map);
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput);

std::map<std::string,std::string> getSet(std::string set_str);
std::string getSetStr(VariableInfo vi);

template <typename T>
void sort(std::vector<T>* v)
{
	for(long unsigned i=0; i<v->size()-1; i++)
	{
		for(long unsigned j=i+1; j<v->size(); j++)
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
