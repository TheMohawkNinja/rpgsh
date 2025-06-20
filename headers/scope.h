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
		std::wstring datasource;

		datamap<Currency>		currencies;
		datamap<Dice>			dice;
		datamap<Var>			vars;
		datamap<Wallet>			wallets;

		//Try to create a filestream
		template<typename T>
		T tryCreateFileStream(std::wstring path);

		//Save formatting
		std::wstring formatLine(char type, std::wstring k, std::wstring v);

		//Default campaign/character loader
		void confirmDefaultCampaign();

	public:
		char sigil = '/';

		//Confirm datasource exists
		bool confirmDatasource();

		//Get/set datasource
		std::wstring getDatasource();
		void setDatasource(std::wstring path);

		//Does the scope have any data?
		bool isEmpty();

		//Check if key exists
		template <typename T>
		bool keyExists(std::wstring key);

		//Get existing case-insensitive key match (if applicable)
		template <typename T>
		std::wstring getExistingKey(std::wstring key);

		//Get single variable
		template <typename T>
		T get(std::wstring key);

		//Get single variable, but return a std::wstring
		template <typename T>
		std::wstring getStr(std::wstring key);

		//Get property from a variable
		template <typename T>
		std::wstring getProperty(std::wstring key, std::wstring property);

		//Get all variables of a specific datatype
		template <typename T>
		datamap<T> getDatamap();

		//Set single variable
		template <typename T>
		void set(std::wstring key, T value);

		//Set a property
		template<typename TK, typename TP>
		void setProperty(std::wstring key, std::wstring property, TP value);

		//Set entire datamap to another datamap
		template <typename T>
		void setDatamap(datamap<T> map);

		//Delete single variable
		template <typename T>
		bool remove(std::wstring key);

		//Load/save data
		void load(std::wstring path, bool loadVar, bool loadDice, bool loadCurrency, bool loadWallet);
		void load();
		void load(std::wstring path);
		void save();
};

class Character: public Scope
{
	public:
		//Load current character
		Character(bool backup=false);

		//Load other character/template
		Character(std::wstring path);

		//Get current character file path
		std::wstring getCurrentCharacterFilePath();

		//Get/set character name
		std::wstring getName();
		void setName(std::wstring name);
};

class Campaign: public Scope
{
	public:
		//Load current campaign
		Campaign();

		//Load other campaign
		Campaign(std::wstring path);
};

class Shell: public Scope
{
	public:
		//Load shell variables
		Shell();
};
