#include <cstring>
#include <strings.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	check_print_app_description(argv,"Prints rpgsh history.");

	Config config = Config();
	long rpgsh_history_length;
	try
	{
		rpgsh_history_length = std::stol(config.setting[HISTORY_LENGTH]);
	}
	catch(...)
	{
		output(Error,"Invalid value for \"%s\" setting: %s",HISTORY_LENGTH,config.setting[HISTORY_LENGTH].c_str());
		return -1;
	}

	unsigned long value;
	if(argc == 1)
	{
		value = rpgsh_history_length;
	}
	else if(!strcasecmp(argv[1],"-?") || !strcasecmp(argv[1],"--help"))
	{
		fprintf(stdout,"Prints rpgsh history.\n\n");
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\thistory\t\t\tPrints all history (max length defined by the \"%s\" setting.\n",HISTORY_LENGTH);
		fprintf(stdout,"\thistory -c\t\tPrints the current number of lines in the rpgsh history file.\n");
		fprintf(stdout,"\thistory %sn%s\t\tPrints the %sn%sth line of rpgsh history.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\thistory -f\t\tPrints the first line of rpgsh history.\n");
		fprintf(stdout,"\thistory -f %sn%s\t\tPrints the first %sn%s lines of rpgsh history.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\thistory -l\t\tPrints the last line of rpgsh history.\n");
		fprintf(stdout,"\thistory -l %sn%s\t\tPrints the last %sn%s lines of rpgsh history.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\thistory [-?|--help]\tPrints this help text.\n");
		return 0;
	}
	else if(argc == 2 && (!strcmp(argv[1],"-f") || !strcmp(argv[1],"-l")))
	{
		value = 1;
	}
	else if(argc == 2 && argv[1][0] != '-')
	{
		try
		{
			value = std::stol(argv[1]);
		}
		catch(...)
		{
			output(Error,"Invalid value for line number: %s",argv[1]);
			return -1;
		}
	}
	else if(!strcmp(argv[1],"-f") || !strcmp(argv[1],"-l"))
	{
		try
		{
			value = std::stol(argv[2]);
		}
		catch(...)
		{
			output(Error,"Invalid value for line count: %s",argv[1]);
			return -1;
		}
	}
	else if(strcmp(argv[1],"-c"))
	{
		output(Error,"Unknown arg \"%s\".",argv[1]);
		return -1;
	}

	confirmHistoryFile();
	unsigned long line_count = 0;
	std::ifstream ifs(history_path.c_str());
	std::string data;
	std::vector<std::string> history;
	while(!ifs.eof())
	{
		std::getline(ifs,data);
		if(ifs.eof()) break;
		history.push_back(data);
		line_count++;
	}
	ifs.close();
	if(value > history.size()) value = history.size();
	if(argc > 1 && !strcmp(argv[1],"-c"))
	{
		fprintf(stdout,"%ld\n",line_count);
		return 0;
	}
	for(unsigned long i=0; i<line_count; i++)
	{
		if(argc == 1)
		{
			fprintf(stdout,"%s\n",history[i].c_str());
		}
		else if(argv[1][0] != '-' && i == value-1)
		{
			fprintf(stdout,"%s\n",history[i].c_str());
			break;
		}
		else if(!strcmp(argv[1],"-f") && i < value)
		{
			fprintf(stdout,"%s\n",history[i].c_str());
		}
		else if(!strcmp(argv[1],"-f") && i == value)
		{
			break;
		}
		else if(!strcmp(argv[1],"-l") && i >= line_count-value)
		{
			fprintf(stdout,"%s\n",history[i].c_str());
			if(argc == 2) break;
		}
	}
	return 0;
}
