#include <map>
#include <regex>
#include "../headers/colors.h"
#include "../headers/define.h"
#include "../headers/pretty.h"
#include "../headers/scope.h"
#include "../headers/text.h"

std::string makePretty(std::string value)
{
	Character c = Character();

	if(value == "") return empty_str.c_str();

	std::map<std::string,const char*> ansi_esc_seqs = {
	{"/",			TEXT_NORMAL},
	{"b",			TEXT_BOLD},
	{"/b",			TEXT_NOBOLD},
	{"i",			TEXT_ITALIC},
	{"/i",			TEXT_NOITALIC},
	{"d",			TEXT_DIM},
	{"/d",			TEXT_NORMALINTENSITY},
	{"u",			TEXT_UNDERLINE},
	{"/u",			TEXT_NOUNDERLINE},
	{"blink",		TEXT_BLINK},
	{"/blink",		TEXT_NOBLINK},
	{"r",			TEXT_REVERSE},
	{"/r",			TEXT_NOREVERSE},
	{"/fg",			TEXT_DEFAULTCOLOR},
	{"black",		TEXT_BLACK},
	{"/black",		TEXT_DEFAULTCOLOR},
	{"red",			TEXT_RED},
	{"/red",		TEXT_DEFAULTCOLOR},
	{"green",		TEXT_GREEN},
	{"/green",		TEXT_DEFAULTCOLOR},
	{"yellow",		TEXT_YELLOW},
	{"/yellow",		TEXT_DEFAULTCOLOR},
	{"blue",		TEXT_BLUE},
	{"/blue",		TEXT_DEFAULTCOLOR},
	{"magenta",		TEXT_MAGENTA},
	{"/magenta",		TEXT_DEFAULTCOLOR},
	{"cyan",		TEXT_CYAN},
	{"/cyan",		TEXT_DEFAULTCOLOR},
	{"lightgray",		TEXT_LIGHTGRAY},
	{"/lightgray",		TEXT_DEFAULTCOLOR},
	{"darkgray",		TEXT_DARKGRAY},
	{"/darkgray",		TEXT_DEFAULTCOLOR},
	{"lightred",		TEXT_LIGHTRED},
	{"/lightred",		TEXT_DEFAULTCOLOR},
	{"lightgreen",		TEXT_LIGHTGREEN},
	{"/lightgreen",		TEXT_DEFAULTCOLOR},
	{"lightyellow",		TEXT_LIGHTYELLOW},
	{"/lightyellow",	TEXT_DEFAULTCOLOR},
	{"lightblue",		TEXT_LIGHTBLUE},
	{"/lightblue",		TEXT_DEFAULTCOLOR},
	{"lightmagenta",	TEXT_LIGHTMAGENTA},
	{"/lightmagenta",	TEXT_DEFAULTCOLOR},
	{"lightcyan",		TEXT_LIGHTCYAN},
	{"/lightcyan",		TEXT_DEFAULTCOLOR},
	{"white",		TEXT_WHITE},
	{"/white",		TEXT_DEFAULTCOLOR},
	{"/bg",			TEXT_BG_DEFAULTCOLOR},
	{"bgblack",		TEXT_BG_BLACK},
	{"/bgblack",		TEXT_BG_DEFAULTCOLOR},
	{"bgred",		TEXT_BG_RED},
	{"/bgred",		TEXT_BG_DEFAULTCOLOR},
	{"bggreen",		TEXT_BG_GREEN},
	{"/bggreen",		TEXT_BG_DEFAULTCOLOR},
	{"bgyellow",		TEXT_BG_YELLOW},
	{"/bgyellow",		TEXT_BG_DEFAULTCOLOR},
	{"bgblue",		TEXT_BG_BLUE},
	{"/bgblue",		TEXT_BG_DEFAULTCOLOR},
	{"bgmagenta",		TEXT_BG_MAGENTA},
	{"/bgmagenta",		TEXT_BG_DEFAULTCOLOR},
	{"bgcyan",		TEXT_BG_CYAN},
	{"/bgcyan",		TEXT_BG_DEFAULTCOLOR},
	{"bglightgray",		TEXT_BG_LIGHTGRAY},
	{"/bglightgray",	TEXT_BG_DEFAULTCOLOR},
	{"bgdarkgray",		TEXT_BG_DARKGRAY},
	{"/bgdarkgray",		TEXT_BG_DEFAULTCOLOR},
	{"bglightred",		TEXT_BG_LIGHTRED},
	{"/bglightred",		TEXT_BG_DEFAULTCOLOR},
	{"bglightgreen",	TEXT_BG_LIGHTGREEN},
	{"/bglightgreen",	TEXT_BG_DEFAULTCOLOR},
	{"bglightyellow",	TEXT_BG_LIGHTYELLOW},
	{"/bglightyellow",	TEXT_BG_DEFAULTCOLOR},
	{"bglightblue",		TEXT_BG_LIGHTBLUE},
	{"/bglightblue",	TEXT_BG_DEFAULTCOLOR},
	{"bglightmagenta",	TEXT_BG_LIGHTMAGENTA},
	{"/bglightmagenta",	TEXT_BG_DEFAULTCOLOR},
	{"bglightcyan",		TEXT_BG_LIGHTCYAN},
	{"/bglightcyan",	TEXT_BG_DEFAULTCOLOR},
	{"bgwhite",		TEXT_BG_WHITE},
	{"/bgwhite",		TEXT_BG_DEFAULTCOLOR}};

	for(long unsigned int i=0; i<value.length(); i++)
	{
		if(value[i] == '\\' && i == value.length())
		{
			value.erase(value.begin()+i);
		}
		else if(value[i] == '\\')
		{
			switch(value[i+1])
			{
				case 'a':
					value[i+1] = '\a'; //Bell
					break;
				case 'b':
					value[i+1] = '\b'; //Backspace
					break;
				case 'f':
					value[i+1] = '\f'; //Form feed
					break;
				case 'n':
					value[i+1] = '\n'; //Newline
					break;
				case 't':
					value[i+1] = '\t'; //Horizontal tab
					break;
				case 'v':
					value[i+1] = '\v'; //Vertical tab
					break;
			}
			value.erase(value.begin()+i);
		}
	}

	for(const auto& [k,v] : ansi_esc_seqs)
		value = std::regex_replace(value,std::regex("%"+k+"%",std::regex_constants::icase),std::string(v));

	value = std::regex_replace(value,std::regex("%name%",std::regex_constants::icase),c.getName());

	std::regex fg_rgb_color_pattern("%fg=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}%");
	std::regex fg_html_color_pattern("%fg=[a-zA-Z]{1,}%");
	std::regex bg_rgb_color_pattern("%bg=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}%");
	std::regex bg_html_color_pattern("%bg=[a-zA-Z]{1,}%");

	std::sregex_iterator fg_rgb_it(value.begin(),value.end(),fg_rgb_color_pattern);
	std::sregex_iterator fg_html_it(value.begin(),value.end(),fg_html_color_pattern);
	std::sregex_iterator bg_rgb_it(value.begin(),value.end(),bg_rgb_color_pattern);
	std::sregex_iterator bg_html_it(value.begin(),value.end(),bg_html_color_pattern);

	std::sregex_iterator it_end;

	std::map<std::string,std::string> string_replacements;

	while(fg_rgb_it != it_end)
	{
		std::string rgb_str = fg_rgb_it->str();
		std::regex color_channels_pattern("[0-9]{1,3}");
		std::sregex_iterator color_channels_it(rgb_str.begin(),rgb_str.end(),color_channels_pattern);
		unsigned char r = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char g = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char b = (char)std::stoi(color_channels_it->str());

		string_replacements[rgb_str] = std::string(TEXT_FG_8BIT_PRE)+std::to_string(r)+";"+std::to_string(g)+";"+std::to_string(b)+"m";
		fg_rgb_it++;
	}
	while(fg_html_it != it_end)
	{
		std::string html_str = fg_html_it->str().substr(4,fg_html_it->str().length()-5);//Pull out just the color name
		Colors colors = Colors();
		RGB cc = colors.as24Bit(html_str);

		string_replacements[fg_html_it->str()] = std::string(TEXT_FG_8BIT_PRE)+std::to_string(cc.r)+";"+std::to_string(cc.g)+";"+std::to_string(cc.b)+"m";
		fg_html_it++;
	}
	while(bg_rgb_it != it_end)
	{
		std::string rgb_str = bg_rgb_it->str();
		std::regex color_channels_pattern("[0-9]{1,3}");
		std::sregex_iterator color_channels_it(rgb_str.begin(),rgb_str.end(),color_channels_pattern);
		unsigned char r = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char g = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char b = (char)std::stoi(color_channels_it->str());

		string_replacements[rgb_str] = std::string(TEXT_BG_8BIT_PRE)+std::to_string(r)+";"+std::to_string(g)+";"+std::to_string(b)+"m";
		bg_rgb_it++;
	}
	while(bg_html_it != it_end)
	{
		std::string html_str = bg_html_it->str().substr(4,bg_html_it->str().length()-5);//Pull out just the color name
		Colors colors = Colors();
		RGB cc = colors.as24Bit(html_str);

		string_replacements[bg_html_it->str()] = std::string(TEXT_BG_8BIT_PRE)+std::to_string(cc.r)+";"+std::to_string(cc.g)+";"+std::to_string(cc.b)+"m";
		bg_html_it++;
	}

	for(const auto& [k,v] : string_replacements) value = std::regex_replace(value,std::regex(k),v);
	value = std::regex_replace(value,std::regex("%"),"%%");//Prevent printf() parsing '%' as format specifiers

	value += std::string(TEXT_NORMAL); //Make sure we don't carry over any unterminated formatting
	return value;
}
std::string stripFormatting(std::string str)
{
	for(unsigned long int i=0; i<str.length(); i++)
	{
		const std::string::iterator cur_it = str.begin()+i;
		if(str[i] == ESC_SEQ)
		{
			str.erase(cur_it,str.begin()+str.find('m',i)+1);
			i--;
		}
		else if(str[i] == '\b' && i)
		{
			str.erase(cur_it-1,cur_it-1);
		}
	}
	return str;
}
