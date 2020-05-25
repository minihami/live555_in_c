#ifndef __LIVEMEDIA_FRAMED_FILTER_H__
#define __LIVEMEDIA_FRAMED_FILTER_H__
#include "framed_source.h"

/*****************************************************************
 * FramedFilter:FramedSource (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_framed_filter_t {
	livemedia_framed_source_t framed_source;

	livemedia_framed_source_t *input_source;

} livemedia_framed_filter_t;

/*****************************************************************
 * FramedFilter:FramedSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_framed_filter_t *__livemedia_framed_filter__alloc(void);
void __livemedia_framed_filter__init(livemedia_framed_filter_t *framed_filter,
		livemedia_framed_source_t *input_source);
void __livemedia_framed_filter__deinit(livemedia_framed_filter_t *framed_filter);
void __livemedia_framed_filter__free(livemedia_framed_filter_t *framed_filter);
/*
 * new() and delete() functions
 */
livemedia_framed_filter_t *livemedia__new__framed_filter(
		livemedia_framed_source_t *input_source);
/* abstract base class */

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char const* livemedia_framed_filter__mime_type__impl(
		livemedia_media_source_t *media_source);

void livemedia_framed_filter__get_attributes__impl(
		livemedia_media_source_t *media_source);

void livemedia_framed_filter__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source);

void livemedia_framed_filter__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
livemedia_framed_source_t *livemedia_framed_filter__input_source(
		livemedia_framed_filter_t *framed_filter);

livemedia_framed_source_t *livemedia_framed_filter__reassign_input_source(
		livemedia_framed_filter_t *framed_filter, 
		livemedia_framed_source_t *new_input_source);

/*
 * Normal functions
 */

  /* Call before destruction if you want to prevent the destructor from closing the input source */
livemedia_framed_source_t *livemedia_framed_filter__detach_input_source( 
		livemedia_framed_filter_t *framed_filter);

#endif
