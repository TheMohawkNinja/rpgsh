#include <cstring>
#include <stdio.h>
#include <string>
#include "../headers/define.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"banner only expects 0 or 1 arguments, ignoring all other arguments.");

	check_print_app_description(argv,"Prints pretty banner art and the author's signature.");

	fprintf(stdout,"                                         %s▟                   %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s█▙        ▁▁        %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"%s  ▜█████████████████████████████████████%s█%s███▒▒▒▒▒▒███%s     %s\n",TEXT_LIGHTGRAY,TEXT_YELLOW,TEXT_BG_BLACK,TEXT_BG_DEFAULTCOLOR,TEXT_NORMAL);
	fprintf(stdout,"                                         %s█▛        ▔▔        %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s▜                   %s\n",TEXT_YELLOW,TEXT_NORMAL);
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
	fprintf(stdout,"%s                                            ▄▄▄▄▄▄▄▄         %s\n",TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s                               ▄▄     ▄█   ██%s██████%s██    %s\n",TEXT_RED,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s  ▀   ▄   ▀     ▀▀  ▄▄   ▀▀        ▄▄     ██%s███%s██%s██%s██%s\n",TEXT_RED,TEXT_YELLOW,TEXT_WHITE,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s             ▀               ▀▀         ▀█ ██%s██████%s██    %s\n",TEXT_RED,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s                                            ▀▀▀▀▀▀▀▀         %s\n",TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"\n");
	fprintf(stdout,"The interactive Linux shell for table-top RPGs.\n\nBy %s%s%s%s\n",TEXT_ITALIC,TEXT_LIGHTBLUE,"bd@0xbd.net",TEXT_NORMAL);
	return 0;
}
