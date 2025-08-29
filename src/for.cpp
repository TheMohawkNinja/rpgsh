#include <cstring>
#include <regex>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(error,"for expects at least 1 argument.");
		return -1;
	}

	chkFlagAppDesc(argv,"Iterates through the key/value pairs of a set");
	chkFlagPreserveVariables(argv,all);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tfor %skey%s,%svalue%s %sset%s { %scommand%s; }\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tfor [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\n");
		fprintf(stdout,"\t%skey%s\tArbitrary variable name for referencing the current key in the set. These are used in the for \"$/%skey%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%svalue%s\tArbitrary variable name for referencing the current value in the set. These are used in the for \"$/%svalue%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%scommand%s\tAn rpgsh command, to be executed if %scondition%s is true. There can be an arbitrary number of commands, each one terminated with a semicolon (;).\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	ComplexCommandInfo cci = parseComplexCommandString(argc, argv, true);

	std::string key, value, set;
	VariableInfo vi;
	if(findu(cci.condition,',') == std::string::npos)
	{
		output(error,"Expected \',\'.");
		return -1;
	}
	key = left(cci.condition,findu(cci.condition,','));
	cci.condition = right(cci.condition,key.length()+1);

	if(findu(cci.condition,' ') == std::string::npos)
	{
		output(error,"Expected value name.");
		return -1;
	}
	value = left(cci.condition,findu(cci.condition,' '));
	set = right(cci.condition,value.length()+1);

	vi = parseVariable(set);
	Scope s = vi.scope;

	for(auto& [k,v] : getSet(getSetStr(vi)))
	{
		for(auto& command : cci.commands)
		{
			while(command.back() == ' ') command.erase(command.end());
			command = std::regex_replace(command,std::regex("\\$/"+key),k);
			command = std::regex_replace(command,std::regex("\\$/"+value),v);
			int status = runApp(handleBackslashEscSeqs(command),false);
			if(status == STATUS_BREAK) return 0;
			if(status == STATUS_CONTINUE) break;
		}
	}

	return 0;
}
