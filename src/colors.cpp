#include "../headers/colors.h"
#include "../headers/functions.h"

RGB Colors::asHTML(std::string color)
{
	for(const auto& [k,v] : color_map)
		if(!stringcasecmp(k,color)) return v;

	return {0,0,0};
}

RGB Colors::asCurses(std::string color)
{
	for(const auto& [k,v] : color_map)
		if(!stringcasecmp(k,color))
			return {(unsigned int)((float)v.r*CURSES_COEFFICIENT),
				(unsigned int)((float)v.g*CURSES_COEFFICIENT),
				(unsigned int)((float)v.b*CURSES_COEFFICIENT)};

	return {0,0,0};
}
