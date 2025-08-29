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

	ComplexCommandInfo cci = parseComplexCommandString(argc, argv, true);

	if(!stob(getAppOutput("eval "+cci.condition).output[0])) return 0;

	for(auto& command : cci.commands)
	{
		int status = runApp(handleBackslashEscSeqs(command),false);
		if(status == STATUS_BREAK || status == STATUS_CONTINUE)
			return status;
	}

	return 0;
}
