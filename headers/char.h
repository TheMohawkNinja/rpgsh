#pragma once

#include <string.h>
#include "config.h"
#include "currency.h"
#include "dice.h"
#include "functions.h"
#include "spell.h"

class RPGSH_CHAR
{
	private:
		std::string characters_dir = campaigns_dir +
						get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
						"characters/";
	public:
		std::string AttributeDesignator		=	"Attr";
		std::string SpellDesignator		=	"Spell";
		std::string DiceDesignator		=	"Dice";
		std::string CurrencyDesignator		=	"Currency";

		RPGSH_OBJ<RPGSH_VAR> Attr;
		RPGSH_OBJ<RPGSH_DICE> Dice;
		RPGSH_CURRENCY		Currency	=	RPGSH_CURRENCY();
		std::vector<RPGSH_SPELL>Spellbook	=	{};

	RPGSH_CHAR()//Create character using default game
	{
		RPGSH_CONFIG config = RPGSH_CONFIG();

		Attr = load_obj_from_file<RPGSH_VAR>(templates_dir+config.setting[DEFAULT_GAME],AttributeDesignator);
		Attr[CHAR_NAME_ATTR] = "/Name";

		Dice = load_obj_from_file<RPGSH_DICE>(templates_dir+config.setting[DEFAULT_GAME],DiceDesignator);

	}
	RPGSH_CHAR(std::string game)
	{
		Attr = load_obj_from_file<RPGSH_VAR>(templates_dir+game,AttributeDesignator);
		Attr[CHAR_NAME_ATTR] = "/Name";

		Dice = load_obj_from_file<RPGSH_DICE>(templates_dir+game,DiceDesignator);
	}

	std::string Name()
	{
		return std::string(Attr[std::string(Attr[CHAR_NAME_ATTR])]);
	}
	void save()
	{
		std::string char_path = characters_dir+Name()+".char";

		if(std::filesystem::exists(char_path.c_str()))
		{
			std::filesystem::rename(char_path.c_str(),(char_path+".bak").c_str());
		}

		save_obj_to_file<RPGSH_OBJ<RPGSH_VAR>>(char_path, Attr, AttributeDesignator);
		save_obj_to_file<RPGSH_OBJ<RPGSH_DICE>>(char_path, Dice, DiceDesignator);
	}
	void load(std::string character, bool load_bak)
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
	void set_as_current()
	{
		set_shell_var(CURRENT_CHAR_SHELL_VAR,Name());
	}
	void update_Name(std::string new_name_attr)
	{
		Attr[CHAR_NAME_ATTR] = new_name_attr;
		set_as_current();
		save();
	}
};
