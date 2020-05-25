#include "video_rtp_sink.h"

/*****************************************************************
 * Video_RTPSink:MultiFramedRTPSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_video_rtp_sink_t *__livemedia_video_rtp_sink__alloc(void)
{
	livemedia_video_rtp_sink_t *video_rtp_sink;
	video_rtp_sink = (livemedia_video_rtp_sink_t *)calloc(1, sizeof(livemedia_video_rtp_sink_t));
	if (video_rtp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_video_rtp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return video_rtp_sink;
}
void __livemedia_video_rtp_sink__init(livemedia_video_rtp_sink_t *video_rtp_sink,
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name)
{
	__livemedia_multi_framed_rtp_sink__init(&video_rtp_sink->multi_framed_rtp_sink,
			rtpgs, rtp_payload_type, rtp_timestamp_frequency, rtp_payload_format_name, 1);

	/* virtual functions configuration */ 
	((livemedia_rtp_sink_t *)video_rtp_sink)->sdp_media_type =
		livemedia_video_rtp_sink__sdp_media_type__impl;

	((livemedia_medium_t *)video_rtp_sink)->delete = 
		livemedia_video_rtp_sink__delete__impl;
}
void __livemedia_video_rtp_sink__deinit(livemedia_video_rtp_sink_t *video_rtp_sink)
{
	__livemedia_multi_framed_rtp_sink__deinit(&video_rtp_sink->multi_framed_rtp_sink);
}

void __livemedia_video_rtp_sink__free(livemedia_video_rtp_sink_t *video_rtp_sink)
{
	if (video_rtp_sink) {
		free(video_rtp_sink);
		video_rtp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_video_rtp_sink_t *livemedia__new__video_rtp_sink(
		groupsock_groupsock_t *rtpgs, unsigned char rtp_payload_type,
		unsigned int rtp_timestamp_frequency,
		char const *rtp_payload_format_name)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_video_rtp_sink_t *video_rtp_sink;

	video_rtp_sink = __livemedia_video_rtp_sink__alloc();
	if (video_rtp_sink)
		__livemedia_video_rtp_sink__init(video_rtp_sink, rtpgs,
				rtp_payload_type, rtp_timestamp_frequency,
				rtp_payload_format_name);

	return video_rtp_sink;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char *livemedia_video_rtp_sink__sdp_media_type__impl(
		livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return "video";
}
void livemedia_video_rtp_sink__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_video_rtp_sink_t *video_rtp_sink;

	video_rtp_sink = (livemedia_video_rtp_sink_t *)medium;

	if (video_rtp_sink) {
		__livemedia_video_rtp_sink__deinit(video_rtp_sink);
		__livemedia_video_rtp_sink__free(video_rtp_sink);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

