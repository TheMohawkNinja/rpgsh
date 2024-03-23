#pragma once

#include "currency.h"
#include "dice.h"
#include "define.h"
#include "functions.h"

class RPGSH_CHAR
{
	private:
		std::string characters_dir = campaigns_dir +
						get_shell_var(CURRENT_CAMPAIGN_SHELL_VAR) +
						"characters/";
	public:
		std::string AttributeDesignator		=	"Attr";
		std::string DiceDesignator		=	"Dice";
		std::string CurrencyDesignator		=	"Currency";

		RPGSH_OBJ<RPGSH_VAR>	Attr;
		RPGSH_OBJ<RPGSH_DICE>	Dice;
		RPGSH_CURRENCY		Currency	=	RPGSH_CURRENCY();

	RPGSH_CHAR();//Create character using default game
	RPGSH_CHAR(std::string game);

	std::string Name();
	void save();
	void load(std::string character, bool load_bak);
	void set_as_current();
	void update_Name(std::string new_name_attr);
};
