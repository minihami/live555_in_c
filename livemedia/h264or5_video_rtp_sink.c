#include "h264or5_video_rtp_sink.h"
#include "h264or5_video_stream_framer.h"

/* Because of the ideosyncracies of the H.264 RTP payload format, we implement
 * "H264or5VideoRTPSink" using a separate "H264or5Fragmenter" h264or5_fragmenter that delivers,
 * to the "H264or5VideoRTPSink", only fragments that will fit within an outgoing
 * RTP packet.  I.e., we implement fragmentation in this separate "H264or5Fragmenter"
 * h264or5_fragmenter, rather than in "H264or5VideoRTPSink".
 * (Note: This h264or5_fragmenter should be used only by "H264or5VideoRTPSink", or a subh264or5_fragmenter.) */
/*****************************************************************
 * H264or5Fragmenter:FramedFilter (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264or5_fragmenter_t {
	livemedia_framed_filter_t framed_filter;

	/* member variables */
	int h_number;
	unsigned int input_buffer_size;
	unsigned int max_output_packet_size;
	unsigned char *input_buffer;
	unsigned int num_valid_data_bytes;
	unsigned int cur_data_offset;
	unsigned int save_num_truncated_bytes;
	bool last_fragment_completed_nal_unit;

} livemedia_h264or5_fragmenter_t;

/*****************************************************************
 * H264or5Fragmenter:FramedFilter (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_fragmenter_t *__livemedia_h264or5_fragmenter__alloc(void);

void __livemedia_h264or5_fragmenter__init(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter, int h_number,
		livemedia_framed_source_t *input_source, unsigned int input_buffer_max,
		unsigned int max_output_packet_size);

void __livemedia_h264or5_fragmenter__deinit(livemedia_h264or5_fragmenter_t *h264or5_fragmenter);

void __livemedia_h264or5_fragmenter__free(livemedia_h264or5_fragmenter_t *h264or5_fragmenter);
/*
 * new() and delete() functions
 */
livemedia_h264or5_fragmenter_t *livemedia__new__h264or5_fragmenter(int h_number,
		livemedia_framed_source_t *input_source, unsigned int input_buffer_max,
		unsigned int max_output_packet_size);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264or5_fragmenter__do_get_next_frame__impl(livemedia_framed_source_t *framed_source);
void livemedia_h264or5_fragmenter__do_stop_getting_frames__impl(livemedia_framed_source_t *framed_source);
void livemedia_h264or5_fragmenter__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
bool livemedia_h264or5_fragmenter__last_fragment_completed_nal_unit(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter);

/*
 * Normal functions
 */
void livemedia_h264or5_fragmenter__after_getting_frame__static(void *client_data,
		unsigned int frame_size, 
		unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds);

void livemedia_h264or5_fragmenter__after_getting_frame1(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter,
		unsigned int frame_size, 
		unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds);

void livemedia_h264or5_fragmenter__reset(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter);

/*****************************************************************
 * H264or5Fragmenter:FramedFilter (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_fragmenter_t *__livemedia_h264or5_fragmenter__alloc(void)
{
	livemedia_h264or5_fragmenter_t *h264or5_fragmenter;
	h264or5_fragmenter = (livemedia_h264or5_fragmenter_t *)calloc(1, sizeof(livemedia_h264or5_fragmenter_t));
	if (h264or5_fragmenter == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264or5_fragmenter_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264or5_fragmenter;
}
void __livemedia_h264or5_fragmenter__init(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter, int h_number,
		livemedia_framed_source_t *input_source, unsigned int input_buffer_max,
		unsigned int max_output_packet_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* parent init */
	__livemedia_framed_filter__init(&h264or5_fragmenter->framed_filter, input_source);

	/* constructor */
	h264or5_fragmenter->h_number = h_number;
	h264or5_fragmenter->input_buffer_size = input_buffer_max + 1;
	h264or5_fragmenter->max_output_packet_size = max_output_packet_size;

	h264or5_fragmenter->input_buffer = 
		(unsigned char *)malloc(sizeof(unsigned char) * (h264or5_fragmenter->input_buffer_size));
	if (h264or5_fragmenter->input_buffer == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char) * (h264or5_fragmenter->input_buffer_size))"
				"failed in %s()\n", __func__);
	}

	livemedia_h264or5_fragmenter__reset(h264or5_fragmenter);
	
	/* virtual functions configuration */ 

	((livemedia_framed_source_t *)h264or5_fragmenter)->do_get_next_frame = 
		livemedia_h264or5_fragmenter__do_get_next_frame__impl;

	((livemedia_framed_source_t *)h264or5_fragmenter)->do_stop_getting_frames = 
		livemedia_h264or5_fragmenter__do_stop_getting_frames__impl;

	((livemedia_medium_t *)h264or5_fragmenter)->delete = livemedia_h264or5_fragmenter__delete__impl;
}
void __livemedia_h264or5_fragmenter__deinit(livemedia_h264or5_fragmenter_t *h264or5_fragmenter)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (h264or5_fragmenter->input_buffer) {
		free(h264or5_fragmenter->input_buffer);
		h264or5_fragmenter->input_buffer = NULL;
	}

	livemedia_framed_filter__detach_input_source((livemedia_framed_filter_t *)h264or5_fragmenter);
	/* parent deinit */
	__livemedia_framed_filter__deinit(&h264or5_fragmenter->framed_filter);
}
void __livemedia_h264or5_fragmenter__free(livemedia_h264or5_fragmenter_t *h264or5_fragmenter)
{
	if (h264or5_fragmenter) {
		free(h264or5_fragmenter);
		h264or5_fragmenter = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264or5_fragmenter_t *livemedia__new__h264or5_fragmenter(int h_number,
		livemedia_framed_source_t *input_source, unsigned int input_buffer_max,
		unsigned int max_output_packet_size)
{
	livemedia_h264or5_fragmenter_t *h264or5_fragmenter;

	h264or5_fragmenter = __livemedia_h264or5_fragmenter__alloc();
	if (h264or5_fragmenter)
		__livemedia_h264or5_fragmenter__init(h264or5_fragmenter,
				h_number, input_source, input_buffer_max,
				max_output_packet_size);

	return h264or5_fragmenter;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264or5_fragmenter__do_get_next_frame__impl(livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_fragmenter_t *h264or5_fragmenter;
	livemedia_framed_filter_t *framed_filter;
	unsigned int num_extra_header_bytes;
	unsigned int num_bytes_to_send;

	h264or5_fragmenter = (livemedia_h264or5_fragmenter_t *)framed_source;
	framed_filter = (livemedia_framed_filter_t *)framed_source;

	if (h264or5_fragmenter->num_valid_data_bytes == 1) {
    /* We have no NAL unit data currently in the buffer.  Read a new one: */

	livemedia_framed_source__get_next_frame(framed_filter->input_source,
			&h264or5_fragmenter->input_buffer[1], h264or5_fragmenter->input_buffer_size-1,
			livemedia_h264or5_fragmenter__after_getting_frame__static, framed_source,
			livemedia_framed_source__handle_closure__static, framed_source);
	} else {
		/* We have NAL unit data in the buffer.  There are three cases to consider:
		 * 1. There is a new NAL unit in the buffer, and it's small enough to deliver
		 *    to the RTP sink (as is).
		 * 2. There is a new NAL unit in the buffer, but it's too large to deliver to
		 *    the RTP sink in its entirety.  Deliver the first fragment of this data,
		 *    as a FU packet, with one extra preceding header byte (for the "FU header").
		 * 3. There is a NAL unit in the buffer, and we've already delivered some
		 *    fragment(s) of this.  Deliver the next fragment of this data,
		 *    as a FU packet, with two (H.264) or three (H.265) extra preceding header bytes
		 *    (for the "NAL header" and the "FU header"). */

		if (framed_source->max_size < h264or5_fragmenter->max_output_packet_size) {
			/* shouldn't happen */
			fprintf(stderr, "H264or5Fragmenter::doGetNextFrame(): fMaxSize (%u)" 
					"is smaller than expected\n", framed_source->max_size);
		} else {
			framed_source->max_size = h264or5_fragmenter->max_output_packet_size;
		}

		h264or5_fragmenter->last_fragment_completed_nal_unit = true; /* by default */
		if (h264or5_fragmenter->cur_data_offset == 1) { /* case 1 or 2 */
			if (h264or5_fragmenter->num_valid_data_bytes - 1 <= framed_source->max_size) {
				/* case 1 */
				memmove(framed_source->to, &h264or5_fragmenter->input_buffer[1], 
						h264or5_fragmenter->num_valid_data_bytes - 1);
				framed_source->frame_size = h264or5_fragmenter->num_valid_data_bytes -1;
				h264or5_fragmenter->cur_data_offset = h264or5_fragmenter->num_valid_data_bytes;
			} else { /* case 2 */
				/* We need to send the NAL unit data as FU packets.  Deliver the first
				 * packet now.  Note that we add "NAL header" and "FU header" bytes to the front
				 * of the packet (overwriting the existing "NAL header"). */
				if (h264or5_fragmenter->h_number == 264) {
					h264or5_fragmenter->input_buffer[0] = 
						(h264or5_fragmenter->input_buffer[1] & 0xE0) | 28; /* FU indicator */
					h264or5_fragmenter->input_buffer[1] = 0x80 | (h264or5_fragmenter->input_buffer[1] & 0x1F);
					/* FU header (with S bit) */
				} else { /* 265 */
					u_int8_t nal_unit_type = (h264or5_fragmenter->input_buffer[1]&0x7E)>>1;
					h264or5_fragmenter->input_buffer[0] = (h264or5_fragmenter->input_buffer[1] & 0x81) | (49<<1);
					/* Payload header (1st byte) */
					h264or5_fragmenter->input_buffer[1] = h264or5_fragmenter->input_buffer[2]; 
					/* Payload header (2nd byte) */
					h264or5_fragmenter->input_buffer[2] = 0x80 | nal_unit_type; 
					/* FU header (with S bit) */
				}
				memmove(framed_source->to, h264or5_fragmenter->input_buffer, framed_source->max_size);
				framed_source->frame_size = framed_source->max_size;
				h264or5_fragmenter->cur_data_offset += framed_source->max_size - 1;
				h264or5_fragmenter->last_fragment_completed_nal_unit = false;
			}
		} else { /* case 3 */
			/* We are sending this NAL unit data as FU packets.  We've already sent the
			 * first packet (fragment).  Now, send the next fragment.  Note that we add
			 * "NAL header" and "FU header" bytes to the front.  (We reuse these bytes that
			 * we already sent for the first fragment, but clear the S bit, and add the E
			 * bit if this is the last fragment.) */
			if (h264or5_fragmenter->h_number == 264) {
				h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset-2] =
					h264or5_fragmenter->input_buffer[0]; /* FU indicator */
				h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset-1] = 
					h264or5_fragmenter->input_buffer[1]&~0x80; /* FU header (no S bit) */
				num_extra_header_bytes = 2;
			} else { /* 265 */
				h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset-3] = 
					h264or5_fragmenter->input_buffer[0]; /* Payload header (1st byte) */
				h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset-2] = 
					h264or5_fragmenter->input_buffer[1]; /* Payload header (2nd byte) */
				h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset-1] = 
					h264or5_fragmenter->input_buffer[2]&~0x80; /* FU header (no S bit) */
				num_extra_header_bytes = 3;
			}
			num_bytes_to_send = num_extra_header_bytes + 
				(h264or5_fragmenter->num_valid_data_bytes + h264or5_fragmenter->cur_data_offset);
			if (num_bytes_to_send > framed_source->max_size) {
				/* We can't send all of the remaining data this time: */
				num_bytes_to_send = framed_source->max_size;
				h264or5_fragmenter->last_fragment_completed_nal_unit = false;
			} else {
				/* This is the last fragment: */
				h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset-1] |= 0x40;
				/* set the E bit in the FU header */

				framed_source->num_truncated_bytes = h264or5_fragmenter->save_num_truncated_bytes;
			}
			memmove(framed_source->to, 
					&h264or5_fragmenter->input_buffer[h264or5_fragmenter->cur_data_offset - num_extra_header_bytes], 
					num_bytes_to_send);
			framed_source->frame_size = num_bytes_to_send;
			h264or5_fragmenter->cur_data_offset += num_bytes_to_send - num_extra_header_bytes;
		}
		if (h264or5_fragmenter->cur_data_offset >= h264or5_fragmenter->num_valid_data_bytes) {
			/* We're done with this data.  Reset the pointers for receiving new data: */
			h264or5_fragmenter->num_valid_data_bytes = h264or5_fragmenter->cur_data_offset = 1;
		}

		/* Complete delivery to the client: */
		livemedia_framed_source__after_getting__static(framed_source);
	}
}
void livemedia_h264or5_fragmenter__do_stop_getting_frames__impl(livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Make sure that we don't have any stale data fragments lying around,
   * should we later resume: */
	livemedia_h264or5_fragmenter_t *h264or5_fragmenter;
	h264or5_fragmenter = (livemedia_h264or5_fragmenter_t *)framed_source;

	livemedia_h264or5_fragmenter__reset(h264or5_fragmenter);
	livemedia_framed_filter__do_stop_getting_frames__impl(framed_source);

}
void livemedia_h264or5_fragmenter__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_fragmenter_t *h264or5_fragmenter;

	h264or5_fragmenter = (livemedia_h264or5_fragmenter_t *)medium;
	if (h264or5_fragmenter) {
		__livemedia_h264or5_fragmenter__deinit(h264or5_fragmenter);
		__livemedia_h264or5_fragmenter__free(h264or5_fragmenter);
	}
}

/*
 * Functions in header file
 */
bool livemedia_h264or5_fragmenter__last_fragment_completed_nal_unit(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return h264or5_fragmenter->last_fragment_completed_nal_unit;

}

/*
 * Normal functions
 */
void livemedia_h264or5_fragmenter__after_getting_frame__static(void *client_data,
		unsigned int frame_size, 
		unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_fragmenter_t *fragmenter;
	
	fragmenter = (livemedia_h264or5_fragmenter_t *)client_data;
	livemedia_h264or5_fragmenter__after_getting_frame1(fragmenter,
			frame_size, num_truncated_bytes, presentation_time,
			duration_in_microseconds);

}

void livemedia_h264or5_fragmenter__after_getting_frame1(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter,
		unsigned int frame_size, 
		unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	h264or5_fragmenter->num_valid_data_bytes += frame_size;
	h264or5_fragmenter->save_num_truncated_bytes = num_truncated_bytes;
	((livemedia_framed_source_t *)h264or5_fragmenter)->presentation_time = 
		presentation_time;
	((livemedia_framed_source_t *)h264or5_fragmenter)->duration_in_microseconds =
		duration_in_microseconds;

	/* Deliver data to the client: */
	livemedia_framed_source__do_get_next_frame(
			(livemedia_framed_source_t *)h264or5_fragmenter);	
}

void livemedia_h264or5_fragmenter__reset(
		livemedia_h264or5_fragmenter_t *h264or5_fragmenter)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	h264or5_fragmenter->num_valid_data_bytes = h264or5_fragmenter->cur_data_offset = 1;
	h264or5_fragmenter->save_num_truncated_bytes = 0;
	h264or5_fragmenter->last_fragment_completed_nal_unit = true;
}
/*****************************************************************
 * H264or5VideoRTPSink:VideoRTPSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_video_rtp_sink_t *__livemedia_h264or5_video_rtp_sink__alloc(void)
{
	livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink;
	h264or5_video_rtp_sink = (livemedia_h264or5_video_rtp_sink_t *)calloc(1, sizeof(livemedia_h264or5_video_rtp_sink_t));
	if (h264or5_video_rtp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264or5_video_rtp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264or5_video_rtp_sink;
}
void __livemedia_h264or5_video_rtp_sink__init(
		livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink,
		int h_number, /* 264 or 265 */
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *vps, unsigned int vps_size,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size)
{
	__livemedia_video_rtp_sink__init(&h264or5_video_rtp_sink->video_rtp_sink,
			rtp_gs, rtp_payload_format, 90000, h_number == 264 ? "H264" : "H265");

	h264or5_video_rtp_sink->h_number = h_number;
	h264or5_video_rtp_sink->our_fragmenter = NULL;
	h264or5_video_rtp_sink->fmtp_sdp_line = NULL;
	if (vps != NULL) {
		h264or5_video_rtp_sink->vps_size = vps_size;
		h264or5_video_rtp_sink->vps= (u_int8_t *)malloc(
				sizeof(u_int8_t)*(h264or5_video_rtp_sink->vps_size));
		if(h264or5_video_rtp_sink->vps == NULL) {
			fprintf(stderr, "malloc(sizeof(u_int8_t)*(h264or5_video_rtp_sink->vps_size))"
					"failed in %s()\n", __func__);
		}
		memmove(h264or5_video_rtp_sink->vps, vps, h264or5_video_rtp_sink->vps_size);
	} else {
		h264or5_video_rtp_sink->vps_size = 0;
		h264or5_video_rtp_sink->vps= NULL;
	}
	if (sps != NULL) {
		h264or5_video_rtp_sink->sps_size = sps_size;
		h264or5_video_rtp_sink->sps= (u_int8_t *)malloc(
				sizeof(u_int8_t)*(h264or5_video_rtp_sink->sps_size));
		if(h264or5_video_rtp_sink->sps == NULL) {
			fprintf(stderr, "malloc(sizeof(u_int8_t)*(h264or5_video_rtp_sink->sps_size))"
					"failed in %s()\n", __func__);
		}
		memmove(h264or5_video_rtp_sink->sps, sps, h264or5_video_rtp_sink->sps_size);
	} else {
		h264or5_video_rtp_sink->sps_size = 0;
		h264or5_video_rtp_sink->sps= NULL;
	}
	if (pps != NULL) {
		h264or5_video_rtp_sink->pps_size = pps_size;
		h264or5_video_rtp_sink->pps= (u_int8_t *)malloc(
				sizeof(u_int8_t)*(h264or5_video_rtp_sink->pps_size));
		if(h264or5_video_rtp_sink->pps == NULL) {
			fprintf(stderr, "malloc(sizeof(u_int8_t)*(h264or5_video_rtp_sink->pps_size))"
					"failed in %s()\n", __func__);
		}
		memmove(h264or5_video_rtp_sink->pps, pps, h264or5_video_rtp_sink->pps_size);
	} else {
		h264or5_video_rtp_sink->pps_size = 0;
		h264or5_video_rtp_sink->pps= NULL;
	}

	
	/* virtual functions configuration */ 
	((livemedia_media_sink_t *)h264or5_video_rtp_sink)->continue_playing = 
		livemedia_h264or5_video_rtp_sink__continue_playing__impl;

	((livemedia_multi_framed_rtp_sink_t *)h264or5_video_rtp_sink)->do_special_frame_handling =
		livemedia_h264or5_video_rtp_sink__do_special_frame_handling__impl;

	((livemedia_multi_framed_rtp_sink_t *)h264or5_video_rtp_sink)->frame_can_appear_after_packet_start =
		livemedia_h264or5_video_rtp_sink__frame_can_appear_after_packet_start__impl;

	((livemedia_medium_t *)h264or5_video_rtp_sink)->delete = livemedia_h264or5_video_rtp_sink__delete__impl;
}
void __livemedia_h264or5_video_rtp_sink__deinit(livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	((livemedia_media_sink_t *)h264or5_video_rtp_sink)->source = 
		(livemedia_framed_source_t *)h264or5_video_rtp_sink->our_fragmenter;
	/* hack: in case "fSource" had gotten set to NULL before we were called */

	if (h264or5_video_rtp_sink->fmtp_sdp_line) {
		free(h264or5_video_rtp_sink->fmtp_sdp_line);
		h264or5_video_rtp_sink->fmtp_sdp_line = NULL;
	}
	if (h264or5_video_rtp_sink->vps) {
		free(h264or5_video_rtp_sink->vps);
		h264or5_video_rtp_sink->vps = NULL;
	}
	if (h264or5_video_rtp_sink->sps) {
		free(h264or5_video_rtp_sink->sps);
		h264or5_video_rtp_sink->sps = NULL;
	}
	if (h264or5_video_rtp_sink->pps) {
		free(h264or5_video_rtp_sink->pps);
		h264or5_video_rtp_sink->pps = NULL;
	}

	livemedia_media_sink__stop_playing((livemedia_media_sink_t *)h264or5_video_rtp_sink);
		/* call this now, because we won't have our 'fragmenter' when the base h264or5_fragmenter destructor calls it later.; */

  /* Close our 'fragmenter' as well: */
	livemedia_medium__close_by_medium_ptr__static(
			(livemedia_medium_t *)(h264or5_video_rtp_sink->our_fragmenter));

	((livemedia_media_sink_t *)h264or5_video_rtp_sink)->source = NULL;

	__livemedia_video_rtp_sink__deinit(&h264or5_video_rtp_sink->video_rtp_sink);
/* for the base h264or5_fragmenter destructor, which gets called next */
}
void __livemedia_h264or5_video_rtp_sink__free(livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink)
{
	if (h264or5_video_rtp_sink) {
		free(h264or5_video_rtp_sink);
		h264or5_video_rtp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264or5_video_rtp_sink_t *livemedia__new__h264or5_video_rtp_sink(
		int h_number, /* 264 or 265 */
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *vps, unsigned int vps_size,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink;

	h264or5_video_rtp_sink = __livemedia_h264or5_video_rtp_sink__alloc();
	if (h264or5_video_rtp_sink)
		__livemedia_h264or5_video_rtp_sink__init(h264or5_video_rtp_sink,
				h_number, rtp_gs, rtp_payload_format,
				vps, vps_size,
				sps, sps_size,
				pps, pps_size);

	return h264or5_video_rtp_sink;
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_h264or5_video_rtp_sink__continue_playing__impl(
		livemedia_media_sink_t *media_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink;

	/* First, check whether we have a 'fragmenter' class set up yet.
	 * If not, create it now: */
	h264or5_video_rtp_sink = (livemedia_h264or5_video_rtp_sink_t *)media_sink;
	if (h264or5_video_rtp_sink->our_fragmenter == NULL) {
		h264or5_video_rtp_sink->our_fragmenter = (livemedia_framed_filter_t *)livemedia__new__h264or5_fragmenter(
				h264or5_video_rtp_sink->h_number, media_sink->source, out_packet_buffer__max_size,
				livemedia_multi_framed_rtp_sink__our_max_packet_size(
					(livemedia_multi_framed_rtp_sink_t *)media_sink) - 12 /*RTP hdr size */);
	} else {
		livemedia_framed_filter__reassign_input_source(h264or5_video_rtp_sink->our_fragmenter, 
			media_sink->source);	
	}
	media_sink->source = (livemedia_framed_source_t *)h264or5_video_rtp_sink->our_fragmenter;

	/* Then call the parent class's implementation: */
	/* Send the first packet.
	 * (This will also schedule any future sends.) */
	livemedia_multi_framed_rtp_sink__build_and_send_packet((livemedia_multi_framed_rtp_sink_t *)media_sink, true);
	return true;
}

void livemedia_h264or5_video_rtp_sink__do_special_frame_handling__impl(
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
  /* Set the RTP 'M' (marker) bit iff
   * 1/ The most recently delivered fragment was the end of (or the only fragment of) an NAL unit, and
   * 2/ This NAL unit was the last NAL unit of an 'access unit' (i.e. video frame). */

	livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink;
	livemedia_h264or5_video_stream_framer_t *framer_source;

	h264or5_video_rtp_sink = (livemedia_h264or5_video_rtp_sink_t *)multi_framed_rtp_sink;
	if (h264or5_video_rtp_sink->our_fragmenter != NULL) {
		framer_source = (livemedia_h264or5_video_stream_framer_t *)(
				livemedia_framed_filter__input_source(
					h264or5_video_rtp_sink->our_fragmenter));
		/* This relies on our fragmenter's source being a "H264or5VideoStreamFramer". */
		if (livemedia_h264or5_fragmenter__last_fragment_completed_nal_unit(
					(livemedia_h264or5_fragmenter_t *)h264or5_video_rtp_sink->our_fragmenter) &&
			framer_source != NULL &&
				*livemedia_mpeg_video_stream_framer__picture_end_marker(
						(livemedia_mpeg_video_stream_framer_t *)framer_source)) {
			livemedia_multi_framed_rtp_sink__set_marker_bit(multi_framed_rtp_sink);	
			*livemedia_mpeg_video_stream_framer__picture_end_marker(
					(livemedia_mpeg_video_stream_framer_t *)framer_source) = false;
		}
	}
	livemedia_multi_framed_rtp_sink__set_timestamp(multi_framed_rtp_sink,
			frame_presentation_time);
}

bool livemedia_h264or5_video_rtp_sink__frame_can_appear_after_packet_start__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *frame_start, unsigned int num_bytes_in_frame)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false;
}

void livemedia_h264or5_video_rtp_sink__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink;
	h264or5_video_rtp_sink = (livemedia_h264or5_video_rtp_sink_t *)medium;

	if (h264or5_video_rtp_sink) {
		__livemedia_h264or5_video_rtp_sink__deinit(h264or5_video_rtp_sink);
		__livemedia_h264or5_video_rtp_sink__free(h264or5_video_rtp_sink);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */
