#include <cstring>
#include <string>
#include "../headers/configuration.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
		output(warning,"exec expects at least one arg.");

	chkFlagAppDesc(argv,"Executes a program, including those outside the rpgsh environment.");
	chkFlagPreserveVariables(argv,none);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\texec [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%sarg_list%s\tExecutes the command and args define by %sarg_list%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	std::string arg_list;
	for(int i=1; i<argc; i++) arg_list += std::string(argv[i])+" ";
	arg_list.erase(arg_list.begin()+arg_list.length()-1);

	//Is this technically bad practice? Pretty sure.
	//Is this the easiest way to make it work intuitively for the user? Yup.
	system(arg_list.c_str());

	return 0;
}
