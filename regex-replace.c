#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>

#include "regex-replace.h"

#define REPCHAR '%'
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

	int nmatch = MAXSUBS;
	regmatch_t matchptr[MAXSUBS];

	int from = 0;
	for (errcode = regexec(&compiled, string_to_search_in, nmatch, matchptr, 0);
		errcode != REG_NOMATCH && (global_replace != 0 || from == 0);
		errcode = regexec(&compiled, string_to_search_in + from, nmatch, matchptr, 0))
	{

		char * pos = NULL;

		if (errcode != 0) {
			return NULL;
		}

		if (matchptr[0].rm_eo - matchptr[0].rm_so < replacement_len) {
			ret = (void *)realloc(ret, strlen(ret) + matchptr[0].rm_so + replacement_len - matchptr[0].rm_eo + matchptr[0].rm_so + 1);
		}

		strncat(ret, string_to_search_in + from, matchptr[0].rm_so);

		/* Iterate through the replacement string, copying it char by char,
		 * unless "%n" is found (assuming '%' is REPCHAR) in which case,
		 * use go back to the search string and use matchptr[n].rm_eo and
		 * .rm_so to paste from it into the result.
		 */
		int rep_off = 0;
		int rep_len = strlen(replacement);

		while (rep_off < rep_len) {
			pos = strchr(replacement+rep_off, REPCHAR);
			if (pos == NULL) {
				strcat(ret, replacement+rep_off);
				rep_off =+ strlen(replacement+rep_off);
				continue;
			}

			strncat(ret, replacement+rep_off, pos - (replacement+rep_off));
			if ((*(pos+1) == '\0') && (*(pos+1) == REPCHAR)) {
				*ret = REPCHAR;
				*(ret+1) = '\0';
				rep_off =+ 1;
				continue;
			}

			if (*(pos+1) == REPCHAR) {
				*ret = REPCHAR;
				*(ret+1) = '\0';
				rep_off =+ 1;
				continue;
			}

			int replacement_group = 0;
			int replacement_group_nextchar = 1;
			char * digits = "0123456789";
			char * n;
			while ((n = strchr(digits, *(pos+replacement_group_nextchar))) != NULL) {
				if ((n - digits) == 10) {
					break;
				}
				replacement_group = replacement_group * 10 + (n - digits);
				++replacement_group_nextchar;
			}
			if (replacement_group < MAXSUBS) {
				strncat(ret, string_to_search_in + matchptr[replacement_group].rm_so, matchptr[replacement_group].rm_eo - matchptr[replacement_group].rm_so);
			}
			rep_off =+ replacement_group_nextchar;
		}

		from += matchptr[0].rm_eo;

	}

	strcat(ret, string_to_search_in + from);

	regfree(&compiled);

	return ret;

}
