#include <cstring>
#include <filesystem>
#include <fstream>
#include <termios.h>
#include "../headers/char.h"
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/text.h"

#define MAX_BUFFER_SIZE 256

RPGSH_CONFIG config = RPGSH_CONFIG();
RPGSH_CHAR c = RPGSH_CHAR();

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
				case '7':	//Home
					if(getchar() == '~' && cur_pos > 0)
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
				case '8':	//End
					if(getchar() == '~' && cur_pos < input.size())
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
		fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%hhu/%hhu%s %s(%hhu)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,c.Name().c_str(),TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,int(c.Attr["/HP/Current"]),int(c.Attr["/HP/Max"]),TEXT_NOBOLD,TEXT_ITALIC,int(c.Attr["/HP/Temp"]),TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
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
	if(in.length() > MAX_BUFFER_SIZE)
	{
		fprintf(stdout,"\n\n");
		output(Error,"Input too big (%llu characters)! Please enter <= %d characters.",in.length(),MAX_BUFFER_SIZE);
		strcpy(buffer,"");
	}
	else
	{
		strncpy(buffer,in.c_str(),in.length()+1);
	}

	fprintf(stdout,"\n");
	if(strcmp(buffer,""))
	{
		if(!strcmp(buffer,"exit"))
		{
			fprintf(stdout,"Exiting...\n");
			return 1; //Non-zero so we can exit, and positive so user can discriminate between good exits and bad exits
		}
		else
		{
			run_rpgsh_prog(buffer);
			return 0;
		}
	}
	return 0;
}
int main()
{
	//Create shell vars file if it doesn't exist
	if(!std::filesystem::exists(shell_vars_file.c_str()))
	{
		output(Info,"Shell variable file not found, creating file at \'%s\'.",shell_vars_file.c_str());
		std::ofstream ofs(shell_vars_file.c_str());
		ofs.close();

		//Set default values for built-in shell variables
		RPGSH_OBJ Attr = load_obj_from_file<RPGSH_VAR>(templates_dir+config.setting[DEFAULT_GAME],c.AttributeDesignator);
		set_shell_var(CURRENT_CHAR_SHELL_VAR,c.Name());
		set_shell_var(CURRENT_CAMPAIGN_SHELL_VAR,"default/");
	}

	//(re)build default character on launch
	c.save();

	run_rpgsh_prog((char*)"banner");
	run_rpgsh_prog((char*)"version");

	while(prompt() == 0);

	return 0;
}
