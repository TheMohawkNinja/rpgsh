#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include "../headers/config.h"
#include "../headers/define.h"
#include "../headers/functions.h"

std::wstring Config::getConfigItem(std::wstring s)
{
	if(findu(s,'=') != std::wstring::npos)
	{
		return left(s,findu(s,'='));
	}
	else//No '='
	{
		output(Warning,"No \'=\' found for config item \"%s\", ignoring...",s.c_str());
		return L"";
	}
}
std::wstring Config::getConfigValue(std::wstring s)
{
	if(findu(s,'=') != std::wstring::npos && findu(s,'=') == s.length()-1)//Nothing after '='
	{
		//If the setting exists, return default value for setting, otherwise return blank
		for(const auto& [k,v] : setting)
		{
			if(stringcasecmp(left(s,findu(s,'=')),std::wstring(k))) continue;

			output(Warning,"Found blank value for config setting \"%s\"",setting[s].c_str());
			return setting[s];
		}
		output(Warning,"Unknown config setting \"%s\", ignoring...",s.c_str());
		return L"";
	}
	else if(findu(s,'=') != std::wstring::npos)//Found value
	{
		return right(s,findu(s,'=')+1);
	}
	else//No '='
	{
		return L"";
	}
}

Config::Config()
{
	// Set defaults
	setting[PADDING]	=	L"true";
	setting[DEFAULT_GAME]	=	L"dnd5e";
	setting[HIDE_TIPS]	=	L"false";
	setting[HISTORY_LENGTH]	=	L"100";
	setting[ALIASES]	=	L"?::help::-::del";
	setting[STARTUP_APPS]	=	L"banner,version";
	setting[POST_RUN_APPS]	=	L"";

	// Create default config file if one does not exist
	if(!std::filesystem::exists(config_path.c_str()))
	{
		output(Info,"Config file not found, creating default at \"%s\".",config_path.c_str());
		std::wofstream fs(config_path.c_str());
		fs<<COMMENT<<L" Places a newline character before and after command output.\n";
		fs<<COMMENT<<L" Default: "<<setting[PADDING]<<L"\n";
		fs<<PADDING<<L"="<<setting[PADDING]<<L"\n";
		fs<<L"\n";
		fs<<COMMENT<<L" Sets the default game for use by rpgsh.\n";
		fs<<COMMENT<<L" Will change things like the character sheet used for the \"print\" command, and what attributes will be created for new characters when using default settings.\n";
		fs<<COMMENT<<L" Default: "<<setting[DEFAULT_GAME]<<L"\n";
		fs<<DEFAULT_GAME<<L"="<<setting[DEFAULT_GAME]<<L"\n";
		fs<<L"\n";
		fs<<COMMENT<<L" If set to \"true\", hides the tip text that appears when launching rpgsh.\n";
		fs<<COMMENT<<L" Default: "<<setting[HIDE_TIPS]<<L"\n";
		fs<<HIDE_TIPS<<L"="<<setting[HIDE_TIPS]<<L"\n";
		fs<<L"\n";
		fs<<COMMENT<<L" Sets the number of lines to save for rpgsh history.\n";
		fs<<COMMENT<<L" Default: "<<setting[HISTORY_LENGTH]<<L"\n";
		fs<<HISTORY_LENGTH<<L"="<<setting[HISTORY_LENGTH]<<L"\n";
		fs<<L"\n";
		fs<<COMMENT<<L" Sets the alias(es) to run a given command.\n";
		fs<<COMMENT<<L" Formatted like a variable set where the keys are the aliases and the values are the commands to run when the alias is ran.\n";
		fs<<COMMENT<<L" Default: "<<setting[ALIASES]<<L"\n";
		fs<<ALIASES<<L"="<<setting[ALIASES]<<L"\n";
		fs<<L"\n";
		fs<<COMMENT<<L" Comma-delimited list of programs to run when rpgsh launches.\n";
		fs<<COMMENT<<L" Default: "<<setting[STARTUP_APPS]<<L"\n";
		fs<<STARTUP_APPS<<L"="<<setting[STARTUP_APPS]<<L"\n";
		fs<<L"\n";
		fs<<COMMENT<<L" Comma-delimited list of programs to run after a command is entered into the prompt.\n";
		fs<<COMMENT<<L" Default: "<<setting[POST_RUN_APPS]<<L"\n";
		fs<<STARTUP_APPS<<L"="<<setting[POST_RUN_APPS]<<L"\n";
		fs.close();
	}
	std::wifstream fs(config_path.c_str());
	while(!fs.eof())
	{
		std::wstring data;
		std::getline(fs,data);
		if(data.length() && data[0] != COMMENT)//TODO: Setup for inline comments
			setting[getConfigItem(data)] = getConfigValue(data);
	}
	fs.close();
}
