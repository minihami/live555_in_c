
#ifndef __LIVEMEDIA_H264_OR_5_VIDEO_STREAM_FRAMER_H__
#define __LIVEMEDIA_H264_OR_5_VIDEO_STREAM_FRAMER_H__

#ifndef __LIVEMEDIA_MPEG_VIDEO_STREAM_FRAMER_H__
#include "mpeg_video_stream_framer.h"
#endif

/*****************************************************************
 * H264or5VideoStreamFramer:MPEGVideoStreamFramer (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264or5_video_stream_framer_t {
	livemedia_mpeg_video_stream_framer_t mpeg_video_stream_framer;

	int h_number;
	bool include_start_code_in_output, insert_access_unit_delimiters;
	u_int8_t *last_seen_vps;
	unsigned int last_seen_vps_size;
	u_int8_t *last_seen_sps;
	unsigned int last_seen_sps_size;
	u_int8_t *last_seen_pps;
	unsigned int last_seen_pps_size;
	struct timeval next_presentation_time;
	/* the presentation time to be used for the next NAL unit
	 * to be parsed/delivered after this */
} livemedia_h264or5_video_stream_framer_t;

/*****************************************************************
 * H264or5VideoStreamFramer:MPEGVideoStreamFramer (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_video_stream_framer_t *__livemedia_h264or5_video_stream_framer__alloc(void);
void __livemedia_h264or5_video_stream_framer__init(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		int number /* 264 or 265 */, livemedia_framed_source_t *input_source,
		bool create_parser, bool include_start_code_in_output,
		bool insert_access_unit_delimiters);
void __livemedia_h264or5_video_stream_framer__deinit(livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer);
void __livemedia_h264or5_video_stream_framer__free(livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer);
/*
 * new() and delete() functions
 */
livemedia_h264or5_video_stream_framer_t *livemedia__new__h264or5_video_stream_framer(
		int number /* 264 or 265 */, livemedia_framed_source_t *input_source,
		bool create_parser, bool include_start_code_in_output,
		bool insert_access_unit_delimiters);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264or5_video_stream_framer__do_get_next_frame__impl(
	livemedia_framed_source_t *framed_source);
void livemedia_h264or5_video_stream_framer__delete__impl(
	livemedia_medium_t *medium);

/*
 * Functions in header file
 */
void livemedia_h264or5_video_stream_framer__get_vps_and_sps_and_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t **vps, unsigned int *vps_size, u_int8_t **sps, unsigned int *sps_size,
		u_int8_t **pps, unsigned int *pps_size);
void livemedia_h264or5_video_stream_framer__set_vps_and_sps_and_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *vps, unsigned int vps_size, u_int8_t *sps, unsigned int sps_size,
		u_int8_t *pps, unsigned int pps_size);

/*
 * Normal functions
 */
void livemedia_h264or5_video_stream_framer__save_copy_of_vps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *from, unsigned int size);
void livemedia_h264or5_video_stream_framer__save_copy_of_sps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *from, unsigned int size);
void livemedia_h264or5_video_stream_framer__save_copy_of_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *from, unsigned int size);

void livemedia_h264or5_video_stream_framer__set_presentation_time(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer);

bool livemedia_h264or5_video_stream_framer__is_vps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_framer__is_sps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_framer__is_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_framer__is_vcl(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type);


unsigned int livemedia__remove_h264or5_emulation_bytes(u_int8_t *to, unsigned int to_max_size,
		u_int8_t *from, unsigned int from_size);
#endif
