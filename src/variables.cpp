#include <cstring>
#include "../headers/char.h"
#include "../headers/functions.h"
#include "../headers/var.h"

rpgsh_char c = rpgsh_char();

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
void set_var(std::string var, Var old_value, Var new_value, char Scope_sigil)
{
	std::string Varype;

	switch(Scope_sigil)
	{
		case CHAR_SIGIL:
			Varype = "Character attribute";
			break;
		case CAMPAIGN_SIGIL:
			Varype = "Campaign variable";
			break;
		case SHELL_SIGIL:
			Varype = "Shell variable";
			break;
		default:
			output(Error,"Unknown Scope sigil \'%c\'.",Scope_sigil);
			exit(-1);
	}

	bool old_is_int = is_int(old_value.Value);
	bool new_is_int = is_int(new_value.Value);

	if(old_is_int && !new_is_int)
		output(Warning,"%s \"%s\" is changing from an integer to a string.",Varype.c_str(),var.c_str());
	else if(!old_is_int && new_is_int)
		output(Warning,"%s \"%s\" is changing from a string to an integer.",Varype.c_str(),var.c_str());

	switch(Scope_sigil)
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
		output(Info,"%s \"%s\" has been changed from \"%s\" to \"%s\".",Varype.c_str(),var.substr(1,var.length()-1).c_str(),old_value.c_str(),new_value.c_str());
}

int main(int argc, char** argv)
{
	if(argv[1] && !strcmp(argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"Not meant for direct call by user. Implicitly called when modifying variables.\n");
		return 0;
	}

	if(std::string(argv[1]).length() == 1)// If the user only enters the Scope sigil
	{
		output(Error,"Empty variable name.");
		exit(-1);
	}

	std::map<std::string,std::string> m;
	std::string path;
	std::string var = std::string(argv[1]).substr(1,std::string(argv[1]).length()-1);
	Var old_value;

	switch(argv[1][0])// Get Scope sigil
	{
		case CHAR_SIGIL:
			c.load(get_shell_var(CURRENT_CHAR_SHELL_VAR),false);
			switch (var[var.length()-1])
			{
				case '/':
				old_value = std::string(c.Attr[var.substr(0,var.length()-1)]);
				break;

				default:
				old_value = std::string(c.Attr[var]);
				break;
			}

			// Convert character sheet to std::map<std::string,std::string>
			// TODO Will need to add currency, currency system, and wallet
			for(const auto& [k,v] : c.Attr)
				m[k] = std::string(v);
			for(const auto& [k,v] : c.Dice)
				m[k] = std::string(v);
			break;
		case CAMPAIGN_SIGIL:
			switch (var[var.length()-1])
			{
				case '/':
				try
				{
					old_value = get_campaign_var(var.substr(0,var.length()-1));
				}
				catch(...){old_value = "";}
				break;

				default:
				old_value = get_campaign_var(var);
				break;
			}
			path = campaigns_dir +
				get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
				".vars";
			m = load_vars_from_file(path);
			break;
		case SHELL_SIGIL:
			switch (var[var.length()-1])
			{
				case '/':
				try
				{
					old_value = get_shell_var(var.substr(0,var.length()-1));
				}
				catch(...){old_value = "";}
				break;

				default:
				old_value = get_shell_var(var);
				break;
			}
			path = shell_vars_file;
			m = load_vars_from_file(path);
			break;
		default:
			output(Error,"Unknown Scope sigil \'%c\'.",argv[1][0]);
			exit(-1);
	}

	if(argc == 2)// Print value if all the user enters is a variable
	{
		unsigned int space = 0;
		std::string var(argv[1]);

		if(var[var.length()-1] != '/')// If last character isn't a '/', just print value
			fprintf(stdout,"%s\n",old_value.c_str());
		else// Print the value and everything downstream of it
		{
			for(const auto& [k,v] : m)
			{
				if(k.substr(0,var.length()-1) == var.substr(1,var.length()-1) && k.length() > space)
					space = k.length();
			}
			if(old_value.Value != "")// Print parent value if exists
			{
				fprintf(stdout,"%c%s",argv[1][0],var.substr(1,var.length()-2).c_str());
				for(int i=var.length()-2; i<space+5; i++)
					fprintf(stdout," ");
				fprintf(stdout,"%s\n",old_value.c_str());
			}
			for(const auto& [k,v] : m)
			{
				try
				{
					if(k.substr(0,var.length()-1) == var.substr(1,var.length()-1) &&
					k != var.substr(1,var.length()-1 && v != ""))//TODO start of var.substr may need to change from 1 to 2 for Scope sigil
					{
						fprintf(stdout,"%c%s",argv[1][0],k.c_str());
						for(int i=k.length(); i<space+5; i++)
							fprintf(stdout," ");
						fprintf(stdout,"%s\n",v.c_str());
					}
				}
				catch(...){}
			}
		}
	}
	else if(argc == 3)// Unary operators
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
	else// Binary operators
	{
		Var new_value = std::string(argv[3]);
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
				Var current_operand = std::string(argv[i+1]);
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
