#ifndef __LIVEMEDIA_MULTI_FRAMED_RTP_SINK_H__
#define __LIVEMEDIA_MULTI_FRAMED_RTP_SINK_H__

#include "rtp_sink.h"

typedef void (livemedia__on_send_error_func)(void *client_data);
/*****************************************************************
 * MultiFramedRTPSink:RTPSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_multi_framed_rtp_sink_t {
	livemedia_rtp_sink_t rtp_sink;

	/* member variables */
	livemedia_out_packet_buffer_t *out_buf;
	bool no_frames_left;
	unsigned int num_frames_used_so_far;
	unsigned int cur_fragmentation_offset;
	bool previous_frame_ended_fragmentation;

	bool is_first_packet;
	struct timeval next_send_time;
	unsigned int timestamp_position;
	unsigned int special_header_positon;
	unsigned int special_header_size_multi_framed_rtp_sink;
	unsigned int cur_frame_specific_header_position;
	unsigned int cur_frame_specific_header_size;
	unsigned int total_frame_specific_header_sizes;
	unsigned int our_max_packet_size;

	livemedia__on_send_error_func *on_send_error_func;
	void *on_send_error_data;

	/* virtual function pointers */
	void (*do_special_frame_handling)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
			unsigned int fragmentation_offset, 
			unsigned char *frame_start,
			unsigned int num_bytes_in_frame,
			struct timeval frame_presentation_time,
			unsigned int num_remaining_bytes);
	/* perform any processing specific to the particular payload format */
	bool (*allow_fragmentation_after_start)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
	/* whether a frame can be fragmented if other frame(s) appear earlier
	 * in the packet (by default: False) */
	bool (*allow_other_frames_after_last_fragment)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
	/* whether other frames can be packed into a packet following the
	 * final fragment of a previous, fragmented frame (by default: False) */
	bool (*frame_can_appear_after_packet_start)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
			unsigned char const *frame_start, unsigned int num_bytes_in_frame);
	unsigned int (*special_header_size)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
	/* returns the size of any special header used (following the RTP header) (default: 0) */
	unsigned int (*frame_specific_header_size)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
	/* returns the size of any frame-specific header used (before each frame
	 * within the packet) (default: 0) */

	unsigned int (*compute_overflow_for_new_frame)(
			struct _livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
			unsigned int new_frame_size);
	/* returns the number of overflow bytes that would be produced by adding a new
	 * frame of size "newFrameSize" to the current RTP packet.
	 * (By default, this just calls "numOverflowBytes()", but subclasses can redefine
	 * this to (e.g.) impose a granularity upon RTP payload fragments.) */

} livemedia_multi_framed_rtp_sink_t;

/*****************************************************************
 * MultiFramedRTPSink:RTPSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_multi_framed_rtp_sink_t *__livemedia_multi_framed_rtp_sink__alloc(void);
void __livemedia_multi_framed_rtp_sink__init(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels);
void __livemedia_multi_framed_rtp_sink__deinit(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
void __livemedia_multi_framed_rtp_sink__free(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
/*
 * new() and delete() functions
 */
livemedia_multi_framed_rtp_sink_t *livemedia__new__multi_framed_rtp_sink(
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels);
/* we're a virtual base class */

/*
 * Functions for virtual functions
 */
void livemedia_multi_framed_rtp_sink__do_special_frame_handling(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int fragmentation_offset, 
		unsigned char *frame_start,
		unsigned int num_bytes_in_frame,
		struct timeval frame_presentation_time,
		unsigned int num_remaining_bytes);
/* perform any processing specific to the particular payload format */

bool livemedia_multi_framed_rtp_sink__allow_fragmentation_after_start(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
/* whether a frame can be fragmented if other frame(s) appear earlier
 * in the packet (by default: False) */

bool livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
/* whether other frames can be packed into a packet following the
 * final fragment of a previous, fragmented frame (by default: False) */

bool livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *frame_start, unsigned int num_bytes_in_frame);
/* returns the size of any special header used (following the RTP header) (default: 0) */

unsigned int livemedia_multi_framed_rtp_sink__special_header_size(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
/* returns the size of any frame-specific header used (before each frame
 * within the packet) (default: 0) */

unsigned int livemedia_multi_framed_rtp_sink__frame_specific_header_size(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);
/* returns the number of overflow bytes that would be produced by adding a new
 * frame of size "newFrameSize" to the current RTP packet.
 * (By default, this just calls "numOverflowBytes()", but subclasses can redefine
 * this to (e.g.) impose a granularity upon RTP payload fragments.) */

unsigned int livemedia_multi_framed_rtp_sink__compute_overflow_for_new_frame(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int new_frame_size);

/*
 * Override functions
 */
void livemedia_multi_framed_rtp_sink__do_special_frame_handling__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int fragmentation_offset, 
		unsigned char *frame_start,
		unsigned int num_bytes_in_frame,
		struct timeval frame_presentation_time,
		unsigned int num_remaining_bytes);

bool livemedia_multi_framed_rtp_sink__allow_fragmentation_after_start__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

bool livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

bool livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *frame_start, unsigned int num_bytes_in_frame);

unsigned int livemedia_multi_framed_rtp_sink__special_header_size__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

unsigned int livemedia_multi_framed_rtp_sink__frame_specific_header_size__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

unsigned int livemedia_multi_framed_rtp_sink__compute_overflow_for_new_frame__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int new_frame_size);

void livemedia_multi_framed_rtp_sink__stop_playing__impl(
		livemedia_media_sink_t *media_sink);

bool livemedia_multi_framed_rtp_sink__continue_playing__impl(
		livemedia_media_sink_t *media_sink);

void livemedia_multi_framed_rtp_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
void livemedia_multi_framed_rtp_sink__set_on_send_error_func(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		livemedia__on_send_error_func *on_send_error_func,
		void *on_send_error_data);
/* Can be used to set a callback function to be called if there's an error sending RTP packets on our socket. */

/* Functions that might be called by doSpecialFrameHandling(), or other subclass virtual functions: */
bool livemedia_multi_framed_rtp_sink__is_first_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

bool livemedia_multi_framed_rtp_sink__is_first_frame_in_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

unsigned int livemedia_multi_framed_rtp_sink__cur_fragmentation_offset(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

unsigned int livemedia_multi_framed_rtp_sink__num_frames_used_so_far(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

unsigned int livemedia_multi_framed_rtp_sink__our_max_packet_size(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

/*
 * Normal functions
 */
void livemedia_multi_framed_rtp_sink__set_packet_sizes(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int preferred_packet_size, unsigned int max_packet_size);

/* Functions that might be called by doSpecialFrameHandling(), or other subclass virtual functions: */
void livemedia_multi_framed_rtp_sink__set_marker_bit(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

void livemedia_multi_framed_rtp_sink__set_timestamp(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		struct timeval frame_presentation_time);

void livemedia_multi_framed_rtp_sink__set_special_header_word(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int word, /* 32 bits, in host order */
		unsigned int word_position);

void livemedia_multi_framed_rtp_sink__set_special_header_bytes(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *bytes, unsigned int num_bytes,
		unsigned int byte_position);

void livemedia_multi_framed_rtp_sink__set_frame_specific_header_word(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int word, /* 32 bits, in host order */
		unsigned int word_position);

void livemedia_multi_framed_rtp_sink__set_frame_specific_header_bytes(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *bytes, unsigned int num_bytes,
		unsigned int byte_position);

void livemedia_multi_framed_rtp_sink__set_frame_padding(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int num_padding_bytes);

void livemedia_multi_framed_rtp_sink__build_and_send_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		bool is_first_packet);

void livemedia_multi_framed_rtp_sink__pack_frame(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

void livemedia_multi_framed_rtp_sink__send_packet_if_necessary(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink);

void livemedia_multi_framed_rtp_sink__send_next__static(void *first_arg);

void livemedia_multi_framed_rtp_sink__after_getting_frame__static(void *client_data,
		unsigned int num_bytes_read, unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds);

void livemedia_multi_framed_rtp_sink__after_getting_frame1(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int num_bytes_read, unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds);

bool livemedia_multi_framed_rtp_sink__is_too_big_for_a_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int num_bytes);

void livemedia_multi_framed_rtp_sink__our_handle_closure__static(void *client_data);

#endif
