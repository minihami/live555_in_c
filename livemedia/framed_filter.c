#include "framed_filter.h"

/*****************************************************************
 * FramedFilter:FramedSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_framed_filter_t *__livemedia_framed_filter__alloc(void)
{
	livemedia_framed_filter_t *framed_filter;
	framed_filter = (livemedia_framed_filter_t *)calloc(1, sizeof(livemedia_framed_filter_t));
	if (framed_filter == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_framed_filter_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return framed_filter;
}
void __livemedia_framed_filter__init(livemedia_framed_filter_t *framed_filter,
		livemedia_framed_source_t *input_source)
{
	__livemedia_framed_source__init(&framed_filter->framed_source);

	framed_filter->input_source = input_source;

	/* virtual functions configuration */ 
	((livemedia_media_source_t *)framed_filter)->mime_type = 
		livemedia_framed_filter__mime_type__impl;

	((livemedia_media_source_t *)framed_filter)->get_attributes = 
		livemedia_framed_filter__get_attributes__impl;

	((livemedia_framed_source_t *)framed_filter)->do_stop_getting_frames = 
		livemedia_framed_filter__do_stop_getting_frames__impl;

	((livemedia_medium_t *)framed_filter)->delete = livemedia_framed_filter__delete__impl;
}
void __livemedia_framed_filter__deinit(livemedia_framed_filter_t *framed_filter)
{
	livemedia_medium__close_by_medium_ptr__static(
			(livemedia_medium_t *)framed_filter->input_source);

	__livemedia_framed_source__deinit(&framed_filter->framed_source);
}
void __livemedia_framed_filter__free(livemedia_framed_filter_t *framed_filter)
{
	if (framed_filter) {
		free(framed_filter);
		framed_filter = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_framed_filter_t *livemedia__new__framed_filter(
		livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_filter_t *framed_filter;

	framed_filter = __livemedia_framed_filter__alloc();
	if (framed_filter)
		__livemedia_framed_filter__init(framed_filter, input_source);

	return framed_filter;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */

char const* livemedia_framed_filter__mime_type__impl(
		livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */
}

void livemedia_framed_filter__get_attributes__impl(
		livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */

}

void livemedia_framed_filter__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_framed_filter_t *framed_filter;

	framed_filter = (livemedia_framed_filter_t *)framed_source;
	livemedia_framed_source__do_stop_getting_frames__impl(framed_source);
	if (framed_filter->input_source != NULL)
		livemedia_framed_source__stop_getting_frames(framed_filter->input_source);
		
}

void livemedia_framed_filter__delete__impl(livemedia_medium_t *medium)
{
	livemedia_framed_filter_t *framed_filter;

	framed_filter = (livemedia_framed_filter_t *)medium;
	if (framed_filter) {
		__livemedia_framed_filter__deinit(framed_filter);
		__livemedia_framed_filter__free(framed_filter);
	}
}

/*
 * Functions in header file
 */
livemedia_framed_source_t *livemedia_framed_filter__input_source(
		livemedia_framed_filter_t *framed_filter)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return framed_filter->input_source;
}

livemedia_framed_source_t *livemedia_framed_filter__reassign_input_source(
		livemedia_framed_filter_t *framed_filter, 
		livemedia_framed_source_t *new_input_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	framed_filter->input_source = new_input_source;
}

/*
 * Normal functions
 */

livemedia_framed_source_t *livemedia_framed_filter__detach_input_source( 
		livemedia_framed_filter_t *framed_filter)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (framed_filter->input_source != NULL) {
		livemedia_framed_source__stop_getting_frames(framed_filter->input_source);
		livemedia_framed_filter__reassign_input_source(framed_filter, NULL);
	}
}
