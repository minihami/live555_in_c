#include "h264_video_file_server_media_subsession.h"
#include "h264_video_rtp_sink.h"
#include "byte_stream_file_source.h"
#include "h264_video_stream_framer.h"

/*****************************************************************
 * H264VideoFileServerMediaSubsession:FileServerMediaSubsession (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_file_server_media_subsession_t *__livemedia_h264_video_file_server_media_subsession__alloc(void)
{
	livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession;
	h264_video_file_server_media_subsession = (livemedia_h264_video_file_server_media_subsession_t *)calloc(1, sizeof(livemedia_h264_video_file_server_media_subsession_t));
	if (h264_video_file_server_media_subsession == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264_video_file_server_media_subsession_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264_video_file_server_media_subsession;
}
void __livemedia_h264_video_file_server_media_subsession__init(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession,
		char const *file_name, bool reuse_first_source)
{
	livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession;

	on_demand_server_media_subsession = 
		(livemedia_on_demand_server_media_subsession_t *)h264_video_file_server_media_subsession;

	/* parent init */
	__livemedia_file_server_media_subsession__init(
			&h264_video_file_server_media_subsession->file_server_media_subsession,
			file_name, reuse_first_source);

	/* constructor */
	h264_video_file_server_media_subsession->aux_sdp_line = NULL;
	h264_video_file_server_media_subsession->done_flag = 0;
	h264_video_file_server_media_subsession->dummy_rtp_sink = NULL;

	
	/* virtual functions configuration */ 
	on_demand_server_media_subsession->get_aux_sdp_line =
		livemedia_h264_video_file_server_media_subsession__get_aux_sdp_line__impl;

	on_demand_server_media_subsession->create_new_stream_source =
		livemedia_h264_video_file_server_media_subsession__create_new_stream_source__impl;

	on_demand_server_media_subsession->create_new_rtp_sink =
		livemedia_h264_video_file_server_media_subsession__create_new_rtp_sink__impl;

	((livemedia_medium_t *)h264_video_file_server_media_subsession)->delete = 
		livemedia_h264_video_file_server_media_subsession__delete__impl;
}
void __livemedia_h264_video_file_server_media_subsession__deinit(livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession)
{
	/* destructor */
	if(h264_video_file_server_media_subsession->aux_sdp_line) {
		free(h264_video_file_server_media_subsession->aux_sdp_line);
		h264_video_file_server_media_subsession->aux_sdp_line = NULL;
	}

	/* parent deinit */
	__livemedia_file_server_media_subsession__deinit(
			&h264_video_file_server_media_subsession->file_server_media_subsession);
}
void __livemedia_h264_video_file_server_media_subsession__free(livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession)
{
	if (h264_video_file_server_media_subsession) {
		free(h264_video_file_server_media_subsession);
		h264_video_file_server_media_subsession = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264_video_file_server_media_subsession_t *livemedia__new__h264_video_file_server_media_subsession(
		char const *file_name, bool reuse_first_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession;

	h264_video_file_server_media_subsession = __livemedia_h264_video_file_server_media_subsession__alloc();
	if (h264_video_file_server_media_subsession)
		__livemedia_h264_video_file_server_media_subsession__init(
				h264_video_file_server_media_subsession, file_name, reuse_first_source);

	return h264_video_file_server_media_subsession;
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
static void livemedia__check_for_aux_sdp_line(void *client_data)
{
	livemedia_h264_video_file_server_media_subsession_t *subsess;
	
	subsess = (livemedia_h264_video_file_server_media_subsession_t *)client_data;
	livemedia_h264_video_file_server_media_subsession__check_for_aux_sdp_line1(subsess);

}
static void livemedia__after_playing_dummy(void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_file_server_media_subsession_t *subsess;
	
	subsess = (livemedia_h264_video_file_server_media_subsession_t *)client_data;
	livemedia_h264_video_file_server_media_subsession__after_playing_dummy1(subsess);
}

char *livemedia_h264_video_file_server_media_subsession__get_aux_sdp_line__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession;

	h264_video_file_server_media_subsession = 
		(livemedia_h264_video_file_server_media_subsession_t *)on_demand_server_media_subsession;

	if (h264_video_file_server_media_subsession->aux_sdp_line != NULL)
		return h264_video_file_server_media_subsession->aux_sdp_line; /* it's already been set up (for a previous client) */
	if (h264_video_file_server_media_subsession->dummy_rtp_sink  == NULL) {
		/* we're not already setting it up for another, concurrent stream
		 * Note: For H264 video files, the 'config' information ("profile-level-id" and "sprop-parameter-sets") isn't known
		 * until we start reading the file.  This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
		 * and we need to start reading data from our file until this changes */
		h264_video_file_server_media_subsession->dummy_rtp_sink = rtp_sink;

		/* Start reading the file */
#if 0
		livemedia_media_sink__start_playing(
				(livemedia_media_sink_t *)h264_video_file_server_media_subsession->dummy_rtp_sink,
				(livemedia_media_source_t *)input_source, livemedia__after_playing_dummy, 
				h264_video_file_server_media_subsession);

		/* Check whether the sink's 'auxSDPLine()' is ready: */
		livemedia__check_for_aux_sdp_line(h264_video_file_server_media_subsession);

#endif
	}

	return h264_video_file_server_media_subsession->aux_sdp_line;

}

livemedia_framed_source_t *livemedia_h264_video_file_server_media_subsession__create_new_stream_source__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		unsigned int client_session_id, unsigned int *est_bitrate)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_byte_stream_file_source_t *file_source;

	*est_bitrate = 500; /* kbps, estimate */

	/* Create the video source: */
	file_source = livemedia_byte_stream_file_source__create_new_by_file_name__static(
			((livemedia_file_server_media_subsession_t *)on_demand_server_media_subsession)->file_name, 0, 0);
	if (file_source == NULL)
		return NULL;
	((livemedia_file_server_media_subsession_t *)on_demand_server_media_subsession)->file_size =
		livemedia_byte_stream_file_source__file_size(file_source);

	/* Create a framer for the Video Elementary Stream: */
	return (livemedia_framed_source_t *)livemedia_h264_video_stream_framer__create_new__static(
			(livemedia_framed_source_t *)file_source, false, false);
}

livemedia_rtp_sink_t *livemedia_h264_video_file_server_media_subsession__create_new_rtp_sink__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtp_groupsock, unsigned char rtp_payload_type_if_dynamic,
		livemedia_framed_source_t *input_source /* not using parameter */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (livemedia_rtp_sink_t *)livemedia_h264_video_rtp_sink__create_new__static(rtp_groupsock, rtp_payload_type_if_dynamic);
}

void livemedia_h264_video_file_server_media_subsession__delete__impl(
		livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession;
	h264_video_file_server_media_subsession = 
		(livemedia_h264_video_file_server_media_subsession_t *)medium;

	/* if child, get parent as parameter and cast to child */
	if (h264_video_file_server_media_subsession) {
		__livemedia_h264_video_file_server_media_subsession__deinit(h264_video_file_server_media_subsession);
		__livemedia_h264_video_file_server_media_subsession__free(h264_video_file_server_media_subsession);
	}
}

/*
 * Functions in header file
 */
void livemedia_h264_video_file_server_media_subsession__set_done_flag(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	h264_video_file_server_media_subsession->done_flag = ~0;
}

/*
 * Normal functions
 */
livemedia_h264_video_file_server_media_subsession_t *livemedia_h264_video_file_server_media_subsession__create_new__static(
		char const *file_name, bool reuse_first_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia__new__h264_video_file_server_media_subsession(
			file_name, reuse_first_source);
}

static void livemedia__check_for_aux_sdp_line1_cb(uv_timer_t *handle)
{
	livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession;

	h264_video_file_server_media_subsession = 
		(livemedia_h264_video_file_server_media_subsession_t *)handle->data;

	livemedia__check_for_aux_sdp_line(h264_video_file_server_media_subsession);
}

void livemedia_h264_video_file_server_media_subsession__check_for_aux_sdp_line1(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	char *dasl;
	int u_secs_to_delay;

	uv_timer_stop(livemedia_medium__next_task(
				(livemedia_medium_t *)h264_video_file_server_media_subsession));

	if (h264_video_file_server_media_subsession->aux_sdp_line != NULL) {
		printf("KSYOON: %s(), if1\n", __func__);
		/* Signal the event loop that we're done: */
		livemedia_h264_video_file_server_media_subsession__set_done_flag(
				h264_video_file_server_media_subsession);
	} else if (h264_video_file_server_media_subsession->dummy_rtp_sink != NULL &&
			(dasl = livemedia_rtp_sink__aux_sdp_line(h264_video_file_server_media_subsession->dummy_rtp_sink)) != NULL) {
		printf("KSYOON: %s(), if2\n", __func__);
		h264_video_file_server_media_subsession->aux_sdp_line = livemedia__str_dup(dasl);
		h264_video_file_server_media_subsession->dummy_rtp_sink = NULL;
		
    /* Signal the event loop that we're done: */
		livemedia_h264_video_file_server_media_subsession__set_done_flag(
				h264_video_file_server_media_subsession);
	} else if (!h264_video_file_server_media_subsession->done_flag) {
		printf("KSYOON: %s(), if3\n", __func__);
		/* try again after a brief delay: */
		u_secs_to_delay = 100000; /* 100 ms */
		livemedia_medium__next_task((livemedia_medium_t *)h264_video_file_server_media_subsession)->data =
			(void *)h264_video_file_server_media_subsession;
		/* TODO: timer start 
		uv_timer_start(livemedia_medium__next_task(
					(livemedia_medium_t *)h264_video_file_server_media_subsession), 
				livemedia__check_for_aux_sdp_line1_cb, u_secs_to_delay/1000, 0);
				*/
	 }
}

void livemedia_h264_video_file_server_media_subsession__after_playing_dummy1(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("TODO - check again(jm): %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	uv_timer_stop(livemedia_medium__next_task(
			(livemedia_medium_t *)h264_video_file_server_media_subsession));
	livemedia_h264_video_file_server_media_subsession__set_done_flag(
			h264_video_file_server_media_subsession);
}


/*
 * Static functions implementation for ancestor
 */
