#include "rtp_sink.h"
#include "groupsock_helper.h"

/*****************************************************************
 * RTPSink:MediaSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_sink_t *__livemedia_rtp_sink__alloc(void)
{
	livemedia_rtp_sink_t *rtp_sink;
	rtp_sink = (livemedia_rtp_sink_t *)calloc(1, sizeof(livemedia_rtp_sink_t));
	if (rtp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_sink;
}
void __livemedia_rtp_sink__init(livemedia_rtp_sink_t *rtp_sink,
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_type,
		u_int32_t rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels)
{
	__livemedia_media_sink__init(&rtp_sink->media_sink);

	rtp_sink->rtp_interface = livemedia__new__rtp_interface(
			(livemedia_medium_t *)rtp_sink, rtp_gs);

	rtp_sink->rtp_payload_type = rtp_payload_type;
	rtp_sink->packet_count = 0;
	rtp_sink->octet_count = 0;
	rtp_sink->total_octet_count = 0;
	rtp_sink->timestamp_frequency = rtp_timestamp_frequency;
	rtp_sink->next_timestamp_has_been_preset = false;
	rtp_sink->enable_rtcp_reports = true;
	rtp_sink->num_channels = num_channels;
	rtp_sink->estimated_bitrate = 0;
	rtp_sink->rtp_payload_format_name = livemedia__str_dup(
			rtp_payload_format_name == NULL ? "???" : rtp_payload_format_name);
	gettimeofday(&rtp_sink->creation_time, NULL);
	rtp_sink->total_octet_count_start_time = rtp_sink->creation_time;
	livemedia_rtp_sink__reset_presentation_times(rtp_sink);

	rtp_sink->seq_no = (u_int16_t)our_random32();
	rtp_sink->ssrc = our_random32();
	rtp_sink->timestamp_base = our_random32();

	rtp_sink->transmission_stats_db = livemedia__new__rtp_transmission_stats_db(rtp_sink);

	/* virtual functions configuration */ 
	rtp_sink->sdp_media_type = livemedia_rtp_sink__sdp_media_type__impl;
	rtp_sink->rtpmap_line =	livemedia_rtp_sink__rtpmap_line__impl;
	rtp_sink->aux_sdp_line = livemedia_rtp_sink__aux_sdp_line__impl;
	((livemedia_media_sink_t *)rtp_sink)->is_rtp_sink = 
		livemedia_rtp_sink__is_rtp_sink__impl;
	((livemedia_medium_t *)rtp_sink)->delete = livemedia_rtp_sink__delete__impl;
}
void __livemedia_rtp_sink__deinit(livemedia_rtp_sink_t *rtp_sink)
{
	livemedia__delete__rtp_transmission_stats_db(rtp_sink->transmission_stats_db);
	free((char *)rtp_sink->rtp_payload_format_name);
	livemedia_rtp_interface__forget_our_groupsock(rtp_sink->rtp_interface);
	livemedia__delete__rtp_interface(rtp_sink->rtp_interface);
	
    /* so that the "fRTPInterface" destructor doesn't turn off background read handling (in case
    * its 'groupsock' is being shared with something else that does background read handling). */

	/* parent deinit */
	__livemedia_media_sink__deinit(&rtp_sink->media_sink);
}
void __livemedia_rtp_sink__free(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink) {
		free(rtp_sink);
		rtp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_sink_t *livemedia__new__rtp_sink(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_type,
		u_int32_t rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_sink_t *rtp_sink;

	rtp_sink = __livemedia_rtp_sink__alloc();
	if (rtp_sink)
		__livemedia_rtp_sink__init(rtp_sink, rtp_gs, rtp_payload_type,
				rtp_timestamp_frequency, rtp_payload_format_name, num_channels);

	return rtp_sink;
}
/*
 * Functions for virtual functions
 */
char *livemedia_rtp_sink__sdp_media_type(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink) {
		return rtp_sink->sdp_media_type(rtp_sink);
	}
}

char *livemedia_rtp_sink__rtpmap_line(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink) {
		return rtp_sink->rtpmap_line(rtp_sink);
	}
}

char *livemedia_rtp_sink__aux_sdp_line(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink) {
		return rtp_sink->aux_sdp_line(rtp_sink);
	}
}


/*
 * Override functions
 */
char *livemedia_rtp_sink__sdp_media_type__impl(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
char *livemedia_rtp_sink__rtpmap_line__impl(livemedia_rtp_sink_t *rtp_sink)
{
	char *encoding_params_part;
	char *rtpmap_fmt;
	unsigned int rtpmap_fmt_size;
	char *rtpmap_line;

	if (livemedia_rtp_sink__rtp_payload_type(rtp_sink) >= 96) { /* the payload format type is dynamic */
		if (livemedia_rtp_sink__num_channels(rtp_sink) != 1) {
			encoding_params_part = (char*)malloc(sizeof(char) * (1 + 20 /* max int len */));
			sprintf(encoding_params_part, "/%d", livemedia_rtp_sink__num_channels(rtp_sink));
		} else {
			encoding_params_part = livemedia__str_dup("");
		}
		rtpmap_fmt = "a=rtpmap:%d %s/%d%s\r\n";
		rtpmap_fmt_size = strlen(rtpmap_fmt)
			+ 3 /* max char len */ + strlen(livemedia_rtp_sink__rtp_payload_format_name(rtp_sink))
			+ 20 /* max int len */ + strlen(encoding_params_part);
		rtpmap_line = (char*)malloc(sizeof(char) * rtpmap_fmt_size);
		sprintf(rtpmap_line, rtpmap_fmt,
				livemedia_rtp_sink__rtp_payload_type(rtp_sink), livemedia_rtp_sink__rtp_payload_format_name(rtp_sink),
				livemedia_rtp_sink__rtp_timestamp_frequency(rtp_sink), encoding_params_part);
		if (encoding_params_part) {
			free(encoding_params_part);
			encoding_params_part = NULL;
		}

		return rtpmap_line;
	} else {
		/* The payload format is staic, so there's no "a=rtpmap:" line: */
		return livemedia__str_dup("");
	}
}
char *livemedia_rtp_sink__aux_sdp_line__impl(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return NULL;
}
bool livemedia_rtp_sink__is_rtp_sink__impl(livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_sink__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_sink_t *rtp_sink;
	rtp_sink = (livemedia_rtp_sink_t *)medium;
	if (rtp_sink) {
		__livemedia_rtp_sink__deinit(rtp_sink);
		__livemedia_rtp_sink__free(rtp_sink);
	}
}

/*
 * Functions in header file
 */

/* used by RTSP servers: */
groupsock_groupsock_t *livemedia_rtp_sink__groupsock_being_used(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_rtp_interface__gs(rtp_sink->rtp_interface);
}

unsigned char livemedia_rtp_sink__rtp_payload_type(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->rtp_payload_type;
}

unsigned int livemedia_rtp_sink__rtp_timestamp_frequency(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->timestamp_frequency;
}

void livemedia_rtp_sink__set_rtp_timestamp_frequency(livemedia_rtp_sink_t *rtp_sink, unsigned int freq)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

char const *livemedia_rtp_sink__rtp_payload_format_name(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->rtp_payload_format_name;
}

unsigned int livemedia_rtp_sink__num_channels(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->num_channels;
}

u_int16_t livemedia_rtp_sink__current_seq_no(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->seq_no;
}

struct _livemedia_rtp_transmission_stats_db_t *livemedia_rtp_sink__transmission_stats_db(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtp_sink__next_timestamp_has_been_preset(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->next_timestamp_has_been_preset;
}

bool *livemedia_rtp_sink__enable_rtcp_reports(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &rtp_sink->enable_rtcp_reports;
}

struct timeval const* livemedia_rtp_sink__creation_time(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

struct timeval const* livemedia_rtp_sink__initial_presentation_time(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

struct timeval const* livemedia_rtp_sink__most_recent_presentation_time(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_sink__set_stream_socket(livemedia_rtp_sink_t *rtp_sink,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_sink__add_stream_socket(livemedia_rtp_sink_t *rtp_sink,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_interface__add_stream_socket(rtp_sink->rtp_interface, 
			sock_num, stream_channel_id);
}

void livemedia_rtp_sink__remove_stream_socket(livemedia_rtp_sink_t *rtp_sink,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int *livemedia_rtp_sink__estimated_bitrate(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &rtp_sink->estimated_bitrate;
}

/* later need a means of changing the SSRC if there's a collision ##### */
u_int32_t livemedia_rtp_sink__ssrc(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->ssrc;
}

u_int32_t livemedia_rtp_sink__convert_to_rtp_timestamp(livemedia_rtp_sink_t *rtp_sink,
		struct timeval tv)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Begin by converting from "struct timeval" units to RTP timestamp units: */
	u_int32_t timestamp_increment;
	u_int32_t rtp_timestamp;

	timestamp_increment = (rtp_sink->timestamp_frequency * tv.tv_sec);
	timestamp_increment += (u_int32_t)(rtp_sink->timestamp_frequency * (tv.tv_sec/1000000.0) + 0.5); 
	/* note: rounding */

	/* Then add this to our 'timestamp base': */
	if (rtp_sink->next_timestamp_has_been_preset) {
		/* Make the returned timestamp the same as the current "fTimestampBase",
		 * so that timestamps begin with the value that was previously preset: */
		rtp_sink->timestamp_base -= timestamp_increment;
		rtp_sink->next_timestamp_has_been_preset = false;
	}

	rtp_timestamp = rtp_sink->timestamp_base + timestamp_increment;
#ifdef DEBUG_TIMESTAMPS
	fprintf(stderr, "fTimestampBase: 0x%08x, tv: %lu.%06ld\n\t=> RTP timestamp: 0x%08x\n",
			rtp_sink->timestamp_base, tv.tv_sec, tv.tv_usec, rtp_timestamp);
	fflush(stderr);
#endif

	return rtp_timestamp;
}

unsigned int livemedia_rtp_sink__packet_count(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->packet_count;
}

unsigned int livemedia_rtp_sink__octet_count(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_sink->octet_count;
}
/*
 * Normal functions
 */

bool livemedia_rtp_sink__lookup_by_name__static(char const *sink_name, livemedia_rtp_sink_t **rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
u_int32_t livemedia_rtp_sink__preset_next_time_stamp(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct timeval time_now;
	u_int32_t ts_now;

	gettimeofday(&time_now, NULL);

	ts_now = livemedia_rtp_sink__convert_to_rtp_timestamp(rtp_sink, time_now);
	if (!groupsock_groupsock__has_multiple_destinations(
				livemedia_rtp_sink__groupsock_being_used(rtp_sink))) {
		/* Don't adjust the timestamp stream if we already have another destination ongoing */
		rtp_sink->timestamp_base = ts_now;
		rtp_sink->next_timestamp_has_been_preset = true;
	}

	return ts_now;
}

void livemedia_rtp_sink__get_total_bitrate(livemedia_rtp_sink_t *rtp_sink,
		unsigned int *out_num_bytes, double *out_elapsed_time)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_sink__reset_presentation_times(livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	rtp_sink->initial_presentation_time.tv_sec = 
		rtp_sink->most_recent_presentation_time.tv_sec = 0;
	rtp_sink->initial_presentation_time.tv_usec = 
		rtp_sink->most_recent_presentation_time.tv_usec = 0;
}

/*****************************************************************
 * RTPTransmissionStatsDB (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_transmission_stats_db_t *__livemedia_rtp_transmission_stats_db__alloc(void)
{
	livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db;
	rtp_transmission_stats_db = (livemedia_rtp_transmission_stats_db_t *)calloc(1, sizeof(livemedia_rtp_transmission_stats_db_t));
	if (rtp_transmission_stats_db == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_transmission_stats_db_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_transmission_stats_db;
}
void __livemedia_rtp_transmission_stats_db__init(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db,
		livemedia_rtp_sink_t *rtp_sink)
{
	rtp_transmission_stats_db->our_rtp_sink = rtp_sink;
	rtp_transmission_stats_db->table =
		livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	rtp_transmission_stats_db->num_receivers = 0;

	rtp_transmission_stats_db->delete = livemedia_rtp_transmission_stats_db__delete__impl;
}
void __livemedia_rtp_transmission_stats_db__deinit(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db)
{
	/* First, remove and delete all stats records from the table: */
	livemedia_rtp_transmission_stats_t *stats;
	while ((stats = (livemedia_rtp_transmission_stats_t *)
				livemedia_hash_table__remove_next(rtp_transmission_stats_db->table)) != NULL) {
		livemedia__delete__rtp_transmission_stats(stats);
	}

	/* Then, delete the table itself: */
	livemedia__delete__hash_table(rtp_transmission_stats_db->table);
}
void __livemedia_rtp_transmission_stats_db__free(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db)
{
	if (rtp_transmission_stats_db) {
		free(rtp_transmission_stats_db);
		rtp_transmission_stats_db = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_transmission_stats_db_t *livemedia__new__rtp_transmission_stats_db(
		livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db;

	rtp_transmission_stats_db = __livemedia_rtp_transmission_stats_db__alloc();
	if (rtp_transmission_stats_db)
		__livemedia_rtp_transmission_stats_db__init(rtp_transmission_stats_db,
				rtp_sink);

	return rtp_transmission_stats_db;
}
void livemedia__delete__rtp_transmission_stats_db(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db)
{
	if (rtp_transmission_stats_db && rtp_transmission_stats_db->delete)
		rtp_transmission_stats_db->delete(rtp_transmission_stats_db);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_transmission_stats_db__delete__impl(livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_transmission_stats_db) {
		__livemedia_rtp_transmission_stats_db__deinit(rtp_transmission_stats_db);
		__livemedia_rtp_transmission_stats_db__free(rtp_transmission_stats_db);
	}
}

/*
 * Functions in header file
 */
unsigned int livemedia_rtp_transmission_stats_db__num_receivers(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*
 * Normal functions
 */
void livemedia_rtp_transmission_stats_db__note_incoming_rr(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db,
		u_int32_t ssrc, struct sockaddr_in const *last_from_address,
		unsigned int loss_stats, unsigned int last_packet_num_received,
		unsigned int jitter, unsigned int last_sr_time, unsigned int diff_sr_rr_time)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void livemedia_rtp_transmission_stats_db__remove_record(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
struct _livemedia_rtp_transmission_stats_t livemedia_rtp_transmission_stats_db__lookup(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_transmission_stats_db__add(
		livemedia_rtp_transmission_stats_db_t *rtp_transmission_stats_db, 
		u_int32_t ssrc, struct _livemedia_rtp_transmission_stats_t stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*****************************************************************
 * RTPTransmissionStats (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_transmission_stats_t *__livemedia_rtp_transmission_stats__alloc(void)
{
	livemedia_rtp_transmission_stats_t *rtp_transmission_stats;
	rtp_transmission_stats = (livemedia_rtp_transmission_stats_t *)calloc(1, sizeof(livemedia_rtp_transmission_stats_t));
	if (rtp_transmission_stats == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_transmission_stats_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_transmission_stats;
}
void __livemedia_rtp_transmission_stats__init(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		livemedia_rtp_sink_t *rtp_sink, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_transmission_stats->delete = livemedia_rtp_transmission_stats__delete__impl;
}
void __livemedia_rtp_transmission_stats__deinit(livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: parent deinit */
}
void __livemedia_rtp_transmission_stats__free(livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
	if (rtp_transmission_stats) {
		free(rtp_transmission_stats);
		rtp_transmission_stats = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_transmission_stats_t *livemedia__new__rtp_transmission_stats(
		livemedia_rtp_sink_t *rtp_sink, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_transmission_stats_t *rtp_transmission_stats;

	rtp_transmission_stats = __livemedia_rtp_transmission_stats__alloc();
	if (rtp_transmission_stats)
		__livemedia_rtp_transmission_stats__init(rtp_transmission_stats,
				rtp_sink, ssrc);

	return rtp_transmission_stats;
}
void livemedia__delete__rtp_transmission_stats(livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_transmission_stats && rtp_transmission_stats->delete)
		rtp_transmission_stats->delete(rtp_transmission_stats);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_transmission_stats__delete__impl(livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/*TODO: if child, get parent as parameter and cast to child */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_transmission_stats) {
		__livemedia_rtp_transmission_stats__deinit(rtp_transmission_stats);
		__livemedia_rtp_transmission_stats__free(rtp_transmission_stats);
	}
}

/*
 * Functions in header file
 */
u_int32_t livemedia_rtp_transmission_stats__ssrc(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

struct sockaddr_in *livemedia_rtp_transmission_stats__last_from_address(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__last_packet_num_received(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__first_packet_num_reported(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__tot_num_packet_lost(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__jitter(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__last_sr_time(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__diff_sr_rr_time(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__round_trip_delay(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

struct timeval livemedia_rtp_transmission_stats__time_created(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

struct timeval livemedia_rtp_transmission_stats__last_time_received(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

u_int8_t livemedia_rtp_transmission_stats__packet_loss_ratio(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

int livemedia_rtp_transmission_stats__packet_lost_between_rr(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*
 * Normal functions
 */

struct timeval livemedia_rtp_transmission_stats__get_total_octet_count(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		u_int32_t *hi, u_int32_t *lo)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

struct timeval livemedia_rtp_transmission_stats__get_total_packet_count(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		u_int32_t *hi, u_int32_t *lo)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtp_transmission_stats__packet_received_since_last_rr(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_transmission_stats__note_incoming_rr(
		livemedia_rtp_transmission_stats_t *rtp_transmission_stats,
		struct sockaddr_in const *last_from_address,
		unsigned int loss_stats, unsigned int last_packet_num_received,
		unsigned int jitter,
		unsigned int last_sr_time, unsigned int diff_sr_rr_time)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*****************************************************************
 * Iterator::RTPTransmissionStatsDB (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_transmission_stats_db_iterator_t *__livemedia_rtp_transmission_stats_db_iterator__alloc(void)
{
	livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator;
	rtp_transmission_stats_db_iterator = (livemedia_rtp_transmission_stats_db_iterator_t *)calloc(1, sizeof(livemedia_rtp_transmission_stats_db_iterator_t));
	if (rtp_transmission_stats_db_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_transmission_stats_db_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_transmission_stats_db_iterator;
}
void __livemedia_rtp_transmission_stats_db_iterator__init(
		livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator,
		livemedia_rtp_transmission_stats_db_t *reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	/* TODO: parent init */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_transmission_stats_db_iterator->delete = livemedia_rtp_transmission_stats_db_iterator__delete__impl;
}
void __livemedia_rtp_transmission_stats_db_iterator__deinit(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator)
{
#ifdef WEBGATE_DEBUG
	/* TODO: destructor */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: parent deinit */
}
void __livemedia_rtp_transmission_stats_db_iterator__free(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator)
{
	if (rtp_transmission_stats_db_iterator) {
		free(rtp_transmission_stats_db_iterator);
		rtp_transmission_stats_db_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_transmission_stats_db_iterator_t *livemedia__new__rtp_transmission_stats_db_iterator(
		livemedia_rtp_transmission_stats_db_t *reception_stats_db)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator;

	rtp_transmission_stats_db_iterator = __livemedia_rtp_transmission_stats_db_iterator__alloc();
	if (rtp_transmission_stats_db_iterator)
		__livemedia_rtp_transmission_stats_db_iterator__init(
				rtp_transmission_stats_db_iterator, reception_stats_db);

	return rtp_transmission_stats_db_iterator;
}
void livemedia__delete__rtp_transmission_stats_db_iterator(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_transmission_stats_db_iterator && rtp_transmission_stats_db_iterator->delete)
		rtp_transmission_stats_db_iterator->delete(rtp_transmission_stats_db_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_transmission_stats_db_iterator__delete__impl(livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator)
{
#ifdef WEBGATE_DEBUG
	/*TODO: if child, get parent as parameter and cast to child */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_transmission_stats_db_iterator) {
		__livemedia_rtp_transmission_stats_db_iterator__deinit(rtp_transmission_stats_db_iterator);
		__livemedia_rtp_transmission_stats_db_iterator__free(rtp_transmission_stats_db_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_rtp_transmission_stats_t *livemedia_rtp_transmission_stats_db_iterator__next(
		livemedia_rtp_transmission_stats_db_iterator_t *rtp_transmission_stats_db_iterator)
{
#ifdef WEBGATE_DEBUG
	/*TODO*/
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
