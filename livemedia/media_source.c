#include "media_source.h"

/*****************************************************************
 * MediaSource:Medium (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_media_source_t *__livemedia_media_source__alloc(void)
{
	livemedia_media_source_t *media_source;
	media_source = (livemedia_media_source_t *)calloc(1, sizeof(livemedia_media_source_t));
	if (media_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_media_source_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return media_source;
}
void __livemedia_media_source__init(livemedia_media_source_t *media_source)
{
	__livemedia_medium__init(&media_source->medium);
	
	media_source->get_attributes = livemedia_media_source__get_attributes__impl;
	media_source->mime_type = livemedia_media_source__mime_type__impl;
	media_source->is_framed_source = livemedia_media_source__is_framed_source__impl;
	media_source->is_rtp_source = livemedia_media_source__is_rtp_source__impl;
	media_source->is_mpeg1or2_video_stream_framer = 
		livemedia_media_source__is_mpeg1or2_video_stream_framer__impl;
	media_source->is_mpeg4_video_stream_framer = 
		livemedia_media_source__is_mpeg4_video_stream_framer__impl;
	media_source->is_h264_video_stream_framer = 
		livemedia_media_source__is_h264_video_stream_framer__impl;
	media_source->is_h265_video_stream_framer = 
		livemedia_media_source__is_h265_video_stream_framer__impl;
	media_source->is_dv_video_stream_framer = 
		livemedia_media_source__is_dv_video_stream_framer__impl;
	media_source->is_jpeg_video_source = livemedia_media_source__is_jpeg_video_source__impl;
	media_source->is_amr_audio_source = livemedia_media_source__is_amr_audio_source__impl;
	media_source->is_mpeg2_transport_stream_multiplexor = 
		livemedia_media_source__is_mpeg2_transport_stream_multiplexor__impl;
	media_source->medium.is_source = livemedia_media_source__is_source__impl;
	media_source->medium.delete = livemedia_media_source__delete__impl;
}
void __livemedia_media_source__deinit(livemedia_media_source_t *media_source)
{
	__livemedia_medium__deinit(&media_source->medium);
}
void __livemedia_media_source__free(livemedia_media_source_t *media_source)
{
	if (media_source) {
		free(media_source);
		media_source = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_media_source_t *livemedia__new__media_source(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_media_source_t *media_source;

	media_source = __livemedia_media_source__alloc();
	if (media_source)
		__livemedia_media_source__init(media_source);

	return media_source;
}
/*
 * Functions for virtual functions
 */
void livemedia_media_source__get_attributes(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->get_attributes)
		return media_source->get_attributes(media_source);
	return;
}
char const *livemedia_media_source__mime_type(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->mime_type)
		return media_source->mime_type(media_source);
	return NULL;
}
bool livemedia_media_source__is_framed_source(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_framed_source)
		return media_source->is_framed_source(media_source);
	return false;
}
bool livemedia_media_source__is_rtp_source(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_rtp_source)
		return media_source->is_rtp_source(media_source);
	return false;
}
bool livemedia_media_source__is_mpeg1or2_video_stream_framer(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_mpeg1or2_video_stream_framer)
		return media_source->is_mpeg1or2_video_stream_framer(media_source);
	return false;
}
bool livemedia_media_source__is_mpeg4_video_stream_framer(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_mpeg4_video_stream_framer)
		return media_source->is_mpeg4_video_stream_framer(media_source);
	return false;
}
bool livemedia_media_source__is_h264_video_stream_framer(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_h264_video_stream_framer)
		return media_source->is_h264_video_stream_framer(media_source);
	return false;
}
bool livemedia_media_source__is_h265_video_stream_framer(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_h265_video_stream_framer)
		return media_source->is_h265_video_stream_framer(media_source);
	return false;
}
bool livemedia_media_source__is_dv_video_stream_framer(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_dv_video_stream_framer)
		return media_source->is_dv_video_stream_framer(media_source);
	return false;
}
bool livemedia_media_source__is_jpeg_video_source(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_jpeg_video_source)
		return media_source->is_jpeg_video_source(media_source);
	return false;
}
bool livemedia_media_source__is_amr_audio_source(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_amr_audio_source)
		return media_source->is_amr_audio_source(media_source);
	return false;
}
bool livemedia_media_source__is_mpeg2_transport_stream_multiplexor(
		struct _livemedia_media_source_t *media_source)
{
	if(media_source->is_mpeg2_transport_stream_multiplexor)
		return media_source->is_mpeg2_transport_stream_multiplexor(media_source);
	return false;
}

/*
 * Override functions
 */
void livemedia_media_source__get_attributes__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
char const *livemedia_media_source__mime_type__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  return "application/OCTET-STREAM"; // default type
}
bool livemedia_media_source__is_framed_source__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_rtp_source__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_mpeg1or2_video_stream_framer__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_mpeg4_video_stream_framer__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_h264_video_stream_framer__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_h265_video_stream_framer__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_dv_video_stream_framer__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_jpeg_video_source__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_amr_audio_source__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_mpeg2_transport_stream_multiplexor__impl(
		struct _livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false; /* default implementation */
}
bool livemedia_media_source__is_source__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true;
}
void livemedia_media_source__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_media_source_t *media_source;

	media_source = (livemedia_media_source_t *)medium;
	
	if (media_source) {
		__livemedia_media_source__deinit(media_source);
		__livemedia_media_source__free(media_source);
	}
}


/*
 * Functions in header file
 */

/*
 * Normal functions
 */
bool livemedia_media_source__lookup_by_name__static(char const* source_name,
		livemedia_media_source_t **result_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	*result_source = NULL; /* unless we succeed */

	livemedia_medium_t *medium;
	if (!livemedia_medium__lookup_by_name__static(source_name, &medium)) 
		return false;

	if (!livemedia_medium__is_source(medium)) {
		fprintf(stderr, "%s is no a media source in %s()", source_name, __func__);
		return false;
	}

	*result_source = (livemedia_media_source_t *)medium;
	return true;
}

