#include <stdio.h>
#include <string>
#include "text.h"

int main()
{
	const char* norm =	std::string(TEXT_NORMAL).c_str();
	const char* blade =	std::string(TEXT_LIGHTGRAY).c_str();
	const char* hilt =	std::string(TEXT_YELLOW).c_str();
	const char* R =		std::string(TEXT_CYAN).c_str();
	const char* P =		std::string(TEXT_CYAN).c_str();
	const char* G =		std::string(TEXT_CYAN).c_str();
	const char* s =		std::string(TEXT_LIGHTGRAY).c_str();
	const char* h =		std::string(TEXT_LIGHTGRAY).c_str();
	const char* fb =	std::string(TEXT_RED).c_str();
	const char* fb_oc =	std::string(TEXT_YELLOW).c_str();
	const char* fb_ic =	std::string(TEXT_WHITE).c_str();

	fprintf(stdout,"                                             %s◢                 %s\n",hilt,norm);
	fprintf(stdout,"                                             %s█                 %s\n",hilt,norm);
	fprintf(stdout,"  %s◥█████████████████████████████████████████%s██████████■██    %s\n",blade,hilt,norm);
	fprintf(stdout,"                                             %s█                 %s\n",hilt,norm);
	fprintf(stdout,"                                             %s◥                 %s\n",hilt,norm);
	fprintf(stdout,"  %s┌────────┐   %s┌────────┐ %s┌────────┐ %s         %s         %s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│########└┐  %s│########└┐%s│########└┐%s┌───────┐%s┌─┐      %s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│#┌─────┐#│  %s│#┌─────┐#│%s│#┌─────┐#│%s│#######│%s│#│      %s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│#└─────┘#│  %s│#└─────┘#│%s│#│     └─┘%s│#┌─────┘%s│#│      %s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│########┬┘  %s│########┌┘%s│#│   ┌───┐%s│#└─────┐%s│#└────┐ %s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│#┌────┐#└┐  %s│#┌──────┘ %s│#│   │###│%s│#######│%s│######└┐%s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│#│    └┐#└┐ %s│#│        %s│#│   └─┐#│%s└─────┐#│%s│#┌───┐#│%s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│#│     └┐#└┐%s│#│        %s│#└─────┘#│%s┌─────┘#│%s│#│   │#│%s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s│#│      └┐#│%s│#│        %s└┐########│%s│#######│%s│#│   │#│%s\n",R,P,G,s,h,norm);
	fprintf(stdout,"  %s└─┘       └─┘%s└─┘        %s └────────┘%s└───────┘%s└─┘   └─┘%s\n",R,P,G,s,h,norm);
	fprintf(stdout,"%s                                             ▄▄▄▄▄▄▄▄          %s\n",fb,norm);
	fprintf(stdout,"%s                ▄               ▄▄     ▄█   ██%s██████%s██     %s\n",fb,fb_oc,fb,norm);
	fprintf(stdout,"%s    ▀    ▄         ▀▀   ▄▄  ▀▀     ▄▄      ██%s███%s██%s██%s██%s\n",fb,fb_oc,fb_ic,fb_oc,fb,norm);
	fprintf(stdout,"%s             ▀                ▀▀         ▀█ ██%s██████%s██     %s\n",fb,fb_oc,fb,norm);
	fprintf(stdout,"%s                                             ▀▀▀▀▀▀▀▀          %s\n",fb,norm);
	fprintf(stdout,"\n");
	fprintf(stdout,"The interactive Linux shell for table-top RPGs.\n\nBy %s.\n","TheMohawkNinja");
	return 0;
}
