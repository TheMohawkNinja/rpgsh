#pragma once

#include "text.h"

enum output_level
{
	Info,
	Warning,
	Error
};
void output(output_level level, const char* format, ...)
{
	FILE* stream;
	std::string prefix = "";
	std::string color;
	va_list args;
	va_start(args, format);

	switch(level)
	{
		case Info:
			stream = stdout;
			prefix = "%s%s[%sINFO%s]%s    ";
			color = TEXT_CYAN;
			break;
		case Warning:
			stream = stderr;
			prefix = "%s%s[%sWARNING%s]%s ";
			color = TEXT_YELLOW;
			break;
		case Error:
			stream = stderr;
			prefix = "%s%s[%sERROR%s]%s   ";
			color = TEXT_RED;
			break;
	}
	fprintf(stream,prefix.c_str(),TEXT_BOLD,TEXT_WHITE,color.c_str(),TEXT_WHITE,color.c_str());
	vfprintf(stream,format,args);
	fprintf(stream,"%s\n",TEXT_NORMAL);
}
