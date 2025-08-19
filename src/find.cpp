#include <string>
#include <vector>
#include <regex>
#include "../headers/functions.h"

const std::string replace = std::string(TEXT_RED)+"$&"+std::string(TEXT_NORMAL);
enum Option
{
	all,
	campaigns,
	characters,
	keys,
	keys_and_values
};

template <typename T>
void printMatches(datamap<T> map, std::regex pattern, char sigil, Option opt)
{
	for(const auto& [k,v] : map)
	{
		std::string k_out = k;
		std::string v_out = std::string(v);

		k_out = std::regex_replace(k_out,pattern,replace);
		if(opt == all || opt == keys_and_values)
			v_out = std::regex_replace(v_out,pattern,replace);

		if(opt == keys && k_out != k)
			fprintf(stdout,"%c/%s\n",sigil,k_out.c_str());
		else if((opt == all || opt == keys_and_values) && (k_out != k || v_out != std::string(v)))
			fprintf(stdout,"%c/%s\n\t%s\n",sigil,k_out.c_str(),v_out.c_str());
	}
}
void printScopeMatches(Scope scope, std::regex pattern, Option opt)
{
	printMatches(scope.getDatamap<Var>(),pattern,scope.sigil,opt);
	printMatches(scope.getDatamap<Dice>(),pattern,scope.sigil,opt);
	printMatches(scope.getDatamap<Currency>(),pattern,scope.sigil,opt);
	printMatches(scope.getDatamap<Wallet>(),pattern,scope.sigil,opt);
}
int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(error,"No pattern or options specified.");
		exit(-1);
	}
	if(argc > 3)
		output(warning,"find only expects 1 or 2 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Searches campaigns, characters, keys, and values for matches to a pattern, case-insensitive.");
	chkFlagPreserveVariables(argv,true);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tfind [[%sOPTIONS%s]|%spattern%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%spattern%s\t\tPrints all rpgsh campaigns, characters, keys, and values that match %spattern%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-m %spattern%s\tPrints all rpgsh campaigns that match %spattern%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-c %spattern%s\tPrints all rpgsh characters that match %spattern%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-k %spattern%s\tPrints all rpgsh keys that match %spattern%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-v %spattern%s\tPrints all rpgsh keys and values that match %spattern%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);

		return 0;
	}

	std::string p;
	Option opt;
	if(argc == 2)
	{
		p = std::string(argv[1]);
		opt = all;
	}
	else if(argv[1][0] == '-')
	{
		if(!argv[1][1])
		{
			output(error,"Unknown option \"%s\".",argv[1]);
			exit(-1);
		}
		p = std::string(argv[2]);
		switch(argv[1][1])
		{
			case('m'):
				opt = campaigns;
				break;
			case('c'):
				opt = characters;
				break;
			case('k'):
				opt = keys;
				break;
			case('v'):
				opt = keys_and_values;
				break;
			default:
				output(error,"Unknown option \"%s\".",argv[1]);
				exit(-1);
		}
	}

	std::regex pattern;
	try
	{
		pattern = std::regex(p,std::regex_constants::icase);
	}
	catch(...)
	{
		output(error,"\"%s\" is not a valid regular expression.",p.c_str());
		exit(-1);
	}

	Character c = Character();
	Campaign m = Campaign();
	Shell s = Shell();

	if(opt == all || opt == campaigns)
	{
		printHeader("campaigns");
		std::vector<std::string> campaigns = getDirectoryListing(campaigns_dir);
		for(const auto& campaign : campaigns)
		{
			if(!std::filesystem::is_directory(campaigns_dir+campaign)) continue;

			std::string m_out = campaign;
			m_out = std::regex_replace(campaign,pattern,replace);

			if(m_out != campaign)
				fprintf(stdout,"%s\n",m_out.c_str());
		}
		fprintf(stdout,"\n");
	}

	if(opt == all || opt == characters)
	{
		printHeader("characters");
		std::string m_dir = left(m.getDatasource(),rfindu(m.getDatasource(),'/'));
		std::vector<std::string> characters = getDirectoryListing(m_dir+"/characters");
		for(const auto& character : characters)
		{
			if(right(character,character.length()-c_ext.length()) != c_ext) continue;

			std::string c_out = character;
			c_out = std::regex_replace(character,pattern,replace);

			if(c_out != character)
				fprintf(stdout,"%s\n",left(c_out,findu(c_out,'.')).c_str());
		}
		fprintf(stdout,"\n");
	}

	if(opt == all || opt == keys || opt == keys_and_values)
	{
		printHeader("Variables");
		printScopeMatches(s,pattern,opt);
		printScopeMatches(m,pattern,opt);
		printScopeMatches(c,pattern,opt);
	}

	return 0;
}
