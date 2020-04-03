
#ifndef __LIVEMEDIA_RTP_SINK_H__
#define __LIVEMEDIA_RTP_SINK_H__
/*****************************************************************
 * RTPSink:MediaSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtp_sink_t {
	/* TODO: parent structure */

	/* TODO: member variables */

	/* TODO: virtual function pointers */

} livemedia_rtp_sink_t;

/*****************************************************************
 * RTPSink:MediaSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_sink_t *__livemedia_rtp_sink__alloc(void);
void __livemedia_rtp_sink__init(livemedia_rtp_sink_t *rtp_sink);
void __livemedia_rtp_sink__deinit(livemedia_rtp_sink_t *rtp_sink);
void __livemedia_rtp_sink__free(livemedia_rtp_sink_t *rtp_sink);
/*
 * new() and delete() functions
 */
livemedia_rtp_sink_t *livemedia__new__rtp_sink(void);
void livemedia__delete__rtp_sink(livemedia_rtp_sink_t *rtp_sink);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_sink__delete__impl(livemedia_rtp_sink_t *rtp_sink);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

#endif
