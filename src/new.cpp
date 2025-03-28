#include <filesystem>
#include <fstream>
#include <iostream>
#include <string.h>
#include "../headers/config.h"
#include "../headers/functions.h"

enum newFileScope
{
	CHARACTER,
	CAMPAIGN
};

template<typename T>
void fillCharacterSheet(Character* p_c)
{
	for(const auto& [k,v] : p_c->getDatamap<T>())
	{
		if(k[0] == '.') continue;
		std::string new_value;
		const char* color;
		switch(std::string(v)[0])
		{
			case VAR_SIGIL:
				color = VAR_COLOR;
				break;
			case DICE_SIGIL:
				color = DICE_COLOR;
				break;
			case WALLET_SIGIL:
				color = WALLET_COLOR;
				break;
			case CURRENCY_SIGIL:
				color = CURRENCY_COLOR;
				break;
			default:
				color = VAR_COLOR;
				break;
		}
		fprintf(stdout,"%s%s%s%s (%s%s%s%s): ",TEXT_WHITE,TEXT_BOLD,k.c_str(),TEXT_NORMAL,color,TEXT_ITALIC,(std::string(v)+'\0').c_str(),TEXT_NORMAL);
		getline(std::cin,new_value);
		if(new_value != "") p_c->set<T>(k,new_value);
	}
}

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(Error,"New expects at least one argument.");
		return -1;
	}

	chkFlagAppDesc(argv,"Creates a new character or campaign.");
	chkFlagModifyVariables(argv,false);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tnew [%sOPTIONS%s]\n\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"OPTIONS:\n");
		fprintf(stdout,"\tnew -c %sname%s %stemplate%s\tCreates a new character file called %sname%s using the character template %stemplate%s. Uses %sdefault_game%s setting if %stemplate%s is omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tnew -m %sname%s\t\tCreates a new campaign file called %sname%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		return 0;
	}

	Config cfg = Config();
	std::string campaign_name;
	std::string template_name;
	newFileScope nfs;
	for(int i=1; i<argc; i++)
	{
		if(!strcmp(argv[i],"-c"))
		{
			if(i == argc-1)
			{
				template_name = cfg.setting[DEFAULT_GAME];
			}
			else
			{
				template_name = std::string(argv[i+1]);
				i++;
			}
			nfs = CHARACTER;
		}
		else if(!strcmp(argv[i],"-m"))
		{
			if(i == argc-1)
			{
				fprintf(stdout,"Please enter a campaign name: ");
				getline(std::cin,campaign_name);
			}
			else
			{
				campaign_name = std::string(argv[i+1]);
				i++;
			}
			nfs = CAMPAIGN;
		}
		else if(argv[i][0] == '-')
		{
			output(Error,"Unknown option \"%s\"\n",argv[i]);
			return -1;
		}
	}

	if(nfs == CHARACTER)
	{
		bool exists = false;
		for(const auto& file : getDirectoryListing(templates_dir))
		{
			if(!stringcasecmp(file,template_name))
			{
				template_name = file;
				exists = true;
				break;
			}
		}
		if(!exists)
		{
			output(Error,"Template file \"%s\" does not exist.",template_name.c_str());
			return -1;
		}

		Character c = Character(templates_dir+template_name);
		fprintf(stdout,"Please enter values for the following keys (default values in parenthesis):\n");
		fillCharacterSheet<Var>(&c);
		fillCharacterSheet<Dice>(&c);
		fillCharacterSheet<Wallet>(&c);
		fillCharacterSheet<Currency>(&c);
		c.setDatasource(campaigns_dir+getEnvVariable(ENV_CURRENT_CAMPAIGN)+"characters/"+c.getProperty<Var>(c.getName()+".char");
		if(std::filesystem::exists(c.getDatasource()))
		{
			fprintf(stdout,"Character file \"%s\" exists, do you want to overwrite? (y/N): ",c.getDatasource().c_str());
			if(getchar() != 'y') return -1;
			fprintf(stdout,"\n");
		}
		c.save();
		output(Info,"Character \"%s\" has been created.",c.getName().c_str());
	}
	else
	{
		std::string campaign_root = campaigns_dir+campaign_name;
		if(std::filesystem::is_directory(campaign_root))
		{
			fprintf(stdout,"Campaign directory \"%s\" exists, do you want to overwrite? (y/N): ",campaign_root.c_str());
			if(getchar() != 'y') return -1;
			fprintf(stdout,"\n");
		}
		else
		{
			std::filesystem::create_directory(campaign_root);
			std::string campaign_variables_file = campaign_root+"/"+variable_file_name;
			if(!std::filesystem::exists(campaign_root+"/characters") || (std::filesystem::exists(campaign_root+"/characters") && !std::filesystem::is_directory(campaign_root+"/characters")))
				std::filesystem::create_directory(campaign_root+"/characters");
			if(std::filesystem::exists(campaign_variables_file))
				std::filesystem::remove(campaign_variables_file);
			std::ofstream ofs(campaign_variables_file);
			ofs.close();
			Campaign m = Campaign(campaign_variables_file);
			m.save();
			output(Info,"Campaign \"%s\" has been created.",campaign_name.c_str());
		}
	}

	return 0;
}
