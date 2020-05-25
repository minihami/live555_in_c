#include "framed_source.h"
#include <stdlib.h>

/*****************************************************************
 * FramedSource:MediaSource (Structure Definitions)
 ****************************************************************/

/*****************************************************************
 * FramedSource:MediaSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_framed_source_t *__livemedia_framed_source__alloc(void)
{
	livemedia_framed_source_t *framed_source;
	framed_source = (livemedia_framed_source_t *)calloc(1, sizeof(livemedia_framed_source_t));
	if (framed_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_framed_source_t)) failed in %s()\n", __func__);
		return NULL;
	}
	return framed_source;
}
void __livemedia_framed_source__init(livemedia_framed_source_t *framed_source)
{
	__livemedia_media_source__init(&framed_source->media_source);

	framed_source->after_getting_func = NULL;
	framed_source->after_getting_client_data = NULL;
	framed_source->on_close_func = NULL;
	framed_source->on_close_client_data = NULL;
	framed_source->is_currently_awaiting_data = false;
	framed_source->presentation_time.tv_sec = framed_source->presentation_time.tv_usec
		= 0; /* initially */

	framed_source->max_frame_size = livemedia_framed_source__max_frame_size__impl;
	framed_source->do_get_next_frame = NULL;
	framed_source->do_stop_getting_frames =
		livemedia_framed_source__do_stop_getting_frames__impl;
	((livemedia_media_source_t *)framed_source)->is_framed_source =
		livemedia_framed_source__is_framed_source__impl;
	((livemedia_medium_t *)framed_source)->delete = livemedia_framed_source__delete__impl;
}
void __livemedia_framed_source__deinit(livemedia_framed_source_t *framed_source)
{
	__livemedia_media_source__deinit(&framed_source->media_source);
}
void __livemedia_framed_source__free(livemedia_framed_source_t *framed_source)
{
	if (framed_source) {
		free(framed_source);
		framed_source = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_framed_source_t *livemedia__new__framed_source(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_source_t *framed_source;

	framed_source = __livemedia_framed_source__alloc();
	if (framed_source)
		__livemedia_framed_source__init(framed_source);

	return framed_source;
}
/*
 * Functions for virtual functions
 */
unsigned int livemedia_framed_source__max_frame_size(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if(framed_source->max_frame_size)
		return framed_source->max_frame_size(framed_source);
	return 0;
}
void livemedia_framed_source__do_get_next_frame(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if(framed_source->do_get_next_frame)
		return framed_source->do_get_next_frame(framed_source);
	return ;

}
void livemedia_framed_source__do_stop_getting_frames(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if(framed_source->do_stop_getting_frames)
		return framed_source->do_stop_getting_frames(framed_source);
	return ;
}

/*
 * Override functions
 */
unsigned int livemedia_framed_source__max_frame_size__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* By default, this source has no maximum frame size. */
	return 0;
}
void livemedia_framed_source__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Default implementation: Do nothing except cancel any pending 'delivery' task: */
	uv_timer_stop(((livemedia_medium_t *)framed_source)->next_task);
  /* Subclasses may wish to redefine this function. */
	return ;
}
bool livemedia_framed_source__is_framed_source__impl(
		livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true;
}
void livemedia_framed_source__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_source_t *framed_source;

	framed_source = (livemedia_framed_source_t *)medium;
	if (framed_source) {
		__livemedia_framed_source__deinit(framed_source);
		__livemedia_framed_source__free(framed_source);
	}
}

/*
 * Functions in header file
 */

bool livemedia_framed_source__is_currently_awaiting_data(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return framed_source->is_currently_awaiting_data;
}
/*
 * Normal functions
 */
bool livemedia_framed_source__lookup_by_name__static(char const *source_name,
		livemedia_framed_source_t **result_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	*result_source = NULL; /* unless we succeed */
	livemedia_media_source_t *source;
	if (!livemedia_media_source__lookup_by_name__static(source_name, &source))
		return false;
	if (!livemedia_media_source__is_framed_source(source)){
		fprintf(stderr, "%s is not a framed source in %s()\n", source_name, __func__);
		return false;
	}
	*result_source = (livemedia_framed_source_t *)source;
	return true;
}
void livemedia_framed_source__get_next_frame(livemedia_framed_source_t *framed_source,
		unsigned char *to, unsigned int max_size,
		livemedia_framed_source__after_getting_func *after_getting_func,
		void *after_getting_client_data,
		livemedia_framed_source__on_close_func *on_close_func,
		void *on_close_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Make sure we're not already being read: */
	if (framed_source->is_currently_awaiting_data) {
		fprintf(stderr, "FramedSource[%p] %s(): attempting to read more than once at the"
				" same time!\n", framed_source, __func__);
	}

	framed_source->to = to;
	framed_source->max_size = max_size;
	framed_source->num_truncated_bytes = 0;
	framed_source->duration_in_microseconds = 0;
	framed_source->after_getting_func = after_getting_func;
	framed_source->after_getting_client_data = after_getting_client_data;
	framed_source->on_close_func = on_close_func;
	framed_source->on_close_client_data = on_close_client_data;
	framed_source->is_currently_awaiting_data = true;

	livemedia_framed_source__do_get_next_frame(framed_source);
}
void livemedia_framed_source__handle_closure__static(void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_source_t *source;

	source = (livemedia_framed_source_t *)client_data;
	livemedia_framed_source__handle_closure(source);
}
void livemedia_framed_source__handle_closure(livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	framed_source->is_currently_awaiting_data = false; /* because we got a close instead */
	if (framed_source->on_close_func != NULL)
		(*(framed_source->on_close_func))(framed_source->on_close_client_data);
}
void livemedia_framed_source__stop_getting_frames(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	framed_source->is_currently_awaiting_data = false;
	/* indicates that we can be read again */
	framed_source->after_getting_func = NULL;
	framed_source->on_close_func = NULL;

	/* Perform any specialized action now: */
	livemedia_framed_source__do_stop_getting_frames(framed_source);
}
void livemedia_framed_source__after_getting__static(livemedia_framed_source_t *source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	source->is_currently_awaiting_data = false;
	/* indicates that we can be read again
	 * Note that this needs to be done here, in case the "fAfterFunc"
	 * called below tries to read another frame (which it usually will) */

	if (source->after_getting_func != NULL) {
		(*(source->after_getting_func))(source->after_getting_client_data,
				source->frame_size, source->num_truncated_bytes,
				source->presentation_time, source->duration_in_microseconds);
	}
}
