#ifndef __LIVEMEDIA_STR_DUP_H__
#define __LIVEMEDIA_STR_DUP_H__

#include <stdlib.h>

char *livemedia__str_dup(char const* str);
char *livemedia__str_dup_size(char const *str);
char *livemedia__str_dup_size_get_buf_size(char const *str, size_t *result_buf_size);
#endif
