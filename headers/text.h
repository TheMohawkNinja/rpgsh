//Text formatting
//https://en.wikipedia.org/wiki/ANSI_escape_code
#define TEXT_NORMAL			"\e[0m"		//Resets all other text formatting
#define TEXT_BOLD			"\e[1m"
#define TEXT_DIM			"\e[2m"
#define TEXT_ITALIC			"\e[3m"
#define TEXT_UNDERLINE			"\e[4m"
#define TEXT_BLINK			"\e[5m"
#define TEXT_RAPIDBLINK			"\e[6m"		//Unsupported, just here for list completeness
#define TEXT_REVERSE			"\e[7m"
#define TEXT_HIDDEN			"\e[8m"		//Unsupported, just here for list completeness
#define TEXT_STRIKETHROUGH		"\e[9m"		//Unsupported, just here for list completeness
#define TEXT_DEFAULTFONT		"\e[10m"	//Same as 0?
#define TEXT_ALTFONT1			"\e[11m"
#define TEXT_ALTFONT2			"\e[12m"
#define TEXT_ALTFONT3			"\e[13m"
#define TEXT_ALTFONT4			"\e[14m"
#define TEXT_ALTFONT5			"\e[15m"
#define TEXT_ALTFONT6			"\e[16m"
#define TEXT_ALTFONT7			"\e[17m"
#define TEXT_ALTFONT8			"\e[18m"
#define TEXT_ALTFONT9			"\e[19m"
#define TEXT_GOTHIC			"\e[20m"	//Unsupported, just here for list completeness
#define TEXT_NOBOLD			"\e[21m"	//May be double-underline in some cases? (EMCA-48)
#define TEXT_NORMALINTENSITY		"\e[22m"	//Resets bold/dim
#define TEXT_NOITALIC			"\e[23m"	//Also resets "blackletter"
#define TEXT_NOUNDERLINE		"\e[24m"
#define TEXT_NOBLINK			"\e[25m"
#define TEXT_PROPSPACING		"\e[26m"	//"Proportional spacing", Likely unsupported
#define TEXT_NOREVERSE			"\e[27m"
#define TEXT_NOHIDDEN			"\e[28m"
#define TEXT_NOSTRIKETHROUGH		"\e[29m"
#define TEXT_BLACK			"\e[30m"
#define TEXT_RED			"\e[31m"
#define TEXT_GREEN			"\e[32m"
#define TEXT_YELLOW			"\e[33m"
#define TEXT_BLUE			"\e[34m"
#define TEXT_MAGENTA			"\e[35m"
#define TEXT_CYAN			"\e[36m"
#define TEXT_LIGHTGRAY			"\e[37m"
#define TEXT_COLOR			"\e[38m"	//Next arguments are 5;n or 2;r;g;b
#define TEXT_DEFAULTCOLOR		"\e[39m"	//Implementation-specific
#define TEXT_BG_BLACK			"\e[40m"
#define TEXT_BG_RED			"\e[41m"
#define TEXT_BG_GREEN			"\e[42m"
#define TEXT_BG_YELLOW			"\e[43m"
#define TEXT_BG_BLUE			"\e[44m"
#define TEXT_BG_MAGENTA			"\e[45m"
#define TEXT_BG_CYAN			"\e[46m"
#define TEXT_BG_LIGHTGRAY		"\e[47m"
#define TEXT_BG_COLOR			"\e[48m"	//Next arguments are 5;n or 2;r;g;b
#define TEXT_BG_DEFAULTCOLOR		"\e[49m"
#define TEXT_NOPROPSPACING		"\e[50m"
#define TEXT_FRAME			"\e[51m"	//Implemented as "emoji variation selector" in mintty
#define TEXT_ENCIRCLE			"\e[52m"	//Implemented as "emoji variation selector" in mintty
#define TEXT_OVERLINE			"\e[53m"
#define TEXT_NOFRAME_NOENCIRCLE		"\e[54m"	//Resets 52 and 53
#define TEXT_NOOVERLINE			"\e[55m"
#define TEXT_UNDERLINECOLOR		"\e[58m"	//Next arguments are 5;n or 2;r;g;b
#define TEXT_DEFAULTUNDERLINECOLOR	"\e[59m"
#define TEXT_IDEO_UNDER			"\e[60m"	//Ideogram underline or right side line
#define TEXT_IDEO_DOUBLEUNDER		"\e[61m"	//Ideogram double underline or double right side line
#define TEXT_IDEO_OVER			"\e[62m"	//Ideogram overline or left side line
#define TEXT_IDEO_DOUBLEOVER		"\e[63m"	//Ideogram double overline or double left side line
#define TEXT_IDEO_STRESS		"\e[64m"
#define TEXT_IDEO_DEFAULT		"\e[65m"	//Resets 60-64
#define TEXT_SUPER			"\e[73m"	//Implemented only in mintty
#define TEXT_SUB			"\e[74m"	//Implemented only in mintty
#define TEXT_NOSUB_NOSUPER		"\e[75m"	//Resets 73 and 74
#define TEXT_DARKGRAY			"\e[90m"
#define TEXT_LIGHTRED			"\e[91m"
#define TEXT_LIGHTGREEN			"\e[92m"
#define TEXT_LIGHTYELLOW		"\e[93m"
#define TEXT_LIGHTBLUE			"\e[94m"
#define TEXT_LIGHTMAGENTA		"\e[95m"
#define TEXT_LIGHTCYAN			"\e[96m"
#define TEXT_WHITE			"\e[97m"
#define TEXT_BG_DARKGRAY		"\e[100m"
#define TEXT_BG_LIGHTRED		"\e[101m"
#define TEXT_BG_LIGHTGREEN		"\e[102m"
#define TEXT_BG_LIGHTYELLOW		"\e[103m"
#define TEXT_BG_LIGHTBLUE		"\e[104m"
#define TEXT_BG_LIGHTMAGENTA		"\e[105m"
#define TEXT_BG_LIGHTCYAN		"\e[106m"
#define TEXT_BG_WHITE			"\e[107m"

//Cursor movement
//https://github.com/fabricio-p/c-ansi-sequences/blob/main/cursor.h
#define _CURSOR_SET(l,c)		"\e["#l";"#c"H"
#define CURSOR_SET			_CURSOR_SET(%d,%d)
#define CURSOR_UP			"\e[A"
#define _CURSOR_UP_N(n)			"\e["#n"A"
#define CURSOR_UP_N			_CURSOR_UP_N(%d)
#define CURSOR_DOWN			"\e[B"
#define _CURSOR_DOWN_N(n)		"\e["#n"B"
#define CURSOR_DOWN_N			_CURSOR_DOWN_N(%d)
#define CURSOR_RIGHT			"\e[C"
#define _CURSOR_RIGHT_N(n)		"\e["#n"C"
#define CURSOR_RIGHT_N			_CURSOR_RIGHT_N(%d)
#define CURSOR_LEFT			"\e[D"
#define _CURSOR_LEFT_N(n)		"\e["#n"D"
#define CURSOR_LEFT_N			_CURSOR_LEFT_N(%d)
#define CURSOR_DOWN_HOME		"\e[E"
#define _CURSOR_DOWN_HOME_N(n)		"\e["#n"E"
#define CURSOR_DOWN_HOME_N		_CURSOR_DOWN_HOME_N(%d)
#define CURSOR_UP_HOME			"\e[F"
#define _CURSOR_UP_HOME_N(n)		"\e["#n"F"
#define CURSOR_UP_HOME_N		_CURSOR_UP_HOME_N(%d)
#define CURSOR_HOME			"\e[G"
#define _CURSOR_SET_COL_N(n)		"\e["#n"G"
#define CURSOR_SET_COL_N		_CURSOR_SET_COL_N(%d)
#define CURSOR_SET_HOME			"\e[H"

//Screen and line clearing
#define CLEAR_TO_SCREEN_END		"\e[J"		//Clears to screen end
#define CLEAR_TO_SCREEN_START		"\e[1J"		//Clears from cursor to start of screen
#define CLEAR_SCREEN			"\e[2J"		//Clears entire screen (like a "clear" command in bash)
#define CLEAR_LINE			"\e[K"		//Clears current line
#define CLEAR_TO_LINE_END		"\e[0K"		//Clears from cursor to end of line
#define CLEAR_TO_LINE_START		"\e[1K"		//Clears from cursor to beginning of line
#define CLEAR_ENTIRE_LINE		"\e[2K"		//Clears entire line, seems to behave identical to K
