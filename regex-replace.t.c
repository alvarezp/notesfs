#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "regex-replace.h"

int test() {

	char * r;
	char * e;

	r = regex_simplereplace("abcdefghijkl", "g", "h", 0, 0);
	e = "abcdefhhijkl";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main() {
	if (test() != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
