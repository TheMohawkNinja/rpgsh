#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "../headers/functions.h"
//#include "../headers/scope.h"
#include "../headers/text.h"

unsigned long int getPrintLength(std::string str)//std::string.length() returns character count, not the apparent length
{
	unsigned long int length = 0;
	for(const auto& ch : str)
	{
		if     (ch == '\t')  length += 8-(length%8);
		else if(isprint(ch)) length++;//TODO: Determine correct values for other common escape sequences
	}
	return length;
}

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
	input.push_back('v'); //
	input.push_back('{'); //TODO: Don't include this if specifying a variable to edit
	for(int i=0; i<w.ws_col-6; i++) input.push_back('a');
	input.push_back('}'); //
	bool insert_mode = false;
	char k = 0;
	char esc_char = 0;
	long unsigned int cur_pos = input.size()-1;
	long unsigned int prev_cur_pos = cur_pos;

	fprintf(stdout,"%s%sCTRL+ALT+ESC or double-tap ESC to quit.%s\n\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);

	while(true)
	{
		//Set terminal flags for non-buffered reading required for handling keyboard input
		tcgetattr(fileno(stdin), &t_old);
		t_new = t_old;
		t_new.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(fileno(stdin), TCSANOW, &t_new);

		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);

		//Handle annoying edge cases that probably mean I didn't code this program very well
		if(cur_pos / w.ws_col && (cur_pos > (unsigned long int)w.ws_col || prev_cur_pos > cur_pos))
			fprintf(stdout,CURSOR_UP_N,(cur_pos/w.ws_col));
		else if(cur_pos == w.ws_col && prev_cur_pos != cur_pos-1)
			fprintf(stdout,CURSOR_UP);
		if((int)cur_pos == w.ws_col-1 && (int)prev_cur_pos == w.ws_col)
			fprintf(stdout,CURSOR_UP);
		if(prev_cur_pos < cur_pos && cur_pos == input.size() && input.size() > w.ws_col)
			fprintf(stdout,CURSOR_DOWN);

		fprintf(stdout,CLEAR_TO_SCREEN_END);
		for(const auto& ch : input) fprintf(stdout,"%c",ch);
		fprintf(stdout," ");

		fprintf(stdout,"\n\n");
		fprintf(stdout,"input.size() = %ld, w.ws_col = %d, prev_cur_pos = %ld, cur_pos = %lu",input.size(),w.ws_col,prev_cur_pos,cur_pos);
		/*for(int i=0; i<w.ws_col; i++)
			fprintf(stdout,"%s%s%s─",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_BOLD);
		fprintf(stdout,"%s\n\n",TEXT_NORMAL);

		std::string value;
		for(unsigned long int i=2; i<input.size()-3; i++)//Remove the start and end bits of explicit constructor, and space
			value += input[i];
		std::string output = makePretty(value);
		fprintf(stdout,output.c_str());
		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);
		fprintf(stdout,CURSOR_UP_N,(unsigned long int)4+countu(output,'\n')+(long unsigned int)((input.size()-3)/w.ws_col)+(long unsigned int)((output.length()-1)/w.ws_col));
		*/
		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);
		fprintf(stdout,CURSOR_UP_N,(long unsigned int)2+(long unsigned int)(input.size()/w.ws_col));
		if(cur_pos > 0 && cur_pos < w.ws_col)
		{
			fprintf(stdout,CURSOR_RIGHT_N,cur_pos);
		}
		else if(cur_pos > 0)
		{
			if(cur_pos / w.ws_col)
				fprintf(stdout,CURSOR_DOWN_N,cur_pos/w.ws_col);
			if(cur_pos % w.ws_col)
				fprintf(stdout,CURSOR_RIGHT_N,cur_pos%w.ws_col);
		}
		fprintf(stdout,CURSOR_SHOW);

		esc_char = 0;
		k = getchar();
		//fprintf(stdout,CURSOR_HIDE);

		if(k == ESC_SEQ)
		{
			if(getchar() == ESC_SEQ)//Consume '[', exiting if key combo is entered.
			{
				fprintf(stdout,CURSOR_DOWN_N,(long unsigned int)3);
				return 0;
			}
			esc_char = getchar();
		}

		prev_cur_pos = cur_pos;

		if(isprint(k))//Printable characters
		{
			if(insert_mode)	//If the "Insert" key is toggled
				input[cur_pos] = k;
			else
				input.insert(input.begin()+cur_pos,k);
			cur_pos++;
		}
		else if(k == KB_BACKSPACE && cur_pos > 0)//Backspace
		{
			cur_pos--;
			input.erase(input.begin()+cur_pos);
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
					if(cur_pos >= w.ws_col)
					{
						cur_pos -= w.ws_col;
						fprintf(stdout,CURSOR_UP);
					}
					break;
				case 'B':	//Down
					if(cur_pos+w.ws_col <= input.size())
					{
						cur_pos += w.ws_col;
						fprintf(stdout,CURSOR_DOWN);
					}
					break;
				case 'C':	//Right
					if(cur_pos == input.size()) break;
					cur_pos++;
					break;
				case 'D':	//Left
					if(cur_pos == 0) break;
					cur_pos--;
					break;
				case 'H':	//Home
					if(cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					else if(cur_pos % w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
						cur_pos -= cur_pos%w.ws_col;;
					}
					cur_pos = 0;
					break;
				case '5':	//PgUp
					if(getchar() != '~' || cur_pos == 0) break;
					if(cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					}
					else
					{
						if(cur_pos / w.ws_col)
							fprintf(stdout,CURSOR_UP_N,cur_pos/w.ws_col);
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
					}
					cur_pos = 0;
					break;
				case '6':	//PgDown
					if(getchar() != '~' || cur_pos == input.size()) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_RIGHT_N,cur_pos);
					}
					else
					{
						if(cur_pos / w.ws_col)
							fprintf(stdout,CURSOR_DOWN_N,cur_pos/w.ws_col);
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_RIGHT_N,cur_pos%w.ws_col);
					}
					cur_pos = input.size();
					break;
				case '7':	//Home
					if(getchar() != '~' || cur_pos == 0) break;
					if(cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					else if(cur_pos % w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
						cur_pos -= cur_pos%w.ws_col;
					}
					break;
				case 'F':	//End TODO: Needs work on multi-line inputs
					if(cur_pos == input.size()) break;
					if((int)cur_pos < w.ws_col-1 && (int)input.size() >= w.ws_col-1)
					{
						fprintf(stdout,CURSOR_RIGHT_N,w.ws_col-cur_pos-1);
						cur_pos = w.ws_col-1;
					}
					else if((int)cur_pos < w.ws_col-1)
					{
						fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos-1);
						cur_pos += input.size()-cur_pos-1;
					}
					else if(cur_pos % w.ws_col)
					{
						fprintf(stdout,CURSOR_RIGHT_N,w.ws_col-(cur_pos%w.ws_col)-1);
						cur_pos += w.ws_col-(cur_pos%w.ws_col)-1;
					}
					break;
				case '8':	//End TODO: Needs work on multi-line inputs
					if(getchar() != '~' || cur_pos == input.size()) break;
					if((int)cur_pos < w.ws_col-1 && (int)input.size() >= w.ws_col-1)
					{
						fprintf(stdout,CURSOR_RIGHT_N,w.ws_col-cur_pos-1);
						cur_pos = w.ws_col-1;
					}
					else if((int)cur_pos < w.ws_col-1)
					{
						fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos-1);
						cur_pos += input.size()-cur_pos-1;
					}
					else if(cur_pos % w.ws_col)
					{
						fprintf(stdout,CURSOR_RIGHT_N,w.ws_col-(cur_pos%w.ws_col));
						cur_pos += w.ws_col-(cur_pos%w.ws_col)-1;
					}
					break;
				case '2':	//Insert
					if(getchar() == '~') insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() != '~' || cur_pos >= input.size()) break;
					input.erase(input.begin()+cur_pos);
					break;
			}
		}

		//Reset terminal flags in-case of sudden program termination
		tcsetattr(fileno(stdin), TCSANOW, &t_old);
	}

	return 0;
}
