#include <fstream>
#include <filesystem>
#include "../headers/char.h"
#include "../headers/config.h"
#include "../headers/output.h"

rpgsh_char::rpgsh_char()//Create character using default game
{
	rpgsh_config config = rpgsh_config();

	Attr = load_obj_from_file<var_t>(templates_dir+config.setting[DEFAULT_GAME],AttributeDesignator);
	Dice = load_obj_from_file<dice_t>(templates_dir+config.setting[DEFAULT_GAME],DiceDesignator);
}
rpgsh_char::rpgsh_char(std::string game)
{
	Attr = load_obj_from_file<var_t>(templates_dir+game,AttributeDesignator);
	Dice = load_obj_from_file<dice_t>(templates_dir+game,DiceDesignator);
}

std::string rpgsh_char::Name()
{
	return std::string(Attr[std::string(Attr[CHAR_NAME_ATTR])]);
}
void rpgsh_char::save()
{
	std::string char_path = characters_dir+Name()+".char";

	if(std::filesystem::exists(char_path.c_str()))
	{
		std::filesystem::rename(char_path.c_str(),(char_path+".bak").c_str());
	}

	save_obj_to_file<RPGSH_OBJ<var_t>>(char_path, Attr, AttributeDesignator);
	save_obj_to_file<RPGSH_OBJ<dice_t>>(char_path, Dice, DiceDesignator);
}
void rpgsh_char::load(std::string character, bool load_bak)
{
	std::string char_path = characters_dir+character+".char"+((load_bak)?".bak":"");

	std::ifstream fs(char_path.c_str());
	if(!fs.good())
	{
		//This should only apply if 'character' gets set to something that isn't a valid character
		//Generate default character and set that as the current character
		output(Warning,"Unable to open character file for \'%s\', loading default character.",character.c_str());
		rpgsh_char dummy = rpgsh_char();
		dummy.save();
		dummy.set_as_current();
		char_path = characters_dir+Name()+".char"+((load_bak)?".bak":"");
		fs.open(char_path.c_str());
	}

	//Clear attributes in-case a character from a non-default game is being loaded
	Attr.clear();
	Dice.clear();

	Attr = load_obj_from_file<var_t>(char_path,AttributeDesignator);
	Dice = load_obj_from_file<dice_t>(char_path,DiceDesignator);

	fs.close();
}
void rpgsh_char::set_as_current()
{
	set_shell_var(CURRENT_CHAR_SHELL_VAR,Name());
}
void rpgsh_char::update_Name(std::string new_name_attr)
{
	Attr[CHAR_NAME_ATTR] = new_name_attr;
	set_as_current();
	save();
}
