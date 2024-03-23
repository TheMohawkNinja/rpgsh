#pragma once

#include <map>
#include <string>

class RPGSH_VAR
{
	public:
		std::string Value = "";

		explicit operator std::string() const;
		explicit operator int() const;
		RPGSH_VAR& operator = (const RPGSH_VAR b);
		RPGSH_VAR& operator = (const std::string b);
		RPGSH_VAR& operator = (const int b);
		RPGSH_VAR& operator += (const RPGSH_VAR b);
		RPGSH_VAR& operator += (const std::string b);
		RPGSH_VAR& operator += (const int b);
		RPGSH_VAR& operator -= (const RPGSH_VAR b);
		RPGSH_VAR& operator -= (const int b);
		RPGSH_VAR& operator *= (const RPGSH_VAR b);
		RPGSH_VAR& operator *= (const int b);
		RPGSH_VAR& operator /= (const RPGSH_VAR b);
		RPGSH_VAR& operator /= (const int b);
		RPGSH_VAR& operator ++ (const int b);
		RPGSH_VAR& operator -- (const int b);
		RPGSH_VAR operator + (const RPGSH_VAR b);
		RPGSH_VAR operator + (const std::string b);
		RPGSH_VAR operator + (const int b);
		RPGSH_VAR operator - (const RPGSH_VAR b);
		RPGSH_VAR operator - (const int b);
		RPGSH_VAR operator * (const RPGSH_VAR b);
		RPGSH_VAR operator * (const int b);
		RPGSH_VAR operator / (const RPGSH_VAR b);
		RPGSH_VAR operator / (const int b);

		// Comparison operators
		bool operator != (const RPGSH_VAR b);

	RPGSH_VAR();
	RPGSH_VAR(std::string _value);
	RPGSH_VAR(int _value);

	const char* c_str() const;
};
