#pragma once

enum OutputLevel
{
	Info,
	Warning,
	Error
};

void output(OutputLevel level, const char* format, ...);
