#include "../headers/define.h"
#include "../headers/text.h"
#include <unistd.h>//getlogin()

//Internal file/directory paths
//dir = Directory
//file = File name
//path = Directory + file name
const char* prng_path = 			"/dev/random";
const char* backup_prng_path = 			"/dev/urandom";
const char* rpgsh_programs_cache_path =		"/tmp/rpgsh_proglist";
const char* rpgsh_output_redirect_path =	"/tmp/rpgsh";
std::string variables_file_name =		".variables";
std::string user = 				getlogin();
std::string root_dir =				"/home/" + user + "/.rpgsh/";
std::string shell_variables_path =		root_dir + variables_file_name;
std::string rpgsh_env_variables_path =		root_dir + ".env_variables";
std::string config_path = 			root_dir + "config";
std::string campaigns_dir = 			root_dir + "campaigns/";
std::string templates_dir = 			root_dir + "templates/";
std::string dice_lists_dir = 			root_dir + "dice-lists/";
std::string history_path =			root_dir + ".history";
std::string prefix =				"rpgsh-";
std::string c_ext =				".char";
std::string autorun_ext =			".autorun";

//RegEx patterns
std::string arg_pattern_str =			"[^\\s]+";
std::string variable_pattern_str =		"["+
						std::string(1,CHARACTER_SIGIL)+
						std::string(1,CAMPAIGN_SIGIL)+
						std::string(1,SHELL_SIGIL)+
						"]["+
						std::string(1,VAR_SIGIL)+
						std::string(1,DICE_SIGIL)+
						std::string(1,CURRENCY_SIGIL)+
						std::string(1,WALLET_SIGIL)+
						"]?[\\w\\/\\.]+";
std::string exp_const_pattern_str =		"["+
						std::string(1,VAR_SIGIL)+
						std::string(1,DICE_SIGIL)+
						std::string(1,CURRENCY_SIGIL)+
						std::string(1,WALLET_SIGIL)+
						"]{1}\\{.*\\}";
std::string d_imp_const_pattern_str =		"[0-9]*d[0-9]+[+-]?[0-9]*";
std::string set_pattern_str =			"(([\\.a-zA-Z0-9]+\\/*)*::["+
						std::string(1,VAR_SIGIL)+
						std::string(1,DICE_SIGIL)+
						std::string(1,CURRENCY_SIGIL)+
						std::string(1,WALLET_SIGIL)+
						"]\\{.*(?!::)\\}(::)?)*";

//Data separator string. Name shortened for brevity in the code.
std::string DS = "::";

//Universal formatted string for user discrimination between empty string and whitespace-only string
std::string empty_str = std::string(TEXT_BOLD)+
			std::string(TEXT_ITALIC)+
			std::string(TEXT_BG_YELLOW)+
			std::string(TEXT_BLACK)+
			"<EMPTY>"+
			std::string(TEXT_NORMAL);
