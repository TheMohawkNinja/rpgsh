#pragma once

#include <map>
#include <string>
//#include <type_traits>//std::is_same<T,U>
#include "currency.h"
#include "define.h"
#include "dice.h"
#include "output.h"
#include "var.h"

class Scope
{
	protected:
		std::string datasource;

		datamap<Currency>		currencies;
		datamap<CurrencySystem>		currencysystems;
		datamap<Dice>			dice;
		datamap<Var>			vars;
		datamap<Wallet>			wallets;

		//Try to create a filestream
		template<typename T>
		T tryCreateFileStream(std::string path);

		//Save formatting
		std::string formatLine(char type, std::string k, std::string v);

	public:
		//Confirm datasource exists
		bool confirmDatasource();

		//Load/save data
		void load();
		void save();

		//Get single variable
		template <typename T>
		T get(std::string key);

		//Get single variable, but return a std::string
		template <typename T>
		std::string getStr(std::string key);

		//Get all variables of a specific datatype
		template <typename T>
		datamap<T> getDatamap();

		//Set single variable
		/*
		May need to check key[1] as the type of 'T'
		may not necessarily be the same as the type of the map that "value"
		is being applied to.
		*/
		template <typename T>
		void set(std::string key, T value);

		//Set entire datamap to another datamap
		template <typename T>
		void setDatamap(datamap<T> map);
};

class Character: public Scope
{
	public:
		//Load current character
		Character(bool backup);

		//Load other character/template
		Character(std::string path);

		//Get current character file path
		std::string getCurrentCharacterFilePath();

		//Get character name
		std::string getName();

		//Set datasource to save, mainly to ensure new characters don't save back to template file
		void setDatasource(std::string path);

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
