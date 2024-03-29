#include "../headers/output.h"
#include "../headers/var.h"

var_t::operator std::string() const
{
	return Value;
}
var_t::operator int() const
{
	return std::stoi(Value);
}
var_t& var_t::operator = (const var_t b)
{
	Value = b.Value;
	return *this;
}
var_t& var_t::operator = (const std::string b)
{
	Value = b;
	return *this;
}
var_t& var_t::operator = (const int b)
{
	Value = std::to_string(b);
	return *this;
}
var_t& var_t::operator += (const var_t b)
{
	bool a_is_num = true;
	bool b_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}
	try{std::stoi(b.Value);}
	catch(...){b_is_num = false;}

	if(a_is_num && b_is_num)
	{
		Value = std::to_string((std::stoi(Value) + std::stoi(b.Value)));
	}
	else if(!a_is_num && !b_is_num)
	{
		Value = Value + b.Value;
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator += (const std::string b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(!a_is_num)
	{
		Value = Value + b;
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator += (const int b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(a_is_num)
	{
		Value = std::to_string((std::stoi(Value) + b));
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator -= (const var_t b)
{
	bool a_is_num = true;
	bool b_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}
	try{std::stoi(b.Value);}
	catch(...){b_is_num = false;}

	if(a_is_num && b_is_num)
	{
		Value = std::to_string((std::stoi(Value) - std::stoi(b.Value)));
	}
	else if(!a_is_num && !b_is_num)
	{
		output(Error,"Cannot subtract two non-numerical values from each other.");
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator -= (const int b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(a_is_num)
	{
		Value = std::to_string((std::stoi(Value) - b));
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator *= (const var_t b)
{
	bool a_is_num = true;
	bool b_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}
	try{std::stoi(b.Value);}
	catch(...){b_is_num = false;}

	if(a_is_num && b_is_num)
	{
		Value = std::to_string((std::stoi(Value) * std::stoi(b.Value)));
	}
	else if(!a_is_num && !b_is_num)
	{
		output(Error,"Cannot multiply two non-numerical values from each other.");
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator *= (const int b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(a_is_num)
	{
		Value = std::to_string((std::stoi(Value) * b));
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator /= (const var_t b)
{
	bool a_is_num = true;
	bool b_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}
	try{std::stoi(b.Value);}
	catch(...){b_is_num = false;}

	if(a_is_num && b_is_num)
	{
		Value = std::to_string((std::stoi(Value) / std::stoi(b.Value)));
	}
	else if(!a_is_num && !b_is_num)
	{
		output(Error,"Cannot divide two non-numerical values from each other.");
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator /= (const int b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(a_is_num)
	{
		Value = std::to_string((std::stoi(Value) / b));
	}
	else
	{
		output(Error,"Ambiguous operation between numerical and non-numerical values.");
	}
	return *this;
}
var_t& var_t::operator ++ (const int b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(a_is_num)
	{
		Value = std::to_string((std::stoi(Value) + 1));
	}
	else
	{
		output(Error,"Cannot increment non-numerical value.");
	}
	return *this;
}
var_t& var_t::operator -- (const int b)
{
	bool a_is_num = true;

	try{std::stoi(Value);}
	catch(...){a_is_num = false;}

	if(a_is_num)
	{
		Value = std::to_string((std::stoi(Value) - 1));
	}
	else
	{
		output(Error,"Cannot decrement non-numerical value.");
	}
	return *this;
}
var_t var_t::operator + (const var_t b)
{
	var_t lhs = *this;
	return (lhs += b);
}
var_t var_t::operator + (const std::string b)
{
	var_t lhs = *this;
	return (lhs += b);
}
var_t var_t::operator + (const int b)
{
	var_t lhs = *this;
	return (lhs += b);
}
var_t var_t::operator - (const var_t b)
{
	var_t lhs = *this;
	return (lhs -= b);
}
var_t var_t::operator - (const int b)
{
	var_t lhs = *this;
	return (lhs -= b);
}
var_t var_t::operator * (const var_t b)
{
	var_t lhs = *this;
	return (lhs *= b);
}
var_t var_t::operator * (const int b)
{
	var_t lhs = *this;
	return (lhs *= b);
}
var_t var_t::operator / (const var_t b)
{
	var_t lhs = *this;
	return (lhs /= b);
}
var_t var_t::operator / (const int b)
{
	var_t lhs = *this;
	return (lhs /= b);
}

var_t::var_t(){}
var_t::var_t(std::string _value)
{
	Value = _value;
}
var_t::var_t(int _value)
{
	Value = std::to_string(_value);
}

const char* var_t::c_str() const
{
	return Value.c_str();
}

// Comparison operators
bool var_t::operator != (const var_t b)
{
	return (Value != b.Value);
}
