#define  _POSIX_C_SOURCE 200809L

#include <string.h>
#include <regex.h>
#include <malloc.h>

#include "regex-replace.h"

char * decode_alloc(char const * const _i) {

	char *ret = malloc(1);
	strcpy(ret, "");

	ret = regex_simplereplace(_i, "<br>", "", 1, 1);

	ret = regex_simplereplace(ret, "<span [^>]+>([^<]+)</span>", "%1", 1, 1);

	ret = regex_simplereplace(ret, "<div>([^<]+)</div>", "%1\n", 1, 1);

	ret = regex_simplereplace(ret, "&lt;", "<", 1, 1);

	ret = regex_simplereplace(ret, "&nbsp;", " ", 1, 1);

	return ret;
}
