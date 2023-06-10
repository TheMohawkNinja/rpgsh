#include <vector>
#include "classes.h"

int main(int argc, char** argv)
{
	DNDSH_CHAR c = DNDSH_CHAR();
	c.load();

	std::vector<std::string> vars;

	fprintf(stdout,"argc=%d\n",argc);
	for(int i=0; i<argc; i++)
	{
		fprintf(stdout,"argv[%d]=%s\n",i,argv[i]);
		if(std::string(argv[i]).substr(0,1) == "%")
		{
			vars.push_back(std::string(argv[i]).substr(1,sizeof(argv[i])-1));
		}
	}

	if(argc == 3)
	{
		fprintf(stdout,"%s\n",std::string(c.Attr[vars[0]]).c_str());
	}
	else
	{
		fprintf(stdout,"vars[0]=%s\n",vars[0].c_str());
		if(!strcmp(argv[3],"="))
		{
			fprintf(stdout,"vars[0]=%s\n",vars[0].c_str());
			c.Attr[vars[0]] = argv[4];
			fprintf(stdout,"%s\n",std::string(c.Attr[vars[0]]).c_str());
		}
	}

	c.set_current();
	c.save();
	return 0;
}
