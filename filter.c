#define  _POSIX_C_SOURCE 200809L

#include <string.h>

char * decode_alloc(char const * const _i) {
	return strdup(_i);
}
