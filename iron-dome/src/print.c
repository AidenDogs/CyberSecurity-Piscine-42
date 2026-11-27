#include "../include/common.h"

void printError(char *error)
{
	write(2, error, strlen(error));
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

	fprintf(fp, "%s", string);
	fclose(fp);
}