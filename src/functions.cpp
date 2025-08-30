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
#include "../headers/configuration.h"
#include "../headers/functions.h"
#include "../headers/scope.h"

void printHeader(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(long unsigned i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
}

std::string defaultPrompt(Character c)
{
	return std::string(TEXT_BOLD)+std::string(TEXT_RED)+c.getName()+std::string(TEXT_WHITE)+"> "+std::string(TEXT_NORMAL);
}

void output(OutputLevel level, const char* format, ...)
{
	Configuration cfg = Configuration();
	int verbosity;
	try
	{
		verbosity = std::stoi(cfg.setting[VERBOSITY]);
	}
	catch(...)
	{
		fprintf(stderr,"%s%sUnable to parse output verbosity!%s\n",TEXT_RED,TEXT_BOLD,TEXT_NORMAL);
		exit(-1);
	}

	FILE* stream;
	std::string prefix = "";
	std::string color;
	va_list args;
	va_start(args, format);

	if((verbosity == 2 && level == info) ||
	   (verbosity == 1 && level != error) ||
	    verbosity < 1) return;

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

std::string handleBackslashEscSeqs(std::string str)
{
	for(long unsigned i=0; i<str.length(); i++)
	{
		if(str[i] == '\\' && i == str.length())
		{
			str.erase(str.begin()+i);
		}
		else if(str[i] == '\\')
		{
			switch(str[i+1])
			{
				case 'a':
					str[i+1] = '\a'; //Bell
					break;
				case 'b':
					str[i+1] = '\b'; //Backspace
					break;
				case 'f':
					str[i+1] = '\f'; //Form feed
					break;
				case 'n':
					str[i+1] = '\n'; //Newline
					break;
				case 't':
					str[i+1] = '\t'; //Horizontal tab
					break;
				case 'v':
					str[i+1] = '\v'; //Vertical tab
					break;
			}
			str.erase(str.begin()+i);
		}
	}
	return str;
}
std::string makePretty(std::string str)
{
	Character c = Character();

	if(str == "") return empty_str.c_str();

	str = std::regex_replace(str,std::regex("%name%",std::regex_constants::icase),c.getName());

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

	str = handleBackslashEscSeqs(str);

	for(const auto& [k,v] : ansi_esc_seqs)
		str = std::regex_replace(str,std::regex("%"+k+"%",std::regex_constants::icase),std::string(v));

	std::regex fg_rgb_color_pattern("%fg=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}%");
	std::regex fg_html_color_pattern("%fg=[a-zA-Z]{1,}%");
	std::regex bg_rgb_color_pattern("%bg=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}%");
	std::regex bg_html_color_pattern("%bg=[a-zA-Z]{1,}%");

	std::sregex_iterator fg_rgb_it(str.begin(),str.end(),fg_rgb_color_pattern);
	std::sregex_iterator fg_html_it(str.begin(),str.end(),fg_html_color_pattern);
	std::sregex_iterator bg_rgb_it(str.begin(),str.end(),bg_rgb_color_pattern);
	std::sregex_iterator bg_html_it(str.begin(),str.end(),bg_html_color_pattern);

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

	for(const auto& [k,v] : string_replacements) str = std::regex_replace(str,std::regex(k),v);
	str = std::regex_replace(str,std::regex("%"),"%%");//Prevent printf() parsing '%' as format specifiers

	str += std::string(TEXT_NORMAL); //Make sure we don't carry over any unterminated formatting
	return str;
}
std::string stripFormatting(std::string str)
{
	for(long unsigned i=0; i<str.length(); i++)
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

std::string btos(bool b)
{
	if(b) return "True";
	return "False";
}
std::string pvltos(PreserveVariableLevel pvl)
{
	if(pvl == none) return "none";
	else if(pvl == first) return "first";
	else return "all";
}
bool stob(std::string s)
{
	return !stringcasecmp(Var(s).Value,"true");
}
PreserveVariableLevel stopvl(std::string s)
{
	if(!stringcasecmp(s,"none")) return none;
	else if(!stringcasecmp(s,"first")) return first;
	else if(!stringcasecmp(s,"all")) return all;
	else throw;
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

bool isEscaped(std::string str, long unsigned pos)
{
	if(pos <= 0 || pos > str.length()) return false;
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

long unsigned findu(std::string str, std::string match, long unsigned start)
{
	if(match.length() > str.length()) return std::string::npos;
	for(long unsigned i=start; i<str.length()-(match.length()-1); i++)
		if(str.substr(i,match.length()) == match && (!i || !isEscaped(str.substr(i-1,match.length()+1),1))) return i;

	return std::string::npos;
}
long unsigned findu(std::string str, char ch, long unsigned start)
{
	return findu(str, std::string(1,ch), start);
}
long unsigned rfindu(std::string str, std::string match, long unsigned start)
{
	if(match.length() > str.length()) return std::string::npos;
	if(start == UINT_MAX) start = str.length();

	bool last_check = false;
	for(long unsigned i=start-match.length(); i>0; i--)
	{
		if(str.substr(i-last_check,match.length()) == match &&
		   !isEscaped(str.substr(i-last_check,match.length()),1)) return i-last_check;
		else if(i == 1 && !last_check)
		{
			i++;
			last_check = true;
		}
	}

	return std::string::npos;
}
long unsigned rfindu(std::string str, char ch, long unsigned start)
{
	return rfindu(str, std::string(1,ch), start);
}
long unsigned nfindu(std::string str, std::string match, long unsigned start)
{
	if(match.length() > str.length()) return std::string::npos;
	for(long unsigned i=start; i<str.length()-(match.length()-1); i++)
		if(str.substr(i,match.length()) != match && (!i || !isEscaped(str.substr(i-1,match.length()+1),1))) return i;

	return std::string::npos;
}
long unsigned nfindu(std::string str, char ch, long unsigned start)
{
	return nfindu(str, std::string(1,ch), start);
}

std::string left(std::string str, int n)
{
	return str.substr(0,n);
}
std::string right(std::string str, int n)
{
	return str.substr(n,str.length()-n);
}

long unsigned countu(std::string str, char ch)
{
	long unsigned count = 0;
	for(long unsigned i=0; i<str.length(); i++)
		if(str[i] == ch && !isEscaped(str,i)) count++;

	return count;
}

long unsigned getDisplayLength(std::string str)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int len = 0;

	for(long unsigned i = 0; i<str.length(); i++)
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
int getCharLength(char c)
{
	unsigned char uc = static_cast<unsigned char>(c);
	if(uc < 0x80) return 1;
	else if((uc&0xe0) == 0xc0) return 2;
	else if((uc&0xf0) == 0xe0) return 3;
	else if((uc&0xf8) == 0xf0) return 4;
	else return 0;
}

void addKeyToMatches(std::vector<std::string>** ppMatches, std::string k, std::string chk_str,
		     std::string key)
{
	//Limit keys to one level past current
	if(!stringcasecmp(left(k,key.length()),key) && k[0] != '.')
	{
		long unsigned key_last_slash = rfindu(key,'/');
		long unsigned k_next_slash = findu(k,'/',key_last_slash+1);
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
void moveCursorBack(winsize w, int start, int end)
{
	fprintf(stdout,CURSOR_HIDE);
	for(int i=start; i>end; i--)
	{
		if(i < start && (i%w.ws_col) == w.ws_col-1)
		{
			fprintf(stdout,CURSOR_UP);
			fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)w.ws_col-1);
		}
		else if(i%w.ws_col >= 1)
		{
			fprintf(stdout,CURSOR_LEFT);
		}
	}
	fprintf(stdout,CURSOR_SHOW);
}
void moveCursorForward(winsize w, int start, int end)
{
	fprintf(stdout,CURSOR_HIDE);
	for(int i=start; i<end; i++)
	{
		if(i > start && (i%w.ws_col) == 1)
		{
			fprintf(stdout,CURSOR_DOWN);
			fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)0);
		}
		else if(i%w.ws_col >= 1)
		{
			fprintf(stdout,CURSOR_RIGHT);
		}
	}
	fprintf(stdout,CURSOR_SHOW);
}
void inputHandler(std::string* pInput, long unsigned offset)
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
	long unsigned cur_pos = 0;
	long unsigned char_pos = cur_pos;
	long unsigned history_len = history.size();
	long unsigned history_ctr = history_len-1;
	long unsigned combined_offset = offset+last_history.length();

	while(k != KB_ENTER)
	{
		esc_char = 0;
		k = getchar();

		if(k == KB_ENTER && countu(*pInput,'{') > countu(*pInput,'}'))
		{
			moveCursorBack(w,cur_pos+offset,offset);
			long unsigned spaces_to_add = w.ws_col-((getDisplayLength(*pInput)+offset)%w.ws_col);
			(*pInput).insert((*pInput).begin()+char_pos,spaces_to_add,' ');
			char_pos += spaces_to_add;
			cur_pos += spaces_to_add;

			fprintf(stdout,CLEAR_TO_SCREEN_END);
			fprintf(stdout,"%s ",(*pInput).c_str());
			moveCursorBack(w,getDisplayLength((*pInput))+offset+1,cur_pos+offset);
			k = 0;
			continue;
		}
		if(k == KB_ENTER) break;

		if(k == ESC_SEQ)
		{
			getchar();//Consume '['
			esc_char = getchar();
		}

		//Reset tab completion variables
		if(k != KB_TAB && esc_char != 'Z')//Z for shift+tab
		{
			if(nfindu(last_match,' ') != std::string::npos &&
			   findu((*pInput),' ',char_pos) == std::string::npos)
			{
				cur_pos = getDisplayLength(*pInput);
				char_pos = (*pInput).size();
			}
			else if(nfindu(last_match,' ') != std::string::npos)
			{
				cur_pos = getDisplayLength(left(*pInput,findu((*pInput),' ',char_pos+1)));
				char_pos = findu((*pInput),' ',char_pos+1);
			}
			tab_ctr = 0;
			last_match = "";
		}

		//Reset history ctr
		if(k != ESC_SEQ || (esc_char != 'A' && esc_char != 'B'))
			history_ctr = history_len-1;

		if(k == KB_BACKSPACE)//Backspace
		{
			if(!cur_pos) continue;
			moveCursorBack(w,cur_pos+offset,offset);
			fprintf(stdout,CLEAR_TO_SCREEN_END);

			cur_pos--;
			char_pos--;
			if(getCharLength((*pInput)[char_pos]) == 1)
			{
				(*pInput).erase((*pInput).begin()+char_pos);
			}
			else
			{
				for(long unsigned i=char_pos; !getCharLength((*pInput)[i]); i--)
				{
					(*pInput).erase((*pInput).begin()+i);
					char_pos--;
				}
				(*pInput).erase((*pInput).begin()+char_pos);
			}

			fprintf(stdout,"%s ",(*pInput).c_str());
			moveCursorBack(w,getDisplayLength(*pInput)+offset+1,cur_pos+offset);
		}
		else if((k == KB_TAB || esc_char == 'Z') &&
			(cur_pos == getDisplayLength(*pInput) ||
			(*pInput)[char_pos] == ')' || (*pInput)[char_pos] == ' ' ||
			last_match != ""))//Tab (completion)
		{
			if(!(*pInput).size()) continue;

			if(k == KB_TAB) tab_ctr++;
			else tab_ctr--;

			Scope tab_comp_scope;
			std::string match = "";
			std::vector<std::string> matches;

			//Create the string which is to be completed
			std::string chk_str = "";
			for(int i=char_pos-1; i>=0 && (*pInput)[i]!=' '; i--)
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

					std::string app_chk_str = app.substr(getDisplayLength(prefix),getDisplayLength(chk_str));
					long unsigned app_ln = getDisplayLength(right(app,getDisplayLength(prefix)));
					if(app_chk_str == chk_str && app_ln > getDisplayLength(chk_str))
						matches.push_back(right(app,getDisplayLength(prefix)));
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
						if(right(c,getDisplayLength(c)-5) != c_ext)//Don't include backup files or non-character files
							continue;

						std::string xref = left(c,findu(c,c_ext));
						std::string chk_str_xref = right(chk_str,2);
						if(getDisplayLength(chk_str) == 2 ||
						   (!stringcasecmp(chk_str_xref,left(xref,getDisplayLength(chk_str)-2)) &&
						   getDisplayLength(xref) > getDisplayLength(chk_str_xref)))
							matches.push_back("  "+xref+"]");//Two spaces are sacrificial for formatting
					}
					for(auto& m : getDirectoryListing(campaigns_dir))
					{
						std::string chk_str_xref = right(chk_str,2);
						if(std::filesystem::is_directory(campaigns_dir+m) &&
						   (getDisplayLength(chk_str) == 2 ||
						   (!stringcasecmp(chk_str_xref,left(m,getDisplayLength(chk_str)-2)) &&
						   m.length() > getDisplayLength(chk_str_xref))))
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
						if(right(c,getDisplayLength(c)-5) != c_ext)//Don't include backup files or non-character files
							continue;

						std::string xref = left(c,findu(c,c_ext));
						std::string chk_str_xref = right(chk_str,findu(chk_str,'/')+1);
						if(!stringcasecmp(chk_str_xref,left(xref,getDisplayLength(chk_str)-findu(chk_str,'/')-1)) &&
						   getDisplayLength(xref) > getDisplayLength(chk_str_xref))
						{
							matches.push_back(addSpaces(getDisplayLength(chk_str)-(getDisplayLength(chk_str)-findu(chk_str,'/'))+1)+
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
						if(getDisplayLength(chk_str) == 2 ||
						   (!stringcasecmp(chk_str_xref,left(m,getDisplayLength(chk_str)-2)) &&
						   getDisplayLength(m) > getDisplayLength(chk_str_xref)))
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
				for(long unsigned i=0; i<getDisplayLength(last_match)-getDisplayLength(chk_str); i++)
					(*pInput).erase((*pInput).begin()+char_pos);
			}

			//Insert match into (*pInput)
			for(long unsigned i=0; i<getDisplayLength(match)-getDisplayLength(chk_str); i++)
				(*pInput).insert((*pInput).begin()+char_pos+i,match[i+getDisplayLength(chk_str)]);

			//Reprint (*pInput)
			if(last_match != "" && match != "" && nfindu(match,' ') != std::string::npos)
			{
				long unsigned start = cur_pos+getDisplayLength(last_match)-countu(last_match,' ')+offset+1;
				long unsigned end = cur_pos+offset+1;
				moveCursorBack(w,start,end-(start/w.ws_col > end/w.ws_col));
				fprintf(stdout,CLEAR_TO_SCREEN_END);
			}

			fprintf(stdout,"%s",right((*pInput),char_pos).c_str());

			unsigned int match_offset = getDisplayLength(match)-countu(match,' ');
			if(cur_pos+match_offset < getDisplayLength(*pInput))
				moveCursorBack(w,getDisplayLength(*pInput)+offset+1,cur_pos+offset+getDisplayLength(match)-countu(match,' ')+1);

			last_match = match;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			long unsigned del_end = (cur_pos < getDisplayLength(*pInput) ? char_pos+getCharLength((*pInput)[char_pos]) : 0);
			switch(esc_char)
			{
				case 'A':	//Up
				case 'B':	//Down
					if(cur_pos > 0) fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					if(combined_offset >= w.ws_col)
					{
						fprintf(stdout,CURSOR_UP_N,combined_offset/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,offset);
					}
					fprintf(stdout,CLEAR_TO_SCREEN_END);
					(*pInput).clear();
					if(esc_char == 'B' && history_ctr == history_len-1)
					{
						cur_pos = 0;
						char_pos = 0;
						break;
					}
					if     (esc_char == 'A' && history_ctr > 0)		history_ctr--;
					else if(esc_char == 'B' && history_ctr < history_len-1)	history_ctr++;
					for(const auto& c : history[history_ctr])
						(*pInput).push_back(c);
					fprintf(stdout,"%s",history[history_ctr].c_str());
					last_history = history[history_ctr];
					cur_pos = getDisplayLength(*pInput);
					char_pos = (*pInput).size();
					combined_offset = offset+getDisplayLength(last_history);
					break;
				case 'C':	//Right
					if(cur_pos == getDisplayLength((*pInput))) continue;
					cur_pos++;
					char_pos+=getCharLength((*pInput)[char_pos]);
					fprintf(stdout,CURSOR_RIGHT);
					if(cur_pos && !((cur_pos+offset)%w.ws_col))
					{
						fprintf(stdout,CURSOR_DOWN);
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)0);
					}
					break;
				case 'D':	//Left
					if(cur_pos == 0) continue;
					cur_pos--;
					char_pos--;
					while(!getCharLength((*pInput)[char_pos])) char_pos--;
					fprintf(stdout,CURSOR_LEFT);
					if(cur_pos && !((cur_pos+offset+1)%w.ws_col))
					{
						fprintf(stdout,CURSOR_UP);
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)w.ws_col);
					}
					break;
				case 'H':	//Home
					if(cur_pos == 0) continue;
					if(cur_pos+offset >= w.ws_col)
					{
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)0);
						fprintf(stdout,CURSOR_UP_N,(cur_pos+offset)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,offset);
					}
					else
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					}
					cur_pos = 0;
					char_pos = 0;
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
						(*pInput).push_back(c);
					fprintf(stdout,"%s",(*pInput).data());
					cur_pos = (*pInput).size();
					char_pos = getDisplayLength(*pInput);
					break;
				case '7':	//Home
					if(getchar() != '~' || cur_pos == 0) continue;
					if(cur_pos+offset >= w.ws_col)
					{
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)0);
						fprintf(stdout,CURSOR_UP_N,(cur_pos+offset)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,offset);
					}
					else
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					}
					cur_pos = 0;
					char_pos = 0;
					break;
				case 'F':	//End
					if(cur_pos >= getDisplayLength(*pInput)) continue;
					if((*pInput).size()-cur_pos>=w.ws_col)
					{
						fprintf(stdout,CURSOR_DOWN_N,(getDisplayLength(*pInput)-cur_pos)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,(getDisplayLength(*pInput)-cur_pos)%w.ws_col);
					}
					else
					{
						fprintf(stdout,CURSOR_RIGHT_N,getDisplayLength(*pInput)-cur_pos);
					}
					cur_pos = getDisplayLength(*pInput);
					char_pos = (*pInput).size();
					break;
				case '8':	//End
					if(getchar() != '~' || cur_pos >= getDisplayLength(*pInput)) continue;
					if(getDisplayLength(*pInput)-cur_pos >= w.ws_col)
					{
						fprintf(stdout,CURSOR_DOWN_N,(getDisplayLength(*pInput)-cur_pos)/w.ws_col);
						fprintf(stdout,CURSOR_RIGHT_N,(getDisplayLength(*pInput)-cur_pos)%w.ws_col);
					}
					else
					{
						fprintf(stdout,CURSOR_RIGHT_N,getDisplayLength(*pInput)-cur_pos);
					}
					cur_pos = getDisplayLength(*pInput);
					char_pos = (*pInput).size();
					break;
				case '2':	//Insert
					if(getchar() == '~') insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() != '~' || cur_pos >= getDisplayLength((*pInput))) continue;
					fprintf(stdout,CLEAR_TO_SCREEN_END);
					for(long unsigned i=char_pos; i<del_end; i++)
						(*pInput).erase((*pInput).begin()+char_pos);
					moveCursorBack(w,cur_pos);
					fprintf(stdout,"%s",(*pInput).data());
					moveCursorBack(w,getDisplayLength(*pInput)-cur_pos);
					if(!((cur_pos+offset+1)%w.ws_col))
					{
						fprintf(stdout,CURSOR_UP);
						fprintf(stdout,CURSOR_SET_COL_N,(long unsigned)w.ws_col);
					}
					break;
			}
		}
		else //Printable character, probably
		{
			if(insert_mode)
			{
				(*pInput)[char_pos] = k;
				while(!getCharLength((*pInput)[char_pos+1]))
					(*pInput).erase((*pInput).begin()+char_pos+1);
			}
			else
			{
				(*pInput).insert((*pInput).begin()+char_pos,k);
			}

			char_pos++;

			bool can_print = false;
			std::string text;
			long unsigned expected_input_size = 0;

			for(const auto& ch : (*pInput))
				expected_input_size += getCharLength(ch);

			//Ensure we have all bytes in the character
			if(expected_input_size == (*pInput).size())
			{
				moveCursorBack(w,cur_pos+offset,offset);
				fprintf(stdout,CLEAR_TO_SCREEN_END);
				can_print = true;
			}

			if(can_print)
			{
				fprintf(stdout,"%s ",(*pInput).c_str());
				cur_pos++;
				moveCursorBack(w,getDisplayLength((*pInput))+offset+1,cur_pos+offset);
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
	for(long unsigned i=0; i<str.length(); i++)
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

	for(long unsigned i=0; i<str.length(); i++)
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

	//Set default values for built-in env variables
	Configuration cfg = Configuration();
	Character c = Character(templates_dir + cfg.setting[DEFAULT_GAME]+".char");
	std::ofstream ofs(rpgsh_env_variables_path);
	ofs<<ENV_CURRENT_CHARACTER+DS+c.getName()+"\n";
	ofs<<ENV_CURRENT_CAMPAIGN+DS+"default/\n";
	ofs.close();
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
std::string getLikeFileName(std::string chk_file, std::string chk_dir, bool is_dir, std::string xref)
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
	long unsigned slash = findu(s,'/');
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
	Configuration cfg = Configuration();

	//Pad output if set
	try
	{
		if(stob(cfg.setting[PADDING]))
			fprintf(stdout,"\n");
	}
	catch(...)
	{
		output(error,"Invalid value \"%s\" for \"%s\".",cfg.setting[PADDING].c_str(),PADDING);
		exit(-1);
	}
}

ComplexCommandInfo parseComplexCommandString(int argc, char** argv, bool hasCondition)
{
	ComplexCommandInfo cci;
	bool found_start = false;
	bool found_end = false;
	std::string commands_str;

	if(hasCondition)
	{
		for(int i=1; i<argc; i++)
		{
			if(!found_start && strcmp(argv[i],"{"))
			{
				cci.condition += std::string(argv[i])+" ";
			}
			else if(!found_start)
			{
				found_start = true;
				cci.condition.erase(cci.condition.end()-1);
			}
			else
			{
				for(int j=argc-1; j>=i; j--)
				{
					std::string j_arg = std::string(argv[j]);
					if(found_end)
						commands_str = j_arg+" "+commands_str;
					else if(!found_end && j_arg.back() == '}')
						found_end = true;
				}
				break;
			}
		}
	}

	cci.commands = split(commands_str,';');

	for(unsigned i=0; i<cci.commands.size(); i++)
	{
		if(countu(cci.commands[i],'{') > countu(cci.commands[i],'}'))
		{
			if(i == cci.commands.size()-1)
			{
				output(error,"\t\'}\' not found.");
				exit(-1);
			}
			else if(findu(cci.commands[i+1],'}') != std::string::npos)
			{
				cci.commands[i] += left(cci.commands[i+1],findu(cci.commands[i+1],'}')+1);
				cci.commands[i+1] = right(cci.commands[i+1],findu(cci.commands[i+1],'}')+1);
				i=0;
			}
			else
			{
				cci.commands[i] += ";"+cci.commands[i+1]+";";
				cci.commands.erase(cci.commands.begin()+i+1);
				i=0;
			}
		}
	}

	//Cleaup with regex
	for(auto& command : cci.commands)
	{
		command = std::regex_replace(command,std::regex(";{2,}"),";");
		command = std::regex_replace(command,std::regex("};"),"}");
		if(findu(command,'{') != std::string::npos)
			command = std::regex_replace(command,std::regex("\\\\"),"\\\\");
	}

	if(!found_start)
	{
		output(error,"No commands specified.");
		exit(-1);
	}
	if(!found_end)
	{
		output(error,"End of commands not found.");
		exit(-1);
	}

	return cci;
}
int replaceVariables(std::string* p_arg_str, PreserveVariableLevel pvl)
{
	if(pvl == all) return 0;

	std::regex variable_pattern = std::regex(variable_pattern_str);
	std::sregex_iterator v_str_it = std::sregex_iterator((*p_arg_str).begin(),(*p_arg_str).end(),variable_pattern);
	std::regex arg_pattern(arg_pattern_str);
	std::sregex_iterator p_arg_str_it((*p_arg_str).begin(),(*p_arg_str).end(),arg_pattern);
	std::sregex_iterator end;

	if(p_arg_str_it != end) p_arg_str_it++;

	if(pvl == first && v_str_it != end && p_arg_str_it != end && v_str_it->str() == p_arg_str_it->str())
		v_str_it++;

	// For some reason, attempting to just use v_str_it in the std::regex_replace line to replace variables with their values
	// results in the iterator getting in some way lost causing it to not replace all the variables.
	// Seems to be limited to cases when the value length > variable length
	// Either way, using a std::vector<std::string> is an effective work around.
	std::vector<std::pair<std::string,long unsigned>> matches;
	while(v_str_it != end)
	{
		matches.push_back(std::pair<std::string,long unsigned>(v_str_it->str(),v_str_it->position()));
		v_str_it++;
	}
	long unsigned offset = 0;
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

	if(pvl == first && v_str_it != end) v_str_it++;

	if(v_str_it != end)
	{
		int replaceVariablesResult = replaceVariables(p_arg_str,pvl);
		if(replaceVariablesResult) return replaceVariablesResult;
	}
	return 0;
}
int runApp(std::string arg_str, bool redirect_output)
{
	//Special return statuses
	if(arg_str[0] == COMMENT) return 0;
	else if(left(arg_str,5) == "break") return STATUS_BREAK;
	else if(left(arg_str,8) == "continue") return STATUS_CONTINUE;

	//Snip out inline comment
	if(findu(arg_str,COMMENT) != std::string::npos)
		arg_str = left(arg_str,findu(arg_str,COMMENT));

	Configuration cfg = Configuration();
	Character c = Character();
	Campaign m = Campaign();
	Shell s = Shell();

	std::vector<std::string> args;
	extern char** environ;
	pid_t pid;

	std::string first_arg;
	while(arg_str[0] == ' ' || arg_str[0] == '\t') arg_str.erase(arg_str.begin());
	if(arg_str == "") return 0;
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
	if((v_str_it != end && v_str_it->str() == first_arg) || first_arg[0] == '(')
		arg_str = "eval " + arg_str;

	if(findu(arg_str," ") != std::string::npos) first_arg = left(arg_str,findu(arg_str," "));
	else					    first_arg = arg_str;

	//See if we need to preserve the argv[1] if it is a variable
	PreserveVariableLevel pvl = none;
	if(findu(arg_str,std::string(FLAG_MODIFYVARIABLES)) == std::string::npos)
	{
		GetAppOutputInfo info = getAppOutput(first_arg + " " + std::string(FLAG_MODIFYVARIABLES));

		if(!info.status) pvl = stopvl(info.output[0]);
		else return info.status;
	}

	//Push back program we are going to run
	std::string path = std::string(RPGSH_INSTALL_DIR);
	args.push_back(path+prefix+left(arg_str,findu(arg_str," ")));

	//Replace variables
	if(pvl != all)
	{
		int replaceVariablesResult = replaceVariables(&arg_str,pvl);
		if(replaceVariablesResult) return replaceVariablesResult;
	}

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

	//Merge explicit constructors
	for(long unsigned i=0; i<args.size(); i++)
	{
		long unsigned lcrlybrkt_ctr = countu(args[i],'{');
		long unsigned rcrlybrkt_ctr = countu(args[i],'}');

		if(lcrlybrkt_ctr == rcrlybrkt_ctr) continue;

		if(args[i] == "{" || args[i] == "}")
		{
			i++;
		}
		else if(lcrlybrkt_ctr < rcrlybrkt_ctr)
		{
			output(error,"Expected \'{\' before \'}\'.");
			return -1;
		}
		else
		{
			i++;
			while(i < args.size())
			{
				long unsigned next_rcrlybrkt = -1;
				do
				{
					if(next_rcrlybrkt+1 > args[i].size()) break;
					next_rcrlybrkt = findu(args[i],'}',next_rcrlybrkt);
					if(next_rcrlybrkt != std::string::npos) rcrlybrkt_ctr++;
				}while(lcrlybrkt_ctr < rcrlybrkt_ctr || next_rcrlybrkt != std::string::npos);

				if(lcrlybrkt_ctr < rcrlybrkt_ctr)
					args[i-1] += " "+args[i];
				else
					args[i-1] += " "+left(args[i],next_rcrlybrkt);

				if(lcrlybrkt_ctr == rcrlybrkt_ctr && next_rcrlybrkt < args[i].length()-1 &&
				   findu(args[i],'}',next_rcrlybrkt+1) != std::string::npos)
				{
					output(error,"Expected \'{\' before \'}\'.");
					return -1;
				}
				args.erase(args.begin()+i);
				if(lcrlybrkt_ctr == rcrlybrkt_ctr) break;
			}
		}
	}

	//Merge args wrapped in quotes
	for(long unsigned i=0; i<args.size(); i++)
	{
		long unsigned quote_start = findu(args[i],'\"');
		long unsigned quote_end = findu(args[i],'\"',quote_start+1);
		long unsigned rcrlybrkt = findu(args[i],'{');
		if(quote_start == std::string::npos ||
		   (rcrlybrkt && rcrlybrkt < quote_start && isTypeSigil(args[i][rcrlybrkt-1]))) continue;

		std::string from_quote_start = right(args[i],quote_start+1);
		if(quote_end != std::string::npos)//Quotes contained in same arg
		{
			args[i] = left(from_quote_start,findu(from_quote_start,'\"'));
		}
		else
		{
			args[i] = from_quote_start;
			i++;
			while(i < args.size())
			{
				quote_end = findu(args[i],'\"');
				if(quote_end == std::string::npos)
					args[i-1] += " "+args[i];
				else
					args[i-1] += " "+left(args[i],quote_end);
				args.erase(args.begin()+i);
				if(quote_end != std::string::npos) break;
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
int runScript(std::string path)
{
	if(path[0] != '/' && left(path,2) != "./" && left(path,3) != "../")
		path = scripts_dir+path;

	if(!std::filesystem::exists(scripts_dir) ||
	   (std::filesystem::exists(scripts_dir) && !std::filesystem::is_directory(scripts_dir)))
	{
		output(info,"Scripts directory not found, creating scripts directory at \"%s\".",scripts_dir.c_str());
		std::filesystem::create_directory(scripts_dir);
	}

	if(!std::filesystem::exists(path))
	{
		output(error,"rpgsh script \"%s\" does not exist.",path.c_str());
		return -1;
	}

	std::string cmd, line;
	std::ifstream ifs(path);
	while(!ifs.eof())
	{
		getline(ifs,line);
		if(line != "" && (line.length() < 3 || (line.length() >= 3 && left(line,3) != "#!/")))
		{
			cmd += line;
			if(countu(cmd,'{') <= countu(cmd,'}'))
			{
				runApp(cmd,false);
				cmd = "";
			}
			else
			{
				cmd += ' ';
			}
		}
	}
	ifs.close();

	if(cmd != "")
	{
		output(error,"\'}\' not found before EOF.");
		return -1;
	}

	return 0;
}

void chkFlagAppDesc(char** _argv, std::string description)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"%s\n",description.c_str());
		exit(0);
	}
}
void chkFlagPreserveVariables(char** _argv, PreserveVariableLevel pvl)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_MODIFYVARIABLES))
	{
		fprintf(stdout,"%s\n",pvltos(pvl).c_str());
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
