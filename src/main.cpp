#include <climits>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <termios.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

Config cfg = Config();
Character c = Character();

bool containsNonSpaceChar(std::string str)
{
	for(const auto& ch : str)
		if(ch != ' ') return true;

	return false;
}
void addKeyToMatches(std::vector<std::string>** ppMatches, std::string k, std::string chk_str, std::string key)
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
void addKeysToMatches(std::vector<std::string>* pMatches, Scope scope, std::string chk_str, std::string key, char type)
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
void addPropertyToMatches(std::vector<std::string>** ppMatches, std::string chk_str, std::string property,
			  std::string property_str)
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
		for(auto& [c,q] : scope.get<Wallet>(key))
			addPropertyToMatches(&pMatches,chk_str,property,std::string(c));
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
std::string inputHandler()
{
	#define KB_TAB		9
	#define KB_ENTER	10
	#define KB_BACKSPACE	127

	//Set terminal flags for non-buffered reading required for handling keyboard input
	struct termios t_old, t_new;
	tcgetattr(fileno(stdin), &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(fileno(stdin), TCSANOW, &t_new);

	bool insert_mode = false;
	char k = 0;
	char esc_char = 0;
	long unsigned int cur_pos = 0;
	int tab_ctr = 0;
	std::string last_match = "";
	std::vector<char> input;
	std::vector<std::string> history = getAppOutput("history").output;
	unsigned long history_len = history.size();
	unsigned long history_ctr = history_len-1;

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
			if(containsNonSpaceChar(last_match) && findInVect<char>(input,' ',cur_pos) == -1)
				cur_pos += (input.size()-cur_pos);
			else if(containsNonSpaceChar(last_match))
				cur_pos = findInVect<char>(input,' ',cur_pos);
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
		else if((k == KB_TAB || esc_char == 'Z') && input.size() &&
			(cur_pos == input.size() || input[cur_pos] == ')' || input[cur_pos] == ' ' || last_match != ""))//Tab (completion)
		{
			if(k == KB_TAB) tab_ctr++;
			else tab_ctr--;

			Scope tab_comp_scope;
			std::string match = "";
			std::vector<std::string> matches;

			//Create the string which is to be completed
			std::string chk_str = "";
			for(int i=cur_pos-1; i>=0&&input[i]!=' '; i--)
				chk_str.insert(0,std::string(1,input[i]));

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
			char type_sigil = chk_str[findu(chk_str,'/',findu(chk_str,']')+1)-1];
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
					input.erase(input.begin()+cur_pos);
			}

			//Insert match into input
			for(long unsigned int i=0; i<match.length()-chk_str.length(); i++)
				input.insert(input.begin()+cur_pos+i,match[i+chk_str.length()]);

			//Reprint input
			if(last_match != "" && match != "" && containsNonSpaceChar(match))
			{
				fprintf(stdout,CURSOR_LEFT_N,last_match.length()-chk_str.length());
				fprintf(stdout,CLEAR_TO_LINE_END);
			}

			for(long unsigned int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);

			//CURSOR_LEFT_N(0) still pushes cursor to the left, so we need to check
			unsigned int spaces = 0;
			for(const auto& i : match) if(i == ' ') spaces++;
			unsigned int offset = match.length()-spaces;
			char base_char = (rfindu(chk_str,'/') == std::string::npos ? '[' : '/');
			if(cur_pos+offset < input.size())
				fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos-offset+(chk_str.length()-(rfindu(chk_str,base_char)+1)));

			last_match = match;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			switch(esc_char)
			{
				case 'A':	//Up
				case 'B':	//Down
					if(cur_pos > 0) fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					fprintf(stdout,CLEAR_TO_LINE_END);
					input.clear();
					if(esc_char == 'B' && history_ctr == history_len-1)
					{
						cur_pos = 0;
						break;
					}
					if     (esc_char == 'A' && history_ctr > 0)		history_ctr--;
					else if(esc_char == 'B' && history_ctr < history_len-1)	history_ctr++;
					for(const auto& c : history[history_ctr])
					{
						input.push_back(c);
						fprintf(stdout,"%c",c);
					}
					cur_pos = input.size();
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
				case '6':	//PgDown
					if(getchar() != '~') continue;
					if(cur_pos > 0) fprintf(stdout,CURSOR_LEFT_N,cur_pos);
					fprintf(stdout,CLEAR_TO_LINE_END);
					input.clear();
					if     (esc_char == '5') history_ctr = 0;
					else if(esc_char == '6') history_ctr = history_len-2;
					for(const auto& c : history[history_ctr])
					{
						input.push_back(c);
						fprintf(stdout,"%c",c);
					}
					cur_pos = input.size();
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
	}

	//Reset terminal flags in-case of sudden program termination
	tcsetattr(fileno(stdin), TCSANOW, &t_old);

	//Strings need to be null-terminated
	input.push_back('\0');
	return input.data();
}
int prompt()
{
	#define MAX_BUFFER_SIZE 65535 //Not sure why UINT_MAX segfaults, but hard-coding the value of UINT_MAX works fine

	confirmHistoryFile();

	bool backup = false;

	prompt:
	c = Character();

	try
	{
		if(c.keyExists<Var>(DOT_PROMPT))
		{
			long unsigned int last_line_length;
			for(const auto& line : getAppOutput("print -r "+c.getStr<Var>(DOT_PROMPT)).output)
			{
				if(line.length() <= 1) continue;
				last_line_length = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stripFormatting(line)).length();//TODO: Remove when we convert everything to std::wstring
				fprintf(stdout,"%s\n",line.c_str());
			}
			fprintf(stdout,CURSOR_SET_COL_N,(long unsigned int)0);
			fprintf(stdout,CURSOR_UP);
			fprintf(stdout,CURSOR_RIGHT_N,last_line_length);
		}
		else
		{
			fprintf(stdout,"%s%s%s %s>%s ",TEXT_BOLD,TEXT_RED,c.getName().c_str(),TEXT_WHITE,TEXT_NORMAL);
		}

		if(backup)
		{
			c.save();
			backup = false;
		}
	}
	catch(...)
	{
		if(!backup)
		{
			output(Warning,"Error while displaying prompt. One or more character attributes is not a valid integer. Attempting to load backup.");
			backup = true;
			goto prompt;
		}
		else
		{
			output(Error,"Loading from backup was unsuccessful. Manual intervention required to restore file.\n\nGood luck.\n");
			exit(-1);
		}
	}

	char buffer[MAX_BUFFER_SIZE];

	//Zero-out buffer so we have a known dataset
	for(unsigned int i=0; i<MAX_BUFFER_SIZE; i++)
		buffer[i] = '\0';

	std::string in = inputHandler();
	if(in.length() > MAX_BUFFER_SIZE)
	{
		fprintf(stdout,"\n\n");
		output(Error,"Input too big (%llu characters)! Please enter <= %lu characters.",in.length(),MAX_BUFFER_SIZE);
		strcpy(buffer,"");
	}
	else
	{
		strncpy(buffer,in.c_str(),in.length()+1);
	}

	fprintf(stdout,"\n");
	if(strcmp(buffer,""))
	{
		if(!strcmp(buffer,"exit"))
		{
			fprintf(stdout,"Exiting...\n");
			return 1; //Non-zero so we can exit, and positive so user can discriminate between good exits and bad exits
		}

		//Auto-escape colons so as to not cause issues with variable sets
		std::regex colon(":");
		std::string tmp_buffer = std::regex_replace(std::string(buffer),colon,"\\:");
		for(unsigned int i=0; i<tmp_buffer.length(); i++)
			buffer[i]=tmp_buffer[i];

		padding();
		(void)runApp(buffer,false);
		padding();

		//Handle rpgsh history
		unsigned long line_count = 0;
		std::ifstream ifs(history_path);
		std::vector<std::string> history;
		std::string data;
		while(!ifs.eof())
		{
			std::getline(ifs,data);
			if(ifs.eof()) break;
			history.push_back(data);
			line_count++;
		}
		ifs.close();

		history.push_back(buffer);
		unsigned long history_len = 0;
		try
		{
			history_len = std::stol(cfg.setting[HISTORY_LENGTH]);
		}
		catch(...)
		{
			output(Error,"Invalid value for \"%s\" setting: %s\n",HISTORY_LENGTH,cfg.setting[HISTORY_LENGTH].c_str());
			exit(-1);
		}

		if(line_count >= history_len)
		{
			line_count = line_count-history_len+1;
			std::ofstream ofs(history_path);
			for(unsigned long i=line_count; i<=history_len; i++)
				ofs<<history[i]+"\n";
			ofs.close();
		}
		else
		{
			std::ofstream ofs(history_path);
			for(unsigned long i=0; i<history.size(); i++)
				ofs<<history[i]+"\n";
			ofs.close();
		}
	}
	return 0;
}
int main()
{
	//Generate cache of valid rpgsh programs to speed up "help" program
	//It takes a noticable amount of time to search through all of /bin even on an i7-4700k
	fprintf(stdout,"Generating rpgsh program list...");
	std::vector<std::string> applications = getDirectoryListing(std::string(RPGSH_INSTALL_DIR));
	std::vector<std::string> rpgsh_apps;
	std::filesystem::remove(rpgsh_programs_cache_path);
	for(const auto& app : applications)
		if(!findu(app,prefix)) rpgsh_apps.push_back(app);

	//Basic alphabetical sort, important for tab completion
	sort<std::string>(&rpgsh_apps);

	//Write alphabetized list to file
	std::ofstream ofs(rpgsh_programs_cache_path);
	for(const auto& app : rpgsh_apps)
		ofs<<app<<"\n";
	ofs.close();

	fprintf(stdout,CLEAR_ENTIRE_LINE);//Delete "Generating..." line from start of main()

	std::string startup_apps = cfg.setting[STARTUP_APPS];
	while(true)
	{
		if(findu(startup_apps,',') == std::string::npos)
		{
			(void)runApp(startup_apps.c_str(),false);
			break;
		}
		else
		{
			(void)runApp(left(startup_apps,findu(startup_apps,',')).c_str(),false);
			startup_apps = right(startup_apps,findu(startup_apps,',')+1);
		}
	}

	if(!stob(cfg.setting[HIDE_TIPS]))
	{
		fprintf(stdout,"\n");
		fprintf(stdout,"%s%sTIP: %sType %s%shelp%s for a list of currently supported commands.\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
		fprintf(stdout,"%s%sTIP: %sFor more info about a given command, try using the %s%s-?%s or %s%s--help%s flags.\n\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
	}

	while(prompt() == 0);

	return 0;
}
