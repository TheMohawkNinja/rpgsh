#include <cstring>
#include <regex>
#include "../headers/colors.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

void printHeader(std::string s)
{
	fprintf(stdout,"%s%s %s %s\n",TEXT_GREEN,TEXT_BOLD,s.c_str(),TEXT_NORMAL);
	fprintf(stdout,"%s",TEXT_WHITE);
	for(long unsigned int i=0; i<s.length()+2; i++)
		fprintf(stdout,"─");
	fprintf(stdout,"%s\n",TEXT_NORMAL);
}
template <typename T>
void getLongestKey(datamap<T> m, unsigned int* p_current_longest_len)
{
	for(auto& [k,v] : m)
	{
		//Get longest key length, omitting hidden variables
		if(k[0] != '.' && k.length() > (*p_current_longest_len))
			(*p_current_longest_len) = k.length();
	}
}
const char* printValue(std::string value)
{
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

	value += std::string(TEXT_NORMAL); //Make sure we don't carry over any unterminated formatting
	return value.c_str();
}
void printData(Scope scope)
{
	for(auto& [k,v] : scope.getDatamap<Var>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sVar%s%s",TEXT_BOLD,VAR_COLOR,TEXT_NORMAL,addSpaces(COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sValue:  %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Value));
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Dice>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sDice%s%s",TEXT_BOLD,DICE_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		if(v.List != "")
		{
			fprintf(stdout,"%sList:         %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.List));
		}
		else
		{
			fprintf(stdout,"%sQuantity:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.Quantity)));
			fprintf(stdout,"%sFaces:        %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.Faces)));
			fprintf(stdout,"%sModifier:     %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.Modifier)));
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Wallet>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;

		//Get longest currency name
		long unsigned int longest_cur = 0;
		for(auto& [c,q] : v)
		{
			if(!q) continue;
			if(c.Name.length() > longest_cur)
				longest_cur = c.Name.length();
		}

		fprintf(stdout,"%s%sWallet%s%s",TEXT_BOLD,WALLET_COLOR,TEXT_NORMAL,addSpaces(longest_cur+COLUMN_PADDING-5).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);

		//Print values
		for(auto& [c,q] : v)
		{
			if(!q) continue;

			fprintf(stdout,"%s%s:%s%s",TEXT_ITALIC,c.Name.c_str(),TEXT_NORMAL,addSpaces(longest_cur-c.Name.length()+COLUMN_PADDING).c_str());
			fprintf(stdout,"%s\n",printValue(std::to_string(q)));
		}
		fprintf(stdout,"\n");
	}
	for(auto& [k,v] : scope.getDatamap<Currency>())
	{
		//Skip hidden variables
		if(k[0] == '.') continue;
		fprintf(stdout,"%s%sCurrency%s%s",TEXT_BOLD,CURRENCY_COLOR,TEXT_NORMAL,addSpaces(2*COLUMN_PADDING).c_str());
		fprintf(stdout,"%s%s%s%s%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_WHITE,k.c_str(),TEXT_NORMAL);
		fprintf(stdout,"%sSystem:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.System));
		fprintf(stdout,"%sName:             %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Name));
		fprintf(stdout,"%sSmallerAmount:    %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(std::to_string(v.SmallerAmount)));
		fprintf(stdout,"%sSmaller:          %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Smaller));
		fprintf(stdout,"%sLarger:           %s%s\n",TEXT_ITALIC,TEXT_NORMAL,printValue(v.Larger));
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\b");//Remove extraneous newline
}
void printCharacterVariables()
{
	Character c = Character();
	std::string sigil(1,CHARACTER_SIGIL);
	printHeader("("+sigil+") "+c.getName());
	printData(c);
}
void printCampaignVariables()
{
	confirmCampaignVariablesFile();
	Campaign m = Campaign();
	std::string sigil(1,CAMPAIGN_SIGIL);
	std::string m_name = getEnvVariable(ENV_CURRENT_CAMPAIGN);
	printHeader("("+sigil+") "+left(m_name,m_name.length()-1));// Omit trailing '/'
	printData(m);
}
void printShellVariables()
{
	Shell s = Shell();
	std::string sigil(1,SHELL_SIGIL);
	printHeader("("+sigil+") "+"Shell");
	printData(s);
}
int main(int argc, char** argv)
{
	chkFlagAppDesc(argv,"Pretty prints variables, variable sets, and scopes.");
	chkFlagModifyVariables(argv,false);

	if(argc > 2)
		output(Warning,"Expected only one argument. All args past \"%s\" will be ignored.",argv[1]);

	if(argc == 1)
	{
		printShellVariables();
		fprintf(stdout,"\n");
		printCampaignVariables();
		fprintf(stdout,"\n");
		printCharacterVariables();
	}
	else if(isTypeSigil(argv[1][0]) && argv[1][1] == '{')
	{
		Scope variable;
		std::string v_str = std::string(argv[1]);

		try
		{
			switch(argv[1][0])
			{
				case(VAR_SIGIL):
					variable.set<Var>("",Var(v_str));
					break;
				case(DICE_SIGIL):
					variable.set<Dice>("",Dice(v_str));
					break;
				case(WALLET_SIGIL):
					variable.set<Wallet>("",Wallet(v_str));
					break;
				case(CURRENCY_SIGIL):
					variable.set<Currency>("",Currency(v_str));
					break;
			}
		}
		catch(const std::runtime_error& e)
		{
			output(Error,"Unable to pretty-print explicit constructor \"%s\": %s",v_str.c_str(),e.what());
			exit(-1);
		}
		printData(variable);
	}
	else if(looksLikeSet(std::string(argv[1])))
	{
		Scope set;

		for(const auto& [k,v] : getSet(std::string(argv[1])))
		{
			switch(v[0])
			{
				case VAR_SIGIL:
					set.set<Var>(k,v);
					break;
				case DICE_SIGIL:
					set.set<Dice>(k,v);
					break;
				case WALLET_SIGIL:
					set.set<Wallet>(k,v);
					break;
				case CURRENCY_SIGIL:
					set.set<Currency>(k,v);
					break;
			}
		}

		printData(set);
	}
	else if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tprint\n");
		fprintf(stdout,"\tprint [%svariable%s | %svariable set%s] [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints all variables in all scopes.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
	}
	else
	{
		output(Error,"Unknown option \"%s\".",argv[1]);
		exit(-1);
	}
}
