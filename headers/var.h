#pragma once

#include <string>

class var_t
{
	public:
		std::string Value = "";

		explicit operator std::string() const;
		explicit operator int() const;
		var_t& operator = (const var_t b);
		var_t& operator = (const std::string b);
		var_t& operator = (const int b);
		var_t& operator += (const var_t b);
		var_t& operator += (const std::string b);
		var_t& operator += (const int b);
		var_t& operator -= (const var_t b);
		var_t& operator -= (const int b);
		var_t& operator *= (const var_t b);
		var_t& operator *= (const int b);
		var_t& operator /= (const var_t b);
		var_t& operator /= (const int b);
		var_t& operator ++ (const int b);
		var_t& operator -- (const int b);
		var_t operator + (const var_t b);
		var_t operator + (const std::string b);
		var_t operator + (const int b);
		var_t operator - (const var_t b);
		var_t operator - (const int b);
		var_t operator * (const var_t b);
		var_t operator * (const int b);
		var_t operator / (const var_t b);
		var_t operator / (const int b);

		// Comparison operators
		bool operator != (const var_t b);

	var_t();
	var_t(std::string _value);
	var_t(int _value);

	const char* c_str() const;
};
