#include <string>
#include <vector>
#include <regex>
#include "../headers/functions.h"

template <typename T>
void printMatches(datamap<T> map, std::regex pattern, char sigil)
{
	for(const auto& [k,v] : map)
	{
		std::string k_out = k;
		std::string v_out = escapeRegexGroupChars(std::string(v));
		bool k_match = false;
		bool v_match = false;

		std::sregex_iterator k_it(k.begin(),k.end(),pattern);
		std::sregex_iterator k_it_end;
		while(k_it != k_it_end && (long unsigned int)k_it->position() < k.length())
		{
			std::string replace = std::string(TEXT_RED)+k_it->str()+std::string(TEXT_NORMAL);
			k_out = std::regex_replace(k_out,std::regex(k_it->str()),replace);
			k_match = true;
			k_it++;
		}

		std::sregex_iterator v_it(v_out.begin(),v_out.end(),pattern);
		std::sregex_iterator v_it_end;
		while(v_it != v_it_end && (long unsigned int)v_it->position() < v_out.length())
		{
			std::string replace = std::string(TEXT_RED)+v_it->str()+std::string(TEXT_NORMAL);
			v_out = std::regex_replace(v_out,std::regex(v_it->str()),replace);
			v_match = true;
			v_it++;
		}

		if(k_match || v_match)
			fprintf(stdout,"%c/%s\n\t%s\n",sigil,k_out.c_str(),v_out.c_str());
	}
}
void printScopeMatches(Scope scope, std::regex pattern)
{
	printMatches(scope.getDatamap<Var>(),pattern,scope.sigil);
	printMatches(scope.getDatamap<Dice>(),pattern,scope.sigil);
	printMatches(scope.getDatamap<Currency>(),pattern,scope.sigil);
	printMatches(scope.getDatamap<Wallet>(),pattern,scope.sigil);
}
int main(int argc, char** argv)
{
	if(argc == 1)
	{
		output(Error,"No pattern or options specified.");
		exit(-1);
	}
	if(argc > 2)
		output(Warning,"find only expects 1 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Searches campaigns, characters, keys, and values for matches to a pattern, case-insensitive.");
	chkFlagModifyVariables(argv,true);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tfind [[%sOPTIONS%s]|%spattern%s]\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%spattern%s\t\tPrints all rpgsh applications, campaigns, characters, and variables that match %spattern%s\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);

		return 0;
	}

	std::string p = std::string(argv[1]);
	std::regex pattern;
	try
	{
		pattern = std::regex(p,std::regex_constants::icase);
	}
	catch(...)
	{
		output(Error,"\"%s\" is not a valid regular expression.",p.c_str());
		exit(-1);
	}

	Character c = Character();
	Campaign m = Campaign();
	Shell s = Shell();

	printHeader("Campaigns");
	std::vector<std::string> campaigns = getDirectoryListing(campaigns_dir);
	for(const auto& campaign : campaigns)
	{
		std::sregex_iterator it(campaign.begin(),campaign.end(),pattern);
		std::sregex_iterator it_end;

		if(it != it_end && std::filesystem::is_directory(campaigns_dir+campaign))
			fprintf(stdout,"%s\n",campaign.c_str());
	}
	fprintf(stdout,"\n");

	printHeader("Characters");
	std::string m_dir = left(m.getDatasource(),rfindu(m.getDatasource(),'/'));
	std::vector<std::string> characters = getDirectoryListing(m_dir+"/characters");
	for(const auto& character : characters)
	{
		if(right(character,character.length()-c_ext.length()) != c_ext) continue;
		std::sregex_iterator it(character.begin(),character.end()-c_ext.length(),pattern);
		std::sregex_iterator it_end;

		if(it != it_end)
			fprintf(stdout,"%s\n",left(character,findu(character,c_ext)).c_str());
	}
	fprintf(stdout,"\n");

	printHeader("Variables");
	printScopeMatches(s,pattern);
	printScopeMatches(m,pattern);
	printScopeMatches(c,pattern);

	return 0;
}
