#include <stdio.h>
#include <string>
#include "text.h"

int main()
{
	fprintf(stdout,"                                         %s▟                   %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s█▙                  %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"%s  ▜█████████████████████████████████████%s██████████■██      %s\n",TEXT_LIGHTGRAY,TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s█▛                  %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"                                         %s▜                   %s\n",TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"%s  ┌────────┐ ┌────────┐ ┌────────┐ %s                        %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │########└┐│########└┐│########└┐%s ┌──────┐┌─┐            %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#┌─────┐#││#┌─────┐#││#┌─────┐#│%s┌┘######││#│            %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#└─────┘#││#└─────┘#││#│     └─┘%s│#┌─────┘│#│            %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │########┌┘│########┌┘│#│   ┌───┐%s│#└────┐ │#└────┐       %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#┌──┐##┌┘ │#┌──────┘ │#│   │###│%s└┐#####└┐│######└┐      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#│  └┐#└┐ │#│        │#│   └─┐#│%s └────┐#││#┌───┐#│      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#│   └┐#└┐│#│        │#└─────┘#│%s┌─────┘#││#│   │#│      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  │#│    └┐#││#│        └┐########│%s│######┌┘│#│   │#│      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s  └─┘     └─┘└─┘         └────────┘%s└──────┘ └─┘   └─┘      %s\n",TEXT_CYAN,TEXT_MAGENTA,TEXT_NORMAL);
	fprintf(stdout,"%s                                            ▄▄▄▄▄▄▄▄         %s\n",TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s                               ▄▄     ▄█   ██%s██████%s██    %s\n",TEXT_RED,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s  ▀   ▄   ▀     ▀▀  ▄▄   ▀▀        ▄▄     ██%s███%s██%s██%s██%s\n",TEXT_RED,TEXT_YELLOW,TEXT_WHITE,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s             ▀               ▀▀         ▀█ ██%s██████%s██    %s\n",TEXT_RED,TEXT_YELLOW,TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"%s                                            ▀▀▀▀▀▀▀▀         %s\n",TEXT_RED,TEXT_NORMAL);
	fprintf(stdout,"\n");
	fprintf(stdout,"The interactive Linux shell for table-top RPGs.\n\nBy %s%s%s%s.\n",TEXT_ITALIC,TEXT_LIGHTBLUE,"TheMohawkNinja",TEXT_NORMAL);
	return 0;
}
