#include <regex>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 2)
		output(warning,"edit only expects 0 or 1 arguments, ignoring all other arguments.");

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
			output(error,"Edit cannot be used to modify properties");
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
		for(const auto& ch : value)
			input.push_back(ch);
	}
	else
	{
		output(error,"Edit expects either a variable or help flags for an argument.");
		return -1;
	}
	bool insert_mode = false;
	char k = 0;
	char esc_char = 0;
	long unsigned int cur_pos = 2;
	long unsigned int char_pos = cur_pos;
	long unsigned int prev_cur_pos = cur_pos;

	enum SaveTextState
	{
		unsaved,
		show_save_text,
		remove_save_text
	};

	SaveTextState sts = unsaved;

	fprintf(stdout,"%s%s%sCTRL%s+%sALT%s+%sESC%s or %sESC%s+%sESC%s Exit w/o saving   %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_ITALIC,TEXT_NOITALIC,TEXT_ITALIC,TEXT_NOITALIC,TEXT_ITALIC,TEXT_NOITALIC,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sESC%s+s                   Save              %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sHome%s                    Beginning of line %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sEnd%s                     End of line       %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sShift%s+%sLeft%s              Back one word     %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sShift%s+%sRight%s             Forward one word  %s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sPgUp%s                    Beginning of input%s\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);
	fprintf(stdout,"%s%s%sPgDown%s                  End of input      %s\n\n",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_ITALIC,TEXT_NOITALIC,TEXT_NORMAL);

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

		if(sts == remove_save_text)
		{
			fprintf(stdout,CURSOR_UP);
			fprintf(stdout,CLEAR_LINE);
			fprintf(stdout,CURSOR_DOWN);
			sts = unsaved;
		}
		else if(sts == show_save_text)
		{
			sts = remove_save_text;
		}

		for(const auto& ch : input) fprintf(stdout,"%c",ch);
		fprintf(stdout," ");

		fprintf(stdout,"%s\n%s\n",CLEAR_TO_LINE_END,CLEAR_LINE);
		for(int i=0; i<w.ws_col; i++)
			fprintf(stdout,"%s%s%s─",TEXT_BG_DARKGRAY,TEXT_WHITE,TEXT_BOLD);

		//Remove beginning and end bits of explicit constructor
		std::string value;
		for(unsigned long int i=2; i<input.size()-1; i++)
			value += input[i];

		replaceVariables(&value,false);
		std::string output = makePretty(value);
		fprintf(stdout,"%s\n%s\n",TEXT_NORMAL,CLEAR_LINE);
		unsigned long int input_display_length = getInputDisplayLength(input);

		//Clear all characters after newlines to remove left over characters from previous buffer if characters in that line were deleted
		if(!(input.size()%w.ws_col)) fprintf(stdout,CLEAR_TO_SCREEN_END);
		fprintf(stdout,std::regex_replace(output,std::regex("\n"),std::string(CLEAR_TO_LINE_END)+"\n").c_str());//If input prior to '\n' was shortened, this removes trailing characters
		fprintf(stdout,CLEAR_TO_SCREEN_END);
		fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);

		//Determine number of paragraphs
		std::string strippedOutput = stripFormatting(output);
		unsigned long int output_display_length = getDisplayLength(strippedOutput);
		unsigned long int total_paragraph_lines = 0;
		if(findu(strippedOutput,'\n') != std::string::npos)
		{
			for(unsigned long int i=0; i<output_display_length-1; i++)
			{
				unsigned long int next_newline = findu(strippedOutput,'\n',i);
				if(next_newline != std::string::npos && next_newline != i)
				{
					total_paragraph_lines += (next_newline-1-i)/w.ws_col;
				}
				else if(next_newline == std::string::npos && next_newline != i)
				{
					total_paragraph_lines += (output_display_length-1-i)/w.ws_col;
					break;
				}
				i = next_newline;
			}
		}

		unsigned long int cursor_vert_offset = 4+countu(strippedOutput,'\n')+countu(strippedOutput,'\f')+countu(strippedOutput,'\v');
		if(total_paragraph_lines)
			cursor_vert_offset += total_paragraph_lines;
		else
			cursor_vert_offset += (output_display_length-1)/w.ws_col;
		fprintf(stdout,CURSOR_UP_N,(long unsigned int)(input_display_length/w.ws_col)+cursor_vert_offset);
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

		esc_char = 0;
		fprintf(stdout,CURSOR_SHOW);
		k = getchar();
		fprintf(stdout,CURSOR_HIDE);

		if(k == ESC_SEQ)
		{
			char next_char = getchar();
			if(next_char == ESC_SEQ)//Consume '[', exiting if key combo is entered.
			{
				fprintf(stdout,CURSOR_DOWN_N,(long unsigned int)((input_display_length-cur_pos)/w.ws_col)+cursor_vert_offset);
				fprintf(stdout,"\n");
				fprintf(stdout,CURSOR_SHOW);
				return 0;
			}
			else if(next_char == 's')
			{
				switch(vi.evalType)
				{
					case VAR_SIGIL:
						vi.scope.set<Var>(vi.key,Var(std::string(input.data())));
						break;
					case DICE_SIGIL:
						vi.scope.set<Dice>(vi.key,Dice(std::string(input.data())));
						break;
					case WALLET_SIGIL:
						vi.scope.set<Wallet>(vi.key,Wallet(std::string(input.data())));
						break;
					case CURRENCY_SIGIL:
						vi.scope.set<Currency>(vi.key,Currency(std::string(input.data())));
						break;
				}
				fprintf(stdout,CURSOR_SET_COL_N,(unsigned long int)0);
				fprintf(stdout,CURSOR_UP_N,(cur_pos/w.ws_col)+1);
				fprintf(stdout,"%s%s%c%c/%s has been saved%s",TEXT_BOLD,TEXT_CYAN,vi.scope.sigil,vi.evalType,vi.key.c_str(),TEXT_NORMAL);
				fprintf(stdout,CURSOR_DOWN_N,(cur_pos/w.ws_col)+1);
				if(cur_pos < w.ws_col)
					fprintf(stdout,CURSOR_RIGHT_N,cur_pos);
				else if(cur_pos % w.ws_col)
					fprintf(stdout,CURSOR_RIGHT_N,cur_pos%w.ws_col);

				vi.scope.save();
				sts = show_save_text;
				esc_char = 0;
			}
			else
			{
				esc_char = getchar();
			}
		}

		prev_cur_pos = cur_pos;

		if(k == KB_BACKSPACE && cur_pos > 0)//Backspace
		{
			if(cur_pos > 1 && (!(cur_pos%w.ws_col) || cur_pos%w.ws_col == 1))
				fprintf(stdout,CURSOR_UP);

			cur_pos--;
			char_pos--;
			if(getCharLength(input[char_pos]) == 1)
			{
				input.erase(input.begin()+char_pos);
				continue;
			}
			for(unsigned long i=char_pos; !getCharLength(input[i]); i--)
			{
				input.erase(input.begin()+i);
				char_pos--;
			}
			input.erase(input.begin()+char_pos);
		}
		else if(k == KB_TAB || k == KB_ENTER)
		{
			char char_to_insert = (k == KB_TAB ? 't' : 'n');
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < getInputDisplayLength(input))
				{
					input[char_pos] = '\\';
					input.insert(input.begin()+char_pos+1,char_to_insert);
				}
				else
				{
					input.push_back('\\');
					input.push_back(char_to_insert);
				}
			}
			else
			{
				input.insert(input.begin()+char_pos,'\\');
				input.insert(input.begin()+char_pos+1,char_to_insert);
			}

			cur_pos+=2;
			char_pos+=2;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			long unsigned int up_end = cur_pos-w.ws_col;
			long unsigned int down_end = cur_pos+w.ws_col;
			long unsigned int del_end = (cur_pos < getInputDisplayLength(input) ? cur_pos+getCharLength(input[char_pos]) : 0);
			switch(esc_char)
			{
				case 'A':	//Up
					if(cur_pos < w.ws_col) break;
					for(long unsigned int i=cur_pos; i>up_end; i--)
					{
						char_pos--;
						while(!getCharLength(input[char_pos])) char_pos--;
					}
					cur_pos = up_end;
					fprintf(stdout,CURSOR_UP);
					if(cur_pos >= w.ws_col  && !(prev_cur_pos%w.ws_col))
						fprintf(stdout,CURSOR_UP);
					break;
				case 'B':	//Down
					if(cur_pos && !(cur_pos%w.ws_col)) fprintf(stdout,CURSOR_UP);
					if(cur_pos+w.ws_col > getInputDisplayLength(input)) break;
					for(long unsigned int i=cur_pos; i<down_end; i++)
						char_pos += getCharLength(input[i]);
					cur_pos = down_end;
					if(cur_pos > w.ws_col) fprintf(stdout,CURSOR_DOWN);
					break;
				case 'C':	//Right
					if(cur_pos == getInputDisplayLength(input)) break;
					cur_pos++;
					char_pos+=getCharLength(input[char_pos]);
					break;
				case 'D':	//Left
					if(cur_pos == 0) break;
					cur_pos--;
					char_pos--;
					while(!getCharLength(input[char_pos])) char_pos--;
					if(cur_pos && (cur_pos/w.ws_col < prev_cur_pos/w.ws_col || !(cur_pos%w.ws_col)))
						fprintf(stdout,CURSOR_UP);
					break;
				case 'c':	//Shift+Right
					if(cur_pos == input.size()) break;
					for(long unsigned int i=cur_pos; input[i]!=' ' && i<input.size(); i++)
					{
						cur_pos++;
						char_pos+=getCharLength(input[char_pos]);
					}
					for(long unsigned int i=cur_pos; input[i]==' ' && i<input.size(); i++)
					{
						cur_pos++;
						char_pos+=getCharLength(input[char_pos]);
					}

					if(cur_pos/(long unsigned int)w.ws_col > prev_cur_pos/(long unsigned int)w.ws_col && cur_pos%w.ws_col)
						fprintf(stdout,CURSOR_DOWN_N,(cur_pos/w.ws_col)-(prev_cur_pos/w.ws_col));
					break;
				case 'd':	//Shift+Left
					if(cur_pos == 0) break;
					for(long unsigned int i=cur_pos; input[i]!=' ' && i>0; i--)
					{
						cur_pos--;
						char_pos--;
						while(!getCharLength(input[char_pos])) char_pos--;
					}
					for(long unsigned int i=cur_pos; input[i]==' ' && i>0; i--)
					{
						cur_pos--;
						char_pos--;
						while(!getCharLength(input[char_pos])) char_pos--;
					}
					for(long unsigned int i=cur_pos; input[i]!=' ' && i>0; i--)
					{
						cur_pos--;
						char_pos--;
						while(!getCharLength(input[char_pos])) char_pos--;
					}
					if(cur_pos > 0)
					{
						cur_pos++;
						char_pos+=getCharLength(input[char_pos]);
					}

					if(cur_pos/(long unsigned int)w.ws_col < prev_cur_pos/(long unsigned int)w.ws_col)
						fprintf(stdout,CURSOR_UP_N,(prev_cur_pos/w.ws_col)-(cur_pos/w.ws_col));

					if(cur_pos && !(cur_pos % w.ws_col)) fprintf(stdout,CURSOR_UP);
					break;
				case 'H':	//Home
					if(cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
						char_pos = 0;
					}
					else
					{
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
						fprintf(stdout,CURSOR_UP);
						for(long unsigned int i=0; i<cur_pos%w.ws_col; i++)
						{
							char_pos--;
							while(!getCharLength(input[char_pos-i])) char_pos--;
						}
						cur_pos -= cur_pos%w.ws_col;
					}
					break;
				case '5':	//PgUp
					if(getchar() != '~' || cur_pos == 0) break;
					if(getInputDisplayLength(input) < w.ws_col)
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
					char_pos = 0;
					break;
				case '6':	//PgDown
					if(getchar() != '~' || cur_pos == input.size()) break;
					if(getInputDisplayLength(input) < w.ws_col)
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
					cur_pos = getInputDisplayLength(input);
					char_pos = input.size();
					break;
				case '7':	//Home
					if(getchar() != '~' || cur_pos == 0) break;
					if(cur_pos < w.ws_col)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
						char_pos = 0;
					}
					else
					{
						if(cur_pos % w.ws_col)
							fprintf(stdout,CURSOR_LEFT_N,cur_pos%w.ws_col);
						fprintf(stdout,CURSOR_UP);
						for(long unsigned int i=0; i<cur_pos%w.ws_col; i++)
						{
							char_pos--;
							while(!getCharLength(input[char_pos-i])) char_pos--;
						}
						cur_pos -= cur_pos%w.ws_col;
					}
					break;
				case 'F':	//End
					if(cur_pos == getInputDisplayLength(input) || !((cur_pos+1)%w.ws_col)) break;
					if((long unsigned int)(cur_pos+w.ws_col) > getInputDisplayLength(input))
					{
						cur_pos = getInputDisplayLength(input);
						char_pos = input.size();
					}
					else if(cur_pos < w.ws_col)
					{
						for(long unsigned int i=0; i<w.ws_col-1-cur_pos; i++)
							char_pos += getCharLength(input[char_pos]);
						cur_pos = w.ws_col-1;
					}
					else
					{
						for(long unsigned int i=0; i<w.ws_col-(cur_pos%w.ws_col)-1; i++)
							char_pos += getCharLength(input[char_pos]);
						cur_pos += w.ws_col-(cur_pos%w.ws_col)-1;
					}
					break;
				case '8':	//End
					if(getchar() != '~' || cur_pos == input.size() || !((cur_pos+1)%w.ws_col)) break;
					if((long unsigned int)(cur_pos+w.ws_col) > getInputDisplayLength(input))
					{
						cur_pos = getInputDisplayLength(input);
						char_pos = input.size();
					}
					else if(cur_pos < w.ws_col)
					{
						for(long unsigned int i=0; i<w.ws_col-1-cur_pos; i++)
							char_pos += getCharLength(input[char_pos]);
						cur_pos = w.ws_col-1;
					}
					else
					{
						for(long unsigned int i=0; i<w.ws_col-(cur_pos%w.ws_col)-1; i++)
							char_pos += getCharLength(input[char_pos]);
						cur_pos += w.ws_col-(cur_pos%w.ws_col)-1;
					}
					break;
				case '2':	//Insert
					if(getchar() == '~') insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() != '~' || cur_pos >= getInputDisplayLength(input)) break;
					for(long unsigned int i=char_pos; i<del_end; i++)
						input.erase(input.begin()+char_pos);
					break;
			}
		}
		else //Printable character, probably
		{
			if(insert_mode)	//If the "Insert" key is toggled
				input[char_pos] = k;
			else
				input.insert(input.begin()+char_pos,k);

			char_pos++;
			if(getCharLength(k)) cur_pos++;
		}

		//Reset terminal flags in-case of sudden program termination
		tcsetattr(fileno(stdin), TCSANOW, &t_old);
	}

	return 0;
}
