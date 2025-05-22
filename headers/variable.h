#pragma once

#include <string>
#include "scope.h"

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

void loadXRef(std::string* arg, Scope* p_scope);
VariableInfo parseVariable(std::string v);

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map);
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput);

std::map<std::string,std::string> getSet(std::string set_str);
std::string getSetStr(VariableInfo vi);
