#pragma once

#include <string>

//Version info
#define MAJOR_VERSION	0
#define MINOR_VERSION	7
#define PATCH_VERSION	3

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
extern const char* random_seed_path;
extern const char* backup_random_seed_path;

//Internal file/directory paths
extern std::string user;
extern std::string root_dir;
extern std::string shell_vars_file;
extern std::string config_file;
extern std::string campaigns_dir;
extern std::string templates_dir;
extern std::string dice_lists_dir;
extern std::string rpgsh_output_redirect_file;

//Data separator string. Name shortened for brevity in the code.
extern std::string DS;
