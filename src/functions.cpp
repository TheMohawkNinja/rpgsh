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
	if(!std::filesystem::exists(rpgsh_env_variables_path.c_str()))
	{
		output(Info,"Environment variables file not found, creating file at \"%s\".",rpgsh_env_variables_path.c_str());
		std::ofstream ofs(rpgsh_env_variables_path.c_str());
		ofs.close();

		//Set default values for built-in env variables
		Config config = Config();
		Character c = Character(templates_dir + config.setting[DEFAULT_GAME].c_str());
		set_env_variable(CURRENT_CHAR_SHELL_VAR,c.getName());
		set_env_variable(CURRENT_CAMPAIGN_SHELL_VAR,"default/");
	}
}

void confirmShellVariablesFile()
{
	if(!std::filesystem::exists(shell_variables_path.c_str()))
	{
		output(Info,"Shell variables file not found, creating file at \"%s\".",shell_variables_path.c_str());
		std::ofstream ofs(shell_variables_path.c_str());
		ofs.close();
	}
}

void confirmCampaignVariablesFile()
{
	std::string campaign_variables_file = campaigns_dir +
					      get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR) +
					      variable_file_name;
	if(!std::filesystem::exists(campaign_variables_file.c_str()))
	{
		output(Info,"Campaign variables file not found, creating file at \'%s\'.",campaign_variables_file.c_str());
		std::ofstream ofs(campaign_variables_file.c_str());
		ofs.close();
	}
}

std::vector<std::string> getDirectoryListing(std::string path)
{
	if(!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		output(Error,"Invalid directory \"%s\".",path.c_str());
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
		{
			fprintf(stdout,"\n");
		}
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

	if(!redirect_output)
		padding();

	if(arg_str[0] == CHARACTER_SIGIL ||
	arg_str[0] == CAMPAIGN_SIGIL ||
	arg_str[0] == SHELL_SIGIL) //Check if user is operating on a variable
	{
		arg_str = "variables " + arg_str;
	}

	std::string path = "/bin/";
	std::regex arg_pattern("[\\@\\#\\$\\[\\]\\/a-zA-Z0-9\\=\\+\\-\\*\\.\\_\\\"\\(\\)]{1,}");

	std::sregex_iterator arg_str_it(arg_str.begin(), arg_str.end(), arg_pattern);
	std::sregex_iterator arg_str_end;

	//Replaces all instances of variables with their respective value
	//Except for the first arg if it is a variable
	args.push_back(path+prefix+arg_str_it->str());
	arg_str_it++;

	if(arg_str.substr(0,9) == "variables")
	{
		args.push_back(arg_str_it->str());
		arg_str_it++;
	}

	while(arg_str_it != arg_str_end)
	{
		std::string arg = arg_str_it->str();

		if(arg[0] == CHARACTER_SIGIL ||
		   arg[0] == CAMPAIGN_SIGIL  ||
		   arg[0] == SHELL_SIGIL)
			arg = get_prog_output(arg)[0];

		//Don't try to run a program if the data type sigil was invalid
		if(arg == "")
		{
			if(!redirect_output)
				padding();

			return;
		}

		args.push_back(arg);
		arg_str_it++;
	}

	//Combine arg_str wrapped in quotes
	//TODO There really has to be a more elegant way to do this
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
			if(waitpid(pid, &status, 0) == -1)
				exit(1);
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
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

	if(!ReplacedValue)
	{
		ofs<<v + DS + value + "\n";
	}
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
	bool lhs_is_int = false;
	bool rhs_is_int = false;
	try{std::stoi(lhs.Value); lhs_is_int = true;}
	catch(...){}
	try{std::stoi(rhs); rhs_is_int = true;}
	catch(...){}

	if(lhs_is_int && rhs_is_int)
		return std::stoi(lhs.Value) == std::stoi(rhs);
	else if((lhs_is_int && !rhs_is_int) || (!lhs_is_int && rhs_is_int))
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
	{
		if(c == rhs)
			return true;
	}
	return false;
}
template<>
bool approxEquals<Wallet,CurrencySystem>(Wallet lhs, CurrencySystem rhs)
{
	for(const auto& [c,q] : lhs)
	{
		if(*c.System == rhs)
			return true;
	}
	return lhs == rhs;
}

template<>
bool approxEquals<Wallet,Wallet>(Wallet lhs, Wallet rhs)
{
	return getWalletValue(lhs) == getWalletValue(rhs);
}

template <>
std::string toString<int>(int t)
{
	return std::to_string(t);
}
template <>
std::string toString<std::string>(std::string t)
{
	return t;
}
template <>
std::string toString<Currency>(Currency t)
{
	return std::string(t);
}
template <>
std::string toString<CurrencySystem>(CurrencySystem t)
{
	return std::string(t);
}
template <>
std::string toString<Dice>(Dice t)
{
	return std::string(t);
}
template <>
std::string toString<Var>(Var t)
{
	return std::string(t);
}
template <>
std::string toString<Wallet>(Wallet t)
{
	return std::string(t);
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
