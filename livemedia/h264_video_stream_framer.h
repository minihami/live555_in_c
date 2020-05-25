
#ifndef __LIVEMEDIA_H264_VIDEO_STREAM_FRAMER_H__
#define __LIVEMEDIA_H264_VIDEO_STREAM_FRAMER_H__
#include "h264or5_video_stream_framer.h"
/*****************************************************************
 * H264VideoStreamFramer:H264or5VideoStreamFramer (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264_video_stream_framer_t {
	livemedia_h264or5_video_stream_framer_t h264or5_video_stream_framer;

} livemedia_h264_video_stream_framer_t;

/*****************************************************************
 * H264VideoStreamFramer:H264or5VideoStreamFramer (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_stream_framer_t *__livemedia_h264_video_stream_framer__alloc(void);

void __livemedia_h264_video_stream_framer__init(
		livemedia_h264_video_stream_framer_t *h264_video_stream_framer,
		livemedia_framed_source_t *input_source,
		bool create_parser,
		bool include_start_code_in_output,
		bool insert_access_unit_delimiters);

void __livemedia_h264_video_stream_framer__deinit(livemedia_h264_video_stream_framer_t *h264_video_stream_framer);

void __livemedia_h264_video_stream_framer__free(livemedia_h264_video_stream_framer_t *h264_video_stream_framer);

/*
 * new() and delete() functions
 */
livemedia_h264_video_stream_framer_t *livemedia__new__h264_video_stream_framer(
		livemedia_framed_source_t *input_source,
		bool create_parser,
		bool include_start_code_in_output,
		bool insert_access_unit_delimiters);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_h264_video_stream_framer__is_h264_video_stream_framer__impl(
		livemedia_media_source_t *media_source);

void livemedia_h264_video_stream_framer__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_h264_video_stream_framer_t *livemedia_h264_video_stream_framer__create_new__static(
		livemedia_framed_source_t *input_source,
		bool include_start_code_in_output,
		bool insert_access_unit_delimiters);

#endif
