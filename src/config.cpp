#include <fstream>
#include <cstring>
#include <string>
#include "../headers/configuration.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int main(int argc, char** argv)
{
	if(argc > 3)
		output(warning,"config only expects 0, 1, or 2 arguments, ignoring all other arguments.");

	chkFlagAppDesc(argv,"Gets and sets configuration settings.");
	chkFlagPreserveVariables(argv,none);

	if(chkFlagHelp(argv))
	{
		fprintf(stdout,"USAGE:\n");
		fprintf(stdout,"\tconfig [%sOPTIONS%s]\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\nOPTIONS:\n");
		fprintf(stdout,"\t%snone%s\t\tPrints current configuration settings and their values.\n",TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%ssetting%s\t\tPrints the current value of the configuration setting %ssetting%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%ssetting%s=%svalue%s\tSets the configuration setting %ssetting%s to the value %svalue%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t-d %ssetting%s\tDeletes the configuration setting %ssetting%s.\n",TEXT_ITALIC,TEXT_NORMAL,TEXT_ITALIC,TEXT_NORMAL);
		fprintf(stdout,"\t%s | %s\tPrint this help text.\n",FLAG_HELPSHORT,FLAG_HELPLONG);
		return 0;
	}

	Configuration cfg = Configuration();

	if(argc == 1)
	{
		unsigned longest_k = 0;
		for(const auto& [k,v] : cfg.setting)
			if(k.length() > longest_k) longest_k = k.length();

		for(const auto& [k,v] : cfg.setting)
			fprintf(stdout,"%s%s%s%s%s%s\n",TEXT_BOLD,TEXT_GREEN,k.c_str(),TEXT_NORMAL,addSpaces(longest_k-k.length()+COLUMN_PADDING).c_str(),(v != "" ? v : empty_str).c_str());
	}
	else if(argc == 2 && findu(std::string(argv[1]),'=') == std::string::npos)
	{
		for(const auto& [k,v] : cfg.setting)
		{
			if(std::string(argv[1]) != k) continue;
			fprintf(stdout,"%s\n",v.c_str());

			return 0;
		}

		output(error,"\"%s\" is not a configured setting.",argv[1]);
		return -1;
	}
	else if(argc == 2)
	{
		bool setSetting = false;
		std::string k = left(std::string(argv[1]),findu(std::string(argv[1]),'='));
		std::string v = right(std::string(argv[1]),findu(std::string(argv[1]),'=')+1);
		cfg.setting[k] = v;

		std::vector<std::string> data;
		std::ifstream ifs(configuration_path.c_str());
		while(!ifs.eof())
		{
			std::string line;
			std::getline(ifs,line);
			if(ifs.eof() && line == "") break;
			if(findu(line,'=') != std::string::npos && left(line,findu(line,'=')) == k)
			{
				line = k+"="+v;
				setSetting = true;
			}
			data.push_back(line);
		}
		ifs.close();

		if(!setSetting) data.push_back(k+"="+v);

		std::ofstream ofs(configuration_path.c_str());
		for(const auto& line : data) ofs<<line<<"\n";
		ofs.close();

		output(info,"Set configuration setting \"%s\" to \"%s\".",k.c_str(),cfg.setting[k].c_str());
	}
	else if(argc == 3 && !strcmp(argv[1],"-d"))
	{
		bool deletedSetting = false;
		std::string k = std::string(argv[2]);
		std::vector<std::string> data;
		std::ifstream ifs(configuration_path.c_str());
		while(!ifs.eof())
		{
			std::string line;
			std::getline(ifs,line);
			if(ifs.eof() && line == "") break;
			if(findu(line,'=') == std::string::npos ||
			   (findu(line,'=') != std::string::npos && left(line,findu(line,'=')) != k))
				data.push_back(line);
			else if(findu(line,'=') != std::string::npos && left(line,findu(line,'=')) == k)
				deletedSetting = true;
		}
		ifs.close();

		if(!deletedSetting)
		{
			output(error,"Configuration setting \"%s\" does not exist to be deleted.",argv[2]);
			return -1;
		}

		std::ofstream ofs(configuration_path.c_str());
		for(const auto& line : data) ofs<<line<<"\n";
		ofs.close();

		output(info,"Deleted configuration setting \"%s\".",k.c_str());
	}
	else
	{
		output(error,"Unknown option \"%s\"",argv[1]);
		return -1;
	}

	return 0;
}
