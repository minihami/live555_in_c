
#ifndef __LIVEMEDIA_ON_DEMAND_SERVER_MEDIA_SUBSESSION_H__
#define __LIVEMEDIA_ON_DEMAND_SERVER_MEDIA_SUBSESSION_H__


#include "server_media_session.h"
#include "rtp_sink.h"
#include "basic_udp_sink.h"
#include "rtcp.h"

/*****************************************************************
 * OnDemandServerMediaSubsession:ServerMediaSubsession (Structure Declarations)
 ****************************************************************/
/* we're a virtual base class */
typedef struct _livemedia_on_demand_server_media_subsession_t {
	livemedia_server_media_subsession_t server_media_subsession;

	/* member variables */
	char *sdp_lines;
	livemedia_hash_table_t *destination_hash_table; /* indexed by client session id */
	bool reuse_first_source;
	port_num_bits  initial_port_num;
	bool multiplex_rtcp_with_rtp;
	void *last_stream_token;
	char cname[100]; /* for RTCP */
	livemedia__rtcp_app_handler_func *app_handler_task;
	void * app_handler_client_data;


	/* virtual function pointers */
	/* new virtual functions, possibly redefined by subclasses */
	char *(*get_aux_sdp_line)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source);
	void (*seek_stream_source_by_relative)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			livemedia_framed_source_t *input_source, double *seek_npt, 
			double stream_duration, u_int64_t *num_bytes);
	void (*seek_stream_source_by_absolute)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			livemedia_framed_source_t *input_source, char **abs_start, char **abs_end);
	void (*set_stream_source_scale)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			livemedia_framed_source_t *input_source, float scale);
	void (*set_stream_source_duration)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			livemedia_framed_source_t *input_source, double stream_duration, u_int64_t *num_bytes);
	void (*close_stream_source)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			livemedia_framed_source_t *input_source);
	/* new virtual functions, defined by all subclasses */
	livemedia_framed_source_t *(*create_new_stream_source)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			unsigned int client_session_id, unsigned int *est_bitrate);
	livemedia_rtp_sink_t *(*create_new_rtp_sink)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			groupsock_groupsock_t *rtp_groupsock, unsigned char rtp_payload_type_if_dynamic,
			livemedia_framed_source_t *input_source);
	/* new virtual functions, may be redefined by a subclass: */
	groupsock_groupsock_t *(*create_groupsock)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			struct in_addr *addr, groupsock_port_t port);
	livemedia_rtcp_instance_t *(*create_rtcp)(
			struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
			groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw, /* in kbps */
			unsigned char const *cname, livemedia_rtp_sink_t *sink);

} livemedia_on_demand_server_media_subsession_t;

/*****************************************************************
 * Destinations(Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_destinations_t {
	/* member variables */
	bool is_tcp;
	struct in_addr addr;
	groupsock_port_t rtp_port;
	groupsock_port_t rtcp_port;
	uv_tcp_t *tcp_socket_num;
	unsigned char rtp_channel_id;
	unsigned char rtcp_channel_id;
	/* virtual function pointers */
	void (*delete)(struct _livemedia_destinations_t *destinations);

} livemedia_destinations_t;

/*****************************************************************
 * StreamState(Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_stream_state_t {
	/* member variables */
	livemedia_on_demand_server_media_subsession_t *master;
	bool are_currently_playing;
	unsigned int reference_count;

	groupsock_port_t server_rtp_port;
	groupsock_port_t server_rtcp_port;

	livemedia_rtp_sink_t *rtp_sink;
	livemedia_basic_udp_sink_t *udp_sink;

	float stream_duration;
	unsigned int total_bw;
	livemedia_rtcp_instance_t *rtcp_instance;

	livemedia_framed_source_t *media_source;
	float start_npt; /* initial 'normal play time'; reset after each seek */

	groupsock_groupsock_t *rtp_gs;
	groupsock_groupsock_t *rtcp_gs;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_stream_state_t *stream_state);

} livemedia_stream_state_t;


/*****************************************************************
 * OnDemandServerMediaSubsession:ServerMediaSubsession (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_on_demand_server_media_subsession_t *__livemedia_on_demand_server_media_subsession__alloc(void);
void __livemedia_on_demand_server_media_subsession__init(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		bool reuse_first_source,
		port_num_bits initial_port_num,
		bool multiplex_rtcp_with_rtp);

void __livemedia_on_demand_server_media_subsession__deinit(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession);

void __livemedia_on_demand_server_media_subsession__free(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession);
/*
 * new() and delete() functions
 */
livemedia_on_demand_server_media_subsession_t *livemedia__new__on_demand_server_media_subsession(
		bool reuse_first_source,
		port_num_bits initial_port_num,
		bool multiplex_rtcp_with_rtp);
/*
 * Functions for virtual functions
 */
char *livemedia_on_demand_server_media_subsession__get_aux_sdp_line(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source);

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_relative(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double *seek_npt, 
		double stream_duration, u_int64_t *num_bytes);
/* This routine is used to seek by relative (i.e., NPT) time.
 * "streamDuration", if >0.0, specifies how much data to stream, past "seekNPT".  (If <=0.0, all remaining data is streamed.)
 * "numBytes" returns the size (in bytes) of the data to be streamed, or 0 if unknown or unlimited. */

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_absolute(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, char **abs_start, char **abs_end);
/* This routine is used to seek by 'absolute' time.
 * "absStart" should be a string of the form "YYYYMMDDTHHMMSSZ" or "YYYYMMDDTHHMMSS.<frac>Z".
 * "absEnd" should be either NULL (for no end time), or a string of the same form as "absStart".
 * These strings may be modified in-place, or can be reassigned to a newly-allocated value (after delete[]ing the original). */

void livemedia_on_demand_server_media_subsession__set_stream_source_scale(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, float scale);

void livemedia_on_demand_server_media_subsession__set_stream_source_duration(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double stream_duration, u_int64_t *num_bytes);

void livemedia_on_demand_server_media_subsession__close_stream_source(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source);

livemedia_framed_source_t *livemedia_on_demand_server_media_subsession__create_new_stream_source(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		unsigned int client_session_id, unsigned int *est_bitrate);
/* "estBitrate" is the stream's estimated bitrate, in kbps */

livemedia_rtp_sink_t *livemedia_on_demand_server_media_subsession__create_new_rtp_sink(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtp_groupsock, unsigned char rtp_payload_type_if_dynamic,
		livemedia_framed_source_t *input_source);

groupsock_groupsock_t *livemedia_on_demand_server_media_subsession__create_groupsock(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		struct in_addr *addr, groupsock_port_t port);

livemedia_rtcp_instance_t *livemedia_on_demand_server_media_subsession__create_rtcp(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw, /* in kbps */
		unsigned char const *cname, livemedia_rtp_sink_t *sink);

/*
 * Override functions
 */
char const *livemedia_on_demand_server_media_subsession__sdp_lines__impl(
			livemedia_server_media_subsession_t *server_media_subsession);

void livemedia_on_demand_server_media_subsession__get_stream_parameters__impl(
			livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, /* in */
			net_address_bits client_address, /* in */
			groupsock_port_t *client_rtp_port, /* in */
			groupsock_port_t *client_rtcp_port, /* in */
			uv_tcp_t *tcp_socket_num,  /* in (-1 means use UDP, not TCP) */
			unsigned char rtp_channel_id, /* in (used if TCP)*/
			unsigned char rtcp_channel_id, /* in (used if TCP)*/
			net_address_bits *destination_address, /* in out */
			u_int8_t *destination_ttl, /* in out */
			bool *is_multicast, /* out */
			groupsock_port_t *server_rtp_port, /* out */
			groupsock_port_t *server_rtcp_port, /* out */
			void **stream_token /* out */);

void livemedia_on_demand_server_media_subsession__start_stream__impl(
			livemedia_server_media_subsession_t *server_media_subsession,
			unsigned int client_session_id, void *stream_token,
			usage_environment__task_func *rtcp_rr_handler, void *rtcp_rr_handler_client_data,
			unsigned short *rtp_seq_num,
			unsigned int *rtp_timestamp,
			livemedia__server_request_alternative_byte_handler *server_request_alternative_byte_handler,
			void *server_request_alternative_byte_handler_client_data);

void livemedia_on_demand_server_media_subsession__pause_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token);

void livemedia_on_demand_server_media_subsession__seek_stream_relative__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, double *seek_npt,
		double stream_duration, u_int64_t *num_bytes);

void livemedia_on_demand_server_media_subsession__seek_stream_absolute__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, char **abs_start,
		char **abs_end);

void livemedia_on_demand_server_media_subsession__null_seek_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token, double stream_end_time,
		u_int64_t *num_bytes);

void livemedia_on_demand_server_media_subsession__set_stream_scale__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned client_session_id, void *stream_token, float scale);

float livemedia_on_demand_server_media_subsession__get_current_npt__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token);

livemedia_framed_source_t *livemedia_on_demand_server_media_subsession__get_stream_source(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token);

void livemedia_on_demand_server_media_subsession__get_rtp_sink_and_rtcp__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token, livemedia_rtp_sink_t const **rtp_sink,
		livemedia_rtcp_instance_t const **rtcp);

void livemedia_on_demand_server_media_subsession__delete_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void **stream_token);

char *livemedia_on_demand_server_media_subsession__get_aux_sdp_line__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source);

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_relative__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double *seek_npt, 
		double stream_duration, u_int64_t *num_bytes);

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_absolute__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, char **abs_start, char **abs_end);

void livemedia_on_demand_server_media_subsession__set_stream_source_scale__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, float scale);

void livemedia_on_demand_server_media_subsession__set_stream_source_duration__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double stream_duration, u_int64_t *num_bytes);

void livemedia_on_demand_server_media_subsession__close_stream_source__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source);

livemedia_framed_source_t *livemedia_on_demand_server_media_subsession__create_new_stream_source__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		unsigned int client_session_id, unsigned int *est_bitrate);

groupsock_groupsock_t *livemedia_on_demand_server_media_subsession__create_groupsock__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		struct in_addr *addr, groupsock_port_t port);

livemedia_rtcp_instance_t *livemedia_on_demand_server_media_subsession__create_rtcp__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw, /* in kbps */
		unsigned char const *cname, livemedia_rtp_sink_t *sink);


void livemedia_on_demand_server_media_subsession__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

void livemedia_on_demand_server_media_subsession__multiplex_rtcp_with_rtp(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession);
/* An alternative to passing the "multiplexRTCPWithRTP" parameter as True in the constructor */

/*
 * Normal functions
 */
void livemedia_on_demand_server_media_subsession__set_rtcp_app_packet_handler(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia__rtcp_app_handler_func *handler, void *client_data);
/* Sets a handler to be called if a RTCP "APP" packet arrives from any future client.
 * (Any current clients are not affected; any "APP" packets from them will continue to be
 * handled by whatever handler existed when the client sent its first RTSP "PLAY" command.)
 * (Call with (NULL, NULL) to remove an existing handler - for future clients only) */

void livemedia_on_demand_server_media_subsession__send_rtcp_app_packet(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		u_int8_t subtype, char const *name,
		uint8_t *app_dependent_data, unsigned int app_dependent_data_size);
/* Sends a custom RTCP "APP" packet to the most recent client (if "reuseFirstSource" was False),
 * or to all current clients (if "reuseFirstSource" was True).
 * The parameters correspond to their
 * respective fields as described in the RTP/RTCP definition (RFC 3550).
 * Note that only the low-order 5 bits of "subtype" are used, and only the first 4 bytes
 * of "name" are used.  (If "name" has fewer than 4 bytes, or is NULL,
 * then the remaining bytes are '\0'.) */

void livemedia_on_demand_server_media_subsession__set_sdp_lines_from_rtp_sink(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source,
		unsigned int est_bitrate);
/* used to implement "sdpLines()" */

/*****************************************************************
 * Destinations(Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_destinations_t *__livemedia_destinations__alloc(void);

void __livemedia_destinations__init_by_dest_addr(livemedia_destinations_t *destinations,
		struct in_addr const *dest_addr,
		groupsock_port_t const *rtp_dest_port,
		groupsock_port_t const *rtcp_dest_port);

void __livemedia_destinations__init_by_tcp_sock_num(livemedia_destinations_t *destinations,
		uv_tcp_t *tcp_sock_num,
		unsigned char rtp_chan_id,
		unsigned char rtcp_chan_id);

void __livemedia_destinations__deinit(livemedia_destinations_t *destinations);

void __livemedia_destinations__free(livemedia_destinations_t *destinations);
/*
 * new() and delete() functions
 */
livemedia_destinations_t *livemedia__new__destinations_by_dest_addr(
		struct in_addr const *dest_addr,
		groupsock_port_t const *rtp_dest_port,
		groupsock_port_t const *rtcp_dest_port);
livemedia_destinations_t *livemedia__new__destinations_by_tcp_sock_num(
		uv_tcp_t *tcp_sock_num,
		unsigned char rtp_chan_id,
		unsigned char rtcp_chan_id);
void livemedia__delete__destinations(livemedia_destinations_t *destinations);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_destinations__delete__impl(livemedia_destinations_t *destinations);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * StreamState(Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_stream_state_t *__livemedia_stream_state__alloc(void);

void __livemedia_stream_state__init(livemedia_stream_state_t *stream_state,
		livemedia_on_demand_server_media_subsession_t *master,
		groupsock_port_t const *server_rtp_port, groupsock_port_t const *server_rtcp_port,
		livemedia_rtp_sink_t *rtp_sink, livemedia_basic_udp_sink_t *udp_sink,
		unsigned int total_bw, livemedia_framed_source_t *media_source,
		groupsock_groupsock_t *rtp_gs, groupsock_groupsock_t *rtcp_gs);

void __livemedia_stream_state__deinit(livemedia_stream_state_t *stream_state);

void __livemedia_stream_state__free(livemedia_stream_state_t *stream_state);

/*
 * new() and delete() functions
 */
livemedia_stream_state_t *livemedia__new__stream_state(
		livemedia_on_demand_server_media_subsession_t *master,
		groupsock_port_t const *server_rtp_port, groupsock_port_t const *server_rtcp_port,
		livemedia_rtp_sink_t *rtp_sink, livemedia_basic_udp_sink_t *udp_sink,
		unsigned int total_bw, livemedia_framed_source_t *media_source,
		groupsock_groupsock_t *rtp_gs, groupsock_groupsock_t *rtcp_gs);

void livemedia__delete__stream_state(livemedia_stream_state_t *stream_state);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_stream_state__delete__impl(livemedia_stream_state_t *stream_state);

/*
 * Functions in header file
 */
unsigned int *livemedia_stream_state__reference_count(livemedia_stream_state_t *stream_state);
groupsock_port_t const *livemedia_stream_state__server_rtp_port(
		livemedia_stream_state_t *stream_state);

groupsock_port_t const *livemedia_stream_state__server_rtcp_port(
		livemedia_stream_state_t *stream_state);

livemedia_rtp_sink_t *livemedia_stream_state__rtp_sink(
		livemedia_stream_state_t *stream_state);

livemedia_rtcp_instance_t *livemedia_stream_state__rtcp_instance(
		livemedia_stream_state_t *stream_state);

float livemedia_stream_state__stream_duration(livemedia_stream_state_t *stream_state);

livemedia_framed_source_t *livemedia_stream_state__media_source(
		livemedia_stream_state_t *stream_state);

float *livemedia_stream_state__start_npt(livemedia_stream_state_t *stream_state);
/*
 * Normal functions
 */
void livemedia_stream_state__start_playing(livemedia_stream_state_t *stream_state,
		livemedia_destinations_t *dests, unsigned int client_session_id,
		usage_environment__task_func *rtcp_rr_handler, void *rtcp_rr_handler_client_data,
		livemedia__server_request_alternative_byte_handler *server_request_alternative_byte_handler,
		void *server_request_alternative_byte_handler_client_data);

void livemedia_stream_state__pause(livemedia_stream_state_t *stream_state);

void livemedia_stream_state__send_rtcp_app_packet(livemedia_stream_state_t *stream_state,
		u_int8_t subtype, char const *name, 
		u_int8_t *app_dependent_data, unsigned int app_dependent_data_size);

void livemedia_stream_state__end_playing(livemedia_stream_state_t *stream_state,
		livemedia_destinations_t *destinations, unsigned int client_session_id);

void livemedia_stream_state__reclaim(livemedia_stream_state_t *stream_state);

#endif
