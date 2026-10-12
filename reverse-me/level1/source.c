#include <stdio.h>
#include <string.h>

int main(void)
{
	char input[16];

	printf("Please enter key: ");

	scanf("%16s", input);

	if (strcmp(input, "__stack_check") == 0)
		printf("Good job.\n");
	else
		printf("Nope.\n");

	return 0;
}