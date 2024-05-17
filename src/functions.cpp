#include <cstdarg>
#include <fcntl.h>
#include <map>
#include <regex>
#include <spawn.h>
#include <string.h>
#include <sys/wait.h>
#include "../headers/config.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/scope.h"

bool stob(std::string s)
{
	if(!strcasecmp(s.c_str(),"true"))
	{
		return true;
	}
	else if(!strcasecmp(s.c_str(),"false"))
	{
		return false;
	}
	else
	{
		throw std::invalid_argument("Parameter for stob() was not \'true\' or \'false\'.");
		return false;
	}
}

void confirmEnvVariablesFile()
{
	if(!std::filesystem::exists(rpgsh_env_variables_file.c_str()))
	{
		output(Info,"Environment variables file not found, creating file at \"%s\".",rpgsh_env_variables_file.c_str());
		std::ofstream ofs(rpgsh_env_variables_file.c_str());
		ofs.close();

		//Set default values for built-in env variables
		Character c = Character(false);
		Config config = Config();
		c.setDatasource(templates_dir + config.setting[DEFAULT_GAME].c_str());
		c.load();
		set_env_variable(CURRENT_CHAR_SHELL_VAR,c.getName());
		set_env_variable(CURRENT_CAMPAIGN_SHELL_VAR,"default/");
	}
}

void confirmShellVarsFile()
{
	if(!std::filesystem::exists(shell_vars_file.c_str()))
	{
		output(Info,"Shell variables file not found, creating file at \"%s\".",shell_vars_file.c_str());
		std::ofstream ofs(shell_vars_file.c_str());
		ofs.close();
	}
}

void confirmCampaignVarsFile()
{
	std::string campaign_vars_file = campaigns_dir +
					get_env_variable(CURRENT_CAMPAIGN_SHELL_VAR) +
					".vars";
	if(!std::filesystem::exists(campaign_vars_file.c_str()))
	{
		output(Info,"Campaign variables file not found, creating file at \'%s\'.",campaign_vars_file.c_str());
		std::ofstream ofs(campaign_vars_file.c_str());
		ofs.close();
	}
}

std::vector<std::string> getDirectoryListing(std::string path)
{
	std::vector<std::string> entries;
	for(const auto& entry : std::filesystem::directory_iterator(path))
		entries.push_back(entry.path().filename().string());

	return entries;
}

void printBadOpAndThrow(std::string bad_op)
{
	output(Error,("Invalid operation: "+bad_op).c_str());
	throw;
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
		output(Error,"Invalid value \'%s\' for \'%s\'.",config.setting[PADDING].c_str(),PADDING);
		exit(-1);
	}
}

std::string replaceVariableWithValue(std::string arg, Scope scope, std::sregex_iterator i)
{
	arg = arg.substr(arg.find("/"),arg.length()-arg.find("/"));
	return std::regex_replace(arg,std::regex(arg),scope.getStr<Var>(arg));
}
void run_rpgsh_prog(std::string args, bool redirect_output)
{
	Character c = Character(false);
	Campaign m = Campaign();
	Shell s = Shell();

	c.load();
	m.load();
	s.load();

	std::vector<std::string> vars;
	extern char** environ;
	pid_t pid;

	if(args[0] == CHARACTER_SIGIL ||
	args[0] == CAMPAIGN_SIGIL ||
	args[0] == SHELL_SIGIL) //Check if user is operating on a variable
	{
		args = "variables " + args;
	}

	std::string path = "/bin/";
	std::regex arg_pattern("[\\@\\#\\$\\/a-zA-Z0-9\\=\\+\\-\\*\\.\\_\\\"]{1,}");

	std::sregex_iterator args_it(args.begin(), args.end(), arg_pattern);
	std::sregex_iterator args_end;

	//Replaces all instances of variables with their respective value
	//Except for the first arg if it is a variable
	vars.push_back(path+prefix+args_it->str());
	args_it++;

	if(args.substr(0,9) == "variables")
	{
		vars.push_back(args_it->str());
		args_it++;
	}

	while(args_it != args_end)
	{
		std::string arg = args_it->str();

		switch(args_it->str()[0])
		{
			case CHARACTER_SIGIL:
			{
				arg = replaceVariableWithValue(arg,c,args_it);
				vars.push_back(arg);
				args_it++;
				continue;
			}
			case CAMPAIGN_SIGIL:
			{
				arg = replaceVariableWithValue(arg,m,args_it);
				vars.push_back(arg);
				args_it++;
				continue;
			}
			case SHELL_SIGIL:
			{
				arg = replaceVariableWithValue(arg,s,args_it);
				vars.push_back(arg);
				args_it++;
				continue;
			}
		}
		vars.push_back(arg);
		args_it++;
	}

	//Combine args wrapped in quotes
	for(int i=0; i<vars.size(); i++)
	{
		if(vars[i].find("\"") != std::string::npos &&
		   vars[i].find("\"",vars[i].find("\"")+1) != std::string::npos)//Quote-wrapped arg doesn't contain a space
		{
			vars[i] = vars[i].substr(vars[i].find("\""),
						(vars[i].find("\"",vars[i].find("\"")+1)+1)-vars[i].find("\""));
		}
		else if(vars[i].find("\"") != std::string::npos)//Quote-wrapped arg with space
		{
			if(i == vars.size())
			{
				output(Error,"Missing terminating quotation mark.");
				exit(-1);
			}

			vars[i] = vars[i].substr(vars[i].find("\""),
						 vars[i].length()-vars[i].find("\""));

			for(int j=i+1; j<vars.size(); j++)
			{
				if(vars[j].find("\"") != std::string::npos)
				{
					vars[i] += " " + vars[j].substr(0,vars[j].find("\"")+1);
					vars.erase(vars.begin()+j);
					break;
				}
				else
				{
					vars[i] += " " + vars[j];
					vars.erase(vars.begin()+j);
					j--;
				}
			}
		}
	}

	//Convert vector to char*[]
	char* argv[vars.size()+1];
	for(int i=0; i<vars.size(); i++)
		argv[i] = (char*)vars[i].c_str();

	//Add a NULL because posix_spawn() needs that for some reason
	argv[vars.size()] = NULL;

	if(!redirect_output)
		padding();

	int status = 0;
	posix_spawn_file_actions_t fa;
	if(redirect_output)
	{
		//https://unix.stackexchange.com/questions/252901/get-output-of-posix-spawn

		if(posix_spawn_file_actions_init(&fa))
			output(Error,"Error code %d during posix_spawn_file_actions_init(): %s",status,strerror(status));

		if(posix_spawn_file_actions_addopen(&fa, 1, rpgsh_output_redirect_file.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644))
			output(Error,"Error code %d during posix_spawn_file_actions_addopen(): %s",status,strerror(status));

		if(posix_spawn_file_actions_adddup2(&fa, 1, 2))
			output(Error,"Error code %d during posix_spawn_file_actions_adddup2(): %s",status,strerror(status));

		status = posix_spawn(&pid, argv[0], &fa, NULL, (char* const*)argv, environ);
	}
	else
	{
		status = posix_spawn(&pid, argv[0], NULL, NULL, (char* const*)argv, environ);
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

	std::ifstream ifs(rpgsh_output_redirect_file.c_str());
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

	std::ifstream ifs(rpgsh_env_variables_file.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == v)
		{
			return data.substr(data.find(DS)+DS.length(),
			       data.length()-(data.find(DS)+DS.length()));
		}
	}
	ifs.close();
	return "";
}
void set_env_variable(std::string v,std::string value)
{
	confirmEnvVariablesFile();

	std::ifstream ifs(rpgsh_env_variables_file.c_str());
	std::filesystem::remove((rpgsh_env_variables_file+".bak").c_str());
	std::ofstream ofs((rpgsh_env_variables_file+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "" || data == "\n")// Prevents writing blank lines back to file
			continue;
		if(data.substr(0,data.find(DS)) == v)
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
	std::filesystem::remove(rpgsh_env_variables_file.c_str());
	std::filesystem::rename((rpgsh_env_variables_file+".bak").c_str(),rpgsh_env_variables_file.c_str());
}

std::map<std::string,std::string> load_vars_from_file(std::string path)
{
	std::map<std::string,std::string> vars;

	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \"%s\"",path.c_str());
		exit(-1);
	}

	std::ifstream ifs(path);
	if(!ifs.good())
	{
		output(Error,"Unable to open \"%s\" for reading",path.c_str());
		exit(-1);
	}
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "")
			break;
		std::string var_name = data.substr(0,data.find(DS));
		std::string var_value = data.substr(var_name.length()+DS.length(),
						data.length()-(var_name.length()+DS.length()));
		vars[var_name] = var_value;
	}
	ifs.close();

	return vars;
}

template <typename T>
void save_obj_to_file(std::string path, datamap<Dice> obj, char obj_id);
void save_obj_to_file(std::string path, datamap<Var> obj, char obj_id);

template <typename T>
datamap<Dice> load_obj_from_file(std::string path, char var_id);
datamap<Var> load_obj_from_file(std::string path, char var_id);

template <typename T>
datamap<Currency> loadDatamapFromAllScopes(char var_id);
