#include <cstring>
#include <filesystem>
#include <fstream>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/scope.h"
#include "../headers/string.h"

//			//
//	BASE CLASS	//
//			//

void Scope::confirmDefaultCampaign()
{
	if(!std::filesystem::exists(campaigns_dir))
		std::filesystem::create_directory(campaigns_dir);
	if(!std::filesystem::exists(campaigns_dir + "default"))
		std::filesystem::create_directory(campaigns_dir + "default");
	if(!std::filesystem::exists(campaigns_dir + "default/characters"))
		std::filesystem::create_directory(campaigns_dir + "default/characters");

	return;
}

//Try to create a filestream
template<>
std::ifstream Scope::tryCreateFileStream<std::ifstream>(std::string path)
{
	if(!std::filesystem::exists(path)) throw std::runtime_error(E_FILE_NOT_FOUND);

	std::ifstream fs(path);
	if(!fs.good()) throw std::runtime_error(E_BAD_FS);

	return fs;
}
template<>
std::ofstream Scope::tryCreateFileStream<std::ofstream>(std::string path)
{
	//Open as an appending stream
	std::ofstream fs(path, std::ios::app);
	if(!fs.good()) throw std::runtime_error(E_BAD_FS);

	return fs;
}

//Confirm datasource exists
bool Scope::confirmDatasource()
{
	return std::filesystem::exists(datasource);
}

//Get/set datasource
std::string Scope::getDatasource()
{
	return datasource;
}
void Scope::setDatasource(std::string path)
{
	datasource = path;
}

//Does the scope have any data?
bool Scope::isEmpty()
{
	return !(currencies.size()+dice.size()+vars.size()+wallets.size());
}

//Check if key exists
template<>
bool Scope::keyExists<Currency>(std::string key)
{
	for(const auto& [k,v] : currencies)
		if(!stringcasecmp(k,key)) return true;

	return false;
}
template<>
bool Scope::keyExists<Dice>(std::string key)
{
	for(const auto& [k,v] : dice)
		if(!stringcasecmp(k,key)) return true;

	return false;
}
template<>
bool Scope::keyExists<Var>(std::string key)
{
	for(const auto& [k,v] : vars)
		if(!stringcasecmp(k,key)) return true;

	return false;
}
template<>
bool Scope::keyExists<Wallet>(std::string key)
{
	for(const auto& [k,v] : wallets)
		if(!stringcasecmp(k,key)) return true;

	return false;
}

//Get existing case-insensitive key match (if applicable)
template<>
std::string Scope::getExistingKey<Currency>(std::string key)
{
	for(const auto& [k,v] : currencies)
		if(!stringcasecmp(k,key)) return k;

	return key;
}
template<>
std::string Scope::getExistingKey<Dice>(std::string key)
{
	for(const auto& [k,v] : dice)
		if(!stringcasecmp(k,key)) return k;

	return key;
}
template<>
std::string Scope::getExistingKey<Var>(std::string key)
{
	for(const auto& [k,v] : vars)
		if(!stringcasecmp(k,key)) return k;

	return key;
}
template<>
std::string Scope::getExistingKey<Wallet>(std::string key)
{
	for(const auto& [k,v] : wallets)
		if(!stringcasecmp(k,key)) return k;

	return key;
}

//Get single variable
template<>
Currency Scope::get<Currency>(std::string key)
{
	return currencies[getExistingKey<Currency>(key)];
}
template<>
Dice Scope::get<Dice>(std::string key)
{
	return dice[getExistingKey<Dice>(key)];
}
template<>
Var Scope::get<Var>(std::string key)
{
	return vars[getExistingKey<Var>(key)];
}
template<>
Wallet Scope::get<Wallet>(std::string key)
{
	return wallets[getExistingKey<Wallet>(key)];
}

//Get single variable, but return a std::string
template<>
std::string Scope::getStr<Currency>(std::string key)
{
	return std::string(Scope::get<Currency>(key));
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
		return get<Currency>(key).System;
	else if(!stringcasecmp(property,"SmallerAmount"))
		return std::to_string(get<Currency>(key).SmallerAmount);
	else if(!stringcasecmp(property,"Name"))
		return get<Currency>(key).Name;
	else if(!stringcasecmp(property,"Smaller"))
		return get<Currency>(key).Smaller;
	else if(!stringcasecmp(property,"Larger"))
		return get<Currency>(key).Larger;

	throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
std::string Scope::getProperty<Dice>(std::string key, std::string property)
{
	if(!stringcasecmp(property,"Quantity"))
		return std::to_string(get<Dice>(key).Quantity);
	else if(!stringcasecmp(property,"Faces"))
		return std::to_string(get<Dice>(key).Faces);
	else if(!stringcasecmp(property,"Modifier"))
		return std::to_string(get<Dice>(key).Modifier);
	else if(!stringcasecmp(property,"List"))
		return get<Dice>(key).List;

	throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
std::string Scope::getProperty<Var>(std::string key, std::string property)
{
	if(!stringcasecmp(property,"Value"))
		return get<Var>(key).Value;

	throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
std::string Scope::getProperty<Wallet>(std::string key, std::string property)
{
	for(const auto& m : get<Wallet>(key).Monies)
	{
		if(!stringcasecmp(property,m.c.Name))
			return std::to_string(m.q);
	}

	throw std::runtime_error(E_INVALID_PROPERTY);
}

//Get all variables of a specific datatype
template<>
datamap<Currency> Scope::getDatamap<Currency>()
{
	return currencies;
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
	currencies[getExistingKey<Currency>(key)] = value;
}
template<>
void Scope::set<Dice>(std::string key, Dice value)
{
	dice[getExistingKey<Dice>(key)] = value;
}
template<>
void Scope::set<Var>(std::string key, Var value)
{
	vars[getExistingKey<Var>(key)] = value;
}
template<>
void Scope::set<Wallet>(std::string key, Wallet value)
{
	wallets[getExistingKey<Wallet>(key)] = value;
}

//Set a property
template<>
void Scope::setProperty<Currency,int>(std::string key, std::string property, int value)
{
	if(!stringcasecmp(property,"SmallerAmount"))
		currencies[getExistingKey<Currency>(key)].SmallerAmount = value;
	else
		throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
void Scope::setProperty<Currency,std::string>(std::string key, std::string property, std::string value)
{
	if(!stringcasecmp(property,"CurrencySystem"))
		currencies[getExistingKey<Currency>(key)].System = value;
	else if(!stringcasecmp(property,"Name"))
		currencies[getExistingKey<Currency>(key)].Name = value;
	else if(!stringcasecmp(property,"Smaller"))
		currencies[getExistingKey<Currency>(key)].Smaller = value;
	else if(!stringcasecmp(property,"Larger"))
		currencies[getExistingKey<Currency>(key)].Larger = value;
	else
		throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
void Scope::setProperty<Dice,int>(std::string key, std::string property, int value)
{
	if(!stringcasecmp(property,"Quantity"))
		dice[getExistingKey<Dice>(key)].Quantity = value;
	else if(!stringcasecmp(property,"Faces"))
		dice[getExistingKey<Dice>(key)].Faces = value;
	else if(!stringcasecmp(property,"Modifier"))
		dice[getExistingKey<Dice>(key)].Modifier = value;
	else
		throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
void Scope::setProperty<Dice,std::string>(std::string key, std::string property, std::string value)
{
	if(!stringcasecmp(property,"List"))
		dice[getExistingKey<Dice>(key)].List = value;
	else
		throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
void Scope::setProperty<Var,std::string>(std::string key, std::string property, std::string value)
{
	if(!stringcasecmp(property,"Value"))
		vars[getExistingKey<Var>(key)].Value = value;
	else
		throw std::runtime_error(E_INVALID_PROPERTY);
}
template<>
void Scope::setProperty<Wallet,int>(std::string key, std::string property, int value)
{
	if(get<Wallet>(key).containsCurrency(property))
		wallets[getExistingKey<Wallet>(key)].set(wallets[getExistingKey<Wallet>(key)].getExistingCurrency(property),value);
	else
		throw std::runtime_error(E_INVALID_PROPERTY);
}

//Set entire datamap to another datamap
template<>
void Scope::setDatamap<Currency>(datamap<Currency> map)
{
	currencies = map;
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

//Delete single variable
template<>
bool Scope::remove<Currency>(std::string key)
{
	return currencies.erase(getExistingKey<Currency>(key));
}
template<>
bool Scope::remove<Dice>(std::string key)
{
	return dice.erase(getExistingKey<Dice>(key));
}
template<>
bool Scope::remove<Var>(std::string key)
{
	return vars.erase(getExistingKey<Var>(key));
}
template<>
bool Scope::remove<Wallet>(std::string key)
{
	return wallets.erase(getExistingKey<Wallet>(key));
}

//Save file formatting
std::string Scope::formatLine(char type, std::string k, std::string v)
{
	std::string type_str(1,type);
	return (type_str + DS + k + DS + v + "\n");
}

//Load all data in from file
void Scope::load(std::string path, bool loadVar, bool loadDice, bool loadCurrency, bool loadWallet)
{
	//Start from a clean slate
	currencies.clear();
	dice.clear();
	vars.clear();
	wallets.clear();

	//Open file
	std::ifstream ifs;
	try
	{
		ifs = tryCreateFileStream<std::ifstream>(path);
	}
	catch(const std::runtime_error& e)
	{
		throw;
	}

	//Load in the data
	int linenum = 0;
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
		if(findu(data,DS) > 1)
		{
			output(Warning,"Multi-character type specifier \"%s\" at \"%s:%d\", skipping line...",left(data,findu(data,DS)).c_str(),path.c_str(),linenum);
			continue;
		}

		if(data == "" || data[0] == COMMENT) continue;

		type = data[0];
		try
		{
			key = data.substr(1+DS.length(),(rfindu(data,DS)-(1+DS.length())));
		}
		catch(...)
		{
			output(Warning,"Unable to get variable key at \"%s:%d\", skipping line...",path.c_str(),linenum);
			break;
		}

		try
		{
			value = right(data,rfindu(data,DS)+DS.length());
		}
		catch(...)
		{
			output(Warning,"Unable to get variable value at \"%s:%d\", skipping line...",path.c_str(),linenum);
			break;
		}

		try
		{
			if(type == CURRENCY_SIGIL && loadCurrency)
				currencies[key] = Currency(value);
			else if(type == DICE_SIGIL && loadDice)
				dice[key] = Dice(value);
			else if(type == VAR_SIGIL && loadVar)
				vars[key] = Var(value);
			else if(type == WALLET_SIGIL && loadWallet)
				wallets[key] = Wallet(value);
			else if(!isTypeSigil(type))
				output(Warning,"Unknown type specifier \'%c\' in \"%s:%d\"",type,path.c_str(),linenum);
		}
		catch(const std::runtime_error& e)
		{
			output(Error,"Unable to load \"%s\" into the character at \"%s:%d\": %s.",data.c_str(),path.c_str(),linenum,e.what());
			exit(-1);
		}
	}
	ifs.close();
}
void Scope::load()
{
	try
	{
		load(datasource,true,true,true,true);
	}
	catch(const std::runtime_error& e)
	{
		throw;
	}
}
void Scope::load(std::string path)
{
	datasource = path;
	try{load();}
	catch(const std::runtime_error& e){throw;}
}

//Save all data to file
void Scope::save()
{
	//Make backup first
	if(std::filesystem::exists(datasource.c_str()) && right(datasource,4) != ".bak")
		 std::filesystem::rename(datasource.c_str(),(datasource+".bak").c_str());

	//Check for name change with character scopes
	if(datasource.substr(datasource.length()-5,5) == c_ext || datasource.substr(datasource.length()-9,9) == (c_ext+".bak"))
	{
		std::string old_name = right(left(datasource,rfindu(datasource,c_ext)),rfindu(datasource,'/')+1);
		std::string new_name = getProperty<Var>(getProperty<Var>(std::string(DOT_NAME),"Value"),"Value");
		if(old_name != new_name)
		{
			std::filesystem::remove(datasource);
			std::filesystem::remove(datasource+".bak");
			datasource = left(datasource,rfindu(datasource,'/')+1) + new_name + c_ext;
			setEnvVariable(ENV_CURRENT_CHARACTER,new_name);
		}
	}

	std::ofstream ofs;

	try
	{
		ofs = tryCreateFileStream<std::ofstream>(datasource);
	}
	catch(const std::runtime_error& e)
	{
		output(Error,"Exception thrown while attempting to save character: %s",e.what());
		throw;
	}

	for(const auto& [k,v] : currencies)
		ofs<<formatLine(CURRENCY_SIGIL,k,std::string(v));
	for(const auto& [k,v] : dice)
		ofs<<formatLine(DICE_SIGIL,k,std::string(v));
	for(const auto& [k,v] : vars)
		ofs<<formatLine(VAR_SIGIL,k,std::string(v));
	for(const auto& [k,v] : wallets)
		ofs<<formatLine(WALLET_SIGIL,k,std::string(v));

	ofs.close();
}

//			//
//	CHARACTER	//
//			//

Character::Character(bool backup)
{
	sigil = CHARACTER_SIGIL;
	datasource = getCurrentCharacterFilePath();
	if(backup) datasource += ".bak";

	if(confirmDatasource())
	{
		load();
	}
	else
	{
		confirmDefaultCampaign();
		Config config = Config();
		load(templates_dir + config.setting[DEFAULT_GAME]);
		datasource = campaigns_dir + "default/characters/" + getName() + c_ext;
		std::filesystem::copy_options co = std::filesystem::copy_options::update_existing;
		std::filesystem::copy(templates_dir + config.setting[DEFAULT_GAME],datasource,co);
		setEnvVariable(ENV_CURRENT_CHARACTER,getName());
		save();
	}
}
Character::Character(std::string path)
{
	sigil = CHARACTER_SIGIL;

	load(path);
}

std::string Character::getCurrentCharacterFilePath()
{
	std::string character = getEnvVariable(ENV_CURRENT_CHARACTER);
	std::string campaign = getEnvVariable(ENV_CURRENT_CAMPAIGN);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_character_dir = current_campaign_dir+"characters/";

	return (current_character_dir + character + c_ext);
}

//Get/set character name
std::string Character::getName()
{
	return getProperty<Var>(getProperty<Var>(std::string(DOT_NAME),"Value"),"Value");
}
void Character::setName(std::string name)
{
	set<Var>(getProperty<Var>(DOT_NAME,"Value"),name);
}

//			//
//	CAMPAIGN	//
//			//

Campaign::Campaign()
{
	confirmDefaultCampaign();
	sigil = CAMPAIGN_SIGIL;
	datasource = campaigns_dir +
		     getEnvVariable(ENV_CURRENT_CAMPAIGN) +
		     variables_file_name;

	if(!confirmDatasource()) save();

	load();
}
Campaign::Campaign(std::string path)
{
	sigil = CAMPAIGN_SIGIL;

	load(path);
}

//			//
//	SHELL		//
//			//

Shell::Shell()
{
	sigil = SHELL_SIGIL;
	confirmShellVariablesFile();

	load(shell_variables_path);
}
