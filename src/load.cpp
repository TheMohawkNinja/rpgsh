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

	std::string character_name, campaign_name;
	if(!strcmp(argv[1],"-c"))
	{
		if(argc == 2)
		{
			output(Error,"Missing character name.");
			return -1;
		}
		else if(findu(std::string(argv[2]),'/') == std::string::npos)
		{
			character_name = std::string(argv[2]);
		}
		else
		{
			std::string arg2 = std::string(argv[2]);
			campaign_name = left(arg2,findu(arg2,'/'));
			character_name = right(arg2,findu(arg2,'/')+1);
		}
	}
	else if(!strcmp(argv[1],"-m"))
	{
		if(argc == 2)
		{
			output(Error,"Missing campaign name.");
			return -1;
		}
		else
		{
			campaign_name = std::string(argv[2]);
		}
	}
	else
	{
		output(Error,"Unknown option \"%s\"",argv[1]);
		return -1;
	}

	if(campaign_name != "" &&
	  (!std::filesystem::exists(campaigns_dir+campaign_name) || (std::filesystem::exists(campaigns_dir+campaign_name) && !std::filesystem::is_directory(campaigns_dir+campaign_name))))
	{
		output(Error,"Campaign \"%s\" does not exist.",campaign_name.c_str());
		return -1;
	}
	else if(character_name != "" && campaign_name == "" && !std::filesystem::exists(campaigns_dir+getEnvVariable(ENV_CURRENT_CAMPAIGN)+"/characters/"+character_name+".char"))
	{
		output(Error,"Character \"%s\" does not exist in this campaign.",character_name.c_str());
		return -1;
	}
	else if(character_name != "" && campaign_name != "" && !std::filesystem::exists(campaigns_dir+campaign_name+"/characters/"+character_name+".char"))
	{
		output(Error,"Character \"%s\" does not exist in the \"%s\" camapign.",character_name.c_str(),campaign_name.c_str());
		return -1;
	}

	if(campaign_name != "")
	{
		if(campaign_name.back() != '/') campaign_name += '/';
		setEnvVariable(ENV_CURRENT_CAMPAIGN,campaign_name);
		output(Info,"Loaded campaign \"%s\".",left(campaign_name,campaign_name.length()-1).c_str());
	}
	if(character_name != "")
	{
		setEnvVariable(ENV_CURRENT_CHAR,character_name);
		output(Info,"Loaded character \"%s\".",character_name.c_str());
	}

	return 0;
}
