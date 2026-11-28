#include "../include/common.h"

static int requireInstall(char **argv)
{
	int i = 0;
	int flag = 1;
	while (argv[i])
	{
		if (strcmp(argv[i], "--install") == 0)
		{
			flag = 0;
			break;
		}
		i++;
	}

	if (flag == 1)
		return (1);
	return (0);	
}

int istalling(char **argv)
{
	if (requireInstall(argv) == 1)
		return (1);
	//install code
	printf("Iron dome installed successfully!\n");
	return (0);
}