#include <regex>

int main()
{
	std::string s = "{ USD,Dime,2,Nickel,Quarter }";
	std::regex pattern("([a-zA-Z0-9]{1,}),");
	std::smatch matches;

	std::regex_search(s,matches,pattern);

	fprintf(stdout,"Matches: %d\n",matches.size());
	for(int i=0; i<matches.size(); i++)
		fprintf(stdout,"Match: \"%s\"\n",matches.str(1).c_str());
	return 0;
}
