#include <filesystem>
#include <fstream>
#include "../headers/functions.h"
#include "../headers/scope.h"

//			//
//	BASE CLASS	//
//			//

//Try to create a filestream
template<>
std::ifstream Scope::tryCreateFileStream<std::ifstream>(std::string path)
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
std::ofstream Scope::tryCreateFileStream<std::ofstream>(std::string path)
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

//Confirm datasource exists
bool Scope::confirmDatasource()
{
	return std::filesystem::exists(datasource);
}

//Load all data in from file
void Scope::load()
{
	//Start from a clean slate
	currencies.clear();
	currencysystems.clear();
	dice.clear();
	vars.clear();
	wallets.clear();

	//Open file
	std::ifstream ifs = tryCreateFileStream<std::ifstream>(datasource);
	int linenum = 0;

	//Load in the data
	while(!ifs.eof())
	{
		linenum++;
		std::string data = "";
		char type = '\0';
		std::string key = "";
		std::string value = "";

		std::getline(ifs,data);

		if(ifs.eof()) break;

		/*
		Handle multi-character type specifiers.
		This is neccessarily an error with the Scope file formatting,
		but it is not neccessarily a fatal error, so just print a warning and keep going.
		*/
		if(data.find(DS) > 1)
		{
			output(Warning,"Multi-character type specifier \"%s\" at \"%s:%d\", skipping line...",data.substr(0,data.find(DS)).c_str(),datasource.c_str(),linenum);
			continue;
		}

		if(data != "" && data[0] != COMMENT)
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
					currencies[key] = Currency(value);
					break;
				case CURRENCYSYSTEM_SIGIL:
					currencysystems[key] = CurrencySystem(value);
					break;
				case DICE_SIGIL:
					dice[key] = Dice(value);
					break;
				case VAR_SIGIL:
					vars[key] = Var(value);
					break;
				case WALLET_SIGIL:
					wallets[key] = Wallet(value);
					break;
				default:
					output(Warning,"Unknown type specifier \'%c\' in \"%s:%d\"",type,datasource.c_str(),linenum);
					break;
			}
		}
	}
	ifs.close();
}
void Scope::load(std::string path)
{
	datasource = path;
	load();
}
//Save file formatting
std::string Scope::formatLine(char type, std::string k, std::string v)
{
	std::string type_str(1,type);
	return (type_str + DS + k + DS + v + "\n");
}
//Save all data to file
void Scope::save()
{
	//Make backup first
	if(std::filesystem::exists(datasource.c_str()) &&
	datasource.substr(datasource.length()-4,4) != ".bak")
		 std::filesystem::rename(datasource.c_str(),(datasource+".bak").c_str());

	std::ofstream ofs = tryCreateFileStream<std::ofstream>(datasource);

	for(const auto& [k,v] : currencies)
		ofs<<formatLine(CURRENCY_SIGIL,k,std::string(v));
	for(const auto& [k,v] : currencysystems)
		ofs<<formatLine(CURRENCYSYSTEM_SIGIL,k,std::string(v));
	for(const auto& [k,v] : dice)
		ofs<<formatLine(DICE_SIGIL,k,std::string(v));
	for(const auto& [k,v] : vars)
		ofs<<formatLine(VAR_SIGIL,k,std::string(v));
	for(const auto& [k,v] : wallets)
		ofs<<formatLine(WALLET_SIGIL,k,std::string(v));

	ofs.close();
}

//Get single variable
template<>
Currency Scope::get<Currency>(std::string key)
{
	return currencies[key];
}
template<>
CurrencySystem Scope::get<CurrencySystem>(std::string key)
{
	return currencysystems[key];
}
template<>
Dice Scope::get<Dice>(std::string key)
{
	return dice[key];
}
template<>
Var Scope::get<Var>(std::string key)
{
	return vars[key];
}
template<>
Wallet Scope::get<Wallet>(std::string key)
{
	return wallets[key];
}

//Get single variable, but return a std::string
template<>
std::string Scope::getStr<Currency>(std::string key)
{
	return std::string(Scope::get<Currency>(key));
}
template<>
std::string Scope::getStr<CurrencySystem>(std::string key)
{
	return std::string(Scope::get<CurrencySystem>(key));
}
template<>
std::string Scope::getStr<Dice>(std::string key)
{
	return std::string(Scope::get<Dice>(key));
}
template<>
std::string Scope::getStr<Var>(std::string key)
{
	return std::string(Scope::get<Var>(key));
}
template<>
std::string Scope::getStr<Wallet>(std::string key)
{
	return std::string(Scope::get<Wallet>(key));
}

//Get property from a variable
template<>
std::string Scope::getProperty<Currency>(std::string key, std::string property)
{
	if(!stringcasecmp(property,"CurrencySystem"))
		return currencies[key].System->Name;
	else if(!stringcasecmp(property,"Name"))
		return currencies[key].Name;
	else if(!stringcasecmp(property,"SmallerAmount"))
		return std::to_string(currencies[key].SmallerAmount);
	else if(!stringcasecmp(property,"Smaller"))
		return currencies[key].Smaller;
	else if(!stringcasecmp(property,"Larger"))
		return currencies[key].Larger;

	output(Error,"\"%s\" is not a valid currency property.",property.c_str());
	exit(-1);
}
template<>
std::string Scope::getProperty<CurrencySystem>(std::string key, std::string property)
{
	if(!stringcasecmp(property,"Name"))
		return currencysystems[key].Name;

	output(Error,"\"%s\" is not a valid currency system property.",property.c_str());
	exit(-1);
}
template<>
std::string Scope::getProperty<Dice>(std::string key, std::string property)
{
	if(!stringcasecmp(property,"Quantity"))
		return std::to_string(dice[key].Quantity);
	else if(!stringcasecmp(property,"Faces"))
		return std::to_string(dice[key].Faces);
	else if(!stringcasecmp(property,"Modifier"))
		return std::to_string(dice[key].Modifier);
	else if(!stringcasecmp(property,"List"))
		return dice[key].List;

	output(Error,"\"%s\" is not a valid dice property.",property.c_str());
	exit(-1);
}
template<>
std::string Scope::getProperty<Var>(std::string key, std::string property)
{
	if(!stringcasecmp(property,"Value"))
		return vars[key].Value;

	output(Error,"\"%s\" is not a valid var property.",property.c_str());
	exit(-1);
}
template<>
std::string Scope::getProperty<Wallet>(std::string key, std::string property)
{
	for(const auto& [currency,quantity] : wallets[key])
	{
		if(!stringcasecmp(property,currency.Name))
			return std::to_string(quantity);
	}

	output(Error,"Currency \"%s\" not found in \"%s\".",property.c_str(),key.c_str());
	exit(-1);
}

//Get all variables of a specific datatype
template<>
datamap<Currency> Scope::getDatamap<Currency>()
{
	return currencies;
}
template<>
datamap<CurrencySystem> Scope::getDatamap<CurrencySystem>()
{
	return currencysystems;
}
template<>
datamap<Dice> Scope::getDatamap<Dice>()
{
	return dice;
}
template<>
datamap<Var> Scope::getDatamap<Var>()
{
	return vars;
}
template<>
datamap<Wallet> Scope::getDatamap<Wallet>()
{
	return wallets;
}

//Set single variable
template<>
void Scope::set<Currency>(std::string key, Currency value)
{
	currencies[key] = value;
}
template<>
void Scope::set<CurrencySystem>(std::string key, CurrencySystem value)
{
	currencysystems[key] = value;
}
template<>
void Scope::set<Dice>(std::string key, Dice value)
{
	dice[key] = value;
}
template<>
void Scope::set<Var>(std::string key, Var value)
{
	vars[key] = value;
}
template<>
void Scope::set<Wallet>(std::string key, Wallet value)
{
	wallets[key] = value;
}

//Set entire datamap to another datamap
template<>
void Scope::setDatamap<Currency>(datamap<Currency> map)
{
	currencies = map;
}
template<>
void Scope::setDatamap<CurrencySystem>(datamap<CurrencySystem> map)
{
	currencysystems = map;
}
template<>
void Scope::setDatamap<Dice>(datamap<Dice> map)
{
	dice = map;
}
template<>
void Scope::setDatamap<Var>(datamap<Var> map)
{
	vars = map;
}
template<>
void Scope::setDatamap<Wallet>(datamap<Wallet> map)
{
	wallets = map;
}

//			//
//	CHARACTER	//
//			//

Character::Character(bool backup)
{
	datasource = getCurrentCharacterFilePath();
	if(backup) datasource += ".bak";
	load();
}
Character::Character(std::string path)
{
	datasource = path;
	load();
}

std::string Character::getCurrentCharacterFilePath()
{
	std::string character = get_env_variable(CURRENT_CHAR_SHELL_VAR);
	std::string campaign = get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_character_dir = current_campaign_dir+"characters/";

	return (current_character_dir + character + ".char");
}

//Get character name
std::string Character::getName()
{
	return getProperty<Var>(getProperty<Var>(std::string(CHAR_NAME_ATTR),"Value"),"Value");
}

//Set datasource to save, mainly to ensure new characters don't save back to template file
void Character::setDatasource(std::string path)
{
	datasource = path;
}

//			//
//	CAMPAIGN	//
//			//

Campaign::Campaign()
{
	datasource = campaigns_dir+
		     get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
		     variable_file_name;
	load();
}
Campaign::Campaign(std::string path)
{
	datasource = path;
	load();
}

//			//
//	SHELL		//
//			//

Shell::Shell()
{
	confirmShellVariablesFile();
	datasource = shell_variables_file;
	load();
}
