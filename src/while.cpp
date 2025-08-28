#include <cstring>
#include <regex>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(error,"while expects at least 1 argument.");
		return -1;
	}

	chkFlagAppDesc(argv,"Evaluates a condition, executing subsequent commands repeatedly as long as the condition is true.");
	chkFlagPreserveVariables(argv,all);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\twhile %scondition%s { %scommand%s; }\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\twhile [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\n");
		fprintf(stdout,"\t%scondition%s\tA condition (as evaluated by the \"eval\" command), that if true, will execute the commands between the \'{\' and \'}\'.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%scommand%s\t\tAn rpgsh command, to be executed if %scondition%s is true. There can be an arbitrary number of commands, each one terminated with a semicolon (;).\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	bool found_start = false;
	bool found_end = false;
	std::string condition, commands_str;
	std::vector<std::string> commands;
	for(int i=1; i<argc; i++)
	{
		if(!found_start && strcmp(argv[i],"{"))
		{
			condition += std::string(argv[i])+" ";
		}
		else if(!found_start)
		{
			found_start = true;
			condition.erase(condition.end()-1);
		}
		else
		{
			for(int j=argc-1; j>=i; j--)
			{
				std::string j_arg = std::string(argv[j]);
				if(found_end)
					commands_str = j_arg+" "+commands_str;
				else if(!found_end && j_arg.back() == '}')
					found_end = true;
			}
			break;
		}
	}

	commands = split(commands_str,';');

	for(unsigned i=0; i<commands.size(); i++)
	{
		if(countu(commands[i],'{') > countu(commands[i],'}'))
		{
			if(i == commands.size()-1)
			{
				output(error,"\t\'}\' not found.");
				return -1;
			}
			else if(findu(commands[i+1],'}') != std::string::npos)
			{
				commands[i] += left(commands[i+1],findu(commands[i+1],'}')+1);
				commands[i+1] = right(commands[i+1],findu(commands[i+1],'}')+1);
				i=0;
			}
			else
			{
				commands[i] += ";"+commands[i+1]+";";
				commands.erase(commands.begin()+i+1);
				i=0;
			}
		}
	}

	//Cleaup with regex
	for(auto& command : commands)
	{
		command = std::regex_replace(command,std::regex(";{2,}"),";");
		command = std::regex_replace(command,std::regex("};"),"}");
		if(findu(command,'{') != std::string::npos)
			command = std::regex_replace(command,std::regex("\\\\"),"\\\\");
	}

	if(!found_start)
	{
		output(error,"No commands specified.");
		return -1;
	}
	if(!found_end)
	{
		output(error,"End of commands not found.");
		return -1;
	}

	while(stob(getAppOutput("eval "+condition).output[0]))
	{
		for(auto& command : commands)
		{
			int status = runApp(handleBackslashEscSeqs(command),false);
			if(status == STATUS_BREAK) return 0;
			if(status == STATUS_CONTINUE) break;;
		}
	}

	return 0;
}
