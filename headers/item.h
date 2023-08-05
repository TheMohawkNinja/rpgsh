#pragma once

#include <string.h>
#include "currency.h"
#include "dice.h"

class RPGSH_ITEM
{
	public:
		std::string	Name		=	"<NO_NAME>";
		std::string	Rarity		=	"<NO_RARITY>";
		RPGSH_CURRENCY	Value		=	RPGSH_CURRENCY();
		std::string	Type		=	"<NO_TYPE>";
		unsigned int	Weight		=	0;
		std::string	Description	=	"<NO_DESCRIPTION>";
		RPGSH_DICE	Damage		=	RPGSH_DICE();
		bool		Equipped	=	false;
	RPGSH_ITEM(){}
	RPGSH_ITEM(	std::string	_Name,
			std::string	_Rarity,
			RPGSH_CURRENCY	_Value,
			std::string	_Type,
			unsigned int	_Weight,
			std::string	_Description,
			RPGSH_DICE	_Damage,
			bool		_Equipped)
			{
				Name = _Name;
				Rarity = _Rarity;
				Value = _Value;
				Type = _Type;
				Weight = _Weight;
				Description = _Description;
				Damage = _Damage;
				Equipped = _Equipped;
			}

};
