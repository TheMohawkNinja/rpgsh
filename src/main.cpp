#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <termios.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/scope.h"
#include "../headers/text.h"

#define MAX_BUFFER_SIZE 256

Config config = Config();
Character c = Character(false);

bool containsNonSpaceChar(std::string str)
{
	for(const auto& character : str)
	{
		if(character != ' ')
			return true;
	}

	return false;
}
void addKeyToMatches(std::vector<std::string>** ppMatches, std::string k, std::string chk_str, std::string key)
{
	if(!stringcasecmp(left(k,key.length()),key) && k[0] != '.')
		(**ppMatches).push_back(addSpaces(chk_str.length())+right(k,key.length()));
}
void addKeysToMatches(std::vector<std::string>* pMatches, Scope scope, std::string chk_str, std::string key, char type)
{
	switch(type)
	{
		case CURRENCY_SIGIL:
			for(auto& [k,v] : scope.getDatamap<Currency>())
				addKeyToMatches(&pMatches,k,chk_str,key);
			break;
		case CURRENCYSYSTEM_SIGIL:
			for(auto& [k,v] : scope.getDatamap<CurrencySystem>())
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
			    std::string key, std::string property, char type)
{
	switch(type)
	{
		case CURRENCY_SIGIL:
			addPropertyToMatches(&pMatches,chk_str,property,"CurrencySystem");
			addPropertyToMatches(&pMatches,chk_str,property,"Name");
			addPropertyToMatches(&pMatches,chk_str,property,"SmallerAmount");
			addPropertyToMatches(&pMatches,chk_str,property,"Smaller");
			addPropertyToMatches(&pMatches,chk_str,property,"Larger");
			break;
		case CURRENCYSYSTEM_SIGIL:
			addPropertyToMatches(&pMatches,chk_str,property,"Name");
			break;
		case DICE_SIGIL:
			addPropertyToMatches(&pMatches,chk_str,property,"Quantity");
			addPropertyToMatches(&pMatches,chk_str,property,"Faces");
			addPropertyToMatches(&pMatches,chk_str,property,"Modifier");
			break;
		case VAR_SIGIL:
			addPropertyToMatches(&pMatches,chk_str,property,"Value");
			break;
		case WALLET_SIGIL:
			for(auto& [c,q] : scope.get<Wallet>(key))
				addPropertyToMatches(&pMatches,chk_str,property,std::string(c));
			break;
	}
}
std::string input_handler()
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
	int cur_pos = 0;
	int tab_ctr = 0;
	std::string last_match = "";
	std::vector<char> input;

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
			if(containsNonSpaceChar(last_match))
				cur_pos += (input.size()-cur_pos);
			tab_ctr = 0;
			last_match = "";
		}

		if(isprint(k))//Printable characters
		{
			if(insert_mode)	//If the "Insert" key is toggled
			{
				if(cur_pos < input.size())
					input[cur_pos] = k;
				else
					input.push_back(k);
			}
			else
			{
				input.insert(input.begin()+cur_pos,k);
			}

			fprintf(stdout,CLEAR_LINE);

			for(int i=cur_pos; i<input.size(); i++)
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

			for(int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);
			if(cur_pos < input.size())
				fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);
		}
		else if((k == KB_TAB || esc_char == 'Z') && input.size() &&
			(cur_pos == input.size() || last_match != ""))//Tab (completion)
		{
			if(k == KB_TAB)
				tab_ctr++;
			else
				tab_ctr--;

			Scope tab_comp_scope;
			std::string match = "";
			std::vector<std::string> matches;

			//Create the string which is to be completed
			std::string chk_str = "";
			for(int i=cur_pos-1; i>=0&&input[i]!=' '; i--)
				chk_str.insert(0,std::string(1,input[i]));

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
					int app_ln = right(app,prefix.length()).length();
					if(app_chk_str == chk_str && app_ln > chk_str.length())
						matches.push_back(right(app,prefix.length()));
				}
				ifs.close();
			}
			else if(chk_str[0] == CHARACTER_SIGIL)//Character variables
			{
				tab_comp_scope = Character(false);
				std::string xref_path = campaigns_dir+
							get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
							"characters/";

				if(chk_str[1] == '[' &&
				   chk_str.find(']') == std::string::npos &&
				   chk_str.find('/') == std::string::npos)//Complete xref (same campaign)
				{
					for(auto& c : getDirectoryListing(xref_path))
					{
						if(right(c,c.length()-5) != ".char")//Don't include backup files or non-character files
							continue;

						std::string xref = left(c,c.find(".char"));
						std::string chk_str_xref = right(chk_str,2);
						if(chk_str.length() == 2 ||
						   (!stringcasecmp(chk_str_xref,left(xref,chk_str.length()-2)) &&
						   xref.length() > chk_str_xref.length()))
							matches.push_back("  "+xref+"]");//Two spaces are sacrificial for formatting
					}
					for(auto& m : getDirectoryListing(campaigns_dir))
					{
						std::string chk_str_xref = right(chk_str,2);
						if(chk_str.length() == 2 ||
						   (!stringcasecmp(chk_str_xref,left(m,chk_str.length()-2)) &&
						   m.length() > chk_str_xref.length()))
							matches.push_back("  "+m+"/");//Two spaces are sacrificial for formatting
					}
				}
				else if(chk_str[1] == '[' &&
					chk_str.find(']') == std::string::npos)//Complete xref (different campaign)
				{
					//Get campaign name
					std::string campaign = right(chk_str,chk_str.find('[')+1);
					campaign = left(campaign,campaign.find('/'));

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
						if(right(c,c.length()-5) != ".char")//Don't include backup files or non-character files
							continue;

						std::string xref = left(c,c.find(".char"));
						std::string chk_str_xref = right(chk_str,chk_str.find('/')+1);
						if(!stringcasecmp(chk_str_xref,left(xref,chk_str.length()-chk_str.find('/')-1)) &&
						   xref.length() > chk_str_xref.length())
						{
							matches.push_back(addSpaces(chk_str.length()-(chk_str.length()-chk_str.find('/'))+1)+
									  xref+"]");//Padding is sacrificial for formatting
						}
					}
				}
				else if(chk_str[1] == '[' &&
					chk_str.find(']') != std::string::npos)//Load xref
				{
					if(chk_str.find('/') != std::string::npos &&
					   chk_str.find('/') > chk_str.find(']'))//Same campaign
					{
						std::string xref_path = campaigns_dir+
									get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
									"characters/"+
									right(left(chk_str,chk_str.find(']')),chk_str.find('[')+1)+
									".char";

						if(std::filesystem::exists(xref_path) && !std::filesystem::is_directory(xref_path))
							tab_comp_scope.load(xref_path);

					}
					else if(chk_str.find('/') != std::string::npos &&
					   chk_str.find('/') < chk_str.find(']'))//Different campaign
					{
						std::string xref_path = campaigns_dir+
									right(left(chk_str,chk_str.find('/')+1),chk_str.find('[')+1)+
									"characters/"+
									right(left(chk_str,chk_str.find(']')),chk_str.find('/')+1)+
									".char";

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
				   chk_str.find(']') == std::string::npos)//Complete xref
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
					chk_str.find(']') != std::string::npos)//Load xref
				{
					std::string xref_path = campaigns_dir+
								right(left(chk_str,chk_str.find(']')),chk_str.find('[')+1)+
								".variables";

					if(std::filesystem::exists(xref_path) && !std::filesystem::is_directory(xref_path))
						tab_comp_scope.load(xref_path);
				}
			}
			else if(chk_str[0] == SHELL_SIGIL)//Shell variables
			{
				tab_comp_scope = Shell();
			}

			int slash = chk_str.find('/',chk_str.find(']')+1);
			int rsqbrkt = chk_str.find(']');
			int period = chk_str.rfind('.');
			char type_sigil = chk_str[chk_str.find('/',chk_str.find(']')+1)-1];
			std::string key = right(chk_str,chk_str.find('/',chk_str.find(']')+1)+1);
			if(slash > rsqbrkt && period < slash)//Keys
			{
				if(isTypeSigil(type_sigil))
				{
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,type_sigil);
				}
				else if(isScopeSigil(type_sigil) || type_sigil == ']')
				{
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,CURRENCY_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,CURRENCYSYSTEM_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,DICE_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,VAR_SIGIL);
					addKeysToMatches(&matches,tab_comp_scope,chk_str,key,WALLET_SIGIL);
				}
			}
			else if(slash > rsqbrkt)//Properties
			{
				std::string property = right(chk_str,chk_str.find('.')+1);
				if(isTypeSigil(type_sigil))
				{
					addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,property,type_sigil);
				}
				else if(isScopeSigil(type_sigil) || type_sigil == ']')
				{
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Var>())
					{
						if(!stringcasecmp(k+".",key))
							addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,property,VAR_SIGIL);
					}
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Dice>())
					{
						if(!stringcasecmp(k+".",key))
							addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,property,DICE_SIGIL);
					}
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Wallet>())
					{
						if(!stringcasecmp(k+".",key))
							addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,property,WALLET_SIGIL);
					}
					for(const auto& [k,v] : tab_comp_scope.getDatamap<Currency>())
					{
						if(!stringcasecmp(k+".",key))
							addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,property,CURRENCY_SIGIL);
					}
					for(const auto& [k,v] : tab_comp_scope.getDatamap<CurrencySystem>())
					{
						if(!stringcasecmp(k+".",key))
							addPropertiesToMatches(&matches,tab_comp_scope,chk_str,key,property,CURRENCYSYSTEM_SIGIL);
					}
				}
			}

			if(!matches.size()) continue;

			//Sort and choose match
			sort<std::string>(&matches);
			match = matches[(tab_ctr-1)%matches.size()];

			//Erase any previous match
			if(last_match != "")
			{
				for(int i=0; i<last_match.length()-chk_str.length(); i++)
					input.erase(input.begin()+cur_pos);
			}

			//Insert match into input
			for(int i=0; i<match.length()-chk_str.length(); i++)
				input.insert(input.begin()+cur_pos+i,match[i+chk_str.length()]);

			//Reprint input
			if(last_match != "" && containsNonSpaceChar(match))
			{
				fprintf(stdout,CURSOR_LEFT_N,last_match.length()-chk_str.length());
				fprintf(stdout,CLEAR_TO_LINE_END);
			}

			for(int i=cur_pos; i<input.size(); i++)
				fprintf(stdout,"%c",input[i]);

			last_match = match;
		}
		else if(k == ESC_SEQ)//Escape sequences
		{
			switch(esc_char)
			{
				case 'C':	//Right
					if(cur_pos < input.size())
					{
						fprintf(stdout,CURSOR_RIGHT);
						cur_pos++;
					}
					break;
				case 'D':	//Left
					if(cur_pos > 0)
					{
						fprintf(stdout,CURSOR_LEFT);
						cur_pos--;
					}
					break;
				case 'H':	//Home
					if(cur_pos > 0)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					break;
				case '7':	//Home
					if(getchar() == '~' && cur_pos > 0)
					{
						fprintf(stdout,CURSOR_LEFT_N,cur_pos);
						cur_pos = 0;
					}
					break;
				case 'F':	//End
					if(cur_pos < input.size())
					{
						fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos);
						cur_pos = input.size();
					}
					break;
				case '8':	//End
					if(getchar() == '~' && cur_pos < input.size())
					{
						fprintf(stdout,CURSOR_RIGHT_N,input.size()-cur_pos);
						cur_pos = input.size();
					}
					break;
				case '2':	//Insert
					if(getchar() == '~')
						insert_mode = !insert_mode;
					break;
				case '3':	//Delete
					if(getchar() == '~' && cur_pos < input.size())
					{
						fprintf(stdout,CLEAR_LINE);
						input.erase(input.begin()+cur_pos);

						for(int i=cur_pos; i<input.size(); i++)
							fprintf(stdout,"%c",input[i]);
						if(cur_pos < input.size())
							fprintf(stdout,CURSOR_LEFT_N,input.size()-cur_pos);
					}
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
	bool backup = false;

	prompt:
	c.load();

	try
	{
		fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%s/%s%s %s(%s)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,c.getName().c_str(),TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,c.getProperty<Var>("HitPoints/Current","Value").c_str(),c.getProperty<Var>("HitPoints/Maximum","Value").c_str(),TEXT_NOBOLD,TEXT_ITALIC,c.getProperty<Var>("HitPoints/Temporary","Value").c_str(),TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
		fprintf(stdout,"%s└─%s$%s ",TEXT_WHITE,TEXT_CYAN,TEXT_NORMAL);

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
	for(int i=0; i<MAX_BUFFER_SIZE; i++)
		buffer[i] = '\0';

	std::string in = input_handler();
	if(in.length() > MAX_BUFFER_SIZE)
	{
		fprintf(stdout,"\n\n");
		output(Error,"Input too big (%llu characters)! Please enter <= %d characters.",in.length(),MAX_BUFFER_SIZE);
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
		else if(buffer[0] != COMMENT)
		{
			run_rpgsh_prog(buffer,false);
		}
	}
	return 0;
}
int main()
{
	//Generate cache of valid rpgsh programs to speed up "help" program
	//It takes a noticable amount of time to search through all of /bin even on an i7-4700k
	fprintf(stdout,"Generating rpgsh program list...");
	std::vector<std::string> applications = getDirectoryListing("/bin");
	std::vector<std::string> rpgsh_apps;
	std::filesystem::remove(rpgsh_programs_cache_path);
	for(const auto& app : applications)
	{
		if(!app.find(prefix))
			rpgsh_apps.push_back(app);
	}

	//Basic alphabetical sort, important for tab completion
	sort<std::string>(&rpgsh_apps);

	//Write alphabetized list to file
	std::ofstream ofs(rpgsh_programs_cache_path);
	for(const auto& app : rpgsh_apps)
		ofs<<app<<"\n";
	ofs.close();

	fprintf(stdout,"\e[2K");//Delete "Generating..." line from start of main()

	//Generate default character if needed
	if(!c.confirmDatasource())
	{
		c.setDatasource(templates_dir + config.setting[DEFAULT_GAME].c_str());
		c.load();
		c.setDatasource(c.getCurrentCharacterFilePath());
		c.save();
	}

	run_rpgsh_prog((char*)"banner",false);
	run_rpgsh_prog((char*)"version",false);

	if(!stob(config.setting[HIDE_TIPS]))
	{
		fprintf(stdout,"%s%sTIP: %sType %s%shelp%s for a list of currently supported commands.\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
		fprintf(stdout,"%s%sTIP: %sFor more info about a given command, try using the %s%s-?%s or %s%s--help%s flags.\n\n",TEXT_BOLD,TEXT_CYAN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL,TEXT_BOLD,TEXT_GREEN,TEXT_NORMAL);
	}

	while(prompt() == 0);

	return 0;
}
