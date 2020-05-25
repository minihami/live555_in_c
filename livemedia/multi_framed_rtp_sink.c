#include "multi_framed_rtp_sink.h"
#include "../groupsock/groupsock_helper.h"

/*****************************************************************
 * MultiFramedRTPSink:RTPSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

#ifndef RTP_PAYLOAD_MAX_SIZE
#define RTP_PAYLOAD_MAX_SIZE 1456
      /* Default max packet size (1500, minus allowance for IP, UDP, UMTP headers)
       * (Also, make it a multiple of 4 bytes, just in case that matters.) */
#endif
#ifndef RTP_PAYLOAD_PREFERRED_SIZE
#define RTP_PAYLOAD_PREFERRED_SIZE ((RTP_PAYLOAD_MAX_SIZE) < 1000 ? (RTP_PAYLOAD_MAX_SIZE) : 1000)
#endif

static unsigned const rtp_header_size = 12;

livemedia_multi_framed_rtp_sink_t *__livemedia_multi_framed_rtp_sink__alloc(void)
{
	livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink;
	multi_framed_rtp_sink = (livemedia_multi_framed_rtp_sink_t *)calloc(1, sizeof(livemedia_multi_framed_rtp_sink_t));
	if (multi_framed_rtp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_multi_framed_rtp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return multi_framed_rtp_sink;
}
void __livemedia_multi_framed_rtp_sink__init(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels)
{
	__livemedia_rtp_sink__init(&multi_framed_rtp_sink->rtp_sink,
			rtpgs, rtp_payload_type, rtp_timestamp_frequency, rtp_payload_format_name,
			num_channels);

	/* constructor */
	multi_framed_rtp_sink->out_buf = NULL;
	multi_framed_rtp_sink->cur_fragmentation_offset = 0;
	multi_framed_rtp_sink->previous_frame_ended_fragmentation = false;
	multi_framed_rtp_sink->on_send_error_func = NULL;
	multi_framed_rtp_sink->on_send_error_data = NULL;

	livemedia_multi_framed_rtp_sink__set_packet_sizes(multi_framed_rtp_sink,
			(RTP_PAYLOAD_PREFERRED_SIZE), (RTP_PAYLOAD_MAX_SIZE));

	/* virtual functions configuration */ 
	multi_framed_rtp_sink->do_special_frame_handling = 
		livemedia_multi_framed_rtp_sink__do_special_frame_handling__impl;
	multi_framed_rtp_sink->allow_fragmentation_after_start = 
		livemedia_multi_framed_rtp_sink__allow_fragmentation_after_start__impl;
	multi_framed_rtp_sink->allow_other_frames_after_last_fragment = 
		livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment__impl;
	multi_framed_rtp_sink->frame_can_appear_after_packet_start = 
		livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start__impl;
	multi_framed_rtp_sink->special_header_size = 
		livemedia_multi_framed_rtp_sink__special_header_size__impl;
	multi_framed_rtp_sink->frame_specific_header_size = 
		livemedia_multi_framed_rtp_sink__frame_specific_header_size__impl;
	multi_framed_rtp_sink->compute_overflow_for_new_frame = 
		livemedia_multi_framed_rtp_sink__compute_overflow_for_new_frame__impl;

	((livemedia_media_sink_t *)multi_framed_rtp_sink)->stop_playing = 
		livemedia_multi_framed_rtp_sink__stop_playing__impl;
	((livemedia_media_sink_t *)multi_framed_rtp_sink)->continue_playing = 
		livemedia_multi_framed_rtp_sink__continue_playing__impl;

	((livemedia_medium_t *)multi_framed_rtp_sink)->delete =
		livemedia_multi_framed_rtp_sink__delete__impl;
}
void __livemedia_multi_framed_rtp_sink__deinit(livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
	/* destructor */
	livemedia__delete__out_packet_buffer(multi_framed_rtp_sink->out_buf);

	/* parent deinit */
	__livemedia_rtp_sink__deinit(&multi_framed_rtp_sink->rtp_sink);
}
void __livemedia_multi_framed_rtp_sink__free(livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
	if (multi_framed_rtp_sink) {
		free(multi_framed_rtp_sink);
		multi_framed_rtp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_multi_framed_rtp_sink_t *livemedia__new__multi_framed_rtp_sink(
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name,
		unsigned int num_channels)
{
	livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink;

	multi_framed_rtp_sink = __livemedia_multi_framed_rtp_sink__alloc();
	if (multi_framed_rtp_sink)
		__livemedia_multi_framed_rtp_sink__init(multi_framed_rtp_sink,
				rtpgs, rtp_payload_type, rtp_timestamp_frequency, rtp_payload_format_name,
				num_channels);

	return multi_framed_rtp_sink;
}
/*
 * Functions for virtual functions
 */
void livemedia_multi_framed_rtp_sink__do_special_frame_handling(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int fragmentation_offset, 
		unsigned char *frame_start,
		unsigned int num_bytes_in_frame,
		struct timeval frame_presentation_time,
		unsigned int num_remaining_bytes)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->do_special_frame_handling)
		return multi_framed_rtp_sink->do_special_frame_handling(multi_framed_rtp_sink,
				fragmentation_offset, frame_start, num_bytes_in_frame,
				frame_presentation_time, num_remaining_bytes);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_multi_framed_rtp_sink__allow_fragmentation_after_start(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->allow_fragmentation_after_start)
		return multi_framed_rtp_sink->allow_fragmentation_after_start(multi_framed_rtp_sink);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
	return false;
}

bool livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->allow_other_frames_after_last_fragment)
		return multi_framed_rtp_sink->allow_other_frames_after_last_fragment(multi_framed_rtp_sink);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
	return false;
}

bool livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *frame_start, unsigned int num_bytes_in_frame)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->frame_can_appear_after_packet_start)
		return multi_framed_rtp_sink->frame_can_appear_after_packet_start(multi_framed_rtp_sink,
				frame_start, num_bytes_in_frame);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
	return false;
}

unsigned int livemedia_multi_framed_rtp_sink__special_header_size(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->special_header_size)
		return multi_framed_rtp_sink->special_header_size(multi_framed_rtp_sink);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

unsigned int livemedia_multi_framed_rtp_sink__frame_specific_header_size(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->frame_specific_header_size)
		return multi_framed_rtp_sink->frame_specific_header_size(multi_framed_rtp_sink);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

unsigned int livemedia_multi_framed_rtp_sink__compute_overflow_for_new_frame(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int new_frame_size)
{
	if(multi_framed_rtp_sink && multi_framed_rtp_sink->compute_overflow_for_new_frame)
		return multi_framed_rtp_sink->compute_overflow_for_new_frame(multi_framed_rtp_sink,
				new_frame_size);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line - virtual funtion initialization\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

/*
 * Override functions
 */
void livemedia_multi_framed_rtp_sink__do_special_frame_handling__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int fragmentation_offset, 
		unsigned char *frame_start,
		unsigned int num_bytes_in_frame,
		struct timeval frame_presentation_time,
		unsigned int num_remaining_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation: If this is the first frame in the packet,
	 * use its presentationTime for the RTP timestamp: */

	if (livemedia_multi_framed_rtp_sink__is_first_frame_in_packet(multi_framed_rtp_sink)) {
		livemedia_multi_framed_rtp_sink__set_timestamp(multi_framed_rtp_sink, frame_presentation_time);
	}
}

bool livemedia_multi_framed_rtp_sink__allow_fragmentation_after_start__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *frame_start, unsigned int num_bytes_in_frame)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true; /* by default */
}

unsigned int livemedia_multi_framed_rtp_sink__special_header_size__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

unsigned int livemedia_multi_framed_rtp_sink__frame_specific_header_size__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

unsigned int livemedia_multi_framed_rtp_sink__compute_overflow_for_new_frame__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int new_frame_size)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__stop_playing__impl(
		livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink;
	multi_framed_rtp_sink = (livemedia_multi_framed_rtp_sink_t *)media_sink;

	printf("multi_framed_rtp_sink: %p\n", multi_framed_rtp_sink);
	livemedia_out_packet_buffer__reset_packet_start(
			multi_framed_rtp_sink->out_buf);
	livemedia_out_packet_buffer__reset_offset(
			multi_framed_rtp_sink->out_buf);
	livemedia_out_packet_buffer__reset_overflow_data(
			multi_framed_rtp_sink->out_buf);

	/* Then call the default "stopPlaying()" function: */

	/* First, tell the source that we're no longer interested: */
	livemedia_media_sink__stop_playing__impl(media_sink);
}

bool livemedia_multi_framed_rtp_sink__continue_playing__impl(
		livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__delete__impl(livemedia_medium_t *medium)
{
	livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink;
	multi_framed_rtp_sink = (livemedia_multi_framed_rtp_sink_t *)medium;

	if (multi_framed_rtp_sink) {
		__livemedia_multi_framed_rtp_sink__deinit(multi_framed_rtp_sink);
		__livemedia_multi_framed_rtp_sink__free(multi_framed_rtp_sink);
	}
}

/*
 * Functions in header file
 */
void livemedia_multi_framed_rtp_sink__set_on_send_error_func(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		livemedia__on_send_error_func *on_send_error_func,
		void *on_send_error_data)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_multi_framed_rtp_sink__is_first_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return multi_framed_rtp_sink->is_first_packet;
}

bool livemedia_multi_framed_rtp_sink__is_first_frame_in_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return multi_framed_rtp_sink->num_frames_used_so_far == 0;
}

unsigned int livemedia_multi_framed_rtp_sink__cur_fragmentation_offset(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_multi_framed_rtp_sink__num_frames_used_so_far(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_multi_framed_rtp_sink__our_max_packet_size(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return multi_framed_rtp_sink->our_max_packet_size;
}

/*
 * Normal functions
 */

void livemedia_multi_framed_rtp_sink__set_packet_sizes(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int preferred_packet_size, unsigned int max_packet_size)
{
	if (preferred_packet_size > max_packet_size || preferred_packet_size == 0)
		return; /* sanity check */

	livemedia__delete__out_packet_buffer(multi_framed_rtp_sink->out_buf);

	multi_framed_rtp_sink->out_buf = livemedia__new__out_packet_buffer(
			preferred_packet_size, max_packet_size, 0);
	multi_framed_rtp_sink->our_max_packet_size = max_packet_size; 
	/* save value, in case subclasses need it */


}

void livemedia_multi_framed_rtp_sink__set_marker_bit(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int rtp_hdr;

	rtp_hdr = livemedia_out_packet_buffer__extract_word(
			multi_framed_rtp_sink->out_buf, 0);
	rtp_hdr |= 0x00800000;
	livemedia_out_packet_buffer__insert_word(
			multi_framed_rtp_sink->out_buf, rtp_hdr, 0);
}

void livemedia_multi_framed_rtp_sink__set_timestamp(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		struct timeval frame_presentation_time)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_sink_t *rtp_sink;
	rtp_sink = (livemedia_rtp_sink_t *)multi_framed_rtp_sink;
  /* First, convert the presentation time to a 32-bit RTP timestamp: */
	rtp_sink->current_timestamp = 
		livemedia_rtp_sink__convert_to_rtp_timestamp(rtp_sink, frame_presentation_time);
	
  /* Then, insert it into the RTP packet: */
	livemedia_out_packet_buffer__insert_word(multi_framed_rtp_sink->out_buf, 
			rtp_sink->current_timestamp, multi_framed_rtp_sink->timestamp_position);

}

void livemedia_multi_framed_rtp_sink__set_special_header_word(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int word, /* 32 bits, in host order */
		unsigned int word_position)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__set_special_header_bytes(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *bytes, unsigned int num_bytes,
		unsigned int byte_position)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__set_frame_specific_header_word(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int word, /* 32 bits, in host order */
		unsigned int word_position)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__set_frame_specific_header_bytes(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *bytes, unsigned int num_bytes,
		unsigned int byte_position)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__set_frame_padding(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int num_padding_bytes)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__build_and_send_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		bool is_first_packet)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int rtp_hdr;
	livemedia_rtp_sink_t *rtp_sink;
	
	rtp_sink = (livemedia_rtp_sink_t *)multi_framed_rtp_sink;

	uv_timer_stop(livemedia_medium__next_task((livemedia_medium_t *)multi_framed_rtp_sink));
	multi_framed_rtp_sink->is_first_packet = is_first_packet;
  
	/* Set up the RTP header: */
	rtp_hdr = 0x80000000; /* RTP version 2; marker ('M') bit not set (by default; it can be set later) */
	rtp_hdr |= (rtp_sink->rtp_payload_type << 16);
	rtp_hdr |= rtp_sink->seq_no;
	livemedia_out_packet_buffer__enqueue_word(multi_framed_rtp_sink->out_buf, rtp_hdr);

	/* Note where the RTP timestamp will go.
	 * (We can't fill this in until we start packing payload frames.) */
	multi_framed_rtp_sink->timestamp_position = 
		livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf);
	livemedia_out_packet_buffer__skip_bytes(multi_framed_rtp_sink->out_buf, 4); /* leave a hole for the timestamp */

	livemedia_out_packet_buffer__enqueue_word(multi_framed_rtp_sink->out_buf, livemedia_rtp_sink__ssrc(rtp_sink));
  
	/* Allow for a special, payload-format-specific header following the
	 * RTP header: */
	multi_framed_rtp_sink->special_header_positon =
		livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf);
	multi_framed_rtp_sink->special_header_size_multi_framed_rtp_sink = 
		livemedia_multi_framed_rtp_sink__special_header_size(multi_framed_rtp_sink);
	livemedia_out_packet_buffer__skip_bytes(multi_framed_rtp_sink->out_buf, 
			multi_framed_rtp_sink->special_header_size_multi_framed_rtp_sink);
	
	/* Begin packing as many (complete) frames into the packet as we can: */
	multi_framed_rtp_sink->total_frame_specific_header_sizes = 0;
	multi_framed_rtp_sink->no_frames_left = false;
	multi_framed_rtp_sink->num_frames_used_so_far = 0;
	livemedia_multi_framed_rtp_sink__pack_frame(multi_framed_rtp_sink);

}

void livemedia_multi_framed_rtp_sink__pack_frame(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int frame_size;
	struct timeval presentation_time;
	unsigned int duration_in_microseconds;
	livemedia_media_sink_t *media_sink;

	media_sink = (livemedia_media_sink_t *)multi_framed_rtp_sink;

  /* Get the next frame.

   * First, skip over the space we'll use for any frame-specific header: */
	multi_framed_rtp_sink->cur_frame_specific_header_position =
		livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf);
	multi_framed_rtp_sink->cur_frame_specific_header_size =
		livemedia_multi_framed_rtp_sink__frame_specific_header_size(multi_framed_rtp_sink);
	livemedia_out_packet_buffer__skip_bytes(multi_framed_rtp_sink->out_buf, 
			multi_framed_rtp_sink->cur_frame_specific_header_size);
	multi_framed_rtp_sink->total_frame_specific_header_sizes +=
		multi_framed_rtp_sink->cur_frame_specific_header_size;

	/* See if we have an overflow frame that was too big for the last pkt */
	if (livemedia_out_packet_buffer__have_overflow_data(multi_framed_rtp_sink->out_buf)) {
		/* Use this frame before reading a new one from the source */
		frame_size = livemedia_out_packet_buffer__overflow_data_size(
				multi_framed_rtp_sink->out_buf);
		presentation_time = livemedia_out_packet_buffer__overflow_presentation_time(
				multi_framed_rtp_sink->out_buf);
		duration_in_microseconds = livemedia_out_packet_buffer__overflow_duration_in_microseconds(
				multi_framed_rtp_sink->out_buf);
		livemedia_out_packet_buffer__use_overflow_data(multi_framed_rtp_sink->out_buf);
		
		livemedia_multi_framed_rtp_sink__after_getting_frame1(multi_framed_rtp_sink,
				frame_size, 0, presentation_time, duration_in_microseconds);
	} else {
		/* Normal case: we need to read a new frame from the source */
		if (media_sink->source == NULL)
			return;
		livemedia_framed_source__get_next_frame(media_sink->source, 
				livemedia_out_packet_buffer__cur_ptr(multi_framed_rtp_sink->out_buf),
				livemedia_out_packet_buffer__total_bytes_available(multi_framed_rtp_sink->out_buf),
				livemedia_multi_framed_rtp_sink__after_getting_frame__static,
				multi_framed_rtp_sink, 
				livemedia_multi_framed_rtp_sink__our_handle_closure__static,
				multi_framed_rtp_sink);
	}
}

void send_packet_if_necessary_timer_cb(uv_timer_t *handle)
{
	livemedia_multi_framed_rtp_sink__send_next__static(handle->data);
}

void livemedia_multi_framed_rtp_sink__send_packet_if_necessary(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int new_packet_start;
	struct timeval time_now;
	int secs_diff;
	int64_t u_seconds_to_go;
	livemedia_rtp_sink_t *rtp_sink;
	uv_timer_t *next_task;

	rtp_sink = (livemedia_rtp_sink_t *)multi_framed_rtp_sink;

	if (multi_framed_rtp_sink->num_frames_used_so_far > 0) {
		/* Send the packet: */
#ifdef TEST_LOSS
		if ((our_random()%10) != 0) /* simulate 10% packet loss ##### */
#endif
			if (!livemedia_rtp_interface__send_packet(rtp_sink->rtp_interface,
						livemedia_out_packet_buffer__packet(multi_framed_rtp_sink->out_buf), 
						livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf))) {
				/* if failure handler has been specified, call it */
				if (multi_framed_rtp_sink->on_send_error_func != NULL)
					multi_framed_rtp_sink->on_send_error_func(multi_framed_rtp_sink->on_send_error_data);
			}
		++rtp_sink->packet_count;
		rtp_sink->total_octet_count += livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf);
		rtp_sink->octet_count += livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf) -
			rtp_header_size - multi_framed_rtp_sink->special_header_size_multi_framed_rtp_sink - multi_framed_rtp_sink->total_frame_specific_header_sizes;

		++rtp_sink->seq_no; /* for next time */
	}
	
	if (livemedia_out_packet_buffer__have_overflow_data(multi_framed_rtp_sink->out_buf) &&
			livemedia_out_packet_buffer__total_bytes_available(multi_framed_rtp_sink->out_buf) >
			livemedia_out_packet_buffer__total_buffer_size(multi_framed_rtp_sink->out_buf) / 2) {
		/* Efficiency hack: Reset the packet start pointer to just in front of
		 * the overflow data (allowing for the RTP header and special headers),
		 * so that we probably don't have to "memmove()" the overflow data
		 * into place when building the next packet: */
		new_packet_start = livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf) -
			(rtp_header_size + multi_framed_rtp_sink->special_header_size_multi_framed_rtp_sink + livemedia_multi_framed_rtp_sink__frame_specific_header_size(multi_framed_rtp_sink));
		livemedia_out_packet_buffer__adjust_packet_start(multi_framed_rtp_sink->out_buf, new_packet_start);
	} else {
		/* Normal case: Reset the packet start pointer back to the start: */
		livemedia_out_packet_buffer__reset_packet_start(multi_framed_rtp_sink->out_buf);
	}
	livemedia_out_packet_buffer__reset_offset(multi_framed_rtp_sink->out_buf);
	multi_framed_rtp_sink->num_frames_used_so_far = 0;

	if (multi_framed_rtp_sink->no_frames_left) {
		/* We're done: */
		livemedia_media_sink__on_source_closure((livemedia_media_sink_t *)multi_framed_rtp_sink);
	} else {
		/* We have more frames left to send.  Figure out when the next frame
		 * is due to start playing, then make sure that we wait this long before
		 * sending the next packet. */
		gettimeofday(&time_now, NULL);
		secs_diff = multi_framed_rtp_sink->next_send_time.tv_sec - time_now.tv_sec;
		u_seconds_to_go = secs_diff * 1000000 + (multi_framed_rtp_sink->next_send_time.tv_usec - time_now.tv_usec);
		if (u_seconds_to_go < 0 || secs_diff < 0) { /* sanity check: Make sure that the time-to-delay is non-negative: */
			u_seconds_to_go = 0;
		}

		/* Delay this amount of time: */
		next_task = livemedia_medium__next_task((livemedia_medium_t *)multi_framed_rtp_sink);
		next_task->data = (void *)multi_framed_rtp_sink;
		uv_timer_start(next_task, send_packet_if_necessary_timer_cb, u_seconds_to_go * 1000, 0);
	}
}

void livemedia_multi_framed_rtp_sink__send_next__static(void *first_arg)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__after_getting_frame__static(void *client_data,
		unsigned int num_bytes_read, unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_multi_framed_rtp_sink_t *sink;

	sink = (livemedia_multi_framed_rtp_sink_t *)client_data;
	livemedia_multi_framed_rtp_sink__after_getting_frame1(sink, num_bytes_read,
			num_truncated_bytes, presentation_time, duration_in_microseconds);
}

void livemedia_multi_framed_rtp_sink__after_getting_frame1(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int frame_size, unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_sink_t *rtp_sink;
	unsigned int buffer_size;
	unsigned int cur_fragmentation_offset;
	unsigned int num_frame_bytes_to_use;
	unsigned int overflow_bytes;
	unsigned char *frame_start;

	if (multi_framed_rtp_sink->is_first_packet) {
		/* Record the fact that we're starting to play now: */
		gettimeofday(&multi_framed_rtp_sink->next_send_time, NULL);
	}

	rtp_sink = (livemedia_rtp_sink_t *)multi_framed_rtp_sink;
	rtp_sink->most_recent_presentation_time = presentation_time;
	if (rtp_sink->initial_presentation_time.tv_sec == 0 &&
			rtp_sink->initial_presentation_time.tv_usec == 0) {
		rtp_sink->initial_presentation_time = presentation_time;
	}

	if (num_truncated_bytes > 0) {
		buffer_size = livemedia_out_packet_buffer__total_bytes_available(
				multi_framed_rtp_sink->out_buf);
		fprintf(stderr, "MultiFramedRTPSink::afterGettingFrame1(): The input frame data was too large for our buffer size (%d)."
	     "%d bytes of trailing data was dropped!  Correct this by increasing \"OutPacketBuffer::maxSize\" to at least "
	     "%d, *before* creating this 'RTPSink'.  (Current value is %d.)\n", buffer_size, num_truncated_bytes,
		 *multi_framed_rtp_sink->out_buf->max_size + num_truncated_bytes, *multi_framed_rtp_sink->out_buf->max_size);
	}
	cur_fragmentation_offset = multi_framed_rtp_sink->cur_fragmentation_offset;
	num_frame_bytes_to_use = frame_size;
	overflow_bytes = 0;

	/* If we have already packed one or more frames into this packet,
	 * check whether this new frame is eligible to be packed after them.
	 * (This is independent of whether the packet has enough room for this
	 * new frame; that check comes later.) */
	if (multi_framed_rtp_sink->num_frames_used_so_far > 0) {
		if ((multi_framed_rtp_sink->previous_frame_ended_fragmentation &&
					!livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment(multi_framed_rtp_sink)) ||
					!livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start(multi_framed_rtp_sink,
						livemedia_out_packet_buffer__cur_ptr(multi_framed_rtp_sink->out_buf), frame_size)) {
			/* Save away this frame for next time: */
			num_frame_bytes_to_use = 0;
			livemedia_out_packet_buffer__set_overflow_data(multi_framed_rtp_sink->out_buf, 
					livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf),
					frame_size,
					&presentation_time, duration_in_microseconds);
		}
	}
	multi_framed_rtp_sink->previous_frame_ended_fragmentation = false;

	if (num_frame_bytes_to_use > 0) {
		/* Check whether this frame overflows the packet */
		if (livemedia_out_packet_buffer__would_overflow(multi_framed_rtp_sink->out_buf, frame_size)) {
			/* Don't use this frame now; instead, save it as overflow data, and
			 * send it in the next packet instead.  However, if the frame is too
			 * big to fit in a packet by itself, then we need to fragment it (and
			 * use some of it in this packet, if the payload format permits this.) */
			if (livemedia_multi_framed_rtp_sink__is_too_big_for_a_packet(multi_framed_rtp_sink, frame_size) &&
					(multi_framed_rtp_sink->num_frames_used_so_far == 0 || 
					 livemedia_multi_framed_rtp_sink__allow_fragmentation_after_start(multi_framed_rtp_sink))) {
				/* We need to fragment this frame, and use some of it now: */
				overflow_bytes = livemedia_multi_framed_rtp_sink__compute_overflow_for_new_frame(
						multi_framed_rtp_sink, frame_size);
				num_frame_bytes_to_use -= overflow_bytes;
				multi_framed_rtp_sink->cur_fragmentation_offset += num_frame_bytes_to_use;
			} else {
				/* We don't use any of this frame now: */
				overflow_bytes = frame_size;
				num_frame_bytes_to_use = 0;
			}
			livemedia_out_packet_buffer__set_overflow_data(multi_framed_rtp_sink->out_buf, 
					livemedia_out_packet_buffer__cur_packet_size(multi_framed_rtp_sink->out_buf) +
					num_frame_bytes_to_use, overflow_bytes, &presentation_time, duration_in_microseconds);
		} else if (multi_framed_rtp_sink->cur_fragmentation_offset > 0) {
			/* This is the last fragment of a frame that was fragmented over
			 * more than one packet.  Do any special handling for this case: */
			multi_framed_rtp_sink->cur_fragmentation_offset = 0;
			multi_framed_rtp_sink->previous_frame_ended_fragmentation = true;
		}
	}

	if (num_frame_bytes_to_use == 0 && frame_size > 0) {
		/* Send our packet now, because we have filled it up: */
		livemedia_multi_framed_rtp_sink__send_packet_if_necessary(multi_framed_rtp_sink);
	} else {
		/* Use this frame in our outgoing packet: */
		frame_start = livemedia_out_packet_buffer__cur_ptr(multi_framed_rtp_sink->out_buf);
		livemedia_out_packet_buffer__increment(multi_framed_rtp_sink->out_buf, num_frame_bytes_to_use);
        /* do this now, in case "doSpecialFrameHandling()" calls "setFramePadding()" to append padding bytes */

		/* Here's where any payload format specific processing gets done: */

		livemedia_multi_framed_rtp_sink__do_special_frame_handling(multi_framed_rtp_sink,
				cur_fragmentation_offset, frame_start, num_frame_bytes_to_use, presentation_time,
				overflow_bytes);

		++multi_framed_rtp_sink->num_frames_used_so_far;

		/* Update the time at which the next packet should be sent, based
		 * on the duration of the frame that we just packed into it.
		 * However, if this frame has overflow data remaining, then don't
		 * count its duration yet. */
		if (overflow_bytes == 0) {
			multi_framed_rtp_sink->next_send_time.tv_usec += duration_in_microseconds;
			multi_framed_rtp_sink->next_send_time.tv_sec += multi_framed_rtp_sink->next_send_time.tv_usec / 1000000;
			multi_framed_rtp_sink->next_send_time.tv_usec %= 1000000;
		}

		/* Send our packet now if (i) it's already at our preferred size, or
		 * (ii) (heuristic) another frame of the same size as the one we just
		 *      read would overflow the packet, or
		 * (iii) it contains the last fragment of a fragmented frame, and we
		 *      don't allow anything else to follow this or
		 * (iv) only one frame per packet is allowed: */
		if (livemedia_out_packet_buffer__is_preferred_size(multi_framed_rtp_sink->out_buf) ||
				livemedia_out_packet_buffer__would_overflow(multi_framed_rtp_sink->out_buf, num_frame_bytes_to_use) ||
				(multi_framed_rtp_sink->previous_frame_ended_fragmentation && 
				 !livemedia_multi_framed_rtp_sink__allow_other_frames_after_last_fragment(multi_framed_rtp_sink)) ||
				!livemedia_multi_framed_rtp_sink__frame_can_appear_after_packet_start(multi_framed_rtp_sink,
					livemedia_out_packet_buffer__cur_ptr(multi_framed_rtp_sink->out_buf) - frame_size,
					frame_size)) {
			/* The packet is ready to be sent now */
			livemedia_multi_framed_rtp_sink__send_packet_if_necessary(multi_framed_rtp_sink);
		} else {
			/* There's room for more frames; try getting another: */
			livemedia_multi_framed_rtp_sink__pack_frame(multi_framed_rtp_sink);
		}
	}
}

bool livemedia_multi_framed_rtp_sink__is_too_big_for_a_packet(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_multi_framed_rtp_sink__our_handle_closure__static(void *client_data)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
