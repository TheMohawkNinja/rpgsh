#pragma once

#include <map>
#include <string>

class Config
{
	// Configuration file definition:
	//
	// Name=Value
	// Ignore COMMENT for use as comments
	public:
		std::map<std::string,std::string> setting;

	private:
	std::string getConfigItem(std::string s);
	std::string getConfigValue(std::string s);

	public:
	Config();
};
