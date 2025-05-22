#include <cstring>
#include <stdio.h>
#include <string>
#include "../headers/chkflag.h"
#include "../headers/define.h"
#include "../headers/functions.h"
#include "../headers/text.h"

void printSword()
{
	fprintf(stdout,"                                         %s▟                   %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s█▙        ▁▁        %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"%s  ▜█████████████████████████████████████%s█%s███▒▒▒▒▒▒███%s  %s\n",TEXT_LIGHTGRAY,TEXT_YELLOW,TEXT_BG_BLACK,TEXT_BG_DEFAULTCOLOR,TEXT_NORMAL);
	fprintf(stdout,"                                         %s█▛        ▔▔        %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s▜                   %s\n",TEXT_YELLOW,TEXT_NORMAL);
}
void printName()
{
	fprintf(stdout,"%s  ┌────────┐ ┌────────┐ ┌────────┐ %s                        %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │########└┐│########└┐│########└┐%s ┌──────┐┌─┐            %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#┌─────┐#││#┌─────┐#││#┌─────┐#│%s┌┘$$$$$$││$│            %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#└─────┘#││#└─────┘#││#│     └─┘%s│$┌─────┘│$│            %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │########┌┘│########┌┘│#│   ┌───┐%s│$└────┐ │$└────┐       %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#┌──┐##┌┘ │#┌──────┘ │#│   │###│%s└┐$$$$$└┐│$$$$$$└┐      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#│  └┐#└┐ │#│        │#│   └─┐#│%s └────┐$││$┌───┐$│      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#│   └┐#└┐│#│        │#└─────┘#│%s┌─────┘$││$│   │$│      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#│    └┐#││#│        └┐########│%s│$$$$$$┌┘│$│   │$│      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  └─┘     └─┘└─┘         └────────┘%s└──────┘ └─┘   └─┘      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
}
void printFireball()
{
	fprintf(stdout,"%s                                            ▄▄▄▄▄▄▄▄         %s\n",TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s                               ▄▄     ▄█   ██%s██████%s██    %s\n",TEXT_RED,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s  ▀   ▄   ▀     ▀▀  ▄▄   ▀▀        ▄▄     ██%s███%s██%s██%s██%s\n",TEXT_RED,TEXT_YELLOW,TEXT_WHITE,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s             ▀               ▀▀         ▀█ ██%s██████%s██    %s\n",TEXT_RED,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s                                            ▀▀▀▀▀▀▀▀         %s\n",TEXT_RED,TEXT_NORMAL);
}
void printSignature()
{
	fprintf(stdout,"The interactive Linux shell for table-top RPGs.\n\nBy %s%s%s%s\n",TEXT_ITALIC,TEXT_LIGHTBLUE,"bd@0xbd.net",TEXT_NORMAL);
}
int main(int argc, char** argv)
{
	chkFlagAppDesc(argv,"Prints pretty banner art and the author's signature.");
	chkFlagModifyVariables(argv,false);

	bool sword, name, fireball, signature;
	sword = name = fireball = signature = false;

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tbanner [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints the sword, name, fireball, and signature.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t--sword\t\tPrints the sword.\n");
		fprintf(stdout,"\t--name\t\tPrints the title.\n");
		fprintf(stdout,"\t--fireball\tPrints the fireball.\n");
		fprintf(stdout,"\t--signature\tPrints the signature.\n");
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);

		return 0;
	}

	for(int i=1; i<argc; i++)
	{
		if(!strcmp(argv[i],"--sword")) sword = true;
		else if(!strcmp(argv[i],"--name")) name = true;
		else if(!strcmp(argv[i],"--fireball")) fireball = true;
		else if(!strcmp(argv[i],"--signature")) signature = true;
	}

	if(argc == 1) sword = name = fireball = signature = true;

	if(sword) printSword();
	if(name) printName();
	if(fireball) printFireball();
	if(signature && (sword | name | fireball)) fprintf(stdout,"\n");
	if(signature) printSignature();

	return 0;
}
