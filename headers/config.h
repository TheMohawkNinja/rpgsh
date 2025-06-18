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
		std::map<std::wstring,std::wstring> setting;

	private:
	std::wstring getConfigItem(std::wstring s);
	std::wstring getConfigValue(std::wstring s);

	public:
	Config();
};
