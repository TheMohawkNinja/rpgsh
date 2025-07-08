#include <cstring>
#include "../headers/define.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	chkFlagAppDesc(argv,"Changes the key used for the loaded character's name.");
	chkFlagModifyVariables(argv,false);

	if(argc < 2)
	{
		output(error,"setname requires at least one argument to function!");
		exit(-1);
	}
	else if(argc > 2)
	{
		output(warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);
	}

	if(!strcmp(argv[1],"-l"))
	{
		Character c = Character();

		fprintf(stdout,"Display name character attribute:\t%s%s%s\n",TEXT_WHITE,c.getStr<Var>(DOT_NAME).c_str(),TEXT_NORMAL);
		fprintf(stdout,"Current value of %s%%%s%s:\t\t\t%s%s%s\n",TEXT_WHITE,c.getStr<Var>(DOT_NAME).c_str(),TEXT_NORMAL,TEXT_WHITE,c.getName().c_str(),TEXT_NORMAL);
		return 0;
	}
	else if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tsetname %skey%s\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tsetname [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t-l\t\tLists current display name character attribute and value.\n");
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	Character c = Character();

	if(std::string(argv[1]) == "") output(warning,"%s is empty.",argv[1]);

	c.set<Var>(DOT_NAME,std::string(argv[1]));
	c.save();
	output(info,"Current character's name is now set to the key \"%s\" (Current value: \"%s\")",argv[1],c.getStr<Var>(argv[1]).c_str());

	return 0;
}
