#include "h264_video_stream_framer.h"

/*****************************************************************
 * H264VideoStreamFramer:H264or5VideoStreamFramer (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_stream_framer_t *__livemedia_h264_video_stream_framer__alloc(void)
{
	livemedia_h264_video_stream_framer_t *h264_video_stream_framer;
	h264_video_stream_framer = (livemedia_h264_video_stream_framer_t *)calloc(1, sizeof(livemedia_h264_video_stream_framer_t));
	if (h264_video_stream_framer == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264_video_stream_framer_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264_video_stream_framer;
}
void __livemedia_h264_video_stream_framer__init(
		livemedia_h264_video_stream_framer_t *h264_video_stream_framer,
		livemedia_framed_source_t *input_source,
		bool create_parser,
		bool include_start_code_in_output,
		bool insert_access_unit_delimiters)
{
	__livemedia_h264or5_video_stream_framer__init(
			&h264_video_stream_framer->h264or5_video_stream_framer, 264, input_source,
			create_parser, include_start_code_in_output, insert_access_unit_delimiters);

	((livemedia_media_source_t *)h264_video_stream_framer)->is_h264_video_stream_framer =
		livemedia_h264_video_stream_framer__is_h264_video_stream_framer__impl;

	((livemedia_medium_t *)h264_video_stream_framer)->delete = 
		livemedia_h264_video_stream_framer__delete__impl;
}
void __livemedia_h264_video_stream_framer__deinit(livemedia_h264_video_stream_framer_t *h264_video_stream_framer)
{
	__livemedia_h264or5_video_stream_framer__deinit(
			&h264_video_stream_framer->h264or5_video_stream_framer);
}
void __livemedia_h264_video_stream_framer__free(livemedia_h264_video_stream_framer_t *h264_video_stream_framer)
{
	if (h264_video_stream_framer) {
		free(h264_video_stream_framer);
		h264_video_stream_framer = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264_video_stream_framer_t *livemedia__new__h264_video_stream_framer(
		livemedia_framed_source_t *input_source,
		bool create_parser,
		bool include_start_code_in_output,
		bool insert_access_unit_delimiters)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_stream_framer_t *h264_video_stream_framer;

	h264_video_stream_framer = __livemedia_h264_video_stream_framer__alloc();
	if (h264_video_stream_framer)
		__livemedia_h264_video_stream_framer__init(h264_video_stream_framer,
				input_source, create_parser, include_start_code_in_output, 
				insert_access_unit_delimiters);

	return h264_video_stream_framer;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_h264_video_stream_framer__is_h264_video_stream_framer__impl(
		livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true;
}

void livemedia_h264_video_stream_framer__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_stream_framer_t *h264_video_stream_framer;
	h264_video_stream_framer = (livemedia_h264_video_stream_framer_t *)medium;

	if (h264_video_stream_framer) {
		__livemedia_h264_video_stream_framer__deinit(h264_video_stream_framer);
		__livemedia_h264_video_stream_framer__free(h264_video_stream_framer);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_h264_video_stream_framer_t *livemedia_h264_video_stream_framer__create_new__static(
		livemedia_framed_source_t *input_source,
		bool include_start_code_in_output, /*defalut: false */
		bool insert_access_unit_delimiters /*defalut: false */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia__new__h264_video_stream_framer(input_source, true,
			include_start_code_in_output, insert_access_unit_delimiters);
}
/*
 * Static functions implementation for ancestor
 */
