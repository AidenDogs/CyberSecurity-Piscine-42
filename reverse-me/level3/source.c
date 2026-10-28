
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ___syscall_malloc(void)
{
	fprintf(stderr, "Nope.\n");
	exit(EXIT_FAILURE);
}

void ____syscall_malloc(void)
{
	puts("Good job.");
}

int main(void)
{
	char input[64] = {0};
	char output[9] = {0};

	printf("Please enter key: ");

	if (scanf("%63s", input) != 1)
		___syscall_malloc();

	if (input[1] != '2')
		___syscall_malloc();

	if (input[0] != '4')
		___syscall_malloc();

	output[0] = '*';

	size_t input_offset = 2;
	size_t output_index = 1;

	while (strlen(output) < 8 && input_offset < strlen(input))
	{

		char digits[4] = {
			input[input_offset],
			input[input_offset + 1],
			input[input_offset + 2],
			'\0'
		};

		output[output_index] = (char)atoi(digits);

		input_offset += 3;
		output_index++;
	}

	output[output_index] = '\0';

	const char *target = "********";
	int result = strcmp(output, target);

	if (result == 0)
		____syscall_malloc();
	else
		___syscall_malloc();

	return 0;
}
