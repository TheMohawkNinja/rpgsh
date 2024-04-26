#pragma once

#include "currency.h"
#include "dice.h"
#include "define.h"
#include "functions.h"

class rpgsh_char
{
	private:
		std::string characters_dir = campaigns_dir +
						get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
						"characters/";
	public:
		datamap<Var>	attr;
		datamap<Dice>	dice;
		Currency	currency = Currency();

	rpgsh_char();//Create character using default game
	rpgsh_char(std::string game);

	std::string Name();
	void save();
	void load(std::string character, bool load_bak);
	void set_as_current();
	void update_Name(std::string new_name_attr);
};
