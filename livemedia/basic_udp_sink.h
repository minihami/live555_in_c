#ifndef __LIVEMEDIA_BASIC_UDP_SINK_H__
#define __LIVEMEDIA_BASIC_UDP_SINK_H__

#include "media_sink.h"
#include "../groupsock/groupsock.h"
/*****************************************************************
 * BasicUDPSink:MediaSink (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_basic_udp_sink_t {
	livemedia_media_sink_t media_sink;

	/* member variables */
	groupsock_groupsock_t *gs;
	unsigned int max_payload_size;
	unsigned char *output_buffer;
	struct timeval next_send_time;

} livemedia_basic_udp_sink_t;

/*****************************************************************
 * BasicUDPSink:MediaSink (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_basic_udp_sink_t *__livemedia_basic_udp_sink__alloc(void);
void __livemedia_basic_udp_sink__init(livemedia_basic_udp_sink_t *basic_udp_sink, 
		groupsock_groupsock_t *gs, unsigned int max_payload_size);
void __livemedia_basic_udp_sink__deinit(livemedia_basic_udp_sink_t *basic_udp_sink);
void __livemedia_basic_udp_sink__free(livemedia_basic_udp_sink_t *basic_udp_sink);
/*
 * new() and delete() functions
 */
livemedia_basic_udp_sink_t *livemedia__new__basic_udp_sink(groupsock_groupsock_t *gs,
		unsigned int max_payload_size);
      /* called only by createNew() */
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_basic_udp_sink__continue_playing__impl(livemedia_media_sink_t *media_sink);
void livemedia_basic_udp_sink__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_basic_udp_sink_t *livemedia_basic_udp_sink__create_new__static(
		groupsock_groupsock_t *gs, unsigned int max_payload_size);

void livemedia_basic_udp_sink__continue_playing1(
		livemedia_basic_udp_sink_t *basic_udp_sink);

void livemedia_basic_udp_sink__after_getting_frame__static(void *client_data, 
		unsigned int frame_size, unsigned int num_truncated_bytes,
		struct timeval presentation_time,
		unsigned int duration_in_microseconds);
void livemedia_basic_udp_sink__after_getting_frame1(
		livemedia_basic_udp_sink_t *basic_udp_sink, unsigned int frame_size,
		unsigned int num_truncated_bytes, unsigned int duration_in_microseconds);

void livemedia_basic_udp_sink__send_next__static(void * first_arg);

#endif
