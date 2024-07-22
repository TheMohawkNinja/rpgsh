#pragma once

#include <string>
#include <vector>
#include "../headers/var.h"

class Dice
{
	private:
		bool just_show_rolls = false;
		bool just_show_total = false;
		bool is_list = false;
		unsigned int count = 0;
		unsigned int total_count = 0;
		std::string dice_list = "";
		std::string Quantity_str = "";
		std::string Faces_str = "";
		std::string count_expr = "";
		std::vector<int> result_quantity;

	int get_value(std::string d, std::string value, long unsigned int start, std::string terminator, bool allow_sign, bool required);

	public:
		unsigned int	Quantity	=	0;
		unsigned int	Faces		=	0;
			 int	Modifier	=	0;
		std::string	List		=	"";

		operator std::string() const;

	Dice();
	Dice(std::string dice_str);
	Dice(unsigned int _Quantity, unsigned int _Faces, int _Modifier);
	Dice(std::string dice_str, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count);
	Dice(unsigned int _Quantity, unsigned int _Faces, int _Modifier, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count);

	Dice& operator ++ (int);
	Dice& operator -- (int);
	bool operator == (const Dice& b) const;
	bool operator != (const Dice& b) const;

	std::string dice() const;
	const char* c_str() const;
	void roll();
	void test();
};
