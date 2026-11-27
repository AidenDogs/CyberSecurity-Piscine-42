#include "include/common.h"
#include "include/print.h"

void start(char **argv)
{
	//char **toProtect;
	(void)argv;

}

static void checkArgError(int argc)
{
	if (argc < 2)
	{
		printError("Invalid number of argument\n");
		//displayUsage();
		exit(1);
	}

	struct utsname sysInfo;
	if (uname(&sysInfo) == -1)
	{
		printError("Failed to retrieve system information\n");
		exit(1);
	}

	if (strcmp(sysInfo.sysname, "Linux") != 0)
	{
		printError("This program must be run on Linux platform\n");
		exit(1);
	}

	if (geteuid() != 0)
	{
		printError("The program must be run as root\n");
		exit(1);
	}
}

int main (int argc, char **argv)
{
	checkArgError(argc);
	start(argv);
}