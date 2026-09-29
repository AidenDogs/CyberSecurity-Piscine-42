#include "include/hotp.hpp"
#include "include/seed.hpp"

static void	exeProgram(char **argv)
{
	if (strcmp(argv[1], "-g") != 0 && strcmp(argv[1], "-k") != 0 && strcmp(argv[1], "-s") != 0)
	{
		std::cerr << "Invalid flag\nAborting" << std::endl;
		exit(1);
	}
	std::string flag = argv[1];
	if (flag == "-g")
	{
		std::ifstream inFile;

		inFile.open(argv[2]);
		if (!inFile)
		{
			std::cerr << "Unable to open file => " << argv[2] << std::endl;
			exit(1);
		}
		storeKey(inFile);
		inFile.close();
	}
	else if (flag == "-k")
		generateOTP(argv[2]);
	else
		generateSeed();
}

int	main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Invalid number of arguments" << std::endl;
		return (1);
	}
	exeProgram(argv);
	return (0);
}
