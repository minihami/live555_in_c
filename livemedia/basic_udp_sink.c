#include "basic_udp_sink.h"

/*****************************************************************
 * BasicUDPSink:MediaSink (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_basic_udp_sink_t *__livemedia_basic_udp_sink__alloc(void)
{
	livemedia_basic_udp_sink_t *basic_udp_sink;
	basic_udp_sink = (livemedia_basic_udp_sink_t *)calloc(1, sizeof(livemedia_basic_udp_sink_t));
	if (basic_udp_sink == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_basic_udp_sink_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return basic_udp_sink;
}
void __livemedia_basic_udp_sink__init(livemedia_basic_udp_sink_t *basic_udp_sink, 
		groupsock_groupsock_t *gs, unsigned int max_payload_size)
{
	__livemedia_media_sink__init(&basic_udp_sink->media_sink);
	/* constructor */
	basic_udp_sink->gs = gs;
	basic_udp_sink->max_payload_size = max_payload_size;
	basic_udp_sink->output_buffer = (unsigned char *)malloc(sizeof(unsigned char)
			* (basic_udp_sink->max_payload_size));
	if (basic_udp_sink->output_buffer == NULL) {
		fprintf(stderr, "(unsigned char *)malloc(sizeof(unsigned char)"
			"* (basic_media_sink->max_payload_size));"
				"failed in %s()\n", __func__);
	}
	
	/* virtual functions configuration */ 
	((livemedia_media_sink_t *)basic_udp_sink)->continue_playing = 
		livemedia_basic_udp_sink__continue_playing__impl;

	((livemedia_medium_t *)basic_udp_sink)->delete =
		livemedia_basic_udp_sink__delete__impl;
}
void __livemedia_basic_udp_sink__deinit(livemedia_basic_udp_sink_t *basic_udp_sink)
{
	if (basic_udp_sink->output_buffer) {
		free(basic_udp_sink->output_buffer);
		basic_udp_sink->output_buffer = NULL;
	}

	__livemedia_media_sink__deinit(&basic_udp_sink->media_sink);
}
void __livemedia_basic_udp_sink__free(livemedia_basic_udp_sink_t *basic_udp_sink)
{
	if (basic_udp_sink) {
		free(basic_udp_sink);
		basic_udp_sink = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_basic_udp_sink_t *livemedia__new__basic_udp_sink(groupsock_groupsock_t *gs,
		unsigned int max_payload_size)
{
	livemedia_basic_udp_sink_t *basic_udp_sink;

	basic_udp_sink = __livemedia_basic_udp_sink__alloc();
	if (basic_udp_sink)
		__livemedia_basic_udp_sink__init(basic_udp_sink, gs, max_payload_size);

	return basic_udp_sink;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_basic_udp_sink__continue_playing__impl(livemedia_media_sink_t *media_sink)
{
	livemedia_basic_udp_sink_t *basic_udp_sink;

	basic_udp_sink = (livemedia_basic_udp_sink_t *)media_sink;

	/* Record the fact that we're starting to play now: */
	gettimeofday(&basic_udp_sink->next_send_time, NULL);
  
	/* Arrange to get and send the first payload.
	 * (This will also schedule any future sends.) */
	livemedia_basic_udp_sink__continue_playing1(basic_udp_sink);
	return true;
}
void livemedia_basic_udp_sink__delete__impl(livemedia_medium_t *medium)
{
	livemedia_basic_udp_sink_t *basic_udp_sink;

	basic_udp_sink = (livemedia_basic_udp_sink_t *)medium;

	if (basic_udp_sink) {
		__livemedia_basic_udp_sink__deinit(basic_udp_sink);
		__livemedia_basic_udp_sink__free(basic_udp_sink);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_basic_udp_sink_t *livemedia_basic_udp_sink__create_new__static(
		groupsock_groupsock_t *gs, unsigned int max_payload_size /*default: 1450 */)
{
	return livemedia__new__basic_udp_sink(gs, max_payload_size);
}

void livemedia_basic_udp_sink__continue_playing1(
		livemedia_basic_udp_sink_t *basic_udp_sink)
{
	livemedia_media_sink_t *media_sink;

	media_sink = (livemedia_media_sink_t *)basic_udp_sink;

	/*TODO: member variable init????
	livemedia_medium__next_task((livemedia_medium_t *)basic_udp_sink) = NULL;
*/
	basic_udp_sink->media_sink.medium.next_task = NULL;
	if (media_sink->source != NULL) {
		livemedia_framed_source__get_next_frame(media_sink->source, 
				basic_udp_sink->output_buffer, basic_udp_sink->max_payload_size,
				livemedia_basic_udp_sink__after_getting_frame__static, basic_udp_sink, 
				livemedia_media_sink__on_source_closure__static, basic_udp_sink);

	}

}

void livemedia_basic_udp_sink__after_getting_frame__static(void *client_data, 
		unsigned int frame_size, unsigned int num_truncated_bytes,
		struct timeval presentation_time,	/*presentationTime*/
		unsigned int duration_in_microseconds)
{
	livemedia_basic_udp_sink_t *sink = (livemedia_basic_udp_sink_t *)client_data;
	livemedia_basic_udp_sink__after_getting_frame1(sink, frame_size, num_truncated_bytes, duration_in_microseconds);
}
void livemedia_basic_udp_sink__after_getting_frame1(
		livemedia_basic_udp_sink_t *basic_udp_sink, unsigned int frame_size,
		unsigned int num_truncated_bytes, unsigned int duration_in_microseconds)
{
	struct timeval time_now;
	int secs_diff;
	int64_t u_seconds_to_go;

	if(num_truncated_bytes > 0) {
		fprintf(stderr, "BasicUDPSink::afterGettingFrame1(): "
				"The input frame data was too large for our specified maximum payload"
				"size (%u). %u bytes of trailing data was dropped!\n",
				basic_udp_sink->max_payload_size, num_truncated_bytes);
	}

	/* send the packet */
	groupsock_groupsock__output(basic_udp_sink->gs, basic_udp_sink->output_buffer,
			frame_size, NULL);

	/* Figure out the time at which the next packet should be sent, based
	 * on the duration of the payload that we just read: */
	basic_udp_sink->next_send_time.tv_usec += duration_in_microseconds;
	basic_udp_sink->next_send_time.tv_sec += 
		basic_udp_sink->next_send_time.tv_usec/1000000;
	basic_udp_sink->next_send_time.tv_usec %= 1000000;


	gettimeofday(&time_now, NULL);
	secs_diff = basic_udp_sink->next_send_time.tv_sec - time_now.tv_sec;
	u_seconds_to_go = secs_diff * 1000000 + 
		(basic_udp_sink->next_send_time.tv_usec - time_now.tv_usec);
	if (u_seconds_to_go < 0 || secs_diff < 0) 
		u_seconds_to_go = 0;
	/* sanity check: Make sure that the time-to-delay is non-negative: */

	/* Delay this amount of time: */
	/*TODO: UV_TIMER????? - scheduleDelayedTask*/

}

void livemedia_basic_udp_sink__send_next__static(void * first_arg)
{
	livemedia_basic_udp_sink_t *sink = (livemedia_basic_udp_sink_t *)first_arg;
	livemedia_basic_udp_sink__continue_playing1(sink);
}


