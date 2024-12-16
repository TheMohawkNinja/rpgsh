#include <stdexcept>
#include "../headers/define.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/var.h"

Var::Var(){}
Var::Var(std::string str)
{
	std::string v(1,VAR_SIGIL);

	//If second character isn't a '{', this isn't an explicit constructor, so just treat it as a normal value
	if(str[1] != '{')
	{
		Value = str;
		return;
	}

	//Make sure first character is VAR_SIGIL
	if(str[0] != VAR_SIGIL || str.find("}") == std::string::npos)
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

	Value = str.substr(2,str.find("}")-2);
}
Var::Var(int _value)
{
	Value = std::to_string(_value);
}
Var::operator std::string() const
{
	return std::string(1,VAR_SIGIL)+"{"+Value+"}";
}
Var::operator int() const
{
	return std::stoi(Value);
}
Var::operator bool() const
{
	if(isInt() && int(*this) != 0)
		return true;
	else if(isInt())
		return false;
	else if(!isInt() && (Value != "" && stringcasecmp(Value,"false")))
		return true;
	else
		return false;
}
const char* Var::c_str() const
{
	return std::string(*this).c_str();
}
bool Var::isInt() const
{
	// std::stoi will return "true" so long as the first character is an integer
	// Therefore, we have to manually check each character to see if the whole string is ACTUALLY an integer -_-
	for(const auto& c : Value)
		if(!isdigit(c)) return false;

	// This should only throw if int(*this) > INT_MAX
	try{int(*this); return true;}
	catch(...){return false;}
}

Var::Var(const Var& b)
{
	Value = b.Value;
}
Var& Var::operator = (const int b)
{
	Value = std::to_string(b);
	return *this;
}
Var& Var::operator = (const std::string b)
{
	Value = mergeQuotes(b);
	return *this;
}
Var& Var::operator = (const Var b)
{
	*this = b.Value;
	return *this;
}
Var& Var::operator = ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator = ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator = ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator = ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator += (const int b)
{
	if(isInt())
		Value = std::to_string(int(*this) + b);
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Var& Var::operator += (const std::string b)
{
	if(isInt())
		throw std::runtime_error(E_INVALID_OPERATION);
	else
		Value = mergeQuotes(Value + b);

	return *this;
}
Var& Var::operator += (const Var b)
{
	if(b.isInt())
		*this += int(b);
	else
		*this += b.Value;

	return *this;
}
Var& Var::operator += ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator += ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator += ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator += ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator -= (const int b)
{
	if(isInt())
		Value = std::to_string((int(*this) - b));
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Var& Var::operator -= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator -= (const Var b)
{
	if(b.isInt())
		*this -= int(b);
	else
		*this -= b.Value;

	return *this;
}
Var& Var::operator -= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator -= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator -= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator -= ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator *= (const int b)
{
	if(isInt())
		Value = std::to_string(int(*this) * b);
	else if(b < 0)
		throw std::runtime_error(E_INVALID_OPERATION);
	else if(b == 0)
		Value = "";
	else
		for(int i=1; i<b; i++){Value += Value;}

	return *this;
}
Var& Var::operator *= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator *= (const Var b)
{
	if(b.isInt())
		*this *= int(b);
	else
		*this *= b.Value;

	return *this;
}
Var& Var::operator *= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator *= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator *= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator *= ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator /= (const int b)
{
	if(isInt())
		Value = std::to_string(int(*this) / b);
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Var& Var::operator /= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator /= (const Var b)
{
	if(b.isInt())
		*this /= int(b);
	else
		*this /= b.Value;

	return *this;
}
Var& Var::operator /= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator /= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator /= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator /= ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator ^= (const int b)
{
	if(isInt())
	{
		int total = int(*this);
		for(int i = 1; i < b; i++)
			total *= int(*this);
		Value = std::to_string(total);
	}
	else
	{
		throw std::runtime_error(E_INVALID_OPERATION);
	}

	return *this;
}
Var& Var::operator ^= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator ^= (const Var b)
{
	if(b.isInt())
		*this ^= int(b);
	else
		*this ^= b.Value;

	return *this;
}
Var& Var::operator ^= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator ^= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator ^= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator ^= ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator %= (const int b)
{
	if(isInt())
		Value = std::to_string(int(*this) % b);
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Var& Var::operator %= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator %= (const Var b)
{
	if(b.isInt())
		*this %= int(b);
	else
		*this %= b.Value;

	return *this;
}
Var& Var::operator %= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator %= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator %= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var& Var::operator %= ([[maybe_unused]] const CurrencySystem b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Var Var::operator + (const int b)
{
	return *this += b;
}
Var Var::operator + (const std::string b)
{
	return *this += b;
}
Var Var::operator + (const Var b)
{
	return *this += b;
}
Var Var::operator + (const Dice b)
{
	return *this += b;
}
Var Var::operator + (const Wallet b)
{
	return *this += b;
}
Var Var::operator + (const Currency b)
{
	return *this += b;
}
Var Var::operator + (const CurrencySystem b)
{
	return *this += b;
}
Var Var::operator - (const int b)
{
	return *this -= b;
}
Var Var::operator - (const std::string b)
{
	return *this -= b;
}
Var Var::operator - (const Var b)
{
	return *this -= b;
}
Var Var::operator - (const Dice b)
{
	return *this -= b;
}
Var Var::operator - (const Wallet b)
{
	return *this -= b;
}
Var Var::operator - (const Currency b)
{
	return *this -= b;
}
Var Var::operator - (const CurrencySystem b)
{
	return *this -= b;
}
Var Var::operator * (const int b)
{
	return *this *= b;
}
Var Var::operator * (const std::string b)
{
	return *this *= b;
}
Var Var::operator * (const Var b)
{
	return *this *= b;
}
Var Var::operator * (const Dice b)
{
	return *this *= b;
}
Var Var::operator * (const Wallet b)
{
	return *this *= b;
}
Var Var::operator * (const Currency b)
{
	return *this *= b;
}
Var Var::operator * (const CurrencySystem b)
{
	return *this *= b;
}
Var Var::operator / (const int b)
{
	return *this /= b;
}
Var Var::operator / (const std::string b)
{
	return *this /= b;
}
Var Var::operator / (const Var b)
{
	return *this /= b;
}
Var Var::operator / (const Dice b)
{
	return *this /= b;
}
Var Var::operator / (const Wallet b)
{
	return *this /= b;
}
Var Var::operator / (const Currency b)
{
	return *this /= b;
}
Var Var::operator / (const CurrencySystem b)
{
	return *this /= b;
}
Var Var::operator ^ (const int b)
{
	return *this ^= b;
}
Var Var::operator ^ (const std::string b)
{
	return *this ^= b;
}
Var Var::operator ^ (const Var b)
{
	return *this ^= b;
}
Var Var::operator ^ (const Dice b)
{
	return *this ^= b;
}
Var Var::operator ^ (const Wallet b)
{
	return *this ^= b;
}
Var Var::operator ^ (const Currency b)
{
	return *this ^= b;
}
Var Var::operator ^ (const CurrencySystem b)
{
	return *this ^= b;
}
Var Var::operator % (const int b)
{
	return *this %= b;
}
Var Var::operator % (const std::string b)
{
	return *this %= b;
}
Var Var::operator % (const Var b)
{
	return *this %= b;
}
Var Var::operator % (const Dice b)
{
	return *this %= b;
}
Var Var::operator % (const Wallet b)
{
	return *this %= b;
}
Var Var::operator % (const Currency b)
{
	return *this %= b;
}
Var Var::operator % (const CurrencySystem b)
{
	return *this %= b;
}
bool Var::operator == (const int b)
{
	if(isInt())
		return int(*this) == b;
	else
		return false;
}
bool Var::operator == (const std::string b)
{
	return (Value == b);
}
bool Var::operator == (const Var b)
{
	if(b.isInt())
		return *this == int(b);
	else
		return *this == b.Value;
}
bool Var::operator == (const Dice b)
{
	if(isInt())
		return Dice(Value) == b;
	else
		return false;
}
bool Var::operator == ([[maybe_unused]] const Wallet b)
{
	return false;
}
bool Var::operator == ([[maybe_unused]] const Currency b)
{
	return false;
}
bool Var::operator == ([[maybe_unused]] const CurrencySystem b)
{
	return false;
}
bool Var::operator < (const int b)
{
if(isInt())
		return int(*this) < b;
	else
		return false;
}
bool Var::operator < (const std::string b)
{
	return (Value < b);
}
bool Var::operator < (const Var b)
{
	if(b.isInt())
		return *this < int(b);
	else
		return *this < b.Value;
}
bool Var::operator < (const Dice b)
{
	if(isInt())
		return Dice(Value) < b;
	else
		return false;
}
bool Var::operator < ([[maybe_unused]] const Wallet b)
{
	return false;
}
bool Var::operator < ([[maybe_unused]] const Currency b)
{
	return false;
}
bool Var::operator < ([[maybe_unused]] const CurrencySystem b)
{
	return false;
}
bool Var::operator > (const int b)
{
	if(isInt())
		return int(*this) > b;
	else
		return false;
}
bool Var::operator > (const std::string b)
{
	return (Value > b);
}
bool Var::operator > (const Var b)
{
	if(b.isInt())
		return *this > int(b);
	else
		return *this > b.Value;
}
bool Var::operator > (const Dice b)
{
	if(isInt())
		return Dice(Value) > b;
	else
		return false;
}
bool Var::operator > ([[maybe_unused]] const Wallet b)
{
	return false;
}
bool Var::operator > ([[maybe_unused]] const Currency b)
{
	return false;
}
bool Var::operator > ([[maybe_unused]] const CurrencySystem b)
{
	return false;
}
bool Var::operator <= (const int b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator <= (const std::string b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator <= (const Var b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator <= (const Dice b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator <= (const Wallet b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator <= (const Currency b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator <= (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool Var::operator >= (const int b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator >= (const std::string b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator >= (const Var b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator >= (const Dice b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator >= (const Wallet b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator >= (const Currency b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator >= (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator != (const int b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator != (const std::string b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator != (const Var b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator != (const Dice b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator != (const Wallet b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator != (const Currency b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator != (const CurrencySystem b)
{
	Var lhs = *this;
	return !(lhs == b);
}
bool Var::operator && (const int b)
{
	return bool(*this) && (b != 0);
}
bool Var::operator && (const std::string b)
{
	return bool(*this) && (b != "" && stringcasecmp(b,"false"));
}
bool Var::operator && (const Var b)
{
	return bool(*this) && bool(b);
}
bool Var::operator && (const Dice b)
{
	return bool(*this) && bool(b);
}
bool Var::operator && (const Wallet b)
{
	return bool(*this) && bool(b);
}
bool Var::operator && (const Currency b)
{
	return bool(*this) && bool(b);
}
bool Var::operator && (const CurrencySystem b)
{
	return bool(*this) && bool(b);
}
bool Var::operator || (const int b)
{
	return bool(*this) || (b != 0);
}
bool Var::operator || (const std::string b)
{
	return bool(*this) || (b != "" && stringcasecmp(b,"false"));
}
bool Var::operator || (const Var b)
{
	return bool(*this) || bool(b);
}
bool Var::operator || (const Dice b)
{
	return bool(*this) || bool(b);
}
bool Var::operator || (const Wallet b)
{
	return bool(*this) || bool(b);
}
bool Var::operator || (const Currency b)
{
	return bool(*this) || bool(b);
}
bool Var::operator || (const CurrencySystem b)
{
	return bool(*this) || bool(b);
}
Var& Var::operator ++ (int)
{
	if(isInt())
		Value = std::to_string(int(*this) + 1);
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Var& Var::operator -- (int)
{
	if(isInt())
		Value = std::to_string(int(*this) - 1);
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
