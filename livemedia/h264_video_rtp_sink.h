#ifndef __LIVEMEDIA_H264_VIDEO_RTP_SINK_H__
#define __LIVEMEDIA_H264_VIDEO_RTP_SINK_H__

#include "h264or5_video_rtp_sink.h"

/*****************************************************************
 * H264VideoRTPSink:H264or5VideoRTPSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264_video_rtp_sink_t {
	livemedia_h264or5_video_rtp_sink_t h264or5_video_rtp_sink;

} livemedia_h264_video_rtp_sink_t;

/*****************************************************************
 * H264VideoRTPSink:H264or5VideoRTPSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_rtp_sink_t *__livemedia_h264_video_rtp_sink__alloc(void);
void __livemedia_h264_video_rtp_sink__init(
		livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink, 
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size);

void __livemedia_h264_video_rtp_sink__deinit(livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink);
void __livemedia_h264_video_rtp_sink__free(livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink);
/*
 * new() and delete() functions
 */
livemedia_h264_video_rtp_sink_t *livemedia__new__h264_video_rtp_sink(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size);
/* called only by createNew() */

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char *livemedia_h264_video_rtp_sink__aux_sdp_line__impl(
		livemedia_rtp_sink_t *rtp_sink);
bool livemedia_h264_video_rtp_sink__source_is_compatible_with_us__impl(
		livemedia_media_sink_t *media_sink, livemedia_media_source_t *source);
void livemedia_h264_video_rtp_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_h264_video_rtp_sink_t *livemedia_h264_video_rtp_sink__create_new__static(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format);

livemedia_h264_video_rtp_sink_t *livemedia_h264_video_rtp_sink__create_new_by_sps_and_pps__static(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size);
    /* an optional variant of "createNew()", useful if we know, in advance,
     * the stream's SPS and PPS NAL units.
     * This avoids us having to 'pre-read' from the input source in order to get these values. */

livemedia_h264_video_rtp_sink_t *livemedia_h264_video_rtp_sink__create_new_by_prop_parameter(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		char const *s_prop_parameter_sets_str);
    /* an optional variant of "createNew()", useful if we know, in advance,
     * the stream's SPS and PPS NAL units.
     * This avoids us having to 'pre-read' from the input source in order to get these values. */
#endif
