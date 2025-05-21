#include <vector>
#include <strings.h>
#include "../headers/string.h"
#include "../headers/var.h"

bool stob(std::string s)
{
	return !stringcasecmp(s,"true");
}
std::string btos(bool b)
{
	if(b) return "True";
	return "False";
}
bool isEscaped(std::string str, long unsigned int pos)
{
	if(pos <= 0 || pos >= str.length()) return false;
	else return str[pos-1] == '\\';
}
int stringcasecmp(std::string a, std::string b)
{
	return strcasecmp(a.c_str(),b.c_str());
}
std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> list;
	if(findu(str,delimiter) == std::string::npos)
	{
		list.push_back(str);
		return list;
	}

	while(true)
	{
		if(findu(str,delimiter) == std::string::npos)
			return list;

		list.push_back(left(str,findu(str,delimiter)));
		str = right(str,findu(str,delimiter)+1);
	}

	return list;
}
long unsigned int findu(std::string str, std::string match, long unsigned int start)
{
	if(match.length() > str.length()) return std::string::npos;
	for(long unsigned int i=start; i<str.length()-(match.length()-1); i++)
		if(str.substr(i,match.length()) == match && !isEscaped(str.substr(i,match.length()),i)) return i;

	return std::string::npos;
}
long unsigned int findu(std::string str, char ch, long unsigned int start)
{
	return findu(str, std::string(1,ch), start);
}
long unsigned int rfindu(std::string str, std::string match, long unsigned int start)
{
	if(match.length() > str.length()) return std::string::npos;
	if(start == UINT_MAX) start = str.length();

	bool last_check = false;
	for(long unsigned int i=start-match.length(); i>0; i--)
	{
		if(str.substr(i-last_check,match.length()) == match &&
		   !isEscaped(str.substr(i-last_check,match.length()),i-last_check)) return i-last_check;
		else if(i == 1 && !last_check)
		{
			i++;
			last_check = true;
		}
	}

	return std::string::npos;
}
long unsigned int rfindu(std::string str, char ch, long unsigned int start)
{
	return rfindu(str, std::string(1,ch), start);
}
std::string left(std::string str, int n)
{
	return str.substr(0,n);
}
std::string right(std::string str, int n)
{
	return str.substr(n,str.length()-n);
}
long unsigned int countu(std::string str, char ch)
{
	long unsigned int count = 0;
	for(long unsigned int i=0; i<str.length(); i++)
		if(str[i] == ch && !isEscaped(str,i)) count++;

	return count;
}
std::string addSpaces(unsigned int n)
{
	std::string ret = "";
	for(unsigned int i=0; i<n; i++) ret += " ";

	return ret;
}
std::string mergeQuotes(std::string str)
{
	std::string ret = "";

	// When running a program, rpgsh should already cut off any charaters outside of unescaped quotation marks.
	// Therefore, we shouldn't need to check for odd inputs

	// If neither end has quotes, just return the string as is.
	if(str.front() != '\"' && (str.back() != '\"' || isEscaped(str,str.length()-1)))
		return str;

	// Strip unescaped quote marks out from anywhere else but the ends of the string
	for(long unsigned int i=0; i<str.length(); i++)
	{
		if(str[i] != '\"' || (str[i] == '\"' && isEscaped(str,i)))
			ret += str[i];
	}

	// If the resulting string isn't a valid integer, return the quote-stripped string
	if(!Var(ret).isInt())
		return ret;

	// The string is a valid integer, and also had quotes around it, so it still should be reated as a string
	return '\"'+ret+'\"';
}
std::string escapeSpaces(std::string str)
{
	std::string ret = "";

	for(long unsigned int i=0; i<str.length(); i++)
	{
		if(str[i] == ' ' && !isEscaped(str,i))
			ret += "\\ ";
		else
			ret += str[i];
	}

	return ret;
}
MCStr parseMCStr(std::string s)
{
	MCStr mc;
	unsigned long int slash = findu(s,'/');
	if(slash == std::string::npos)
	{
		mc.c = s;
		return mc;
	}
	else
	{
		mc.m = left(s,slash);
		mc.c = right(s,slash+1);
	}
	return mc;
}
