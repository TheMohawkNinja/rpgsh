#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <termios.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

#define MAX_BUFFER_SIZE 256

Config config = Config();
Character c = Character(false);

std::string input_handler()
{
	#define KB_TAB		9
	#define KB_ENTER	10
	#define KB_BACKSPACE	127

	//Set terminal flags for non-buffered reading required for handling keyboard input
	struct termios t_old, t_new;
	tcgetattr(fileno(stdin), &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(fileno(stdin), TCSANOW, &t_new);

	bool insert_mode = false;
	char k = 0;
	int cur_pos = 0;
	int tab_ctr = 0;
	std::string last_match = "";
	std::vector<char> input;

	while(k != KB_ENTER)
	{
		k = getchar();

		//Reset tab completion variables
		if(k != KB_TAB)
		{
			tab_ctr = 0;
			last_match = "";
		}

		if(isprint(k))//Printable characters
		{
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < input.size())
					input[cur_pos] = k;
				else
					input.push_back(k);
			}
			else
			{
				input.insert(input.begin()+cur_pos,k);
			}

			fprintf(stdout,CLEAR_LINE);

			for(int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);
			if(cur_pos < input.size()-1)
				fprintf(stdout,CURSOR_LEFT_N,input.size()-1-cur_pos);

			cur_pos++;
		}
		else if(k == KB_BACKSPACE && cur_pos > 0)//Backspace
		{
			fprintf(stdout,"\b%s",CLEAR_TO_LINE_END);

			cur_pos--;
			input.erase(input.begin()+cur_pos);

			for(int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);
			if(cur_pos < input.size())
				fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);
		}
		else if(k == KB_TAB && input.size())//Tab (completion)
		{
			tab_ctr++;

			std::string match = "";
			std::vector<std::string> matches;

			//TODO: Tab completion
			//Create the string which is to be completed
			std::string chk_str = "";
			for(int i=cur_pos-1; i>=0&&input[i]!=' '; i--)
				chk_str.insert(0,std::string(1,input[i]));

			if(isalpha(chk_str[0]))//Applications
			{
				//Get matches
				std::string app = "";
				std::ifstream ifs(rpgsh_programs_cache_path);
				while(!ifs.eof())
				{
					getline(ifs,app);
					if(app == "") break;

					std::string app_chk_str = app.substr(prefix.length(),
									     chk_str.length());
					if(app.length() > prefix.length() && app_chk_str == chk_str)
					{
						matches.push_back(app.substr(prefix.length(),
									     match.length()-prefix.length()));
					}
				}
				ifs.close();
			}
			else if(chk_str[0] == CHARACTER_SIGIL)//Character variables
			{
				std::string xref_path = campaigns_dir+
							get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
							"characters/";
				if(chk_str.length() == 1)
				{
					matches.push_back(std::string(1,CHARACTER_SIGIL)+"/");
				}
				else if(chk_str.length() == 2 && chk_str[1] == '[')
				{
					matches = getDirectoryListing(xref_path);
					for(auto& m : matches)
						m = "  "+left(m,m.find(".char"));
				}
			}

			if(!matches.size()) continue;

			match = matches[(tab_ctr-1)%matches.size()];

			//Erase any previous match
			if(last_match != "")
			{
				for(int i=0; i<last_match.length()-chk_str.length(); i++)
					input.erase(input.begin()+cur_pos);
			}

			//Insert match into input
			for(int i=0; i<match.length()-chk_str.length(); i++)
				input.insert(input.begin()+cur_pos+i,match[i+chk_str.length()]);

			//Reprint input
			fprintf(stdout,CLEAR_TO_LINE_END);
			for(int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);

			fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);

			last_match = match;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			getchar();//skip '['
			switch(getchar())
			{
				case 'C':	//Right
					if(cur_pos < input.size())
					{
						fprintf(stdout,CURSOR_RIGHT);
						cur_pos++;
					}
					break;
				case 'D':	//Left
					if(cur_pos > 0)
					{
						fprintf(stdout,CURSOR_LEFT);
						cur_pos--;
					}
					break;
				case 'H':	//Home
					if(cur_pos > 0)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					break;
				case '7':	//Home
					if(getchar() == '~' && cur_pos > 0)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					break;
				case 'F':	//End
					if(cur_pos < input.size())
					{
						fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos);
						cur_pos = input.size();
					}
					break;
				case '8':	//End
					if(getchar() == '~' && cur_pos < input.size())
					{
						fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos);
						cur_pos = input.size();
					}
					break;
				case '2':	//Insert
					if(getchar() == '~')
						insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() == '~' && cur_pos < input.size())
					{
						fprintf(stdout,CLEAR_LINE);
						input.erase(input.begin()+cur_pos);

						for(int i=cur_pos; i<input.size(); i++)
							fprintf(stdout,"%c",input[i]);
						if(cur_pos < input.size())
							fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);
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
	c.load();

	try
	{
		fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%s/%s%s %s(%s)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,c.getName().c_str(),TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,c.getProperty<Var>("HP/Current","Value").c_str(),c.getProperty<Var>("HP/Max","Value").c_str(),TEXT_NOBOLD,TEXT_ITALIC,c.getProperty<Var>("HP/Temp","Value").c_str(),TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
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
		buffer[i] = '\0';

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
		else if(buffer[0] != COMMENT)
		{
			run_rpgsh_prog(buffer,false);
		}
	}
	return 0;
}
int main()
{
	//Generate cache of valid rpgsh programs to speed up "help" program
	//It takes a noticable amount of time to search through all of /bin even on an i7-4700k
	fprintf(stdout,"Generating rpgsh program list...");
	std::vector<std::string> applications = getDirectoryListing("/bin");
	std::vector<std::string> rpgsh_apps;
	std::filesystem::remove(rpgsh_programs_cache_path);
	for(const auto& app : applications)
	{
		if(!app.find(prefix))
			rpgsh_apps.push_back(app);
	}

	//Basic alphabetical sort, important for tab completion
	for(int i=0; i<rpgsh_apps.size()-1; i++)
	{
		for(int j=i+1; j<rpgsh_apps.size(); j++)
		{
			if(rpgsh_apps[j] < rpgsh_apps[i])
			{
				std::string tmp = rpgsh_apps[i];
				rpgsh_apps[i]=rpgsh_apps[j];
				rpgsh_apps[j]=tmp;
			}
		}
	}

	//Write alphabetized list to file
	std::ofstream ofs(rpgsh_programs_cache_path);
	for(const auto& app : rpgsh_apps)
		ofs<<app<<"\n";
	ofs.close();

	fprintf(stdout,"\e[2K");//Delete "Generating..." line from start of main()

	//Generate default character if needed
	if(!c.confirmDatasource())
	{
		c.setDatasource(templates_dir + config.setting[DEFAULT_GAME].c_str());
		c.load();
		c.setDatasource(c.getCurrentCharacterFilePath());
		c.save();
	}

	run_rpgsh_prog((char*)"banner",false);
	run_rpgsh_prog((char*)"version",false);

	if(!stob(config.setting[HIDE_TIPS]))
	{
		fprintf(stdout,"%s%sTIP: %sType %s%shelp%s for a list of currently supported commands.\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
		fprintf(stdout,"%s%sTIP: %sFor more info about a given command, try using the %s%s-?%s or %s%s--help%s flags.\n\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
	}

	while(prompt() == 0);

	return 0;
}
