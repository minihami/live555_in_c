#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "livemedia/rtsp_server.h"

#include "uv.h"
#include "livemedia/h264_video_file_server_media_subsession.h"
static void announce_stream(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *sms,
		char const *stream_name, char const *inpu_file_name);


int main_init(void)
{
	/* TODO */
	livemedia_rtsp_server_t *rtsp_server;
	groupsock_port_t *port;
	livemedia_server_media_session_t *sms;
	char const *stream_name;
	char const *input_file_name;
	char const *description_string;
	livemedia_h264_video_file_server_media_subsession_t *h264_subsession;	
	bool reuse_first_source;


	reuse_first_source = false;
	port = groupsock__new__port(8555);
	rtsp_server = livemedia_rtsp_server__create_new__static(port, NULL, 65);

	stream_name = "h264ESVideoTest";
	input_file_name = "test.264";
	description_string = "Session streamed by \"rtsp_server main\"";

	sms = livemedia_server_media_session__create_new__static(stream_name, stream_name, 
			description_string, false, NULL);

	h264_subsession =
		livemedia_h264_video_file_server_media_subsession__create_new__static(
			input_file_name, reuse_first_source);
	
	livemedia_server_media_session__add_subsession(sms, 
			(livemedia_server_media_subsession_t *)h264_subsession);

	livemedia_generic_media_server__add_server_media_session(
			(livemedia_generic_media_server_t *)rtsp_server, sms);


	announce_stream(rtsp_server, sms, stream_name, input_file_name);


	return 0;
}

int main_deinit(void)
{
	/* TODO */

	uv_loop_close(uv_default_loop());

	return 0;
}

int main(int argc, char *argv[])
{
	main_init();

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	main_deinit();

	return 0;
}

static void announce_stream(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *sms,
		char const *stream_name, char const *input_file_name)
{
	char *url = livemedia_rtsp_server__rtsp_url(rtsp_server, sms, NULL);
	fprintf(stdout, "%s stream, from the file %s\n", stream_name, input_file_name);
	fprintf(stdout, "Play this stream using the URL %s\n", url);
	if (url) {
		free(url);
		url = NULL;
	}
}
