#include <unistd.h>
#include <string.h>
#include <stdio.h>

void printError(char *error)
{
	write(2, &error, strlen(error));
}

void writeLog(char *string)
{
	const char *filename = "/var/log/irondome/irondome.log";

	FILE *fp = fopen(filename, "w");

	if (fp == NULL)
	{
		perror("Error creating file");
		exit(1);
	}

	fprintf(fp, string);
	fclose(fp);
}

int main (int argc, char **argv)
{
	if (argc < 2)
	{
		printError("Invalid number of argument\n");
		//displayUsage();
		return (1);
	}

	if (geteuid() != 0)
	{
		printError("The program must be run as root\n");
		return (1);
	}
}