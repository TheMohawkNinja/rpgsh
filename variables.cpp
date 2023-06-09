#include <vector>
#include "classes.h"

int main(int argc, char** argv)
{
	DNDSH_CHAR c = DNDSH_CHAR();//TODO replace with load operation when implemented
	std::vector<std::string> vars;

	for(int i=0; i<argc; i++)
	{
		if(std::string(argv[i]).substr(0,1) == "%")
		{
			vars.push_back(std::string(argv[i]).substr(1,sizeof(argv[i])-1));
		}
		fprintf(stdout,"argv[%d]=%s\n",i,argv[i]);
	}

	if(argc == 3)
	{
		fprintf(stdout,"%s\n",std::string(c.Attr[vars[0]]).c_str());
	}
	else
	{
		if(argv[3] == "=")
		{
			c.Attr[vars[0]] = argv[4];
			fprintf(stdout,"%s\n",std::string(c.Attr[vars[0]]).c_str());
		}
	}

	return 0;
}
