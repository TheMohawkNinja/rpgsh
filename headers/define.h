#pragma once

#include <string.h>

//Version info
#define MAJOR_VERSION		0
#define MINOR_VERSION		5
#define PATCH_VERSION		8

//Variable character definitions
#define SHELL_VAR		'$'
#define CHAR_VAR		'%'

//Built-in variables
#define CURRENT_CHAR_SHELL_VAR	".CURRENT_CHAR"
#define CHAR_NAME_ATTR		".NAME"

//Setting names
#define PADDING			"padding"
#define DEFAULT_GAME		"default_game"

//Locations of files to get random data from for things like 'roll'
const char* random_seed_path = "/dev/random";
const char* backup_random_seed_path = "/dev/urandom";

//Internal file/directory paths
std::string user = getlogin();
std::string root_dir = "/home/"+user+"/.rpgsh/";
std::string shell_vars_file = root_dir+".shell";
std::string config_file = root_dir+"config";
std::string characters_dir = root_dir + "characters/";
std::string templates_dir = root_dir + "templates/";

//Data separator string. Name shortened for brevity in the code.
std::string DS = "::";

