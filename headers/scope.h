#pragma once

#include <fstream>
#include <map>
#include <string>
#include <type_traits>//std::is_same<T,U>
#include "currency.h"
#include "define.h"
#include "dice.h"
#include "output.h"
#include "var.h"

enum scope_level
{
	Character,
	Campaign,
	Shell
};
class scope
{
	private:
		std::string datasource;
		scope_level level;

		datamap<currency_t>		currencies;
		datamap<currencysystem_t>	currencysystems;
		datamap<dice_t>			dice;
		datamap<var_t>			vars;
		datamap<wallet_t>		wallets;

		//Try to create a filestream
		template<typename T>
		T tryCreateFileStream(std::string path);

		//Save formatting
		void saveline(std::ofstream &ofs, char type, std::string k, std::string v);

	public:
		//Constructors
		scope();
		scope(scope_level level);

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
