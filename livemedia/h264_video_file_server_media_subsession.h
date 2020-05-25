#ifndef __LIVEMEDIA_H264_VIDEO_FILE_SERVER_MEDIA_SUBSESSION_H__
#define __LIVEMEDIA_H264_VIDEO_FILE_SERVER_MEDIA_SUBSESSION_H__
#include "file_server_media_subsession.h"

/*****************************************************************
 * H264VideoFileServerMediaSubsession:FileServerMediaSubsession (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264_video_file_server_media_subsession_t {
	livemedia_file_server_media_subsession_t file_server_media_subsession;

	char *aux_sdp_line;
	char done_flag;		/* used when setting up "fAuxSDPLine" */
	livemedia_rtp_sink_t *dummy_rtp_sink;	/* ditto */

} livemedia_h264_video_file_server_media_subsession_t;

/*****************************************************************
 * H264VideoFileServerMediaSubsession:FileServerMediaSubsession (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_file_server_media_subsession_t *__livemedia_h264_video_file_server_media_subsession__alloc(void);

void __livemedia_h264_video_file_server_media_subsession__init(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession,
		char const *file_name, bool reuse_first_source);

void __livemedia_h264_video_file_server_media_subsession__deinit(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession);

void __livemedia_h264_video_file_server_media_subsession__free(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession);
/*
 * new() and delete() functions
 */
livemedia_h264_video_file_server_media_subsession_t *livemedia__new__h264_video_file_server_media_subsession(
		char const *file_name, bool reuse_first_source);
      /* called only by createNew(); */

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char *livemedia_h264_video_file_server_media_subsession__get_aux_sdp_line__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source);

livemedia_framed_source_t *livemedia_h264_video_file_server_media_subsession__create_new_stream_source__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		unsigned int client_session_id, unsigned int *est_bitrate);

livemedia_rtp_sink_t *livemedia_h264_video_file_server_media_subsession__create_new_rtp_sink__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtp_groupsock, unsigned char rtp_payload_type_if_dynamic,
		livemedia_framed_source_t *input_source);

void livemedia_h264_video_file_server_media_subsession__delete__impl(
		livemedia_medium_t *medium);

/*
 * Functions in header file
 */
void livemedia_h264_video_file_server_media_subsession__set_done_flag(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession);

/*
 * Normal functions
 */
livemedia_h264_video_file_server_media_subsession_t *livemedia_h264_video_file_server_media_subsession__create_new__static(
		char const *file_name, bool reuse_first_source);

/* Used to implement "getAuxSDPLine()": */
void livemedia_h264_video_file_server_media_subsession__check_for_aux_sdp_line1(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession);

void livemedia_h264_video_file_server_media_subsession__after_playing_dummy1(
		livemedia_h264_video_file_server_media_subsession_t *h264_video_file_server_media_subsession);

#endif
