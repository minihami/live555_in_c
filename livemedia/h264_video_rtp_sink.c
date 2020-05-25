#include "h264_video_rtp_sink.h"
#include "h264_video_stream_framer.h"
#include "base64.h"
#include "h264_video_rtp_source.h"

/*****************************************************************
 * H264VideoRTPSink:H264or5VideoRTPSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_rtp_sink_t *__livemedia_h264_video_rtp_sink__alloc(void)
{
	livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink;
	h264_video_rtp_sink = (livemedia_h264_video_rtp_sink_t *)calloc(1, sizeof(livemedia_h264_video_rtp_sink_t));
	if (h264_video_rtp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264_video_rtp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264_video_rtp_sink;
}
void __livemedia_h264_video_rtp_sink__init(
		livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink, 
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size)
{
	__livemedia_h264or5_video_rtp_sink__init(&h264_video_rtp_sink->h264or5_video_rtp_sink,
			264, rtp_gs, rtp_payload_format, NULL, 0, sps, sps_size, pps, pps_size);

	/* virtual functions configuration */ 

	((livemedia_rtp_sink_t *)h264_video_rtp_sink)->aux_sdp_line = 
		livemedia_h264_video_rtp_sink__aux_sdp_line__impl;
	((livemedia_media_sink_t *)h264_video_rtp_sink)->source_is_compatible_with_us = 
		livemedia_h264_video_rtp_sink__source_is_compatible_with_us__impl;
	((livemedia_medium_t *)h264_video_rtp_sink)->delete = 
		livemedia_h264_video_rtp_sink__delete__impl;
}
void __livemedia_h264_video_rtp_sink__deinit(
		livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink)
{
	__livemedia_h264or5_video_rtp_sink__deinit(
			&h264_video_rtp_sink->h264or5_video_rtp_sink);
}
void __livemedia_h264_video_rtp_sink__free(
		livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink)
{
	if (h264_video_rtp_sink) {
		free(h264_video_rtp_sink);
		h264_video_rtp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264_video_rtp_sink_t *livemedia__new__h264_video_rtp_sink(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *sps/*defalut: NULL */, unsigned int sps_size/*default: 0*/,
		u_int8_t const *pps/*defalut: NULL */, unsigned int pps_size/*default: 0*/)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink;

	h264_video_rtp_sink = __livemedia_h264_video_rtp_sink__alloc();
	if (h264_video_rtp_sink)
		__livemedia_h264_video_rtp_sink__init(h264_video_rtp_sink, rtp_gs, 
				rtp_payload_format, sps, sps_size, pps, pps_size);

	return h264_video_rtp_sink;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char *livemedia_h264_video_rtp_sink__aux_sdp_line__impl(
		livemedia_rtp_sink_t *rtp_sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Generate a new "a=fmtp:" line each time, using our SPS and PPS (if we have them),
   * otherwise parameters from our framer source (in case they've changed since the last time that
   * we were called): */
	u_int8_t *vps_dummy;
	unsigned int vps_dummy_size;
	u_int8_t *sps;
	unsigned int sps_size;
	u_int8_t *pps;
	unsigned int pps_size;
	u_int8_t *sps_web;
	unsigned int sps_web_size;
	u_int32_t profile_level_id;
	char *sps_base64;
	char *pps_base64;
	char *fmtp_fmt;
	unsigned int fmtp_fmt_size;
	char *fmtp;

	livemedia_h264or5_video_stream_framer_t *framer_source;
	livemedia_h264or5_video_rtp_sink_t *h264or5_video_rtp_sink;

	h264or5_video_rtp_sink = (livemedia_h264or5_video_rtp_sink_t *)rtp_sink;

	framer_source = NULL;
	vps_dummy = NULL;
	vps_dummy_size = 0;
	sps = h264or5_video_rtp_sink->sps;
	sps_size = h264or5_video_rtp_sink->sps_size;
	pps = h264or5_video_rtp_sink->pps;
	pps_size = h264or5_video_rtp_sink->pps_size;
	if (sps ==NULL || pps == NULL) {
    /* We need to get SPS and PPS from our framer source: */
		if (h264or5_video_rtp_sink->our_fragmenter == NULL)
			return NULL;/* we don't yet have a fragmenter (and therefore not a source) */
		framer_source = (livemedia_h264or5_video_stream_framer_t *)(
				livemedia_framed_filter__input_source(
					h264or5_video_rtp_sink->our_fragmenter));
		if (framer_source == NULL)
			return NULL; /* we don't yet have a source */

		livemedia_h264or5_video_stream_framer__get_vps_and_sps_and_pps(
				framer_source, &vps_dummy, &vps_dummy_size, &sps, &sps_size,
				&pps, &pps_size);
		if (sps == NULL || pps == NULL)
			return NULL; /*our source isn't ready */;
	}

	/* Set up the "a=fmtp:" SDP line for this stream: */
	sps_web = (u_int8_t *)malloc(sizeof(u_int8_t) * sps_size);
	/* "WEB" means "Without Emulation Bytes" */
	if (sps_web == NULL) {
		fprintf(stderr, "malloc(sizeof(u_int8_t) * sps_size);"
				"failed in %s()\n", __func__);
	}
	sps_web_size = livemedia__remove_h264or5_emulation_bytes(sps_web, sps_size, 
			sps, sps_size);
	if (sps_web_size < 4) { /* Bad SPS size => assume our source isn't ready */
		
		if (sps_web) {
			free(sps_web);
			sps_web = NULL;
		}
		return NULL;
	}
	profile_level_id = (sps_web[1] << 16) | (sps_web[2] << 8) | sps_web[3];
	if (sps_web) {
		free(sps_web);
		sps_web = NULL;
	}

	sps_base64 = livemedia__base64_encode((char *)sps, sps_size);
	pps_base64 = livemedia__base64_encode((char *)pps, pps_size);

	fmtp_fmt = 
		"a=fmtp:%d packetization-mode=1"
		";profile-level-id=%06X"
		";sprop-parameter-sets=%s,%s\r\n";
	fmtp_fmt_size = strlen(fmtp_fmt)
		+ 3 /* max char len */
		+ 6 /* 3 bytes in hex */
		+ strlen(sps_base64) + strlen(pps_base64);
	fmtp = (char *)malloc(sizeof(char) * fmtp_fmt_size);
	sprintf(fmtp, fmtp_fmt,
				livemedia_rtp_sink__rtp_payload_type(rtp_sink),
				profile_level_id, sps_base64, pps_base64);

	if (sps_base64) {
		free(sps_base64);
		sps_base64 = NULL;
	}
	if (pps_base64) {
		free(pps_base64);
		pps_base64 = NULL;
	}
	if (h264or5_video_rtp_sink->fmtp_sdp_line) {
		free(h264or5_video_rtp_sink->fmtp_sdp_line);
		h264or5_video_rtp_sink->fmtp_sdp_line = NULL;
	}

	h264or5_video_rtp_sink->fmtp_sdp_line = fmtp;
	return h264or5_video_rtp_sink->fmtp_sdp_line;
}

bool livemedia_h264_video_rtp_sink__source_is_compatible_with_us__impl(
		livemedia_media_sink_t *media_sink, livemedia_media_source_t *source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_media_source__is_h264_video_stream_framer(source);
}

void livemedia_h264_video_rtp_sink__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_rtp_sink_t *h264_video_rtp_sink;
	h264_video_rtp_sink = (livemedia_h264_video_rtp_sink_t *)medium;

	if (h264_video_rtp_sink) {
		__livemedia_h264_video_rtp_sink__deinit(h264_video_rtp_sink);
		__livemedia_h264_video_rtp_sink__free(h264_video_rtp_sink);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_h264_video_rtp_sink_t *livemedia_h264_video_rtp_sink__create_new__static(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia__new__h264_video_rtp_sink(rtp_gs, rtp_payload_format,
			NULL, 0, NULL, 0);
}

livemedia_h264_video_rtp_sink_t *livemedia_h264_video_rtp_sink__create_new_by_sps_and_pps__static(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		u_int8_t const *sps, unsigned int sps_size,
		u_int8_t const *pps, unsigned int pps_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia__new__h264_video_rtp_sink(rtp_gs, rtp_payload_format,
			sps, sps_size, pps, pps_size);
}

livemedia_h264_video_rtp_sink_t *livemedia_h264_video_rtp_sink__create_new_by_prop_parameter(
		groupsock_groupsock_t *rtp_gs, unsigned char rtp_payload_format,
		char const *s_prop_parameter_sets_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int8_t *sps;	
	unsigned int sps_size;
	u_int8_t *pps;	
	unsigned int pps_size;
	unsigned int num_s_prop_records;
	livemedia_s_prop_record_t *s_prop_records;
	unsigned int i;
	u_int8_t nal_unit_type;
	livemedia_h264_video_rtp_sink_t *result;


	s_prop_records = livemedia__parse_s_prop_parameter_sets(s_prop_parameter_sets_str,
			&num_s_prop_records);

	for (i = 0; i < num_s_prop_records; ++i) {
		if (s_prop_records[i].s_prop_length == 0)
			continue; /* bad data */
		nal_unit_type = (s_prop_records[i].s_prop_bytes[0]) & 0x1F;
		if(nal_unit_type == 8 /* SPS */) {
			sps = s_prop_records[i].s_prop_bytes;
			sps_size = s_prop_records[i].s_prop_length;
		} else if (nal_unit_type == 8 /* PPS */) {
			pps = s_prop_records[i].s_prop_bytes;
			pps_size = s_prop_records[i].s_prop_length;
		}
	}

	result = livemedia__new__h264_video_rtp_sink(rtp_gs, rtp_payload_format,
			sps, sps_size, pps, pps_size);
	livemedia__delete__s_prop_record_array(s_prop_records);

	return result;
}
