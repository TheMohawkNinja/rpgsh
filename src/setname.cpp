#include <cstring>
#include "../headers/define.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/scope.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argv[1] && !strcmp(argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"Changes the variable used for the loaded character's name.\n");
		return 0;
	}

	if(argc < 2)
	{
		output(Error,"setname requires at least one argument to function!");
		exit(-1);
	}
	else if(argc > 2)
	{
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);
	}

	if(!strcmp(argv[1],"-l") || !strcmp(argv[1],"--list"))
	{
		Character c = Character(false);
		c.load();

		fprintf(stdout,"Display name character attribute:\t%s%s%s\n",TEXT_WHITE,c.getStr<Var>(CHAR_NAME_ATTR).c_str(),TEXT_NORMAL);
		fprintf(stdout,"Current value of %s%%%s%s:\t\t\t%s%s%s\n",TEXT_WHITE,c.getStr<Var>(CHAR_NAME_ATTR).c_str(),TEXT_NORMAL,TEXT_WHITE,c.getName().c_str(),TEXT_NORMAL);
		return 0;
	}
	else if(!strcmp(argv[1],"-?") || !strcmp(argv[1],"--help"))
	{
		fprintf(stdout,"Sets the character attribute to be used when displaying the character's name.\n\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tsetname %sattr%s		Sets the character attribute for their display name to the character attribute %sattr%s. The \'%%\' must be omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tsetname [-l|--list]	Lists current display name character attribute and value.\n");
		fprintf(stdout,"\tsetname [-?|--help]	Displays this help text.\n");
		fprintf(stdout,"\n");
		fprintf(stdout,"EXAMPLES:\n");
		fprintf(stdout,"\tsetname FirstName	Sets the character attribute for their display name to \"FirstName\".\n");
		fprintf(stdout,"\tsetname --list		Display the current display name attribute and value.\n");
		fprintf(stdout,"\tsetname -?		Displays help text.\n");
		return 0;
	}

	Character c = Character(false);
	c.load();

	if(std::string(argv[1]) == "")
		output(Warning,"%s is empty.",argv[1]);

	c.set<Var>(CHAR_NAME_ATTR,std::string(argv[1]));
	c.save();
	output(Info,"Character's display name is now set to the character attribute \"%s\" (Current value: \"%s\")",argv[1],c.getStr<Var>(argv[1]).c_str());

	return 0;
}
