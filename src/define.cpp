#include "../headers/define.h"
#include <unistd.h>//getlogin()

//Locations of files to get random data from for things like 'roll'
const char* random_seed_path = 		"/dev/random";
const char* backup_random_seed_path = 	"/dev/urandom";

//Internal file/directory paths
std::string user = 				getlogin();
std::string root_dir =				"/home/" + user + "/.rpgsh/";
std::string shell_vars_file = 			root_dir + ".vars";
std::string rpgsh_env_variables_file =		root_dir + ".rpgsh_variables";
std::string config_file = 			root_dir + "config";
std::string campaigns_dir = 			root_dir + "campaigns/";
std::string templates_dir = 			root_dir + "templates/";
std::string dice_lists_dir = 			root_dir + "dice-lists/";
std::string rpgsh_output_redirect_file =	"/tmp/rpgsh";
std::string prefix =				"rpgsh-";

//Data separator string. Name shortened for brevity in the code.
std::string DS = "::";
