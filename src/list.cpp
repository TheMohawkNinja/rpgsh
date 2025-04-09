#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 3)
	{
		output(Error,"list only expects 1, or 2 arguments.");
		return -1;
	}

	chkFlagAppDesc(argv,"Lists characters and/or campaigns.");
	chkFlagModifyVariables(argv,true);
	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tlist [%sOPTIONS%s] [%svalue%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tLists all characters in all campaigns\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-m\t\tLists all campaigns\n");
		fprintf(stdout,"\t-m %scampaign%s\tLists all characters in the campaign %scampaign%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	std::string campaign;
	bool campaign_flag = false;
	for(int i=1; i<argc; i++)
	{
		if(!strcmp(argv[i],"-m") && i < argc-1)
		{
			campaign = argv[i+1];
			campaign_flag = true;
			break;
		}
		else if(!strcmp(argv[i],"-m"))
		{
			campaign_flag = true;
			break;
		}
		else if(argv[i][0] == '-' && argv[i][1] != 'm')
		{
			output(Error,"Unknown option \"%s\"",argv[i]);
			return -1;
		}
	}

	if(campaign == "" && !campaign_flag)
	{
		for(const auto& campaign_entry : getDirectoryListing(campaigns_dir))
		{
			if(std::filesystem::is_directory(campaigns_dir+campaign_entry))
				campaign = campaign_entry;
			else
				continue;

			fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,campaign.c_str(),TEXT_NORMAL);
			fprintf(stdout,"%s",TEXT_WHITE);
			for(long unsigned int i=0; i<campaign.length()+2; i++)
				fprintf(stdout,"─");
			fprintf(stdout,"%s\n",TEXT_NORMAL);
			for(const auto& character_entry : getDirectoryListing(campaigns_dir+campaign+"/characters/"))
			{
				if(character_entry.length() > 5 && right(character_entry,character_entry.length()-5) == ".char")
					fprintf(stdout,"%s\n",left(character_entry,character_entry.length()-5).c_str());
			}
			fprintf(stdout,"\n");
		}
		fprintf(stdout,"\b");
	}
	else
	{
		std::string campaign_path = campaigns_dir;
		if(campaign != "")
		{
			for(const auto& campaign_entry : getDirectoryListing(campaigns_dir))
			{
				if(std::filesystem::is_directory(campaigns_dir+campaign_entry) &&
				   !stringcasecmp(campaign_entry,campaign))
					campaign = campaign_entry;
				else
					continue;
			}
			campaign_path += campaign+"/characters/";
			for(const auto& character_entry : getDirectoryListing(campaign_path))
			{
				if(character_entry.length() > 5 && right(character_entry,character_entry.length()-5) == ".char")
					fprintf(stdout,"%s\n",left(character_entry,character_entry.length()-5).c_str());
				else
					continue;
			}
		}
		else
		{
			for(const auto& campaign_entry : getDirectoryListing(campaigns_dir))
			{
				if(std::filesystem::is_directory(campaigns_dir+campaign_entry))
					fprintf(stdout,"%s\n",campaign_entry.c_str());
				else
					continue;
			}
		}
	}

	return 0;
}
