#pragma once

#include <string>
#include <unistd.h>//getlogin()

//Version info
#define MAJOR_VERSION	0
#define MINOR_VERSION	6
#define PATCH_VERSION	2

//Comment character definition
#define COMMENT		'#'

//Variable character scope sigils
#define SHELL_SIGIL	'$'
#define CAMPAIGN_SIGIL	'#'
#define CHAR_SIGIL	'@'

//Built-in variables
#define CURRENT_CHAR_SHELL_VAR		".CURRENT_CHAR"
#define CURRENT_CAMPAIGN_SHELL_VAR	".CURRENT_CAMPAIGN"
#define CHAR_NAME_ATTR			".NAME"

//Setting names
#define PADDING		"padding"
#define DEFAULT_GAME	"default_game"

//Locations of files to get random data from for things like 'roll'
const char* random_seed_path = "/dev/random";
const char* backup_random_seed_path = "/dev/urandom";

//Internal file/directory paths
std::string user = getlogin();
std::string root_dir = "/home/"+user+"/.rpgsh/";
std::string shell_vars_file = root_dir+".vars";
std::string config_file = root_dir+"config";
std::string campaigns_dir = root_dir + "campaigns/";
std::string templates_dir = root_dir + "templates/";
std::string dice_lists_dir = root_dir + "dice-lists/";

//Data separator string. Name shortened for brevity in the code.
std::string DS = "::";
