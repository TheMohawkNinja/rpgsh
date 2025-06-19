#pragma once

#include <cassert>
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
datamap<Currency> getCurrencySystem(std::wstring system);
void MakeChange(Currency c, Wallet* w);
void TradeUp(std::wstring system, Wallet* w);

class Currency
{
	public:
		std::wstring System = L"";
		std::wstring Name = L"";
		int SmallerAmount = 0;
		std::wstring Smaller = L"";
		std::wstring Larger = L"";

	private:

	//Parsing methods for Currency(std::wstring)
	void tryParseCurrencySystem(std::wstring* str);
	void tryParseName(std::wstring* str);
	void tryParseSmallerAmount(std::wstring* str);
	void tryParseSmaller(std::wstring* str);
	void tryParseLarger(std::wstring* str);

	public:
	Currency();
	Currency(const Currency& b);
	Currency(std::wstring str);
	Currency(std::wstring _Name, int _SmallerAmount, std::wstring _Smaller, std::wstring _Larger);
	Currency(std::wstring _System, std::wstring _Name, int _SmallerAmount, std::wstring _Smaller, std::wstring _Larger);

	explicit operator std::wstring() const;
	explicit operator bool() const;
	const wchar_t* c_str() const;
	Currency& operator = ([[maybe_unused]] const int b);
	Currency& operator = ([[maybe_unused]] const std::wstring b);
	Currency& operator = (const Var b);
	Currency& operator = ([[maybe_unused]] const Dice b);
	Currency& operator = ([[maybe_unused]] const Wallet b);
	Currency& operator = (const Currency b);
	Currency& operator += (const int b);
	Currency& operator += ([[maybe_unused]] const std::wstring b);
	Currency& operator += (const Var b);
	Currency& operator += ([[maybe_unused]] const Dice b);
	Wallet operator += (const Wallet b);
	Wallet operator += (const Currency b);
	Currency& operator -= (const int b);
	Currency& operator -= ([[maybe_unused]] const std::wstring b);
	Currency& operator -= (const Var b);
	Currency& operator -= ([[maybe_unused]] const Dice b);
	Currency& operator -= ([[maybe_unused]] const Wallet b);
	Currency& operator -= ([[maybe_unused]] const Currency b);
	Wallet operator *= (const int b);
	Wallet operator *= ([[maybe_unused]] const std::wstring b);
	Wallet operator *= (const Var b);
	Currency& operator *= ([[maybe_unused]] const Dice b);
	Currency& operator *= ([[maybe_unused]] const Wallet b);
	Currency& operator *= ([[maybe_unused]] const Currency b);
	Currency& operator /= ([[maybe_unused]] const int b);
	Currency& operator /= ([[maybe_unused]] const std::wstring b);
	Currency& operator /= (const Var b);
	Currency& operator /= ([[maybe_unused]] const Dice b);
	Currency& operator /= ([[maybe_unused]] const Wallet b);
	Currency& operator /= ([[maybe_unused]] const Currency b);
	Currency& operator ^= ([[maybe_unused]] const int b);
	Currency& operator ^= ([[maybe_unused]] const std::wstring b);
	Currency& operator ^= (const Var b);
	Currency& operator ^= ([[maybe_unused]] const Dice b);
	Currency& operator ^= ([[maybe_unused]] const Wallet b);
	Currency& operator ^= ([[maybe_unused]] const Currency b);
	Currency& operator %= ([[maybe_unused]] const int b);
	Currency& operator %= ([[maybe_unused]] const std::wstring b);
	Currency& operator %= (const Var b);
	Currency& operator %= ([[maybe_unused]] const Dice b);
	Currency& operator %= ([[maybe_unused]] const Wallet b);
	Currency& operator %= ([[maybe_unused]] const Currency b);
	Currency operator + (const int b);
	Currency operator + (const std::wstring b);
	Currency operator + (const Var b);
	Currency operator + (const Dice b);
	Wallet operator + (const Wallet b);
	Wallet operator + (const Currency b);
	Currency operator - (const int b);
	Currency operator - (const std::wstring b);
	Currency operator - (const Var b);
	Currency operator - (const Dice b);
	Currency operator - (const Wallet b);
	Currency operator - (const Currency b);
	Wallet operator * (const int b);
	Wallet operator * (const std::wstring b);
	Wallet operator * (const Var b);
	Currency operator * (const Dice b);
	Currency operator * (const Wallet b);
	Currency operator * (const Currency b);
	Currency operator / (const int b);
	Currency operator / (const std::wstring b);
	Currency operator / (const Var b);
	Currency operator / (const Dice b);
	Currency operator / (const Wallet b);
	Currency operator / (const Currency b);
	Currency operator ^ (const int b);
	Currency operator ^ (const std::wstring b);
	Currency operator ^ (const Var b);
	Currency operator ^ (const Dice b);
	Currency operator ^ (const Wallet b);
	Currency operator ^ (const Currency b);
	Currency operator % (const int b);
	Currency operator % (const std::wstring b);
	Currency operator % (const Var b);
	Currency operator % (const Dice b);
	Currency operator % (const Wallet b);
	Currency operator % (const Currency b);
	bool operator == ([[maybe_unused]] const int& b) const;
	bool operator == ([[maybe_unused]] const std::wstring& b) const;
	bool operator == (const Var& b) const;
	bool operator == ([[maybe_unused]] const Dice& b) const;
	bool operator == ([[maybe_unused]] const Wallet& b) const;
	bool operator == (const Currency& b) const;
	bool operator < ([[maybe_unused]] const int& b) const;
	bool operator < ([[maybe_unused]] const std::wstring& b) const;
	bool operator < (const Var& b) const;
	bool operator < ([[maybe_unused]] const Dice& b) const;
	bool operator < ([[maybe_unused]] const Wallet& b) const;
	bool operator < (const Currency& b) const;
	bool operator > ([[maybe_unused]] const int& b) const;
	bool operator > ([[maybe_unused]] const std::wstring& b) const;
	bool operator > (const Var& b) const;
	bool operator > ([[maybe_unused]] const Dice& b) const;
	bool operator > ([[maybe_unused]] const Wallet& b) const;
	bool operator > (const Currency& b) const;
	bool operator <= (const int& b) const;
	bool operator <= (const std::wstring& b) const;
	bool operator <= (const Var& b) const;
	bool operator <= (const Dice& b) const;
	bool operator <= (const Wallet& b) const;
	bool operator <= (const Currency& b) const;
	bool operator >= (const int& b) const;
	bool operator >= (const std::wstring& b) const;
	bool operator >= (const Var& b) const;
	bool operator >= (const Dice& b) const;
	bool operator >= (const Wallet& b) const;
	bool operator >= (const Currency& b) const;
	bool operator != (const int& b) const;
	bool operator != (const std::wstring& b) const;
	bool operator != (const Var& b) const;
	bool operator != (const Dice& b) const;
	bool operator != (const Wallet& b) const;
	bool operator != (const Currency& b) const;
	bool operator && (const int b);
	bool operator && (const std::wstring b);
	bool operator && (const Var b);
	bool operator && (const Dice b);
	bool operator && (const Wallet b);
	bool operator && (const Currency b);
	bool operator || (const int b);
	bool operator || (const std::wstring b);
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
	unsigned int getEquivalentValueInLowestDenomination(std::wstring system);
	void print();
	void FloatQuantityToIntCurrency(Currency c, float q);
	bool containsCurrency(std::wstring currency_str);
	Currency getExistingCurrency(std::wstring currency_str);

	explicit operator std::wstring() const;
	explicit operator bool() const;
	const wchar_t* c_str() const;

	void sort();

	int get(const Currency c);
	void set(const Currency c, const int q);

	Wallet();
	Wallet(const Wallet& b);
	Wallet(const Money m);
	Wallet(std::wstring s);
	Wallet& operator = ([[maybe_unused]] const int b);
	Wallet& operator = ([[maybe_unused]] const std::wstring b);
	Wallet& operator = (const Money b);
	Wallet& operator = (const Var b);
	Wallet& operator = ([[maybe_unused]] const Dice b);
	Wallet& operator = (const Wallet b);
	Wallet& operator = (const Currency b);
	Wallet& operator += ([[maybe_unused]] const int b);
	Wallet& operator += ([[maybe_unused]] const std::wstring b);
	Wallet& operator += (const Money b);
	Wallet& operator += (const Var b);
	Wallet& operator += ([[maybe_unused]] const Dice b);
	Wallet& operator += (const Wallet b);
	Wallet& operator += (const Currency b);
	Wallet& operator -= ([[maybe_unused]] const int b);
	Wallet& operator -= ([[maybe_unused]] const std::wstring b);
	Wallet& operator -= (const Money b);
	Wallet& operator -= (const Var b);
	Wallet& operator -= ([[maybe_unused]] const Dice b);
	Wallet& operator -= (const Wallet b);
	Wallet& operator -= (const Currency b);
	Wallet& operator *= (const int b);
	Wallet& operator *= ([[maybe_unused]] const std::wstring b);
	Wallet& operator *= ([[maybe_unused]] const Money b);
	Wallet& operator *= (const Var b);
	Wallet& operator *= ([[maybe_unused]] const Dice b);
	Wallet& operator *= ([[maybe_unused]] const Wallet b);
	Wallet& operator *= ([[maybe_unused]] const Currency b);
	Wallet& operator /= (const int b);
	Wallet& operator /= ([[maybe_unused]] const std::wstring b);
	Wallet& operator /= ([[maybe_unused]] const Money b);
	Wallet& operator /= (const Var b);
	Wallet& operator /= ([[maybe_unused]] const Dice b);
	Wallet& operator /= ([[maybe_unused]] const Wallet b);
	Wallet& operator /= ([[maybe_unused]] const Currency b);
	Wallet& operator ^= ([[maybe_unused]] const int b);
	Wallet& operator ^= ([[maybe_unused]] const std::wstring b);
	Wallet& operator ^= (const Var b);
	Wallet& operator ^= ([[maybe_unused]] const Dice b);
	Wallet& operator ^= ([[maybe_unused]] const Wallet b);
	Wallet& operator ^= ([[maybe_unused]] const Currency b);
	Wallet& operator %= (const int b);
	Wallet& operator %= ([[maybe_unused]] const std::wstring b);
	Wallet& operator %= (const Var b);
	Wallet& operator %= ([[maybe_unused]] const Dice b);
	Wallet& operator %= ([[maybe_unused]] const Wallet b);
	Wallet& operator %= ([[maybe_unused]] const Currency b);
	Wallet operator + (const int b);
	Wallet operator + (const std::wstring b);
	Wallet operator + (const Var b);
	Wallet operator + (const Dice b);
	Wallet operator + (const Wallet b);
	Wallet operator + (const Currency b);
	Wallet operator - (const int b);
	Wallet operator - (const std::wstring b);
	Wallet operator - (const Var b);
	Wallet operator - (const Dice b);
	Wallet operator - (const Wallet b);
	Wallet operator - (const Currency b);
	Wallet operator * (const int b);
	Wallet operator * (const std::wstring b);
	Wallet operator * (const Var b);
	Wallet operator * (const Dice b);
	Wallet operator * (const Wallet b);
	Wallet operator * (const Currency b);
	Wallet operator / (const int b);
	Wallet operator / (const std::wstring b);
	Wallet operator / (const Var b);
	Wallet operator / (const Dice b);
	Wallet operator / (const Wallet b);
	Wallet operator / (const Currency b);
	Wallet operator ^ (const int b);
	Wallet operator ^ (const std::wstring b);
	Wallet operator ^ (const Var b);
	Wallet operator ^ (const Dice b);
	Wallet operator ^ (const Wallet b);
	Wallet operator ^ (const Currency b);
	Wallet operator % (const int b);
	Wallet operator % (const std::wstring b);
	Wallet operator % (const Var b);
	Wallet operator % (const Dice b);
	Wallet operator % (const Wallet b);
	Wallet operator % (const Currency b);
	bool operator == ([[maybe_unused]] const int& b) const;
	bool operator == ([[maybe_unused]] const std::wstring& b) const;
	bool operator == (const Var& b) const;
	bool operator == ([[maybe_unused]] const Dice& b) const;
	bool operator == (const Wallet& b) const;
	bool operator == (const Currency& b) const;
	bool operator < ([[maybe_unused]] const int& b) const;
	bool operator < ([[maybe_unused]] const std::wstring& b) const;
	bool operator < (const Var& b) const;
	bool operator < ([[maybe_unused]] const Dice& b) const;
	bool operator < (const Wallet& b) const;
	bool operator < (const Currency& b) const;
	bool operator > ([[maybe_unused]] const int& b) const;
	bool operator > ([[maybe_unused]] const std::wstring& b) const;
	bool operator > (const Var& b) const;
	bool operator > ([[maybe_unused]] const Dice& b) const;
	bool operator > (const Wallet& b) const;
	bool operator > (const Currency& b) const;
	bool operator <= (const int& b) const;
	bool operator <= (const std::wstring& b) const;
	bool operator <= (const Var& b) const;
	bool operator <= (const Dice& b) const;
	bool operator <= (const Wallet& b) const;
	bool operator <= (const Currency& b) const;
	bool operator >= (const int& b) const;
	bool operator >= (const std::wstring& b) const;
	bool operator >= (const Var& b) const;
	bool operator >= (const Dice& b) const;
	bool operator >= (const Wallet& b) const;
	bool operator >= (const Currency& b) const;
	bool operator != (const int& b) const;
	bool operator != (const std::wstring& b) const;
	bool operator != (const Var& b) const;
	bool operator != (const Dice& b) const;
	bool operator != (const Wallet& b) const;
	bool operator != (const Currency& b) const;
	bool operator && (const int b);
	bool operator && (const std::wstring b);
	bool operator && (const Var b);
	bool operator && (const Dice b);
	bool operator && (const Wallet b);
	bool operator && (const Currency b);
	bool operator || (const int b);
	bool operator || (const std::wstring b);
	bool operator || (const Var b);
	bool operator || (const Dice b);
	bool operator || (const Wallet b);
	bool operator || (const Currency b);
	Wallet& operator ++ ();
	Wallet& operator ++ (int);
	Wallet& operator -- ();
	Wallet& operator -- (int);
};
