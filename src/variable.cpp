#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/string.h"
#include "../headers/variable.h"

void loadXRef(std::string* arg, VariableInfo* p_vi)
{
	// Ending square bracket not found
	if(findu(*arg,']') == std::string::npos)
	{
		output(Error,"No terminating \']\' found for xref.");
		exit(-1);
	}

	// Get string between the square brackets
	p_vi->xref = arg->substr(findu(*arg,'[')+1,findu(*arg,']')-(findu(*arg,'[')+1));

	// Actually load the xref into the scope
	std::vector<std::string> campaigns;
	std::string xref_dir = campaigns_dir+
			       getEnvVariable(ENV_CURRENT_CAMPAIGN)+
			       "characters/";
	std::string xref_char = p_vi->xref;
	std::string campaign = "";
	switch((*arg)[0])
	{
		case CHARACTER_SIGIL:
			if(findu(p_vi->xref,'/') != std::string::npos)// Attempt to load character from another campaign
			{
				campaign = left(p_vi->xref,findu(p_vi->xref,'/'));
				xref_char = right(p_vi->xref,findu(p_vi->xref,'/')+1);
				xref_dir = campaigns_dir+
					   getLikeFileName(campaign,campaigns_dir,true,p_vi->xref)+
					   "/characters/";
			}

			p_vi->scope.setDatasource(xref_dir+getLikeFileName(xref_char+c_ext,xref_dir,false,p_vi->xref));
			p_vi->scope.load();
			break;
		case CAMPAIGN_SIGIL:
			p_vi->scope.setDatasource(campaigns_dir+
						  getLikeFileName(p_vi->xref,campaigns_dir,true,p_vi->xref)+
						  "/"+
						  variables_file_name);
			p_vi->scope.load();
			break;
		case SHELL_SIGIL:
			output(Error,"Cannot use xref with shell scope.");
			exit(-1);
	}

	// Remove external reference string so we can continue to use the current arg under the new context
	*arg = (*arg)[0]+left(*arg,findu(*arg,'[')-1)+right(*arg,findu(*arg,']')+1);
}
VariableInfo parseVariable(std::string v)
{
	VariableInfo vi;

	// Check scope sigil
	switch(v[0])
	{
		case CHARACTER_SIGIL:
			vi.scope = Character();
			break;
		case CAMPAIGN_SIGIL:
			vi.scope = Campaign();
			break;
		case SHELL_SIGIL:
			vi.scope = Shell();
			break;
		default:
			output(Error,"Unknown scope sigil \'%c\'.",v[0]);
			exit(-1);
	}

	// Check for external references
	if(v[1] == '[') loadXRef(&v,&vi);

	// Check type sigil
	if(!isTypeSigil(v[1]) && v[1] != '/')
	{
		output(Error,"Unknown type sigil \'%c\'.",v[1]);
		exit(-1);
	}

	vi.type = v[1];
	vi.variable = v;
	vi.key = right(v,findu(v,'/')+1);
	if((v[rfindu(v,'/')+1] == '.' && rfindu(v,'.') > rfindu(v,'/')+1) ||
	   (findu(v,'.') > rfindu(v,'/')+1 && rfindu(v,'.') < UINT_MAX))
	{
		vi.key = left(vi.key,rfindu(vi.key,'.'));
		vi.property = right(v,rfindu(v,'.')+1);
	}

	if(isTypeSigil(vi.type))
	{
		vi.evalType = vi.type;
	}
	else
	{
		if(vi.scope.keyExists<Var>(vi.key))
			vi.evalType = VAR_SIGIL;
		else if(vi.scope.keyExists<Dice>(vi.key))
			vi.evalType = DICE_SIGIL;
		else if(vi.scope.keyExists<Wallet>(vi.key))
			vi.evalType = WALLET_SIGIL;
		else if(vi.scope.keyExists<Currency>(vi.key))
			vi.evalType = CURRENCY_SIGIL;
		else
			vi.evalType = VAR_SIGIL;
	}

	return vi;
}
