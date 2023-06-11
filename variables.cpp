#include <vector>
#include "classes.h"

int set_var(std::string var, std::string value)
{
	bool old_is_digit, new_is_digit;

	try
	{
		std::stoi(std::string(
	}
}

int main(int argc, char** argv)
{
	DNDSH_CHAR c = DNDSH_CHAR();
	c.load();

	std::vector<std::string> v_argv = argv_handler(argc, argv);
	std::vector<std::string> vars;

	for(int i=0; i<v_argv.size(); i++)
	{
		if(std::string(v_argv[i]).substr(0,1) == "%")
		{
			vars.push_back(std::string(v_argv[i]).substr(1,sizeof(v_argv[i])-1));
		}
	}

	if(argc == 3)
	{
		fprintf(stdout,"%s\n",std::string(c.Attr[vars[0]]).c_str());
	}
	else
	{
		if(v_argv[3] == "=")
		{
			std::string old = std::string(c.Attr[vars[0]]);
			c.Attr[vars[0]] = v_argv[4];
			DNDSH_OUTPUT(Info,"\"%s\" has been changed from \"%s\" to \"%s\".",vars[0].c_str(),old.c_str(),std::string(c.Attr[vars[0]]).c_str());
		}
	}

	c.set_current();
	c.save();
	return 0;
}
