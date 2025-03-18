#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "../headers/functions.h"
#include "../headers/text.h"

unsigned long int getPrintLength(std::string str)//std::string.length() returns character count, not the printed length
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	unsigned long int length = 0;
	for(unsigned long int i=0; i<str.length(); i++)
	{
		if(str[i] == ESC_SEQ) i=str.find('m',i);
		else if(str[i] == '\t' && (length+(8-(length%8)))/w.ws_col == length/w.ws_col)	length += 8-(length%8);
		else if(str[i] == '\t' && (length+(8-(length%8)))/w.ws_col > length/w.ws_col)	length += w.ws_col-length-1;
		else if(str[i] == '\b')  length--;
		else if(isprint(str[i])) length++;
	}
	return length;
}

int main(int argc, char** argv)
{
	if(argc > 2)
		output(Warning,"edit only expects 0 or 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Barebones WYSIWYG text editor for variables. Useful for long and/or heavily formatted text.");
	chkFlagModifyVariables(argv,true);

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
	VariableInfo vi;
	if(argc == 1)
	{
		input.push_back('v');
		input.push_back('{');
		input.push_back('}');
	}
	else if(looksLikeVariable(std::string(argv[argc-1])))
	{
		vi = parseVariable(std::string(argv[argc-1]));

		if(vi.property != "")
		{
			output(Error,"Edit cannot be used to modify properties");
			return -1;
		}

		std::string value;
		switch(vi.evalType)
		{
			case VAR_SIGIL:
				value = vi.scope.getStr<Var>(vi.key);
				break;
			case DICE_SIGIL:
				value = vi.scope.getStr<Dice>(vi.key);
				break;
			case WALLET_SIGIL:
				value = vi.scope.getStr<Wallet>(vi.key);
				break;
			case CURRENCY_SIGIL:
				value = vi.scope.getStr<Currency>(vi.key);
				break;
		}
		for(const auto& ch : value) input.push_back(ch);
	}
	else
	{
		output(Error,"Edit expects either a variable or help flags for an argument.");
		return -1;
	}
	bool insert_mode = false;
	char k = 0;
	char esc_char = 0;
	long unsigned int cur_pos = 2;
	long unsigned int prev_cur_pos = cur_pos;

	fprintf(stdout,"%s%sCTRL+ALT+ESC or ESC+ESC Exit w/o saving   %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sESC+s                   Save              %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sHome                    Beginning of line %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sEnd                     End of line       %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sShift+Left              Back one word     %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sShift+Right             Forward one word  %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sPgUp                    Beginning of input%s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);
	fprintf(stdout,"%s%sPgDown                  End of input      %s\n\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_NORMAL);

	while(true)
	{
		//Set terminal flags for non-buffered reading required for handling keyboard input
		tcgetattr(fileno(stdin), &t_old);
		t_new = t_old;
		t_new.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(fileno(stdin), TCSANOW, &t_new);

		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);

		//Move cursor to beginning of input
		if(cur_pos && (cur_pos-1)/w.ws_col) fprintf(stdout,CURSOR_UP_N,((cur_pos-1)/w.ws_col));

		fprintf(stdout,CLEAR_TO_SCREEN_END);
		for(const auto& ch : input) fprintf(stdout,"%c",ch);
		fprintf(stdout," ");

		fprintf(stdout,"\n\n");
		for(int i=0; i<w.ws_col; i++)
			fprintf(stdout,"%s%s%s─",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_BOLD);

		std::string value;
		for(unsigned long int i=2; i<input.size()-1; i++)//Remove the start and end bits of explicit constructor, and space
			value += input[i];
		std::string output = makePretty(value);
		fprintf(stdout,"%s\nw.ws_col=%d, input.size()=%lu, output_length=%lu, cur_pos=%lu\n",TEXT_NORMAL,w.ws_col,input.size(),getPrintLength(output)-1-(countu(output,'%')/2),cur_pos);
		fprintf(stdout,output.c_str());
		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);
		unsigned long int output_length = getPrintLength(output)-1-(countu(output,'%')/2);
		unsigned long int cursor_vert_offset = 4+(output_length/w.ws_col)+countu(output,'\n')+countu(output,'\f')+countu(output,'\v');
		fprintf(stdout,CURSOR_UP_N,(long unsigned int)(input.size()/w.ws_col)+cursor_vert_offset);
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
		fprintf(stdout,CURSOR_HIDE);

		if(k == ESC_SEQ)
		{
			char next_char = getchar();
			if(next_char == ESC_SEQ)//Consume '[', exiting if key combo is entered.
			{
				fprintf(stdout,CURSOR_DOWN_N,(long unsigned int)((input.size()-cur_pos)/w.ws_col)+cursor_vert_offset);
				fprintf(stdout,"\n");
				fprintf(stdout,CURSOR_SHOW);
				return 0;
			}
			else if(next_char == 's')
			{
				vi.scope.save();
				fprintf(stdout,"%s%sSAVED!%s",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL);
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

			cur_pos+=2;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			switch(esc_char)
			{
				case 'A':	//Up
					if(cur_pos < w.ws_col) break;
					cur_pos -= w.ws_col;
					fprintf(stdout,CURSOR_UP);
					if(cur_pos == w.ws_col  && !(prev_cur_pos%w.ws_col))
						fprintf(stdout,CURSOR_UP);
					break;
				case 'B':	//Down
					if(cur_pos && !(cur_pos%w.ws_col)) fprintf(stdout,CURSOR_UP);
					if(cur_pos+w.ws_col > input.size()) break;
					if(cur_pos/w.ws_col > 1) fprintf(stdout,CURSOR_UP);
					cur_pos += w.ws_col;
					if(cur_pos > w.ws_col) fprintf(stdout,CURSOR_DOWN);
					break;
				case 'C':	//Right
					if(cur_pos == input.size()) break;
					cur_pos++;
					break;
				case 'D':	//Left
					if(cur_pos == 0) break;
					cur_pos--;
					if(cur_pos && (cur_pos/w.ws_col < prev_cur_pos/w.ws_col || !(cur_pos%w.ws_col)))
						fprintf(stdout,CURSOR_UP);
					break;
				case 'c':	//Shift+Right
					if(cur_pos == input.size()) break;
					for(long unsigned int i=cur_pos; input[i]!=' ' && i<input.size(); i++)
						cur_pos++;
					for(long unsigned int i=cur_pos; input[i]==' ' && i<input.size(); i++)
						cur_pos++;

					if(cur_pos/(long unsigned int)w.ws_col > prev_cur_pos/(long unsigned int)w.ws_col)
						fprintf(stdout,CURSOR_DOWN_N,(cur_pos/w.ws_col)-(prev_cur_pos/w.ws_col));
					break;
				case 'd':	//Shift+Left
					if(cur_pos == 0) break;
					for(long unsigned int i=cur_pos; input[i]!=' ' && i>0; i--)
						cur_pos--;
					for(long unsigned int i=cur_pos; input[i]==' ' && i>0; i--)
						cur_pos--;
					for(long unsigned int i=cur_pos; input[i]!=' ' && i>0; i--)
						cur_pos--;
					if(cur_pos > 0) cur_pos++;

					if(cur_pos/(long unsigned int)w.ws_col < prev_cur_pos/(long unsigned int)w.ws_col)
						fprintf(stdout,CURSOR_UP_N,(prev_cur_pos/w.ws_col)-(cur_pos/w.ws_col));
					break;
				case 'H':	//Home
					if(cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					else
					{
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
						fprintf(stdout,CURSOR_UP);
						cur_pos -= cur_pos%w.ws_col;
					}
					cur_pos = 0;
					break;
				case '5':	//PgUp
					if(getchar() != '~' || cur_pos == 0) break;
					if(input.size() < w.ws_col)
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
					if(input.size() < w.ws_col)
					{
						fprintf(stdout,CURSOR_RIGHT_N,w.ws_col-cur_pos);
					}
					else
					{
						if(input.size()/w.ws_col > cur_pos/w.ws_col)
							fprintf(stdout,CURSOR_DOWN_N,((input.size()/w.ws_col)-(cur_pos/w.ws_col)));
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_RIGHT_N,cur_pos%w.ws_col);
					}
					cur_pos = input.size();
					break;
				case '7':	//Home
					if(getchar() != '~' || cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					else
					{
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
						fprintf(stdout,CURSOR_UP);
						cur_pos -= cur_pos%w.ws_col;
					}
					break;
				case 'F':	//End
					if(cur_pos == input.size() || !((cur_pos+1)%w.ws_col)) break;
					if((long unsigned int)(cur_pos+w.ws_col) > input.size())
						cur_pos = input.size();
					else if(cur_pos < w.ws_col)
						cur_pos = w.ws_col-1;
					else
						cur_pos += w.ws_col-(cur_pos%w.ws_col)-1;
					break;
				case '8':	//End
					if(getchar() != '~' || cur_pos == input.size() || !((cur_pos+1)%w.ws_col)) break;
					if((long unsigned int)(cur_pos+w.ws_col) > input.size())
						cur_pos = input.size();
					else if(cur_pos < w.ws_col)
						cur_pos = w.ws_col-1;
					else
						cur_pos += w.ws_col-(cur_pos%w.ws_col)-1;
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
