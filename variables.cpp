#include <vector>
#include "classes.h"

RPGSH_CHAR c = RPGSH_CHAR();

void set_var(std::string var, std::string value)
{
	std::string old = std::string(c.Attr[var]);
	bool old_is_digit, new_is_digit;

	try
	{
		std::stoi(old);
		old_is_digit = true;
	}
	catch(...)
	{
		old_is_digit = false;
	}

	try
	{
		std::stoi(value);
		new_is_digit = true;
	}
	catch(...)
	{
		new_is_digit = false;
	}

	if(old_is_digit && !new_is_digit)
	{
		RPGSH_OUTPUT(Warning,"\"%s\" is changing from an integer to a string.",var.c_str());
	}
	else if(!old_is_digit && new_is_digit)
	{
		RPGSH_OUTPUT(Warning,"\"%s\" is changing from a string to an integer.",var.c_str());
	}

	c.Attr[var] = value;
	RPGSH_OUTPUT(Info,"\"%s\" has been changed from \"%s\" to \"%s\".",var.c_str(),old.c_str(),value.c_str());
}

int main(int argc, char** argv)
{
	c.load(false);

	for(int i=0; i<argc; i++)
	{
		fprintf(stdout,"argv[%d]: \'%s\'\n",i,argv[i]);
	}

	std::string var = std::string(argv[2]).substr(1,std::string(argv[2]).length()-1);

	if(argc == 3)
	{
		fprintf(stdout,"%s\n",c.Attr[var].c_str());
	}
	else
	{
		if(!strcmp(argv[3],"="))
		{
			set_var(var,std::string(argv[4]));
		}
	}

	c.set_current();
	c.save();
	return 0;
}
