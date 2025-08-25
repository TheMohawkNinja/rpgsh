#pragma once

#include <map>
#include <string>

class Configuration
{
	// Configuration file definition:
	//
	// Name=Value
	// Ignore COMMENT for use as comments
	public:
		std::map<std::string,std::string> setting;

	private:
	std::string getConfigurationItem(std::string s);
	std::string getConfigurationValue(std::string s);

	public:
	Configuration();
};
