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
Var& Var::operator ++ (int)
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
Var& Var::operator -- (int)
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

const char* Var::c_str() const
{
	return Value.c_str();
}

// Comparison operators
bool Var::operator != (const Var b)
{
	return (Value != b.Value);
}
