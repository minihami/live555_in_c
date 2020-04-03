#ifndef __LIVEMEDIA_FRAMED_SOURCE_H__
#define __LIVEMEDIA_FRAMED_SOURCE_H__

#include "../groupsock/net_common.h"
#include "media_source.h"
/*****************************************************************
 * FramedSource:MediaSource (Structure Declarations)
 ****************************************************************/
typedef void (*livemedia_framed_source__after_getting_func)(void *client_data,
		unsigned int frame_size, unsigned int num_truncated_bytes, 
		struct timeval presentation_time, unsigned int duration_in_microseconds);
typedef void (*livemedia_framed_source__on_close_func)(void *client_data);
typedef struct _livemedia_framed_source_t {
	livemedia_media_source_t media_source;

  /* The following variables are typically accessed/set by doGetNextFrame() */
	unsigned char *to; /* in */
	unsigned int max_size; /* in */
	unsigned int frame_size;  /* out */
	unsigned int num_truncated_bytes; /* out */
	struct timeval presentation_time; /* out */
	unsigned int duration_in_microseconds; /* out */
	livemedia_framed_source__after_getting_func *after_getting_func;
	void *after_getting_client_data;
	livemedia_framed_source__on_close_func *on_close_func;
	void *on_close_client_data;
	bool is_currently_awaiting_data;

	unsigned int (*max_frame_size)(struct _livemedia_framed_source_t *framed_source);
	void (*do_get_next_frame)(struct _livemedia_framed_source_t *framed_source);
	void (*do_stop_getting_frames)(struct _livemedia_framed_source_t *framed_source);

} livemedia_framed_source_t;

/*****************************************************************
 * FramedSource:MediaSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_framed_source_t *__livemedia_framed_source__alloc(void);
void __livemedia_framed_source__init(livemedia_framed_source_t *framed_source);
void __livemedia_framed_source__deinit(livemedia_framed_source_t *framed_source);
void __livemedia_framed_source__free(livemedia_framed_source_t *framed_source);
/*
 * new() and delete() functions
 */
livemedia_framed_source_t *livemedia__new__framed_source(void);
/*
 * Functions for virtual functions
 */
unsigned int livemedia_framed_source__max_frame_size(
		livemedia_framed_source_t *framed_source);
void livemedia_framed_source__do_get_next_frame(
		livemedia_framed_source_t *framed_source);
void livemedia_framed_source__do_stop_getting_frames(
		livemedia_framed_source_t *framed_source);

/*
 * Override functions
 */
unsigned int livemedia_framed_source__max_frame_size__impl(
		livemedia_framed_source_t *framed_source);
void livemedia_framed_source__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source);
bool livemedia_framed_source__is_framed_source__impl(
		livemedia_media_source_t *media_source);
void livemedia_framed_source__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

bool livemedia_framed_source__is_currently_awaiting_data(
		livemedia_framed_source_t *framed_source);
/*
 * Normal functions
 */
bool livemedia_framed_source__lookup_by_name__static(char const *source_name,
		livemedia_framed_source_t **result_source);
void livemedia_framed_source__get_next_frame(livemedia_framed_source_t *framed_source,
		unsigned char *to, unsigned int max_size,
		livemedia_framed_source__after_getting_func *after_getting_func,
		void *after_getting_client_data,
		livemedia_framed_source__on_close_func *on_close_func,
		void *on_close_client_data);
void livemedia_framed_source__handle_closure__static(void *client_data);
void livemedia_framed_source__handle_closure(livemedia_framed_source_t *framed_source);
void livemedia_framed_source__stop_getting_frames(
		livemedia_framed_source_t *framed_source);
void livemedia_framed_source__after_getting__static(livemedia_framed_source_t *source);


#endif
