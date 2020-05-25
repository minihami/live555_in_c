#include "server_media_session.h"
#include "../groupsock/groupsock_helper.h"
#include <math.h>
#include "media.h"
#include "../groupsock/net_address.h"
#include "rtcp.h"


#include "../usage_environment/usage_environment_version.h"

static char const* const lib_name_str = "livemedia streaming media v";
char const* const lib_version_str = WEBGATE_LIBRARY_VERSION_STRING;

/*****************************************************************
 * ServerMediaSession:Medium (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_server_media_session_t *__livemedia_server_media_session__alloc(void)
{
	livemedia_server_media_session_t *server_media_session;
	server_media_session = (livemedia_server_media_session_t *)calloc(1, 
			sizeof(livemedia_server_media_session_t));
	if (server_media_session == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_server_media_session_t)) failed in"
				"%s()\n", __func__);
		return NULL;
	}
	return server_media_session;
}
void __livemedia_server_media_session__init(
		livemedia_server_media_session_t *server_media_session, char const *stream_name,
		char const *info, char const *description, bool is_ssm,
		char const *misc_sdp_lines)
{
	__livemedia_medium__init(&server_media_session->medium);

	server_media_session->is_ssm = is_ssm;
	server_media_session->subsessions_head = NULL;
	server_media_session->subsessions_tail = NULL;
	server_media_session->subsession_counter = 0;
	server_media_session->reference_count = 0;
	server_media_session->delete_when_unreferenced = false;
	server_media_session->stream_name = livemedia__str_dup(stream_name == NULL ?
			"" : stream_name);
	char *lib_name_plus_version_str = NULL; /* by default */
	if (info == NULL || description == NULL) {
		lib_name_plus_version_str = (char *)malloc(sizeof(char) * (strlen(lib_name_str) +
					strlen(lib_version_str) + 1));
		sprintf(lib_name_plus_version_str, "%s%s", lib_name_str, lib_version_str);
	}
	server_media_session->info_sdp_string =
		livemedia__str_dup(info == NULL ? lib_name_plus_version_str : info);
	server_media_session->description_sdp_string =
		livemedia__str_dup(description == NULL ? lib_name_plus_version_str : description);
	free(lib_name_plus_version_str);
	lib_name_plus_version_str = NULL;

	server_media_session->misc_sdp_lines = livemedia__str_dup(misc_sdp_lines == NULL ?
			"" : misc_sdp_lines);

	gettimeofday(&server_media_session->creation_time, NULL);
	
	server_media_session->note_liveness =  
		livemedia_server_media_session__note_liveness__impl;
	server_media_session->medium.is_server_media_session = 
		livemedia_server_media_session__is_server_media_session__impl;
	server_media_session->medium.delete = livemedia_server_media_session__delete__impl;
}
void __livemedia_server_media_session__deinit(
		livemedia_server_media_session_t *server_media_session)
{
	livemedia_server_media_session__delete_all_subsessions(server_media_session);
	if (server_media_session->stream_name)
		free(server_media_session->stream_name);
	if (server_media_session->info_sdp_string)
		free(server_media_session->info_sdp_string);
	if (server_media_session->description_sdp_string)
		free(server_media_session->description_sdp_string);
	if (server_media_session->misc_sdp_lines)
		free(server_media_session->misc_sdp_lines);

	__livemedia_medium__deinit(&server_media_session->medium);
}
void __livemedia_server_media_session__free(
		livemedia_server_media_session_t *server_media_session)
{
	if (server_media_session) {
		free(server_media_session);
		server_media_session = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_server_media_session_t *livemedia__new__server_media_session(
		 char const *stream_name, char const *info, char const *description, bool is_ssm,
		char const *misc_sdp_lines)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_session_t *server_media_session;

	server_media_session = __livemedia_server_media_session__alloc();
	if (server_media_session)
		__livemedia_server_media_session__init(server_media_session, stream_name,
				info, description, is_ssm, misc_sdp_lines);

	return server_media_session;
}
/*
 * Functions for virtual functions
 */
void livemedia_server_media_session__note_liveness(
		livemedia_server_media_session_t *server_media_session)
{
	if (server_media_session && server_media_session->note_liveness)
		server_media_session->note_liveness(server_media_session);
}

/*
 * Override functions
 */
void livemedia_server_media_session__note_liveness__impl(
			livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementaion: do nothing */
}
bool livemedia_server_media_session__is_server_media_session__impl(
		livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true;
}
void livemedia_server_media_session__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_session_t *server_media_session;
	server_media_session = (livemedia_server_media_session_t *)medium;
	if (server_media_session) {
		__livemedia_server_media_session__deinit(server_media_session);
		__livemedia_server_media_session__free(server_media_session);
	}
}

/*
 * Functions in header file
 */
char const *livemedia_server_media_session__stream_name(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return server_media_session->stream_name;
}
unsigned int livemedia_server_media_session__num_subsessions(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return server_media_session->subsession_counter;
}
unsigned int livemedia_server_media_session__reference_count(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return server_media_session->reference_count;
}
void livemedia_server_media_session__increment_reference_count(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	++server_media_session->reference_count;
}
void livemedia_server_media_session__decrement_reference_count(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (server_media_session->reference_count > 0)
		--server_media_session->reference_count;
}
bool *livemedia_server_media_session__delete_when_unreferenced(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &server_media_session->delete_when_unreferenced;
}

/*
 * Normal functions
 */
livemedia_server_media_session_t *livemedia_server_media_session__create_new__static(
		char const *stream_name, char const *info, char const *description, bool is_ssm,
		char const *misc_sdp_lines)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia__new__server_media_session(stream_name, info, description, is_ssm,
			misc_sdp_lines);
}
bool livemedia_server_media_session__lookup_by_name__static(char const *medium_name,
		livemedia_server_media_session_t **result_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	*result_session = NULL;
	livemedia_medium_t *medium;
	if (!livemedia_medium__lookup_by_name__static(medium_name, &medium))
		return false;
	if(!livemedia_medium__is_server_media_session(medium)) {
		fprintf(stderr, "%s is not a 'server_media_session' object in %s()",
				medium_name, __func__);
		return false;
	}
	*result_session = (livemedia_server_media_session_t *)medium;
	return true;
}
char *livemedia_server_media_session__generate_sdp_description(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_string_t *ip_address_str;
	unsigned int ip_address_str_size;
	char *source_filter_line;
	char *range_line;
	char *sdp;
	unsigned int sdp_length;
	livemedia_server_media_subsession_t *subsession;
	char const* sdp_lines;
	float dur;
	char buf[100];
	char *media_sdp;
	unsigned int media_sdp_length;

	ip_address_str= groupsock__new__address_string_by_net_address_bits(
			groupsock__our_ip_address()); 
	ip_address_str_size = strlen(groupsock_address_string__val(ip_address_str));
  
	/* For a SSM sessions, we need a "a=source-filter: incl ..." line also: */

	if (server_media_session->is_ssm) {
		char const* const source_filter_fmt = 
			"a=source-filter: incl IN IP4 * %s\r\n"
			"a=rtcp-unicast: reflection\r\n";
		unsigned int const source_filter_fmt_size = strlen(source_filter_fmt) + 
			ip_address_str_size + 1;
		source_filter_line = (char *)malloc(sizeof(char) * source_filter_fmt_size);
		sprintf(source_filter_line, source_filter_fmt, 
				groupsock_address_string__val(ip_address_str));
	} else {
		source_filter_line = livemedia__str_dup("");
	}
	
	range_line = NULL; /* for now */
	sdp = NULL; /* for now */
	
	do {
		/* Count the lengths of each subsession's media-level SDP lines.
		 * (We do this first, because the call to "subsession->sdpLines()"
		 * causes correct subsession 'duration()'s to be calculated later.) */
		
		 sdp_length = 0;
		 for (subsession = server_media_session->subsessions_head; subsession != NULL;
				 subsession = subsession->next) {
			 sdp_lines = livemedia_server_media_subsession__sdp_lines(subsession);
			 printf("sdp_lines in %s()\n", sdp_lines);
			 if (sdp_lines == NULL)
				 continue; /* the media is not available */
			 sdp_length += strlen(sdp_lines);
		 }
		 if (sdp_length == 0)
			 break; /* the session has no usable subsessions */

		 /* Unless subsessions have differing durations,
		  * 	we also have a "a=range:" line: */
		 dur = livemedia_server_media_session__duration(server_media_session);
		 if (dur == 0.0) {
			 range_line = livemedia__str_dup("a=range:npt=0-\r\n");
		 } else if (dur > 0.0) {
			 sprintf(buf, "a=range:npt=0-%.3f\r\n", dur);
			 range_line = livemedia__str_dup(buf);
		 } else {
			 /* subsessions have differing durations, so "a=range:" lines go there */
			 range_line = livemedia__str_dup("");
		 }

		 char const* const sdp_prefix_fmt =
			 "v=0\r\n"
			 "o=- %ld%06ld %d IN IP4 %s\r\n"
			 "s=%s\r\n"
			 "i=%s\r\n"
			 "t=0 0\r\n"
			 "a=tool:%s%s\r\n"
			 "a=type:broadcast\r\n"
			 "a=control:*\r\n"
			 "%s"
			 "%s"
			 "a=x-qt-text-nam:%s\r\n"
			 "a=x-qt-text-inf:%s\r\n"
			 "%s";
		 sdp_length += strlen(sdp_prefix_fmt)
			 + 20 + 6 + 20 + ip_address_str_size
			 + strlen(server_media_session->description_sdp_string)
			 + strlen(server_media_session->info_sdp_string)
			 + strlen(lib_name_str)
			 + strlen(lib_version_str)
			 + strlen(source_filter_line)
			 + strlen(range_line)
			 + strlen(server_media_session->description_sdp_string)
			 + strlen(server_media_session->info_sdp_string)
			 + strlen(server_media_session->misc_sdp_lines);
		sdp_length += 1000;
		/* in case the length of the "subsession->sdpLines()" calls below change */
		sdp = (char *)malloc(sizeof(char) * sdp_length);
		if (sdp == NULL)
			break;

		/* Generate the SDP prefix (session-level lines): */
		snprintf(sdp, sdp_length, sdp_prefix_fmt,
				server_media_session->creation_time.tv_sec,
				server_media_session->creation_time.tv_usec, /* o= <session id> */
				1, /* o= <version> (needs to change if params are modified) */
				groupsock_address_string__val(ip_address_str), /* o= <address> */
				server_media_session->description_sdp_string, /* s= <description> */
				server_media_session->info_sdp_string, /* i= <info> */
				lib_name_str, lib_version_str, /* a=tool: */
				source_filter_line, /* a=source-filter: incl (if a SSM session) */
				range_line, /* a=range: line */
				server_media_session->description_sdp_string, /* a=x-qt-text-nam: line */
				server_media_session->info_sdp_string, /* a=x-qt-text-inf: line */
				server_media_session->misc_sdp_lines);
		/* miscellaneous session SDP lines (if any) */

		/* Then, add the (media-level) lines for each subsession: */
		media_sdp = sdp;
		for (subsession = server_media_session->subsessions_head; subsession != NULL;
				subsession = subsession->next) {
			media_sdp_length = strlen(media_sdp);
			media_sdp += media_sdp_length;
			sdp_length -= media_sdp_length;
			if (sdp_length <= 1)
				break; /* the SDP has somehow become too long */
			sdp_lines = livemedia_server_media_subsession__sdp_lines(subsession);
			if (sdp_lines != NULL)
				snprintf(media_sdp, sdp_length, "%s", sdp_lines);
		}
	} while (0);

	if(range_line) {
		free(range_line);
		range_line = NULL;
	}
	if(source_filter_line) {
		free(source_filter_line);
		source_filter_line = NULL;
	}
	return sdp;
}
bool livemedia_server_media_session__add_subsession(
		livemedia_server_media_session_t *server_media_session,
		livemedia_server_media_subsession_t *subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (subsession->parent_session != NULL)
		return false; /* it's already used */

	if (server_media_session->subsessions_tail == NULL) {
		server_media_session->subsessions_head = subsession;
	} else {
		server_media_session->subsessions_tail->next = subsession;
	}
	server_media_session->subsessions_tail = subsession;

	subsession->parent_session = server_media_session;
	subsession->track_number = ++server_media_session->subsession_counter;
	return true;
}
void livemedia_server_media_session__test_scale_factor(
		livemedia_server_media_session_t *server_media_session, float *scale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* First, try setting all subsessions to the desired scale.
	 * If the subsessions' actual scales differ from each other, choose the
	 * value that's closest to 1, and then try re-setting all subsessions to that
	 * value.  If the subsessions' actual scales still differ, re-set them all to 1. */
	float min_ss_scale;
	float max_ss_scale;
	float best_ss_scale;
	float best_distance_to1;
	livemedia_server_media_subsession_t *subsession;
	float ssscale;
	float distance_to1;
	
	min_ss_scale = 1.0;
	max_ss_scale = 1.0;
	best_ss_scale = 1.0;
	best_distance_to1 = 1.0;
	for (subsession = server_media_session->subsessions_head; subsession != NULL;
			subsession = subsession->next) {
		ssscale = *scale;
		livemedia_server_media_subsession__test_scale_factor(subsession, &ssscale);
		if (subsession == server_media_session->subsessions_head) {
			/* this is the first subsession */
			min_ss_scale = max_ss_scale = best_ss_scale = ssscale;
			best_distance_to1 = (float)fabs(ssscale - 1.0f);
		} else {
			if (ssscale < min_ss_scale) 
				min_ss_scale = ssscale;
			else if (ssscale > max_ss_scale)
				max_ss_scale = ssscale;

			distance_to1 = (float)fabs(ssscale - 1.0f);
			if (distance_to1 < best_distance_to1) {
				best_ss_scale = ssscale;
				best_distance_to1 = distance_to1;
			}
		}
	}
	if (min_ss_scale == max_ss_scale) {
		/* All subsessions are at the same scale: 
		 * 		minSSScale == bestSSScale == maxSSScale */
		*scale = min_ss_scale;
		return;
	}
	/* The scales for each subsession differ.  Try to set each one to the value
	 * that's closest to 1: */
	for (subsession = server_media_session->subsessions_head; subsession != NULL;
			subsession = subsession->next) {
		ssscale	= best_ss_scale;
		livemedia_server_media_subsession__test_scale_factor(subsession, &ssscale);
		if (ssscale != best_ss_scale)
			break; /* no luck */
	}
	if (subsession == NULL) {
		/* All subsessions are at the same scale: bestSSScale */
		*scale = best_ss_scale;
		return;
	}
  /* Still no luck.  Set each subsession's scale to 1: */
	for (subsession = server_media_session->subsessions_head; subsession != NULL;
			subsession = subsession->next) {
		ssscale = 1;
		livemedia_server_media_subsession__test_scale_factor(subsession, &ssscale);
	}
	*scale = 1;
}
float livemedia_server_media_session__duration(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	float min_subsession_duration;
	float max_subsession_duration;
	livemedia_server_media_subsession_t *subsession;
	char *abs_start_time;
	char *abs_end_time;
	float ssduration;

	min_subsession_duration = 0.0;
	max_subsession_duration = 0.0;
	for (subsession = server_media_session->subsessions_head; subsession != NULL;
			subsession = subsession->next) {
    /* Hack: If any subsession supports seeking by 'absolute' time,
	 * 		then return a negative value, to indicate that only subsessions
	 * 		will have a "a=range:" attribute: */
		abs_start_time = NULL;
		abs_end_time = NULL;
		livemedia_server_media_subsession__get_absolute_time_range(subsession,
				&abs_start_time, &abs_end_time);
		if (abs_start_time != NULL)
			return -1.0f;

		ssduration = livemedia_server_media_subsession__duration(subsession);
		if (subsession == server_media_session->subsessions_head)
			/* this is the first subsession */
			min_subsession_duration = max_subsession_duration = ssduration;
		else if (ssduration < min_subsession_duration) 
			min_subsession_duration = ssduration;
		else if (ssduration > max_subsession_duration)
			max_subsession_duration = ssduration;
	}
	
	if (max_subsession_duration != min_subsession_duration)
		return -max_subsession_duration; /* because subsession durations differ */
	else
		return max_subsession_duration; /* all subsession durations are the same */
}
void livemedia_server_media_session__delete_all_subsessions(
		livemedia_server_media_session_t *server_media_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_medium__close_by_medium_ptr__static((livemedia_medium_t *)server_media_session->subsessions_head);
	server_media_session->subsessions_head = server_media_session->subsessions_tail = NULL;
	server_media_session->subsession_counter = 0;
}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * ServerMediaSubsessionIterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_server_media_subsession_iterator_t *__livemedia_server_media_subsession_iterator__alloc(void)
{
	livemedia_server_media_subsession_iterator_t *server_media_subsession;
	server_media_subsession = (livemedia_server_media_subsession_iterator_t *)calloc(1, 
			sizeof(livemedia_server_media_subsession_iterator_t));
	if (server_media_subsession == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_server_media_subsession_iterator_t)) failed in"
				"%s()\n", __func__);
		return NULL;
	}
	return server_media_subsession;
}
void __livemedia_server_media_subsession_iterator__init(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator,
		livemedia_server_media_session_t *session)
{
	server_media_subsession_iterator->our_session = session;
	livemedia_server_media_subsession_iterator__reset(server_media_subsession_iterator);
	
	server_media_subsession_iterator->delete = 
		livemedia_server_media_subsession_iterator__delete__impl;

}
void __livemedia_server_media_subsession_iterator__deinit(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator)
{

}
void __livemedia_server_media_subsession_iterator__free(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator)
{
	if (server_media_subsession_iterator) {
		free(server_media_subsession_iterator);
		server_media_subsession_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_server_media_subsession_iterator_t *livemedia__new__server_media_subsession_iterator(
		livemedia_server_media_session_t *session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator;

	server_media_subsession_iterator = __livemedia_server_media_subsession_iterator__alloc();
	if (server_media_subsession_iterator)
		__livemedia_server_media_subsession_iterator__init(server_media_subsession_iterator, session);

	return server_media_subsession_iterator;

}
void livemedia__delete__server_media_subsession_iterator(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator)
{
	if (server_media_subsession_iterator && server_media_subsession_iterator->delete)
		server_media_subsession_iterator->delete(server_media_subsession_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_server_media_subsession_iterator__delete__impl(livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (server_media_subsession_iterator) {
		__livemedia_server_media_subsession_iterator__deinit(server_media_subsession_iterator);
		__livemedia_server_media_subsession_iterator__free(server_media_subsession_iterator);
	}
}
/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_server_media_subsession_t * livemedia_server_media_subsession_iterator__next(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_subsession_t *result;
	result = server_media_subsession_iterator->next_ptr;
	
	if (server_media_subsession_iterator->next_ptr != NULL)
		server_media_subsession_iterator->next_ptr = 
			server_media_subsession_iterator->next_ptr->next;

	return result;
}
void livemedia_server_media_subsession_iterator__reset(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	server_media_subsession_iterator->next_ptr =
		server_media_subsession_iterator->our_session->subsessions_head;
}

/*****************************************************************
 * ServerMediaSubsession:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_server_media_subsession_t *__livemedia_server_media_subsession__alloc(void)
{
	livemedia_server_media_subsession_t *server_media_subsession;
	server_media_subsession = (livemedia_server_media_subsession_t *)calloc(1, sizeof(livemedia_server_media_subsession_t));
	if (server_media_subsession == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_server_media_subsession_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return server_media_subsession;
}
void __livemedia_server_media_subsession__init(livemedia_server_media_subsession_t *server_media_subsession)
{

	__livemedia_medium__init(&server_media_subsession->medium);

	server_media_subsession->parent_session = NULL;
	server_media_subsession->server_address_for_sdp = 0;
	server_media_subsession->port_num_for_sdp = 0;
	server_media_subsession->next = NULL;
	server_media_subsession->track_number = 0;
	server_media_subsession->track_id = NULL;
	
	/* virtual functions configuration */ 

	server_media_subsession->pause_stream = 
		livemedia_server_media_subsession__pause_stream__impl;
	server_media_subsession->seek_stream_relative = 
		livemedia_server_media_subsession__seek_stream_relative;
	server_media_subsession->seek_stream_absolute = 
		livemedia_server_media_subsession__seek_stream_absolute;
	server_media_subsession->null_seek_stream = 
		livemedia_server_media_subsession__null_seek_stream__impl;
	server_media_subsession->set_stream_scale=
		livemedia_server_media_subsession__set_stream_scale__impl;

	server_media_subsession->get_current_npt = 
		livemedia_server_media_subsession__get_current_npt__impl;
	server_media_subsession->get_stream_source = 
		livemedia_server_media_subsession__get_stream_source__impl;
	server_media_subsession->test_scale_factor = 
		livemedia_server_media_subsession__test_scale_factor__impl;
	server_media_subsession->duration = 
		livemedia_server_media_subsession__duration__impl;
	server_media_subsession->get_absolute_time_range = 
		livemedia_server_media_subsession__get_absolute_time_range__impl;
	server_media_subsession->medium.delete = 
		livemedia_server_media_subsession__delete__impl;
}
void __livemedia_server_media_subsession__deinit(livemedia_server_media_subsession_t *server_media_subsession)
{

	if(server_media_subsession->track_id)
		free(server_media_subsession->track_id);

	livemedia_medium__close_by_medium_ptr__static((livemedia_medium_t *)server_media_subsession->next);

	__livemedia_medium__deinit(&server_media_subsession->medium);
}
void __livemedia_server_media_subsession__free(livemedia_server_media_subsession_t *server_media_subsession)
{
	if (server_media_subsession) {
		free(server_media_subsession);
		server_media_subsession = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_server_media_subsession_t *livemedia__new__server_media_subsession(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_subsession_t *server_media_subsession;

	server_media_subsession = __livemedia_server_media_subsession__alloc();
	if (server_media_subsession)
		__livemedia_server_media_subsession__init(server_media_subsession);

	return server_media_subsession;
}
/*
 * Functions for virtual functions
 */
char const *livemedia_server_media_subsession__sdp_lines(
		livemedia_server_media_subsession_t *server_media_subsession)
{
	if (server_media_subsession && server_media_subsession->sdp_lines)
		server_media_subsession->sdp_lines(server_media_subsession);
}
void livemedia_server_media_subsession__get_stream_parameters(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, 
		net_address_bits client_address, 
		groupsock_port_t const *client_rtp_port, 
		groupsock_port_t const *client_rtcp_port, 
		uv_tcp_t *tcp_socket_num,  
		unsigned char rtp_channel_id, 
		unsigned char rtcp_channel_id, 
		net_address_bits *destination_address, 
		u_int8_t *destination_ttl, 
		bool *is_multicast, 
		groupsock_port_t *server_rtp_port, 
		groupsock_port_t *server_rtcp_port, 
		void **stream_token)
{
	if (server_media_subsession && server_media_subsession->get_stream_parameters)
		server_media_subsession->get_stream_parameters(server_media_subsession,
				client_session_id, client_address, client_rtp_port,
				client_rtcp_port, tcp_socket_num, rtp_channel_id, rtcp_channel_id,
				destination_address, destination_ttl, is_multicast,
				server_rtp_port, server_rtcp_port, stream_token);
}
void livemedia_server_media_subsession__start_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token,
		usage_environment__task_func *rtcp_rr_handler, void *rtcp_rr_handler_client_data,
		unsigned short *rtp_seq_num,
		unsigned int *rtp_timestamp,
		livemedia__server_request_alternative_byte_handler *server_request_alternative_byte_handler,
		void *server_request_alternative_byte_handler_client_data)
{
	if (server_media_subsession && server_media_subsession->start_stream)
		server_media_subsession->start_stream(server_media_subsession,
				client_session_id, stream_token, rtcp_rr_handler,
				rtcp_rr_handler_client_data, rtp_seq_num, rtp_timestamp, 
				server_request_alternative_byte_handler,
				server_request_alternative_byte_handler_client_data);
}
void livemedia_server_media_subsession__pause_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token)
{
	if(server_media_subsession && server_media_subsession->pause_stream)
		server_media_subsession->pause_stream(server_media_subsession,
				client_session_id, stream_token);
}
void livemedia_server_media_subsession__seek_stream_relative(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, double *seek_ntp,
		double stream_duration, u_int64_t *num_bytes)
{
	if(server_media_subsession && server_media_subsession->seek_stream_relative)
		server_media_subsession->seek_stream_relative(server_media_subsession,
				client_session_id, stream_token, seek_ntp, stream_duration,
				num_bytes);
}
void livemedia_server_media_subsession__seek_stream_absolute(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, char **abs_start,
		char **abs_end)
{
	if(server_media_subsession && server_media_subsession->seek_stream_absolute)
		server_media_subsession->seek_stream_absolute(server_media_subsession,
				client_session_id, stream_token, abs_start, abs_end);
}
void livemedia_server_media_subsession__null_seek_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token, double stream_end_time,
		u_int64_t *num_bytes)
{
	if(server_media_subsession && server_media_subsession->null_seek_stream)
		server_media_subsession->null_seek_stream(server_media_subsession,
				client_session_id, stream_token, stream_end_time, num_bytes);
}
void livemedia_server_media_subsession__set_stream_scale(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned client_session_id, void *stream_token, float scale)
{
	if(server_media_subsession && server_media_subsession->set_stream_scale)
		server_media_subsession->set_stream_scale(server_media_subsession,
				client_session_id, stream_token, scale);
}
float livemedia_server_media_subsession__get_current_npt(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token)
{
	if(server_media_subsession && server_media_subsession->get_current_npt)
		server_media_subsession->get_current_npt(server_media_subsession,
				stream_token);
}
livemedia_framed_source_t *livemedia_server_media_subsession__get_stream_source(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token)
{
	if(server_media_subsession && server_media_subsession->get_stream_source)
		server_media_subsession->get_stream_source(server_media_subsession,
				stream_token);
}
void livemedia_server_media_subsession__get_rtp_sink_and_rtcp(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token, livemedia_rtp_sink_t const **rtp_sink,
		livemedia_rtcp_instance_t const **rtcp)
{
	if(server_media_subsession && server_media_subsession->get_rtp_sink_and_rtcp)
		server_media_subsession->get_rtp_sink_and_rtcp(server_media_subsession,
				stream_token, rtp_sink, rtcp);

}
void livemedia_server_media_subsession__delete_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void **stream_token)
{
	if(server_media_subsession && server_media_subsession->delete_stream)
		server_media_subsession->delete_stream(server_media_subsession,
				client_session_id, stream_token);

}
void livemedia_server_media_subsession__test_scale_factor(
		livemedia_server_media_subsession_t *server_media_subsession,
		float *scale)
{
	if(server_media_subsession && server_media_subsession->test_scale_factor)
		server_media_subsession->test_scale_factor(server_media_subsession,
				scale);
}
float livemedia_server_media_subsession__duration(
		livemedia_server_media_subsession_t *server_media_subsession)
{
	if(server_media_subsession && server_media_subsession->duration)
		server_media_subsession->duration(server_media_subsession);
}
void livemedia_server_media_subsession__get_absolute_time_range(
		livemedia_server_media_subsession_t *server_media_subsession,
		char **abs_start_time, char **abs_end_time)
{
	if(server_media_subsession && server_media_subsession->get_absolute_time_range)
		server_media_subsession->get_absolute_time_range(
				server_media_subsession, abs_start_time, abs_end_time);
}

/*
 * Override functions
 */
void livemedia_server_media_subsession__pause_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: do nothing */
}
void livemedia_server_media_subsession__seek_stream_relative__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, double *seek_ntp,
		double stream_duration, u_int64_t *num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: do nothing */
	*num_bytes = 0;
}
void livemedia_server_media_subsession__seek_stream_absolute__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, char **abs_start,
		char **abs_end)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: do nothing (but delete[] and assign 
	 * "absStart" and "absEnd" to NULL, to show that we don't handle this) */
	if(*abs_start) {
		free(*abs_start);
		*abs_start = NULL;
	}
	if(*abs_end) {
		free(*abs_end);
		*abs_end = NULL;
	}
}
void livemedia_server_media_subsession__null_seek_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token, double stream_end_time,
		u_int64_t *num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: do nothing */
	*num_bytes = 0;
}
void livemedia_server_media_subsession__set_stream_scale__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned client_session_id, void *stream_token, float scale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: do nothing */
}
float livemedia_server_media_subsession__get_current_npt__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: return 0.0 */
	return 0.0;
}
livemedia_framed_source_t *livemedia_server_media_subsession__get_stream_source__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: return NULL */
	return NULL;
}
void livemedia_server_media_subsession__delete_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void **stream_token)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: do nothing */
}
void livemedia_server_media_subsession__test_scale_factor__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		float *scale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: Support scale = 1 only */
	*scale = 1;
}
float livemedia_server_media_subsession__duration__impl(
		livemedia_server_media_subsession_t *server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: assume an unbounded session */
	return 0.0;
}
void livemedia_server_media_subsession__get_absolute_time_range__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		char **abs_start_time, char **abs_end_time)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* default implementation: We don't support seeking by 'absolute' time, 
   * so indicate this by setting both parameters to NULL: */
	*abs_start_time = *abs_end_time = NULL;
}

void livemedia_server_media_subsession__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_subsession_t* server_media_subsession;

	server_media_subsession = (livemedia_server_media_subsession_t *)medium;
	if(server_media_subsession->track_id) {
		free(server_media_subsession->track_id);
		server_media_subsession->track_id = NULL;
	}
	livemedia_medium__close_by_medium_ptr__static((livemedia_medium_t *)server_media_subsession->next);

	if(server_media_subsession) {
		__livemedia_server_media_subsession__deinit(server_media_subsession);
		__livemedia_server_media_subsession__free(server_media_subsession);
	}
}

/*
 * Functions in header file
 */
unsigned int livemedia_server_media_subsession__track_number(
		livemedia_server_media_subsession_t *server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return server_media_subsession->track_number;
}

/*
 * Normal functions
 */
char const *livemedia_server_media_subsession__track_id(
		livemedia_server_media_subsession_t *server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	char buf[100];
	if(server_media_subsession->track_number == 0)
		return NULL;	/* not yet in a ServerMediaSession */
	if(server_media_subsession->track_id == NULL) {
		sprintf(buf, "track%d", server_media_subsession->track_number);
		server_media_subsession->track_id = livemedia__str_dup(buf);
	}
	return server_media_subsession->track_id;
}
void livemedia_server_media_subsession__set_server_address_and_port_for_sdp(
		livemedia_server_media_subsession_t *server_media_subsession,
		net_address_bits address_bits, port_num_bits port_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	server_media_subsession->server_address_for_sdp = address_bits;
	server_media_subsession->port_num_for_sdp = port_bits;
}
char *livemedia_server_media_subsession__range_sdp_line(
		livemedia_server_media_subsession_t *server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

  /* First, check for the special case where we support seeking by 'absolute' time: */

	char *abs_start;
	char *abs_end;
	char buf[100];
	float our_duration;

	abs_start = NULL;
	abs_end = NULL;
	livemedia_server_media_subsession__get_absolute_time_range(server_media_subsession,
			&abs_start, &abs_end);

	if (abs_start != NULL) {
		if (abs_end != NULL)
			sprintf(buf, "a=range:clock=%s-%s\r\n", abs_start, abs_end);
		else
			sprintf(buf, "a=range:clock=%s-\r\n", abs_start);

		return livemedia__str_dup(buf);
	}
	
	if (server_media_subsession->parent_session == NULL)
		return NULL;

  /* If all of our parent's subsessions have the same duration
  * (as indicated by "fParentSession->duration() >= 0"), there's no "a=range:" line: */
	if (livemedia_server_media_session__duration(server_media_subsession->parent_session)
			>= 0.0)
		return livemedia__str_dup("");

  /* Use our own duration for a "a=range:" line: */
	our_duration = livemedia_server_media_subsession__duration(server_media_subsession);
	if (our_duration == 0.0) {
		return livemedia__str_dup("a=range:npt=0-\r\n");
	} else {
		memset(buf, 0x00, 100);
		sprintf(buf, "a=range:npt=0-%.3f\r\n", our_duration);
		return livemedia__str_dup(buf);
	}
}

