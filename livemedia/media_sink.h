#ifndef __LIVEMEDIA_MEDIA_SINK_H__
#define __LIVEMEDIA_MEDIA_SINK_H__
#include "framed_source.h"

/*****************************************************************
 * MediaSink:Medium (Structure Declarations)
 ****************************************************************/

typedef void (livemedia_after_plying_func)(void *client_data);

typedef struct _livemedia_media_sink_t {
	livemedia_medium_t medium;

	livemedia_framed_source_t *source;
	/* The following fields are used when we're being played: */
	livemedia_after_plying_func *after_func;
	void *after_client_data;
	/* TODO: virtual function pointers */
	void (*stop_playing)(struct _livemedia_media_sink_t *media_sink);
	bool (*is_rtp_sink)(struct _livemedia_media_sink_t *media_sink);
	bool (*source_is_compatible_with_us)(struct _livemedia_media_sink_t *media_sink,
			livemedia_media_source_t *media_source);
	bool (*continue_playing)(struct _livemedia_media_sink_t *media_sink);

} livemedia_media_sink_t;

/*****************************************************************
 * OutPacketBuffer (Structure Declarations)
 ****************************************************************/
static unsigned int out_packet_buffer__max_size = 60000;
/* A data structure that a sink may use for an output packet: */
typedef struct _livemedia_out_packet_buffer_t {
	unsigned int *max_size;
	unsigned int packet_start;
	unsigned int cur_offset;
	unsigned int preferred;
	unsigned int max;
	unsigned int limit;
	unsigned char *buf;
	unsigned int overflow_data_offset;
	unsigned int overflow_data_size;
	struct timeval overflow_presentation_time;
	unsigned int overflow_duration_in_microseconds;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_out_packet_buffer_t *out_packet_buffer);

} livemedia_out_packet_buffer_t;

/*****************************************************************
 * MediaSink:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_media_sink_t *__livemedia_media_sink__alloc(void);
void __livemedia_media_sink__init(livemedia_media_sink_t *media_sink);
void __livemedia_media_sink__deinit(livemedia_media_sink_t *media_sink);
void __livemedia_media_sink__free(livemedia_media_sink_t *media_sink);
/*
 * new() and delete() functions
 */
livemedia_media_sink_t *livemedia__new__media_sink(void);
/* if "maxBufferSize" is >0, use it - instead of "maxSize" to compute the buffer size */
/*
 * Functions for virtual functions
 */
void livemedia_media_sink__stop_playing(livemedia_media_sink_t *media_sink);
bool livemedia_media_sink__is_rtp_sink(livemedia_media_sink_t *media_sink);
bool livemedia_media_sink__source_is_compatible_with_us(livemedia_media_sink_t *media_sink,
		livemedia_media_source_t *media_source);
bool livemedia_media_sink__continue_playing(livemedia_media_sink_t *media_sink);

/*
 * Override functions
 */
void livemedia_media_sink__stop_playing__impl(livemedia_media_sink_t *media_sink);
/* Test for specific types of sink: */
bool livemedia_media_sink__is_rtp_sink__impl(livemedia_media_sink_t *media_sink);
/* called by startPlaying() */
bool livemedia_media_sink__source_is_compatible_with_us__impl(livemedia_media_sink_t *media_sink,
		livemedia_media_source_t *source);

  /* redefined virtual functions: */
bool livemedia_media_sink__is_sink__impl(livemedia_medium_t *medium);
void livemedia_media_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
livemedia_framed_source_t *livemedia_media_sink__source(livemedia_media_sink_t *media_sink);

/*
 * Normal functions
 */
bool livemedia_media_sink__lookup_by_name__static(char const *sink_name, 
		livemedia_media_sink_t **result_sink);
bool livemedia_media_sink__start_playing(livemedia_media_sink_t *media_sink,
		livemedia_media_source_t *source, livemedia_after_plying_func *after_func,
		void *after_client_data);
void livemedia_media_sink__on_source_closure__static(void *client_data);	/* can be used in "getNextFrame()" calls */
void livemedia_media_sink__on_source_closure(livemedia_media_sink_t *media_sink);
/* should be called (on ourselves) by continuePlaying() when it
 * discovers that the source we're playing from has closed. */

/*****************************************************************
 * OutPacketBuffer (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_out_packet_buffer_t *__livemedia_out_packet_buffer__alloc(void);
void __livemedia_out_packet_buffer__init(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int preferred_packet_size,
		unsigned int max_packet_size, unsigned int max_buffer_size);
void __livemedia_out_packet_buffer__deinit(livemedia_out_packet_buffer_t *out_packet_buffer);
void __livemedia_out_packet_buffer__free(livemedia_out_packet_buffer_t *out_packet_buffer);
/*
 * new() and delete() functions
 */
livemedia_out_packet_buffer_t *livemedia__new__out_packet_buffer(unsigned int preferred_packet_size,
		unsigned int max_packet_size, unsigned int max_buffer_size);
void livemedia__delete__out_packet_buffer(livemedia_out_packet_buffer_t *out_packet_buffer);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_out_packet_buffer__delete__impl(livemedia_out_packet_buffer_t *out_packet_buffer);

/*
 * Functions in header file
 */
void livemedia_out_packet_buffer__increase_max_size_to__static(livemedia_out_packet_buffer_t *out_packet_buffer, 
		unsigned int new_max_size);
unsigned char *livemedia_out_packet_buffer__cur_ptr(livemedia_out_packet_buffer_t *out_packet_buffer);
unsigned int livemedia_out_packet_buffer__total_bytes_available(livemedia_out_packet_buffer_t *out_packet_buffer);
unsigned int livemedia_out_packet_buffer__total_buffer_size(livemedia_out_packet_buffer_t *out_packet_buffer);
unsigned char *livemedia_out_packet_buffer__packet(livemedia_out_packet_buffer_t *out_packet_buffer);
unsigned int livemedia_out_packet_buffer__cur_packet_size(livemedia_out_packet_buffer_t *out_packet_buffer);
void livemedia_out_packet_buffer__increment(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int num_bytes);
bool livemedia_out_packet_buffer__is_preferred_size(livemedia_out_packet_buffer_t *out_packet_buffer);
bool livemedia_out_packet_buffer__would_overflow(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int num_bytes);
unsigned int livemedia_out_packet_buffer__num_overflow_bytes(livemedia_out_packet_buffer_t *out_packet_buffer, 
		unsigned int num_bytes);
bool livemedia_out_packet_buffer__is_too_big_for_a_packet(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int num_bytes);
unsigned int livemedia_out_packet_buffer__overflow_data_size(livemedia_out_packet_buffer_t *out_packet_buffer);
struct timeval livemedia_out_packet_buffer__overflow_presentation_time(livemedia_out_packet_buffer_t *out_packet_buffer);
unsigned int livemedia_out_packet_buffer__overflow_duration_in_microseconds(livemedia_out_packet_buffer_t *out_packet_buffer);
bool livemedia_out_packet_buffer__have_overflow_data(livemedia_out_packet_buffer_t *out_packet_buffer);
void livemedia_out_packet_buffer__reset_offset(livemedia_out_packet_buffer_t *out_packet_buffer);
void livemedia_out_packet_buffer__reset_overflow_data(livemedia_out_packet_buffer_t *out_packet_buffer);

/*
 * Normal functions
 */
void livemedia_out_packet_buffer__enqueue(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned char const *from, unsigned int num_bytes);
void livemedia_out_packet_buffer__enqueue_word(livemedia_out_packet_buffer_t *out_packet_buffer,
		u_int32_t word);
void livemedia_out_packet_buffer__insert(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned char const *from, unsigned int num_bytes, unsigned int to_position);
void livemedia_out_packet_buffer__insert_word(livemedia_out_packet_buffer_t *out_packet_buffer,
		u_int32_t word, unsigned int to_position);
void livemedia_out_packet_buffer__extract(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned char *to, unsigned int num_bytes, unsigned int from_position);
u_int32_t livemedia_out_packet_buffer__extract_word(livemedia_out_packet_buffer_t *out_packet_buffer,
		 unsigned int from_position);
void livemedia_out_packet_buffer__skip_bytes(livemedia_out_packet_buffer_t *out_packet_buffer, unsigned int num_bytes);
void livemedia_out_packet_buffer__set_overflow_data(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int overflow_data_offset, unsigned int overflow_data_size, 
		struct timeval const *presentation_time, 
		unsigned int duration_in_microseconds);
void livemedia_out_packet_buffer__use_overflow_data(livemedia_out_packet_buffer_t *out_packet_buffer);
void livemedia_out_packet_buffer__adjust_packet_start(livemedia_out_packet_buffer_t *out_packet_buffer,
		unsigned int num_bytes);
void livemedia_out_packet_buffer__reset_packet_start(livemedia_out_packet_buffer_t *outpacket_buffer);
#endif
