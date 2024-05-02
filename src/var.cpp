#include "../headers/define.h"
#include "../headers/output.h"
#include "../headers/var.h"

Var::operator std::string() const
{
	return Value;
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
Var& Var::operator += (const std::string b)
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
Var& Var::operator += (const int b)
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
Var& Var::operator -= (const Var b)
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
Var& Var::operator -= (const int b)
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
Var& Var::operator *= (const Var b)
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
Var& Var::operator *= (const int b)
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
Var& Var::operator /= (const Var b)
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
Var& Var::operator /= (const int b)
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
Var& Var::operator ++ (const int b)
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
Var& Var::operator -- (const int b)
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
Var Var::operator + (const Var b)
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
Var Var::operator / (const int b)
{
	Var lhs = *this;
	return (lhs /= b);
}

Var::Var(){}
Var::Var(std::string _value)
{
	std::string v(1,VAR_SIGIL);
	if(_value.substr(0,2) == (v+"{") && _value.find("}") != std::string::npos)// Explicit constructor
	{
		Value = _value.substr(2,_value.find("}")-2);
	}
	else if(_value.substr(0,2) == (v+"{"))
	{
		output(Error,"Var explicit constructor missing terminating \'}\'.");
		exit(-1);
	}
	else
	{
		Value = _value;
	}
}
Var::Var(int _value)
{
	Value = std::to_string(_value);
}

const char* Var::c_str() const
{
	return Value.c_str();
}

// Comparison operators
bool Var::operator != (const Var b)
{
	return (Value != b.Value);
}
