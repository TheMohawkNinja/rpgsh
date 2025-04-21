#include <filesystem>
#include <string.h>
#include "../headers/functions.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(Error,"Load expects at least one argument.");
		return -1;
	}

	chkFlagAppDesc(argv,"Loads a campaign and/or character.");
	chkFlagModifyVariables(argv,false);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tload [%sOPTIONS%s]\n\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"OPTIONS:\n");
		fprintf(stdout,"\t-c %scharacter%s\tSets %scharacter%s as the currently loaded character if it exists\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-c %s/campaign%s/%scharacter%s\tSets %scamapign%s as the curerntly loaded campaign if it exists, and sets %scharacter%s as the currently loaded character if it exists\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-m %scampaign%s\tSets %scampaign%s as the currently loaded campaign if it exists\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		return 0;
	}

	if(!strcmp(argv[1],"-c") && argc == 2)
	{
		output(Error,"Missing character name.");
		return -1;
	}
	else if(!strcmp(argv[1],"-m") && argc == 2)
	{
		output(Error,"Missing campaign name.");
		return -1;
	}
	else if(argc == 2)
	{
		output(Error,"Unknown option \"%s\"",argv[1]);
		return -1;
	}

	MCStr mc = parseMCStr(argv[2]);

	if(mc.m != "" &&
	  (!std::filesystem::exists(campaigns_dir+mc.m) || (std::filesystem::exists(campaigns_dir+mc.m) && !std::filesystem::is_directory(campaigns_dir+mc.m))))
	{
		output(Error,"Campaign \"%s\" does not exist.",mc.m.c_str());
		return -1;
	}
	else if(mc.c != "" && mc.m == "" && !std::filesystem::exists(campaigns_dir+getEnvVariable(ENV_CURRENT_CAMPAIGN)+"/characters/"+mc.c+".char"))
	{
		output(Error,"Character \"%s\" does not exist in this campaign.",mc.c.c_str());
		return -1;
	}
	else if(mc.c != "" && mc.m != "" && !std::filesystem::exists(campaigns_dir+mc.m+"/characters/"+mc.c+".char"))
	{
		output(Error,"Character \"%s\" does not exist in the \"%s\" camapign.",mc.c.c_str(),mc.m.c_str());
		return -1;
	}

	if(mc.m != "")
	{
		setEnvVariable(ENV_CURRENT_CAMPAIGN,mc.m+"/");
		output(Info,"Loaded campaign \"%s\".",mc.m.c_str());
	}
	if(mc.c != "")
	{
		setEnvVariable(ENV_CURRENT_CHARACTER,mc.c);
		output(Info,"Loaded character \"%s\".",mc.c.c_str());
	}

	return 0;
}
