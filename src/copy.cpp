#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(error,"copy expects at least 1 argument.");
		exit(-1);
	}
	if(argc > 5)
		output(warning,"copy expects no more than 5 arguments, ignoring all other arguments");

	chkFlagAppDesc(argv,"Copies a character or campaign, appending an incrementing number to each copy.");
	chkFlagModifyVariables(argv,true);
	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tcopy [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t-c %ssource%s [%sdestination%s]\tCopies character matching %ssource%s to %sdestination%s. If %sdestination%s is omitted, create a copy of the same name with an incrementing number appended.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-m %ssource%s [%sdestination%s]\tCopies campaign matching %ssource%s to %sdestination%s. If %sdestination%s is omitted, create a copy of the same name with an incrementing number appended.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-n %scopies%s\t\tMake %scopies%s copies of the character or campaign, appending an incrementing number to the end of each copy.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\t\tPrints this help text\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	std::string src, dst, src_campaign_path, dst_campaign_path, src_character_path, dst_character_path;
	MCStr src_mc, dst_mc;
	bool copy_c = false;
	bool copy_m = false;
	int copies = 1;
	for(int i=1; i<argc; i++)
	{
		if((!strcmp(argv[i],"-c") || !strcmp(argv[i],"-m") || !strcmp(argv[i],"-n")) && (i == argc-1 || argv[i+1][0] == '-'))
		{
			output(error,"Missing argument for \"%s\".",argv[i]);
			return -1;
		}
		else if(argv[i][0] == '-' && (argv[i][1] != 'c' && argv[i][1] != 'm' && argv[i][1] != 'n'))
		{
			output(error,"Unknown option \"%s\".",argv[i]);
			return -1;
		}
		else if(!strcmp(argv[i],"-c"))
		{
			copy_c = true;
			copy_m = false;
			Character c = Character();
			src = std::string(argv[i+1]);

			//Check if destination is specified. If not, use source name
			if(argv[i+2] && argv[i+2][0] != '-')
			{
				dst = std::string(argv[i+2]);
				i++;
			}
			else
			{
				dst = src;
			}

			src_mc = parseMCStr(src);
			dst_mc = parseMCStr(dst);

			//Check if using campaign/character syntax
			if(src_mc.m == "")
				src_character_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1);
			else
				src_character_path = campaigns_dir+src_mc.m+"/characters/";

			if(dst_mc.m == "")
				dst_character_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1);
			else
				dst_character_path = campaigns_dir+dst_mc.m+"/characters/";

			//Parse campaign if using campaign/character syntax
			bool character_exists = false;
			bool campaign_exists = false;
			if(src_mc.m != "")
			{
				for(const auto& entry : getDirectoryListing(campaigns_dir))
				{
					if(stringcasecmp(entry,src_mc.m) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;
					campaign_exists = true;
				}
				if(!campaign_exists)
				{
					output(error,"Source campaign \"%s\" does not exist.",src_mc.m.c_str());
					return -1;
				}
			}
			campaign_exists = false;
			if(dst_mc.m != "")
			{
				for(const auto& entry : getDirectoryListing(campaigns_dir))
				{
					if(stringcasecmp(entry,dst_mc.m) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;
					campaign_exists = true;
				}
				if(!campaign_exists)
				{
					output(error,"Source campaign \"%s\" does not exist.",dst_mc.m.c_str());
					return -1;
				}
			}

			//Check if source character exists and if potentially overwriting destination character
			for(const auto& entry : getDirectoryListing(left(src_character_path,rfindu(src_character_path,'/'))))
			{
				if(stringcasecmp(entry,src_mc.c+c_ext) || std::filesystem::is_directory(src_character_path+entry)) continue;
				src_character_path += entry;
				character_exists = true;
			}
			if(!character_exists)
			{
				output(error,"Character \"%s\" does not exist to be copied.",src.c_str());
				return -1;
			}

			dst_character_path += dst_mc.c+c_ext;
		}
		else if(!strcmp(argv[i],"-m"))
		{
			copy_c = false;
			copy_m = true;
			Character c = Character();
			src = std::string(argv[i+1]);

			//Check if destination is specified. If not, use source name
			if(argv[i+2] && argv[i+2][0] != '-')
			{
				dst = std::string(argv[i+2]);
				i++;
			}
			else
			{
				dst = src;
			}

			//Check if source campaign exists
			bool campaign_exists = false;
			for(const auto& entry : getDirectoryListing(campaigns_dir))
			{
				if(stringcasecmp(entry,src) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;
				src_campaign_path = campaigns_dir+src;
				dst_campaign_path = campaigns_dir+dst;
				campaign_exists = true;
			}
			if(!campaign_exists)
			{
				output(error,"Campaign \"%s\" does not exist to be copied.",src.c_str());
				return -1;
			}
		}
		else if(!strcmp(argv[i],"-n"))
		{
			if(!Var(argv[i+1]).isInt())
			{
				output(error,"Number of copies specified is not a number.");
				return -1;
			}
			copies = atoi(argv[i+1]);
		}
	}

	if(!copy_c && !copy_m)
	{
		output(error,"Expected \"-c\" or \"-m\".");
		return -1;
	}

	int copy_start = 1;
	if(isdigit(src[src.length()-1]))
	{
		std::string copy_start_str;
		for(int i=src.length(); i>0; i--)
		{
			if(!isdigit(src[i])) break;
			copy_start_str.insert(copy_start_str.begin(),src[i]);
		}
		try
		{
			copy_start = std::stoi(copy_start_str);
		}
		catch(...)//Should only trigger if the number is >INT_MAX, but doesn't hurt to catch other issues
		{
			output(error,"Unable to get starting copy index.");
			return -1;
		}
	}

	//If just making one copy and changing the name, don't append a number
	if(copy_c && (src_mc.m != dst_mc.m || src_mc.c != dst_mc.c) && copies == 1)
	{
		std::filesystem::copy(src_character_path,left(dst_character_path,dst_character_path.length()-5)+c_ext);
		output(info,"Character \"%s\" has been created.",dst.c_str());
		return 0;
	}
	if(copy_m && src != dst && copies == 1)
	{
		std::filesystem::copy(src_campaign_path,dst_campaign_path,std::filesystem::copy_options::recursive);
		output(info,"Campaign \"%s\" has been created.",dst.c_str());
		return 0;
	}

	for(int i=0; i<copies; i++)
	{
		std::string copy_num = std::to_string(i+copy_start);
		if(copy_c)
		{
			std::filesystem::copy(src_character_path,left(dst_character_path,dst_character_path.length()-5)+copy_num+c_ext);
			output(info,"Character \"%s\" has been created.",(dst+copy_num).c_str());
		}
		else if(copy_m)
		{
			std::filesystem::copy(src_campaign_path,dst_campaign_path+copy_num,std::filesystem::copy_options::recursive);
			output(info,"Campaign \"%s\" has been created.",(dst+copy_num).c_str());
		}
	}

	return 0;
}
