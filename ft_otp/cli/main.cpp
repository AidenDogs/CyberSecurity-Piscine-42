#include "include/hotp.hpp"
#include "include/seed.hpp"

static void displayUsage()
{
    std::cout << "\033[36mUsage: ft_otp <option> <argument>\033[0m\n"
        << "\n"
        << "\033[33mOptions:\033[0m\n"
        << "  \033[33m-g <file>\033[0m   Read a hexadecimal key (at least 64 characters) from <file>,\n"
        << "              encrypt it and store it in ft_otp.key\n"
        << "  \033[33m-k <key>\033[0m    Generate a new temporary one-time password from the key\n"
        << "              (e.g. ft_otp.key) and print it on the standard output\n"
        << "  \033[33m-s\033[0m          Generate a seed (extra)\n"
        << "  \033[33m-h, --help\033[0m  Display this help and exit\n"
        << "\n"
        << "\033[36mExamples:\033[0m\n"
        << "ft_otp -g key.hex\n"
        << "ft_otp -k ft_otp.key\n";
}

static void	exeProgram(char **argv)
{
	if (strcmp(argv[1], "-g") != 0 && strcmp(argv[1], "-k") != 0 && strcmp(argv[1], "-s") != 0)
	{
		displayUsage();
		exit(1);
	}
	std::string flag = argv[1];
	if (flag == "-g")
	{
		if (!argv[2])
		{
			std::cerr << "-g requires a file" << std::endl;
			exit(1);
		}
		
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
	{
		if (!argv[2])
		{
			std::cerr << "-k requires a key" << std::endl;
			exit(1);
		}		
		generateOTP(argv[2]);
	}
	else if (flag == "-s")
		generateSeed();
	else if (flag == "-h" || flag == "--help")
	{
		displayUsage();
		exit(0);
	}
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
