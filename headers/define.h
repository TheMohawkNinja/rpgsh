#pragma once

#include <map>
#include <string>

//Version info
#define MAJOR_VERSION			0
#define MINOR_VERSION			24
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
#define OP_NE				"<>"
#define OP_AND				"&&"
#define OP_OR				"||"
#define OP_ASSIGN			"="
#define OP_ADDA				"+="
#define OP_SUBA				"-="
#define OP_MULA				"*="
#define OP_DIVA				"/="
#define OP_EXPA				"^="
#define OP_MODA				"%="

//Useful keyboard macros
#define KB_TAB				9
#define KB_ENTER			10
#define KB_BACKSPACE			127

//Environment variables
#define ENV_CURRENT_CHARACTER		"CURRENT_CHARACTER"
#define ENV_CURRENT_CAMPAIGN		"CURRENT_CAMPAIGN"

//Special hidden variables
#define DOT_NAME			".NAME"
#define DOT_PROMPT			".PROMPT"

//Setting names
#define PADDING				"padding"
#define DEFAULT_GAME			"default_game"
#define HIDE_TIPS			"hide_tips"
#define HISTORY_LENGTH			"history_length"
#define ALIASES				"aliases"
#define STARTUP_APPS			"startup_apps"
#define PRE_RUN_APPS			"pre_run_apps"
#define POST_RUN_APPS			"post_run_apps"
#define VERBOSITY			"verbosity"

//Universal flags
#define FLAG_APPDESCRIPTION		"--appdescription"
#define FLAG_MODIFYVARIABLES		"--modifyvariables"
#define FLAG_HELPSHORT			"-?"
#define FLAG_HELPLONG			"--help"

//Custom exceptions
#define E_INVALID_PROPERTY		"invalid_property"
#define E_INVALID_OPERATION		"invalid_operation"
#define E_INVALID_EXPLICIT_CONSTRUCTOR	"invalid_explicit_constructor"
#define E_UNKNOWN_OPERATION		"unknown_operation"
#define E_INSUFFICIENT_FUNDS		"insufficient_funds"
#define E_NO_PRNG_FILE			"no_prng_file"
#define E_FILE_NOT_FOUND		"file_not_found"
#define E_BAD_FS			"bad_fs"

//Column formatting
#define COLUMN_PADDING			5

//Color-coding for variable data types
#define CURRENCY_COLOR			TEXT_YELLOW
#define DICE_COLOR			TEXT_LIGHTBLUE
#define VAR_COLOR			TEXT_CYAN
#define WALLET_COLOR			TEXT_MAGENTA

//Special return codes
#define STATUS_BREAK			100
#define STATUS_CONTINUE			101

//Internal file/directory paths
//dir = Directory
//file = File name
//path = Directory + file name
#define RPGSH_INSTALL_DIR		"/usr/local/bin/"
extern const char* prng_path;
extern const char* backup_prng_path;
extern const char* rpgsh_programs_cache_path;
extern const char* rpgsh_output_redirect_path;
extern std::string variables_file_name;
extern std::string user;
extern std::string root_dir;
extern std::string shell_variables_path;
extern std::string rpgsh_env_variables_path;
extern std::string configuration_path;
extern std::string campaigns_dir;
extern std::string templates_dir;
extern std::string dice_lists_dir;
extern std::string scripts_dir;
extern std::string history_path;
extern std::string prefix;
extern std::string c_ext;
extern std::string autorun_ext;

//RegEx patterns
extern std::string arg_pattern_str;
extern std::string variable_pattern_str;
extern std::string exp_const_pattern_str;
extern std::string d_imp_const_pattern_str;
extern std::string set_pattern_str;

//Data separator string. Name shortened for brevity in the code.
extern std::string DS;

//Universal formatted string for user discrimination between empty string and whitespace-only string
extern std::string empty_str;

//typedef for handing the various std::string key'd maps that are used
template <typename T>
using datamap = std::map<std::string, T>;
