#include <fcntl.h>
#include <map>
#include <regex>
#include <spawn.h>
#include <string.h>
#include <sys/wait.h>
#include "../headers/colors.h"
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/string.h"

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

void confirmEnvVariablesFile()
{
	if(std::filesystem::exists(rpgsh_env_variables_path.c_str())) return;

	output(Info,"Environment variables file not found, creating file at \"%s\".",rpgsh_env_variables_path.c_str());
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

	output(Info,"Shell variables file not found, creating file at \"%s\".",shell_variables_path.c_str());
	std::ofstream ofs(shell_variables_path.c_str());
	ofs.close();
}

void confirmCampaignVariablesFile()
{
	std::string campaign_variables_file = campaigns_dir +
					      getEnvVariable(ENV_CURRENT_CAMPAIGN) +
					      variables_file_name;

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
	//This is "Dice implicit w/ modifier OR Dice implicit w/o modifier OR integer OR variable"
	std::regex variable_pattern("[0-9]{1,}d[0-9]{1,}[+,-]?[0-9]{1,}?|[0-9]{1,}d[0-9]{1,}|[0-9]{1,}|"+variable_pattern_str);
	std::sregex_iterator v_str_it(first_arg.begin(),first_arg.end(),variable_pattern);
	std::sregex_iterator v_str_end;
	if(v_str_it != v_str_end && v_str_it->str() == first_arg) arg_str = "eval " + arg_str;

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

	//Replaces all instances of variables with their respective value
	variable_pattern = std::regex(variable_pattern_str);
	v_str_it = std::sregex_iterator(arg_str.begin(), arg_str.end(), variable_pattern);
	std::regex arg_pattern(arg_pattern_str);
	std::sregex_iterator arg_str_it(arg_str.begin(), arg_str.end(), arg_pattern);
	std::sregex_iterator arg_str_end;

	if(arg_str_it != arg_str_end) arg_str_it++;

	if(preserveSecondArg &&
	   v_str_it != v_str_end &&
	   arg_str_it != arg_str_end &&
	   v_str_it->str() == arg_str_it->str()) v_str_it++;

	// For some reason, attempting to just use v_str_it in the std::regex_replace line to replace variables with their values
	// results in the iterator getting in some way lost causing it to not replace all the variables.
	// Seems to be limited to cases when the value length > variable length
	// Either way, using a std::vector<std::string> is an effective work around.
	std::vector<std::string> matches;
	while(v_str_it != v_str_end)
	{
		matches.push_back(v_str_it->str());
		v_str_it++;
	}
	for(const auto& match : matches)
	{
		if(runApp(match,true))
		{
			output(Error,"%s is not a valid variable string.",match.c_str());
			return -1;
		}
		std::string v_str_it_pattern = match;
		std::vector<std::string> patterns = {"\\[","\\]","\\(","\\)","\\{","\\}"};
		for(const auto& p : patterns)
			v_str_it_pattern = std::regex_replace(v_str_it_pattern,std::regex(p),p);
		arg_str = std::regex_replace(arg_str,std::regex(v_str_it_pattern),getAppOutput(match).output[0]);
	}

	//Get args for program
	arg_str_it = std::sregex_iterator(arg_str.begin(), arg_str.end(), arg_pattern);
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
