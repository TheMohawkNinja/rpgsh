#pragma once

#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "define.h"

//External forward declarations
class Dice;
class Var;

//Internal forward declarations
class Currency;
class Wallet;

//Function declarations
datamap<Currency> getCurrencySystem(std::string system);
void MakeChange(Currency c, Wallet* w);
void TradeUp(std::string system, Wallet* w);

class Currency
{
	public:
		std::string System = "";
		std::string Name = "";
		int SmallerAmount = 0;
		std::string Smaller = "";
		std::string Larger = "";

	private:

	//Parsing methods for Currency(std::string)
	void tryParseCurrencySystem(std::string* str);
	void tryParseName(std::string* str);
	void tryParseSmallerAmount(std::string* str);
	void tryParseSmaller(std::string* str);
	void tryParseLarger(std::string* str);

	public:
	Currency();
	Currency(const Currency& b);
	Currency(std::string str);
	Currency(std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);
	Currency(std::string _System, std::string _Name, int _SmallerAmount, std::string _Smaller, std::string _Larger);

	explicit operator std::string() const;
	explicit operator bool() const;
	const char* c_str() const;
	Currency& operator = ([[maybe_unused]] const int b);
	Currency& operator = ([[maybe_unused]] const std::string b);
	Currency& operator = (const Var b);
	Currency& operator = ([[maybe_unused]] const Dice b);
	Currency& operator = ([[maybe_unused]] const Wallet b);
	Currency& operator = (const Currency b);
	Currency& operator += (const int b);
	Currency& operator += ([[maybe_unused]] const std::string b);
	Currency& operator += (const Var b);
	Currency& operator += ([[maybe_unused]] const Dice b);
	Wallet operator += (const Wallet b);
	Wallet operator += (const Currency b);
	Currency& operator -= (const int b);
	Currency& operator -= ([[maybe_unused]] const std::string b);
	Currency& operator -= (const Var b);
	Currency& operator -= ([[maybe_unused]] const Dice b);
	Currency& operator -= ([[maybe_unused]] const Wallet b);
	Currency& operator -= ([[maybe_unused]] const Currency b);
	Wallet operator *= (const int b);
	Wallet operator *= ([[maybe_unused]] const std::string b);
	Wallet operator *= (const Var b);
	Currency& operator *= ([[maybe_unused]] const Dice b);
	Currency& operator *= ([[maybe_unused]] const Wallet b);
	Currency& operator *= ([[maybe_unused]] const Currency b);
	Currency& operator /= ([[maybe_unused]] const int b);
	Currency& operator /= ([[maybe_unused]] const std::string b);
	Currency& operator /= (const Var b);
	Currency& operator /= ([[maybe_unused]] const Dice b);
	Currency& operator /= ([[maybe_unused]] const Wallet b);
	Currency& operator /= ([[maybe_unused]] const Currency b);
	Currency& operator ^= ([[maybe_unused]] const int b);
	Currency& operator ^= ([[maybe_unused]] const std::string b);
	Currency& operator ^= (const Var b);
	Currency& operator ^= ([[maybe_unused]] const Dice b);
	Currency& operator ^= ([[maybe_unused]] const Wallet b);
	Currency& operator ^= ([[maybe_unused]] const Currency b);
	Currency& operator %= ([[maybe_unused]] const int b);
	Currency& operator %= ([[maybe_unused]] const std::string b);
	Currency& operator %= (const Var b);
	Currency& operator %= ([[maybe_unused]] const Dice b);
	Currency& operator %= ([[maybe_unused]] const Wallet b);
	Currency& operator %= ([[maybe_unused]] const Currency b);
	Currency operator + (const int b);
	Currency operator + (const std::string b);
	Currency operator + (const Var b);
	Currency operator + (const Dice b);
	Wallet operator + (const Wallet b);
	Wallet operator + (const Currency b);
	Currency operator - (const int b);
	Currency operator - (const std::string b);
	Currency operator - (const Var b);
	Currency operator - (const Dice b);
	Currency operator - (const Wallet b);
	Currency operator - (const Currency b);
	Wallet operator * (const int b);
	Wallet operator * (const std::string b);
	Wallet operator * (const Var b);
	Currency operator * (const Dice b);
	Currency operator * (const Wallet b);
	Currency operator * (const Currency b);
	Currency operator / (const int b);
	Currency operator / (const std::string b);
	Currency operator / (const Var b);
	Currency operator / (const Dice b);
	Currency operator / (const Wallet b);
	Currency operator / (const Currency b);
	Currency operator ^ (const int b);
	Currency operator ^ (const std::string b);
	Currency operator ^ (const Var b);
	Currency operator ^ (const Dice b);
	Currency operator ^ (const Wallet b);
	Currency operator ^ (const Currency b);
	Currency operator % (const int b);
	Currency operator % (const std::string b);
	Currency operator % (const Var b);
	Currency operator % (const Dice b);
	Currency operator % (const Wallet b);
	Currency operator % (const Currency b);
	bool operator == ([[maybe_unused]] const int& b) const;
	bool operator == ([[maybe_unused]] const std::string& b) const;
	bool operator == (const Var& b) const;
	bool operator == ([[maybe_unused]] const Dice& b) const;
	bool operator == ([[maybe_unused]] const Wallet& b) const;
	bool operator == (const Currency& b) const;
	bool operator < ([[maybe_unused]] const int& b) const;
	bool operator < ([[maybe_unused]] const std::string& b) const;
	bool operator < (const Var& b) const;
	bool operator < ([[maybe_unused]] const Dice& b) const;
	bool operator < ([[maybe_unused]] const Wallet& b) const;
	bool operator < (const Currency& b) const;
	bool operator > ([[maybe_unused]] const int& b) const;
	bool operator > ([[maybe_unused]] const std::string& b) const;
	bool operator > (const Var& b) const;
	bool operator > ([[maybe_unused]] const Dice& b) const;
	bool operator > ([[maybe_unused]] const Wallet& b) const;
	bool operator > (const Currency& b) const;
	bool operator <= (const int& b) const;
	bool operator <= (const std::string& b) const;
	bool operator <= (const Var& b) const;
	bool operator <= (const Dice& b) const;
	bool operator <= (const Wallet& b) const;
	bool operator <= (const Currency& b) const;
	bool operator >= (const int& b) const;
	bool operator >= (const std::string& b) const;
	bool operator >= (const Var& b) const;
	bool operator >= (const Dice& b) const;
	bool operator >= (const Wallet& b) const;
	bool operator >= (const Currency& b) const;
	bool operator != (const int& b) const;
	bool operator != (const std::string& b) const;
	bool operator != (const Var& b) const;
	bool operator != (const Dice& b) const;
	bool operator != (const Wallet& b) const;
	bool operator != (const Currency& b) const;
	bool operator && (const int b);
	bool operator && (const std::string b);
	bool operator && (const Var b);
	bool operator && (const Dice b);
	bool operator && (const Wallet b);
	bool operator && (const Currency b);
	bool operator || (const int b);
	bool operator || (const std::string b);
	bool operator || (const Var b);
	bool operator || (const Dice b);
	bool operator || (const Wallet b);
	bool operator || (const Currency b);
	Currency& operator ++ (int);
	Currency& operator -- (int);
};
struct Money
{
	Currency c;
	int q;
};
class Wallet
{
	public:
		std::vector<Money> Monies;
		Money transaction;

	bool HasEffectivelyAtLeast(int q, Currency c);
	unsigned int getEquivalentValueInLowestDenomination(std::string system);
	void print();
	void FloatQuantityToIntCurrency(Currency c, float q);
	bool containsCurrency(std::string currency_str);
	Currency getExistingCurrency(std::string currency_str);

	explicit operator std::string() const;
	explicit operator bool() const;
	const char* c_str() const;

	int get(const Currency c);
	void set(const Currency c, const int q);

	Wallet();
	Wallet(const Wallet& b);
	Wallet(const Money m);
	Wallet(std::string s);
	Wallet& operator = ([[maybe_unused]] const int b);
	Wallet& operator = ([[maybe_unused]] const std::string b);
	Wallet& operator = (const Money b);
	Wallet& operator = (const Var b);
	Wallet& operator = ([[maybe_unused]] const Dice b);
	Wallet& operator = (const Wallet b);
	Wallet& operator = (const Currency b);
	Wallet& operator += ([[maybe_unused]] const int b);
	Wallet& operator += ([[maybe_unused]] const std::string b);
	Wallet& operator += (const Money b);
	Wallet& operator += (const Var b);
	Wallet& operator += ([[maybe_unused]] const Dice b);
	Wallet& operator += (const Wallet b);
	Wallet& operator += (const Currency b);
	Wallet& operator -= ([[maybe_unused]] const int b);
	Wallet& operator -= ([[maybe_unused]] const std::string b);
	Wallet& operator -= (const Money b);
	Wallet& operator -= (const Var b);
	Wallet& operator -= ([[maybe_unused]] const Dice b);
	Wallet& operator -= (const Wallet b);
	Wallet& operator -= (const Currency b);
	Wallet& operator *= (const int b);
	Wallet& operator *= ([[maybe_unused]] const std::string b);
	Wallet& operator *= ([[maybe_unused]] const Money b);
	Wallet& operator *= (const Var b);
	Wallet& operator *= ([[maybe_unused]] const Dice b);
	Wallet& operator *= ([[maybe_unused]] const Wallet b);
	Wallet& operator *= ([[maybe_unused]] const Currency b);
	Wallet& operator /= (const int b);
	Wallet& operator /= ([[maybe_unused]] const std::string b);
	Wallet& operator /= ([[maybe_unused]] const Money b);
	Wallet& operator /= (const Var b);
	Wallet& operator /= ([[maybe_unused]] const Dice b);
	Wallet& operator /= ([[maybe_unused]] const Wallet b);
	Wallet& operator /= ([[maybe_unused]] const Currency b);
	Wallet& operator ^= ([[maybe_unused]] const int b);
	Wallet& operator ^= ([[maybe_unused]] const std::string b);
	Wallet& operator ^= (const Var b);
	Wallet& operator ^= ([[maybe_unused]] const Dice b);
	Wallet& operator ^= ([[maybe_unused]] const Wallet b);
	Wallet& operator ^= ([[maybe_unused]] const Currency b);
	Wallet& operator %= (const int b);
	Wallet& operator %= ([[maybe_unused]] const std::string b);
	Wallet& operator %= (const Var b);
	Wallet& operator %= ([[maybe_unused]] const Dice b);
	Wallet& operator %= ([[maybe_unused]] const Wallet b);
	Wallet& operator %= ([[maybe_unused]] const Currency b);
	Wallet operator + (const int b);
	Wallet operator + (const std::string b);
	Wallet operator + (const Var b);
	Wallet operator + (const Dice b);
	Wallet operator + (const Wallet b);
	Wallet operator + (const Currency b);
	Wallet operator - (const int b);
	Wallet operator - (const std::string b);
	Wallet operator - (const Var b);
	Wallet operator - (const Dice b);
	Wallet operator - (const Wallet b);
	Wallet operator - (const Currency b);
	Wallet operator * (const int b);
	Wallet operator * (const std::string b);
	Wallet operator * (const Var b);
	Wallet operator * (const Dice b);
	Wallet operator * (const Wallet b);
	Wallet operator * (const Currency b);
	Wallet operator / (const int b);
	Wallet operator / (const std::string b);
	Wallet operator / (const Var b);
	Wallet operator / (const Dice b);
	Wallet operator / (const Wallet b);
	Wallet operator / (const Currency b);
	Wallet operator ^ (const int b);
	Wallet operator ^ (const std::string b);
	Wallet operator ^ (const Var b);
	Wallet operator ^ (const Dice b);
	Wallet operator ^ (const Wallet b);
	Wallet operator ^ (const Currency b);
	Wallet operator % (const int b);
	Wallet operator % (const std::string b);
	Wallet operator % (const Var b);
	Wallet operator % (const Dice b);
	Wallet operator % (const Wallet b);
	Wallet operator % (const Currency b);
	bool operator == ([[maybe_unused]] const int& b) const;
	bool operator == ([[maybe_unused]] const std::string& b) const;
	bool operator == (const Var& b) const;
	bool operator == ([[maybe_unused]] const Dice& b) const;
	bool operator == (const Wallet& b) const;
	bool operator == (const Currency& b) const;
	bool operator < ([[maybe_unused]] const int& b) const;
	bool operator < ([[maybe_unused]] const std::string& b) const;
	bool operator < (const Var& b) const;
	bool operator < ([[maybe_unused]] const Dice& b) const;
	bool operator < (const Wallet& b) const;
	bool operator < (const Currency& b) const;
	bool operator > ([[maybe_unused]] const int& b) const;
	bool operator > ([[maybe_unused]] const std::string& b) const;
	bool operator > (const Var& b) const;
	bool operator > ([[maybe_unused]] const Dice& b) const;
	bool operator > (const Wallet& b) const;
	bool operator > (const Currency& b) const;
	bool operator <= (const int& b) const;
	bool operator <= (const std::string& b) const;
	bool operator <= (const Var& b) const;
	bool operator <= (const Dice& b) const;
	bool operator <= (const Wallet& b) const;
	bool operator <= (const Currency& b) const;
	bool operator >= (const int& b) const;
	bool operator >= (const std::string& b) const;
	bool operator >= (const Var& b) const;
	bool operator >= (const Dice& b) const;
	bool operator >= (const Wallet& b) const;
	bool operator >= (const Currency& b) const;
	bool operator != (const int& b) const;
	bool operator != (const std::string& b) const;
	bool operator != (const Var& b) const;
	bool operator != (const Dice& b) const;
	bool operator != (const Wallet& b) const;
	bool operator != (const Currency& b) const;
	bool operator && (const int b);
	bool operator && (const std::string b);
	bool operator && (const Var b);
	bool operator && (const Dice b);
	bool operator && (const Wallet b);
	bool operator && (const Currency b);
	bool operator || (const int b);
	bool operator || (const std::string b);
	bool operator || (const Var b);
	bool operator || (const Dice b);
	bool operator || (const Wallet b);
	bool operator || (const Currency b);
	Wallet& operator ++ ();
	Wallet& operator ++ (int);
	Wallet& operator -- ();
	Wallet& operator -- (int);
};
