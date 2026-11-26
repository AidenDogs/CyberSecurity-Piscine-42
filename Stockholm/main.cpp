#include "include/disease.hpp"

static void parseArgs(int argc, char **argv)
{
	bool 		isSilenced = false;
	std::string	key = "";

	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-h" || arg == "-help")
			displayUsage();
		if (arg == "-v" || arg == "-version")
			displayVersion();
		if (arg == "-s" || arg == "-silent")
			isSilenced = true;
		if (arg == "-r" || arg == "-reverse")
		{
			if (!argv[i + 1])
			{
				std::cerr << "-r flag requires a key after it" << std::endl;
				exit(1);
			}

			key = argv[i + 1];
		}
	}
	if (!key.empty()) 
		cureThreads(key, isSilenced);
	else
		infectThreads(isSilenced);
}

int main (int argc, char **argv)
{
	parseArgs(argc, argv);
	return (0);
}