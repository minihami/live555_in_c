
#ifndef __LIVEMEDIA_MEDIA_SINK_H__
#define __LIVEMEDIA_MEDIA_SINK_H__
/*****************************************************************
 * MediaSink:Medium (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_media_sink_t {
	/* TODO: parent structure */

	/* TODO: member variables */

	/* TODO: virtual function pointers */

} livemedia_media_sink_t;

/*****************************************************************
 * MediaSink:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_media_sink_t *__livemedia_media_sink__alloc(void);
void __livemedia_media_sink__init(livemedia_media_sink_t *media_sink);
void __livemedia_media_sink__deinit(livemedia_media_sink_t *media_sink);
void __livemedia_media_sink__free(livemedia_media_sink_t *media_sink);
/*
 * new() and delete() functions
 */
livemedia_media_sink_t *livemedia__new__media_sink(void);
void livemedia__delete__media_sink(livemedia_media_sink_t *media_sink);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_media_sink__delete__impl(livemedia_media_sink_t *media_sink);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
