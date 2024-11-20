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

bool stob(std::string s)
{
	if(!stringcasecmp(s,"true"))
		return true;
	else if(!stringcasecmp(s,"false"))
		return false;
	else
		throw;

	return false;
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
		c == CURRENCYSYSTEM_SIGIL ||
		c == DICE_SIGIL ||
		c == VAR_SIGIL ||
		c == WALLET_SIGIL);
}

int stringcasecmp(std::string a, std::string b)
{
	return strcasecmp(a.c_str(),b.c_str());
}

int findInStrVect(std::vector<std::string> v, std::string str, unsigned int start)
{
	for(long unsigned int i=start; i<v.size(); i++)
		if(v[i] == str) return i;

	return -1;
}

std::string left(std::string str, int n)
{
	return str.substr(0,n);
}

std::string right(std::string str, int n)
{
	return str.substr(n,str.length()-n);
}

std::string addSpaces(int n)
{
	std::string ret = "";
	for(int i=0; i<n; i++)
		ret += " ";

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
	set_env_variable(CURRENT_CHAR_SHELL_VAR,c.getName());
	set_env_variable(CURRENT_CAMPAIGN_SHELL_VAR,"default/");
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
					      get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR) +
					      variable_file_name;

	if(std::filesystem::exists(campaign_variables_file.c_str())) return;

	output(Info,"Campaign variables file not found, creating file at \'%s\'.",campaign_variables_file.c_str());
	std::ofstream ofs(campaign_variables_file.c_str());
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

void run_rpgsh_prog(std::string arg_str, bool redirect_output)
{
	Character c = Character(false);
	Campaign m = Campaign();
	Shell s = Shell();

	std::vector<std::string> args;
	extern char** environ;
	pid_t pid;

	if(!redirect_output) padding();

	if(isScopeSigil(arg_str[0])) //Check if user is operating on a variable
		arg_str = "variables " + arg_str;

	bool runningVariables = (left(arg_str,9) == "variables");

	std::string path = "/bin/";
	std::regex arg_pattern("[^\\s]{1,}");
	std::sregex_iterator arg_str_it(arg_str.begin(), arg_str.end(), arg_pattern);
	std::sregex_iterator arg_str_end;

	//Push back program we are going to run
	args.push_back(path+prefix+arg_str_it->str());
	arg_str_it++;

	//Replaces all instances of variables with their respective value
	//Except for the first arg if it is a variable
	if(runningVariables)
	{
		args.push_back(arg_str_it->str());
		arg_str_it++;
	}

	while(arg_str_it != arg_str_end)
	{
		std::string arg = arg_str_it->str();

		if(isScopeSigil(arg[0])) arg = get_prog_output(arg)[0];

		//Don't try to run a program if the data type sigil was invalid
		if(arg == "")
		{
			if(!redirect_output) padding();
			return;
		}

		args.push_back(arg);
		arg_str_it++;
	}

	//Combine arg_str wrapped in quotes
	for(long unsigned int i=0; i<args.size(); i++)
	{
		long unsigned int quote_begin = args[i].find("\"");
		long unsigned int quote_end = args[i].find("\"",quote_begin+1);
		if(quote_begin != std::string::npos && quote_end != std::string::npos)//Quote-wrapped arg doesn't contain a space
		{
			args[i] = args[i].substr(quote_begin,(quote_end+1)-quote_begin);
		}
		else if(quote_begin != std::string::npos)//Quote-wrapped arg with space
		{
			if(i == args.size())
			{
				output(Error,"Missing terminating quotation mark.");
				exit(-1);
			}

			args[i] = args[i].substr(quote_begin,args[i].length()-quote_begin);

			for(long unsigned int j=i+1; j<args.size(); j++)
			{
				quote_end = args[j].find("\"");
				if(quote_end != std::string::npos)
				{
					args[i] += " " + args[j].substr(0,quote_end+1);
					args.erase(args.begin()+j);
					break;
				}
				else
				{
					args[i] += " " + args[j];
					args.erase(args.begin()+j);
					j--;
				}
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
	}
	else
	{
		std::string displayed_command = std::string(argv[0]).substr(prefix.length()+path.length(),
							    std::string(argv[0]).length()-prefix.length()-path.length());
		if(status == 2)//File not found
			output(Error,"Error code %d while attempting to run \"%s\": Not a valid rpgsh command.",status,displayed_command.c_str());
		else
			output(Error,"Error code %d while attempting to run \"%s\": %s",status,displayed_command.c_str(),strerror(status));
	}

	if(redirect_output && posix_spawn_file_actions_destroy(&fa))
		output(Error,"Error code %d during posix_spawn_file_actions_destroy(): %s",status,strerror(status));

	if(!redirect_output)
		padding();
}

std::vector<std::string> get_prog_output(std::string prog)
{
	std::vector<std::string> output;

	run_rpgsh_prog(prog,true);

	std::ifstream ifs(rpgsh_output_redirect_path);
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		output.push_back(data);
	}
	ifs.close();

	return output;
}

void check_print_app_description(char** _argv, std::string description)
{
	if(_argv[1] && !strcmp(_argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"%s\n",description.c_str());
		exit(0);
	}
}

std::string get_env_variable(std::string v)
{
	confirmEnvVariablesFile();

	std::ifstream ifs(rpgsh_env_variables_path.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(left(data,data.find(DS)) == v)
			return right(data,data.find(DS)+DS.length());
	}
	ifs.close();
	return "";
}
void set_env_variable(std::string v,std::string value)
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
		if(left(data,data.find(DS)) == v)
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
		if(findInStrVect(systems,c.System->Name,0) != -1)
		{
			systems.push_back(c.System->Name);
			total += w.getEquivalentValueInLowestDenomination(*c.System);
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

std::string getSet(VariableInfo vi)
{
	std::map<std::string,std::string> c_map;
	std::map<std::string,std::string> cs_map;
	std::map<std::string,std::string> d_map;
	std::map<std::string,std::string> v_map;
	std::map<std::string,std::string> w_map;

	// When printing entire containers, treat type sigil as a filter
	switch(vi.variable[1])
	{
		case CURRENCY_SIGIL:
			appendMap<Currency>(vi.scope,&c_map);
			break;
		case CURRENCYSYSTEM_SIGIL:
			appendMap<CurrencySystem>(vi.scope,&cs_map);
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
			appendMap<CurrencySystem>(vi.scope,&cs_map);
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
	appendOutput(cs_map,vi.key,&output);
	appendOutput(d_map,vi.key,&output);
	appendOutput(v_map,vi.key,&output);
	appendOutput(w_map,vi.key,&output);

	// Cut off the extraneous DS
	return left(output,output.length()-DS.length());
}

template<>//Forward definition for <Dice,Dice> due to it being called before being defined.
bool approxEquals<Dice,Dice>(Dice lhs, Dice rhs);
template<>
bool approxEquals<Var,int>(Var lhs, int rhs)
{
	return lhs == rhs;
}
template<>
bool approxEquals<Var,std::string>(Var lhs, std::string rhs)
{
	bool rhs_is_int = false;
	try{std::stoi(rhs); rhs_is_int = true;}
	catch(...){}

	if(lhs.isInt() && rhs_is_int)
		return int(lhs) == std::stoi(rhs);
	else if((lhs.isInt() && !rhs_is_int) || (!lhs.isInt() && rhs_is_int))
		return false;
	else
		return stringcasecmp(lhs.Value,rhs);
}
template<>
bool approxEquals<Var,Var>(Var lhs, Var rhs)
{
	return approxEquals<Var,std::string>(lhs,rhs.Value);
}
template<>
bool approxEquals<Var,Dice>(Var lhs, Dice rhs)
{
	try{return approxEquals<Dice,Dice>(Dice(lhs.Value),rhs);}//TODO: Dice constructor (and really all constructors) need to call "throw"
	catch(...){return false;}
}
template<>
bool approxEquals<Var,Currency>([[maybe_unused]]Var lhs, [[maybe_unused]]Currency rhs)
{
	return false;
}
template<>
bool approxEquals<Var,CurrencySystem>([[maybe_unused]]Var lhs, [[maybe_unused]]CurrencySystem rhs)
{
	return false;
}
template<>
bool approxEquals<Var,Wallet>([[maybe_unused]]Var lhs, [[maybe_unused]]Wallet rhs)
{
	return false;
}
template<>
bool approxEquals<Dice,int>([[maybe_unused]]Dice lhs, [[maybe_unused]]int rhs)
{
	return false;
}
template<>
bool approxEquals<Dice,std::string>(Dice lhs, std::string rhs)
{
	try{return approxEquals<Dice,Dice>(lhs,Dice(rhs));}
	catch(...){return false;}
}
template<>
bool approxEquals<Dice,Var>(Dice lhs, Var rhs)
{
	try{return approxEquals<Dice,Dice>(lhs,Dice(rhs.Value));}
	catch(...){return false;}
}
template<>
bool approxEquals<Dice,Dice>(Dice lhs, Dice rhs)
{
	if(lhs.List != "" && rhs.List != "")
		return lhs.List == rhs.List;
	else
		return lhs.Faces == rhs.Faces;
}
template<>
bool approxEquals<Dice,Currency>([[maybe_unused]]Dice lhs, [[maybe_unused]]Currency rhs)
{
	return false;
}
template<>
bool approxEquals<Dice,CurrencySystem>([[maybe_unused]]Dice lhs, [[maybe_unused]]CurrencySystem rhs)
{
	return false;
}
template<>
bool approxEquals<Dice,Wallet>([[maybe_unused]]Dice lhs, [[maybe_unused]]Wallet rhs)
{
	return false;
}
template<>
bool approxEquals<Currency,int>([[maybe_unused]]Currency lhs, [[maybe_unused]]int rhs)
{
	return false;
}
template<>
bool approxEquals<Currency,std::string>([[maybe_unused]]Currency lhs, [[maybe_unused]]std::string rhs)
{
	return false;
}
template<>
bool approxEquals<Currency,Var>([[maybe_unused]]Currency lhs, [[maybe_unused]]Var rhs)
{
	return false;
}
template<>
bool approxEquals<Currency,Dice>([[maybe_unused]]Currency lhs, [[maybe_unused]]Dice rhs)
{
	return false;
}
template<>
bool approxEquals<Currency,Currency>(Currency lhs, Currency rhs)
{
	return lhs.System == rhs.System;
}
template<>
bool approxEquals<Currency,CurrencySystem>([[maybe_unused]]Currency lhs, [[maybe_unused]]CurrencySystem rhs)
{
	return false;
}
template<>
bool approxEquals<Currency,Wallet>([[maybe_unused]]Currency lhs, [[maybe_unused]]Wallet rhs)
{
	return false;
}
template<>
bool approxEquals<CurrencySystem,int>([[maybe_unused]]CurrencySystem lhs, [[maybe_unused]]int rhs)
{
	return false;
}
template<>
bool approxEquals<CurrencySystem,std::string>([[maybe_unused]]CurrencySystem lhs, [[maybe_unused]]std::string rhs)
{
	return false;
}
template<>
bool approxEquals<CurrencySystem,Var>([[maybe_unused]]CurrencySystem lhs, [[maybe_unused]]Var rhs)
{
	return false;
}
template<>
bool approxEquals<CurrencySystem,Dice>([[maybe_unused]]CurrencySystem lhs, [[maybe_unused]]Dice rhs)
{
	return false;
}
template<>
bool approxEquals<CurrencySystem,Currency>([[maybe_unused]]CurrencySystem lhs, [[maybe_unused]]Currency rhs)
{
	return false;
}
template<>
bool approxEquals<CurrencySystem,CurrencySystem>(CurrencySystem lhs, CurrencySystem rhs)
{
	return lhs == rhs;
}
template<>
bool approxEquals<CurrencySystem,Wallet>([[maybe_unused]]CurrencySystem lhs, [[maybe_unused]]Wallet rhs)
{
	return false;
}
template<>
bool approxEquals<Wallet,int>([[maybe_unused]]Wallet lhs, [[maybe_unused]]int rhs)
{
	return false;
}
template<>
bool approxEquals<Wallet,std::string>([[maybe_unused]]Wallet lhs, [[maybe_unused]]std::string rhs)
{
	return false;
}
template<>
bool approxEquals<Wallet,Var>([[maybe_unused]]Wallet lhs, [[maybe_unused]]Var rhs)
{
	return false;
}
template<>
bool approxEquals<Wallet,Dice>([[maybe_unused]]Wallet lhs, [[maybe_unused]]Dice rhs)
{
	return false;
}
template<>
bool approxEquals<Wallet,Currency>(Wallet lhs, Currency rhs)
{
	for(const auto& [c,q] : lhs)
		if(c == rhs) return true;

	return false;
}
template<>
bool approxEquals<Wallet,CurrencySystem>(Wallet lhs, CurrencySystem rhs)
{
	for(const auto& [c,q] : lhs)
		if(*c.System == rhs) return true;

	return lhs == rhs;
}

template<>
bool approxEquals<Wallet,Wallet>(Wallet lhs, Wallet rhs)
{
	return getWalletValue(lhs) == getWalletValue(rhs);
}

template <typename T>
void sort(std::vector<std::string>* v);

template <typename T>
void save_obj_to_file(std::string path, datamap<Dice> obj, char obj_id);
void save_obj_to_file(std::string path, datamap<Var> obj, char obj_id);

template <typename T>
datamap<Dice> load_obj_from_file(std::string path, char var_id);
datamap<Var> load_obj_from_file(std::string path, char var_id);

template <typename T>
datamap<Currency> loadDatamapFromAllScopes(char type);
