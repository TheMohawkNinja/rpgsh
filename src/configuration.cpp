#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include "../headers/configuration.h"
#include "../headers/define.h"
#include "../headers/functions.h"

std::string Configuration::getConfigurationItem(std::string s)
{
	if(s.find("=") != std::string::npos)
		return left(s,s.find("="));
	else
		output(warning,"No \'=\' found for configuration item \"%s\", ignoring...",s.c_str());
	return "";
}
std::string Configuration::getConfigurationValue(std::string s)
{
	if(s.find("=") != std::string::npos)
		return right(s,s.find("=")+1);
	else
		return "";
}

Configuration::Configuration()
{
	// Set defaults
	setting[PADDING]	=	"true";
	setting[DEFAULT_GAME]	=	"dnd5e";
	setting[HIDE_TIPS]	=	"false";
	setting[HISTORY_LENGTH]	=	"100";
	setting[ALIASES]	=	"cp::copy::-::del::ls::list::?::help::ver::version";
	setting[STARTUP_APPS]	=	"banner,version";
	setting[PRE_RUN_APPS]	=	"";
	setting[POST_RUN_APPS]	=	"autorun";
	setting[VERBOSITY]	=	"3";

	// Create default configuration file if one does not exist
	if(!std::filesystem::exists(configuration_path.c_str()))
	{
		output(info,"Configuration file not found, creating default at \"%s\".",configuration_path.c_str());
		std::ofstream fs(configuration_path.c_str());
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
		fs<<"\n";
		fs<<COMMENT<<" Comma-delimited list of programs to run before a command is entered into the prompt.\n";
		fs<<COMMENT<<" Default: "<<setting[PRE_RUN_APPS]<<"\n";
		fs<<PRE_RUN_APPS<<"="<<setting[PRE_RUN_APPS]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Comma-delimited list of programs to run after a command is entered into the prompt.\n";
		fs<<COMMENT<<" Default: "<<setting[POST_RUN_APPS]<<"\n";
		fs<<POST_RUN_APPS<<"="<<setting[POST_RUN_APPS]<<"\n";
		fs<<"\n";
		fs<<COMMENT<<" Determines which (if any) info/warning/error messages will be printed.\n";
		fs<<COMMENT<<" 3=info,warning,error, 2=warning,error, 1=error, 0=none\n";
		fs<<COMMENT<<" Default: "<<setting[VERBOSITY]<<"\n";
		fs<<VERBOSITY<<"="<<setting[VERBOSITY]<<"\n";
		fs.close();
	}
	std::ifstream fs(configuration_path.c_str());
	while(!fs.eof())
	{
		std::string data;
		std::getline(fs,data);
		if(findu(data,COMMENT) != std::string::npos)
			data = left(data,findu(data,COMMENT));
		if(data.length() && data[0] != COMMENT)
			setting[getConfigurationItem(data)] = getConfigurationValue(data);
	}
	fs.close();
}
