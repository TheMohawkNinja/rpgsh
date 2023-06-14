#include <vector>
#include "classes.h"

DNDSH_CHAR c = DNDSH_CHAR();

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
		DNDSH_OUTPUT(Warning,"\"%s\" is changing from an integer to a string.",var.c_str());
	}
	else if(!old_is_digit && new_is_digit)
	{
		DNDSH_OUTPUT(Warning,"\"%s\" is changing from a string to an integer.",var.c_str());
	}

	c.Attr[var] = value;
	DNDSH_OUTPUT(Info,"\"%s\" has been changed from \"%s\" to \"%s\".",var.c_str(),old.c_str(),value.c_str());
}

int main(int argc, char** argv)
{
	c.load(false);

	std::string var;
	std::vector<std::string> v_argv = argv_handler(argc, argv);

	for(int i=0; i<v_argv.size(); i++)
	{
		if(std::string(v_argv[i]).substr(0,1) == "%")
		{
			var = std::string(v_argv[i]).substr(1,sizeof(v_argv[i])-1);
		}
	}

	if(argc == 3)
	{
		fprintf(stdout,"%s\n",c.Attr[var].c_str());
	}
	else
	{
		if(v_argv[3] == "=")
		{
			set_var(var,v_argv[4]);
		}
	}

	c.set_current();
	c.save();
	return 0;
}
