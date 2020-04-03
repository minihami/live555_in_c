#ifndef __LIVEMEDIA_SERVER_MEDIA_SESSION_H__
#define __LIVEMEDIA_SERVER_MEDIA_SESSION_H__

#include <time.h>
#include <sys/types.h>
#include <stdbool.h>
#include "media.h"
#include "../groupsock/net_address.h"
#include "framed_source.h"
#include "rtp_sink.h"
#include "rtcp.h"

typedef void (*task_func)(void *client_data);
typedef void server_request_alternative_byte_handler(void* instance,
		u_int8_t request_byte);
struct _livemedia_server_media_subsession_t;

/*****************************************************************
 * ServerMediaSession:Medium (Structure Declarations)
 ****************************************************************/
/* forward */
typedef struct _livemedia_server_media_session_t {
	livemedia_medium_t medium;

	bool is_ssm;
	struct _livemedia_server_media_subsession_t* subsessions_head;
	struct _livemedia_server_media_subsession_t* subsessions_tail;
	unsigned int subsession_counter;

	char *stream_name;
	char *info_sdp_string;
	char *description_sdp_string;
	char *misc_sdp_lines;
	struct timeval creation_time;
	unsigned int reference_count;
	bool delete_when_unreferenced;

	void (*note_liveness)(struct _livemedia_server_media_session_t* server_media_session);

} livemedia_server_media_session_t;

/*****************************************************************
 * ServerMediaSubsessionIterator (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_server_media_subsession_iterator_t {
	livemedia_server_media_session_t *our_session;
	struct _livemedia_server_media_subsession_t *next_ptr;

	void (*delete)(
			struct _livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);
} livemedia_server_media_subsession_iterator_t;

/*****************************************************************
 * ServerMediaSubsession:Medium (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_server_media_subsession_t {
	livemedia_medium_t medium;

	livemedia_server_media_session_t *parent_session;
	net_address_bits server_address_for_sdp;
	port_num_bits port_num_for_sdp;
	struct _livemedia_server_media_subsession_t *next;
	unsigned int track_number; /* within an enclosing ServerMediaSession */
	char const* track_id;

	char const *(*sdp_lines)(
			struct _livemedia_server_media_subsession_t *server_media_subsession);
	void (*get_stream_parameters)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, /* in */
			net_address_bits client_address, /* in */
			groupsock_port_t const *client_rtp_port, /* in */
			groupsock_port_t const *client_rtcp_port, /* in */
			int tcp_socket_num,  /* in (-1 means use UDP, not TCP) */
			unsigned char rtp_channel_id, /* in (used if TCP)*/
			unsigned char rtcp_channel_id, /* in (used if TCP)*/
			net_address_bits *destination_address, /* in out */
			u_int8_t *destination_ttl, /* in out */
			bool *is_multicast, /* out */
			groupsock_port_t *server_rtp_port, /* out */
			groupsock_port_t *server_rtcp_port, /* out */
			void **stream_token /* out */);
	void (*start_stream)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void *stream_token,
			task_func (*rtcp_rr_handler), void *rtcp_rr_handler_client_data,
			unsigned short *rtp_seq_num,
			unsigned int *rtp_time_stamp,
			server_request_alternative_byte_handler *server_request_alternative_byte_handler,
			void *server_request_alternative_byte_handler_client_data);
	void (*pause_stream)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void* stream_token);
	void (*seek_stream_relative)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void* stream_token, double *seek_ntp,
			double stream_duration, u_int64_t *num_bytes);
	/* This routine is used to seek by relative (i.e., NPT) time.
	 * "streamDuration", if >0.0, specifies how much data to stream, past "seekNPT".
	 * (If <=0.0, all remaining data is streamed.)
	 * "numBytes" returns the size (in bytes) of the data to be streamed,
	 * or 0 if unknown or unlimited. */
	void (*seek_stream_absolute)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void* stream_token, char **abs_start,
			char **abs_end);
	/* This routine is used to seek by 'absolute' time.
	 * "absStart" should be a string of the form
	 * 		"YYYYMMDDTHHMMSSZ" or "YYYYMMDDTHHMMSS.<frac>Z".
	 * "absEnd" should be either NULL (for no end time), 
	 *		or a string of the same form as "absStart".
	 * These strings may be modified in-place, or can be reassigned 
	 * 		to a newly-allocated value (after delete[]ing the original). */
	void (*null_seek_stream)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void *stream_token, double stream_end_time,
			u_int64_t *num_bytes);
	/* Called whenever we're handling a "PLAY" command without a specified start time.*/
	void (*set_stream_scale)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned client_session_id, void *stream_token, float scale);
	float (*get_current_npt)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			void *stream_token);
	livemedia_framed_source_t *(*get_stream_source)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			void *stream_token);
	void (*get_rtp_sink_and_rtcp)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			void *stream_token, livemedia_rtp_sink_t const **rtp_sink,
			livemedia_rtcp_instance_t const **rtcp);
	/* Returns pointers to the "RTPSink" and "RTCPInstance" objects for "streamToken".
	 * (This can be useful if you want to get the associated 
	 * 		'Groupsock' objects, for example.)
	 * You must not delete these objects, or start/stop playing them; 
	 * 		instead, that is done using the "startStream()"
	 * 		and "deleteStream()" functions. */
	void (*delete_stream)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void **stream_token);
	void (*test_scale_factor)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			float *scale);
	/* sets "scale" to the actual supported scale */
	float (*duration)(
			struct _livemedia_server_media_subsession_t *server_media_subsession);
	/* returns 0 for an unbounded session (the default)
	 * returns > 0 for a bounded session */
	void (*get_absolute_time_range)(
			struct _livemedia_server_media_subsession_t *server_media_subsession,
			char **abs_start_time, char **abs_end_time);
	/* Subclasses can reimplement this iff they support seeking by 'absolute' time. */

} livemedia_server_media_subsession_t;

/*****************************************************************
 * ServerMediaSession:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_server_media_session_t *__livemedia_server_media_session__alloc(void);
void __livemedia_server_media_session__init(
		livemedia_server_media_session_t *server_media_session, char const *stream_name,
		char const *info, char const *description, bool is_ssm,
		char const *misc_sdp_lines);
void __livemedia_server_media_session__deinit(livemedia_server_media_session_t *server_media_session);
void __livemedia_server_media_session__free(livemedia_server_media_session_t *server_media_session);
/*
 * new() and delete() functions
 */
livemedia_server_media_session_t *livemedia__new__server_media_session(
		char const *stream_name, char const *info, char const *description, bool is_ssm,
		char const *misc_sdp_line);
/*
 * Functions for virtual functions
 */
void livemedia_server_media_session__note_liveness(
		livemedia_server_media_session_t *server_media_session);

/*
 * Override functions
 */
void livemedia_server_media_session__note_liveness__impl(
		livemedia_server_media_session_t *server_media_session);
bool livemedia_server_media_session__is_server_media_session__impl(
		livemedia_medium_t *medium);
void livemedia_server_media_session__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
char const *livemedia_server_media_session__stream_name(
		livemedia_server_media_session_t *server_media_session);
unsigned int livemedia_server_media_session__num_subsessions(
		livemedia_server_media_session_t *server_media_session);
unsigned int livemedia_server_media_session__reference_count(
		livemedia_server_media_session_t *server_media_session);
void livemedia_server_media_session__increment_reference_count(
		livemedia_server_media_session_t *server_media_session);
void livemedia_server_media_session__decrement_reference_count(
		livemedia_server_media_session_t *server_media_session);
bool *livemedia_server_media_session__delete_when_unreferenced(
		livemedia_server_media_session_t *server_media_session);
/*
 * Normal functions
 */
livemedia_server_media_session_t *livemedia_server_media_session__create_new__static(
		char const *stream_name, char const *info, char const *description, bool is_ssm,
		char const *misc_sdp_lines);
bool livemedia_server_media_session__lookup_by_name__static(char const *medium_name,
		livemedia_server_media_session_t **result_session);
char *livemedia_server_media_session__generate_sdp_description(
		livemedia_server_media_session_t *server_media_session);
bool livemedia_server_media_session__add_subsession(
		livemedia_server_media_session_t *server_media_session,
		livemedia_server_media_subsession_t *subsession);
void livemedia_server_media_session__test_scale_factor(
		livemedia_server_media_session_t *server_media_session, float *scale);
float livemedia_server_media_session__duration(
		livemedia_server_media_session_t *server_media_session);
void livemedia_server_media_session__delete_all_subsessions(
		livemedia_server_media_session_t *server_media_session);


/*****************************************************************
 * ServerMediaSubsessionIterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_server_media_subsession_iterator_t *__livemedia_server_media_subsession_iterator__alloc(void);
void __livemedia_server_media_subsession_iterator__init(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator,
		livemedia_server_media_session_t *session);
void __livemedia_server_media_subsession_iterator__deinit(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);
void __livemedia_server_media_subsession_iterator__free(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);
/*
 * new() and delete() functions
 */
livemedia_server_media_subsession_iterator_t *livemedia__new__server_media_subsession_iterator(
		livemedia_server_media_session_t *session);
void livemedia__delete__server_media_subsession_iterator(livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_server_media_subsession_iterator__delete__impl(livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_server_media_subsession_t * livemedia_server_media_subsession_iterator__next(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);
void livemedia_server_media_subsession_iterator__reset(
		livemedia_server_media_subsession_iterator_t *server_media_subsession_iterator);

/*****************************************************************
 * ServerMediaSubsession:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_server_media_subsession_t *__livemedia_server_media_subsession__alloc(void);
void __livemedia_server_media_subsession__init(livemedia_server_media_subsession_t *server_media_subsession);
void __livemedia_server_media_subsession__deinit(livemedia_server_media_subsession_t *server_media_subsession);
void __livemedia_server_media_subsession__free(livemedia_server_media_subsession_t *server_media_subsession);
/*
 * new() and delete() functions
 */
livemedia_server_media_subsession_t *livemedia__new__server_media_subsession(void);
/*
 * Functions for virtual functions
 */
char const *livemedia_server_media_subsession__sdp_lines(
		livemedia_server_media_subsession_t *server_media_subsession);
void livemedia_server_media_subsession__get_stream_parameters(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, 
		net_address_bits client_address, 
		groupsock_port_t const *client_rtp_port, 
		groupsock_port_t const *client_rtcp_port, 
		int tcp_socket_num,  
		unsigned char rtp_channel_id, 
		unsigned char rtcp_channel_id, 
		net_address_bits *destination_address, 
		u_int8_t *destination_ttl, 
		bool *is_multicast, 
		groupsock_port_t *server_rtp_port, 
		groupsock_port_t *server_rtcp_port, 
		void **stream_token);
void livemedia_server_media_subsession__start_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token,
		task_func *rtcp_rr_handler, void *rtcp_rr_handler_client_data,
		unsigned short *rtp_seq_num,
		unsigned int *rtp_time_stamp,
		server_request_alternative_byte_handler *server_request_alternative_byte_handler,
		void *server_request_alternative_byte_handler_client_data);
void livemedia_server_media_subsession__pause_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token);
void livemedia_server_media_subsession__seek_stream_relative(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, double *seek_ntp,
		double stream_duration, u_int64_t *num_bytes);
void livemedia_server_media_subsession__seek_stream_absolute(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, char **abs_start,
		char **abs_end);
void livemedia_server_media_subsession__null_seek_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token, double stream_end_time,
		u_int64_t *num_bytes);
void livemedia_server_media_subsession__set_stream_scale(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned client_session_id, void *stream_token, float scale);
float livemedia_server_media_subsession__get_current_npt(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token);
livemedia_framed_source_t *livemedia_server_media_subsession__get_stream_source(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token);
void livemedia_server_media_subsession__get_rtp_sink_and_rtcp(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token, livemedia_rtp_sink_t const **rtp_sink,
		livemedia_rtcp_instance_t const **rtcp);
void livemedia_server_media_subsession__delete_stream(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void **stream_token);
void livemedia_server_media_subsession__test_scale_factor(
		livemedia_server_media_subsession_t *server_media_subsession,
		float *scale);
float livemedia_server_media_subsession__duration(
		livemedia_server_media_subsession_t *server_media_subsession);
void livemedia_server_media_subsession__get_absolute_time_range(
		livemedia_server_media_subsession_t *server_media_subsession,
		char **abs_start_time, char **abs_end_time);

/*
 * Override functions
 */
void livemedia_server_media_subsession__pause_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token);
void livemedia_server_media_subsession__seek_stream_relative__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, double *seek_ntp,
		double stream_duration, u_int64_t *num_bytes);
void livemedia_server_media_subsession__seek_stream_absolute__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, char **abs_start,
		char **abs_end);
void livemedia_server_media_subsession__null_seek_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token, double stream_end_time,
		u_int64_t *num_bytes);
void livemedia_server_media_subsession__set_stream_scale__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned client_session_id, void *stream_token, float scale);
float livemedia_server_media_subsession__get_current_npt__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token);
livemedia_framed_source_t *livemedia_server_media_subsession__get_stream_source__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token);
void livemedia_server_media_subsession__test_scale_factor__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		float *scale);
float livemedia_server_media_subsession__duration__impl(
		livemedia_server_media_subsession_t *server_media_subsession);
void livemedia_server_media_subsession__get_absolute_time_range__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		char **abs_start_time, char **abs_end_time);
void livemedia_server_media_subsession__delete_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void **stream_token);

void livemedia_server_media_subsession__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
unsigned int livemedia_server_media_subsession__track_num(
		livemedia_server_media_subsession_t *server_media_subsession);

/*
 * Normal functions
 */
char const *livemedia_server_media_subsession__track_id(
		livemedia_server_media_subsession_t *server_media_subsession);
void livemedia_server_media_subsession__set_server_address_and_port_for_sdp(
		livemedia_server_media_subsession_t *server_media_subsession,
		net_address_bits address_bits, port_num_bits port_bits);
char const *livemedia_server_media_subsession__range_sdp_line(
		livemedia_server_media_subsession_t *server_media_subsession);

#endif
