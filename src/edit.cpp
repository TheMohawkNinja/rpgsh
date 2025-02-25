#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "../headers/functions.h"
//#include "../headers/scope.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"edit only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Barebones WYSIWYG text editor for variables. Useful for long and/or heavily formatted text.");
	chkFlagModifyVariables(argv,false);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tedit [[%sOPTIONS%s]|%svariable%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tIf no %svariable%s is specified, treat as scratchpad (no saving). Otherwise, load the %svariable%s into the editor\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);

		return 0;
	}

	#define KB_TAB		9
	#define KB_ENTER	10
	#define KB_BACKSPACE	127

	//Get terminal dimensions
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	struct termios t_old, t_new;

	std::vector<char> input;
	input.push_back('v');//
	input.push_back('{');//TODO: Don't include this if specifying a variable to edit
	input.push_back('}');//
	bool insert_mode = false;
	char k = 0;
	char esc_char = 0;
	long unsigned int cur_pos = 0;

	fprintf(stdout,"%s%sCTRL+ALT+ESC or double-tap ESC to quit.%s\n\n\n\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	for(int i=0; i<w.ws_col; i++)
		fprintf(stdout,"%s%s%s─",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_BOLD);
	fprintf(stdout,"%s\n\n",TEXT_NORMAL);
	fprintf(stdout,CURSOR_UP_N,(long unsigned int)4);
	for(const auto& ch : input) fprintf(stdout,"%c",ch);
	fprintf(stdout,CURSOR_LEFT_N,(long unsigned int)1);//TODO: Derive this number from any arguments provided
	cur_pos = 2;

	while(true)
	{
		//Set terminal flags for non-buffered reading required for handling keyboard input
		tcgetattr(fileno(stdin), &t_old);
		t_new = t_old;
		t_new.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(fileno(stdin), TCSANOW, &t_new);

		esc_char = 0;
		k = getchar();

		if(k == ESC_SEQ)
		{
			if(getchar() == ESC_SEQ)//Consume '[', exiting if key combo is entered.
			{
				fprintf(stdout,CURSOR_DOWN_N,(long unsigned int)3);
				return 0;
			}
			esc_char = getchar();
		}

		if(isprint(k))//Printable characters
		{
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < input.size()) input[cur_pos] = k;
				else input.push_back(k);
			}
			else
			{
				input.insert(input.begin()+cur_pos,k);
			}

			fprintf(stdout,CLEAR_LINE);

			for(long unsigned int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);
			if(cur_pos < input.size()-1)
				fprintf(stdout,CURSOR_LEFT_N,input.size()-1-cur_pos);

			cur_pos++;
		}
		else if(k == KB_BACKSPACE && cur_pos > 0)//Backspace
		{
			fprintf(stdout,"\b%s",CLEAR_TO_LINE_END);

			cur_pos--;
			input.erase(input.begin()+cur_pos);

			for(long unsigned int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);
			if(cur_pos < input.size())
				fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);
		}
		else if(k == KB_TAB || k == KB_ENTER)
		{
			char char_to_insert = (k == KB_TAB ? 't' : 'n');
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < input.size())
				{
					input[cur_pos] = '\\';
					input.insert(input.begin()+cur_pos+1,char_to_insert);
				}
				else
				{
					input.push_back('\\');
					input.push_back(char_to_insert);
				}
			}
			else
			{
				input.insert(input.begin()+cur_pos,'\\');
				input.insert(input.begin()+cur_pos+1,char_to_insert);
			}

			fprintf(stdout,CLEAR_LINE);

			for(long unsigned int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);
			if(cur_pos < input.size()-2)
				fprintf(stdout,CURSOR_LEFT_N,input.size()-2-cur_pos);

			cur_pos+=2;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			switch(esc_char)
			{
				case 'A':	//Up
					fprintf(stdout,CURSOR_UP_N,cur_pos/w.ws_col);
					break;
				case 'B':	//Down
					//TODO: Cursor down
					break;
				case 'C':	//Right
					if(cur_pos >= input.size()) continue;
					fprintf(stdout,CURSOR_RIGHT);
					cur_pos++;
					break;
				case 'D':	//Left
					if(cur_pos == 0) continue;
					fprintf(stdout,CURSOR_LEFT);
					cur_pos--;
					break;
				case 'H':	//Home
					if(cur_pos == 0) continue;
					fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					cur_pos = 0;
					break;
				case '5':	//PgUp
					if(getchar() != '~' || cur_pos == 0) continue;
					//TODO: Go to beginning of input
					break;
				case '6':	//PgDown
					if(getchar() != '~' || cur_pos == input.size()) continue;
					//TODO: Go to end of input
					break;
				case '7':	//Home
					if(getchar() != '~' || cur_pos == 0) continue;
					fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					cur_pos = 0;
					break;
				case 'F':	//End
					if(cur_pos >= input.size()) continue;
					fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos);
					cur_pos = input.size();
					break;
				case '8':	//End
					if(getchar() != '~' || cur_pos >= input.size()) continue;
					fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos);
					cur_pos = input.size();
					break;
				case '2':	//Insert
					if(getchar() == '~') insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() != '~' || cur_pos >= input.size()) continue;
					fprintf(stdout,CLEAR_LINE);
					input.erase(input.begin()+cur_pos);
					for(long unsigned int i=cur_pos; i<input.size(); i++)
						fprintf(stdout,"%c",input[i]);
					if(cur_pos < input.size())
						fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);
					break;
			}
		}

		if(cur_pos-input.size()) fprintf(stdout,CURSOR_RIGHT_N,cur_pos-input.size());
		input[input.size()] = '\0';
		fprintf(stdout,CLEAR_TO_SCREEN_END);
		fprintf(stdout,"\n\n");
		for(int i=0; i<w.ws_col; i++)
			fprintf(stdout,"%s%s%s─",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_BOLD);
		fprintf(stdout,"%s\n\n",TEXT_NORMAL);
		std::string value;
		for(unsigned long int i=2; i<input.size()-1; i++)//Remove the start and end bits of explicit constructor
			value += input[i];
		std::string output = makePretty(value);
		fprintf(stdout,output.c_str());
		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);
		fprintf(stdout,CURSOR_UP_N,(unsigned long int)4+countu(output,'\n'));
		fprintf(stdout,CURSOR_RIGHT_N,cur_pos);

		//Reset terminal flags in-case of sudden program termination
		tcsetattr(fileno(stdin), TCSANOW, &t_old);
	}

	return 0;
}
