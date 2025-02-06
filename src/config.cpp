#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include "../headers/config.h"
#include "../headers/define.h"
#include "../headers/functions.h"

std::string Config::getConfigItem(std::string s)
{
	if(s.find("=") != std::string::npos)
	{
		return left(s,s.find("="));
	}
	else//No '='
	{
		output(Warning,"No \'=\' found for config item \"%s\", ignoring...",s.c_str());
		return "";
	}
}
std::string Config::getConfigValue(std::string s)
{
	if(s.find("=") != std::string::npos && s.find("=") == s.length()-1)//Nothing after '='
	{
		//If the setting exists, return default value for setting, otherwise return blank
		for(const auto& [k,v] : setting)
		{
			if(stringcasecmp(left(s,s.find("=")),k)) continue;

			output(Warning,"Found blank value for config setting \"%s\"",setting[s].c_str());
			return setting[s];
		}
		output(Warning,"Unknown config setting \"%s\", ignoring...",s.c_str());
		return "";
	}
	else if(s.find("=") != std::string::npos)//Found value
	{
		return right(s,s.find("=")+1);
	}
	else//No '='
	{
		return "";
	}
}

Config::Config()
{
	// Set defaults
	setting[PADDING]	=	"true";
	setting[DEFAULT_GAME]	=	"dnd5e";
	setting[HIDE_TIPS]	=	"false";
	setting[HISTORY_LENGTH]	=	"100";
	setting[ALIASES]	=	"?::help";
	setting[STARTUP_APPS]	=	"banner,version";

	// Create default config file if one does not exist
	if(!std::filesystem::exists(config_path.c_str()))
	{
		output(Info,"Config file not found, creating default at \"%s\".",config_path.c_str());
		std::ofstream fs(config_path.c_str());
		fs<<COMMENT<<" Places a newline character before and after command output.\n";
		fs<<COMMENT<<" Default: "<<setting[PADDING]<<"\n";
		fs<<PADDING<<"="<<setting[PADDING]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Sets the default game for use by rpgsh.\n";
		fs<<COMMENT<<" Will change things like the character sheet used for the \"print\" command, and what attributes will be created for new characters when using default settings.\n";
		fs<<COMMENT<<" Default: "<<setting[DEFAULT_GAME]<<"\n";
		fs<<DEFAULT_GAME<<"="<<setting[DEFAULT_GAME]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" If set to \"true\", hides the tip text that appears when launching rpgsh.\n";
		fs<<COMMENT<<" Default: "<<setting[HIDE_TIPS]<<"\n";
		fs<<HIDE_TIPS<<"="<<setting[HIDE_TIPS]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Sets the number of lines to save for rpgsh history.\n";
		fs<<COMMENT<<" Default: "<<setting[HISTORY_LENGTH]<<"\n";
		fs<<HISTORY_LENGTH<<"="<<setting[HISTORY_LENGTH]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Sets the alias(es) to run a given command.\n";
		fs<<COMMENT<<" Formatted like a variable set where the keys are the aliases and the values are the commands to run when the alias is ran.\n";
		fs<<COMMENT<<" Default: "<<setting[ALIASES]<<"\n";
		fs<<ALIASES<<"="<<setting[ALIASES]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Comma-delimited list of programs to run when rpgsh launches.\n";
		fs<<COMMENT<<" Default: "<<setting[STARTUP_APPS]<<"\n";
		fs<<STARTUP_APPS<<"="<<setting[STARTUP_APPS]<<"\n";
		fs.close();
	}
	std::ifstream fs(config_path.c_str());
	while(!fs.eof())
	{
		std::string data;
		std::getline(fs,data);
		if(data.length() && data[0] != COMMENT)//TODO: Setup for inline comments
			setting[getConfigItem(data)] = getConfigValue(data);
	}
	fs.close();
}
