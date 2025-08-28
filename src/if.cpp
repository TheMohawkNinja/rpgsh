#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(error,"if expects at least 1 argument.");
		return -1;
	}

	chkFlagAppDesc(argv,"Evaluates a condition, executing subsequent commands if the condition is true.");
	chkFlagPreserveVariables(argv,all);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tif %scondition%s { %scommand%s; }\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tif [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
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
		if(!found_start && (strcmp(argv[i],"{") && strcmp(argv[i],"{;")))
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
				if(!found_end)
				{
					if(strcmp(argv[j],"}") && strcmp(argv[j],"};")) continue;
					found_end = true;
				}
				commands_str = std::string(argv[j])+" "+commands_str;
			}
			break;
		}
	}

	fprintf(stdout,"command_str: \"%s\"\n",commands_str.c_str());
	commands = split(commands_str,';');
	commands.erase(commands.end()-1);
	bool nested = false;
	unsigned nested_level = 0;
	for(auto& command : commands)
	{
		if(left(command,3) == " if")
		{
			nested = true;
			nested_level++;
		}

		if(command.length() >= 2 && right(command,command.length()-2) == "};")
			nested_level--;

		if(!nested_level)
			nested = false;

		if(nested)
			command += ";";
	}
	for(auto& command : commands)
		fprintf(stdout,"Command: \"%s\"\n",command.c_str());

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

	if(!stob(getAppOutput(condition).output[0])) return 0;

	for(auto& command : commands)
		runApp(handleBackslashEscSeqs(command),false);

	return 0;
}
