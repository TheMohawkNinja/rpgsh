#pragma once

#include <map>
#include <string>

//Version info
#define MAJOR_VERSION			0
#define MINOR_VERSION			8
#define PATCH_VERSION			2

//Comment character definition
#define COMMENT				'!'

//Variable character Scope sigils
#define SHELL_SIGIL			'$'
#define CAMPAIGN_SIGIL			'#'
#define CHARACTER_SIGIL			'@'

//Variable character type sigils
#define CURRENCY_SIGIL			'c'
#define CURRENCYSYSTEM_SIGIL		's'
#define DICE_SIGIL			'd'
#define VAR_SIGIL			'v'
#define WALLET_SIGIL			'w'

//Built-in variables
#define CURRENT_CHAR_SHELL_VAR		".CURRENT_CHAR"
#define CURRENT_CAMPAIGN_SHELL_VAR	".CURRENT_CAMPAIGN"
#define CHAR_NAME_ATTR			".NAME"

//Setting names
#define PADDING				"padding"
#define DEFAULT_GAME			"default_game"
#define HIDE_TIPS			"hide_tips"

//Universal flags
#define FLAG_APPDESCRIPTION		"--appdescription"

//Column formatting
#define COLUMN_PADDING			5

//Color-coding for variable data types
#define CURRENCY_COLOR			TEXT_YELLOW
#define CURRENCYSYSTEM_COLOR		TEXT_RED
#define DICE_COLOR			TEXT_LIGHTBLUE
#define VAR_COLOR			TEXT_WHITE
#define WALLET_COLOR			TEXT_MAGENTA

//Locations of files to get random data from for things like 'roll'
extern const char* random_seed_path;
extern const char* backup_random_seed_path;

//Internal file/directory paths
extern std::string user;
extern std::string root_dir;
extern std::string shell_vars_file;
extern std::string rpgsh_env_variables_file;
extern std::string config_file;
extern std::string campaigns_dir;
extern std::string templates_dir;
extern std::string dice_lists_dir;
extern std::string rpgsh_output_redirect_file;
extern std::string prefix;

//Data separator string. Name shortened for brevity in the code.
extern std::string DS;

//typedef for handing the various std::string key'd maps that are used
template <typename T>
using datamap = std::map<std::string, T>;
