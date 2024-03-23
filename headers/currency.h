#pragma once

#include <map>
#include <string>

//External forward declarations
class RPGSH_DICE;
class RPGSH_VAR;

//Internal forward declarations
class RPGSH_CURRENCYSYSTEM;
class RPGSH_WALLET;

class RPGSH_CURRENCY
{
	public:
		RPGSH_CURRENCYSYSTEM* System = nullptr;
		std::string Name = "";
		int SmallerAmount = 0;
		std::string Smaller = "";
		std::string Larger = "";

	void AttachToCurrencySystem(RPGSH_CURRENCYSYSTEM* _CurrencySystem);

	RPGSH_CURRENCY();
	RPGSH_CURRENCY(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);
	RPGSH_CURRENCY(RPGSH_CURRENCYSYSTEM* _CS, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);

	bool operator == (const RPGSH_CURRENCY& b) const;
	bool operator < (const RPGSH_CURRENCY& b) const;
};
class RPGSH_CURRENCYSYSTEM
{
	public:
		std::map<std::string, RPGSH_CURRENCY> Denomination;
		RPGSH_CURRENCY transaction = RPGSH_CURRENCY();

	RPGSH_CURRENCY& operator [] (const std::string b);

	//Iterator type for the class
	using iterator = typename std::map<std::string, RPGSH_CURRENCY>::const_iterator;

	//Beginning and end iterators.
	//So I can use "for(const auto& [key,val] : RPGSH_CURRENCYSYSTEM){}"
	std::map<std::string, RPGSH_CURRENCY>::const_iterator begin() const;
	std::map<std::string, RPGSH_CURRENCY>::const_iterator end() const;

	RPGSH_CURRENCYSYSTEM();

	void MakeChange(RPGSH_CURRENCY c, RPGSH_WALLET* w);
	void TradeUp(RPGSH_CURRENCYSYSTEM* S, RPGSH_WALLET* w);
	bool HasEquivalentTo(int Quantity, std::string Denomination);
};
typedef std::pair<RPGSH_CURRENCY, int> money_t;
class RPGSH_WALLET
{
	public:
		std::map<RPGSH_CURRENCY, int> Money;
		std::pair<RPGSH_CURRENCY,int> transaction;

	bool HasEffectivelyAtLeast(int q, RPGSH_CURRENCY c);
	void print(int tab);
	void FloatQuantityToWholeCurrency(RPGSH_CURRENCY c, float q);

	int& operator [] (const RPGSH_CURRENCY b);

	//Iterator for the class
	using iterator = typename std::map<RPGSH_CURRENCY, int>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_WALLET){}"
	std::map<RPGSH_CURRENCY, int>::const_iterator begin() const;
	std::map<RPGSH_CURRENCY, int>::const_iterator end() const;

	RPGSH_WALLET();
	RPGSH_WALLET(const money_t m);
	RPGSH_WALLET(std::string s);//TODO: Assumes a format "<quantity> <currency>"

	RPGSH_WALLET& operator = (const unsigned int b);
	RPGSH_WALLET& operator = (const money_t b);
	RPGSH_WALLET& operator = (const RPGSH_DICE b);
	RPGSH_WALLET& operator = (const RPGSH_VAR b);
	RPGSH_WALLET& operator -= (const unsigned int b);
	RPGSH_WALLET& operator -= (const money_t b);
	RPGSH_WALLET& operator -= (const RPGSH_WALLET b);
	RPGSH_WALLET& operator -= (const RPGSH_DICE b);
	RPGSH_WALLET& operator -= (const RPGSH_VAR b);
	RPGSH_WALLET& operator += (const unsigned int b);
	RPGSH_WALLET& operator += (const money_t b);
	RPGSH_WALLET& operator += (const RPGSH_WALLET b);
	RPGSH_WALLET& operator += (const RPGSH_DICE b);
	RPGSH_WALLET& operator += (const RPGSH_VAR b);
	RPGSH_WALLET& operator *= (const unsigned int b);
	RPGSH_WALLET& operator *= (const money_t b);
	RPGSH_WALLET& operator *= (const RPGSH_WALLET b);
	RPGSH_WALLET& operator *= (const RPGSH_DICE b);
	RPGSH_WALLET& operator *= (const RPGSH_VAR b);
	RPGSH_WALLET& operator /= (const unsigned int b);
	RPGSH_WALLET& operator /= (const money_t b);
	RPGSH_WALLET& operator /= (const RPGSH_WALLET b);
	RPGSH_WALLET& operator /= (const RPGSH_DICE b);
	RPGSH_WALLET& operator /= (const RPGSH_VAR b);
	RPGSH_WALLET& operator ++ ();
	RPGSH_WALLET& operator ++ (int);
	RPGSH_WALLET& operator -- ();
	RPGSH_WALLET& operator -- (int);

	bool operator != (const std::string b);//TODO: May need to revisit this for a possible RPGSH_WALLET(std::string) constructor
};
