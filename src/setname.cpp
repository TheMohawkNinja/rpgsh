#include "classes.h"

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		output(Error,"setname requires at least one argument to function!");
		exit(-1);
	}
	else if(argc > 2)
	{
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);
	}

	if(!strcmp(argv[1],"-?") || !strcmp(argv[1],"--help"))
	{
		fprintf(stdout,"Sets the character attribute to be used when displaying the character's name.\n\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tsetname %sattr%s		Sets the character attribute for their display name to the character attribute %sattr%s. The \'%%\' must be omitted.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\tsetname [-?|--help]	Displays this help text.\n");
		fprintf(stdout,"\n");
		fprintf(stdout,"EXAMPLES:\n");
		fprintf(stdout,"\tsetname FirstName	Sets the character attribute for their display name to \"FirstName\".\n");
		fprintf(stdout,"\tsetname -?		Displays help text.\n");
		return 0;
	}

	RPGSH_CHAR c = RPGSH_CHAR();
	c.load(get_shell_var(CURRENT_CHAR_SHELL_VAR),false);

	if(std::string(c.Attr[argv[1]]) == "")
	{
		output(Warning,"%s is empty.",argv[1]);
	}

	c.update_Name(argv[1]);
	output(Info,"Character's display name is now set to the character attribute \"%s\" (Current value: \"%s\")",argv[1],c.Attr[argv[1]].c_str());

	return 0;
}