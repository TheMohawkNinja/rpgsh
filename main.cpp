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

void run_rpgsh_prog(RPGSH_CHAR c, std::string args)
{
	std::vector<std::string> v;
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
			int find_percent = args.find("%",params_start);
			std::string var = "";
			if(args.substr(i,1) == "%")
			{
				for(int j=i+1; j<args.length() && args.substr(j,1) != " "; j++)
				{
					var+=args.substr(j,1);
				}
				fprintf(stdout,"var = \'%s\'\n",var.c_str());
				int new_args_start = find_percent + var.length() + 1;
				args = args.substr(0,find_percent)+c.Attr[var].c_str()+
					args.substr(new_args_start,args.length()-(args.substr(0,new_args_start).length()));
			}
		}
	}
	if(args.find(" ") != std::string::npos)
	{
		v.push_back(path+prefix+args.substr(0,args.find(" ")));
		args = args.substr(args.find(" ")+1,(args.length() - args.find(" ")+1));
		v.push_back(std::string(c.Attr["Name"]));

		for(int i=0; i<args.length(); i++)
		{
			if(args.substr(i,1) == "\"")//Combine args wrapped in quotes
			{
				fprintf(stdout,"Found quote-wrapped string\n");
				if(args.find("\"",1) == std::string::npos)
				{
					RPGSH_OUTPUT(Error,"Unmatched quote in argument list.");
					return;
				}
				v.push_back(args.substr(i+1,args.find("\"",i+1)-1));
				fprintf(stdout,"i = %d\n",i);
				i+=v[v.size()-1].length();
				fprintf(stdout,"i = %d\n",i);
			}
			else
			{
				if(args.find(" ") != std::string::npos)
				{
					if(args.find(" ",args.find(" ")+1) != std::string::npos)
					{
						v.push_back(args.substr(i,
									args.find(" ",
										(args.find(" ")+i+1)) - (args.find(" ",i))));
					}
					else
					{
						v.push_back(args.substr(i,args.length()-args.find(" ",i)));
					}
					fprintf(stdout,"i = %d\n",i);
					i+=v[v.size()-1].length();
					fprintf(stdout,"i = %d\n",i);
				}
				else
				{
					v.push_back(args.substr(i,args.length()-i));
					break;
				}
			}
		}
	}
	else//If only one arg is called
	{
		v.push_back(path+prefix+args);
		v.push_back(std::string(c.Attr["Name"]));
	}

	char* argv[v.size()+1];
	for(int i=0; i<v.size(); i++)
	{
		argv[i] = (char*)v[i].c_str();
		fprintf(stdout,"argv[%d] = \'%s\'\n",i,argv[i]);
	}
	argv[v.size()] = NULL;

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
			if(buffer[0] == '$'|| buffer[0] == '%')//Check if user is operating on a variable
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
