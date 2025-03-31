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
		fprintf(stdout,"\t-c\t\tDeletes character matching %svalue%s\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-m\t\tDeletes campaign matching %svalue%s\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	bool onlyChkC = !strcmp(argv[1],"-c");
	bool onlyChkM = !strcmp(argv[1],"-m");

	if((onlyChkC || onlyChkM) && argc == 2)
	{
		output(Error,"Missing argument for \"%s\".",argv[1]);
		return -1;
	}
	else if(argv[1][0] == '-' && !onlyChkC && !onlyChkM)
	{
		output(Error,"Unknown option \"%s\".",argv[1]);
		return -1;
	}

	VariableInfo vi;
	std::string obj_to_be_deleted = std::string(argv[1]);
	if(looksLikeVariable(obj_to_be_deleted)) vi = parseVariable(obj_to_be_deleted);

	if(!onlyChkC && !onlyChkM && looksLikeVariable(vi.variable) && vi.variable.back() != '/')
	{
		std::string value = getAppOutput(vi.variable).output[0];
		bool deleted = false;

		if(vi.property != "")
		{
			output(Error,"Properties cannot be deleted.");
			return -1;
		}

		switch(vi.evalType)
		{
			case VAR_SIGIL:
				deleted = vi.scope.remove<Var>(vi.key);
				break;
			case DICE_SIGIL:
				deleted = vi.scope.remove<Dice>(vi.key);
				break;
			case WALLET_SIGIL:
				deleted = vi.scope.remove<Wallet>(vi.key);
				break;
			case CURRENCY_SIGIL:
				deleted = vi.scope.remove<Currency>(vi.key);
				break;
		}

		vi.scope.save();
		std::string xref = (vi.xref != "" ? "["+vi.xref+"]" : "");
		if(deleted)	output(Warning,"Variable \"%c%s%c/%s\" (value: \"%s\") has been deleted.",vi.scope.sigil,xref.c_str(),vi.evalType,vi.key.c_str(),value.c_str());
		else		output(Error,"Variable \"%c%s%c/%s\" does not exist to be deleted.",vi.scope.sigil,xref.c_str(),vi.evalType,vi.key.c_str());
	}
	else if(!onlyChkC && !onlyChkM && vi.variable.back() == '/')
	{
		bool atLeastOneKeyRemoved = false;
		struct RemovedKey
		{
			bool isRemoved = false;
			char type = 0;
		};
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

			std::string xref = (vi.xref != "" ? "["+vi.xref+"]" : "");
			if(rk.isRemoved)
			{
				output(Warning,"Variable \"%c%s%c/%s\" (value: \"%s\") has been deleted.",vi.scope.sigil,xref.c_str(),rk.type,k.c_str(),v.c_str());
				atLeastOneKeyRemoved = true;
			}
		}

		vi.scope.save();
		if(!atLeastOneKeyRemoved)
			output(Error,"Variable set \"%s\" appears to be empty.",vi.variable.c_str());
	}
	else if(onlyChkC || !onlyChkM)
	{
		Character c = Character();
		std::string campaign_path;
		std::string c_to_be_deleted = std::string(argv[2]);
		if(findu(c_to_be_deleted,'/') == std::string::npos)
			campaign_path = left(c.getDatasource(),rfindu(c.getDatasource(),'/')+1);
		else
			campaign_path = campaigns_dir + right(c_to_be_deleted,findu(c_to_be_deleted,'/')+1);
		for(const auto& entry : getDirectoryListing(campaign_path))
		{
			if(stringcasecmp(entry,c_to_be_deleted+".char") || std::filesystem::is_directory(campaign_path+entry)) continue;

			std::filesystem::remove(campaign_path+entry);
			if(std::filesystem::exists(campaign_path+entry+".bak")) std::filesystem::remove(campaign_path+entry+".bak");
			output(Warning,"Character file \"%s\" has been deleted.",(campaign_path+entry).c_str());
			return 0;
		}

		output(Error,"Character file \"%s.char\" does not exist to be deleted.",(campaign_path+c_to_be_deleted).c_str());
		return -1;
	}
	else
	{
		std::string m_to_be_deleted = std::string(argv[2]);
		for(const auto& entry : getDirectoryListing(campaigns_dir))
		{
			if(stringcasecmp(entry,m_to_be_deleted) || !std::filesystem::is_directory(campaigns_dir+entry)) continue;
			std::filesystem::remove_all(campaigns_dir+entry);
			output(Warning,"Campaign directory \"%s\" has been deleted.",(campaigns_dir+entry).c_str());
			return 0;
		}

		output(Error,"Campaign directory \"%s\" does not exist to be deleted.",(campaigns_dir+m_to_be_deleted).c_str());
		return -1;
	}

	return 0;
}
