#include <cstring>
#include <iostream>
#include "../headers/functions.h"

void confirmPath(std::string path, std::string scope)
{
	if(!std::filesystem::exists(path))
	{
		std::ofstream ofs(path);
		ofs.close();
		output(info,"Created %s autorun file at \"%s\"",scope.c_str(),path.c_str());
		return;
	}
}
void execAutorun(std::string path, std::string scope, bool verbose)
{
	std::ifstream ifs(path);
	while(!ifs.eof())
	{
		std::string command;
		getline(ifs,command);
		if(!command.length() || command[0] == COMMENT) continue;
		if(findu(command,COMMENT)) command = left(command,findu(command,COMMENT));
		bool echo = stob(left(command,findu(command,',')-1));
		if(verbose)
			output(info,"Running %s autorun command \"%s\"",scope.c_str(),command.c_str());
		runApp(right(command,findu(command,',')+1),!echo);
	}
	ifs.close();
}
void saveAutorun(std::string path, std::string scope)
{
	while(true)
	{
		std::string command, echo;
		fprintf(stdout,"Please enter a command: ");
		getline(std::cin,command);
		if(!command.length()) continue;
		do
		{
			fprintf(stdout,"Echo output when running command? [Y/n]: ");
			getline(std::cin,echo);
		}while(stringcasecmp(echo,"y") && stringcasecmp(echo,"n"));

		echo = (!stringcasecmp(echo,"n") ? "true," : "false,");
		std::ofstream ofs(path,std::ios::app);
		ofs<<echo+command+"\n";
		output(info,"Added \"%s%s\" to %s autorun file",echo.c_str(),command.c_str(),scope.c_str());
		ofs.close();
		return;
	}
}
void printCommands(std::string path, bool print_index, int* cmd_ctr)
{
	std::ifstream ifs(path);
	while(true)
	{
		std::string command;
		getline(ifs,command);
		if(ifs.eof())
		{
			fprintf(stdout,"\n");
			return;
		}
		if(!command.length()) continue;
		(*cmd_ctr)++;
		std::string index_str = std::string(TEXT_BOLD)+std::string(TEXT_WHITE)+"["+std::string(TEXT_GREEN)+std::to_string(*cmd_ctr)+std::string(TEXT_WHITE)+"]"+std::string(TEXT_NORMAL);
		fprintf(stdout,"%s%s\n",(print_index ? index_str+" " : "").c_str(),command.c_str());
	}
	ifs.close();
}
bool removeCommand(std::string path, int* index, std::string scope)
{
	bool removingCommand = false;
	std::ifstream ifs(path);
	std::string deletedCommand;
	std::vector<std::string> commands;
	while(true)
	{
		std::string command;
		getline(ifs,command);
		if(ifs.eof()) break;
		if(!command.length()) continue;
		if((*index) != 1)
		{
			commands.push_back(command);
			(*index)--;
		}
		else
		{
			removingCommand = true;
			deletedCommand = command;
			break;
		}

	}
	ifs.close();
	if(removingCommand)
	{
		std::filesystem::remove(path);
		std::ofstream ofs(path,std::ios::app);
		for(const auto& command : commands)
			ofs<<command+"\n";
		ofs.close();
		output(info,"Removed \"%s\" from %s autorun file",deletedCommand.c_str(),scope.c_str());
	}

	return removingCommand;
}
int main(int argc, char** argv)
{
	if(argc > 2)
		output(warning,"autorun only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Sets and executes commands designated by the user on a per-scope basis.");
	chkFlagModifyVariables(argv,false);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tautorun [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tExecute all saved commands.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-c\t\tCreate autorun command for current character.\n");
		fprintf(stdout,"\t-m\t\tCreate autorun command for current campaign.\n");
		fprintf(stdout,"\t-s\t\tCreate autorun command for shell scope.\n");
		fprintf(stdout,"\t-l\t\tList currently saved commands in all scopes.\n");
		fprintf(stdout,"\t-r\t\tRemove one or more commands.\n");
		fprintf(stdout,"\t-v\t\tSame as %snone%s, but outputs what command is ran from which scope.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	Character c = Character();
	Campaign m = Campaign();
	std::string s_path = root_dir+autorun_ext;
	std::string m_path = left(m.getDatasource(),rfindu(m.getDatasource(),variables_file_name))+autorun_ext;
	std::string c_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1)+c.getName()+autorun_ext;

	confirmPath(s_path,"shell");
	confirmPath(m_path,"campaign");
	confirmPath(c_path,"character");

	if(argc == 1 || !strcasecmp(argv[1],"-v"))
	{
		bool verbose = false;
		if(argv[1] && !strcasecmp(argv[1],"-v"))
			verbose = true;
		execAutorun(s_path,"shell",verbose);
		execAutorun(m_path,"camapign",verbose);
		execAutorun(c_path,"character",verbose);
	}
	else if(!strcasecmp(argv[1],"-c"))
	{
		saveAutorun(c_path,"character");
	}
	else if(!strcasecmp(argv[1],"-m"))
	{
		saveAutorun(m_path,"campaign");
	}
	else if(!strcasecmp(argv[1],"-s"))
	{
		saveAutorun(s_path,"shell");
	}
	else if(!strcasecmp(argv[1],"-l"))
	{
		int cmd_ctr = 0;
		std::string m_name = getEnvVariable(ENV_CURRENT_CAMPAIGN);
		printHeader("("+std::string(1,SHELL_SIGIL)+") "+"Shell");
		printCommands(s_path,false,&cmd_ctr);
		printHeader("("+std::string(1,CAMPAIGN_SIGIL)+") "+left(m_name,m_name.length()-1));
		printCommands(m_path,false,&cmd_ctr);
		printHeader("("+std::string(1,CHARACTER_SIGIL)+") "+c.getName());
		printCommands(c_path,false,&cmd_ctr);
	}
	else if(!strcasecmp(argv[1],"-r"))
	{
		int cmd_ctr = 0;
		std::string m_name = getEnvVariable(ENV_CURRENT_CAMPAIGN);
		printHeader("("+std::string(1,SHELL_SIGIL)+") "+"Shell");
		printCommands(s_path,true,&cmd_ctr);
		printHeader("("+std::string(1,CAMPAIGN_SIGIL)+") "+left(m_name,m_name.length()-1));
		printCommands(m_path,true,&cmd_ctr);
		printHeader("("+std::string(1,CHARACTER_SIGIL)+") "+c.getName());
		printCommands(c_path,true,&cmd_ctr);
		fprintf(stdout,"\b");

		int index = 0;
		std::string index_str;

		while(true)
		{
			fprintf(stdout,"\nPlease enter the number associated with the command you wish to delete: ");
			getline(std::cin,index_str);
			try
			{
				index = std::stoi(index_str);
				break;
			}
			catch(...){}
		}

		if(!removeCommand(s_path,&index,"shell") &&
		   !removeCommand(m_path,&index,"campaign") &&
		   !removeCommand(c_path,&index,"character"))
		{
			output(error,"No command exists at position %s",index_str.c_str());
			return -1;
		}
	}
	else
	{
		output(error,"Unknown option \"%s\"",argv[1]);
		return -1;
	}

	return 0;
}
