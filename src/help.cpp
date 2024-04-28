#include <cstring>
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	check_print_app_description(argv,"Prints this helpful list of applications and what they do.");

	int longestNameLength = 0;
	std::string appname;
	std::vector<std::string> applications = getDirectoryListing("/bin");
	std::vector<std::string> appdescription;
	for(int i=0; i<applications.size(); i++)
	{
		std::string app = applications[i];
		if(app.substr(0,prefix.length()) == prefix)
		{
			appname = app.substr(prefix.length(),
						app.length()-prefix.length());
			if(appname.length() > longestNameLength)
				longestNameLength = appname.length();
		}
		else
		{
			applications.erase(applications.begin()+i);
			i--;
		}
	}
	for(const auto& app : applications)
	{
		appname = app.substr(prefix.length(),
					app.length()-prefix.length());
		appdescription = get_prog_output(appname+" "+FLAG_APPDESCRIPTION);

		//Not sure why %*s doesn't work here
		fprintf(stdout,"%s%s%s%s",TEXT_BOLD,TEXT_GREEN,appname.c_str(),TEXT_NORMAL);
		for(int i=0; i<(longestNameLength-appname.length()+COLUMN_PADDING); i++)
			fprintf(stdout," ");
		//Description should really be just one line, so assume as much
		if(appdescription[0] != "")
			fprintf(stdout,"%s\n",appdescription[0].c_str());
		else
			fprintf(stdout,"%s%s%s%s<NO_DESCRIPTION>%s\n",TEXT_BOLD,TEXT_ITALIC,TEXT_BG_YELLOW,TEXT_BLACK,TEXT_NORMAL);
	}
	return 0;
}
