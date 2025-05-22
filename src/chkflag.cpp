#include <cstring>
#include <string>
#include "../headers/define.h"

void chkFlagAppDesc(char** _argv, std::string description)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"%s\n",description.c_str());
		exit(0);
	}
}
void chkFlagModifyVariables(char** _argv, bool canModify)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_MODIFYVARIABLES))
	{
		//Don't use btos(), as that would require linking all of the dependancies for it when we just need this one function
		fprintf(stdout,"%s\n",(canModify ? "True" : "False"));
		exit(0);
	}
}
bool chkFlagHelp(char** _argv)
{
	return _argv[1] && (!strcasecmp(_argv[1],FLAG_HELPSHORT) || !strcasecmp(_argv[1],FLAG_HELPLONG));
}
