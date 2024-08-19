#include <stdexcept>
#include "../headers/define.h"
#include "../headers/output.h"
#include "../headers/var.h"

Var::Var(const Var& b)
{
	Value = b.Value;
}
Var::operator std::string() const
{
	return std::string(1,VAR_SIGIL)+"{"+Value+"}";
}
const char* Var::c_str() const
{
	return std::string(*this).c_str();
}
Var::operator int() const
{
	return std::stoi(Value);
}
Var& Var::operator = (const Var b)
{
	Value = b.Value;
	return *this;
}
Var& Var::operator = (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator = (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator = (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator = (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator = (const std::string b)
{
	Value = b;
	return *this;
}
Var& Var::operator = (const int b)
{
	Value = std::to_string(b);
	return *this;
}
Var& Var::operator += (const Var b)
{
	bool a_is_num = true;
	bool b_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}
	try{std::stoi(b.Value);}
	catch(...){b_is_num = false;}

	if(a_is_num && b_is_num)
		Value = std::to_string(std::stoi(Value) + std::stoi(b.Value));
	else if(!a_is_num && !b_is_num)
		Value = Value + b.Value;
	else
		throw std::runtime_error("invalid_operation");

	return *this;
}
Var& Var::operator += (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator += (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator += (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator += (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator += (const std::string b)
{
	try{std::stoi(Value); throw std::runtime_error("invalid_operation");}
	catch(...){Value = Value + b;}

	return *this;
}
Var& Var::operator += (const int b)
{
	try{std::stoi(Value); Value = std::to_string(std::stoi(Value) + b);}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}
Var& Var::operator -= (const Var b)
{
	try
	{
		std::stoi(Value);
		std::stoi(b.Value);
		Value = std::to_string(std::stoi(Value) - std::stoi(b.Value));
	}
	catch(...)
	{
		throw std::runtime_error("invalid_operation");
	}

	return *this;
}
Var& Var::operator -= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator -= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator -= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator -= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator -= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator -= (const int b)
{
	try{std::stoi(Value); Value = std::to_string((std::stoi(Value) - b));}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}
Var& Var::operator *= (const Var b)
{
	bool a_is_num = true;
	bool b_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}
	try{std::stoi(b.Value);}
	catch(...){b_is_num = false;}

	if(a_is_num && b_is_num)
		Value = std::to_string(std::stoi(Value) * std::stoi(b.Value));
	else if(!a_is_num && b_is_num && std::stoi(b.Value) == 0)
		Value = "";
	else if(!a_is_num && b_is_num && std::stoi(b.Value) > 0)
		for(int i=1; i<std::stoi(b.Value); i++){Value += Value;}
	else
		throw std::runtime_error("invalid_operation");

	return *this;
}
Var& Var::operator *= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator *= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator *= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator *= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator *= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator *= (const int b)
{
	try
	{
		std::stoi(Value);
		Value = std::to_string(std::stoi(Value) * b);
	}
	catch(...)
	{
		if(b < 0)
			throw std::runtime_error("invalid_operation");
		else if(b == 0)
			Value = "";
		else
			for(int i=1; i<b; i++){Value += Value;}
	}

	return *this;
}
Var& Var::operator /= (const Var b)
{
	try
	{
		std::stoi(Value);
		std::stoi(b.Value);
		Value = std::to_string(std::stoi(Value) / std::stoi(b.Value));
	}
	catch(...)
	{
		throw std::runtime_error("invalid_operation");
	}

	return *this;
}
Var& Var::operator /= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator /= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator /= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator /= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator /= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator /= (const int b)
{
	try{std::stoi(Value); Value = std::to_string(std::stoi(Value) / b);}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}
Var& Var::operator ^= (const Var b)
{
	try
	{
		std::stoi(Value);
		std::stoi(b.Value);
	}
	catch(...)
	{
		throw std::runtime_error("invalid_operation");
	}

	Value = std::to_string(std::stoi(Value) ^ std::stoi(b.Value));

	return *this;
}
Var& Var::operator ^= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator ^= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator ^= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator ^= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator ^= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator ^= (const int b)
{
	try{std::stoi(Value); Value ^= b;}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}
Var& Var::operator %= (const Var b)
{
	try
	{
		std::stoi(Value);
		std::stoi(b.Value);
		Value = std::to_string(std::stoi(Value) % std::stoi(b.Value));
	}
	catch(...)
	{
		throw std::runtime_error("invalid_operation");
	}

	return *this;
}
Var& Var::operator %= (const Dice b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator %= (const Wallet b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator %= (const Currency b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator %= (const CurrencySystem b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator %= (const std::string b)
{
	(void)decltype(b)();
	throw std::runtime_error("invalid_operation");
}
Var& Var::operator %= (const int b)
{
	try{std::stoi(Value); Value = std::to_string(std::stoi(Value) % b);}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}
Var Var::operator + (const Var b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator + (const Dice b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator + (const Wallet b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator + (const Currency b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator + (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator + (const std::string b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator + (const int b)
{
	Var lhs = *this;
	return (lhs += b);
}
Var Var::operator - (const Var b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator - (const Dice b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator - (const Wallet b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator - (const Currency b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator - (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator - (const std::string b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator - (const int b)
{
	Var lhs = *this;
	return (lhs -= b);
}
Var Var::operator * (const Var b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator * (const Dice b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator * (const Wallet b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator * (const Currency b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator * (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator * (const std::string b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator * (const int b)
{
	Var lhs = *this;
	return (lhs *= b);
}
Var Var::operator / (const Var b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator / (const Dice b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator / (const Wallet b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator / (const Currency b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator / (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator / (const std::string b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator / (const int b)
{
	Var lhs = *this;
	return (lhs /= b);
}
Var Var::operator ^ (const Var b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator ^ (const Dice b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator ^ (const Wallet b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator ^ (const Currency b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator ^ (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator ^ (const std::string b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator ^ (const int b)
{
	Var lhs = *this;
	return (lhs ^= b);
}
Var Var::operator % (const Var b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var Var::operator % (const Dice b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var Var::operator % (const Wallet b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var Var::operator % (const Currency b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var Var::operator % (const CurrencySystem b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var Var::operator % (const std::string b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var Var::operator % (const int b)
{
	Var lhs = *this;
	return (lhs %= b);
}
Var& Var::operator ++ (int)
{
	try{std::stoi(Value); Value = std::to_string(std::stoi(Value) + 1);}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}
Var& Var::operator -- (int)
{
	try{std::stoi(Value); Value = std::to_string(std::stoi(Value) - 1);}
	catch(...){throw std::runtime_error("invalid_operation");}

	return *this;
}

// Comparison operators TODO: Continue work. Make Var RHS below int and string so we can reference them
bool Var::operator == (const Var b)
{
	return (Value == b.Value);
}
bool operator < (const Var b)
{
	return (Value < b.Value);
}
bool operator > (const Var b)
{
	return (Value > b.Value);
}
bool operator <= (const Var b)
{
	Var lhs = *this;
	return (lhs < b || lhs == b);
}
bool operator >= (const Var b)
{
	Var lhs = *this;
	return (lhs > b || lhs == b);
}
bool Var::operator != (const Var b)
{
	Var lhs = *this;
	return !(lhs == b);
}

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

	//Make sure first character is 'c'
	if(str[0] != VAR_SIGIL)
	{
		output(Error,"Incorrect data type specifier sigil for var explicit constructor.");
		exit(-1);
	}

	//Check for end of explicit constructor definition
	if(str.find("}") == std::string::npos)
	{
		output(Error,"Missing terminating \'}\' for var explicit constructor.");
		exit(-1);
	}

	if(str.substr(0,2) == (v+"{") && str.find("}") != std::string::npos)// Explicit constructor
	{
		Value = str.substr(2,str.find("}")-2);
	}
}
Var::Var(int _value)
{
	Value = std::to_string(_value);
}
