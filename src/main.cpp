#include <climits>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
//#include <sys/ioctl.h>
//#include <termios.h>
#include <unistd.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

Config cfg = Config();
Character c = Character();

std::string getBufferStr()
{
	long unsigned int last_prompt_line_length;

	//Print prompt
	if(c.keyExists<Var>(DOT_PROMPT))
	{
		for(const auto& line : getAppOutput("print -r "+c.getStr<Var>(DOT_PROMPT)).output)
		{
			if(line.length() <= 1) continue;
			last_prompt_line_length = getDisplayLength(stripFormatting(line));
		}
	}
	else
	{
		last_prompt_line_length = defaultPrompt(c).length();
	}

	//Get input
	std::string input;
	inputHandler(&input, last_prompt_line_length);

	return input;
}
int prompt()
{
	#define MAX_BUFFER_SIZE 65535 //Not sure why UINT_MAX segfaults, but hard-coding the value of UINT_MAX works fine

	confirmHistoryFile();

	bool backup = false;

	prompt:
	c = Character();

	try
	{
		if(c.keyExists<Var>(DOT_PROMPT))
		{
			long unsigned int last_prompt_line_length;
			for(const auto& line : getAppOutput("print -r "+c.getStr<Var>(DOT_PROMPT)).output)
			{
				if(line.length() <= 1) continue;
				last_prompt_line_length = getDisplayLength(stripFormatting(line));
				fprintf(stdout,"%s\n",line.c_str());
			}
			fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)0);
			fprintf(stdout,CURSOR_UP);
			fprintf(stdout,CURSOR_RIGHT_N,last_prompt_line_length);
		}
		else
		{
			fprintf(stdout,"%s",defaultPrompt(c).c_str());
		}

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
			output(warning,"Error while displaying prompt. One or more character attributes is not a valid integer. Attempting to load backup.");
			backup = true;
			goto prompt;
		}
		else
		{
			output(error,"Loading from backup was unsuccessful. Manual intervention required to restore file.\n\nGood luck.\n");
			exit(-1);
		}
	}

	char buffer[MAX_BUFFER_SIZE];

	//Zero-out buffer so we have a known dataset
	for(unsigned int i=0; i<MAX_BUFFER_SIZE; i++)
		buffer[i] = '\0';

	std::string in = getBufferStr();
	if(in.length() > MAX_BUFFER_SIZE)
	{
		fprintf(stdout,"\n\n");
		output(error,"Input too big (%llu characters)! Please enter <= %lu characters.",in.length(),MAX_BUFFER_SIZE);
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

		//Auto-escape colons so as to not cause issues with variable sets
		std::regex colon(":");
		std::string tmp_buffer = std::regex_replace(std::string(buffer),colon,"\\:");
		for(unsigned int i=0; i<tmp_buffer.length(); i++)
			buffer[i]=tmp_buffer[i];

		for(const auto& app : split(cfg.setting[PRE_RUN_APPS],','))
			if(app.length()) (void)runApp(app,false);
		padding();
		(void)runApp(buffer,false);
		for(const auto& app : split(cfg.setting[POST_RUN_APPS],','))
			if(app.length()) (void)runApp(app,false);
		padding();

		//Handle rpgsh history
		long unsigned line_count = 0;
		std::ifstream ifs(history_path);
		std::vector<std::string> history;
		std::string data;
		while(!ifs.eof())
		{
			std::getline(ifs,data);
			if(ifs.eof()) break;
			history.push_back(data);
			line_count++;
		}
		ifs.close();

		history.push_back(buffer);
		long unsigned history_len = 0;
		try
		{
			history_len = std::stol(cfg.setting[HISTORY_LENGTH]);
		}
		catch(...)
		{
			output(error,"Invalid value for \"%s\" setting: %s\n",HISTORY_LENGTH,cfg.setting[HISTORY_LENGTH].c_str());
			exit(-1);
		}

		if(line_count >= history_len)
		{
			line_count = line_count-history_len+1;
			std::ofstream ofs(history_path);
			for(long unsigned i=line_count; i<=history_len; i++)
				ofs<<history[i]+"\n";
			ofs.close();
		}
		else
		{
			std::ofstream ofs(history_path);
			for(long unsigned i=0; i<history.size(); i++)
				ofs<<history[i]+"\n";
			ofs.close();
		}
	}
	return 0;
}
int main(int argc, char** argv)
{
	//Generate cache of valid rpgsh programs to speed up "help" program
	//It takes a noticable amount of time to search through all of /bin even on an i7-4700k
	fprintf(stdout,"Generating rpgsh program list...");
	std::vector<std::string> applications = getDirectoryListing(std::string(RPGSH_INSTALL_DIR));
	std::vector<std::string> rpgsh_apps;
	std::filesystem::remove(rpgsh_programs_cache_path);
	for(const auto& app : applications)
		if(!findu(app,prefix)) rpgsh_apps.push_back(app);

	//Basic alphabetical sort, important for tab completion
	sort<std::string>(&rpgsh_apps);

	//Write alphabetized list to file
	std::ofstream ofs(rpgsh_programs_cache_path);
	for(const auto& app : rpgsh_apps)
		ofs<<app<<"\n";
	ofs.close();

	fprintf(stdout,CLEAR_ENTIRE_LINE);//Delete "Generating..." line from start of main()

	//Handle arguments
	if(argc > 3) output(warning,"rpgsh expects 0, 1, or 2 arguments. Ignoring everything past \"%s\".",argv[2]);
	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\trpgsh [%sOPTIONS%s]\n\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"OPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tStarts rpgsh normally.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-c %scommand%s\tExecutes rpgsh command %scommand%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-s %spath%s\t\tExecutes rpgsh script at path %spath%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}
	else if(argv[1] && strcmp(argv[1],"-c") && strcmp(argv[1],"-s"))
	{
		output(error,"Unknown option \"%s\".",argv[1]);
		return -1;
	}

	//Handle commands
	if(argc > 2 && !strcmp(argv[1],"-c"))
	{
		runApp(std::string(argv[2]),false);
		return 0;
	}
	else if(argv[1] && !strcmp(argv[1],"-c"))
	{
		output(error,"No rpgsh command specified.");
		return -1;
	}

	//Handle scripts
	if(argc > 2 && !strcmp(argv[1],"-s") && std::filesystem::exists(argv[2]))
	{
		std::string cmd;
		std::ifstream ifs(argv[2]);
		while(!ifs.eof())
		{
			getline(ifs,cmd);
			if(cmd != "" && (cmd.length() < 3 || (cmd.length() >= 3 && left(cmd,3) != "#!/")))
				runApp(cmd,false);
		}
		return 0;
	}
	else if(argc > 2 && !strcmp(argv[1],"-s"))
	{
		output(error,"rpgsh script \"%s\" does not exist.");
		return -1;
	}
	else if(argv[1] && !strcmp(argv[1],"-s"))
	{
		output(error,"No rpgsh script specified.");
		return -1;
	}

	//Normal operation (no args)
	for(const auto& app : split(cfg.setting[STARTUP_APPS],','))
		if(app.length()) (void)runApp(app,false);

	if(!stob(cfg.setting[HIDE_TIPS]))
	{
		fprintf(stdout,"\n");
		fprintf(stdout,"%s%sTIP: %sType %s%shelp%s for a list of currently supported commands.\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
		fprintf(stdout,"%s%sTIP: %sFor more info about a given command, try using the %s%s-?%s or %s%s--help%s flags.\n\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
	}
	else fprintf(stdout,"\n");

	while(!prompt());

	return 0;
}
