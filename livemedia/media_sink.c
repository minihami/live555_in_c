#include "media_sink.h"

/*****************************************************************
 * MediaSink:Medium (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_media_sink_t *__livemedia_media_sink__alloc(void)
{
	livemedia_media_sink_t *media_sink;
	media_sink = (livemedia_media_sink_t *)calloc(1, sizeof(livemedia_media_sink_t));
	if (media_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_media_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return media_sink;
}
void __livemedia_media_sink__init(livemedia_media_sink_t *media_sink)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	media_sink->delete = livemedia_media_sink__delete__impl;
}
void __livemedia_media_sink__deinit(livemedia_media_sink_t *media_sink)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_media_sink__free(livemedia_media_sink_t *media_sink)
{
	if (media_sink) {
		free(media_sink);
		media_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_media_sink_t *livemedia__new__media_sink(void)
{
	livemedia_media_sink_t *media_sink;

	media_sink = __livemedia_media_sink__alloc();
	if (media_sink)
		__livemedia_media_sink__init(media_sink);

	return media_sink;
}
void livemedia__delete__media_sink(livemedia_media_sink_t *media_sink)
{
	if (media_sink && media_sink->delete)
		media_sink->delete(media_sink);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_media_sink__delete__impl(livemedia_media_sink_t *media_sink)
{
	if (media_sink) {
		__livemedia_media_sink__deinit(media_sink);
		__livemedia_media_sink__free(media_sink);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */
