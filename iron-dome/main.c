#include "include/common.h"
#include "include/print.h"
#include "include/install.h"
#include <signal.h>
#include <stdbool.h>

static volatile sig_atomic_t running = 1;

void handle_signal(int sig)
{
	(void)sig;
	running = 0;
}

void start(char **argv)
{
	//char **toProtect;
	(void)argv;
	signal(SIGTERM, handle_signal);
	signal(SIGINT, handle_signal);

	while (running)
	{
		printf("Daemon is running...\n");
		sleep(5);
	}

	printf("Daemon stopping...\n");
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
	if (istalling(argv) == 0)
		return (0);
	start(argv);
	return (0);
}