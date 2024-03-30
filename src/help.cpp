#include <algorithm>
#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argv[1] && !strcmp(argv[1],FLAG_APPDESCRIPTION))
	{
		fprintf(stdout,"Prints this helpful list of applications and what they do.\n");
		return 0;
	}

	int longestNameLength = 0;
	std::string appname;
	std::vector<std::string> applications = getDirectoryListing("/bin");
	std::vector<std::string> appdescription;
	for(const auto& app : applications)
	{
		if(app.substr(0,prefix.length()) == prefix)
		{
			appname = app.substr(prefix.length(),
						app.length()-prefix.length());
			if(appname.length() > longestNameLength)
				longestNameLength = appname.length();
		}
	}
	for(const auto& app : applications)
	{
		if(app.substr(0,prefix.length()) == prefix)
		{
			appname = app.substr(prefix.length(),
						app.length()-prefix.length());
			appdescription = get_prog_output(appname+" "+FLAG_APPDESCRIPTION);

			//Not sure why %*s doesn't work here
			fprintf(stdout,"%s%s%s",TEXT_WHITE,appname.c_str(),TEXT_NORMAL);
			for(int i=0; i<(longestNameLength-appname.length()+COLUMN_PADDING); i++)
				fprintf(stdout," ");
			fprintf(stdout,"%s\n",appdescription[0].c_str());
		}
	}
	return 0;
}
