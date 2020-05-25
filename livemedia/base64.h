#ifndef __LIVEMEDIA_BASE64_H__
#define __LIVEMEDIA_BASE64_H__
#include <stdbool.h>

unsigned char *livemedia__base64_decode(char const *in, unsigned int *result_size,
		bool trim_trailing_zeros);
/* returns a newly allocated array - of size "resultSize" - that
 * the caller is responsible for delete[]ing. */

unsigned char *livemedia__base64_decode_by_input_size(char const *in, 
		unsigned int in_size,
		unsigned int *result_size,
		bool trim_trailing_zeros);
/* As above, but includes the size of the input string (i.e., the number of bytes to decode) as a parameter.
 * This saves an extra call to "strlen()" if we already know the length of the input string. */

char *livemedia__base64_encode(char const *orig, unsigned int orig_length);
    /* returns a 0-terminated string that
     * the caller is responsible for delete[]ing. */
#endif

