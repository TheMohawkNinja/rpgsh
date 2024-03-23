#pragma once

class RPGSH_CONFIG
{
	// Configuration file definition:
	//
	// Name=Value
	// Ignore '#' for use as comments
	public:
		std::string default_value = "";
		std::map<std::string,std::string> setting;

	private:
	std::string get_config_item(std::string s);
	std::string get_config_value(std::string s);

	public:
	RPGSH_CONFIG();
};
