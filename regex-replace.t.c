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

	/* regex_simplereplace() was always doing global replaces; make sure we fixed that. */
	r = regex_simplereplace("10 quick fox", " ", "_", 0, 0);
	e = "10_quick fox";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	/* Just to make sure we didn't break anything with the above test. */
	r = regex_simplereplace("20 quick fox", " ", "_", 0, 1);
	e = "20_quick_fox";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	/* Make sure case_insensitive == 1 is working correctly */
	r = regex_simplereplace("Rapid roadrunner", "r", ":", 1, 1);
	e = ":apid :oad:unne:";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	/* Make sure case_insensitive == 0 is working correctly */
	r = regex_simplereplace("Rapid roadrunner", "r", ":", 0, 1);
	e = "Rapid :oad:unne:";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	r = regex_simplereplace("metete tete que te metas tete", "((te)+ )+", "T! ", 0, 1);
	e = "meT! que T! metas tete";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	r = regex_simplereplace("abc<span tag='x'>def</span>ghi", "<span[^>]+>([^<]+)</span>", "%1", 1, 1);
	e = "abcdefghi";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	r = regex_simplereplace("<div>para1</div>para2", "<div>([^<]+)</div>", "%1\n", 1, 1);
	e = "para1\npara2";
	if ((strcmp(r, e)) != 0) {
		fprintf(stderr, "I was expecting:\n==> %s\nbut I got:\n==> %s\n", e, r);
		return EXIT_FAILURE;
	}

	r = regex_simplereplace("<div>Botella</div><div>Cuartito</div>Caguama", "<div>([^<]+)</div>", "%1\n", 1, 1);
	e = "Botella\nCuartito\nCaguama";
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
