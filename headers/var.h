#pragma once

#include "output.h"

class RPGSH_VAR
{
	public:
		std::string Value = "";

		explicit operator std::string() const
		{
			return Value;
		}
		explicit operator int() const
		{
			return std::stoi(Value);
		}
		RPGSH_VAR& operator = (const RPGSH_VAR b)
		{
			Value = b.Value;
			return *this;
		}
		RPGSH_VAR& operator = (const std::string b)
		{
			Value = b;
			return *this;
		}
		RPGSH_VAR& operator = (const int b)
		{
			Value = std::to_string(b);
			return *this;
		}
		RPGSH_VAR& operator += (const RPGSH_VAR b)
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
		RPGSH_VAR& operator += (const std::string b)
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
		RPGSH_VAR& operator += (const int b)
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
		RPGSH_VAR& operator -= (const RPGSH_VAR b)
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
		RPGSH_VAR& operator -= (const int b)
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
		RPGSH_VAR& operator *= (const RPGSH_VAR b)
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
		RPGSH_VAR& operator *= (const int b)
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
		RPGSH_VAR& operator /= (const RPGSH_VAR b)
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
		RPGSH_VAR& operator /= (const int b)
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
		RPGSH_VAR& operator ++ (const int b)
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
		RPGSH_VAR& operator -- (const int b)
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

	RPGSH_VAR(){}
	RPGSH_VAR(std::string _value)
	{
		Value = _value;
	}
	RPGSH_VAR(int _value)
	{
		Value = std::to_string(_value);
	}

	const char* c_str()
	{
		return Value.c_str();
	}
};
RPGSH_VAR operator + (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result += b;
	return result;
}
RPGSH_VAR operator + (const RPGSH_VAR a, const std::string b)
{
	RPGSH_VAR result = a;
	result += b;
	return result;
}
RPGSH_VAR operator + (const std::string a, const RPGSH_VAR b)
{
	RPGSH_VAR result = RPGSH_VAR(a);
	result += b;
	return result;
}
RPGSH_VAR operator + (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result += b;
	return result;
}
RPGSH_VAR operator + (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result += b;
	return result;
}
RPGSH_VAR operator - (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result -= b;
	return result;
}
RPGSH_VAR operator - (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result -= b;
	return result;
}
RPGSH_VAR operator - (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result -= b;
	return result;
}
RPGSH_VAR operator * (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result *= b;
	return result;
}
RPGSH_VAR operator * (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result *= b;
	return result;
}
RPGSH_VAR operator * (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result *= b;
	return result;
}
RPGSH_VAR operator / (const RPGSH_VAR a, const RPGSH_VAR b)
{
	RPGSH_VAR result = a;
	result /= b;
	return result;
}
RPGSH_VAR operator / (const RPGSH_VAR a, const int b)
{
	RPGSH_VAR result = a;
	result /= b;
	return result;
}
RPGSH_VAR operator / (const int a, const RPGSH_VAR b)
{
	RPGSH_VAR result = std::to_string(a);
	result /= b;
	return result;
}

template <typename T>
class RPGSH_OBJ
{
	private:
		std::map<std::string, T> Properties;

	public:

	//Allows an std::map<std::string,RPGSH_OBJ> to be called as: map_name["obj_str"]["var_str"]
	//Basically, a map of RPGSH_OBJ acts as a 2D std::map which takes two string coordinates and returns an RPGSH_VAR
	T& operator [] (const std::string s)
	{
		return Properties[s];
	}

	//Iterator type for the class
	using iterator = typename std::map<std::string, T>::const_iterator;

	//Beginning and end iterators. This is so I can use "for(const auto& [key,val] : RPGSH_OBJ){}"
	iterator begin() const
	{
		return Properties.begin();
	}
	iterator end() const
	{
		return Properties.end();
	}

	RPGSH_OBJ(){}
	RPGSH_OBJ(std::string s, T var)
	{
		Properties[s] = var;
	}
	RPGSH_OBJ(std::map<std::string, T> map)
	{
		for(auto const& [key, value] : map)
		{
			Properties[key] = value;
		}
	}

	void clear()
	{
		Properties.clear();
	}
};
