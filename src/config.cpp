#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include "../headers/config.h"
#include "../headers/define.h"
#include "../headers/functions.h"
#include "../headers/output.h"

std::string Config::get_config_item(std::string s)
{
	if(s.find("=")!=std::string::npos)
	{
		return left(s,s.find('='));
	}
	else//No '='
	{
		output(Warning,"No \'=\' found for config item \'%s\', ignoring...",s.c_str());
		return default_value;
	}
}
std::string Config::get_config_value(std::string s)
{
	if(s.find("=")!=std::string::npos)
	{
		if(s.find("=") == s.length()-1)//Nothing after '='
		{
			//If the setting exists, return default value for setting, otherwise return default_value
			if(auto search = setting.find(s); search != setting.end())
			{
				output(Warning,"Found blank config setting \'%s\'",setting[s].c_str());
				return setting[s];
			}
			else
			{
				output(Warning,"Unknown config setting \'%s\', ignoring...",s.c_str());
				return default_value;
			}
		}
		else//Found value
		{
			return right(s,s.find("=")+1);
		}
	}
	else//No '='
	{
		return default_value;
	}
}

Config::Config()
{
	// Set defaults
	setting[PADDING]	=	"true";
	setting[DEFAULT_GAME]	=	"dnd5e";
	setting[HIDE_TIPS]	=	"false";

	// Create default config file if one does not exist
	if(!std::filesystem::exists(config_path.c_str()))
	{
		output(Info,"Config file not found, creating default at \'%s\'.",config_path.c_str());
		std::ofstream fs(config_path.c_str());
		fs<<COMMENT<<" Places a newline character before and after command output.\n";
		fs<<COMMENT<<" Default: "<<setting[PADDING]<<"\n";
		fs<<PADDING<<"="<<setting[PADDING]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Sets the default game for use by rpgsh.\n";
		fs<<COMMENT<<" Will change things like the character sheet used for the \'print\' command, and what attributes will be created for new characters when using default settings.\n";
		fs<<COMMENT<<" Default: "<<setting[DEFAULT_GAME]<<"\n";
		fs<<DEFAULT_GAME<<"="<<setting[DEFAULT_GAME]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" If set to \"true\", hides the tip text that appears when launching rpgsh.\n";
		fs<<COMMENT<<" Default: "<<setting[HIDE_TIPS]<<"\n";
		fs<<HIDE_TIPS<<"="<<setting[HIDE_TIPS]<<"\n";
		fs.close();
	}
	std::ifstream fs(config_path.c_str());
	while(!fs.eof())
	{
		std::string data;
		std::getline(fs,data);
		if(data.length() && data[0] != COMMENT)
		{
			setting[get_config_item(data)] = get_config_value(data);
		}
	}
	fs.close();
}
