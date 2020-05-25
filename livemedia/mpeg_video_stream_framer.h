#ifndef __LIVEMEDIA_MPEG_VIDEO_STREAM_FRAMER_H__
#define __LIVEMEDIA_MPEG_VIDEO_STREAM_FRAMER_H__

#ifndef __LIVEMEDIA_FRAMED_FILTER_H__
#include "framed_filter.h"
#endif

/*****************************************************************
 * TimeCode (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_time_code_t {
	unsigned int days, hours, minutes, secons, pictures;

	void (*delete)(struct _livemedia_time_code_t *time_code);
} livemedia_time_code_t;

/*****************************************************************
 * MPEGVideoStreamFramer:FramedFilter (Structure Declarations)
 ****************************************************************/
struct _livemedia_mpeg_video_stream_parser_t;

typedef struct _livemedia_mpeg_video_stream_framer_t {
	livemedia_framed_filter_t framed_filter;

	double frame_rate; /* Note: For MPEG-4, this is really a 'tick rate' */
	unsigned int picture_count; /*hack used to implement doGetNextFrame() */
	bool picture_end_marker;
	struct timeval presentation_time_base;

	struct _livemedia_mpeg_video_stream_parser_t *parser;
	livemedia_time_code_t cur_gop_time_code, prev_gop_time_code;
	unsigned int pictures_adjustment;
	double picture_time_base;
	unsigned tc_secs_base;
	bool have_seen_first_time_code;

} livemedia_mpeg_video_stream_framer_t;

/*****************************************************************
 * TimeCode (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_time_code_t *__livemedia_time_code__alloc(void);
void __livemedia_time_code__init(livemedia_time_code_t *time_code);
void __livemedia_time_code__deinit(livemedia_time_code_t *time_code);
void __livemedia_time_code__free(livemedia_time_code_t *time_code);
/*
 * new() and delete() functions
 */
livemedia_time_code_t *livemedia__new__time_code(void);
void livemedia__delete__time_code(livemedia_time_code_t *time_code);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_time_code__delete__impl(livemedia_time_code_t *time_code);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * MPEGVideoStreamFramer:FramedFilter (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mpeg_video_stream_framer_t *__livemedia_mpeg_video_stream_framer__alloc(void);
void __livemedia_mpeg_video_stream_framer__init(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer,
		livemedia_framed_source_t *input_source);
void __livemedia_mpeg_video_stream_framer__deinit(livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer);
void __livemedia_mpeg_video_stream_framer__free(livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer);
/*
 * new() and delete() functions
 */
livemedia_mpeg_video_stream_framer_t *livemedia__new__mpeg_video_stream_framer(
		livemedia_framed_source_t *input_source);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_mpeg_video_stream_framer__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source);
void livemedia_mpeg_video_stream_framer__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source);
void livemedia_mpeg_video_stream_framer__delete__impl(
		livemedia_medium_t *medium);

/*
 * Functions in header file
 */
bool *livemedia_mpeg_video_stream_framer__picture_end_marker(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer);

/*
 * Normal functions
 */
void livemedia_mpeg_video_stream_framer__flush_input(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer);
void livemedia_mpeg_video_stream_framer__compute_presentation_time(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer,
		unsigned num_additional_pictures);
/* sets "fPresentationTime */
void livemedia_mpeg_video_stream_framer__set_time_code(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer,
		unsigned int hours, unsigned int minutes, unsigned int seconds,
		unsigned int pictures, unsigned int pictures_since_last_gop);
void livemedia_mpeg_video_stream_framer__reset(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer);
void livemedia_mpeg_video_stream_framer__continue_read_processing__static(
		void* client_data, unsigned char *ptr, unsigned int size,
		struct timeval presentation_time);
void livemedia_mpeg_video_stream_framer__continue_read_processing(
		livemedia_mpeg_video_stream_framer_t *mpeg_video_stream_framer);

#endif
