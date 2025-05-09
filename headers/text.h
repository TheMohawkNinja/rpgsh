//https://en.wikipedia.org/wiki/ANSI_escape_code
//https://github.com/fabricio-p/c-ansi-sequences/blob/main/cursor.h

#define ESC_SEQ				'\033'

//Text formatting
#define TEXT_NORMAL			"\033[0m"		//Resets all other text formatting
#define TEXT_BOLD			"\033[1m"
#define TEXT_DIM			"\033[2m"
#define TEXT_ITALIC			"\033[3m"
#define TEXT_UNDERLINE			"\033[4m"
#define TEXT_BLINK			"\033[5m"
#define TEXT_RAPIDBLINK			"\033[6m"		//Unsupported
#define TEXT_REVERSE			"\033[7m"
#define TEXT_HIDDEN			"\033[8m"		//Unsupported
#define TEXT_STRIKETHROUGH		"\033[9m"		//Unsupported
#define TEXT_DEFAULTFONT		"\033[10m"
#define TEXT_ALTFONT1			"\033[11m"
#define TEXT_ALTFONT2			"\033[12m"
#define TEXT_ALTFONT3			"\033[13m"
#define TEXT_ALTFONT4			"\033[14m"
#define TEXT_ALTFONT5			"\033[15m"
#define TEXT_ALTFONT6			"\033[16m"
#define TEXT_ALTFONT7			"\033[17m"
#define TEXT_ALTFONT8			"\033[18m"
#define TEXT_ALTFONT9			"\033[19m"
#define TEXT_GOTHIC			"\033[20m"		//Unsupported
#define TEXT_NOBOLD			"\033[21m"		//May be double-underline in some cases? (EMCA-48)
#define TEXT_NORMALINTENSITY		"\033[22m"		//Resets bold/dim
#define TEXT_NOITALIC			"\033[23m"		//Also resets "blackletter"
#define TEXT_NOUNDERLINE		"\033[24m"
#define TEXT_NOBLINK			"\033[25m"
#define TEXT_PROPSPACING		"\033[26m"		//"Proportional spacing", likely unsupported
#define TEXT_NOREVERSE			"\033[27m"
#define TEXT_NOHIDDEN			"\033[28m"
#define TEXT_NOSTRIKETHROUGH		"\033[29m"
#define TEXT_BLACK			"\033[30m"
#define TEXT_RED			"\033[31m"
#define TEXT_GREEN			"\033[32m"
#define TEXT_YELLOW			"\033[33m"
#define TEXT_BLUE			"\033[34m"
#define TEXT_MAGENTA			"\033[35m"
#define TEXT_CYAN			"\033[36m"
#define TEXT_LIGHTGRAY			"\033[37m"
#define TEXT_FG_4BIT_PRE		"\033[38;5;"
#define _TEXT_FG_4BIT(n)		TEXT_FG_4BIT_PRE#n"m"	//0-15, 0-7 low intensity, 8-15 high intensity
#define TEXT_FG_4BIT			_TEXT_FG_4BIT(%u)
#define TEXT_FG_8BIT_PRE		"\033[38;2;"
#define _TEXT_FG_8BIT(r,g,b)		TEXT_FG_8BIT_PRE#r";"#g";"#b"m"
#define TEXT_FG_8BIT			_TEXT_FG_8BIT(%u,%u,%u)
#define TEXT_DEFAULTCOLOR		"\033[39m"		//Implementation-specific
#define TEXT_BG_BLACK			"\033[40m"
#define TEXT_BG_RED			"\033[41m"
#define TEXT_BG_GREEN			"\033[42m"
#define TEXT_BG_YELLOW			"\033[43m"
#define TEXT_BG_BLUE			"\033[44m"
#define TEXT_BG_MAGENTA			"\033[45m"
#define TEXT_BG_CYAN			"\033[46m"
#define TEXT_BG_LIGHTGRAY		"\033[47m"
#define TEXT_BG_4BIT_PRE		"\033[48;5;"
#define _TEXT_BG_4BIT(n)		TEXT_BG_4BIT_PRE#n"m"	//0-15, 0-7 low intensity, 8-15 high intensity
#define TEXT_BG_4BIT			_TEXT_BG_4BIT(%u)
#define TEXT_BG_8BIT_PRE		"\033[48;2;"
#define _TEXT_BG_8BIT(r,g,b)		TEXT_BG_8BIT_PRE#r";"#g";"#b"m"
#define TEXT_BG_8BIT			_TEXT_BG_8BIT(%u,%u,%u)
#define TEXT_BG_DEFAULTCOLOR		"\033[49m"
#define TEXT_NOPROPSPACING		"\033[50m"
#define TEXT_FRAME			"\033[51m"		//Implemented as "emoji variation selector" in mintty
#define TEXT_ENCIRCLE			"\033[52m"		//Implemented as "emoji variation selector" in mintty
#define TEXT_OVERLINE			"\033[53m"
#define TEXT_NOFRAME_NOENCIRCLE		"\033[54m"		//Resets 52 and 53
#define TEXT_NOOVERLINE			"\033[55m"
#define TEXT_UL_4BIT_PRE		"\033[58;5;"
#define _TEXT_UL_4BIT(n)		TEXT_UL_4BIT_PRE#n"m"	//0-15, 0-7 low intensity, 8-15 high intensity
#define TEXT_UL_4BIT			_TEXT_UL_4BIT(%u)
#define TEXT_UL_8BIT_PRE		"\033[58;2;"
#define _TEXT_UL_8BIT(r,g,b)		TEXT_UL_8BIT_PRE#r";"#g";"#b"m"
#define TEXT_UL_8BIT			_TEXT_UL_8BIT(%u,%u,%u)
#define TEXT_DEFAULTUL			"\033[59m"
#define TEXT_IDEO_UNDER			"\033[60m"		//Ideogram underline or right side line
#define TEXT_IDEO_DOUBLEUNDER		"\033[61m"		//Ideogram double underline or double right side line
#define TEXT_IDEO_OVER			"\033[62m"		//Ideogram overline or left side line
#define TEXT_IDEO_DOUBLEOVER		"\033[63m"		//Ideogram double overline or double left side line
#define TEXT_IDEO_STRESS		"\033[64m"
#define TEXT_IDEO_DEFAULT		"\033[65m"		//Resets 60-64
#define TEXT_SUPER			"\033[73m"		//Implemented only in mintty
#define TEXT_SUB			"\033[74m"		//Implemented only in mintty
#define TEXT_NOSUB_NOSUPER		"\033[75m"		//Resets 73 and 74
#define TEXT_DARKGRAY			"\033[90m"
#define TEXT_LIGHTRED			"\033[91m"
#define TEXT_LIGHTGREEN			"\033[92m"
#define TEXT_LIGHTYELLOW		"\033[93m"
#define TEXT_LIGHTBLUE			"\033[94m"
#define TEXT_LIGHTMAGENTA		"\033[95m"
#define TEXT_LIGHTCYAN			"\033[96m"
#define TEXT_WHITE			"\033[97m"
#define TEXT_BG_DARKGRAY		"\033[100m"
#define TEXT_BG_LIGHTRED		"\033[101m"
#define TEXT_BG_LIGHTGREEN		"\033[102m"
#define TEXT_BG_LIGHTYELLOW		"\033[103m"
#define TEXT_BG_LIGHTBLUE		"\033[104m"
#define TEXT_BG_LIGHTMAGENTA		"\033[105m"
#define TEXT_BG_LIGHTCYAN		"\033[106m"
#define TEXT_BG_WHITE			"\033[107m"

//Cursor movement
#define _CURSOR_SET(l,c)		"\033["#l";"#c"H"
#define CURSOR_SET			_CURSOR_SET(%lu,%lu)
#define CURSOR_UP			"\033[A"
#define _CURSOR_UP_N(n)			"\033["#n"A"
#define CURSOR_UP_N			_CURSOR_UP_N(%lu)
#define CURSOR_DOWN			"\033[B"
#define _CURSOR_DOWN_N(n)		"\033["#n"B"
#define CURSOR_DOWN_N			_CURSOR_DOWN_N(%lu)
#define CURSOR_RIGHT			"\033[C"
#define _CURSOR_RIGHT_N(n)		"\033["#n"C"
#define CURSOR_RIGHT_N			_CURSOR_RIGHT_N(%lu)
#define CURSOR_LEFT			"\033[D"
#define _CURSOR_LEFT_N(n)		"\033["#n"D"
#define CURSOR_LEFT_N			_CURSOR_LEFT_N(%lu)
#define CURSOR_DOWN_HOME		"\033[E"	//Move to next line down and go to column 0
#define _CURSOR_DOWN_HOME_N(n)		"\033["#n"E"	//Move n lines down and go to column 0
#define CURSOR_DOWN_HOME_N		_CURSOR_DOWN_HOME_N(%lu)
#define CURSOR_UP_HOME			"\033[F"	//Move to next line up and go to column 0
#define _CURSOR_UP_HOME_N(n)		"\033["#n"F"	//Move n lines up and go to column 0
#define CURSOR_UP_HOME_N		_CURSOR_UP_HOME_N(%lu)
#define CURSOR_HOME			"\033[G"
#define _CURSOR_SET_COL_N(n)		"\033["#n"G"
#define CURSOR_SET_COL_N		_CURSOR_SET_COL_N(%lu)
#define CURSOR_SET_HOME			"\033[H"
#define CURSOR_HIDE			"\033[?25l"
#define CURSOR_SHOW			"\033[?25h"

//Screen and line clearing
#define CLEAR_TO_SCREEN_END		"\033[J"	//Clears to screen end
#define CLEAR_TO_SCREEN_START		"\033[1J"	//Clears from cursor to start of screen
#define CLEAR_SCREEN			"\033[2J"	//Clears entire screen (like "clear" in bash)
#define CLEAR_LINE			"\033[K"	//Clears current line
#define CLEAR_TO_LINE_END		"\033[0K"	//Clears from cursor to end of line
#define CLEAR_TO_LINE_START		"\033[1K"	//Clears from cursor to beginning of line
#define CLEAR_ENTIRE_LINE		"\033[2K"	//Clears entire line, seems to behave identical to K
