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
		T get(std::string key)
		{
			if(std::is_same<T,currency_t>::value)
				return currencies[key];
			else if(std::is_same<T,currencysystem_t>::value)
				return currencysystems[key];
			else if(std::is_same<T,dice_t>::value)
				return dice[key];
			else if(std::is_same<T,var_t>::value)
				return vars[key];
			else if(std::is_same<T,wallet_t>::value)
				return wallets[key];
			else//Unsupported data type
				throw;
		}

		//Get all variables of a specific datatype
		template <typename T>
		datamap<T> getDatamap()
		{
			if(std::is_same<T,currency_t>::value)
				return currencies;
			else if(std::is_same<T,currencysystem_t>::value)
				return currencysystems;
			else if(std::is_same<T,dice_t>::value)
				return dice;
			else if(std::is_same<T,var_t>::value)
				return vars;
			else if(std::is_same<T,wallet_t>::value)
				return wallets;
			else//Unsupported data type
				throw;
		}

		//Set single variable
		template <typename T>
		void set(std::string key, T value)
		{
			/*
			Switching key[1] instead of std::is_same as the type of 'T'
			may not necessarily be the same as the type of the map that "value"
			is being applied to.
			*/

			switch(key[1])
			{
				case CURRENCY_SIGIL:
					currencies[key] = value;
					break;
				case CURRENCYSYSTEM_SIGIL:
					currencysystems[key] = value;
					break;
				case DICE_SIGIL:
					dice[key] = value;
					break;
				case VAR_SIGIL:
				case '/'://If no type sigil is used, assume var
					vars[key] = value;
					break;
				case WALLET_SIGIL:
					wallets[key] = value;
					break;
				default://Unsupported data type
					throw;
			}
		}

		//Set entire datamap to another datamap
		template <typename T>
		void setDatamap(datamap<T> map)
		{
			if(std::is_same<T,currency_t>::value)
				currencies = map;
			else if(std::is_same<T,currencysystem_t>::value)
				currencysystems = map;
			else if(std::is_same<T,dice_t>::value)
				dice = map;
			else if(std::is_same<T,var_t>::value)
				vars = map;
			else if(std::is_same<T,wallet_t>::value)
				wallets= map;
			else//Unsupported data type
				throw;
		}
};
