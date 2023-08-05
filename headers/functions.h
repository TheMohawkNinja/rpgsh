#pragma once

#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <cctype>
#include <climits>
#include <fstream>
#include <filesystem>
#include <cstdarg>
#include <unistd.h>
#include "define.h"
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

bool stob(std::string s)
{
	if(!strcasecmp(s.c_str(),"true"))
	{
		return true;
	}
	else if(!strcasecmp(s.c_str(),"false"))
	{
		return false;
	}
	else
	{
		throw std::invalid_argument("Parameter for stob() was not \'true\' or \'false\'.");
		return false;
	}
}

std::string get_shell_var(std::string var)
{
	std::ifstream ifs(shell_vars_file.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == var)
		{
			return data.substr(data.find(DS)+DS.length(),
				data.length()-(data.find(DS)+DS.length()));
		}
	}
	ifs.close();
	return "";
}
void set_shell_var(std::string var,std::string value)
{
	std::ifstream ifs(shell_vars_file.c_str());
	std::filesystem::remove((shell_vars_file+".bak").c_str());
	std::ofstream ofs((shell_vars_file+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == var)
		{
			ofs<<var + DS + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue)
	{
		ofs<<var + DS + value + "\n";
	}
	ifs.close();
	ofs.close();
	std::filesystem::remove(shell_vars_file.c_str());
	std::filesystem::rename((shell_vars_file+".bak").c_str(),shell_vars_file.c_str());
}

template <typename T>
void save_map_to_file(std::string path, std::map<std::string, T> map, std::string obj_id)
{
	std::ofstream fs(path, std::ios::app);
	if(!fs.good())
	{
		output(Error,"Unable to open \'%s\' for writing",path.c_str());
		exit(-1);
	}

	for(const auto& [key,value] : map)
	{
		//Template/Character file format definition
		std::string data =	obj_id+
					DS+
					key+
					DS+
					std::string(value)+
					"\n";
		fs<<data;
	}
	fs.close();
}

template <typename T>
std::map<std::string, T>load_map_from_file(std::string path, std::string obj_id)
{
	std::map<std::string, T> map;

	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \'%s\'",path.c_str());
		exit(-1);
	}

	std::ifstream fs(path);
	if(!fs.good())
	{
		output(Error,"Unable to open \'%s\' for reading",path.c_str());
		exit(-1);
	}

	while(!fs.eof())
	{
		std::string data = "";
		std::string obj = "";
		std::string key = "";
		T value;

		std::getline(fs,data);
		if(data != "")
		{
			if(data.substr(0,data.find(DS)) ==  obj_id)
			{
				obj = data.substr(0,data.find(DS));
				key = data.substr(data.find(obj)+obj.length()+DS.length(),
						 (data.rfind(DS)-(data.find(obj)+obj.length()+DS.length())));

				// IMPORTANT: This requires that there exist a constructor T(std::string)
				value = T(data.substr(data.rfind(DS)+DS.length(),
						     (data.length()-data.rfind(DS)+DS.length())));
				map[key] = value;
			}
		}
	}
	fs.close();

	return map;
}
