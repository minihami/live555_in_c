#ifndef __LIVEMEDIA_H264_OR_5_VIDEO_RTP_SINK_H__
#define __LIVEMEDIA_H264_OR_5_VIDEO_RTP_SINK_H__
#include "video_rtp_sink.h"
#include "framed_filter.h"

/*****************************************************************
 * H264or5VideoRTPSink:VideoRTPSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264or5_video_rtp_sink_t {
	livemedia_video_rtp_sink_t video_rtp_sink;

	int h_number;
	livemedia_framed_filter_t *our_fragmenter;
	char *fmtp_sdp_line;
	u_int8_t *vps;
	unsigned int vps_size;
	u_int8_t *sps;
	unsigned int sps_size;
	u_int8_t *pps;
	unsigned int pps_size;

} livemedia_h264or5_video_rtp_sink_t;

/*****************************************************************
 * H264or5VideoRTPSink:VideoRTPSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_video_rtp_sink_t *__livemedia_h264or5_video_rtp_sink__alloc(void);

void __livemedia_h264or5_video_rtp_sink__init(
		livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink,
		int h_number, /* 264 or 265 */
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *vps, unsigned int vps_size,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size);

void __livemedia_h264or5_video_rtp_sink__deinit(
		livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink);

void __livemedia_h264or5_video_rtp_sink__free(
		livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink);
/*
 * new() and delete() functions
 */
livemedia_h264or5_video_rtp_sink_t *livemedia__new__h264or5_video_rtp_sink(
		int h_number, /* 264 or 265 */
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *vps, unsigned int vps_size,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
/* redefined virtual functions: */
bool livemedia_h264or5_video_rtp_sink__continue_playing__impl(
		livemedia_media_sink_t *media_sink);

void livemedia_h264or5_video_rtp_sink__do_special_frame_handling__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned int fragmentation_offset, 
		unsigned char *frame_start,
		unsigned int num_bytes_in_frame,
		struct timeval frame_presentation_time,
		unsigned int num_remaining_bytes);

bool livemedia_h264or5_video_rtp_sink__frame_can_appear_after_packet_start__impl(
		livemedia_multi_framed_rtp_sink_t *multi_framed_rtp_sink,
		unsigned char const *frame_start, unsigned int num_bytes_in_frame);

void livemedia_h264or5_video_rtp_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
