#include "../headers/colors.h"
#include "../headers/functions.h"

RGB Colors::as24Bit(std::string color)
{
	for(const auto& [k,v] : color_map)
		if(!stringcasecmp(k,color)) return v;

	return BAD_COLOR;
}

RGB Colors::asCurses(std::string color)
{
	for(const auto& [k,v] : color_map)
		if(!stringcasecmp(k,color))
			return {(unsigned int)((float)v.r*CURSES_COEFFICIENT),
				(unsigned int)((float)v.g*CURSES_COEFFICIENT),
				(unsigned int)((float)v.b*CURSES_COEFFICIENT)};

	return BAD_COLOR;
}
