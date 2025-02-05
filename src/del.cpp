#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(Error,"del expects at least 1 argument.");
		exit(-1);
	}
	if(argc > 3)
		output(Warning,"del only expects 1, 2, or 3 arguments, ignoring all other arguments");
	chkFlagAppDesc(argv,"Deletes a variable, variable set, character, or campaign.");
	chkFlagModifyVariables(argv,true);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tdel [%sOPTIONS%s] %svalue%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tChecks %svalue%s against variables, variable sets, characters, and campaigns (in that order) and deletes the first match.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-c\t\tDeletes character matching %svalue%s",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-m\t\tDeletes campaign matching %svalue%s",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\t\tPrints this help text",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	bool onlyChkC = !strcmp(argv[1],"-c");
	bool onlyChkM = !strcmp(argv[1],"-m");

	std::string value = std::string(argv[argc]);

	if(!onlyChkC && !onlyChkM && looksLikeVariable(value))
	{
		struct RemovedVariable
		{
			char type = '\0';
			std::string name = "";
		};

		VariableInfo vi = parseVariable(value);
		RemovedVariable rv;

		if(vi.property != "")
		{
			output(Error,"Properties cannot be deleted.");
			exit(-1);
		}

		switch(vi.evalType)
		{
			case VAR_SIGIL:
				if(vi.scope.remove<Var>(vi.key))
					rv = {VAR_SIGIL,value};
				break;
			case DICE_SIGIL:
				if(vi.scope.remove<Dice>(vi.key))
					rv = {DICE_SIGIL,value};
				break;
			case WALLET_SIGIL:
				if(vi.scope.remove<Wallet>(vi.key))
					rv = {WALLET_SIGIL,value};
				break;
			case CURRENCY_SIGIL:
				if(vi.scope.remove<Currency>(vi.key))
					rv = {CURRENCY_SIGIL,value};
				break;
		}

		vi.scope.save();
		if(rv.type)	output(Warning,"Variable %s has been deleted.",vi.key.c_str());
		else		output(Error,"Variable %s does not exist to be deleted.",vi.key.c_str());
	}
	else if(!onlyChkC && !onlyChkM && looksLikeSet(value))
	{
		struct RemovedKey
		{
			bool isRemoved = false;
			char type = 0;
		};
		VariableInfo vi = parseVariable(value);
		for(const auto& [k,v] : getSet(getSetStr(vi)))
		{
			RemovedKey rk;

			switch(v[0])
			{
				case VAR_SIGIL:
					if(vi.type == '/' || vi.type == VAR_SIGIL)
						rk = {vi.scope.remove<Var>(k),VAR_SIGIL};
					break;
				case DICE_SIGIL:
					if(vi.type == '/' || vi.type == DICE_SIGIL)
						rk = {vi.scope.remove<Dice>(k),DICE_SIGIL};
					break;
				case WALLET_SIGIL:
					if(vi.type == '/' || vi.type == WALLET_SIGIL)
						rk = {vi.scope.remove<Wallet>(k),WALLET_SIGIL};
					break;
				case CURRENCY_SIGIL:
					if(vi.type == '/' || vi.type == CURRENCY_SIGIL)
						rk = {vi.scope.remove<Currency>(k),CURRENCY_SIGIL};
					break;
			}

			if(rk.isRemoved)
				output(Warning,"Removed \"%c%c/%s\"",vi.scope.sigil,rk.type,k.c_str());
		}

		vi.scope.save();
	}
	else if(onlyChkC || !onlyChkM)
	{
		Character c = Character();
		std::string campaign_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/'));
		std::string path = campaign_path + value;
		if(std::filesystem::exists(path))
		{
			std::filesystem::remove(path);
			output(Warning,"Character file \"%s\" has been deleted.",path.c_str());
		}
		else
		{
			output(Error,"Character file \"%s\" does not exist to be deleted.",path.c_str());
			exit(-1);
		}
	}
	else
	{
		std::string path = root_dir + value;
		if(std::filesystem::exists(path))
		{
			std::filesystem::remove_all(path);
			output(Warning,"Campaign directory \"%s\" has been deleted.",path.c_str());
		}
		else
		{
			output(Error,"Campaign directory \"%s\" does not exist to be deleted.",path.c_str());
			exit(-1);
		}
	}

	return 0;
}
