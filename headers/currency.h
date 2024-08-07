#pragma once

#include <map>
#include <memory>
#include <string>
#include "define.h"

//External forward declarations
class Dice;
class Var;

//Internal forward declarations
class CurrencySystem;
class Wallet;

class Currency
{
	public:
		std::shared_ptr<CurrencySystem> System = nullptr;
		std::string Name = "";
		int SmallerAmount = 0;
		std::string Smaller = "";
		std::string Larger = "";

	void AttachToCurrencySystem(std::shared_ptr<CurrencySystem> _CurrencySystem);

	private:

	//Parsing methods for Currency(datamap<CurrencySystem, std::string)
	void tryParseCurrencySystem(datamap<CurrencySystem> currencysystems, std::string* str, std::string fullstr);
	void tryParseName(std::string* str, std::string fullstr);
	void tryParseSmallerAmount(std::string* str, std::string fullstr);
	void tryParseSmaller(std::string* str, std::string fullstr);
	void tryParseLarger(std::string* str, std::string fullstr);

	public:
	Currency();
	Currency(std::string str);
	Currency(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);
	Currency(std::shared_ptr<CurrencySystem> _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);

	explicit operator std::string() const;
	const char* c_str() const;

	Currency& operator ++ (int);
	Currency& operator -- (int);
	bool operator == (const Currency& b) const;
	bool operator < (const Currency& b) const;
	bool operator != (const Currency& b) const;
};
class CurrencySystem
{
	public:
		std::string Name = "";
		std::map<std::string, Currency> Denomination;
		Currency transaction = Currency();

	explicit operator std::string() const;
	const char* c_str() const;

	Currency& operator [] (const std::string b);
	CurrencySystem& operator ++ (int);
	CurrencySystem& operator -- (int);
	bool operator == (const CurrencySystem& b) const;
	bool operator != (const CurrencySystem& b) const;

	//Iterator type for the class
	using iterator = typename std::map<std::string, Currency>::const_iterator;

	//Beginning and end iterators.
	//So I can use "for(const auto& [key,val] : CurrencySystem){}"
	std::map<std::string, Currency>::const_iterator begin() const;
	std::map<std::string, Currency>::const_iterator end() const;

	CurrencySystem();
	CurrencySystem(std::string str);

	void MakeChange(Currency c, Wallet* w);
	void TradeUp(std::shared_ptr<CurrencySystem> S, Wallet* w);
	bool HasEquivalentTo(int Quantity, std::string Denomination);
};
typedef std::pair<Currency, int> money_t;
class Wallet
{
	public:
		std::map<Currency, int> Money;
		std::pair<Currency,int> transaction;

	bool HasEffectivelyAtLeast(int q, Currency c);
	void print();
	void FloatQuantityToIntCurrency(Currency c, float q);
	bool containsCurrency(std::string currency_str);
	Currency getExistingCurrency(std::string currency_str);

	explicit operator std::string() const;
	const char* c_str() const;

	int& operator [] (const Currency b);
	bool operator == (const Wallet& b) const;
	bool operator != (const Wallet& b) const;

	//Iterator for the class
	using iterator = typename std::map<Currency, int>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : Wallet){}"
	std::map<Currency, int>::const_iterator begin() const;
	std::map<Currency, int>::const_iterator end() const;

	Wallet();
	Wallet(const money_t m);
	Wallet(std::string s);

	Wallet& operator = (const unsigned int b);
	Wallet& operator = (const money_t b);
	Wallet& operator = (const Dice b);
	Wallet& operator = (const Var b);
	Wallet& operator -= (const unsigned int b);
	Wallet& operator -= (const money_t b);
	Wallet& operator -= (const Wallet b);
	Wallet& operator -= (const Dice b);
	Wallet& operator -= (const Var b);
	Wallet& operator += (const unsigned int b);
	Wallet& operator += (const money_t b);
	Wallet& operator += (const Wallet b);
	Wallet& operator += (const Dice b);
	Wallet& operator += (const Var b);
	Wallet& operator *= (const unsigned int b);
	Wallet& operator *= (const money_t b);
	Wallet& operator *= (const Wallet b);
	Wallet& operator *= (const Dice b);
	Wallet& operator *= (const Var b);
	Wallet& operator /= (const unsigned int b);
	Wallet& operator /= (const money_t b);
	Wallet& operator /= (const Wallet b);
	Wallet& operator /= (const Dice b);
	Wallet& operator /= (const Var b);
	Wallet& operator ++ ();
	Wallet& operator ++ (int);
	Wallet& operator -- ();
	Wallet& operator -- (int);
};
