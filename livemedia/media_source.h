#ifndef __LIVEMEDIA_MEDIA_SOURCE_H__
#define __LIVEMEDIA_MEDIA_SOURCE_H__

#include "media.h"

/*****************************************************************
 * MediaSource:Medium (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_media_source_t {
	livemedia_medium_t medium;

	/* TODO: virtual function pointers */
	void (*get_attributes)(struct _livemedia_media_source_t *media_source);
	char const *(*mime_type)(struct _livemedia_media_source_t *media_source);
	bool (*is_framed_source)(struct _livemedia_media_source_t *media_source);
	bool (*is_rtp_source)(struct _livemedia_media_source_t *media_source);
	bool (*is_mpeg1or2_video_stream_framer)(struct _livemedia_media_source_t *media_source);
	bool (*is_mpeg4_video_stream_framer)(struct _livemedia_media_source_t *media_source);
	bool (*is_h264_video_stream_framer)(struct _livemedia_media_source_t *media_source);
	bool (*is_h265_video_stream_framer)(struct _livemedia_media_source_t *media_source);
	bool (*is_dv_video_stream_framer)(struct _livemedia_media_source_t *media_source);
	bool (*is_jpeg_video_source)(struct _livemedia_media_source_t *media_source);
	bool (*is_amr_audio_source)(struct _livemedia_media_source_t *media_source);
	bool (*is_mpeg2_transport_stream_multiplexor)(struct _livemedia_media_source_t *media_source);
} livemedia_media_source_t;

/*****************************************************************
 * MediaSource:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_media_source_t *__livemedia_media_source__alloc(void);
void __livemedia_media_source__init(livemedia_media_source_t *media_source);
void __livemedia_media_source__deinit(livemedia_media_source_t *media_source);
void __livemedia_media_source__free(livemedia_media_source_t *media_source);

/*
 * new() and delete() functions
 */
livemedia_media_source_t *livemedia__new__media_source(void);

/*
 * Functions for virtual functions
 */
void livemedia_media_source__get_attributes(struct _livemedia_media_source_t *media_source);
char const *livemedia_media_source__mime_type(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_framed_source(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_rtp_source(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_mpeg1or2_video_stream_framer(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_mpeg4_video_stream_framer(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_h264_video_stream_framer(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_h265_video_stream_framer(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_dv_video_stream_framer(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_jpeg_video_source(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_amr_audio_source(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_mpeg2_transport_stream_multiplexor(struct _livemedia_media_source_t *media_source);

/*
 * Override functions
 */
void livemedia_media_source__get_attributes__impl(struct _livemedia_media_source_t *media_source);
char const *livemedia_media_source__mime_type__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_framed_source__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_rtp_source__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_mpeg1or2_video_stream_framer__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_mpeg4_video_stream_framer__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_h264_video_stream_framer__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_h265_video_stream_framer__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_dv_video_stream_framer__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_jpeg_video_source__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_amr_audio_source__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_mpeg2_transport_stream_multiplexor__impl(struct _livemedia_media_source_t *media_source);
bool livemedia_media_source__is_source__impl(livemedia_medium_t *medium);
void livemedia_media_source__delete__impl(livemedia_medium_t *medium);


/*
 * Functions in header file
 */

/*
 * Normal functions
 */
bool livemedia_media_source__lookup_by_name__static(char const* source_name,
		livemedia_media_source_t **result_source);

#endif
