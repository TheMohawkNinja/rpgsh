#include <cassert>
#include <cstdarg>
#include <fcntl.h>
#include <map>
#include <regex>
#include <spawn.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/scope.h"

void output(OutputLevel level, const char* format, ...)
{
	FILE* stream;
	std::string prefix = "";
	std::string color;
	va_list args;
	va_start(args, format);

	switch(level)
	{
		case Info:
			stream = stdout;
			prefix = "%s%s[%sINFO%s]%s    ";
			color = TEXT_CYAN;
			break;
		case Warning:
			stream = stderr;
			prefix = "%s%s[%sWARNING%s]%s ";
			color = TEXT_YELLOW;
			break;
		case Error:
			stream = stderr;
			prefix = "%s%s[%sERROR%s]%s   ";
			color = TEXT_RED;
			break;
	}
	fprintf(stream,prefix.c_str(),TEXT_BOLD,TEXT_WHITE,color.c_str(),TEXT_WHITE,color.c_str());
	vfprintf(stream,format,args);
	fprintf(stream,"%s\n",TEXT_NORMAL);
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
	if(!pos) return false;
	else return str[pos-1] == '\\';
}

int stringcasecmp(std::string a, std::string b)
{
	return strcasecmp(a.c_str(),b.c_str());
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
	if(str.front() != '\"' && (str.back() != '\"' || isEscaped(str,str.length()-1)))
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

void confirmEnvVariablesFile()
{
	if(std::filesystem::exists(rpgsh_env_variables_path.c_str())) return;

	output(Info,"Environment variables file not found, creating file at \"%s\".",rpgsh_env_variables_path.c_str());
	std::ofstream ofs(rpgsh_env_variables_path.c_str());
	ofs.close();

	//Set default values for built-in env variables
	Config config = Config();
	Character c = Character(templates_dir + config.setting[DEFAULT_GAME].c_str());
	setEnvVariable(CURRENT_CHAR_SHELL_VAR,c.getName());
	setEnvVariable(CURRENT_CAMPAIGN_SHELL_VAR,"default/");
}

void confirmShellVariablesFile()
{
	if(std::filesystem::exists(shell_variables_path.c_str())) return;

	output(Info,"Shell variables file not found, creating file at \"%s\".",shell_variables_path.c_str());
	std::ofstream ofs(shell_variables_path.c_str());
	ofs.close();
}

void confirmCampaignVariablesFile()
{
	std::string campaign_variables_file = campaigns_dir +
					      getEnvVariable(CURRENT_CAMPAIGN_SHELL_VAR) +
					      variable_file_name;

	if(std::filesystem::exists(campaign_variables_file.c_str())) return;

	output(Info,"Campaign variables file not found, creating file at \'%s\'.",campaign_variables_file.c_str());
	std::ofstream ofs(campaign_variables_file.c_str());
	ofs.close();
}

void confirmHistoryFile()
{
	if(std::filesystem::exists(history_path.c_str())) return;

	output(Info,"rpgsh history file not found, creating file at \"%s\".",history_path.c_str());
	std::ofstream ofs(history_path.c_str());
	ofs.close();
}

std::vector<std::string> getDirectoryListing(std::string path)
{
	if(!std::filesystem::exists(path))
	{
		output(Error,"Directory \"%s\" does not exist.",path.c_str());
		exit(-1);
	}
	else if(!std::filesystem::is_directory(path))
	{
		output(Error,"\"%s\" is not a directory.",path.c_str());
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
	output(Error,"Invalid xref \"%s\".",xref.c_str());
	exit(-1);
}
void loadXRef(std::string* arg, Scope* p_scope)
{
	// Ending square bracket not found
	if(findu(*arg,']') == std::string::npos)
	{
		output(Error,"No terminating \']\' found for xref.");
		exit(-1);
	}

	// Get string between the square brackets
	std::string xref = arg->substr(findu(*arg,'[')+1,findu(*arg,']')-(findu(*arg,'[')+1));

	// Actually load the xref into the scope
	std::vector<std::string> campaigns;
	std::string xref_dir = campaigns_dir+
			       getEnvVariable(CURRENT_CAMPAIGN_SHELL_VAR)+
			       "characters/";
	std::string xref_char = xref;
	std::string campaign = "";
	switch((*arg)[0])
	{
		case CHARACTER_SIGIL:
			if(findu(xref,'/') != std::string::npos)// Attempting to get a character from another campaign
			{
				campaign = left(xref,findu(xref,'/'));
				xref_char = right(xref,findu(xref,'/')+1);
				xref_dir = campaigns_dir+
					   getLikeFileName(campaign,campaigns_dir,true,xref)+
					   "/characters/";
			}

			p_scope->load(xref_dir+
				      getLikeFileName(xref_char+".char",xref_dir,false,xref));
			break;
		case CAMPAIGN_SIGIL:
			p_scope->load(campaigns_dir+
				      getLikeFileName(xref,campaigns_dir,true,xref)+
				      "/"+
				      variable_file_name);
			break;
		case SHELL_SIGIL:
			output(Error,"Cannot use xref with shell scope.");
			exit(-1);
	}

	// Remove external reference string so we can continue to use the current arg under the new context
	*arg = (*arg)[0]+left(*arg,findu(*arg,'[')-1)+right(*arg,findu(*arg,']')+1);
}
VariableInfo parseVariable(std::string v)// Derive information about variable from string
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
			output(Error,"Unknown scope sigil \'%c\'.",v[0]);
			exit(-1);
	}

	// Check for external references
	if(v[1] == '[') loadXRef(&v,&vi.scope);

	// Check type sigil
	if(!isTypeSigil(v[1]) && v[1] != '/')
	{
		output(Error,"Unknown type sigil \'%c\'.",v[1]);
		exit(-1);
	}

	vi.type = v[1];
	vi.variable = v;
	vi.key = right(v,findu(v,'/')+1);
	if(findu(v,'.') > rfindu(v,'/') && rfindu(v,'.') < UINT_MAX)
	{
		vi.key = left(vi.key,findu(vi.key,'.'));
		vi.property = right(v,findu(v,'.')+1);
	}

	if(isTypeSigil(v[1]))
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
		output(Error,"Invalid value \"%s\" for \"%s\".",config.setting[PADDING].c_str(),PADDING);
		exit(-1);
	}
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

	//Check aliases
	std::string first_arg;
	if(findu(arg_str," ") != std::string::npos) first_arg = left(arg_str,findu(arg_str," "));
	else					    first_arg = arg_str;
	for(const auto& [k,v] : getSet(cfg.setting[ALIASES]))
	{
		if(k == first_arg)
		{
			arg_str = v + " " + right(arg_str,first_arg.length());
			break;
		}
	}

	std::regex variable_pattern(variable_pattern_str);
	std::sregex_iterator v_str_it(first_arg.begin(),first_arg.end(),variable_pattern);
	std::sregex_iterator v_str_end;
	if(v_str_it != v_str_end) arg_str = "eval " + arg_str;

	//See if we need to preserve the argv[1] if it is a variable
	if(findu(arg_str," ") != std::string::npos) first_arg = left(arg_str,findu(arg_str," "));
	else					    first_arg = arg_str;

	bool preserveSecondArg;
	if(findu(arg_str,std::string(FLAG_MODIFYVARIABLES)) == std::string::npos)
	{
		GetAppOutputInfo info = getAppOutput(first_arg + " " + std::string(FLAG_MODIFYVARIABLES));

		if(!info.status) preserveSecondArg = stob(info.output[0]);
		else return info.status;
	}
	else
	{
		preserveSecondArg = false;
	}

	//Push back program we are going to run
	std::string path = std::string(RPGSH_INSTALL_DIR);
	args.push_back(path+prefix+left(arg_str,findu(arg_str," ")));

	//Replaces all instances of variables with their respective value
	v_str_it = std::sregex_iterator(arg_str.begin(), arg_str.end(), variable_pattern);

	long unsigned int first_space = findu(arg_str," ");
	long unsigned int second_space = findu(arg_str," ",findu(arg_str," ")+1);
	std::string second_arg;
	if(second_space != std::string::npos)
		second_arg = right(left(arg_str,second_space),first_space+1);
	else
		second_arg = right(arg_str,first_space+1);

	if(preserveSecondArg && v_str_it != v_str_end && v_str_it->str() == second_arg) v_str_it++;

	while(v_str_it != v_str_end)
	{
		std::string v_str = v_str_it->str();
		if(runApp(v_str,true))
		{
			output(Error,"%s is not a valid variable string.",v_str.c_str());
			return -1;
		}
		arg_str = std::regex_replace(arg_str,std::regex(v_str),getAppOutput(v_str).output[0]);
		v_str_it++;
	}

	//Get args for program
	std::regex arg_pattern(arg_pattern_str);
	std::sregex_iterator arg_str_it(arg_str.begin(), arg_str.end(), arg_pattern);
	std::sregex_iterator arg_str_end;
	arg_str_it++;

	while(arg_str_it != arg_str_end)
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
	argv[args.size()] = NULL;

	int status = 0;
	int exit_code = 0;
	posix_spawn_file_actions_t fa;
	if(redirect_output)
	{
		//https://unix.stackexchange.com/questions/252901/get-output-of-posix-spawn

		if(posix_spawn_file_actions_init(&fa))
			output(Error,"Error code %d during posix_spawn_file_actions_init(): %s",status,strerror(status));

		if(posix_spawn_file_actions_addopen(&fa, 1, rpgsh_output_redirect_path, O_CREAT | O_TRUNC | O_WRONLY, 0644))
			output(Error,"Error code %d during posix_spawn_file_actions_addopen(): %s",status,strerror(status));

		if(posix_spawn_file_actions_adddup2(&fa, 1, 2))
			output(Error,"Error code %d during posix_spawn_file_actions_adddup2(): %s",status,strerror(status));

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
			output(Error,"Error code %d while attempting to run \"%s\": Not a valid rpgsh command.",status,displayed_command.c_str());
		else
			output(Error,"Error code %d while attempting to run \"%s\": %s",status,displayed_command.c_str(),strerror(status));

		return status;
	}

	if(redirect_output && posix_spawn_file_actions_destroy(&fa))
	{
		output(Error,"Error code %d during posix_spawn_file_actions_destroy(): %s",status,strerror(status));

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

	for(const auto& [c,q] : w.Money)
	{
		if(findInVect<std::string>(systems,c.System) != -1)
		{
			systems.push_back(c.System);
			total += w.getEquivalentValueInLowestDenomination(c.System);
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
			output(Error,"Unknown type specifier \'%c\' in \"%s\"",vi.variable[1],vi.variable.c_str());
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
	std::string character = getEnvVariable(CURRENT_CHAR_SHELL_VAR);
	std::string campaign = getEnvVariable(CURRENT_CAMPAIGN_SHELL_VAR);
	std::string current_campaign_dir = campaigns_dir+campaign;
	std::string current_campaign_path = current_campaign_dir+variable_file_name;
	std::string current_character_path = current_campaign_dir+"characters/"+character+".char";

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
