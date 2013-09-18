#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "regex-replace.h"

int test() {

	char * r = regex_simplereplace("abcdefghijkl", "g", "h", 0, 0);

	if ((strcmp(r, "abcdefhhijkl")) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", "abcdefhhijkl", r);
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
