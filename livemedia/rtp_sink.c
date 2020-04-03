#include "rtp_sink.h"

/*****************************************************************
 * RTPSink:MediaSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_sink_t *__livemedia_rtp_sink__alloc(void)
{
	livemedia_rtp_sink_t *rtp_sink;
	rtp_sink = (livemedia_rtp_sink_t *)calloc(1, sizeof(livemedia_rtp_sink_t));
	if (rtp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_sink;
}
void __livemedia_rtp_sink__init(livemedia_rtp_sink_t *rtp_sink)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	rtp_sink->delete = livemedia_rtp_sink__delete__impl;
}
void __livemedia_rtp_sink__deinit(livemedia_rtp_sink_t *rtp_sink)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_rtp_sink__free(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink) {
		free(rtp_sink);
		rtp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_sink_t *livemedia__new__rtp_sink(void)
{
	livemedia_rtp_sink_t *rtp_sink;

	rtp_sink = __livemedia_rtp_sink__alloc();
	if (rtp_sink)
		__livemedia_rtp_sink__init(rtp_sink);

	return rtp_sink;
}
void livemedia__delete__rtp_sink(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink && rtp_sink->delete)
		rtp_sink->delete(rtp_sink);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_sink__delete__impl(livemedia_rtp_sink_t *rtp_sink)
{
	if (rtp_sink) {
		__livemedia_rtp_sink__deinit(rtp_sink);
		__livemedia_rtp_sink__free(rtp_sink);
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
