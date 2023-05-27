#include <stdio.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include "text.h"
#include "classes.h"

void run_dndsh_prog(const char prog[],DNDSH_CHARACTER c,char args[])
{
	char* prog_name = (char*)prog;
	extern char** environ;
	pid_t pid;

	//strcat() calls for setting up the correct parameter values for posix_spawn()
	//Also defines naming convention for all sub-programs of dndsh to be prefixed with "dndsh-"
	char prefix[] = "dndsh-";
	char full_prog[(sizeof(prog_name)+sizeof(prefix))+2];
	for(int i = 0; i<sizeof(full_prog); i++)
	{
		full_prog[i]='\0';
	}
	strcat(full_prog,prefix);
	strcat(full_prog,prog);

	char path[] = "/home/terminus/code/git/dndsh/";
	char full_path[sizeof(full_prog)+sizeof(path)+2];
	for(int i = 0; i<sizeof(full_path); i++)
	{
		full_path[i]='\0';
	}
	strcat(full_path,path);
	strcat(full_path,full_prog);

	char* const argv[] = {full_prog,c.Name,args,NULL};
	int status;

	fprintf(stdout,"\n");

	status = posix_spawn(&pid, full_path, NULL, NULL, argv, environ);

	if(status == 0)
	{
		do
		{
			if(waitpid(pid, &status, 0) == -1)
			{
				exit(1);
			}
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	else
	{
		if(status == 2)//File not found
		{
			fprintf(stderr,"%s%sERROR [%d]: \"%s\" is not a valid dndsh command.%s\n",TEXT_BOLD,TEXT_RED,status,prog,TEXT_NORMAL);
		}
		else
		{
			fprintf(stderr,"%s%sERROR [%d]: %s%s\n",TEXT_BOLD,TEXT_RED,status,strerror(status),TEXT_NORMAL);
		}
	}

	fprintf(stdout,"\n");
}

int prompt(DNDSH_CHARACTER c)
{
	fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%hhu/%hhu%s %s(%hhu)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,c.Name,TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,c.HP,c.MaxHP,TEXT_NOBOLD,TEXT_ITALIC,c.TempHP,TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
	fprintf(stdout,"%s└─%sĐ₦Đ%s─%s$%s ",TEXT_WHITE,TEXT_CYAN,TEXT_WHITE,TEXT_CYAN,TEXT_NORMAL);

	char input[256];
	fgets(input,sizeof(input),stdin);
	input[strcspn(input,"\n")] = 0; //Omits newline character from input buffer (https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)

	if(!strcmp(input,"exit"))
	{
		fprintf(stdout,"Exiting...\n");
		return -1;
	}
	else
	{
		std::string prog = "";

		for(int i=0; i<sizeof(input); i++)
		{
			int num = input[i];
			if(num == 0){break;} //null terminator
			else if(num == 32){break;} //space
			else{prog += input[i];}
		}
		if(strcmp(prog.c_str(),""))
		{
			run_dndsh_prog(prog.c_str(),c,input);
		}
		return 0;
	}

	return 0;
}
int main()
{
	DNDSH_CHARACTER character = DNDSH_CHARACTER();

	run_dndsh_prog("banner",character,NULL);
	run_dndsh_prog("version",character,NULL);

	while(prompt(character) >= 0){}

	return 0;
}
