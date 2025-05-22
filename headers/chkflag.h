#pragma once

#include <string>

void chkFlagAppDesc(char** _argv, std::string description);
void chkFlagModifyVariables(char** _argv, bool canModify=false);
bool chkFlagHelp(char** _argv);
