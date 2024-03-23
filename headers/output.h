#pragma once

enum output_level
{
	Info,
	Warning,
	Error
};
void output(output_level level, const char* format, ...);
