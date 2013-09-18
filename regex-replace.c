#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>

#include "regex-replace.h"

#define MAXSUBS 4096

char * regex_simplereplace(const char* string_to_search_in, const char* restrict pattern, const char* replacement, int case_insensitive, int global_replace) {

	int replacement_len = strlen(replacement);

	char *ret = (void *)malloc(strlen(string_to_search_in)+1);
	strcpy(ret, "");

	regex_t compiled;
	int cflags = REG_EXTENDED | (REG_ICASE * (case_insensitive ? 1 : 0));

	int errcode;
	errcode = regcomp(&compiled, pattern, cflags);

	if (errcode != 0) {
		return NULL;
	}

	int nmatch = 1;
	regmatch_t matchptr[MAXSUBS];

	int from = 0;
	for (errcode = regexec(&compiled, string_to_search_in, nmatch, matchptr, 0);
		errcode != REG_NOMATCH && (global_replace != 0 || from == 0);
		errcode = regexec(&compiled, string_to_search_in + from, nmatch, matchptr, 0))
	{

		if (errcode != 0) {
			return NULL;
		}

		if (matchptr[0].rm_eo - matchptr[0].rm_so < replacement_len) {
			ret = (void *)realloc(ret, strlen(ret) + matchptr[0].rm_so + replacement_len - matchptr[0].rm_eo + matchptr[0].rm_so + 1);
		}

		strncat(ret, string_to_search_in + from, matchptr[0].rm_so);
		strcat(ret, replacement);
		from += matchptr[0].rm_eo;

	}

	strcat(ret, string_to_search_in + from);

	regfree(&compiled);

	return ret;

}
