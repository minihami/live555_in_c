#ifndef __LIVEMEDIA_MPEG_VIDEO_STREAM_PARSER_H__
#define __LIVEMEDIA_MPEG_VIDEO_STREAM_PARSER_H__

#ifndef __LIVEMEDIA_STREAM_PARSER_H__
#include "stream_parser.h"
#endif
#ifndef _MPEG_VIDEO_STREAM_FRAMER_HH
#include "mpeg_video_stream_framer.h"
#endif

/*****************************************************************
 * MPEGVideoStreamParser:StreamParser (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_mpeg_video_stream_parser_t {
	livemedia_stream_parser_t stream_parser;

	livemedia_mpeg_video_stream_framer_t *using_source;

	/* state of the frame that's currently being read */
	unsigned char *start_of_frame;
	unsigned char *to;
	unsigned char *limit;
	unsigned int num_truncated_bytes;
	unsigned char *saved_to;
	unsigned int saved_num_truncated_bytes;

	unsigned int (*parse)(
			struct _livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);
} livemedia_mpeg_video_stream_parser_t;

/*****************************************************************
 * MPEGVideoStreamParser:StreamParser (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mpeg_video_stream_parser_t *__livemedia_mpeg_video_stream_parser__alloc(void);
void __livemedia_mpeg_video_stream_parser__init(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser,
		livemedia_mpeg_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source);
void __livemedia_mpeg_video_stream_parser__deinit(livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);
void __livemedia_mpeg_video_stream_parser__free(livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);
/*
 * new() and delete() functions
 */
livemedia_mpeg_video_stream_parser_t *livemedia__new__mpeg_video_stream_parser(
		livemedia_mpeg_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source);
/*
 * Functions for virtual functions
 */
unsigned int livemedia_mpeg_video_stream_parser__parse(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);

/*
 * Override functions
 */
void livemedia_mpeg_video_stream_parser__restore_saved_parser_state__impl(
		livemedia_stream_parser_t *stream_parser);
void livemedia_mpeg_video_stream_parser__delete__impl(livemedia_stream_parser_t *stream_parser);

/*
 * Functions in header file
 */
unsigned int livemedia_mpeg_video_stream_parser__num_truncated_bytes(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);
void livemedia_mpeg_video_stream_parser__set_parse_state(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);
/* Record "byte" in the current output frame: */
void livemedia_mpeg_video_stream_parser__save_byte(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int8_t byte);
void livemedia_mpeg_video_stream_parser__save4_bytes(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int32_t word);
/* Save data until we see a sync word (0x000001xx): */
void livemedia_mpeg_video_stream_parser__save_to_next_code(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int32_t *cur_word);
/* Skip data until we see a sync word (0x000001xx): */
void livemedia_mpeg_video_stream_parser__skip_to_next_code(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int32_t *cur_word);

unsigned int livemedia_mpeg_video_stream_parser__cur_frame_size(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);

/*
 * Normal functions
 */
void livemedia_mpeg_video_stream_parser__register_read_interest(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser,
		unsigned char *to, unsigned int max_size);

#endif
