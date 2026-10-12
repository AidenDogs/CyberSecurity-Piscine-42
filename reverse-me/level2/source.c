#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void no(void)
{
	printf("Nope.\n");
}

void ok(void)
{
	printf("Good job.\n");
}

int main(void)
{
	char input[23];
	char result[9];

	int scan_result;
	int i = 2;
	int j = 1;

	printf("Please enter key: ");

	scan_result = scanf("%23s", input);

	if (scan_result != 1)
		no();

	if (input[1] != '0')
		no();

	if (input[0] != '0')
		no();

	fflush(stdout);

	memset(result, 0, 9);
	result[0] = 'd';

	while (strlen(result) < 8 && i < strlen(input)) {
		char digits[4];

		digits[0] = input[i];
		digits[1] = input[i + 1];
		digits[2] = input[i + 2];
		digits[3] = '\0';

		result[j] = (char)atoi(digits);

		i += 3;
		j++;
	}

	result[j] = '\0';

	if (strcmp(result, "delabere") == 0)
		ok();
	else
		no();

	return 0;
}