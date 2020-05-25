
#ifndef __LIVEMEDIA_RTP_SINK_H__
#define __LIVEMEDIA_RTP_SINK_H__
#include "media_sink.h"
#include "rtp_interface.h"

struct _livemedia_rtp_transmission_stats_db_t;
/*****************************************************************
 * RTPSink:MediaSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_sink_t {
	/* parent structure */
	livemedia_media_sink_t media_sink;

	/* member variables */
	livemedia_rtp_interface_t *rtp_interface;
	unsigned char rtp_payload_type;
	unsigned int packet_count;
	unsigned int octet_count;
	unsigned int total_octet_count;
	/*incl RTP hdr*/;
	struct timeval total_octet_count_start_time;
	struct timeval initial_presentation_time;
	struct timeval most_recent_presentation_time;
	u_int32_t current_timestamp;
	u_int16_t seq_no;

	u_int32_t ssrc;
	u_int32_t timestamp_base;
	unsigned int timestamp_frequency;
	bool next_timestamp_has_been_preset;
	bool enable_rtcp_reports; 
	/* whether RTCP "SR" reports should be sent for this sink (default: True) */
	char const *rtp_payload_format_name;
	unsigned int num_channels;
	struct timeval creation_time;
	unsigned int estimated_bitrate;	   /* set on creation if known; otherwise 0 */

	struct _livemedia_rtp_transmission_stats_db_t *transmission_stats_db;

	/* virtual function pointers */
	char *(*sdp_media_type)(struct _livemedia_rtp_sink_t *rtp_sink);
	/* for use in SDP m= lines */
	char *(*rtpmap_line)(struct _livemedia_rtp_sink_t *rtp_sink);
	/* returns a string to be delete[]d */
	char *(*aux_sdp_line)(struct _livemedia_rtp_sink_t *rtp_sink);
      /* optional SDP line (e.g. a=fmtp:...) */

} livemedia_rtp_sink_t;

/* forward */
struct _livemedia_rtp_transmission_stats_t;
/*****************************************************************
 * RTPTransmissionStatsDB (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_transmission_stats_db_t {
	/* member variables */
	unsigned int num_receivers;
	livemedia_rtp_sink_t *our_rtp_sink;
	livemedia_hash_table_t *table;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db);

} livemedia_rtp_transmission_stats_db_t;

/*****************************************************************
 * RTPTransmissionStats (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_transmission_stats_t {
	/*  member variables */
	livemedia_rtp_sink_t *our_rtp_sink;
	u_int32_t ssrc;
	struct sockaddr_in last_from_address;
	unsigned int last_packet_num_received;
	u_int8_t packet_loss_ratio;
	unsigned int tot_num_packets_lost;
	unsigned int jitter;
	unsigned int last_sr_time;
	unsigned int diff_sr_rr_time;
	struct timeval time_created;
	struct timeval time_received;
	bool at_least_two_rrs_have_been_received;
	unsigned int old_last_packet_num_received;
	unsigned int old_tot_num_packets_lost;
	bool first_packet;
	unsigned int first_packet_num_reported;
	u_int32_t last_octet_count;
	u_int32_t total_octet_count_hi;
	u_int32_t total_octet_count_lo;
	u_int32_t last_packet_count;
	u_int32_t total_packet_count_hi;
	u_int32_t totoal_packet_count_lo;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

} livemedia_rtp_transmission_stats_t;

/*****************************************************************
 * Iterator::RTPTransmissionStatsDB (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_transmission_stats_db_iterator_t {

	/* member variables */
	livemedia_hash_table_iterator_t *iter;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator);

} livemedia_rtp_transmission_stats_db_iterator_t;

/*****************************************************************
 * RTPSink:MediaSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_sink_t *__livemedia_rtp_sink__alloc(void);
void __livemedia_rtp_sink__init(livemedia_rtp_sink_t *rtp_sink,
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_type,
		u_int32_t rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels);
void __livemedia_rtp_sink__deinit(livemedia_rtp_sink_t *rtp_sink);
void __livemedia_rtp_sink__free(livemedia_rtp_sink_t *rtp_sink);
/*
 * new() and delete() functions
 */
livemedia_rtp_sink_t *livemedia__new__rtp_sink(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_type,
		u_int32_t rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels);
/* abstract base rtp_transmission_stats_db */
/*
 * Functions for virtual functions
 */
char *livemedia_rtp_sink__sdp_media_type(livemedia_rtp_sink_t *rtp_sink);

char *livemedia_rtp_sink__rtpmap_line(livemedia_rtp_sink_t *rtp_sink);

char *livemedia_rtp_sink__aux_sdp_line(livemedia_rtp_sink_t *rtp_sink);

/*
 * Override functions
 */
char *livemedia_rtp_sink__sdp_media_type__impl(livemedia_rtp_sink_t *rtp_sink);

char *livemedia_rtp_sink__rtpmap_line__impl(livemedia_rtp_sink_t *rtp_sink);

char *livemedia_rtp_sink__aux_sdp_line__impl(livemedia_rtp_sink_t *rtp_sink);

/*redefined virtaul functions*/
bool livemedia_rtp_sink__is_rtp_sink__impl(livemedia_media_sink_t *media_sink);

void livemedia_rtp_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
  /* used by RTSP servers: */
groupsock_groupsock_t *livemedia_rtp_sink__groupsock_being_used(livemedia_rtp_sink_t *rtp_sink);

unsigned char livemedia_rtp_sink__rtp_payload_type(livemedia_rtp_sink_t *rtp_sink);

unsigned int livemedia_rtp_sink__rtp_timestamp_frequency(livemedia_rtp_sink_t *rtp_sink);

void livemedia_rtp_sink__set_rtp_timestamp_frequency(livemedia_rtp_sink_t *rtp_sink, unsigned int freq);

char const *livemedia_rtp_sink__rtp_payload_format_name(livemedia_rtp_sink_t *rtp_sink);

unsigned int livemedia_rtp_sink__num_channels(livemedia_rtp_sink_t *rtp_sink);

u_int16_t livemedia_rtp_sink__current_seq_no(livemedia_rtp_sink_t *rtp_sink);

struct _livemedia_rtp_transmission_stats_db_t *livemedia_rtp_sink__transmission_stats_db(livemedia_rtp_sink_t *rtp_sink);

bool livemedia_rtp_sink__next_timestamp_has_been_preset(livemedia_rtp_sink_t *rtp_sink);

bool *livemedia_rtp_sink__enable_rtcp_reports(livemedia_rtp_sink_t *rtp_sink);

struct timeval const* livemedia_rtp_sink__creation_time(livemedia_rtp_sink_t *rtp_sink);

struct timeval const* livemedia_rtp_sink__initial_presentation_time(livemedia_rtp_sink_t *rtp_sink);

struct timeval const* livemedia_rtp_sink__most_recent_presentation_time(livemedia_rtp_sink_t *rtp_sink);

  /* Hacks to allow sending RTP over TCP (RFC 2236, section 10.12): */
void livemedia_rtp_sink__set_stream_socket(livemedia_rtp_sink_t *rtp_sink,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);

void livemedia_rtp_sink__add_stream_socket(livemedia_rtp_sink_t *rtp_sink,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);

void livemedia_rtp_sink__remove_stream_socket(livemedia_rtp_sink_t *rtp_sink,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);

unsigned int *livemedia_rtp_sink__estimated_bitrate(livemedia_rtp_sink_t *rtp_sink);

u_int32_t livemedia_rtp_sink__ssrc(livemedia_rtp_sink_t *rtp_sink);
     /* later need a means of changing the SSRC if there's a collision ##### */

u_int32_t livemedia_rtp_sink__convert_to_rtp_timestamp(livemedia_rtp_sink_t *rtp_sink,
		struct timeval tv);

unsigned int livemedia_rtp_sink__packet_count(livemedia_rtp_sink_t *rtp_sink);

unsigned int livemedia_rtp_sink__octet_count(livemedia_rtp_sink_t *rtp_sink);
/*
 * Normal functions
 */

bool livemedia_rtp_sink__lookup_by_name__static(char const *sink_name, livemedia_rtp_sink_t **rtp_sink);
u_int32_t livemedia_rtp_sink__preset_next_time_stamp(livemedia_rtp_sink_t *rtp_sink);
      /* ensures that the next timestamp to be used will correspond to
       * the current 'wall clock' time. */

void livemedia_rtp_sink__get_total_bitrate(livemedia_rtp_sink_t *rtp_sink,
		unsigned int *out_num_bytes, double *out_elapsed_time);
      /* returns the number of bytes sent since the last time that we
       * were called, and resets the counter. */

void livemedia_rtp_sink__reset_presentation_times(livemedia_rtp_sink_t *rtp_sink);


/*****************************************************************
 * RTPTransmissionStatsDB (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_transmission_stats_db_t *__livemedia_rtp_transmission_stats_db__alloc(void);
void __livemedia_rtp_transmission_stats_db__init(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db,
		livemedia_rtp_sink_t *rtp_sink);
void __livemedia_rtp_transmission_stats_db__deinit(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db);
void __livemedia_rtp_transmission_stats_db__free(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db);
/*
 * new() and delete() functions
 */
livemedia_rtp_transmission_stats_db_t *livemedia__new__rtp_transmission_stats_db(livemedia_rtp_sink_t *rtp_sink);
void livemedia__delete__rtp_transmission_stats_db(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_transmission_stats_db__delete__impl(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db);

/*
 * Functions in header file
 */
unsigned int livemedia_rtp_transmission_stats_db__num_receivers(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db);

/*
 * Normal functions
 */

/* The following is called whenever a RTCP RR packet is received: */
void livemedia_rtp_transmission_stats_db__note_incoming_rr(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db,
		u_int32_t ssrc, struct sockaddr_in const *last_from_address,
		unsigned int loss_stats, unsigned int last_packet_num_received,
		unsigned int jitter, unsigned int last_sr_time, unsigned int diff_sr_rr_time);
  /* The following is called when a RTCP BYE packet is received: */
void livemedia_rtp_transmission_stats_db__remove_record(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db, u_int32_t ssrc);
struct _livemedia_rtp_transmission_stats_t livemedia_rtp_transmission_stats_db__lookup(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db, u_int32_t ssrc);

void livemedia_rtp_transmission_stats_db__add(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db, 
		u_int32_t ssrc, struct _livemedia_rtp_transmission_stats_t stats);

/*****************************************************************
 * RTPTransmissionStats (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_transmission_stats_t *__livemedia_rtp_transmission_stats__alloc(void);
void __livemedia_rtp_transmission_stats__init(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		livemedia_rtp_sink_t *rtp_sink, u_int32_t ssrc);
void __livemedia_rtp_transmission_stats__deinit(livemedia_rtp_transmission_stats_t *rtp_transmission_stats);
void __livemedia_rtp_transmission_stats__free(livemedia_rtp_transmission_stats_t *rtp_transmission_stats);
/*
 * new() and delete() functions
 */
livemedia_rtp_transmission_stats_t *livemedia__new__rtp_transmission_stats(
		livemedia_rtp_sink_t *rtp_sink, u_int32_t ssrc);
void livemedia__delete__rtp_transmission_stats(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_transmission_stats__delete__impl(livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

/*
 * Functions in header file
 */
u_int32_t livemedia_rtp_transmission_stats__ssrc(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

struct sockaddr_in *livemedia_rtp_transmission_stats__last_from_address(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__last_packet_num_received(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__first_packet_num_reported(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__tot_num_packet_lost(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__jitter(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__last_sr_time(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__diff_sr_rr_time(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

unsigned int livemedia_rtp_transmission_stats__round_trip_delay(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);
      /* The round-trip delay (in units of 1/65536 seconds) computed from
       * the most recently-received RTCP RR packet. */

struct timeval livemedia_rtp_transmission_stats__time_created(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

struct timeval livemedia_rtp_transmission_stats__last_time_received(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

u_int8_t livemedia_rtp_transmission_stats__packet_loss_ratio(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);
     /* as an 8-bit fixed-point number*/
int livemedia_rtp_transmission_stats__packet_lost_between_rr(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

/*
 * Normal functions
 */

struct timeval livemedia_rtp_transmission_stats__get_total_octet_count(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		u_int32_t *hi, u_int32_t *lo);

struct timeval livemedia_rtp_transmission_stats__get_total_packet_count(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		u_int32_t *hi, u_int32_t *lo);
  /* Information which requires at least two RRs to have been received: */
unsigned int livemedia_rtp_transmission_stats__packet_received_since_last_rr(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats);

void livemedia_rtp_transmission_stats__note_incoming_rr(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		struct sockaddr_in const *last_from_address,
		unsigned int loss_stats, unsigned int last_packet_num_received,
		unsigned int jitter,
		unsigned int last_sr_time, unsigned int diff_sr_rr_time);

/*****************************************************************
 * Iterator::RTPTransmissionStatsDB (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_transmission_stats_db_iterator_t *__livemedia_rtp_transmission_stats_db_iterator__alloc(void);
void __livemedia_rtp_transmission_stats_db_iterator__init(
		livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator,
		livemedia_rtp_transmission_stats_db_t *reception_stats_db);
void __livemedia_rtp_transmission_stats_db_iterator__deinit(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator);
void __livemedia_rtp_transmission_stats_db_iterator__free(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator);
/*
 * new() and delete() functions
 */
livemedia_rtp_transmission_stats_db_iterator_t *livemedia__new__rtp_transmission_stats_db_iterator(
		livemedia_rtp_transmission_stats_db_t *reception_stats_db);
void livemedia__delete__rtp_transmission_stats_db_iterator(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_transmission_stats_db_iterator__delete__impl(
		livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_rtp_transmission_stats_t *livemedia_rtp_transmission_stats_db_iterator__next(
		livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator);
        /* NULL if none */

#endif
