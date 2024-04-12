#pragma once

#include <map>
#include <string>
#include "define.h"

//External forward declarations
class dice_t;
class var_t;

//Internal forward declarations
class currencysystem_t;
class wallet_t;

class currency_t
{
	public:
		currencysystem_t* System = nullptr;
		std::string Name = "";
		int SmallerAmount = 0;
		std::string Smaller = "";
		std::string Larger = "";

	void AttachToCurrencySystem(currencysystem_t* _CurrencySystem);

	private:
	//Parsing methods for currency_t(datamap<currencysystem_t, std::string)
	void tryParseCurrencySystem(datamap<currencysystem_t> currencysystems, std::string* str, std::string fullstr);
	void tryParseName(std::string* str, std::string fullstr);
	void tryParseSmallerAmount(std::string* str, std::string fullstr);
	void tryParseSmaller(std::string* str, std::string fullstr);
	void tryParseLarger(std::string* str, std::string fullstr);

	public:
	currency_t();
	currency_t(std::string str);
	currency_t(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);
	currency_t(currencysystem_t* _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);

	bool operator == (const currency_t& b) const;
	bool operator < (const currency_t& b) const;
};
class currencysystem_t
{
	public:
		std::string Name = "";
		std::map<std::string, currency_t> Denomination;
		currency_t transaction = currency_t();

	currency_t& operator [] (const std::string b);

	//Iterator type for the class
	using iterator = typename std::map<std::string, currency_t>::const_iterator;

	//Beginning and end iterators.
	//So I can use "for(const auto& [key,val] : currencysystem_t){}"
	std::map<std::string, currency_t>::const_iterator begin() const;
	std::map<std::string, currency_t>::const_iterator end() const;

	currencysystem_t();
	currencysystem_t(std::string str);

	void MakeChange(currency_t c, wallet_t* w);
	void TradeUp(currencysystem_t* S, wallet_t* w);
	bool HasEquivalentTo(int Quantity, std::string Denomination);
};
typedef std::pair<currency_t, int> money_t;
class wallet_t
{
	public:
		std::map<currency_t, int> Money;
		std::pair<currency_t,int> transaction;

	bool HasEffectivelyAtLeast(int q, currency_t c);
	void print(int tab);
	void FloatQuantityToIntCurrency(currency_t c, float q);

	int& operator [] (const currency_t b);

	//Iterator for the class
	using iterator = typename std::map<currency_t, int>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : wallet_t){}"
	std::map<currency_t, int>::const_iterator begin() const;
	std::map<currency_t, int>::const_iterator end() const;

	wallet_t();
	wallet_t(const money_t m);
	wallet_t(std::string s);

	wallet_t& operator = (const unsigned int b);
	wallet_t& operator = (const money_t b);
	wallet_t& operator = (const dice_t b);
	wallet_t& operator = (const var_t b);
	wallet_t& operator -= (const unsigned int b);
	wallet_t& operator -= (const money_t b);
	wallet_t& operator -= (const wallet_t b);
	wallet_t& operator -= (const dice_t b);
	wallet_t& operator -= (const var_t b);
	wallet_t& operator += (const unsigned int b);
	wallet_t& operator += (const money_t b);
	wallet_t& operator += (const wallet_t b);
	wallet_t& operator += (const dice_t b);
	wallet_t& operator += (const var_t b);
	wallet_t& operator *= (const unsigned int b);
	wallet_t& operator *= (const money_t b);
	wallet_t& operator *= (const wallet_t b);
	wallet_t& operator *= (const dice_t b);
	wallet_t& operator *= (const var_t b);
	wallet_t& operator /= (const unsigned int b);
	wallet_t& operator /= (const money_t b);
	wallet_t& operator /= (const wallet_t b);
	wallet_t& operator /= (const dice_t b);
	wallet_t& operator /= (const var_t b);
	wallet_t& operator ++ ();
	wallet_t& operator ++ (int);
	wallet_t& operator -- ();
	wallet_t& operator -- (int);

	bool operator != (const std::string b);//TODO: May need to revisit this for a possible wallet_t(std::string) constructor
};
