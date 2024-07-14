#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	check_print_app_description(argv,"Prints this helpful list of applications and what they do.");

	//Generate list of apps from cache file
	std::vector<std::string> applications;
	std::ifstream ifs(rpgsh_programs_cache_path);
	std::string line = "";
	while(!ifs.eof())
	{
		getline(ifs,line);
		applications.push_back(line);
	}
	ifs.close();

	//Determine longest name for column formatting
	int longestNameLength = 0;
	std::string appname;
	std::vector<std::string> appdescription;
	for(int i=0; i<applications.size(); i++)
	{
		std::string app = applications[i];
		if(app.substr(0,prefix.length()) == prefix)
		{
			appname = right(app,prefix.length());
			if(appname.length() > longestNameLength)
				longestNameLength = appname.length();
		}
		else
		{
			applications.erase(applications.begin()+i);
			i--;
		}
	}

	//Print results
	for(const auto& app : applications)
	{
		appname = right(app,prefix.length());
		appdescription = get_prog_output(appname+" "+FLAG_APPDESCRIPTION);

		//Not sure why %*s doesn't work here
		fprintf(stdout,"%s%s%s%s%s",TEXT_BOLD,TEXT_GREEN,appname.c_str(),TEXT_NORMAL,addSpaces(longestNameLength-appname.length()+COLUMN_PADDING).c_str());

		//Description should really be just one line, so just print the first line
		if(appdescription[0] != "")
			fprintf(stdout,"%s\n",appdescription[0].c_str());
		else
			fprintf(stdout,"%s\n",empty_str.c_str());
	}
	return 0;
}
