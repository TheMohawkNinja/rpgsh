#include <vector>
#include <filesystem>
#include "classes.h"

RPGSH_CHAR c = RPGSH_CHAR();
RPGSH_VAR v = RPGSH_VAR();

bool is_operator(std::string s)
{
	if(s == "="  ||
	   s == "+=" || s == "-=" || s== "*=" || s == "/=" ||
	   s == "+"  || s == "-"  || s == "*" || s == "/")
	{
		return true;
	}
	else{return false;}
}
bool is_int(char* s)
{
	try
	{
		std::stoi(std::string(s));
		return true;
	}
	catch(...)
	{
		return false;
	}
}
void set_var(std::string var, std::string old, std::string value, bool is_char_var, std::vector<std::string> shell_vars)
{
	std::string var_type;

	if(!is_char_var)
	{
		var_type = "Shell variable";
	}
	else
	{
		var_type = "Character attribute";
	}

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
		RPGSH_OUTPUT(Warning,"%s \"%s\" is changing from an integer to a string.",var_type.c_str(),var.c_str());
	}
	else if(!old_is_digit && new_is_digit)
	{
		RPGSH_OUTPUT(Warning,"%s \"%s\" is changing from a string to an integer.",var_type.c_str(),var.c_str());
	}

	if(!is_char_var)
	{
		std::filesystem::remove(shell_vars_path.c_str());
		std::ofstream ofs(shell_vars_path.c_str());
		for(int i=0; i<shell_vars.size(); i++)
		{
			ofs<<shell_vars[i];
		}
		ofs<<var+v.DataSeparator+value;
		ofs.close();
	}
	else
	{
		c.Attr[var] = value;
		c.save();
	}
	RPGSH_OUTPUT(Info,"%s \"%s\" has been changed from \"%s\" to \"%s\".",var_type.c_str(),var.c_str(),old.c_str(),value.c_str());
}

int main(int argc, char** argv)
{
	bool is_char_var = (argv[2][0] == CHAR_VAR);
	std::vector<std::string> shell_vars;
	std::string var = std::string(argv[2]).substr(1,std::string(argv[2]).length()-1);
	std::string old;

	if(!is_char_var)
	{
		std::ifstream ifs(shell_vars_path.c_str());
		for(int i=0; !ifs.eof(); i++)
		{
			std::string data = "";
			std::getline(ifs,data);

			if(data.substr(0,data.find(v.DataSeparator)) == var)
			{
				old = data.substr(data.find(v.DataSeparator)+2,data.length()-(data.find(v.DataSeparator)+2));
			}
			else
			{
				shell_vars.push_back(data);
			}
		}
		ifs.close();
	}
	else
	{
		c.load(false);
		old = std::string(c.Attr[var]);
	}

	if(argc == 3)
	{
		fprintf(stdout,"%s\n",old.c_str());
	}
	else if(argc == 4)
	{
		if(is_operator(std::string(argv[3])))
		{
			RPGSH_OUTPUT(Error,"Expected value after \'%s\'.",argv[argc-1]);
			exit(-1);
		}
		else
		{
			RPGSH_OUTPUT(Error,"Expected operator before \'%s\'.",argv[argc-1]);
			exit(-1);
		}
	}
	else if(argc == 5)
	{
		if(!is_operator(argv[3]))
		{
			RPGSH_OUTPUT(Error,"Expected operator at \'%s\'.",argv[3]);
			exit(-1);
		}

		RPGSH_VAR value = RPGSH_VAR(old);
		if(!strcmp(argv[3],"="))
		{
			set_var(var,old,argv[4],is_char_var,shell_vars);
			return 0;
		}
		else if(!strcmp(argv[3],"+="))
		{
			if(is_int(argv[4]))
			{
				value += std::stoi(argv[4]);
			}
			else
			{
				value += std::string(argv[4]);
			}
		}
		else if(!strcmp(argv[3],"-="))
		{
			if(is_int(argv[4]))
			{
				value -= std::stoi(argv[4]);
			}
			else
			{
				RPGSH_OUTPUT(Error,"Cannot subtract \'%s\' from left-hand operand.",argv[4]);
				exit(-1);
			}
		}
		else if(!strcmp(argv[3],"*="))
		{
			if(is_int(argv[4]))
			{
				value *= std::stoi(argv[4]);
			}
			else
			{
				RPGSH_OUTPUT(Error,"Cannot multiply left-hand operand by \'%s\'.",argv[4]);
				exit(-1);
			}
		}
		else if(!strcmp(argv[3],"/="))
		{
			if(is_int(argv[4]))
			{
				value /= std::stoi(argv[4]);
			}
			else
			{
				RPGSH_OUTPUT(Error,"Cannot divide left-hand operand by \'%s\'.",argv[4]);
				exit(-1);
			}
		}
		else
		{
			RPGSH_OUTPUT(Error,"Invalid operator \'%s\'.",argv[3]);
			exit(-1);
		}
		set_var(var,old,std::string(value),is_char_var,shell_vars);
	}
	else
	{
		RPGSH_VAR value = std::string(argv[4]);
		for(int i=4; i<argc; i++)
		{
			if(!i%2 && is_operator(argv[i]))
			{
				RPGSH_OUTPUT(Error,"Expected non-operator value at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 && !is_operator(argv[i]))
			{
				RPGSH_OUTPUT(Error,"Expected operator at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2)
			{
				if(!strcmp(argv[i],"+"))
				{
					if(is_int(argv[i+1]))
					{
						value += std::stoi(argv[i+1]);
					}
					else
					{
						value += std::string(argv[i+1]);
					}
				}
				else if(!strcmp(argv[i],"-"))
				{
					if(is_int(argv[i+1]))
					{
						value -= std::stoi(argv[i+1]);
					}
					else
					{
						RPGSH_OUTPUT(Error,"Cannot subtract \'%s\' from left-hand operand.",argv[i+1]);
						exit(-1);
					}
				}
				else if(!strcmp(argv[i],"*"))
				{
					if(is_int(argv[i+1]))
					{
						value *= std::stoi(argv[i+1]);
					}
					else
					{
						RPGSH_OUTPUT(Error,"Cannot multiply left-hand operand by \'%s\'.",argv[i+1]);
						exit(-1);
					}
				}
				else if(!strcmp(argv[i],"/"))
				{
					if(is_int(argv[i+1]))
					{
						value /= std::stoi(argv[i+1]);
					}
					else
					{
						RPGSH_OUTPUT(Error,"Cannot divide left-hand operand by \'%s\'.",argv[i+1]);
						exit(-1);
					}
				}
				else
				{
					RPGSH_OUTPUT(Error,"Invalid operator \'%s\'.",argv[i]);
					exit(-1);
				}
			}
		}
		set_var(var,old,std::string(value),is_char_var,shell_vars);
	}

	if(is_char_var)
	{
		c.set_current();
		c.save();
	}
	return 0;
}
