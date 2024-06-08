#include "../headers/define.h"
#include "../headers/text.h"
#include <unistd.h>//getlogin()

//Internal file/directory paths
const char* random_seed_path = 			"/dev/random";
const char* backup_random_seed_path = 		"/dev/urandom";
std::string variable_file_name =		".variables";
std::string user = 				getlogin();
std::string root_dir =				"/home/" + user + "/.rpgsh/";
std::string shell_variables_file =		root_dir + variable_file_name;
std::string rpgsh_env_variables_file =		root_dir + ".rpgsh_variables";
std::string config_file = 			root_dir + "config";
std::string campaigns_dir = 			root_dir + "campaigns/";
std::string templates_dir = 			root_dir + "templates/";
std::string dice_lists_dir = 			root_dir + "dice-lists/";
std::string rpgsh_output_redirect_file =	"/tmp/rpgsh";
std::string prefix =				"rpgsh-";

//Data separator string. Name shortened for brevity in the code.
std::string DS = "::";

//Universal formatted string for user discrimination between empty string and whitespace-only string
std::string empty_str = std::string(TEXT_BOLD)+
			std::string(TEXT_ITALIC)+
			std::string(TEXT_BG_YELLOW)+
			std::string(TEXT_BLACK)+
			"<EMPTY>"+
			std::string(TEXT_NORMAL);
