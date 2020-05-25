#ifndef __LIVEMEDIA_VIDEO_RTP_SINK_H__
#define __LIVEMEDIA_VIDEO_RTP_SINK_H__
#include "multi_framed_rtp_sink.h"

/*****************************************************************
 * Video_RTPSink:MultiFramedRTPSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_video_rtp_sink_t {
	livemedia_multi_framed_rtp_sink_t multi_framed_rtp_sink;

} livemedia_video_rtp_sink_t;

/*****************************************************************
 * Video_RTPSink:MultiFramedRTPSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_video_rtp_sink_t *__livemedia_video_rtp_sink__alloc(void);
void __livemedia_video_rtp_sink__init(livemedia_video_rtp_sink_t *video_rtp_sink,
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name);
void __livemedia_video_rtp_sink__deinit(livemedia_video_rtp_sink_t *video_rtp_sink);
void __livemedia_video_rtp_sink__free(livemedia_video_rtp_sink_t *video_rtp_sink);
/*
 * new() and delete() functions
 */
livemedia_video_rtp_sink_t *livemedia__new__video_rtp_sink(
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name);
/* (we're an abstract base class) */

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char *livemedia_video_rtp_sink__sdp_media_type__impl(
		livemedia_rtp_sink_t *rtp_sink);
void livemedia_video_rtp_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
