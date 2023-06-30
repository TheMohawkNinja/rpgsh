#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include "text.h"
#include "classes.h"

#define MAX_BUFFER_SIZE 256

RPGSH_CHAR c = RPGSH_CHAR();
RPGSH_VAR v = RPGSH_VAR();

void run_rpgsh_prog(RPGSH_CHAR c, std::string args)
{
	std::vector<std::string> vars;
	extern char** environ;
	pid_t pid;

	//Set char*[] for args.
	int params_start = args.find(" ", args.find(" ")+1);
	std::string prefix = "rpgsh-";
	std::string path = "./";

	if(params_start != std::string::npos)
	{
		//Replaces all instances of variables with their respective value
		//Except for the first arg if it is a variable
		//TODO if the regex lib is ever properly fully-implemented, replace this shit with regex like God intended
		for(int i=params_start+1; i<args.length(); i++)
		{
			params_start = args.find(" ", args.find(" ")+1);
			std::string var = "";
			if(args[i] == CHAR_VAR)
			{
				int find_percent = args.find(CHAR_VAR,params_start);
				for(int j=i+1; j<args.length() && args.substr(j,1) != " "; j++)
				{
					var+=args.substr(j,1);
				}
				int new_args_start = find_percent + var.length() + 1;
				args = args.substr(0,find_percent)+c.Attr[var].c_str()+
					args.substr(new_args_start,args.length()-(args.substr(0,new_args_start).length()));
			}
			else if(args[i] == SHELL_VAR)
			{
				int find_dollar = args.find(SHELL_VAR,params_start);
				for(int j=i+1; j<args.length() && args.substr(j,1) != " "; j++)
				{
					var+=args.substr(j,1);
				}

				std::ifstream ifs(shell_vars_path.c_str());
				while(!ifs.eof())
				{
					std::string old;
					std::string data = "";
					std::getline(ifs,data);

					if(data.substr(0,data.find(v.DataSeparator)) == var)
					{
						int new_args_start = find_dollar + var.length() + 1;
						old = data.substr(data.find(v.DataSeparator)+v.DataSeparator.length(),
								data.length()-(data.find(v.DataSeparator)+v.DataSeparator.length()));

						if(old.find(" ") != std::string::npos)
						{
							old = "\"" + old + "\"";
						}

						args = args.substr(0,find_dollar)+old+
							args.substr(new_args_start,args.length()-(args.substr(0,new_args_start).length()));
						break;
					}
				}
				ifs.close();
			}
		}
	}
	if(args.find(" ") != std::string::npos)
	{
		vars.push_back(path+prefix+args.substr(0,args.find(" ")));
		args = args.substr(args.find(" ")+1,(args.length() - args.find(" ")+1));
		vars.push_back(std::string(c.Attr["Name"]));

		for(int i=0; i<args.length(); i++)
		{
			if(args.substr(i,1) == "\"")//Combine args wrapped in quotes
			{
				if(args.find("\"",i+1) == std::string::npos)
				{
					RPGSH_OUTPUT(Error,"Unmatched quote in argument list.");
					return;
				}
				vars.push_back(args.substr(i+1,(args.find("\"",i+1)-i-1)));
				i+=vars[vars.size()-1].length()+1;
			}
			else
			{
				if(args.find(" ",i) != std::string::npos)
				{
					std::string var = args.substr(i,args.find(" ",i)-i);
					bool var_is_valid = false;
					for(int c_i=0; c_i<var.length(); c_i++)//Make sure we're not just grabbing extraneous whitespace
					{
						std::string c = var.substr(c_i,1);
						if(c != " " && c != "")
						{
							var_is_valid = true;
						}
					}

					if(var_is_valid)
					{
						vars.push_back(args.substr(i,args.find(" ",i)-i));
						i+=vars[vars.size()-1].length();
					}
				}
				else
				{
					vars.push_back(args.substr(i,args.length()-i));
					break;
				}
			}
		}
	}
	else//If only one arg is called
	{
		vars.push_back(path+prefix+args);
		vars.push_back(std::string(c.Attr["Name"]));
	}

	char* argv[vars.size()+1];
	for(int i=0; i<vars.size(); i++)
	{
		argv[i] = (char*)vars[i].c_str();
	}
	argv[vars.size()] = NULL;

	fprintf(stdout,"\n");

	int status = posix_spawn(&pid, argv[0], NULL, NULL, (char* const*)argv, environ);

	if(status == 0)
	{
		do
		{
			if(waitpid(pid, &status, 0) == -1)
			{
				exit(1);
			}
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	else
	{
		if(status == 2)//File not found
		{
			RPGSH_OUTPUT(Error,"\"%s\" is not a valid rpgsh command. (%d)",argv[0],status);
		}
		else
		{
			RPGSH_OUTPUT(Error,"%s (%d)",strerror(status),status);
		}
	}

	fprintf(stdout,"\n");
}

int prompt()
{
	bool backup = false;

	prompt:
	c.load(backup);

	try
	{
		fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%hhu/%hhu%s %s(%hhu)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,c.Attr["Name"].c_str(),TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,int(c.Attr["HP"]),int(c.Attr["MaxHP"]),TEXT_NOBOLD,TEXT_ITALIC,int(c.Attr["TempHP"]),TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
		fprintf(stdout,"%s└─%s$%s ",TEXT_WHITE,TEXT_CYAN,TEXT_NORMAL);

		if(backup)
		{
			c.save();
			backup = false;
		}
	}
	catch(...)
	{
		if(!backup)
		{
			RPGSH_OUTPUT(Warning,"Error while displaying prompt. One or more character attributes is not a valid integer. Attempting to load backup.");
			backup = true;
			goto prompt;
		}
		else
		{
			RPGSH_OUTPUT(Error,"Loading from backup was unsuccessful. Manual intervention required to restore file.\n\nGood luck.\n");
			exit(-1);
		}
	}

	char buffer[MAX_BUFFER_SIZE];
	fgets(buffer,sizeof(buffer),stdin);
	buffer[strcspn(buffer,"\n")] = 0; //Omits newline character from input buffer (https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)

	if(strcmp(buffer,""))
	{
		if(!strcmp(buffer,"exit"))
		{
			fprintf(stdout,"Exiting...\n");
			return -1;
		}
		else
		{
			if(buffer[0] == SHELL_VAR || buffer[0] == CHAR_VAR)//Check if user is operating on a variable
			{
				std::string prefix = "variables ";
				prefix += buffer;
				strcpy(buffer,prefix.c_str());
			}
			run_rpgsh_prog(c,buffer);
			return 0;
		}
	}
	return 0;
}
int main()
{
	//Create shell vars file if it doesn't exist
	if(!std::filesystem::exists(shell_vars_path.c_str()))
	{
		std::ofstream ofs(shell_vars_path.c_str());
		ofs.close();
	}

	//Forces default character to be created so first load() works correctly
	RPGSH_CHAR *dummy = new RPGSH_CHAR();
	if(!std::filesystem::exists(dummy->current_char_path.c_str()))
	{
		dummy->save();
		dummy->set_current();
	}
	run_rpgsh_prog(*dummy,(char*)"banner");
	run_rpgsh_prog(*dummy,(char*)"version");
	delete dummy;

	while(prompt() >= 0);

	return 0;
}
