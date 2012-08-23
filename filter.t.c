#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "filter.h"

int main(void) {

	int test = 0;

	const int test_n = 2;

	char const * input[test_n];
	char const * expected[test_n];

	input[0] = "";
	expected[0] = "";

	input[1] = "a";
	expected[1] = "a";

	for (test = 0; test < test_n; ++test) {
		char *result = NULL;

		/* Test information */
		printf("  [%4d] Test %3u: decode_alloc(\"%s\") should return \"%s\"\n", __LINE__, test, input[test], expected[test]);

		/* Test execution */
		result = decode_alloc(input[test]);

		/* Test assertions */
		assert(result != NULL);
		if (strcmp(result, expected[test]) != 0) {
			printf("        FAILED: Instead, got \"%s\"\n", result);
			free(result);
			return EXIT_FAILURE;
		};

		free(result);
	}

	return EXIT_SUCCESS;
}
