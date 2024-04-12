#include <cstdarg>
#include <fcntl.h>
#include <map>
#include <spawn.h>
#include <string.h>
#include <sys/wait.h>
#include "../headers/char.h"
#include "../headers/config.h"
#include "../headers/dice.h"
#include "../headers/functions.h"

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

void confirmShellVarsFile()
{
	if(!std::filesystem::exists(shell_vars_file.c_str()))
	{
		output(Info,"Shell variables file not found, creating file at \'%s\'.",shell_vars_file.c_str());
		std::ofstream ofs(shell_vars_file.c_str());
		ofs.close();

		rpgsh_config config = rpgsh_config();
		rpgsh_char c = rpgsh_char();

		//Set default values for built-in shell variables
		datamap<var_t> Attr = load_obj_from_file<var_t>(templates_dir+config.setting[DEFAULT_GAME],VAR_SIGIL);
		set_shell_var(CURRENT_CHAR_SHELL_VAR,c.Name());
		set_shell_var(CURRENT_CAMPAIGN_SHELL_VAR,"default/");
	}
}

void confirmCampaignVarsFile()
{
	std::string campaign_vars_file = campaigns_dir +
					get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
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
	rpgsh_config config = rpgsh_config();

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

void run_rpgsh_prog(std::string args, bool redirect_output)
{
	rpgsh_char c = rpgsh_char();

	std::vector<std::string> vars;
	extern char** environ;
	pid_t pid;

	if(args[0] == CHAR_SIGIL ||
	args[0] == CAMPAIGN_SIGIL ||
	args[0] == SHELL_SIGIL) //Check if user is operating on a variable
	{
		args = "variables " + args;
	}

	int params_start = args.find(" ", args.find(" ")+1);
	std::string path = "/bin/";

	if(params_start != std::string::npos)
	{
		//Replaces all instances of variables with their respective value
		//Except for the first arg if it is a variable
		//TODO if the regex lib is ever properly fully-implemented, replace this shit with regex like God intended
		for(int i=params_start+1; i<args.length(); i++)
		{
			params_start = args.find(" ", args.find(" ")+1);
			std::string var = "";
			if(args[i] == CHAR_SIGIL)
			{
				int find_percent = args.find(CHAR_SIGIL,params_start);
				for(int j=i+1; j<args.length() && args.substr(j,1) != " "; j++)
				{
					var+=args.substr(j,1);
				}
				int new_args_start = find_percent + var.length() + 1;
				args = args.substr(0,find_percent)+c.Attr[var].c_str()+
				       args.substr(new_args_start,args.length()-(args.substr(0,new_args_start).length()));
			}
			else if(args[i] == CAMPAIGN_SIGIL)
			{
				std::string campaign_vars_file = campaigns_dir +
								get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
								".vars";
				int sigil_pos = args.find(CAMPAIGN_SIGIL,params_start);
				for(int j=i+1; j<args.length() && args.substr(j,1) != " "; j++)
				{
					var+=args.substr(j,1);
				}

				std::ifstream ifs(campaign_vars_file.c_str());
				while(!ifs.eof())
				{
					std::string old;
					std::string data = "";
					std::getline(ifs,data);

					if(data.substr(0,data.find(DS)) == var)
					{
						int new_args_start = sigil_pos + var.length() + 1;
						old = data.substr(data.find(DS)+DS.length(),
								  data.length()-(data.find(DS)+DS.length()));

						if(old.find(" ") != std::string::npos)
						{
							old = "\"" + old + "\"";
						}

						args = args.substr(0,sigil_pos)+
						       old+
						       args.substr(new_args_start,args.length()-(args.substr(0,new_args_start).length()));
						break;
					}
				}
				ifs.close();
			}
			else if(args[i] == SHELL_SIGIL)
			{
				int sigil_pos = args.find(SHELL_SIGIL,params_start);
				for(int j=i+1; j<args.length() && args.substr(j,1) != " "; j++)
				{
					var+=args.substr(j,1);
				}

				std::ifstream ifs(shell_vars_file.c_str());
				while(!ifs.eof())
				{
					std::string old;
					std::string data = "";
					std::getline(ifs,data);

					if(data.substr(0,data.find(DS)) == var)
					{
						int new_args_start = sigil_pos + var.length() + 1;
						old = data.substr(data.find(DS)+DS.length(),
								  data.length()-(data.find(DS)+DS.length()));

						if(old.find(" ") != std::string::npos)
						{
							old = "\"" + old + "\"";
						}

						args = args.substr(0,sigil_pos)+
						       old+
						       args.substr(new_args_start,args.length()-(args.substr(0,new_args_start).length()));
						break;
					}
				}
				ifs.close();
			}
		}
	}
	if(args.find(" ") != std::string::npos)
	{
		vars.push_back(path+prefix+args.substr(0,args.find(" ")));
		args = args.substr(args.find(" ")+1,(args.length() - args.find(" ")+1));

		for(int i=0; i<args.length(); i++)
		{
			if(args.substr(i,1) == "\"")//Combine args wrapped in quotes
			{
				if(args.find("\"",i+1) == std::string::npos)
				{
					output(Error,"Unmatched quote in argument list.");
					return;
				}
				vars.push_back(args.substr(i+1,(args.find("\"",i+1)-i-1)));
				i += vars[vars.size()-1].length()+1;
			}
			else
			{
				if(args.find(" ",i) != std::string::npos)
				{
					std::string var = args.substr(i,args.find(" ",i)-i);
					bool var_is_valid = false;
					for(int c_i=0; c_i<var.length(); c_i++)//Make sure we're not just grabbing extraneous whitespace
					{
						std::string c = var.substr(c_i,1);
						if(c != " " && c != "")
						{
							var_is_valid = true;
						}
					}

					if(var_is_valid)
					{
						vars.push_back(args.substr(i,args.find(" ",i)-i));
						i+=vars[vars.size()-1].length();
					}
				}
				else
				{
					vars.push_back(args.substr(i,args.length()-i));
					break;
				}
			}
		}
	}
	else//If only one arg is called
	{
		vars.push_back(path+prefix+args);
	}

	//Convert vector to char*[]
	char* argv[vars.size()+1];
	for(int i=0; i<vars.size(); i++)
	{
		argv[i] = (char*)vars[i].c_str();
	}

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

std::string get_shell_var(std::string var)
{
	confirmShellVarsFile();

	std::ifstream ifs(shell_vars_file.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == var)
		{
			return data.substr(data.find(DS)+DS.length(),
				data.length()-(data.find(DS)+DS.length()));
		}
	}
	ifs.close();
	return "";
}
void set_shell_var(std::string var,std::string value)
{
	std::ifstream ifs(shell_vars_file.c_str());
	std::filesystem::remove((shell_vars_file+".bak").c_str());
	std::ofstream ofs((shell_vars_file+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "" || data == "\n")// Prevents writing blank lines back to file
			continue;
		if(data.substr(0,data.find(DS)) == var)
		{
			ofs<<var + DS + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue)
	{
		ofs<<var + DS + value + "\n";
	}
	ifs.close();
	ofs.close();
	std::filesystem::remove(shell_vars_file.c_str());
	std::filesystem::rename((shell_vars_file+".bak").c_str(),shell_vars_file.c_str());
}

std::string get_campaign_var(std::string var)
{
	confirmCampaignVarsFile();

	std::string campaign_vars_file = campaigns_dir +
					get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
					".vars";
	std::ifstream ifs(campaign_vars_file.c_str());
	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);

		if(data.substr(0,data.find(DS)) == var)
		{
			return data.substr(data.find(DS)+DS.length(),
				data.length()-(data.find(DS)+DS.length()));
		}
	}
	ifs.close();
	return "";
}
void set_campaign_var(std::string var,std::string value)
{
	std::string campaign_vars_file = campaigns_dir +
					get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
					".vars";
	std::ifstream ifs(campaign_vars_file.c_str());
	std::filesystem::remove((campaign_vars_file+".bak").c_str());
	std::ofstream ofs((campaign_vars_file+".bak").c_str());
	bool ReplacedValue = false;

	while(!ifs.eof())
	{
		std::string data = "";
		std::getline(ifs,data);
		if(data == "" || data == "\n")// Prevents writing blank lines back to file
			continue;
		if(data.substr(0,data.find(DS)) == var)
		{
			ofs<<var + DS + value + "\n";
			ReplacedValue = true;
		}
		else
		{
			ofs<<data + "\n";
		}
	}

	if(!ReplacedValue)
	{
		ofs<<var + DS + value + "\n";
	}
	ifs.close();
	ofs.close();
	std::filesystem::remove(campaign_vars_file.c_str());
	std::filesystem::rename((campaign_vars_file+".bak").c_str(),campaign_vars_file.c_str());
}

std::map<std::string,std::string> load_vars_from_file(std::string path)
{
	std::map<std::string,std::string> vars;

	if(!std::filesystem::exists(path))
	{
		output(Error,"File not found at \'%s\'",path.c_str());
		exit(-1);
	}

	std::ifstream ifs(path);
	if(!ifs.good())
	{
		output(Error,"Unable to open \'%s\' for reading",path.c_str());
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
void save_obj_to_file(std::string path, datamap<dice_t> obj, char obj_id);
void save_obj_to_file(std::string path, datamap<var_t> obj, char obj_id);

template <typename T>
datamap<dice_t> load_obj_from_file(std::string path, char var_id);
datamap<var_t> load_obj_from_file(std::string path, char var_id);

template <typename T>
datamap<currency_t> loadDatamapFromAllScopes(char var_id);
