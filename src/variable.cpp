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

template<typename T>
void appendMap(Scope scope, std::map<std::string,std::string>* p_map)
{
	for(const auto& [k,v] : scope.getDatamap<T>())
		(*p_map)[k] = scope.getStr<T>(k);
}
void appendOutput(std::map<std::string,std::string> map, std::string key, std::string* pOutput)
{
	for(const auto& [k,v] : map)
	{
		// Get root variable if it exists
		if(!stringcasecmp((left(k,key.length())+"/"),key))
			pOutput->append(k + DS + v + DS);

		// Get all subkeys
		if(!stringcasecmp((left(k,key.length())),key))
			pOutput->append(k + DS + v + DS);
	}
}

std::map<std::string,std::string> getSet(std::string set_str)
{
	std::map<std::string,std::string> set;
	std::string key, value;

	while(findu(set_str,DS) != std::string::npos)
	{
		key = left(set_str,findu(set_str,DS));
		set_str = right(set_str,findu(set_str,key)+key.length()+DS.length());

		if(findu(set_str,DS) != std::string::npos)
		{
			value = left(set_str,findu(set_str,DS));
			set_str = right(set_str,findu(set_str,value)+value.length()+DS.length());
		}
		else
		{
			value = set_str;
		}
		set[key] = value;
	}

	return set;
}
std::string getSetStr(VariableInfo vi)
{
	std::map<std::string,std::string> c_map;
	std::map<std::string,std::string> d_map;
	std::map<std::string,std::string> v_map;
	std::map<std::string,std::string> w_map;

	// When printing entire containers, treat type sigil as a filter
	switch(vi.variable[1])
	{
		case CURRENCY_SIGIL:
			appendMap<Currency>(vi.scope,&c_map);
			break;
		case DICE_SIGIL:
			appendMap<Dice>(vi.scope,&d_map);
			break;
		case VAR_SIGIL:
			appendMap<Var>(vi.scope,&v_map);
			break;
		case WALLET_SIGIL:
			appendMap<Wallet>(vi.scope,&w_map);
			break;
		case '/':
			appendMap<Currency>(vi.scope,&c_map);
			appendMap<Dice>(vi.scope,&d_map);
			appendMap<Var>(vi.scope,&v_map);
			appendMap<Wallet>(vi.scope,&w_map);
			break;
		default:
			output(Error,"Unknown type specifier \'%c\' in \"%s\"",vi.variable[1],vi.variable.c_str());
			exit(-1);
	}

	// Create output string from map
	std::string output = "";
	appendOutput(c_map,vi.key,&output);
	appendOutput(d_map,vi.key,&output);
	appendOutput(v_map,vi.key,&output);
	appendOutput(w_map,vi.key,&output);

	// Cut off the extraneous DS
	return left(output,output.length()-DS.length());
}
