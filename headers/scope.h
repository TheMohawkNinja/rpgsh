#pragma once

#include <map>
#include <string>
#include "currency.h"
#include "define.h"
#include "dice.h"
#include "functions.h"
#include "var.h"

class Scope
{
	protected:
		std::string datasource;

		datamap<Currency>		currencies;
		datamap<Dice>			dice;
		datamap<Var>			vars;
		datamap<Wallet>			wallets;

		//Try to create a filestream
		template<typename T>
		T tryCreateFileStream(std::string path);

		//Save formatting
		std::string formatLine(char type, std::string k, std::string v);

		//Default campaign/character loader
		void confirmDefaultCampaign();

	public:
		char sigil = '/';

		//Confirm datasource exists
		bool confirmDatasource();

		//Get/set datasource
		std::string getDatasource();
		void setDatasource(std::string path);

		//Does the scope have any data?
		bool isEmpty();

		//Check if key exists
		template <typename T>
		bool keyExists(std::string key);

		//Get existing case-insensitive key match (if applicable)
		template <typename T>
		std::string getExistingKey(std::string key);

		//Get single variable
		template <typename T>
		T get(std::string key);

		//Get single variable, but return a std::string
		template <typename T>
		std::string getStr(std::string key);

		//Get property from a variable
		template <typename T>
		std::string getProperty(std::string key, std::string property);

		//Get all variables of a specific datatype
		template <typename T>
		datamap<T> getDatamap();

		//Set single variable
		template <typename T>
		void set(std::string key, T value);

		//Set a property
		template<typename TK, typename TP>
		void setProperty(std::string key, std::string property, TP value);

		//Set entire datamap to another datamap
		template <typename T>
		void setDatamap(datamap<T> map);

		//Delete single variable
		template <typename T>
		bool remove(std::string key);

		//Load/save data
		void load(std::string path, bool loadVar, bool loadDice, bool loadCurrency, bool loadWallet);
		void load();
		void load(std::string path);
		void save();
};

class Character: public Scope
{
	public:
		//Load current character
		Character(bool backup=false);

		//Load other character/template
		Character(std::string path);

		//Get current character file path
		std::string getCurrentCharacterFilePath();

		//Get character name
		std::string getName();
		};

class Campaign: public Scope
{
	public:
		//Load current campaign
		Campaign();

		//Load other campaign
		Campaign(std::string path);
};

class Shell: public Scope
{
	public:
		//Load shell variables
		Shell();
};
