#include <string>
#include <string.h>
#include <vector>
#include <cctype>
#include <climits>
#include "text.h"

class DNDSH_DICE
{
	private:
		std::string Count_str = "";
		std::string Faces_str = "";

		void get_Faces(std::string d, int start)
		{
			for(int i = start; i <= d.length(); i++)
			{
				if(i == d.length())
				{
					Faces = std::stoi(Faces_str);
					fprintf(stdout,"Dice faces = \"%d\"\n",Faces);
					break;
				}
				Faces_str += d.substr(i,1);
			}
		}
	public:
		unsigned int	Count	=	0;
		unsigned int	Faces	=	0;

	DNDSH_DICE(){}
	DNDSH_DICE(unsigned int _count,unsigned int _faces)
	{
		Count = _count;
		Faces = _faces;
	}
	DNDSH_DICE(std::string dice)
	{
		if(dice.substr(0,1) != "d")
		{
			try //Get number of dice
			{
				for(int i=0; dice.substr(i,1) != "d"; i++)
				{
					Count_str += dice.substr(i,1);
				}
				Count = std::stoi(Count_str);
				fprintf(stdout,"Dice count = \"%d\"\n",Count);
			}
			catch(...)
			{
				if(Count > 0)
				{
					fprintf(stderr,"%s%sERROR: Unable to get number of dice. First character is not \'d\' or an integer.%s\n",TEXT_BOLD,TEXT_RED,TEXT_NORMAL);
				}
				else
				{
					fprintf(stderr,"%s%sERROR: Unable to get number of dice. You've tried rolling too many dice (%s > %d)!.%s\n",TEXT_BOLD,TEXT_RED,Count_str.c_str(),INT_MAX,TEXT_NORMAL);
				}
			}
			get_Faces(dice,Count_str.length() + 1);
		}
		else if(dice.substr(0,1) == "d")
		{
			Count = 1;
			fprintf(stdout,"Dice count = \"%d\"\n",Count);
			get_Faces(dice,1);
		}
	}

	void roll()
	{
		
	}
};

class DNDSH_CURRENCY
{
	public:
		unsigned int 	Copper		=	0;
		unsigned int	Silver		=	0;
		unsigned int	Electrum	=	0;
		unsigned int	Gold		=	0;
		unsigned int	Platinum	=	0;
	DNDSH_CURRENCY(){}
	DNDSH_CURRENCY( unsigned int _Copper,
			unsigned int _Silver,
			unsigned int _Electrum,
			unsigned int _Gold,
			unsigned int _Platinum)
			{
				Copper = _Copper;
				Silver = _Silver;
				Electrum = _Electrum;
				Gold = _Gold;
				Platinum = _Platinum;
			}
};

class DNDSH_ITEM
{
	public:
		std::string	Name		=	"<NO_NAME>";
		std::string	Rarity		=	"<NO_RARITY>";
		DNDSH_CURRENCY	Value		=	DNDSH_CURRENCY();
		std::string	Type		=	"<NO_TYPE>";
		unsigned int	Weight		=	0;
		std::string	Description	=	"<NO_DESCRIPTION>";
		DNDSH_DICE	Damage		=	DNDSH_DICE();
		bool		Equipped	=	false;
	DNDSH_ITEM(){}
	DNDSH_ITEM(	std::string	_Name,
			std::string	_Rarity,
			DNDSH_CURRENCY	_Value,
			std::string	_Type,
			unsigned int	_Weight,
			std::string	_Description,
			DNDSH_DICE	_Damage,
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

class DNDSH_SPELL
{
	public:
		std::string	Name		=	"<NO_NAME>";
		unsigned char	Level		=	0;
		std::string	School		=	"<NO_SCHOOL>";
		std::string	Components	=	"<NO_COMPONENTS>";
		std::string	CastingTime	=	"<NO_CASTING_TIME>";
		std::string	Range		=	"<NO_RANGE>";
		std::string	Duration	=	"<NO_DURATION>";
		std::string	Description	=	"<NO_DESCRIPTION>";
};

class DNDSH_CHARACTER
{
	public:
		char				Name[32]		=	"<NO_NAME>";
		std::string			Class			=	"<NO_CLASS>";
		unsigned char			Level			=	1;
		std::string			Background		=	"<NO_BACKGROUND>";
		std::string			Player			=	"<NO_PLAYER>";
		std::string			Race			=	"<NO_RACE>";
		std::string			Alignment		=	"<NO_ALIGNMENT>";
		unsigned int			XP			=	0;
		bool				Inspiration		=	false;
		unsigned char			Proficiency		=	0;
		unsigned char			AC			=	0;
		unsigned char			Inititiative		=	0;
		unsigned char			Speed			=	0;

		unsigned char			HP			=	0;
		unsigned char			MaxHP			=	0;
		unsigned char			TempHP			=	0;
		DNDSH_DICE			CurrentHitDice		=	DNDSH_DICE();
		DNDSH_DICE			TotalHitDice		=	DNDSH_DICE();
		unsigned char			FailedDeathSaves	=	0;
		unsigned char			SucceededDeathSaves	=	0;

		unsigned char			Str			=	0;
		unsigned char			Dex			=	0;
		unsigned char			Con			=	0;
		unsigned char			Int			=	0;
		unsigned char			Wis			=	0;
		unsigned char			Cha			=	0;

		std::string			PersonalityTraits	=	"<NO_PERSONALITY_TRAITS>";
		std::string			Ideals			=	"<NO_IDEALS>";
		std::string			Bonds			=	"<NO_BONDS>";
		std::string			Flaws			=	"<NO_FLAWS>";
		std::string			FeaturesAndTraits	=	"<NO_FEATURES_AND_TRAITS>";
		DNDSH_CURRENCY			Currency		=	DNDSH_CURRENCY();

		unsigned int			Age			=	0;
		unsigned int			Height			=	0;
		unsigned int			Weight			=	0;
		std::string			EyeColor		=	"<NO_EYE_COLOR>";
		std::string			SkinColor		=	"<NO_SKIN_COLOR>";
		std::string			Hair			=	"<NO_HAIR>";

		std::string			Appearance		=	"<NO_APPEARANCE>";
		std::string			Allies			=	"<NO_ALLIES_OR_ORGANIZATIONS>";
		std::string			Backstory		=	"<NO_BACKSTORY>";
		std::string			Treasure		=	"<NO_TREASURE>";

		std::string			SpellcastingAbility	=	"<NO_SPELLCASTING_ABILITY>";
		unsigned char			SpellSaveDC		=	0;
		unsigned char			SpellAttackBonus	=	0;
		std::vector<DNDSH_SPELL>	Spellbook		=	{};
};

