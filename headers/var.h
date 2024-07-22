#pragma once

#include <string>

class Var
{
	public:
		std::string Value = "";

		Var(const Var& b);
		explicit operator std::string() const;
		explicit operator int() const;
		Var& operator = (const Var b);
		Var& operator = (const std::string b);
		Var& operator = (const int b);
		Var& operator += (const Var b);
		Var& operator += (const std::string b);
		Var& operator += (const int b);
		Var& operator -= (const Var b);
		Var& operator -= (const int b);
		Var& operator *= (const Var b);
		Var& operator *= (const int b);
		Var& operator /= (const Var b);
		Var& operator /= (const int b);
		Var& operator ++ (int);
		Var& operator -- (int);
		Var operator + (const Var b);
		Var operator + (const std::string b);
		Var operator + (const int b);
		Var operator - (const Var b);
		Var operator - (const int b);
		Var operator * (const Var b);
		Var operator * (const int b);
		Var operator / (const Var b);
		Var operator / (const int b);

		// Comparison operators
		bool operator != (const Var b);

	Var();
	Var(std::string _value);
	Var(int _value);

	const char* c_str() const;
};
