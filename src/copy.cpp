#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(Error,"copy expects at least 1 argument.");
		exit(-1);
	}
	if(argc > 4)
		output(Warning,"copy only expects 1, 2, or 3 arguments, ignoring all other arguments");

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

	std::string src, dst, src_campaign_path, dst_campaign_path, src_character_path, dst_character_path, src_character, dst_character;
	bool copy_c = false;
	bool copy_m = false;
	int copies = 1;
	for(int i=1; i<argc; i++)
	{
		if((!strcmp(argv[i],"-c") || !strcmp(argv[i],"-m") || !strcmp(argv[i],"-n")) && (i == argc-1 || argv[i+1][0] == '-'))
		{
			output(Error,"Missing argument for \"%s\".",argv[i]);
			return -1;
		}
		else if(argv[i][0] == '-' && (argv[i][1] != 'c' && argv[i][1] != 'm' && argv[i][1] != 'n'))
		{
			output(Error,"Unknown option \"%s\".",argv[i]);
			return -1;
		}
		else if(!strcmp(argv[i],"-c"))
		{
			copy_c = true;
			copy_m = false;
			Character c = Character();
			src = std::string(argv[i+1]);
			i++;
			if(argc > i+1)
			{
				dst = std::string(argv[i+1]);
				i++;
			}
			else
			{
				dst = src;
			}

			if(findu(src,'/') == std::string::npos)
			{
				src_character_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1);
				src_character = src;
			}
			else
			{
				src_character_path = campaigns_dir+left(src,findu(src,'/')+1)+"characters/";
				src_character = right(src,findu(src,'/')+1);
			}
			if(findu(dst,'/') == std::string::npos)
			{
				dst_character_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1);
				dst_character = dst;
			}
			else
			{
				dst_character_path = campaigns_dir+left(dst,findu(dst,'/')+1)+"characters/";
				dst_character = right(dst,findu(dst,'/')+1);
			}

			bool character_exists = false;
			bool campaign_exists = false;
			if(findu(src,'/') != std::string::npos)
			{
				for(const auto& entry : getDirectoryListing(campaigns_dir))
				{
					if(stringcasecmp(entry,left(src,findu(src,'/'))) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;
					campaign_exists = true;
				}
				if(!campaign_exists)
				{
					output(Error,"Source campaign \"%s\" does not exist.",left(src,findu(src,'/')).c_str());
					return -1;
				}
			}
			campaign_exists = false;
			if(findu(dst,'/') != std::string::npos)
			{
				for(const auto& entry : getDirectoryListing(campaigns_dir))
				{
					if(stringcasecmp(entry,left(dst,findu(dst,'/'))) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;
					campaign_exists = true;
				}
				if(!campaign_exists)
				{
					output(Error,"Destination campaign \"%s\" does not exist.",left(dst,findu(dst,'/')).c_str());
					return -1;
				}
			}

			for(const auto& entry : getDirectoryListing(left(src_character_path,rfindu(src_character_path,'/'))))
			{
				if(stringcasecmp(entry,src_character+".char") || std::filesystem::is_directory(src_character_path+entry)) continue;
				src_character_path += entry;
				character_exists = true;
			}
			if(!character_exists)
			{
				output(Error,"Character \"%s\" does not exist to be copied.",src.c_str());
				return -1;
			}
			character_exists = false;
			for(const auto& entry : getDirectoryListing(left(dst_character_path,rfindu(dst_character_path,'/'))))
			{
				if(stringcasecmp(entry,dst_character+".char") || std::filesystem::is_directory(dst_character_path+entry)) continue;
				fprintf(stdout,"%s%sThe character \"%s\" exists, overwrite? [y/N]: %s",TEXT_YELLOW,TEXT_BOLD,dst.c_str(),TEXT_NORMAL);
				if(getchar() != 'y') return 0;
				//std::filesystem::remove(dst_character_path+entry);
				output(Info,"Character file \"%s\" has been deleted.",dst_character_path.c_str());
			}

			dst_character_path += dst_character+".char";
			fprintf(stdout,"Will copy \"%s\" (%s) to \"%s\" (%s)\n",src_character.c_str(),src_character_path.c_str(),dst_character.c_str(),dst_character_path.c_str());
		}
		else if(!strcmp(argv[i],"-m"))
		{
			copy_c = false;
			copy_m = true;
			Character c = Character();
			src = std::string(argv[i+1]);
			i++;
			if(argc > i+1)
			{
				dst = std::string(argv[i+1]);
				i++;
			}
			else
			{
				dst = src;
			}

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
				output(Error,"Campaign \"%s\" does not exist to be copied.",src.c_str());
				return -1;
			}
			campaign_exists = false;
			for(const auto& entry : getDirectoryListing(campaigns_dir))
			{
				if(stringcasecmp(entry,dst) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;

				fprintf(stdout,"%s%sThe campaign \"%s\" exists, overwrite? [y/N]: %s",TEXT_YELLOW,TEXT_BOLD,dst.c_str(),TEXT_NORMAL);
				if(getchar() != 'y') return 0;
				//std::filesystem::remove_all(campaigns_dir+entry);
				output(Info,"Campaign directory \"%s\" has been deleted.",(campaigns_dir+entry).c_str());
				dst_character_path = dst_character_path+entry;
			}

			fprintf(stdout,"Will copy \"%s\" to \"%s\"\n",src_campaign_path.c_str(),dst_campaign_path.c_str());
		}
		else if(!strcmp(argv[i],"-n"))
		{
			if(!Var(argv[i+1]).isInt())
			{
				output(,"Number of copies specified is not a number.");
				return -1;
			}
			copies = atoi(argv[i+1]);
		}

		for(int i=0; i<copies; i++)
		{
			if(copy_c)
			{
				for(int ch=dst_character.length(); ch>0; ch--)
			}
		}
	}

	return 0;
}
