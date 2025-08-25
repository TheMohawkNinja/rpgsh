#include <cstring>
#include <strings.h>
#include "../headers/config.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	chkFlagAppDesc(argv,"Prints rpgsh history.");
	chkFlagPreserveVariables(argv,none);

	Config cfg = Config();
	long rpgsh_history_length;
	try
	{
		rpgsh_history_length = std::stol(cfg.setting[HISTORY_LENGTH]);
	}
	catch(...)
	{
		output(error,"Invalid value for \"%s\" setting: %s",HISTORY_LENGTH,cfg.setting[HISTORY_LENGTH].c_str());
		return -1;
	}

	long unsigned value;
	if(argc == 1)
	{
		value = rpgsh_history_length;
	}
	else if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\thistory [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints all history (max length defined by the \"%s\" setting.\n",TEXT_ITALIC,TEXT_NORMAL,HISTORY_LENGTH);
		fprintf(stdout,"\t-c\t\tPrints the current number of lines in the rpgsh history file.\n");
		fprintf(stdout,"\t%sn%s\t\tPrints the %sn%sth line of rpgsh history.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-f\t\tPrints the first line of rpgsh history.\n");
		fprintf(stdout,"\t-f %sn%s\t\tPrints the first %sn%s lines of rpgsh history.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-l\t\tPrints the last line of rpgsh history.\n");
		fprintf(stdout,"\t-l %sn%s\t\tPrints the last %sn%s lines of rpgsh history.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrints this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
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
			output(error,"Invalid value for line number: %s",argv[1]);
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
			output(error,"Invalid value for line count: %s",argv[1]);
			return -1;
		}
	}
	else if(strcmp(argv[1],"-c"))
	{
		output(error,"Unknown arg \"%s\".",argv[1]);
		return -1;
	}

	confirmHistoryFile();
	long unsigned line_count = 0;
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
	for(long unsigned i=0; i<line_count; i++)
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
