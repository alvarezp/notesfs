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

	/* This one is to prevent a crash */
	r = regex_simplereplace("10 quick fox", "x", "xes waiting for chickens", 0, 0);
	e = "10 quick foxes waiting for chickens";
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
