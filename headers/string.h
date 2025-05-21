#pragma once

#include <climits>
#include <vector>
#include <string>

struct MCStr
{
	std::string m, c;
};

std::string btos(bool b);
bool stob(std::string s);
bool isEscaped(std::string str, long unsigned int pos);
int stringcasecmp(std::string a, std::string b);
std::vector<std::string> split(std::string str, char delimiter);
long unsigned int findu(std::string str, std::string match, long unsigned int start=0);
long unsigned int findu(std::string str, char ch, long unsigned int start=0);
long unsigned int rfindu(std::string str, std::string match, long unsigned int start=UINT_MAX);
long unsigned int rfindu(std::string str, char ch, long unsigned int start=UINT_MAX);
std::string left(std::string str, int n);
std::string right(std::string str, int n);
long unsigned int countu(std::string str, char ch);
std::string addSpaces(unsigned int n);
std::string mergeQuotes(std::string str);
std::string escapeSpaces(std::string str);
MCStr parseMCStr(std::string s);
