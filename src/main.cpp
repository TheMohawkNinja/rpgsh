#include <spawn.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../headers/char.h"
#include "../headers/config.h"
#include "../headers/functions.h"

#define MAX_BUFFER_SIZE 256

RPGSH_CONFIG config = RPGSH_CONFIG();
RPGSH_CHAR c = RPGSH_CHAR();

void padding()
{
	//Pad output if set
	try
	{
		if(stob(config.setting[PADDING]))
		{
			fprintf(stdout,"\n");
		}
	}
	catch(...)
	{
		output(Error,"Invalid value \'%s\' for \'%s\'.",config.setting[PADDING].c_str(),PADDING);
		exit(-1);
	}
}
void run_rpgsh_prog(std::string args)
{
	std::vector<std::string> vars;
	extern char** environ;
	pid_t pid;

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

				std::ifstream ifs(shell_vars_file.c_str());
				while(!ifs.eof())
				{
					std::string old;
					std::string data = "";
					std::getline(ifs,data);

					if(data.substr(0,data.find(DS)) == var)
					{
						int new_args_start = find_dollar + var.length() + 1;
						old = data.substr(data.find(DS)+DS.length(),
								data.length()-(data.find(DS)+DS.length()));

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

		for(int i=0; i<args.length(); i++)
		{
			if(args.substr(i,1) == "\"")//Combine args wrapped in quotes
			{
				if(args.find("\"",i+1) == std::string::npos)
				{
					output(Error,"Unmatched quote in argument list.");
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
	}

	//Convert vector to char*[]
	char* argv[vars.size()+1];
	for(int i=0; i<vars.size(); i++)
	{
		argv[i] = (char*)vars[i].c_str();
	}

	//Add a NULL because posix_spawn() needs that for some reason
	argv[vars.size()] = NULL;

	padding();

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
			std::string displayed_command = std::string(argv[0]).substr(prefix.length()+2,std::string(argv[0]).length()-prefix.length()-2);
			output(Error,"\"%s\" is not a valid rpgsh command. (%d)",displayed_command.c_str(),status);
		}
		else
		{
			output(Error,"%s (%d)",strerror(status),status);
		}
	}
	padding();
}
std::string input_handler()
{
	#define ESC_SEQ		'\033'
	#define KB_ENTER	10
	#define KB_BACKSPACE	127

	//Set terminal flags for non-buffered reading required for handling keyboard input
	struct termios t_old, t_new;
	tcgetattr(fileno(stdin), &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(fileno(stdin), TCSANOW, &t_new);

	std::vector<char> input;
	char c = 0;
	int cur_pos = 0;
	bool insert_mode = false;

	while(c != KB_ENTER)
	{
		c = getchar();

		if(isprint(c))
		{
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < input.size())
				{
					input[cur_pos] = c;
				}
				else
				{
					input.push_back(c);
				}
			}
			else
			{
				input.insert(input.begin()+cur_pos,c);
			}
			fprintf(stdout,"\e[K");
			for(int i=cur_pos; i<input.size(); i++)
			{
				fprintf(stdout,"%c",input[i]);
			}
			if(cur_pos < input.size()-1)
			{
				fprintf(stdout,"\e[%dD",input.size()-1-cur_pos);
			}
			cur_pos++;
		}
		else if(c == KB_BACKSPACE && cur_pos > 0)
		{
			fprintf(stdout,"\b\e[K");// \b backs up one character and \e[K deletes everything to EOL
			cur_pos--;
			input.erase(input.begin()+cur_pos);
			for(int i=cur_pos; i<input.size(); i++)
			{
				fprintf(stdout,"%c",input[i]);
			}
			if(cur_pos < input.size())
			{
				fprintf(stdout,"\e[%dD",input.size()-cur_pos);
			}
		}
		else if(c == ESC_SEQ)//Escape sequences
		{
			getchar();//skip '['
			switch(getchar())
			{
				case 'C':	//Right
					if(cur_pos < input.size())
					{
						fprintf(stdout,"\e[C");
						cur_pos++;
					}
					break;
				case 'D':	//Left
					if(cur_pos > 0)
					{
						fprintf(stdout,"\e[D");
						cur_pos--;
					}
					break;
				case 'H':	//Home
					if(cur_pos > 0)
					{
						fprintf(stdout,"\e[%dD",cur_pos);
						cur_pos = 0;
					}
					break;
				case 'F':	//End
					if(cur_pos < input.size())
					{
						fprintf(stdout,"\e[%dC",input.size()-cur_pos);
						cur_pos = input.size();
					}
					break;
				case '2':
					if(getchar() == '~')	//Insert
					{
						insert_mode = !insert_mode;
					}
					break;
				case '3':
					if(getchar() == '~')	//Delete
					{
						if(cur_pos < input.size())
						{
							fprintf(stdout,"\e[K");// \b backs up one character and \e[K deletes everything to EOL
							input.erase(input.begin()+cur_pos);
							for(int i=cur_pos; i<input.size(); i++)
							{
								fprintf(stdout,"%c",input[i]);
							}
							if(cur_pos < input.size())
							{
								fprintf(stdout,"\e[%dD",input.size()-cur_pos);
							}
						}
					}
					break;
			}
		}
	}

	//Reset terminal flags in-case of sudden program termination
	tcsetattr(fileno(stdin), TCSANOW, &t_old);

	//Strings need to be null-terminated
	input.push_back('\0');
	return input.data();
}
int prompt()
{
	bool backup = false;

	prompt:
	c.load(get_shell_var(CURRENT_CHAR_SHELL_VAR),backup);

	try
	{
		fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%hhu/%hhu%s %s(%hhu)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,c.Name().c_str(),TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,int(c.Attr["HP"]),int(c.Attr["MaxHP"]),TEXT_NOBOLD,TEXT_ITALIC,int(c.Attr["TempHP"]),TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
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
			output(Warning,"Error while displaying prompt. One or more character attributes is not a valid integer. Attempting to load backup.");
			backup = true;
			goto prompt;
		}
		else
		{
			output(Error,"Loading from backup was unsuccessful. Manual intervention required to restore file.\n\nGood luck.\n");
			exit(-1);
		}
	}

	char buffer[MAX_BUFFER_SIZE];

	//Zero-out buffer so we have a known dataset
	for(int i=0; i<MAX_BUFFER_SIZE; i++)
	{
		buffer[i] = '\0';
	}

	std::string in = input_handler();
	if(in.length())
	{
		strncpy(buffer,in.c_str(),in.length()+1);
	}

	if(strcmp(buffer,""))
	{
		if(!strcmp(buffer,"exit"))
		{
			fprintf(stdout,"\nExiting...\n");
			return 1; //Non-zero so we can exit, and positive so user can discriminate between good exits and bad exits
		}
		else
		{
			if(buffer[0] == SHELL_VAR || buffer[0] == CHAR_VAR)//Check if user is operating on a variable
			{
				std::string prefix = "variables ";
				prefix += buffer;
				strcpy(buffer,prefix.c_str());
			}
			fprintf(stdout,"\n");
			run_rpgsh_prog(buffer);
			return 0;
		}
	}
	else
	{
		fprintf(stdout,"\n");
	}
	return 0;
}
int main()
{
	//Create shell vars file if it doesn't exist
	if(!std::filesystem::exists(shell_vars_file.c_str()))
	{
		output(Info,"Shell variable storage file not found, creating file at \'%s\'.",shell_vars_file.c_str());
		std::ofstream ofs(shell_vars_file.c_str());
		ofs.close();

		//Set default values for built-in shell variables
		std::map<std::string, RPGSH_VAR> Attr = load_map_from_file<RPGSH_VAR>(templates_dir+config.setting[DEFAULT_GAME],c.AttributeDesignator);
		set_shell_var(CURRENT_CHAR_SHELL_VAR,std::string(Attr["Name"]));
	}

	//(re)build default character on launch
	c.save();

	run_rpgsh_prog((char*)"banner");
	run_rpgsh_prog((char*)"version");

	while(prompt() == 0);

	return 0;
}
