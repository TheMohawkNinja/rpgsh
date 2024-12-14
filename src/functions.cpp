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

bool looksLikeSet(std::string s)
{
	std::regex set_pattern(set_pattern_str);
	std::sregex_iterator match_it(s.begin(), s.end(), set_pattern);
	std::sregex_iterator match_end;

	return match_it->str() == s;
}

bool isEscaped(std::string str, int pos)
{
	if(!pos) return false;
	else return str[pos-1] == '\\';
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
	if(arg->find(']') == std::string::npos)
	{
		output(Error,"No terminating \']\' found for xref.");
		exit(-1);
	}

	// Get string between the square brackets
	std::string xref = arg->substr(arg->find('[')+1,arg->find(']')-(arg->find('[')+1));

	// Actually load the xref into the scope
	std::vector<std::string> campaigns;
	std::string xref_dir = campaigns_dir+
			       get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR)+
			       "characters/";
	std::string xref_char = xref;
	std::string campaign = "";
	switch((*arg)[0])
	{
		case CHARACTER_SIGIL:
			if(xref.find('/') != std::string::npos)// Attempting to get a character from another campaign
			{
				campaign = left(xref,xref.find('/'));
				xref_char = right(xref,xref.find('/')+1);
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
	(*arg) = (*arg)[0]+left((*arg),arg->find('[')-1)+right((*arg),arg->find(']')+1);
}
VariableInfo parseVariable(std::string v)// Derive information about variable from string
{
	VariableInfo vi;

	// Check scope sigil
	switch(v[0])
	{
		case CHARACTER_SIGIL:
			vi.scope = Character(false);
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
	vi.key = right(v,v.find('/')+1);
	if((int)v.find('.') > (int)v.rfind('/'))
	{
		vi.key = left(vi.key,vi.key.find('.'));
		vi.property = right(v,v.find('.')+1);
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
		else if(vi.scope.keyExists<CurrencySystem>(vi.key))
			vi.evalType = CURRENCYSYSTEM_SIGIL;
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

int run_rpgsh_prog(std::string arg_str, bool redirect_output)
{
	Character c = Character(false);
	Campaign m = Campaign();
	Shell s = Shell();

	std::vector<std::string> args;
	extern char** environ;
	pid_t pid;

	if(!redirect_output) padding();

	if(isScopeSigil(arg_str[0])) //Check if user is operating on a variable
		arg_str = "eval " + arg_str;
	bool runningVariables = (left(arg_str,4) == "eval");

	//Push back program we are going to run
	//This does mean no spaces for program names, but meh
	std::string path = std::string(RPGSH_INSTALL_DIR);
	args.push_back(path+prefix+left(arg_str,arg_str.find(" ")));

	//Replaces all instances of variables with their respective value
	//Except for the first arg if it is a variable
	std::regex variable_pattern(variable_pattern_str);
	std::sregex_iterator v_str_it(arg_str.begin(), arg_str.end(), variable_pattern);
	std::sregex_iterator v_str_end;

	if(runningVariables && v_str_it != v_str_end) v_str_it++;

	while(v_str_it != v_str_end)
	{
		std::string v_str = v_str_it->str();
		arg_str = std::regex_replace(arg_str,std::regex(v_str),get_prog_output(v_str)[0]);
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
		args.push_back(arg);
		arg_str_it++;
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
				quote_end = args[i].find('\"',c+1);
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

		if(!redirect_output) padding();
		return status;
	}

	if(redirect_output && posix_spawn_file_actions_destroy(&fa))
	{
		output(Error,"Error code %d during posix_spawn_file_actions_destroy(): %s",status,strerror(status));

		if(!redirect_output) padding();
		return status;
	}

	if(!redirect_output) padding();

	return 0;
}

std::vector<std::string> get_prog_output(std::string prog)
{
	std::vector<std::string> output;

	(void)run_rpgsh_prog(prog,true);

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

CurrencySystem findMatchingCurrencySystem(std::string str)
{
	for(const auto& [k,v] : getDatamapFromAllScopes<CurrencySystem>(CURRENCYSYSTEM_SIGIL))
		if(!stringcasecmp(k,str)) return v;

	return CurrencySystem();
}

unsigned int getWalletValue(Wallet w)
{
	unsigned int total = 0;
	std::vector<std::string> systems;

	for(const auto& [c,q] : w.Money)
	{
		if(findInStrVect(systems,c.System,0) != -1)
		{
			systems.push_back(c.System);
			total += w.getEquivalentValueInLowestDenomination(findMatchingCurrencySystem(c.System));
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
		if(c.System == rhs.Name) return true;

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
datamap<Currency> load_obj_from_file(std::string path, char var_id);

template <typename T>
datamap<Currency> getDatamapFromAllScopes(char type);
