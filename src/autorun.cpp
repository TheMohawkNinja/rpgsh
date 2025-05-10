#include <cstring>
#include <iostream>
#include "../headers/functions.h"

void confirmPath(std::string path, std::string scope)
{
	if(!std::filesystem::exists(path))
	{
		std::ofstream ofs(path);
		ofs.close();
		output(Info,"Created %s autorun file at \"%s\"",scope.c_str(),path.c_str());
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
		if(command == "") continue;
		if(verbose)
			output(Info,"Running %s autorun command \"%s\"",scope.c_str(),command.c_str());
		runApp(command,false);
	}
	ifs.close();
}
void saveAutorun(std::string path, std::string scope)
{
	while(true)
	{
		std::string command;
		fprintf(stdout,"Please enter a command: ");
		getline(std::cin,command);
		if(command == "") continue;

		std::ofstream ofs(path);
		ofs<<command+"\n";
		output(Info,"Added \"%s\" to %s autorun file",command.c_str(),scope.c_str());
		return;
	}
}
void printHeader(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(long unsigned int i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
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
		(*cmd_ctr)++;
		fprintf(stdout,"%s%s\n",(print_index ? "["+std::to_string(*cmd_ctr)+"] " : "").c_str(),command.c_str());
	}
}
bool removeCommand(std::string path, int* index, std::string scope)
{
	bool thisFile = false;
	std::ifstream ifs(path);
	std::string deletedCommand;
	std::vector<std::string> commands;
	while(true)
	{
		std::string command;
		getline(ifs,command);
		if(ifs.eof()) break;
		if((*index) != 1)
		{
			commands.push_back(command);
			(*index)--;
		}
		else
		{
			thisFile = true;
			deletedCommand = command;
		}

	}
	ifs.close();
	if(thisFile)
	{
		std::filesystem::remove(path);
		std::ofstream ofs(path);
		for(const auto& command : commands)
			ofs<<command+"\n";
		ofs.close();
		output(Info,"Removed \"%s\" from %s autorun file",deletedCommand.c_str(),scope.c_str());
	}

	return thisFile;
}
int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"autorun only expects 0 or 1 arguments, ignoring all other arguments.");

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
	std::string s_path = root_dir+".autorun";
	std::string m_path = left(m.getDatasource(),rfindu(m.getDatasource(),".variables"))+".autorun";
	std::string c_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1)+c.getName()+".autorun";

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
		fprintf(stdout,"\b");
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
			output(Error,"No command exists at position %s",index_str.c_str());
			return -1;
		}
	}
	else
	{
		output(Error,"Unknown option \"%s\"",argv[1]);
		return -1;
	}

	return 0;
}
