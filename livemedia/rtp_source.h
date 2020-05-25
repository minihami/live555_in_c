#ifndef __LIVEMEDIA_RTP_SOURCE_H__
#define __LIVEMEDIA_RTP_SOURCE_H__

#ifndef __LIVEMEDIA_FRAMED_SOURCE_H__
#include "framed_source.h"
#endif
#ifndef __LIVEMEDIA_RTP_INTERFACE_H__
#include "rtp_interface.h"
#endif
#ifndef __LIVEMEDIA_SRTP_CRYPTOGRAPHIC_CONTEXT_H__
#include "srtp_cryptographic_context.h"
#endif

struct _livemedia_rtp_reception_stats_db_t;
struct _livemedia_rtcp_instance_t;
/*****************************************************************
 * RTPSource:FramedSource (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_source_t {
	livemedia_framed_source_t framed_source;

	livemedia_rtp_interface_t rtp_interface;
	u_int16_t cur_packet_rtp_seq_num;
	u_int32_t cur_packet_rtp_time_stamp;
	bool cur_packet_marker_bit;
	bool cur_packet_has_been_synchronized_using_rtcp;
	u_int32_t last_received_ssrc;
	struct _livemedia_rtcp_instance_t *rtcp_instance_for_multiplexed_rtcp_packets;
	livemedia_srtp_cryptographic_context_t *crypto;
	unsigned char rtp_payload_format;
	unsigned timestamp_frequency;
	u_int32_t ssrc;
	bool enable_rtcp_reports; /* whether RTCP "RR" reports should be sent for this source (default: True) */
	struct _livemedia_rtp_reception_stats_db *reception_stats_db;

	bool (*has_been_synchronized_using_rtcp)(struct _livemedia_rtp_source_t *rtp_source);
	void (*set_packet_reordering_threshold_time)(struct _livemedia_rtp_source_t *rtp_source,
			unsigned int u_seconds);
	void (*delete)(struct _livemedia_rtp_source_t *rtp_source);
} livemedia_rtp_source_t;

struct _livemedia_rtp_reception_stats_t;
/*****************************************************************
 * RTPReceptionStatsDB (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_reception_stats_db_t {
	unsigned int num_active_sources_since_last_reset;
	livemedia_hash_table_t *table;
	unsigned tot_num_packet_received; /* for all SSRCs */

	void (*delete)(struct _livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
} livemedia_rtp_reception_stats_db_t;

/*****************************************************************
 * RTPReceptionStatsDB::Iterator (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_reception_stats_db_iterator_t {
	livemedia_hash_table_iterator_t *iter;

	void (*delete)(struct _livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator);
} livemedia_rtp_reception_stats_db_iterator_t;

/*****************************************************************
 * RTPReceptionStats (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_reception_stats_t {
	u_int32_t ssrc;
	unsigned int num_packets_received_since_last_reset;
	unsigned int tot_num_packets_received;
	u_int32_t tot_bytes_received_hi, tot_bytes_received_lo;
	bool have_seen_initial_sequence_number;
	unsigned int base_ext_seq_num_received;
	unsigned int last_reset_ext_seq_num_received;
	unsigned int highest_ext_seq_num_received;
	int last_transit; /*used in the hitter calculation */
	u_int32_t previous_packet_rtp_timestamp;
	double jitter;
	/* The following are recorded whenever we receive a RTCP SR for this SSRC: */
	unsigned int last_received_sr_ntp_msw; /* NTP timestamp (from SR), most-signif */
	unsigned int last_received_sr_ntp_lsw; /* NTP timestamp (from SR), least-signif */
	struct timeval last_received_sr_time;
	struct timeval last_packet_reception_time;
	unsigned int min_inter_packet_gap_us, max_inter_packet_gap_us;
	struct timeval total_inter_packet_gaps;
	/* Used to convert from RTP timestamp to 'wall clock' time: */
	bool has_been_synchronized;
	u_int32_t sync_timestamp;
	struct timeval sync_time;

	void (*delete)(struct _livemedia_rtp_reception_stats_t *rtp_reception_stats_t);
} livemedia_rtp_reception_stats_t;

/*****************************************************************
 * RTPSource:FramedSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_source_t *__livemedia_rtp_source__alloc(void);
void __livemedia_rtp_source__init(livemedia_rtp_source_t *rtp_source,
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int32_t rtp_timestamp_frequency);
void __livemedia_rtp_source__deinit(livemedia_rtp_source_t *rtp_source);
void __livemedia_rtp_source__free(livemedia_rtp_source_t *rtp_source);
/*
 * new() and delete() functions
 */
livemedia_rtp_source_t *livemedia__new__rtp_source(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int32_t rtp_timestamp_frequency);
void livemedia__delete__rtp_source(livemedia_rtp_source_t *rtp_source);
/*
 * Functions for virtual functions
 */

bool livemedia_rtp_source__has_been_synchronized_using_rtcp(struct _livemedia_rtp_source_t *rtp_source);
void livemedia_rtp_source__set_packet_reordering_threshold_time(
		struct _livemedia_rtp_source_t *rtp_source, unsigned int u_seconds);

/*
 * Override functions
 */
bool livemedia_rtp_source__has_been_synchronized_using_rtcp__impl(
		livemedia_rtp_source_t *rtp_source);
/* redefined virtual functions: */
bool livemedia_rtp_source__is_rtp_source__impl(livemedia_media_source_t *media_source);
void livemedia_rtp_source__get_attributes__impl(livemedia_media_source_t *media_source);

void livemedia_rtp_source__delete__impl(livemedia_rtp_source_t *rtp_source);

/*
 * Functions in header file
 */
bool livemedia_rtp_source__cur_packet_maker_bit(livemedia_rtp_source_t *rtp_source);
unsigned char livemedia_rtp_source__rtp_payload_format(livemedia_rtp_source_t *rtp_source);
groupsock_groupsock_t *livemedia_rtp_source__rtp_gs(livemedia_rtp_source_t *rtp_source);
void livemedia_groupsock__set_crypto(livemedia_rtp_source_t *rtp_source,
		livemedia_srtp_cryptographic_context_t *crypto);
/* used by RTCP: */
u_int32_t livemedia_rtp_source__ssrc(livemedia_rtp_source_t *rtp_source);
/* Note: This is *our* SSRC, not the SSRC in incoming RTP packets.
 * later need a means of changing the SSRC if there's a collision ##### */
void livemedia_rtp_source__register_for_multiplexed_rtcp_packets(
		livemedia_rtp_source_t *rtp_source, struct _livemedia_rtcp_instance_t *rtcp_instance);
void livemedia_rtp_source__deregister_for_multiplexed_rtcp_packets(
		livemedia_rtp_source_t *rtp_source);
unsigned int livemedia_rtp_source__timestamp_frequency(livemedia_rtp_source_t *rtp_source);
struct _livemedia_rtp_reception_stats_db_t *livemedia_rtp_source__reception_stats_db(
		livemedia_rtp_source_t *rtp_source);
u_int32_t livemedia_rtp_source__last_received_ssrc(livemedia_rtp_source_t *rtp_source);
/* Note: This is the SSRC in the most recently received RTP packet; not *our* SSRC */
bool *livemedia_rtp_source__enable_rtcp_reports(livemedia_rtp_source_t *rtp_source);
void livemedia_rtp_source__set_stream_socket(livemedia_rtp_source_t *rtp_source,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);
void livemedia_rtp_source__set_auxilliary_read_handler(livemedia_rtp_source_t *rtp_source,
		livemedia__aux_handler_func *handler_func, void *handler_client_data);
/* Note that RTP receivers will usually not need to call either of the following two functions, because
 * RTP sequence numbers and timestamps are usually not useful to receivers.
 * (Our implementation of RTP reception already does all needed handling of RTP sequence numbers and timestamps.) */
u_int16_t livemedia_rtp_source_cur_packet_rtp_seq_num(livemedia_rtp_source_t *rtp_source);
u_int16_t livemedia_rtp_source_cur_packet_rtp_timestamp(livemedia_rtp_source_t *rtp_source);


/*
 * Normal functions
 */
bool livemedia_rtp_source__lookup_by_name__static(char const *source_name,
		livemedia_rtp_source_t **result_source);

/*****************************************************************
 * RTPReceptionStatsDB (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_reception_stats_db_t *__livemedia_rtp_reception_stats_db__alloc(void);
void __livemedia_rtp_reception_stats_db__init(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
void __livemedia_rtp_reception_stats_db__deinit(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
void __livemedia_rtp_reception_stats_db__free(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
/*
 * new() and delete() functions
 */
livemedia_rtp_reception_stats_db_t *livemedia__new__rtp_reception_stats_db(void);
void livemedia__delete__rtp_reception_stats_db(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_reception_stats_db__delete__impl(livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);

/*
 * Functions in header file
 */
unsigned int livemedia_rtp_reception_stats_db__tot_num_packets_received(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
unsigned int livemedia_rtp_reception_stats_db__num_active_sources_since_last_reset(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);

/*
 * Normal functions
 */
void livemedia_rtp_reception_stats_db__reset(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db);
/* The following is called whenever a RTP packet is received: */
void livemedia_rtp_reception_stats_db__note_incoming_packet(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc, u_int16_t seq_num,
		u_int32_t rtp_timestamp,
		unsigned int timestamp_frequency,
		bool use_for_jitter_calculation,
		struct timeval *result_presentation_time,
		bool *result_has_been_synced_using_rtcp,
		unsigned packet_size /* payload only */);
/* The following is called whenever a RTCP SR packet is received: */
void livemedia_rtp_reception_stats_db__note_incoming_sr(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ntp_timestamp_msw, u_int32_t ntp_timestamp_lsw,
		u_int32_t rtp_timestamp);
/* The following is called when a RTCP BYE packet is received: */
void livemedia_rtp_reception_stats_db__remove_record(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc);
struct _livemedia_rtp_reception_stats_t *livemedia_rtp_reception_stats_db__lookup(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc);
void livemedia_rtp_reception_stats_db__add(
		livemedia_rtp_reception_stats_db_t *rtp_reception_stats_db,
		u_int32_t ssrc, struct _livemedia_rtp_reception_stats_t *stats);

/*****************************************************************
 * RTPReceptionStatsDB::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_reception_stats_db_iterator_t *__livemedia_rtp_reception_stats_db_iterator__alloc(void);
void __livemedia_rtp_reception_stats_db_iterator__init(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator,
		livemedia_rtp_reception_stats_db_t *reception_stats_db);
void __livemedia_rtp_reception_stats_db_iterator__deinit(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator);
void __livemedia_rtp_reception_stats_db_iterator__free(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator);
/*
 * new() and delete() functions
 */
livemedia_rtp_reception_stats_db_iterator_t *livemedia__new__rtp_reception_stats_db_iterator(
		livemedia_rtp_reception_stats_db_t *reception_stats_db);
void livemedia__delete__rtp_reception_stats_db_iterator(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_reception_stats_db_iterator__delete__impl(livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
struct _livemedia_rtp_reception_stats_t *livemedia_rtp_reception_stats_db_iterator__next(
		livemedia_rtp_reception_stats_db_iterator_t *rtp_reception_stats_db_iterator,
		bool include_inactive_sources);

/*****************************************************************
 * RTPReceptionStats (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_reception_stats_t *__livemedia_rtp_reception_stats__alloc(void);
void __livemedia_rtp_reception_stats__init(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int32_t ssrc);
void __livemedia_rtp_reception_stats__init_by_initial_seq_num(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int32_t ssrc, u_int16_t initial_seq_num);
void __livemedia_rtp_reception_stats__deinit(livemedia_rtp_reception_stats_t *rtp_reception_stats);
void __livemedia_rtp_reception_stats__free(livemedia_rtp_reception_stats_t *rtp_reception_stats);
/*
 * new() and delete() functions
 */
livemedia_rtp_reception_stats_t *livemedia__new__rtp_reception_stats(u_int32_t ssrc);
livemedia_rtp_reception_stats_t *livemedia__new__rtp_reception_stats_by_initial_seq_num(u_int32_t ssrc, u_int16_t initial_seq_num);
void livemedia__delete__rtp_reception_stats(livemedia_rtp_reception_stats_t *rtp_reception_stats);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_reception_stats__delete__impl(livemedia_rtp_reception_stats_t *rtp_reception_stats);

/*
 * Functions in header file
 */
u_int32_t livemedia_rtp_reception_stats__ssrc(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__num_packets_received_since_last_reset(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__tot_num_packets_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__tot_num_packets_expected(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__base_ext_seq_num_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__last_reset_ext_seq_num_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__highest_ext_seq_num_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__last_received_sr_ntp_msw(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__last_received_sr_ntp_lsw(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
struct timeval const *livemedia_rtp_reception_stats__last_received_sr_time(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__min_inter_packet_gap_us(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__max_inter_packet_gap_us(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);

/*
 * Normal functions
 */
double livemedia_rtp_reception_stats__tot_num_bytes_received(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
unsigned int livemedia_rtp_reception_stats__jitter(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
void livemedia_rtp_reception_stats__note_incoming_packet(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int16_t seq_num, u_int32_t rtp_timestamp,
		unsigned int timestamp_frequency,
		bool use_for_jitter_calculation,
		struct timeval *result_presentaion_time,
		bool *result_has_been_synced_using_rtcp,
		unsigned int packet_size /* payload only */);
void livemedia_rtp_reception_stats__note_incoming_sr(
		livemedia_rtp_reception_stats_t *rtp_reception_stats,
		u_int32_t ntp_timestamp_msw, u_int32_t ntp_timestamp_lsw,
		u_int32_t rtp_timestamp);
void livemedia_rtp_reception_stats__init(
		livemedia_rtp_reception_stats_t *rtp_reception_stats, u_int32_t ssrc);
void livemedia_rtp_reception_stats__init_seq_num(
		livemedia_rtp_reception_stats_t *rtp_reception_stats, u_int16_t initial_seq_num);
void livemedia_rtp_reception_stats__reset(
		livemedia_rtp_reception_stats_t *rtp_reception_stats);
/* resets periodic stats (called each time they're used to
 * generate a reception report) */

#endif
