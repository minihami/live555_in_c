#ifndef __LIVEMEDIA_RTSP_COMMON_H__
#define __LIVEMEDIA_RTSP_COMMON_H__

#include <stdbool.h>
#include "media.h" 	// includes some definitions perhaps needed for Borland compilers?

#if defined(__WIN32__) || defined(_WIN32) || defined(_QNX4)
#define _strncasecmp _strnicmp
#define snprintf _snprintf
#else
#define _strncasecmp strncasecmp
#endif

#define RTSP_PARAM_STRING_MAX 200

bool livemedia__parse_rtsp_request_string(char const *req_str, unsigned int req_str_size,
		char *result_cmd_name,
		unsigned int result_cmd_name_max_size,
		char *result_url_pre_suffix,
		unsigned int result_url_pre_suffix_max_size,
		char *result_url_suffix,
		unsigned int result_url_suffix_max_size,
		char *result_cseq,
		unsigned int result_cseq_max_size,
		char *result_session_id,
		unsigned int result_session_id_max_size,
		unsigned int *content_length);

bool livemedia__parse_range_param(char const *param_str, double *range_start, double *range_end, 
			char **abs_start_time, char **abs_end_time, bool *start_time_is_now);

bool livemedia__parse_range_header(char const *buf, double *range_start, double *range_end, 
			char **abs_start_time, char **abs_end_time, bool *start_time_is_now);

bool livemedia__parse_scale_header(char const *buf, float *scale);

bool livemedia__rtsp_options_is_supported(char const *command_name,
		char const *options_response_string);
    /* Returns True iff the RTSP command "commandName" is mentioned as one of the commands supported in "optionsResponseString"
    * (which should be the 'resultString' from a previous RTSP "OPTIONS" request). */

char const *livemedia__date_header(); /* A "Date:" header that can be used in a RTSP (or HTTP) response */

#endif
