#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "filter.h"

char * quote(char const * s) {
	int i = 0;
	int o = 0;
	char * r = malloc(strlen(s) * 2 + 1);
	while (s[i] != '\0') {
		switch(s[i]) {
		case '\n':
			r[i+o] = '\\';
			r[i+o+1] = 'n';
			++o;
			break;
		default:
			r[i+o] = s[i];
		}
		++i;
	}
	r[i+o] = '\0';
	return r;
}

int main(void) {

	int test = 0;

	const int test_n = 7;

	char const * input[test_n];
	char const * expected[test_n];

	input[0] = "";
	expected[0] = "";

	input[1] = "a";
	expected[1] = "a";

	input[2] = "<div>para1</div>";
	expected[2] = "para1";

	input[3] = "<div>para1</div><div>para2</div>";
	expected[3] = "para1\npara2";

	input[4] = "<div>para1 <span attr='val'>text</span></div><div>para2</div>";
	expected[4] = "para1 text\npara2";

	input[5] = "<div>para1 <span attr='val'>text</span>more<br>text</div><div>para2</div>";
	expected[5] = "para1 textmoretext\npara2";

	input[6] = "<div>para1 <span attr='val'>text</span>mo&lt;re<br>text</div><div>para2</div>";
	expected[6] = "para1 textmo<retext\npara2";

	for (test = 0; test < test_n; ++test) {
		char *result = NULL;

		char *qi;
		char *qe;
		char *qr;

		/* Test information */
		qi = quote(input[test]);
		qe = quote(expected[test]);
		printf("  [%4d] Test %3u: decode_alloc(\"%s\") should return \"%s\"\n", __LINE__, test, qi, qe);

		/* Test execution */
		result = decode_alloc(input[test]);

		/* Test assertions */
		qr = quote(result);
		assert(result != NULL);
		if (strcmp(result, expected[test]) != 0) {
			printf("        FAILED: Instead, got \"%s\"\n", qr);

			free(result);
			free(qi);
			free(qe);
			free(qr);
			return EXIT_FAILURE;
		};

		free(result);
		free(qi);
		free(qe);
		free(qr);

	}

	return EXIT_SUCCESS;
}
