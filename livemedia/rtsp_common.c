#include "rtsp_common.h"
/* TODO: #include "locale.h" */
#include <string.h>
#include <stdio.h>
#include <ctype.h> // for "isxdigit()
#include <time.h> // for "strftime()" and "gmtime()"

static void livemedia__decode_url(char *url) 
{
  /* Replace (in place) any %<hex><hex> sequences with the appropriate 8-bit character. */
  char* cursor;
  char hex[3];
  cursor = url;
  while (*cursor) {
    if ((cursor[0] == '%') &&
	cursor[1] && isxdigit(cursor[1]) &&
	cursor[2] && isxdigit(cursor[2])) {
      /* We saw a % followed by 2 hex digits, so we copy the literal hex value into the URL, then advance the cursor past it: */
      hex[0] = cursor[1];
      hex[1] = cursor[2];
      hex[2] = '\0';
      *url++ = (char)strtol(hex, NULL, 16);
      cursor += 3;
    } else {
      /* Common case: This is a normal character or a bogus % expression, so just copy it */
      *url++ = *cursor++;
    }
  }
  *url = '\0';
}

static bool livemedia__is_separator(char c)
{

}

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
		unsigned int *content_length)
{
	/* TODO: variable declaration position */
  /* This parser is currently rather dumb; it should be made smarter ##### */

  /* "Be liberal in what you accept": Skip over any whitespace at the start of the request: */
  unsigned int  i;
  char c;

  for (i = 0; i < req_str_size; ++i) {
    c = req_str[i];
    if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\0')) break;
  }
  if (i == req_str_size) return False; // The request consisted of nothing but whitespace!

  // Then read everything up to the next space (or tab) as the command name:
  bool parse_succeeded = False;
  unsigned int  i1 = 0;
  for (; i1 < result_cmd_name_max_size-1 && i < req_str_size; ++i,++i1) {
    char c = req_str[i];
    if (c == ' ' || c == '\t') {
      parse_succeeded = True;
      break;
    }

    result_cmd_name[i1] = c;
  }
  result_cmd_name[i1] = '\0';
  if (!parse_succeeded) return False;

  // Skip over the prefix of any "rtsp://" or "rtsp:/" URL that follows:
  unsigned int  j = i+1;
  while (j < req_str_size && (req_str[j] == ' ' || req_str[j] == '\t')) ++j; // skip over any additional white space
  for (; (int)j < (int)(req_str_size-8); ++j) {
    if ((req_str[j] == 'r' || req_str[j] == 'R')
	&& (req_str[j+1] == 't' || req_str[j+1] == 'T')
	&& (req_str[j+2] == 's' || req_str[j+2] == 'S')
	&& (req_str[j+3] == 'p' || req_str[j+3] == 'P')
	&& req_str[j+4] == ':' && req_str[j+5] == '/') {
      j += 6;
      if (req_str[j] == '/') {
	// This is a "rtsp://" URL; skip over the host:port part that follows:
	++j;
	while (j < req_str_size && req_str[j] != '/' && req_str[j] != ' ') ++j;
      } else {
	// This is a "rtsp:/" URL; back up to the "/":
	--j;
      }
      i = j;
      break;
    }
  }

  // Look for the URL suffix (before the following "RTSP/"):
  parse_succeeded = False;
  for (unsigned int  k = i+1; (int)k < (int)(req_str_size-5); ++k) {
    if (req_str[k] == 'R' && req_str[k+1] == 'T' &&
	req_str[k+2] == 'S' && req_str[k+3] == 'P' && req_str[k+4] == '/') {
      while (--k >= i && req_str[k] == ' ') {} // go back over all spaces before "RTSP/"
      unsigned int  k1 = k;
      while (k1 > i && req_str[k1] != '/') --k1;

      // ASSERT: At this point
      //   i: first space or slash after "host" or "host:port"
      //   k: last non-space before "RTSP/"
      //   k1: last slash in the range [i,k]

      // The URL suffix comes from [k1+1,k]
      // Copy "result_url_suffix":
      unsigned int  n = 0, k2 = k1+1;
      if (k2 <= k) {
        if (k - k1 + 1 > result_url_suffix_max_size) return False; // there's no room
        while (k2 <= k) result_url_suffix[n++] = req_str[k2++];
      }
      result_url_suffix[n] = '\0';

      // The URL 'pre-suffix' comes from [i+1,k1-1]
      // Copy "result_url_pre_suffix":
      n = 0; k2 = i+1;
      if (k2+1 <= k1) {
        if (k1 - i > result_url_pre_suffix_max_size) return False; // there's no room
        while (k2 <= k1-1) result_url_pre_suffix[n++] = req_str[k2++];
      }
      result_url_pre_suffix[n] = '\0';
      livemedia__decode_url(result_url_pre_suffix);

      i = k + 7; // to go past " RTSP/"
      parse_succeeded = True;
      break;
    }
  }
  if (!parse_succeeded) return False;

  // Look for "CSeq:" (mandatory, case insensitive), skip whitespace,
  // then read everything up to the next \r or \n as 'CSeq':
  parse_succeeded = False;
  for (j = i; (int)j < (int)(req_str_size-5); ++j) {
    if (_strncasecmp("CSeq:", &req_str[j], 5) == 0) {
      j += 5;
      while (j < req_str_size && (req_str[j] ==  ' ' || req_str[j] == '\t')) ++j;
      unsigned int  n;
      for (n = 0; n < result_cseq_max_size-1 && j < req_str_size; ++n,++j) {
	char c = req_str[j];
	if (c == '\r' || c == '\n') {
	  parse_succeeded = True;
	  break;
	}

	result_cseq[n] = c;
      }
      result_cseq[n] = '\0';
      break;
    }
  }
  if (!parse_succeeded) return False;

  // Look for "Session:" (optional, case insensitive), skip whitespace,
  // then read everything up to the next \r or \n as 'Session':
  result_session_id_str[0] = '\0'; // default value (empty string)
  for (j = i; (int)j < (int)(req_str_size-8); ++j) {
    if (_strncasecmp("Session:", &req_str[j], 8) == 0) {
      j += 8;
      while (j < req_str_size && (req_str[j] ==  ' ' || req_str[j] == '\t')) ++j;
      unsigned int  n;
      for (n = 0; n < result_session_id_str_max_size-1 && j < req_str_size; ++n,++j) {
	char c = req_str[j];
	if (c == '\r' || c == '\n') {
	  break;
	}

	result_session_id_str[n] = c;
      }
      result_session_id_str[n] = '\0';
      break;
    }
  }

  // Also: Look for "Content-Length:" (optional, case insensitive)
  content_length = 0; // default value
  for (j = i; (int)j < (int)(req_str_size-15); ++j) {
    if (_strncasecmp("Content-Length:", &(req_str[j]), 15) == 0) {
      j += 15;
      while (j < req_str_size && (req_str[j] ==  ' ' || req_str[j] == '\t')) ++j;
      unsigned int  num;
      if (sscanf(&req_str[j], "%u", &num) == 1) {
	content_length = num;
      }
    }
  }
  return True;

}

bool livemedia__parse_range_param(char const *param_str, double *range_start, double *range_end, 
			char **abs_start_time, char **abs_end_time, bool *start_time_is_now)
{

}

bool livemedia__parse_range_header(char const *buf, double *range_start, double *range_end, 
			char **abs_start_time, char **abs_end_time, bool *start_time_is_now)
{

}

bool livemedia__parse_scale_header(char const *buf, float *scale)
{

}

bool livemedia__rtsp_options_is_supported(char const *command_name,
		char const *options_response_string)
{

}

char const *livemedia__date_header()
{
  static char buf[200];
#if !defined(_WIN32_WCE)
  time_t tt = time(NULL);
  strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
#else
  /* WinCE apparently doesn't have "time()", "strftime()", or "gmtime()",
  * so generate the "Date:" header a different, WinCE-specific way.
  * (Thanks to Pierre l'Hussiez for this code)
  * RSF: But where is the "Date: " string?  This code doesn't look quite right... */
  SYSTEMTIME system_time;
  GetSystemTime(&system_time);
  WCHAR *date_format = L"ddd, MMM dd yyyy";
  WCHAR *time_format = L"HH:mm:ss GMT\r\n";
  WCHAR in_buf[200];
  DWORD locale = LOCALE_NEUTRAL;

  int ret = GetDateFormat(locale, 0, &system_time,
                          (LPTSTR)date_format, (LPTSTR)in_buf, sizeof(in_buf));
  inBuf[ret - 1] = ' ';
  ret = GetTimeFormat(locale, 0, &system_time,
                      (LPTSTR)time_format,
                      (LPTSTR)in_buf + ret, (sizeof(in_buf)) - ret);
  wcstombs(buf, in_buf, wcslen(in_buf));
#endif
  return buf;

}
