#pragma once

#include <map>
#include <string>

//Version info
#define MAJOR_VERSION			0
#define MINOR_VERSION			11
#define PATCH_VERSION			0

//Comment character definition
#define COMMENT				'!'

//Variable character Scope sigils
#define SHELL_SIGIL			'$'
#define CAMPAIGN_SIGIL			'#'
#define CHARACTER_SIGIL			'@'

//Variable character type sigils
#define CURRENCY_SIGIL			'c'
#define DICE_SIGIL			'd'
#define VAR_SIGIL			'v'
#define WALLET_SIGIL			'w'

//Operations
#define OP_INC				"++"
#define OP_DEC				"--"
#define OP_MUL				"*"
#define OP_DIV				"/"
#define OP_EXP				"^"
#define OP_MOD				"%"
#define OP_ADD				"+"
#define OP_SUB				"-"
#define OP_LT				"<"
#define OP_LE				"<="
#define OP_GT				">"
#define OP_GE				">="
#define OP_EQ				"=="
#define OP_NE				"!="
#define OP_AEQ				"~="
#define OP_AND				"&&"
#define OP_OR				"||"
#define OP_ASSIGN			"="
#define OP_ADDA				"+="
#define OP_SUBA				"-="
#define OP_MULA				"*="
#define OP_DIVA				"/="
#define OP_EXPA				"^="
#define OP_MODA				"%="

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

//Custom exceptions
#define E_INVALID_PROPERTY		"invalid_property"
#define E_INVALID_OPERATION		"invalid_operation"
#define E_INVALID_EXPLICIT_CONSTRUCTOR	"invalid_explicit_constructor"
#define E_UNKNOWN_OPERATION		"unknown_operation"
#define E_INSUFFICIENT_FUNDS		"insufficient_funds"

//Column formatting
#define COLUMN_PADDING			5

//Color-coding for variable data types
#define CURRENCY_COLOR			TEXT_YELLOW
#define CURRENCYSYSTEM_COLOR		TEXT_RED
#define DICE_COLOR			TEXT_LIGHTBLUE
#define VAR_COLOR			TEXT_CYAN
#define WALLET_COLOR			TEXT_MAGENTA

//Internal file/directory paths
//dir = Directory
//file = File name
//path = Directory + file name
#define RPGSH_INSTALL_DIR		"/bin/"
extern const char* random_seed_path;
extern const char* backup_random_seed_path;
extern const char* rpgsh_programs_cache_path;
extern const char* rpgsh_output_redirect_path;
extern std::string variable_file_name;
extern std::string user;
extern std::string root_dir;
extern std::string shell_variables_path;
extern std::string rpgsh_env_variables_path;
extern std::string config_path;
extern std::string campaigns_dir;
extern std::string templates_dir;
extern std::string dice_lists_dir;
extern std::string prefix;

//RegEx patterns
extern std::string arg_pattern_str;
extern std::string variable_pattern_str;
extern std::string exp_const_pattern_str;
extern std::string set_pattern_str;

//Data separator string. Name shortened for brevity in the code.
extern std::string DS;

//Universal formatted string for user discrimination between empty string and whitespace-only string
extern std::string empty_str;

//typedef for handing the various std::string key'd maps that are used
template <typename T>
using datamap = std::map<std::string, T>;
