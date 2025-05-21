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
