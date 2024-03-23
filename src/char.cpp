#include <fstream>
#include <filesystem>
#include "../headers/char.h"
#include "../headers/config.h"
#include "../headers/output.h"

RPGSH_CHAR::RPGSH_CHAR()//Create character using default game
{
	RPGSH_CONFIG config = RPGSH_CONFIG();

	Attr = load_obj_from_file<RPGSH_VAR>(templates_dir+config.setting[DEFAULT_GAME],AttributeDesignator);
	Dice = load_obj_from_file<RPGSH_DICE>(templates_dir+config.setting[DEFAULT_GAME],DiceDesignator);
}
RPGSH_CHAR::RPGSH_CHAR(std::string game)
{
	Attr = load_obj_from_file<RPGSH_VAR>(templates_dir+game,AttributeDesignator);
	Dice = load_obj_from_file<RPGSH_DICE>(templates_dir+game,DiceDesignator);
}

std::string RPGSH_CHAR::Name()
{
	return std::string(Attr[std::string(Attr[CHAR_NAME_ATTR])]);
}
void RPGSH_CHAR::save()
{
	std::string char_path = characters_dir+Name()+".char";

	if(std::filesystem::exists(char_path.c_str()))
	{
		std::filesystem::rename(char_path.c_str(),(char_path+".bak").c_str());
	}

	save_obj_to_file<RPGSH_OBJ<RPGSH_VAR>>(char_path, Attr, AttributeDesignator);
	save_obj_to_file<RPGSH_OBJ<RPGSH_DICE>>(char_path, Dice, DiceDesignator);
}
void RPGSH_CHAR::load(std::string character, bool load_bak)
{
	std::string char_path = characters_dir+character+".char"+((load_bak)?".bak":"");

	std::ifstream fs(char_path.c_str());
	if(!fs.good())
	{
		//This should only apply if 'character' gets set to something that isn't a valid character
		//Generate default character and set that as the current character
		output(Warning,"Unable to open character file for \'%s\', loading default character.",character.c_str());
		RPGSH_CHAR dummy = RPGSH_CHAR();
		dummy.save();
		dummy.set_as_current();
		char_path = characters_dir+Name()+".char"+((load_bak)?".bak":"");
		fs.open(char_path.c_str());
	}

	//Clear attributes in-case a character from a non-default game is being loaded
	Attr.clear();
	Dice.clear();

	Attr = load_obj_from_file<RPGSH_VAR>(char_path,AttributeDesignator);
	Dice = load_obj_from_file<RPGSH_DICE>(char_path,DiceDesignator);

	fs.close();
}
void RPGSH_CHAR::set_as_current()
{
	set_shell_var(CURRENT_CHAR_SHELL_VAR,Name());
}
void RPGSH_CHAR::update_Name(std::string new_name_attr)
{
	Attr[CHAR_NAME_ATTR] = new_name_attr;
	set_as_current();
	save();
}
