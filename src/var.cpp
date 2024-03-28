#include "../headers/output.h"
#include "../headers/var.h"

RPGSH_VAR::operator std::string() const
{
	return Value;
}
RPGSH_VAR::operator int() const
{
	return std::stoi(Value);
}
RPGSH_VAR& RPGSH_VAR::operator = (const RPGSH_VAR b)
{
	Value = b.Value;
	return *this;
}
RPGSH_VAR& RPGSH_VAR::operator = (const std::string b)
{
	Value = b;
	return *this;
}
RPGSH_VAR& RPGSH_VAR::operator = (const int b)
{
	Value = std::to_string(b);
	return *this;
}
RPGSH_VAR& RPGSH_VAR::operator += (const RPGSH_VAR b)
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
RPGSH_VAR& RPGSH_VAR::operator += (const std::string b)
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
RPGSH_VAR& RPGSH_VAR::operator += (const int b)
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
RPGSH_VAR& RPGSH_VAR::operator -= (const RPGSH_VAR b)
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
RPGSH_VAR& RPGSH_VAR::operator -= (const int b)
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
RPGSH_VAR& RPGSH_VAR::operator *= (const RPGSH_VAR b)
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
RPGSH_VAR& RPGSH_VAR::operator *= (const int b)
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
RPGSH_VAR& RPGSH_VAR::operator /= (const RPGSH_VAR b)
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
RPGSH_VAR& RPGSH_VAR::operator /= (const int b)
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
RPGSH_VAR& RPGSH_VAR::operator ++ (const int b)
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
RPGSH_VAR& RPGSH_VAR::operator -- (const int b)
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
RPGSH_VAR RPGSH_VAR::operator + (const RPGSH_VAR b)
{
	RPGSH_VAR lhs = *this;
	return (lhs += b);
}
RPGSH_VAR RPGSH_VAR::operator + (const std::string b)
{
	RPGSH_VAR lhs = *this;
	return (lhs += b);
}
RPGSH_VAR RPGSH_VAR::operator + (const int b)
{
	RPGSH_VAR lhs = *this;
	return (lhs += b);
}
RPGSH_VAR RPGSH_VAR::operator - (const RPGSH_VAR b)
{
	RPGSH_VAR lhs = *this;
	return (lhs -= b);
}
RPGSH_VAR RPGSH_VAR::operator - (const int b)
{
	RPGSH_VAR lhs = *this;
	return (lhs -= b);
}
RPGSH_VAR RPGSH_VAR::operator * (const RPGSH_VAR b)
{
	RPGSH_VAR lhs = *this;
	return (lhs *= b);
}
RPGSH_VAR RPGSH_VAR::operator * (const int b)
{
	RPGSH_VAR lhs = *this;
	return (lhs *= b);
}
RPGSH_VAR RPGSH_VAR::operator / (const RPGSH_VAR b)
{
	RPGSH_VAR lhs = *this;
	return (lhs /= b);
}
RPGSH_VAR RPGSH_VAR::operator / (const int b)
{
	RPGSH_VAR lhs = *this;
	return (lhs /= b);
}

RPGSH_VAR::RPGSH_VAR(){}
RPGSH_VAR::RPGSH_VAR(std::string _value)
{
	Value = _value;
}
RPGSH_VAR::RPGSH_VAR(int _value)
{
	Value = std::to_string(_value);
}

const char* RPGSH_VAR::c_str() const
{
	return Value.c_str();
}

// Comparison operators
bool RPGSH_VAR::operator != (const RPGSH_VAR b)
{
	return (Value != b.Value);
}
