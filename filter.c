#define  _POSIX_C_SOURCE 200809L

#include <string.h>
#include <regex.h>
#include <malloc.h>

#include "regex-replace.h"

char * decode_alloc(char const * const _i) {

	char *ret = NULL;
	char *ret2 = NULL;

	ret = regex_simplereplace(_i, "<br>", "", 1, 1);
	ret2 = strdup(ret);
	free(ret);

	ret = regex_simplereplace(ret2, "<span [^>]+>([^<]+)</span>", "%1", 1, 1);
	free(ret2);
	ret2 = strdup(ret);
	free(ret);

	ret = regex_simplereplace(ret2, "<div>([^<]+)</div>", "%1\n", 1, 1);
	free(ret2);
	ret2 = strdup(ret);
	free(ret);

	ret = regex_simplereplace(ret2, "&lt;", "<", 1, 1);
	free(ret2);
	ret2 = strdup(ret);
	free(ret);

	ret = regex_simplereplace(ret2, "&nbsp;", " ", 1, 1);
	free(ret2);
	ret2 = strdup(ret);
	free(ret);

	return ret2;
}
