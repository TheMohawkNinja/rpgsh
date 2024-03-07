#include "../headers/char.h"
#include "../headers/functions.h"
#include "../headers/var.h"

RPGSH_CHAR c = RPGSH_CHAR();

bool is_operator(std::string s)
{
	return (s == "="  ||
	   	s == "+"  || s == "-"  || s == "*"  || s == "/"  ||
	  	s == "+=" || s == "-=" || s == "*=" || s == "/=" ||
	  	s == "++" || s == "--");
}
bool is_int(std::string s)
{
	try
	{
		std::stoi(s);
		return true;
	}
	catch(...)
	{
		return false;
	}
}
void set_var(std::string var, RPGSH_VAR old_value, RPGSH_VAR new_value, char scope_sigil)
{
	std::string var_type;

	switch(scope_sigil)
	{
		case CHAR_SIGIL:
			var_type = "Character attribute";
			break;
		case CAMPAIGN_SIGIL:
			var_type = "Campaign variable";
			break;
		case SHELL_SIGIL:
			var_type = "Shell variable";
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",scope_sigil);
			exit(-1);
	}

	bool old_is_int = is_int(old_value.Value);
	bool new_is_int = is_int(new_value.Value);

	if(old_is_int && !new_is_int)
		output(Warning,"%s \"%s\" is changing from an integer to a string.",var_type.c_str(),var.c_str());
	else if(!old_is_int && new_is_int)
		output(Warning,"%s \"%s\" is changing from a string to an integer.",var_type.c_str(),var.c_str());

	switch(scope_sigil)
	{
		case CHAR_SIGIL:
			c.Attr[var] = new_value;
			c.save();
			c.set_as_current();
			break;
		case CAMPAIGN_SIGIL:
			set_campaign_var(var,std::string(new_value));
			break;
		case SHELL_SIGIL:
			set_shell_var(var,std::string(new_value));
			break;
	}
	if(old_value != new_value)
		output(Info,"%s \"%s\" has been changed from \"%s\" to \"%s\".",var_type.c_str(),var.c_str(),old_value.c_str(),new_value.c_str());
}

int main(int argc, char** argv)
{
	if(std::string(argv[1]).length() == 1)// If the user only enters the scope sigil
	{
		output(Error,"Empty variable name.");
		exit(-1);
	}

	std::string var = std::string(argv[1]).substr(1,std::string(argv[1]).length()-1);
	RPGSH_VAR old_value;

	switch(argv[1][0])
	{
		case CHAR_SIGIL:
			c.load(get_shell_var(CURRENT_CHAR_SHELL_VAR),false);
			old_value = std::string(c.Attr[var]);
			break;
		case CAMPAIGN_SIGIL:
			old_value = get_campaign_var(var);
			break;
		case SHELL_SIGIL:
			old_value = get_shell_var(var);
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",argv[1][0]);
			exit(-1);
	}

	if(argc == 2)
	{
		fprintf(stdout,"%s\n",old_value.c_str());
	}
	else if(argc == 3)
	{
		if(is_operator(std::string(argv[2])))
		{
			if(std::string(argv[2]) == "++")
			{
				set_var(var,old_value,old_value+1,argv[1][0]);
				return 0;
			}
			else if(std::string(argv[2]) == "--")
			{
				set_var(var,old_value,old_value-1,argv[1][0]);
				return 0;
			}
			else
			{
				output(Error,"Expected new value after \'%s\'.",argv[argc-1]);
				exit(-1);
			}
		}
		else
		{
			output(Error,"Expected operator before \'%s\'.",argv[argc-1]);
			exit(-1);
		}
	}
	else
	{
		RPGSH_VAR new_value = std::string(argv[3]);
		for(int i=4; i<argc; i++)
		{
			if(i%2 == 1 && is_operator(argv[i]))
			{
				output(Error,"Expected non-operator new value at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 == 0 && !is_operator(argv[i]))
			{
				output(Error,"Expected operator at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 == 0)
			{
				RPGSH_VAR current_operand = std::string(argv[i+1]);
				if(!strcmp(argv[i],"+"))
				{
					new_value += current_operand;
				}
				else if(!strcmp(argv[i],"-"))
				{
					new_value -= current_operand;
				}
				else if(!strcmp(argv[i],"*"))
				{
					new_value *= current_operand;
				}
				else if(!strcmp(argv[i],"/"))
				{
					new_value /= current_operand;
				}
				else
				{
					output(Error,"Invalid operator \'%s\'.",argv[i]);
					exit(-1);
				}
			}
		}

		if(!strcmp(argv[2],"="))
		{
			set_var(var,old_value,new_value,argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"+="))
		{
			set_var(var,old_value,std::string(old_value+new_value),argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"-="))
		{
			set_var(var,old_value,std::string(old_value-new_value),argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"*="))
		{
			set_var(var,old_value,std::string(old_value*new_value),argv[1][0]);
			return 0;
		}
		else if(!strcmp(argv[2],"/="))
		{
			set_var(var,old_value,std::string(old_value/new_value),argv[1][0]);
			return 0;
		}
		else
		{
			output(Error,"Invalid operator \'%s\'.",argv[2]);
			exit(-1);
		}
	}
	return 0;
}
