#include <filesystem>
#include "../headers/functions.h"
#include "../headers/scope.h"

//Try to create a filestream
template<>
std::ifstream scope::tryCreateFileStream<std::ifstream>(std::string path)
{
	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \"%s\"",path.c_str());
		exit(-1);
	}

	std::ifstream fs(path);
	if(!fs.good())
	{
		output(Error,"Unable to open \"%s\" for reading",path.c_str());
		exit(-1);
	}

	return fs;
}
template<>
std::ofstream scope::tryCreateFileStream<std::ofstream>(std::string path)
{
	//Open as an appending stream
	std::ofstream fs(path, std::ios::app);
	if(!fs.good())
	{
		output(Error,"Unable to open \"%s\" for writing",path.c_str());
		exit(-1);
	}

	return fs;
}

//Constructors
scope::scope(){}
scope::scope(scope_level level)
{
	std::string character = get_shell_var(CURRENT_CHAR_SHELL_VAR);
	std::string campaign = get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_character_dir = current_campaign_dir+"characters/";

	switch(level)
	{
		case Character:
			datasource = current_character_dir + character + ".char";
		case Campaign:
			datasource = current_campaign_dir + ".vars";
		case Shell:
			datasource = shell_vars_file;
	}

	load();
}

//Load all data in from file
void scope::load()
{
	std::ifstream ifs = tryCreateFileStream<std::ifstream>(datasource);
	int linenum = 0;

	while(!ifs.eof())
	{
		linenum++;
		std::string data = "";
		char type = '\0';
		std::string key = "";
		std::string value = "";

		std::getline(ifs,data);

		/*
		Handle multi-character type specifiers.
		This is neccessarily an error with the scope file formatting,
		but it is not neccessarily a fatal error, so just print a warning and keep going.
		*/
		if(data.find(DS) > 1)
		{
			output(Warning,"Multi-character type specifier \"%s\" at \"%s:%d\", skipping line...",data.substr(0,data.find(DS)).c_str(),datasource.c_str(),linenum);
			continue;
		}

		if(data != "" && data[0] == COMMENT)
		{
			type = data[0];
			try
			{
				key = data.substr(1+DS.length(),
						 (data.rfind(DS)-(1+DS.length())));
			}
			catch(...)
			{
				output(Warning,"Unable to get variable key at \"%s:%d\", skipping line...",datasource.c_str(),linenum);
				break;
			}

			try
			{
				value = data.substr(data.rfind(DS)+DS.length(),
						   (data.length()-data.rfind(DS)+DS.length()));
			}
			catch(...)
			{
				output(Warning,"Unable to get variable value at \"%s:%d\", skipping line...",datasource.c_str(),linenum);
				break;
			}

			//NOTE: This requires that a string constructor exists!
			switch(type)
			{
				case CURRENCY_SIGIL:
					currencies[key] = currency_t(value);
					break;
				case CURRENCYSYSTEM_SIGIL:
					currencysystems[key] = currencysystem_t(value);
					break;
				case DICE_SIGIL:
					dice[key] = dice_t(value);
					break;
				case VAR_SIGIL:
					vars[key] = var_t(value);
					break;
				case WALLET_SIGIL:
					wallets[key] = wallet_t(value);
					break;
				default:
					output(Warning,"Unknown type specifier \'%c\' in \"%s:%d\"",type,datasource.c_str(),linenum);
					break;
			}
		}
	}
	ifs.close();
}
//Save file formatting
void scope::saveline(std::ofstream &ofs, char type, std::string k, std::string v)
{
	std::string data = std::to_string(type) + DS + k + DS + v + "\n";
	ofs<<data;
}
//Save all data to file
void scope::save()
{
	//Make backup first
	if(std::filesystem::exists(datasource.c_str()))
		 std::filesystem::rename(datasource.c_str(),(datasource+".bak").c_str());

	std::ofstream ofs = tryCreateFileStream<std::ofstream>(datasource);
/*
	for(const auto& [k,v] : currencies)
		saveline(ofs,CURRENCY_SIGIL,k,std::string(v));
	for(const auto& [k,v] : currencysystems)
		saveline(ofs,CURRENCYSYSTEM_SIGIL,k,std::string(v));
	for(const auto& [k,v] : dice)
		saveline(ofs,DICE_SIGIL,k,std::string(v));
	for(const auto& [k,v] : vars)
		saveline(ofs,VAR_SIGIL,k,std::string(v));
	for(const auto& [k,v] : wallets)
		saveline(ofs,WALLET_SIGIL,k,std::string(v));
*/
	ofs.close();
}

//Get single variable
template<>
currency_t scope::get<currency_t>(std::string key)
{
	return currencies[key];
}
template<>
currencysystem_t scope::get<currencysystem_t>(std::string key)
{
	return currencysystems[key];
}
template<>
dice_t scope::get<dice_t>(std::string key)
{
	return dice[key];
}
template<>
var_t scope::get<var_t>(std::string key)
{
	return vars[key];
}
template<>
wallet_t scope::get<wallet_t>(std::string key)
{
	return wallets[key];
}

//Get single variable, but return a std::string
template<>
std::string scope::getStr<currency_t>(std::string key)
{
	return std::string(scope::get<currency_t>(key));
}
template<>
std::string scope::getStr<currencysystem_t>(std::string key)
{
	return std::string(scope::get<currencysystem_t>(key));
}
template<>
std::string scope::getStr<dice_t>(std::string key)
{
	return std::string(scope::get<dice_t>(key));
}
template<>
std::string scope::getStr<var_t>(std::string key)
{
	return std::string(scope::get<var_t>(key));
}
template<>
std::string scope::getStr<wallet_t>(std::string key)
{
	return std::string(scope::get<wallet_t>(key));
}

//Get all variables of a specific datatype
template<>
datamap<currency_t> scope::getDatamap<currency_t>()
{
	return currencies;
}
template<>
datamap<currencysystem_t> scope::getDatamap<currencysystem_t>()
{
	return currencysystems;
}
template<>
datamap<dice_t> scope::getDatamap<dice_t>()
{
	return dice;
}
template<>
datamap<var_t> scope::getDatamap<var_t>()
{
	return vars;
}
template<>
datamap<wallet_t> scope::getDatamap<wallet_t>()
{
	return wallets;
}

//Set single variable
template<>
void scope::set<currency_t>(std::string key, currency_t value)
{
	currencies[key] = value;
}
template<>
void scope::set<currencysystem_t>(std::string key, currencysystem_t value)
{
	currencysystems[key] = value;
}
template<>
void scope::set<dice_t>(std::string key, dice_t value)
{
	dice[key] = value;
}
template<>
void scope::set<var_t>(std::string key, var_t value)
{
	vars[key] = value;
}
template<>
void scope::set<wallet_t>(std::string key, wallet_t value)
{
	wallets[key] = value;
}

//Set entire datamap to another datamap
template<>
void scope::setDatamap<currency_t>(datamap<currency_t> map)
{
	currencies = map;
}
template<>
void scope::setDatamap<currencysystem_t>(datamap<currencysystem_t> map)
{
	currencysystems = map;
}
template<>
void scope::setDatamap<dice_t>(datamap<dice_t> map)
{
	dice = map;
}
template<>
void scope::setDatamap<var_t>(datamap<var_t> map)
{
	vars = map;
}
template<>
void scope::setDatamap<wallet_t>(datamap<wallet_t> map)
{
	wallets = map;
}
