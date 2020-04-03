#include <stdio.h>
#include <string.h>
#include "str_dup.h"

char *livemedia__str_dup(char const* str)
{
	size_t len;
	char *copy;

	if (str == NULL) {
		return NULL;
	}
	len = strlen(str) + 1;
	copy = malloc(sizeof(char) * len);
	if (copy == NULL) {
		fprintf(stderr, "malloc(%ld) failed in %s()\n",
				sizeof(char) * len, __func__);
		return NULL;
	}
	memcpy(copy, str, len);

	return copy;
}

char *livemedia__str_dup_size(char const *str, size_t *result_buf_size)
{
	size_t len;
	char *copy;

	if (str == NULL) {
		if (result_buf_size) {
			*result_buf_size = 0;
		}
		return NULL;
	}

	len = strlen(str) + 1;
	copy = malloc(sizeof(char) * len);
	if (copy == NULL) {
		fprintf(stderr, "malloc(%ld) failed in %s()\n",
				sizeof(char) * len, __func__);
		if (result_buf_size) {
			*result_buf_size = 0;
		}
		return NULL;
	}

	memcpy(copy, str, len);
	if (result_buf_size) {
		*result_buf_size = len;
	}

	return copy;
}
