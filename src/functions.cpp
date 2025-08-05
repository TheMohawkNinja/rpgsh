#include <cassert>
#include <cstdarg>
#include <fcntl.h>
#include <map>
#include <regex>
#include <spawn.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include "../headers/colors.h"
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/scope.h"

void printHeader(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(long unsigned int i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
}

std::string defaultPrompt(Character c)
{
	return std::string(TEXT_BOLD)+std::string(TEXT_RED)+c.getName()+std::string(TEXT_WHITE)+"> "+std::string(TEXT_NORMAL);
}

void output(OutputLevel level, const char* format, ...)
{
	FILE* stream;
	std::string prefix = "";
	std::string color;
	va_list args;
	va_start(args, format);

	switch(level)
	{
		case info:
			stream = stdout;
			prefix = "%s%s[%sINFO%s]%s    ";
			color = TEXT_CYAN;
			break;
		case warning:
			stream = stderr;
			prefix = "%s%s[%sWARNING%s]%s ";
			color = TEXT_YELLOW;
			break;
		case error:
			stream = stderr;
			prefix = "%s%s[%sERROR%s]%s   ";
			color = TEXT_RED;
			break;
	}
	fprintf(stream,prefix.c_str(),TEXT_BOLD,TEXT_WHITE,color.c_str(),TEXT_WHITE,color.c_str());
	vfprintf(stream,format,args);
	fprintf(stream,"%s\n",TEXT_NORMAL);
}

std::string makePretty(std::string value)
{
	Character c = Character();

	if(value == "") return empty_str.c_str();

	std::map<std::string,const char*> ansi_esc_seqs = {
	{"/",			TEXT_NORMAL},
	{"b",			TEXT_BOLD},
	{"/b",			TEXT_NOBOLD},
	{"i",			TEXT_ITALIC},
	{"/i",			TEXT_NOITALIC},
	{"d",			TEXT_DIM},
	{"/d",			TEXT_NORMALINTENSITY},
	{"u",			TEXT_UNDERLINE},
	{"/u",			TEXT_NOUNDERLINE},
	{"blink",		TEXT_BLINK},
	{"/blink",		TEXT_NOBLINK},
	{"r",			TEXT_REVERSE},
	{"/r",			TEXT_NOREVERSE},
	{"/fg",			TEXT_DEFAULTCOLOR},
	{"black",		TEXT_BLACK},
	{"/black",		TEXT_DEFAULTCOLOR},
	{"red",			TEXT_RED},
	{"/red",		TEXT_DEFAULTCOLOR},
	{"green",		TEXT_GREEN},
	{"/green",		TEXT_DEFAULTCOLOR},
	{"yellow",		TEXT_YELLOW},
	{"/yellow",		TEXT_DEFAULTCOLOR},
	{"blue",		TEXT_BLUE},
	{"/blue",		TEXT_DEFAULTCOLOR},
	{"magenta",		TEXT_MAGENTA},
	{"/magenta",		TEXT_DEFAULTCOLOR},
	{"cyan",		TEXT_CYAN},
	{"/cyan",		TEXT_DEFAULTCOLOR},
	{"lightgray",		TEXT_LIGHTGRAY},
	{"/lightgray",		TEXT_DEFAULTCOLOR},
	{"darkgray",		TEXT_DARKGRAY},
	{"/darkgray",		TEXT_DEFAULTCOLOR},
	{"lightred",		TEXT_LIGHTRED},
	{"/lightred",		TEXT_DEFAULTCOLOR},
	{"lightgreen",		TEXT_LIGHTGREEN},
	{"/lightgreen",		TEXT_DEFAULTCOLOR},
	{"lightyellow",		TEXT_LIGHTYELLOW},
	{"/lightyellow",	TEXT_DEFAULTCOLOR},
	{"lightblue",		TEXT_LIGHTBLUE},
	{"/lightblue",		TEXT_DEFAULTCOLOR},
	{"lightmagenta",	TEXT_LIGHTMAGENTA},
	{"/lightmagenta",	TEXT_DEFAULTCOLOR},
	{"lightcyan",		TEXT_LIGHTCYAN},
	{"/lightcyan",		TEXT_DEFAULTCOLOR},
	{"white",		TEXT_WHITE},
	{"/white",		TEXT_DEFAULTCOLOR},
	{"/bg",			TEXT_BG_DEFAULTCOLOR},
	{"bgblack",		TEXT_BG_BLACK},
	{"/bgblack",		TEXT_BG_DEFAULTCOLOR},
	{"bgred",		TEXT_BG_RED},
	{"/bgred",		TEXT_BG_DEFAULTCOLOR},
	{"bggreen",		TEXT_BG_GREEN},
	{"/bggreen",		TEXT_BG_DEFAULTCOLOR},
	{"bgyellow",		TEXT_BG_YELLOW},
	{"/bgyellow",		TEXT_BG_DEFAULTCOLOR},
	{"bgblue",		TEXT_BG_BLUE},
	{"/bgblue",		TEXT_BG_DEFAULTCOLOR},
	{"bgmagenta",		TEXT_BG_MAGENTA},
	{"/bgmagenta",		TEXT_BG_DEFAULTCOLOR},
	{"bgcyan",		TEXT_BG_CYAN},
	{"/bgcyan",		TEXT_BG_DEFAULTCOLOR},
	{"bglightgray",		TEXT_BG_LIGHTGRAY},
	{"/bglightgray",	TEXT_BG_DEFAULTCOLOR},
	{"bgdarkgray",		TEXT_BG_DARKGRAY},
	{"/bgdarkgray",		TEXT_BG_DEFAULTCOLOR},
	{"bglightred",		TEXT_BG_LIGHTRED},
	{"/bglightred",		TEXT_BG_DEFAULTCOLOR},
	{"bglightgreen",	TEXT_BG_LIGHTGREEN},
	{"/bglightgreen",	TEXT_BG_DEFAULTCOLOR},
	{"bglightyellow",	TEXT_BG_LIGHTYELLOW},
	{"/bglightyellow",	TEXT_BG_DEFAULTCOLOR},
	{"bglightblue",		TEXT_BG_LIGHTBLUE},
	{"/bglightblue",	TEXT_BG_DEFAULTCOLOR},
	{"bglightmagenta",	TEXT_BG_LIGHTMAGENTA},
	{"/bglightmagenta",	TEXT_BG_DEFAULTCOLOR},
	{"bglightcyan",		TEXT_BG_LIGHTCYAN},
	{"/bglightcyan",	TEXT_BG_DEFAULTCOLOR},
	{"bgwhite",		TEXT_BG_WHITE},
	{"/bgwhite",		TEXT_BG_DEFAULTCOLOR}};

	for(long unsigned int i=0; i<value.length(); i++)
	{
		if(value[i] == '\\' && i == value.length())
		{
			value.erase(value.begin()+i);
		}
		else if(value[i] == '\\')
		{
			switch(value[i+1])
			{
				case 'a':
					value[i+1] = '\a'; //Bell
					break;
				case 'b':
					value[i+1] = '\b'; //Backspace
					break;
				case 'f':
					value[i+1] = '\f'; //Form feed
					break;
				case 'n':
					value[i+1] = '\n'; //Newline
					break;
				case 't':
					value[i+1] = '\t'; //Horizontal tab
					break;
				case 'v':
					value[i+1] = '\v'; //Vertical tab
					break;
			}
			value.erase(value.begin()+i);
		}
	}

	for(const auto& [k,v] : ansi_esc_seqs)
		value = std::regex_replace(value,std::regex("%"+k+"%",std::regex_constants::icase),std::string(v));

	value = std::regex_replace(value,std::regex("%name%",std::regex_constants::icase),c.getName());

	std::regex fg_rgb_color_pattern("%fg=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}%");
	std::regex fg_html_color_pattern("%fg=[a-zA-Z]{1,}%");
	std::regex bg_rgb_color_pattern("%bg=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}%");
	std::regex bg_html_color_pattern("%bg=[a-zA-Z]{1,}%");

	std::sregex_iterator fg_rgb_it(value.begin(),value.end(),fg_rgb_color_pattern);
	std::sregex_iterator fg_html_it(value.begin(),value.end(),fg_html_color_pattern);
	std::sregex_iterator bg_rgb_it(value.begin(),value.end(),bg_rgb_color_pattern);
	std::sregex_iterator bg_html_it(value.begin(),value.end(),bg_html_color_pattern);

	std::sregex_iterator it_end;

	std::map<std::string,std::string> string_replacements;

	while(fg_rgb_it != it_end)
	{
		std::string rgb_str = fg_rgb_it->str();
		std::regex color_channels_pattern("[0-9]{1,3}");
		std::sregex_iterator color_channels_it(rgb_str.begin(),rgb_str.end(),color_channels_pattern);
		unsigned char r = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char g = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char b = (char)std::stoi(color_channels_it->str());

		string_replacements[rgb_str] = std::string(TEXT_FG_8BIT_PRE)+std::to_string(r)+";"+std::to_string(g)+";"+std::to_string(b)+"m";
		fg_rgb_it++;
	}
	while(fg_html_it != it_end)
	{
		std::string html_str = fg_html_it->str().substr(4,fg_html_it->str().length()-5);//Pull out just the color name
		Colors colors = Colors();
		RGB cc = colors.as24Bit(html_str);

		string_replacements[fg_html_it->str()] = std::string(TEXT_FG_8BIT_PRE)+std::to_string(cc.r)+";"+std::to_string(cc.g)+";"+std::to_string(cc.b)+"m";
		fg_html_it++;
	}
	while(bg_rgb_it != it_end)
	{
		std::string rgb_str = bg_rgb_it->str();
		std::regex color_channels_pattern("[0-9]{1,3}");
		std::sregex_iterator color_channels_it(rgb_str.begin(),rgb_str.end(),color_channels_pattern);
		unsigned char r = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char g = (char)std::stoi(color_channels_it->str());
		color_channels_it++;
		unsigned char b = (char)std::stoi(color_channels_it->str());

		string_replacements[rgb_str] = std::string(TEXT_BG_8BIT_PRE)+std::to_string(r)+";"+std::to_string(g)+";"+std::to_string(b)+"m";
		bg_rgb_it++;
	}
	while(bg_html_it != it_end)
	{
		std::string html_str = bg_html_it->str().substr(4,bg_html_it->str().length()-5);//Pull out just the color name
		Colors colors = Colors();
		RGB cc = colors.as24Bit(html_str);

		string_replacements[bg_html_it->str()] = std::string(TEXT_BG_8BIT_PRE)+std::to_string(cc.r)+";"+std::to_string(cc.g)+";"+std::to_string(cc.b)+"m";
		bg_html_it++;
	}

	for(const auto& [k,v] : string_replacements) value = std::regex_replace(value,std::regex(k),v);
	value = std::regex_replace(value,std::regex("%"),"%%");//Prevent printf() parsing '%' as format specifiers

	value += std::string(TEXT_NORMAL); //Make sure we don't carry over any unterminated formatting
	return value;
}
std::string stripFormatting(std::string str)
{
	for(unsigned long int i=0; i<str.length(); i++)
	{
		const std::string::iterator cur_it = str.begin()+i;
		if(str[i] == ESC_SEQ)
		{
			str.erase(cur_it,str.begin()+str.find('m',i)+1);
			i--;
		}
		else if(str[i] == '\b' && i)
		{
			str.erase(cur_it-1,cur_it-1);
		}
	}
	return str;
}

bool stob(std::string s)
{
	return !stringcasecmp(s,"true");
}
std::string btos(bool b)
{
	if(b) return "True";
	return "False";
}

bool isScopeSigil(char c)
{
	return (c == CHARACTER_SIGIL ||
		c == CAMPAIGN_SIGIL ||
		c == SHELL_SIGIL);
}
bool isTypeSigil(char c)
{
	return (c == CURRENCY_SIGIL ||
		c == DICE_SIGIL ||
		c == VAR_SIGIL ||
		c == WALLET_SIGIL);
}

bool looksLikeSet(std::string s)
{
	std::regex set_pattern(set_pattern_str);
	std::sregex_iterator match_it(s.begin(), s.end(), set_pattern);
	std::sregex_iterator match_end;

	if(match_it != match_end)
		return match_it->str() == s;
	else
		return false;
}
bool looksLikeVariable(std::string s)
{
	std::regex variable_pattern(variable_pattern_str);
	std::sregex_iterator match_it(s.begin(), s.end(), variable_pattern);
	std::sregex_iterator match_end;

	if(match_it != match_end)
		return match_it->str() == s;
	else
		return false;
}

bool isEscaped(std::string str, long unsigned int pos)
{
	if(pos <= 0 || pos >= str.length()) return false;
	else return str[pos-1] == '\\';
}

int stringcasecmp(std::string a, std::string b)
{
	return strcasecmp(a.c_str(),b.c_str());
}

std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> list;
	if(findu(str,delimiter) == std::string::npos)
	{
		list.push_back(str);
		return list;
	}

	while(true)
	{
		if(findu(str,delimiter) == std::string::npos)
		{
			list.push_back(str);
			return list;
		}

		list.push_back(left(str,findu(str,delimiter)));
		str = right(str,findu(str,delimiter)+1);
	}

	return list;
}

long unsigned int findu(std::string str, std::string match, long unsigned int start)
{
	if(match.length() > str.length()) return std::string::npos;
	for(long unsigned int i=start; i<str.length()-(match.length()-1); i++)
		if(str.substr(i,match.length()) == match && !isEscaped(str.substr(i,match.length()),i)) return i;

	return std::string::npos;
}
long unsigned int findu(std::string str, char ch, long unsigned int start)
{
	return findu(str, std::string(1,ch), start);
}
long unsigned int rfindu(std::string str, std::string match, long unsigned int start)
{
	if(match.length() > str.length()) return std::string::npos;
	if(start == UINT_MAX) start = str.length();

	bool last_check = false;
	for(long unsigned int i=start-match.length(); i>0; i--)
	{
		if(str.substr(i-last_check,match.length()) == match &&
		   !isEscaped(str.substr(i-last_check,match.length()),i-last_check)) return i-last_check;
		else if(i == 1 && !last_check)
		{
			i++;
			last_check = true;
		}
	}

	return std::string::npos;
}
long unsigned int rfindu(std::string str, char ch, long unsigned int start)
{
	return rfindu(str, std::string(1,ch), start);
}

std::string left(std::string str, int n)
{
	return str.substr(0,n);
}
std::string right(std::string str, int n)
{
	return str.substr(n,str.length()-n);
}

long unsigned int countu(std::string str, char ch)
{
	long unsigned int count = 0;
	for(long unsigned int i=0; i<str.length(); i++)
		if(str[i] == ch && !isEscaped(str,i)) count++;

	return count;
}

long unsigned int getDisplayLength(std::string str)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int len = 0;

	for(long unsigned int i = 0; i<str.length(); i++)
	{
		unsigned char c = static_cast<unsigned char>(str[i]);

		if(str[i] == ESC_SEQ)
			i = str.find('m',i);
		else if(str[i] == '\t' && (len+(8-(len%8)))/w.ws_col == len/w.ws_col)
			len += 8-(len%8);
		else if(str[i] == '\t' && (len+(8-(len%8)))/w.ws_col > len/w.ws_col)
			len += w.ws_col-len-1;
		else if(str[i] == '\b')
			len--;
		else if(c < 0x80 || (c&0xe0) == 0xc0 || (c&0xf0) == 0xe0 || (c&0xf8) == 0xf0)
			len++;
	}
	return len;
}
long unsigned int getInputDisplayLength(std::vector<char> input)
{
	std::string str;
	for(const auto& ch : input) str += ch;

	return getDisplayLength(str);
}
int getCharLength(char c)
{
	unsigned char uc = static_cast<unsigned char>(c);
	if(uc < 0x80) return 1;
	else if((uc&0xe0) == 0xc0) return 2;
	else if((uc&0xf0) == 0xe0) return 3;
	else if((uc&0xf8) == 0xf0) return 4;
	else return 0;
}

bool containsNonSpaceChar(std::string str)
{
	for(const auto& ch : str)
		if(ch != ' ') return true;

	return false;
}
void addKeyToMatches(std::vector<std::string>** ppMatches, std::string k, std::string chk_str,
		     std::string key)
{
	//Limit keys to one level past current
	if(!stringcasecmp(left(k,key.length()),key) && k[0] != '.')
	{
		long unsigned int key_last_slash = rfindu(key,'/');
		long unsigned int k_next_slash = findu(k,'/',key_last_slash+1);
		if(k_next_slash < key.length())
		{
			(**ppMatches).push_back(addSpaces(chk_str.length())+right(k,key.length()));
		}
		else if(k_next_slash > key.length())
		{
			std::string potential_match = left(right(k,key.length()),(k_next_slash-key.length()));
			potential_match = addSpaces(chk_str.length())+potential_match;
			for(const auto& match : (**ppMatches))//Ensure match list is unique
				if(match == potential_match) return;

			(**ppMatches).push_back(potential_match);
		}
	}
}
void addKeysToMatches(std::vector<std::string>* pMatches, Scope scope, std::string chk_str,
		      std::string key, char type)
{
	switch(type)
	{
		case CURRENCY_SIGIL:
			for(auto& [k,v] : scope.getDatamap<Currency>())
				addKeyToMatches(&pMatches,k,chk_str,key);
			break;
		case DICE_SIGIL:
			for(auto& [k,v] : scope.getDatamap<Dice>())
				addKeyToMatches(&pMatches,k,chk_str,key);
			break;
		case VAR_SIGIL:
			for(auto& [k,v] : scope.getDatamap<Var>())
				addKeyToMatches(&pMatches,k,chk_str,key);
			break;
		case WALLET_SIGIL:
			for(auto& [k,v] : scope.getDatamap<Wallet>())
				addKeyToMatches(&pMatches,k,chk_str,key);
			break;
	}
}
void addPropertyToMatches(std::vector<std::string>** ppMatches, std::string chk_str,
			  std::string property,std::string property_str)
{
	if(!stringcasecmp(property,left(property_str,property.length())))
		(**ppMatches).push_back(addSpaces(chk_str.length())+right(property_str,property.length()));
}
void addPropertiesToMatches(std::vector<std::string>* pMatches, Scope scope, std::string chk_str,
			    std::string k, std::string key, std::string property, char type)
{
	if(stringcasecmp(k,key)) return;

	if(type == VAR_SIGIL && scope.keyExists<Var>(key))
	{
		addPropertyToMatches(&pMatches,chk_str,property,"Value");
	}
	else if(type == DICE_SIGIL && scope.keyExists<Dice>(key))
	{
		addPropertyToMatches(&pMatches,chk_str,property,"Quantity");
		addPropertyToMatches(&pMatches,chk_str,property,"Faces");
		addPropertyToMatches(&pMatches,chk_str,property,"Modifier");
	}
	else if(type == WALLET_SIGIL && scope.keyExists<Wallet>(key))
	{
		for(auto& m : scope.get<Wallet>(key).Monies)
			addPropertyToMatches(&pMatches,chk_str,property,std::string(m.c));
	}
	else if(type == CURRENCY_SIGIL && scope.keyExists<Currency>(key))
	{
		addPropertyToMatches(&pMatches,chk_str,property,"CurrencySystem");
		addPropertyToMatches(&pMatches,chk_str,property,"Name");
		addPropertyToMatches(&pMatches,chk_str,property,"SmallerAmount");
		addPropertyToMatches(&pMatches,chk_str,property,"Smaller");
		addPropertyToMatches(&pMatches,chk_str,property,"Larger");
	}
}
void moveCursorBack(winsize w, long unsigned int start, long unsigned int end)
{
	fprintf(stdout,CURSOR_HIDE);
	for(long unsigned int i=start; i>end; i--)
	{
		if(i && !(i%w.ws_col))
		{
			fprintf(stdout,CURSOR_UP);
			fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)w.ws_col);
		}
		else
		{
			fprintf(stdout,CURSOR_LEFT);
		}
	}
	fprintf(stdout,CURSOR_SHOW);
}
void moveCursorForward(winsize w, long unsigned int start, long unsigned int end)
{
	fprintf(stdout,CURSOR_HIDE);
	for(long unsigned int i=start; i<end; i++)
	{
		if(i && !(i%w.ws_col))
		{
			fprintf(stdout,CURSOR_DOWN);
			fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)0);
		}
		else
		{
			fprintf(stdout,CURSOR_RIGHT);
		}
	}
	fprintf(stdout,CURSOR_SHOW);
}
void inputHandler(std::string* pInput, long unsigned int offset)
{
	//Set terminal flags for non-buffered reading required for handling keyboard input
	struct termios t_old, t_new;
	tcgetattr(fileno(stdin), &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(fileno(stdin), TCSANOW, &t_new);

	//Get terminal dimensions
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	bool insert_mode = false;
	char k = 0;
	char esc_char = 0;
	int tab_ctr = 0;
	std::string last_match, last_history;
	std::vector<std::string> history = getAppOutput("history").output;
	long unsigned int cur_pos = 0;
	//long unsigned int char_pos = 0;
	long unsigned int history_len = history.size();
	long unsigned int history_ctr = history_len-1;
	long unsigned int combined_offset = offset+last_history.length();

	while(k != KB_ENTER)
	{
		esc_char = 0;
		k = getchar();

		if(k == ESC_SEQ)
		{
			getchar();//Consume '['
			esc_char = getchar();
		}

		//Reset tab completion variables
		if(k != KB_TAB && esc_char != 'Z')//Z for shift+tab
		{
			if(containsNonSpaceChar(last_match) && (*pInput).find(' ',cur_pos) == std::string::npos)
				cur_pos += ((*pInput).size()-cur_pos);
			else if(containsNonSpaceChar(last_match))
				cur_pos = (*pInput).find(' ',cur_pos) == std::string::npos;
			tab_ctr = 0;
			last_match = "";
		}

		//Reset history ctr
		if(k != ESC_SEQ || (esc_char != 'A' && esc_char != 'B'))
			history_ctr = history_len-1;

		if(isprint(k))//Printable characters
		{
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < (*pInput).size()) (*pInput)[cur_pos] = k;
				else (*pInput).push_back(k);
			}
			else
			{
				(*pInput).insert((*pInput).begin()+cur_pos,k);
			}

			fprintf(stdout,CLEAR_TO_SCREEN_END);
			for(long unsigned int i=cur_pos; i<(*pInput).size(); i++)
				fprintf(stdout,"%c",(*pInput)[i]);

			if(cur_pos < (*pInput).size()-1)
				moveCursorBack(w,(*pInput).size()+offset,cur_pos+offset+1);

			cur_pos++;
		}
		else if(k == KB_BACKSPACE && cur_pos > 0)//Backspace
		{
			fprintf(stdout,"\b%s",CLEAR_TO_LINE_END);

			cur_pos--;
			(*pInput).erase((*pInput).begin()+cur_pos);

			for(long unsigned int i=cur_pos; i<(*pInput).size(); i++)
				fprintf(stdout,"%c",(*pInput)[i]);

			if(cur_pos < (*pInput).size())
				moveCursorBack(w,(*pInput).size()+offset,cur_pos+offset);
		}
		else if((k == KB_TAB || esc_char == 'Z') && (*pInput).size() &&
			(cur_pos == (*pInput).size() || (*pInput)[cur_pos] == ')' || (*pInput)[cur_pos] == ' ' || last_match != ""))//Tab (completion)
		{
			if(k == KB_TAB) tab_ctr++;
			else tab_ctr--;

			Scope tab_comp_scope;
			std::string match = "";
			std::vector<std::string> matches;

			//Create the string which is to be completed
			std::string chk_str = "";
			for(int i=cur_pos-1; i>=0&&(*pInput)[i]!=' '; i--)
				chk_str.insert(0,std::string(1,(*pInput)[i]));

			if(chk_str[0] == '(') chk_str.erase(chk_str.begin());

			if(isalpha(chk_str[0]))//Applications
			{
				//Get matches
				std::string app = "";
				std::ifstream ifs(rpgsh_programs_cache_path);
				while(!ifs.eof())
				{
					getline(ifs,app);
					if(app == "") break;

					std::string app_chk_str = app.substr(prefix.length(),
									     chk_str.length());
					long unsigned int app_ln = right(app,prefix.length()).length();
					if(app_chk_str == chk_str && app_ln > chk_str.length())
						matches.push_back(right(app,prefix.length()));
				}
				ifs.close();
			}
			else if(chk_str[0] == CHARACTER_SIGIL)//Character variables
			{
				tab_comp_scope = Character();
				std::string xref_path = campaigns_dir+
							getEnvVariable(ENV_CURRENT_CAMPAIGN)+
							"characters/";

				if(chk_str[1] == '[' &&
				   findu(chk_str,']') == std::string::npos &&
				   findu(chk_str,'/') == std::string::npos)//Complete xref (same campaign)
				{
					for(auto& c : getDirectoryListing(xref_path))
					{
						if(right(c,c.length()-5) != c_ext)//Don't include backup files or non-character files
							continue;

						std::string xref = left(c,findu(c,c_ext));
						std::string chk_str_xref = right(chk_str,2);
						if(chk_str.length() == 2 ||
						   (!stringcasecmp(chk_str_xref,left(xref,chk_str.length()-2)) &&
						   xref.length() > chk_str_xref.length()))
							matches.push_back("  "+xref+"]");//Two spaces are sacrificial for formatting
					}
					for(auto& m : getDirectoryListing(campaigns_dir))
					{
						std::string chk_str_xref = right(chk_str,2);
						if(std::filesystem::is_directory(campaigns_dir+m) &&
						   (chk_str.length() == 2 ||
						   (!stringcasecmp(chk_str_xref,left(m,chk_str.length()-2)) &&
						   m.length() > chk_str_xref.length())))
							matches.push_back("  "+m+"/");//Two spaces are sacrificial for formatting
					}
				}
				else if(chk_str[1] == '[' &&
					findu(chk_str,']') == std::string::npos)//Complete xref (different campaign)
				{
					//Get campaign name
					std::string campaign = right(chk_str,findu(chk_str,'[')+1);
					campaign = left(campaign,findu(campaign,'/'));

					for(auto& m : getDirectoryListing(campaigns_dir))
					{
						if(!stringcasecmp(campaign,m))
						{
							campaign = m;
							break;
						}
					}

					std::string xref_path = campaigns_dir+
								campaign+"/"+
								"characters/";

					if(!std::filesystem::exists(xref_path) || !std::filesystem::is_directory(xref_path))
						continue;

					for(auto& c : getDirectoryListing(xref_path))
					{
						if(right(c,c.length()-5) != c_ext)//Don't include backup files or non-character files
							continue;

						std::string xref = left(c,findu(c,c_ext));
						std::string chk_str_xref = right(chk_str,findu(chk_str,'/')+1);
						if(!stringcasecmp(chk_str_xref,left(xref,chk_str.length()-findu(chk_str,'/')-1)) &&
						   xref.length() > chk_str_xref.length())
						{
							matches.push_back(addSpaces(chk_str.length()-(chk_str.length()-findu(chk_str,'/'))+1)+
									  xref+"]");//Padding is sacrificial for formatting
						}
					}
				}
				else if(chk_str[1] == '[' &&
					findu(chk_str,']') != std::string::npos)//Load xref
				{
					if(findu(chk_str,'/') != std::string::npos &&
					   findu(chk_str,'/') > findu(chk_str,']'))//Same campaign
					{
						std::string xref_path = campaigns_dir+
									getEnvVariable(ENV_CURRENT_CAMPAIGN)+
									"characters/"+
									right(left(chk_str,findu(chk_str,']')),findu(chk_str,'[')+1)+
									c_ext;

						if(std::filesystem::exists(xref_path) && !std::filesystem::is_directory(xref_path))
							tab_comp_scope.load(xref_path);

					}
					else if(findu(chk_str,'/') != std::string::npos &&
					   findu(chk_str,'/') < findu(chk_str,']'))//Different campaign
					{
						std::string xref_path = campaigns_dir+
									right(left(chk_str,findu(chk_str,'/')+1),findu(chk_str,'[')+1)+
									"characters/"+
									right(left(chk_str,findu(chk_str,']')),findu(chk_str,'/')+1)+
									c_ext;

						if(std::filesystem::exists(xref_path) && !std::filesystem::is_directory(xref_path))
							tab_comp_scope.load(xref_path);
					}
				}
			}
			else if(chk_str[0] == CAMPAIGN_SIGIL)//Campaign variables
			{
				tab_comp_scope = Campaign();
				std::string xref_path = campaigns_dir;

				if(chk_str[1] == '[' &&
				   findu(chk_str,']') == std::string::npos)//Complete xref
				{
					for(auto& m : getDirectoryListing(campaigns_dir))
					{
						std::string chk_str_xref = right(chk_str,2);
						if(chk_str.length() == 2 ||
						   (!stringcasecmp(chk_str_xref,left(m,chk_str.length()-2)) &&
						   m.length() > chk_str_xref.length()))
							matches.push_back("  "+m+"]");//Two spaces are sacrificial for formatting
					}
				}
				else if(chk_str[1] == '[' &&
					findu(chk_str,']') != std::string::npos)//Load xref
				{
					std::string xref_path = campaigns_dir+
								right(left(chk_str,findu(chk_str,']')),findu(chk_str,'[')+1)+
								variables_file_name;

					if(std::filesystem::exists(xref_path) && !std::filesystem::is_directory(xref_path))
						tab_comp_scope.load(xref_path);
				}
			}
			else if(chk_str[0] == SHELL_SIGIL)//Shell variables
			{
				tab_comp_scope = Shell();
			}

			int slash = findu(chk_str,'/',findu(chk_str,']')+1);
			int rsqbrkt = findu(chk_str,']');
			int period = rfindu(chk_str,'.');
			char type_sigil = '\0';
			if(findu(chk_str,'/') != std::string::npos)
				type_sigil = chk_str[findu(chk_str,'/',findu(chk_str,']')+1)-1];
			std::string key = right(chk_str,findu(chk_str,'/',findu(chk_str,']')+1)+1);
			if(slash > rsqbrkt && period < slash)//Keys
			{
				if(isTypeSigil(type_sigil))
				{
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,type_sigil);
				}
				else if(isScopeSigil(type_sigil) || type_sigil == ']')
				{
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,CURRENCY_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,DICE_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,VAR_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,WALLET_SIGIL);
				}
			}
			else if(slash > rsqbrkt)//Properties
			{
				key = left(key,rfindu(key,'.'));
				std::string property = right(chk_str,findu(chk_str,'.')+1);
				if(isTypeSigil(type_sigil))
				{
					addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,key,property,type_sigil);
				}
				else if(isScopeSigil(type_sigil) || type_sigil == ']')
				{
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Var>())
						addPropertiesToMatches(&matches,tab_comp_scope,chk_str,k,key,property,VAR_SIGIL);
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Dice>())
						addPropertiesToMatches(&matches,tab_comp_scope,chk_str,k,key,property,DICE_SIGIL);
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Wallet>())
						addPropertiesToMatches(&matches,tab_comp_scope,chk_str,k,key,property,WALLET_SIGIL);
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Currency>())
						addPropertiesToMatches(&matches,tab_comp_scope,chk_str,k,key,property,CURRENCY_SIGIL);
				}
			}

			if(!matches.size()) continue;

			//Sort and choose match
			sort<std::string>(&matches);
			match = matches[(tab_ctr-1)%matches.size()];

			//Erase any previous match
			if(last_match != "")
			{
				for(long unsigned int i=0; i<last_match.length()-chk_str.length(); i++)
					(*pInput).erase((*pInput).begin()+cur_pos);
			}

			//Insert match into (*pInput)
			for(long unsigned int i=0; i<match.length()-chk_str.length(); i++)
				(*pInput).insert((*pInput).begin()+cur_pos+i,match[i+chk_str.length()]);

			//Reprint (*pInput)
			if(last_match != "" && match != "" && containsNonSpaceChar(match))
			{
				moveCursorBack(w,last_match.length()-chk_str.length());
				fprintf(stdout,CLEAR_TO_SCREEN_END);
			}

			for(long unsigned int i=cur_pos; i<(*pInput).size(); i++)
				fprintf(stdout,"%c",(*pInput)[i]);

			//CURSOR_LEFT_N(0) still pushes cursor to the left, so we need to check
			unsigned int spaces = 0;
			for(const auto& i : match) if(i == ' ') spaces++;
			unsigned int offset = match.length()-spaces;
			char base_char = (rfindu(chk_str,'/') == std::string::npos ? '[' : '/');
			if(cur_pos+offset < (*pInput).size())
				fprintf(stdout,CURSOR_LEFT_N,(*pInput).size()-cur_pos-offset+(chk_str.length()-(rfindu(chk_str,base_char)+1)));

			last_match = match;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			switch(esc_char)
			{
				case 'A':	//Up
				case 'B':	//Down
					if(cur_pos > 0) fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					if(combined_offset>=w.ws_col)
					{
						fprintf(stdout,CURSOR_UP_N,combined_offset/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,offset);
					}
					fprintf(stdout,CLEAR_TO_SCREEN_END);
					(*pInput).clear();
					if(esc_char == 'B' && history_ctr == history_len-1)
					{
						cur_pos = 0;
						break;
					}
					if     (esc_char == 'A' && history_ctr > 0)		history_ctr--;
					else if(esc_char == 'B' && history_ctr < history_len-1)	history_ctr++;
					for(const auto& c : history[history_ctr])
						(*pInput).push_back(c);
					fprintf(stdout,"%s",history[history_ctr].c_str());
					last_history = history[history_ctr];
					cur_pos = (*pInput).size();
					combined_offset = offset+last_history.length();
					break;
				case 'C':	//Right
					if(cur_pos >= (*pInput).size()) continue;
					cur_pos++;
					moveCursorForward(w,offset+cur_pos,offset+cur_pos+1);
					break;
				case 'D':	//Left
					if(cur_pos == 0) continue;
					cur_pos--;
					moveCursorBack(w,offset+cur_pos+1,offset+cur_pos);
					break;
				case 'H':	//Home
					if(cur_pos == 0) continue;
					if(cur_pos+offset>=w.ws_col)
					{
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)0);
						fprintf(stdout,CURSOR_UP_N,(cur_pos+offset)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,offset);
					}
					else
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					}
					cur_pos = 0;
					break;
				case '5':	//PgUp
				case '6':	//PgDown
					if(getchar() != '~') continue;
					if(cur_pos > 0) fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					fprintf(stdout,CLEAR_TO_LINE_END);
					(*pInput).clear();
					if     (esc_char == '5') history_ctr = 0;
					else if(esc_char == '6') history_ctr = history_len-2;
					for(const auto& c : history[history_ctr])
					{
						(*pInput).push_back(c);
						fprintf(stdout,"%c",c);
					}
					cur_pos = (*pInput).size();
					break;
				case '7':	//Home
					if(getchar() != '~' || cur_pos == 0) continue;
					if(cur_pos+offset>=w.ws_col)
					{
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)0);
						fprintf(stdout,CURSOR_UP_N,(cur_pos+offset)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,offset);
					}
					else
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					}
					cur_pos = 0;
					break;
				case 'F':	//End
					if(cur_pos >= (*pInput).size()) continue;
					if((*pInput).size()-cur_pos>=w.ws_col)
					{
						fprintf(stdout,CURSOR_DOWN_N,((*pInput).size()-cur_pos)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,((*pInput).size()-cur_pos)%w.ws_col);
					}
					else
					{
						fprintf(stdout,CURSOR_RIGHT_N,(*pInput).size()-cur_pos);
					}
					cur_pos = (*pInput).size();
					break;
				case '8':	//End
					if(getchar() != '~' || cur_pos >= (*pInput).size()) continue;
					if((*pInput).size()-cur_pos>=w.ws_col)
					{
						fprintf(stdout,CURSOR_DOWN_N,((*pInput).size()-cur_pos)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,((*pInput).size()-cur_pos)%w.ws_col);
					}
					else
					{
						fprintf(stdout,CURSOR_RIGHT_N,(*pInput).size()-cur_pos);
					}
					cur_pos = (*pInput).size();
					break;
				case '2':	//Insert
					if(getchar() == '~') insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() != '~' || cur_pos >= (*pInput).size()) continue;
					fprintf(stdout,CLEAR_TO_SCREEN_END);
					(*pInput).erase((*pInput).begin()+cur_pos);
					for(long unsigned int i=cur_pos; i<(*pInput).size(); i++)
						fprintf(stdout,"%c",(*pInput)[i]);
					if(cur_pos < (*pInput).size())
						moveCursorBack(w,(*pInput).size()-cur_pos);
					if(!((cur_pos+offset+1)%w.ws_col))
					{
						fprintf(stdout,CURSOR_UP);
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)w.ws_col);
					}
					break;
			}
		}
	}
	//Reset terminal flags
	tcsetattr(fileno(stdin), TCSANOW, &t_old);

}

std::string addSpaces(unsigned int n)
{
	std::string ret = "";
	for(unsigned int i=0; i<n; i++) ret += " ";

	return ret;
}

std::string mergeQuotes(std::string str)
{
	std::string ret = "";

	// When running a program, rpgsh should already cut off any charaters outside of unescaped quotation marks.
	// Therefore, we shouldn't need to check for odd inputs

	// If neither end has quotes, just return the string as is.
	if(!str.length() || (str.front() != '\"' && (str.back() != '\"' || isEscaped(str,str.length()-1))))
		return str;

	// Strip unescaped quote marks out from anywhere else but the ends of the string
	for(long unsigned int i=0; i<str.length(); i++)
	{
		if(str[i] != '\"' || (str[i] == '\"' && isEscaped(str,i)))
			ret += str[i];
	}

	// If the resulting string isn't a valid integer, return the quote-stripped string
	if(!Var(ret).isInt())
		return ret;

	// The string is a valid integer, and also had quotes around it, so it still should be reated as a string
	return '\"'+ret+'\"';
}

std::string escapeSpaces(std::string str)
{
	std::string ret = "";

	for(long unsigned int i=0; i<str.length(); i++)
	{
		if(str[i] == ' ' && !isEscaped(str,i))
			ret += "\\ ";
		else
			ret += str[i];
	}

	return ret;
}
std::string escapeRegexGroupChars(std::string str)
{
	std::vector<std::string> patterns = {"\\[","\\]","\\(","\\)","\\{","\\}"};
	for(const auto& p : patterns)
		str = std::regex_replace(str,std::regex(p),p);

	return str;
}

void confirmEnvVariablesFile()
{
	if(std::filesystem::exists(rpgsh_env_variables_path.c_str())) return;

	output(info,"Environment variables file not found, creating file at \"%s\".",rpgsh_env_variables_path.c_str());
	std::ofstream ofs(rpgsh_env_variables_path.c_str());
	ofs.close();

	//Set default values for built-in env variables
	Config config = Config();
	Character c = Character(templates_dir + config.setting[DEFAULT_GAME].c_str());
	setEnvVariable(ENV_CURRENT_CHARACTER,c.getName());
	setEnvVariable(ENV_CURRENT_CAMPAIGN,"default/");
}

void confirmShellVariablesFile()
{
	if(std::filesystem::exists(shell_variables_path.c_str())) return;

	output(info,"Shell variables file not found, creating file at \"%s\".",shell_variables_path.c_str());
	std::ofstream ofs(shell_variables_path.c_str());
	ofs.close();
}

void confirmCampaignVariablesFile()
{
	std::string campaign_variables_file = campaigns_dir +
					      getEnvVariable(ENV_CURRENT_CAMPAIGN) +
					      variables_file_name;

	if(std::filesystem::exists(campaign_variables_file.c_str())) return;

	output(info,"Campaign variables file not found, creating file at \'%s\'.",campaign_variables_file.c_str());
	std::ofstream ofs(campaign_variables_file.c_str());
	ofs.close();
}

void confirmHistoryFile()
{
	if(std::filesystem::exists(history_path.c_str())) return;

	output(info,"rpgsh history file not found, creating file at \"%s\".",history_path.c_str());
	std::ofstream ofs(history_path.c_str());
	ofs.close();
}

std::vector<std::string> getDirectoryListing(std::string path)
{
	if(!std::filesystem::exists(path))
	{
		output(error,"Directory \"%s\" does not exist.",path.c_str());
		exit(-1);
	}
	else if(!std::filesystem::is_directory(path))
	{
		output(error,"\"%s\" is not a directory.",path.c_str());
		exit(-1);
	}

	std::vector<std::string> entries;
	for(const auto& entry : std::filesystem::directory_iterator(path))
		entries.push_back(entry.path().filename().string());

	return entries;
}
std::string getLikeFileName(std::string chk_file,std::string chk_dir,bool is_dir,std::string xref)
{
	for(const auto& entry : getDirectoryListing(chk_dir))
	{
		if(is_dir &&
		   !stringcasecmp(entry,chk_file) &&
		   std::filesystem::is_directory(campaigns_dir+entry))
			return entry;
		else if(!is_dir &&
			!stringcasecmp(entry,chk_file) &&
			std::filesystem::is_regular_file(chk_dir+entry))
			return entry;
	}
	output(error,"Invalid xref \"%s\".",xref.c_str());
	exit(-1);
}
void loadXRef(std::string* arg, VariableInfo* p_vi)
{
	// Ending square bracket not found
	if(findu(*arg,']') == std::string::npos)
	{
		output(error,"No terminating \']\' found for xref.");
		exit(-1);
	}

	// Get string between the square brackets
	p_vi->xref = arg->substr(findu(*arg,'[')+1,findu(*arg,']')-(findu(*arg,'[')+1));

	// Actually load the xref into the scope
	std::vector<std::string> campaigns;
	std::string xref_dir = campaigns_dir+
			       getEnvVariable(ENV_CURRENT_CAMPAIGN)+
			       "characters/";
	std::string xref_char = p_vi->xref;
	std::string campaign = "";
	switch((*arg)[0])
	{
		case CHARACTER_SIGIL:
			if(findu(p_vi->xref,'/') != std::string::npos)// Attempt to load character from another campaign
			{
				campaign = left(p_vi->xref,findu(p_vi->xref,'/'));
				xref_char = right(p_vi->xref,findu(p_vi->xref,'/')+1);
				xref_dir = campaigns_dir+
					   getLikeFileName(campaign,campaigns_dir,true,p_vi->xref)+
					   "/characters/";
			}

			p_vi->scope.setDatasource(xref_dir+getLikeFileName(xref_char+c_ext,xref_dir,false,p_vi->xref));
			p_vi->scope.load();
			break;
		case CAMPAIGN_SIGIL:
			p_vi->scope.setDatasource(campaigns_dir+
						  getLikeFileName(p_vi->xref,campaigns_dir,true,p_vi->xref)+
						  "/"+
						  variables_file_name);
			p_vi->scope.load();
			break;
		case SHELL_SIGIL:
			output(error,"Cannot use xref with shell scope.");
			exit(-1);
	}

	// Remove external reference string so we can continue to use the current arg under the new context
	*arg = (*arg)[0]+left(*arg,findu(*arg,'[')-1)+right(*arg,findu(*arg,']')+1);
}
VariableInfo parseVariable(std::string v)
{
	VariableInfo vi;

	// Check scope sigil
	switch(v[0])
	{
		case CHARACTER_SIGIL:
			vi.scope = Character();
			break;
		case CAMPAIGN_SIGIL:
			vi.scope = Campaign();
			break;
		case SHELL_SIGIL:
			vi.scope = Shell();
			break;
		default:
			output(error,"Unknown scope sigil \'%c\'.",v[0]);
			exit(-1);
	}

	// Check for external references
	if(v[1] == '[') loadXRef(&v,&vi);

	// Check type sigil
	if(!isTypeSigil(v[1]) && v[1] != '/')
	{
		output(error,"Unknown type sigil \'%c\'.",v[1]);
		exit(-1);
	}

	vi.type = v[1];
	vi.variable = v;
	vi.key = right(v,findu(v,'/')+1);
	if((v[rfindu(v,'/')+1] == '.' && rfindu(v,'.') > rfindu(v,'/')+1) ||
	   (findu(v,'.') > rfindu(v,'/')+1 && rfindu(v,'.') < UINT_MAX))
	{
		vi.key = left(vi.key,rfindu(vi.key,'.'));
		vi.property = right(v,rfindu(v,'.')+1);
	}

	if(isTypeSigil(vi.type))
	{
		vi.evalType = vi.type;
	}
	else
	{
		if(vi.scope.keyExists<Var>(vi.key))
			vi.evalType = VAR_SIGIL;
		else if(vi.scope.keyExists<Dice>(vi.key))
			vi.evalType = DICE_SIGIL;
		else if(vi.scope.keyExists<Wallet>(vi.key))
			vi.evalType = WALLET_SIGIL;
		else if(vi.scope.keyExists<Currency>(vi.key))
			vi.evalType = CURRENCY_SIGIL;
		else
			vi.evalType = VAR_SIGIL;
	}

	return vi;
}

MCStr parseMCStr(std::string s)
{
	MCStr mc;
	unsigned long int slash = findu(s,'/');
	if(slash == std::string::npos)
	{
		mc.c = s;
		return mc;
	}
	else
	{
		mc.m = left(s,slash);
		mc.c = right(s,slash+1);
	}
	return mc;
}

void padding()
{
	Config config = Config();

	//Pad output if set
	try
	{
		if(stob(config.setting[PADDING]))
			fprintf(stdout,"\n");
	}
	catch(...)
	{
		output(error,"Invalid value \"%s\" for \"%s\".",config.setting[PADDING].c_str(),PADDING);
		exit(-1);
	}
}

int replaceVariables(std::string* p_arg_str, bool preserveSecondArg)
{
	std::regex variable_pattern = std::regex(variable_pattern_str);
	std::sregex_iterator v_str_it = std::sregex_iterator((*p_arg_str).begin(),(*p_arg_str).end(),variable_pattern);
	std::regex arg_pattern(arg_pattern_str);
	std::sregex_iterator p_arg_str_it((*p_arg_str).begin(),(*p_arg_str).end(),arg_pattern);
	std::sregex_iterator end;

	if(p_arg_str_it != end) p_arg_str_it++;

	if(preserveSecondArg && v_str_it != end && p_arg_str_it != end && v_str_it->str() == p_arg_str_it->str())
		v_str_it++;

	// For some reason, attempting to just use v_str_it in the std::regex_replace line to replace variables with their values
	// results in the iterator getting in some way lost causing it to not replace all the variables.
	// Seems to be limited to cases when the value length > variable length
	// Either way, using a std::vector<std::string> is an effective work around.
	std::vector<std::pair<std::string,long unsigned int>> matches;
	while(v_str_it != end)
	{
		matches.push_back(std::pair<std::string,long unsigned int>(v_str_it->str(),v_str_it->position()));
		v_str_it++;
	}
	long unsigned int offset = 0;
	for(const auto& match : matches)
	{
		GetAppOutputInfo info = getAppOutput(match.first);
		if(info.status)
		{
			output(error,"%s is not a valid variable string.",match.first.c_str());
			return -1;
		}
		(*p_arg_str) = left((*p_arg_str),match.second-offset)+
			  std::regex_replace(match.first,std::regex(escapeRegexGroupChars(match.first)),info.output[0])+
			  right((*p_arg_str),match.second+match.first.length()-offset);
		offset += match.first.length()-info.output[0].length();
	}

	//Recursively replace variables
	v_str_it = std::sregex_iterator((*p_arg_str).begin(),(*p_arg_str).end(),variable_pattern);

	if(preserveSecondArg && v_str_it != end) v_str_it++;

	if(v_str_it != end)
	{
		int replaceVariablesResult = replaceVariables(p_arg_str,preserveSecondArg);
		if(replaceVariablesResult) return replaceVariablesResult;
	}
	return 0;
}
int runApp(std::string arg_str, bool redirect_output)
{
	Config cfg = Config();
	Character c = Character();
	Campaign m = Campaign();
	Shell s = Shell();

	std::vector<std::string> args;
	extern char** environ;
	pid_t pid;

	std::string first_arg;
	if(findu(arg_str," ") != std::string::npos) first_arg = left(arg_str,findu(arg_str," "));
	else					    first_arg = arg_str;

	//Check aliases
	for(const auto& [k,v] : getSet(cfg.setting[ALIASES]))
	{
		if(k != first_arg) continue;
		arg_str = v + right(arg_str,first_arg.length());
		break;
	}

	//Check if implicitly running eval
	std::regex variable_pattern(d_imp_const_pattern_str+"|"+variable_pattern_str);
	std::sregex_iterator v_str_it(first_arg.begin(),first_arg.end(),variable_pattern);
	std::sregex_iterator end;
	if(v_str_it != end && v_str_it->str() == first_arg) arg_str = "eval " + arg_str;

	if(findu(arg_str," ") != std::string::npos) first_arg = left(arg_str,findu(arg_str," "));
	else					    first_arg = arg_str;

	//See if we need to preserve the argv[1] if it is a variable
	bool preserveSecondArg = false;
	if(findu(arg_str,std::string(FLAG_MODIFYVARIABLES)) == std::string::npos)
	{
		GetAppOutputInfo info = getAppOutput(first_arg + " " + std::string(FLAG_MODIFYVARIABLES));

		if(!info.status) preserveSecondArg = stob(info.output[0]);
		else return info.status;
	}

	//Push back program we are going to run
	std::string path = std::string(RPGSH_INSTALL_DIR);
	args.push_back(path+prefix+left(arg_str,findu(arg_str," ")));

	//Replace variables
	int replaceVariablesResult = replaceVariables(&arg_str,preserveSecondArg);
	if(replaceVariablesResult) return replaceVariablesResult;

	//Get args for program
	std::regex arg_pattern(arg_pattern_str);
	std::sregex_iterator arg_str_it = std::sregex_iterator(arg_str.begin(),arg_str.end(),arg_pattern);
	arg_str_it++;

	while(arg_str_it != end)
	{
		std::string arg = arg_str_it->str();
		while(arg.back() == '\\')//Merge args with escaped spaces
		{
			arg_str_it++;
			arg = left(arg,arg.length()) + " " + arg_str_it->str();
		}

		//Handle comments
		if(findu(arg,COMMENT) != std::string::npos && findu(arg,COMMENT) > 0)
		{
			args.push_back(left(arg,findu(arg,COMMENT)));
			break;
		}
		else if(findu(arg,COMMENT) == std::string::npos)
		{
			args.push_back(arg);
			arg_str_it++;
		}
		else
		{
			break;
		}
	}

	//Merge args wrapped in quotes
	unsigned long int quote_begin = std::string::npos;
	unsigned long int quote_end = std::string::npos;
	unsigned long int quote_start_arg = 0;
	for(unsigned long int i=0; i<args.size(); i++)
	{
		//Find unescaped quote marks
		for(unsigned long int c=0; c<args[i].length(); c++)
		{
			if(quote_begin == std::string::npos && args[i][c] == '\"' && !isEscaped(args[i],c))
			{
				quote_begin = c;
				quote_end = findu(args[i],'\"',c+1);
				quote_start_arg = i;
				if(quote_end == std::string::npos)
				{
					args[i] = right(args[i],c+1);
				}
				else//No spaces in quote-wrapped string
				{
					args[i] = mergeQuotes(left(args[i],quote_end));
					quote_begin = std::string::npos;
					quote_end = std::string::npos;
					quote_start_arg = 0;
					break;
				}
			}
			else if(quote_end == std::string::npos && args[i][c] == '\"' && !isEscaped(args[i],c))
			{
				quote_end = c;

				for(unsigned long int q=quote_start_arg+1; q<=i;)
				{
					if(q < i) args[quote_start_arg] += " "+args[q];
					else args[quote_start_arg] += " "+left(args[q],quote_end);

					args.erase(args.begin()+q);
					i--;
				}
				args[i] = mergeQuotes(args[i]);
				quote_begin = std::string::npos;
				quote_end = std::string::npos;
				quote_start_arg = 0;
				break;
			}
		}
	}

	//Convert string vector to char* vector for posix_spawn
	std::vector<char*> argv;
	for(auto& arg : args)
		argv.push_back((char*)arg.c_str());

	//Add a NULL because posix_spawn() needs that for some reason
	argv.push_back(NULL);

	int status = 0;
	int exit_code = 0;
	posix_spawn_file_actions_t fa;
	if(redirect_output)
	{
		//https://unix.stackexchange.com/questions/252901/get-output-of-posix-spawn

		if(posix_spawn_file_actions_init(&fa))
			output(error,"Error code %d during posix_spawn_file_actions_init(): %s",status,strerror(status));

		if(posix_spawn_file_actions_addopen(&fa, 1, rpgsh_output_redirect_path, O_CREAT | O_TRUNC | O_WRONLY, 0644))
			output(error,"Error code %d during posix_spawn_file_actions_addopen(): %s",status,strerror(status));

		if(posix_spawn_file_actions_adddup2(&fa, 1, 2))
			output(error,"Error code %d during posix_spawn_file_actions_adddup2(): %s",status,strerror(status));

		status = posix_spawn(&pid, argv[0], &fa, NULL, &argv[0], environ);
	}
	else
	{
		status = posix_spawn(&pid, argv[0], NULL, NULL, &argv[0], environ);
	}

	if(status == 0)
	{
		do
		{
			if(waitpid(pid, &status, 0) == -1) exit(1);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));

		exit_code = WEXITSTATUS(status);
	}
	else
	{
		std::string displayed_command = std::string(argv[0]).substr(prefix.length()+path.length(),
							    std::string(argv[0]).length()-prefix.length()-path.length());
		if(status == 2)//File not found
			output(error,"Error code %d while attempting to run \"%s\": Not a valid rpgsh command.",status,displayed_command.c_str());
		else
			output(error,"Error code %d while attempting to run \"%s\": %s",status,displayed_command.c_str(),strerror(status));

		return status;
	}

	if(redirect_output && posix_spawn_file_actions_destroy(&fa))
	{
		output(error,"Error code %d during posix_spawn_file_actions_destroy(): %s",status,strerror(status));

		return status;
	}

	return exit_code;
}
GetAppOutputInfo getAppOutput(std::string prog)
{
	GetAppOutputInfo info;

	info.status = runApp(prog,true);

	std::ifstream ifs(rpgsh_output_redirect_path);
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		info.output.push_back(data);
	}
	ifs.close();

	return info;
}

void chkFlagAppDesc(char** _argv, std::string description)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"%s\n",description.c_str());
		exit(0);
	}
}
void chkFlagModifyVariables(char** _argv, bool canModify)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_MODIFYVARIABLES))
	{
		fprintf(stdout,"%s\n",btos(canModify).c_str());
		exit(0);
	}
}
bool chkFlagHelp(char** _argv)
{
	return _argv[1] && (!strcasecmp(_argv[1],FLAG_HELPSHORT) || !strcasecmp(_argv[1],FLAG_HELPLONG));
}

std::string getEnvVariable(std::string v)
{
	confirmEnvVariablesFile();

	std::ifstream ifs(rpgsh_env_variables_path.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(left(data,findu(data,DS)) == v)
			return right(data,findu(data,DS)+DS.length());
	}
	ifs.close();
	return "";
}
void setEnvVariable(std::string v,std::string value)
{
	confirmEnvVariablesFile();

	std::ifstream ifs(rpgsh_env_variables_path.c_str());
	std::filesystem::remove((rpgsh_env_variables_path+".bak").c_str());
	std::ofstream ofs((rpgsh_env_variables_path+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "" || data == "\n")// Prevents writing blank lines back to file
			continue;
		if(left(data,findu(data,DS)) == v)
		{
			ofs<<v + DS + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue) ofs<<v + DS + value + "\n";

	ifs.close();
	ofs.close();
	std::filesystem::remove(rpgsh_env_variables_path.c_str());
	std::filesystem::rename((rpgsh_env_variables_path+".bak").c_str(),rpgsh_env_variables_path.c_str());
}

unsigned int getWalletValue(Wallet w)
{
	unsigned int total = 0;
	std::vector<std::string> systems;

	for(const auto& m : w.Monies)
	{
		if(findInVect<std::string>(systems,m.c.System) != -1)
		{
			systems.push_back(m.c.System);
			total += w.getEquivalentValueInLowestDenomination(m.c.System);
		}
	}

	return total;
}

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map)
{
	for(const auto& [k,v] : scope.getDatamap<T>())
		(*p_map)[k] = scope.getStr<T>(k);
}
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput)
{
	for(const auto& [k,v] : map)
	{
		// Get root variable if it exists
		if(!stringcasecmp((left(k,key.length())+"/"),key))
			pOutput->append(k + DS + v + DS);

		// Get all subkeys
		if(!stringcasecmp((left(k,key.length())),key))
			pOutput->append(k + DS + v + DS);
	}
}

std::map<std::string,std::string> getSet(std::string set_str)
{
	std::map<std::string,std::string> set;
	std::string key, value;

	while(findu(set_str,DS) != std::string::npos)
	{
		key = left(set_str,findu(set_str,DS));
		set_str = right(set_str,findu(set_str,key)+key.length()+DS.length());

		if(findu(set_str,DS) != std::string::npos)
		{
			value = left(set_str,findu(set_str,DS));
			set_str = right(set_str,findu(set_str,value)+value.length()+DS.length());
		}
		else
		{
			value = set_str;
		}
		set[key] = value;
	}

	return set;
}
std::string getSetStr(VariableInfo vi)
{
	std::map<std::string,std::string> c_map;
	std::map<std::string,std::string> d_map;
	std::map<std::string,std::string> v_map;
	std::map<std::string,std::string> w_map;

	// When printing entire containers, treat type sigil as a filter
	switch(vi.variable[1])
	{
		case CURRENCY_SIGIL:
			appendMap<Currency>(vi.scope,&c_map);
			break;
		case DICE_SIGIL:
			appendMap<Dice>(vi.scope,&d_map);
			break;
		case VAR_SIGIL:
			appendMap<Var>(vi.scope,&v_map);
			break;
		case WALLET_SIGIL:
			appendMap<Wallet>(vi.scope,&w_map);
			break;
		case '/':
			appendMap<Currency>(vi.scope,&c_map);
			appendMap<Dice>(vi.scope,&d_map);
			appendMap<Var>(vi.scope,&v_map);
			appendMap<Wallet>(vi.scope,&w_map);
			break;
		default:
			output(error,"Unknown type specifier \'%c\' in \"%s\"",vi.variable[1],vi.variable.c_str());
			exit(-1);
	}

	// Create output string from map
	std::string output = "";
	appendOutput(c_map,vi.key,&output);
	appendOutput(d_map,vi.key,&output);
	appendOutput(v_map,vi.key,&output);
	appendOutput(w_map,vi.key,&output);

	// Cut off the extraneous DS
	return left(output,output.length()-DS.length());
}

template <typename T>
void sort(std::vector<std::string>* v);

template <>
datamap<Currency> getDatamapFromAllScopes()
{
	std::string character = getEnvVariable(ENV_CURRENT_CHARACTER);
	std::string campaign = getEnvVariable(ENV_CURRENT_CAMPAIGN);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_campaign_path = current_campaign_dir+variables_file_name;
	std::string current_character_path = current_campaign_dir+"characters/"+character+c_ext;

	datamap<Currency> ret;
	Scope scope;

	scope.load(current_character_path, false, false, true, false);
	for(const auto& [k,v] : scope.getDatamap<Currency>())
		ret[k] = v;

	scope.load(current_campaign_path, false, false, true, false);
	for(const auto& [k,v] : scope.getDatamap<Currency>())
		ret[k] = v;

	scope.load(shell_variables_path, false, false, true, false);
	for(const auto& [k,v] : scope.getDatamap<Currency>())
		ret[k] = v;

	return ret;

}
