#include "on_demand_server_media_subsession.h"
#include "groupsock_helper.h"

/*****************************************************************
 * OnDemandServerMediaSubsession:ServerMediaSubsession (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_on_demand_server_media_subsession_t *__livemedia_on_demand_server_media_subsession__alloc(void)
{
	livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession;
	on_demand_server_media_subsession = (livemedia_on_demand_server_media_subsession_t *)calloc(1, sizeof(livemedia_on_demand_server_media_subsession_t));
	if (on_demand_server_media_subsession == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_on_demand_server_media_subsession_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return on_demand_server_media_subsession;
}
void __livemedia_on_demand_server_media_subsession__init(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		bool reuse_first_source,
		port_num_bits initial_port_num,
		bool multiplex_rtcp_with_rtp)
{

	livemedia_server_media_subsession_t *server_media_subsession;

	server_media_subsession = 
		(livemedia_server_media_subsession_t *)on_demand_server_media_subsession;

	__livemedia_server_media_subsession__init(
			&on_demand_server_media_subsession->server_media_subsession);

	/* constructor */
	on_demand_server_media_subsession->sdp_lines = NULL;
	on_demand_server_media_subsession->reuse_first_source = reuse_first_source;
	on_demand_server_media_subsession->multiplex_rtcp_with_rtp = multiplex_rtcp_with_rtp;
	on_demand_server_media_subsession->last_stream_token = NULL;
	on_demand_server_media_subsession->app_handler_task = NULL;
	on_demand_server_media_subsession->app_handler_client_data = NULL;

	on_demand_server_media_subsession->destination_hash_table =
		livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	if (on_demand_server_media_subsession->multiplex_rtcp_with_rtp) {
		on_demand_server_media_subsession->initial_port_num = initial_port_num;
	} else {
		/* Make sure RTP ports are even-numbered: */
		on_demand_server_media_subsession->initial_port_num = (initial_port_num+1)&~1;
	}
	gethostname(on_demand_server_media_subsession->cname, 
			sizeof(on_demand_server_media_subsession->cname));
	on_demand_server_media_subsession->cname[sizeof(on_demand_server_media_subsession->cname)-1] = '\0';
	/* just in case */

	/* virtual functions configuration */ 
	server_media_subsession->sdp_lines =
		livemedia_on_demand_server_media_subsession__sdp_lines__impl;

	server_media_subsession->get_stream_parameters =
		livemedia_on_demand_server_media_subsession__get_stream_parameters__impl;

	server_media_subsession->start_stream =
		livemedia_on_demand_server_media_subsession__start_stream__impl;

	server_media_subsession->pause_stream =
		livemedia_on_demand_server_media_subsession__pause_stream__impl;

	server_media_subsession->seek_stream_relative =
		livemedia_on_demand_server_media_subsession__seek_stream_relative__impl;

	server_media_subsession->seek_stream_absolute =
		livemedia_on_demand_server_media_subsession__seek_stream_absolute__impl;

	server_media_subsession->null_seek_stream =
		livemedia_on_demand_server_media_subsession__null_seek_stream__impl;

	server_media_subsession->set_stream_scale =
		livemedia_on_demand_server_media_subsession__set_stream_scale__impl;

	server_media_subsession->get_current_npt =
		livemedia_on_demand_server_media_subsession__get_current_npt__impl;

	server_media_subsession->get_stream_source =
		livemedia_on_demand_server_media_subsession__get_stream_source;

	server_media_subsession->get_rtp_sink_and_rtcp =
		livemedia_on_demand_server_media_subsession__get_rtp_sink_and_rtcp__impl;

	server_media_subsession->delete_stream =
		livemedia_on_demand_server_media_subsession__delete_stream__impl;

	on_demand_server_media_subsession->get_aux_sdp_line = 
		livemedia_on_demand_server_media_subsession__get_aux_sdp_line__impl;

	on_demand_server_media_subsession->seek_stream_source_by_relative = 
		livemedia_on_demand_server_media_subsession__seek_stream_source_by_relative__impl;

	on_demand_server_media_subsession->seek_stream_source_by_absolute = 
		livemedia_on_demand_server_media_subsession__seek_stream_source_by_absolute__impl;

	on_demand_server_media_subsession->set_stream_source_scale = 
		livemedia_on_demand_server_media_subsession__set_stream_source_scale__impl;

	on_demand_server_media_subsession->set_stream_source_duration = 
		livemedia_on_demand_server_media_subsession__set_stream_source_duration__impl;

	on_demand_server_media_subsession->close_stream_source = 
		livemedia_on_demand_server_media_subsession__close_stream_source__impl;

	on_demand_server_media_subsession->create_groupsock = 
		livemedia_on_demand_server_media_subsession__create_groupsock__impl;

	on_demand_server_media_subsession->create_rtcp = 
		livemedia_on_demand_server_media_subsession__create_rtcp__impl;

	((livemedia_medium_t *)on_demand_server_media_subsession)->delete = livemedia_on_demand_server_media_subsession__delete__impl;

}
void __livemedia_on_demand_server_media_subsession__deinit(livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession)
{
	livemedia_destinations_t *destinations;
	/* destructor */
	if (on_demand_server_media_subsession->sdp_lines) {
		free (on_demand_server_media_subsession->sdp_lines);
		on_demand_server_media_subsession->sdp_lines = NULL;
	}
	/* Clean out the destinations hash table: */
	while (1) {
		destinations = (livemedia_destinations_t *)(livemedia_hash_table__remove_next(
					on_demand_server_media_subsession->destination_hash_table));
		if (destinations == NULL)
			break;
		livemedia__delete__destinations(destinations);

	}
	livemedia__delete__hash_table(on_demand_server_media_subsession->destination_hash_table);

	/* parent deinit */
	__livemedia_server_media_subsession__deinit(
			&on_demand_server_media_subsession->server_media_subsession);
}
void __livemedia_on_demand_server_media_subsession__free(livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession)
{
	if (on_demand_server_media_subsession) {
		free(on_demand_server_media_subsession);
		on_demand_server_media_subsession = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_on_demand_server_media_subsession_t *livemedia__new__on_demand_server_media_subsession(
		bool reuse_first_source,
		port_num_bits initial_port_num,
		bool multiplex_rtcp_with_rtp)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession;

	on_demand_server_media_subsession = __livemedia_on_demand_server_media_subsession__alloc();
	if (on_demand_server_media_subsession)
		__livemedia_on_demand_server_media_subsession__init(
				on_demand_server_media_subsession, reuse_first_source, initial_port_num,
				multiplex_rtcp_with_rtp);

	return on_demand_server_media_subsession;
}
/*
 * Functions for virtual functions
 */
char *livemedia_on_demand_server_media_subsession__get_aux_sdp_line(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->get_aux_sdp_line(
				on_demand_server_media_subsession, rtp_sink, input_source);
	}
	return NULL;
}

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_relative(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double *seek_npt, 
		double stream_duration, u_int64_t *num_bytes)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->seek_stream_source_by_relative(
				on_demand_server_media_subsession, input_source, seek_npt, 
				stream_duration, num_bytes);
	}
	return;
}

void livemedia_on_demand_server_media_subsession__set_stream_source_scale(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, float scale)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->set_stream_source_scale(
				on_demand_server_media_subsession, input_source, scale); 
	}
	return;
}

void livemedia_on_demand_server_media_subsession__set_stream_source_duration(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double stream_duration, u_int64_t *num_bytes)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->set_stream_source_duration(
				on_demand_server_media_subsession, input_source, stream_duration,
				num_bytes); 
	}
	return;
}

void livemedia_on_demand_server_media_subsession__close_stream_source(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->close_stream_source(
				on_demand_server_media_subsession, input_source);
	}
	return;
}

livemedia_framed_source_t *livemedia_on_demand_server_media_subsession__create_new_stream_source(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		unsigned int client_session_id, unsigned int *est_bitrate)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->create_new_stream_source(
				on_demand_server_media_subsession, client_session_id, est_bitrate);
	}
	return NULL;
}

livemedia_rtp_sink_t *livemedia_on_demand_server_media_subsession__create_new_rtp_sink(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtp_groupsock, unsigned char rtp_payload_type_if_dynamic,
		livemedia_framed_source_t *input_source)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->create_new_rtp_sink(
				on_demand_server_media_subsession, rtp_groupsock, 
				rtp_payload_type_if_dynamic, input_source);
	}
	return NULL;
}

groupsock_groupsock_t *livemedia_on_demand_server_media_subsession__create_groupsock(
		struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		struct in_addr *addr, groupsock_port_t port)
{
	/* Default implementation; may be redefined by subclasses: */
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->create_groupsock(
				on_demand_server_media_subsession, addr, port);
	}
	return NULL;
}

livemedia_rtcp_instance_t *livemedia_on_demand_server_media_subsession__create_rtcp(
		struct _livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw, /* in kbps */
		unsigned char const *cname, livemedia_rtp_sink_t *sink)
{
	if (on_demand_server_media_subsession) {
		return on_demand_server_media_subsession->create_rtcp(
				on_demand_server_media_subsession, rtcp_gs, tot_session_bw, cname, sink);
	}
	return NULL;
}
/*
 * Override functions
 */
char const *livemedia_on_demand_server_media_subsession__sdp_lines__impl(
		livemedia_server_media_subsession_t *server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession;
	unsigned int est_bitrate;
	livemedia_framed_source_t *input_source;
	struct in_addr dummy_addr;
	groupsock_groupsock_t *dummy_groupsock;
	unsigned char rtp_payload_type;
	livemedia_rtp_sink_t *dummy_rtp_sink;
	groupsock_port_t *port;

	on_demand_server_media_subsession = 
		(livemedia_on_demand_server_media_subsession_t *)server_media_subsession;

	if (on_demand_server_media_subsession->sdp_lines == NULL) {
		/* We need to construct a set of SDP lines that describe this
		 * subsession (as a unicast stream).  To do so, we first create
		 * dummy (unused) source and "RTPSink" objects,
		 * whose parameters we use for the SDP lines: */
		input_source = livemedia_on_demand_server_media_subsession__create_new_stream_source(
				on_demand_server_media_subsession, 0, &est_bitrate);
		if (input_source == NULL)
			return NULL; /* file not found */

		dummy_addr.s_addr = 0;
		port = groupsock__new__port(0);
		dummy_groupsock = livemedia_on_demand_server_media_subsession__create_groupsock(
				on_demand_server_media_subsession, &dummy_addr, *port);
		groupsock__delete__port(port);

		rtp_payload_type = 96 + livemedia_server_media_subsession__track_number(server_media_subsession) - 1; /* if dynamic */
		dummy_rtp_sink = livemedia_on_demand_server_media_subsession__create_new_rtp_sink(
				on_demand_server_media_subsession, dummy_groupsock, rtp_payload_type, input_source);
		if (dummy_rtp_sink != NULL && *livemedia_rtp_sink__estimated_bitrate(dummy_rtp_sink) > 0)
			est_bitrate = *livemedia_rtp_sink__estimated_bitrate(dummy_rtp_sink);

		livemedia_on_demand_server_media_subsession__set_sdp_lines_from_rtp_sink(on_demand_server_media_subsession,
				dummy_rtp_sink, input_source, est_bitrate);
		printf("\n\nbefore close medium\n\n");
		livemedia_medium__close_by_medium_ptr__static((livemedia_medium_t *)dummy_rtp_sink);
		printf("\n\nafter close medium\n\n");
		groupsock__delete__net_interface((groupsock_net_interface_t *)dummy_groupsock);
		livemedia_on_demand_server_media_subsession__close_stream_source(
				on_demand_server_media_subsession, input_source);
	}
	printf("in %s(), sdp_lines:%s\n",__func__, on_demand_server_media_subsession->sdp_lines);
	return on_demand_server_media_subsession->sdp_lines;
}

void livemedia_on_demand_server_media_subsession__get_stream_parameters__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, /* in */
		net_address_bits client_address, /* in */
		groupsock_port_t *client_rtp_port, /* in */
		groupsock_port_t *client_rtcp_port, /* in */
		uv_tcp_t *tcp_socket_num,  /* in (-1 means use UDP, not TCP) */
		unsigned char rtp_channel_id, /* in (used if TCP)*/
		unsigned char rtcp_channel_id, /* in (used if TCP)*/
		net_address_bits *destination_address, /* in out */
		u_int8_t *destination_ttl, /* in out */
		bool *is_multicast, /* out */
		groupsock_port_t *server_rtp_port, /* out */
		groupsock_port_t *server_rtcp_port, /* out */
		void **stream_token /* out */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	struct in_addr destination_addr;
	unsigned int *reference_count;
	unsigned int stream_bitrate;
	livemedia_framed_source_t *media_source;
	livemedia_rtp_sink_t *rtp_sink;
	livemedia_basic_udp_sink_t *udp_sink;
	groupsock_groupsock_t *rtp_groupsock;
	groupsock_groupsock_t *rtcp_groupsock;
	port_num_bits server_port_num;
	groupsock_no_reuse_t *dummy;
	struct in_addr dummy_addr;
	unsigned char rtp_payload_type;
	unsigned int rtp_buf_size;
	livemedia_destinations_t *destinations;
	groupsock_port_t *tmp_port;
	

	livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession;
	on_demand_server_media_subsession =
		(livemedia_on_demand_server_media_subsession_t *)server_media_subsession;

	if(*destination_address == 0)
		*destination_address = client_address;
	destination_addr.s_addr = *destination_address;
	*is_multicast = false;

	if (on_demand_server_media_subsession->last_stream_token != NULL &&
			on_demand_server_media_subsession->reuse_first_source) {
		/* Special case: Rather than creating a new 'StreamState',
		 * we reuse the one that we've already created: */
		server_rtp_port = livemedia_stream_state__server_rtp_port(
					((livemedia_stream_state_t *)on_demand_server_media_subsession->last_stream_token));
		server_rtcp_port = livemedia_stream_state__server_rtcp_port(
					((livemedia_stream_state_t *)on_demand_server_media_subsession->last_stream_token));
		reference_count = livemedia_stream_state__reference_count(
					((livemedia_stream_state_t *)on_demand_server_media_subsession->last_stream_token));
		++(*reference_count);
		*stream_token = on_demand_server_media_subsession->last_stream_token;
	} else {
		/* Normal case: Create a new media source: */
		media_source = livemedia_on_demand_server_media_subsession__create_new_stream_source(
				on_demand_server_media_subsession, client_session_id, &stream_bitrate);

		/* Create 'groupsock' and 'sink' objects for the destination,
		 * using previously unused server port numbers: */
		rtp_sink = NULL;
		udp_sink = NULL;
		rtp_groupsock = NULL;
		rtcp_groupsock = NULL;

		if (groupsock_port__num(client_rtp_port) != 0 || tcp_socket_num != NULL) {
			/* Normal case: Create destinations */
			if(groupsock_port__num(client_rtcp_port) == 0) {
				/* We're streaming raw UDP (not RTP). Create a single groupsock: */
				dummy = groupsock__new__no_reuse();
				/* ensures that we skip over ports that are already in use */
				for (server_port_num =
						on_demand_server_media_subsession->initial_port_num; ;
						++server_port_num) {
					dummy_addr.s_addr = 0;

					tmp_port = groupsock__new__port(server_port_num);
					*server_rtp_port = *tmp_port;
					groupsock__delete__port(tmp_port);
					rtp_groupsock = livemedia_on_demand_server_media_subsession__create_groupsock(
							on_demand_server_media_subsession, &dummy_addr, *server_rtp_port);
					if (groupsock_socket__socket_num((groupsock_socket_t *)rtp_groupsock) >= 0)
						break; /* success */
				}
				udp_sink = livemedia_basic_udp_sink__create_new__static(rtp_groupsock, 1450);
			} else {
				/* Normal case: We're streaming RTP (over UDP or TCP).  Create a pair of
				 * groupsocks (RTP and RTCP), with adjacent port numbers (RTP port number even).
				 * (If we're multiplexing RTCP and RTP over the same port number, it can be odd or even.) */
				dummy = groupsock__new__no_reuse(); 
				/* ensures that we skip over ports that are already in use */
				for (server_port_num =
						on_demand_server_media_subsession->initial_port_num; ;
						++server_port_num) {
					dummy_addr.s_addr = 0;
					tmp_port = groupsock__new__port(server_port_num);
					*server_rtp_port = *tmp_port;
					groupsock__delete__port(tmp_port);
					rtp_groupsock = livemedia_on_demand_server_media_subsession__create_groupsock(
							on_demand_server_media_subsession, &dummy_addr, *server_rtp_port);
					if (groupsock_socket__socket_num((groupsock_socket_t *)rtp_groupsock) == NULL) {
						groupsock__delete__net_interface((groupsock_net_interface_t *)rtp_groupsock);
						continue; /* try again */
					}
					
					if (on_demand_server_media_subsession->multiplex_rtcp_with_rtp) {
						/* Use the RTP 'groupsock' object for RTCP as well: */
						*server_rtcp_port = *server_rtp_port;
						rtcp_groupsock = rtp_groupsock;
					} else {
						/* Create a separate 'groupsock' object (with the next (odd) port number) for RTCP: */
						++server_port_num;
						tmp_port = groupsock__new__port(server_port_num);
						*server_rtcp_port = *tmp_port;
						groupsock__delete__port(tmp_port);
						rtcp_groupsock = livemedia_on_demand_server_media_subsession__create_groupsock(
								on_demand_server_media_subsession, &dummy_addr, *server_rtcp_port);
						if (groupsock_socket__socket_num((groupsock_socket_t *)rtcp_groupsock) == NULL) {
							groupsock__delete__net_interface((groupsock_net_interface_t *)rtp_groupsock);
							groupsock__delete__net_interface((groupsock_net_interface_t *)rtcp_groupsock);
							continue; /* try again */
						}
					}


					break; /* success */
				}

				rtp_payload_type = 96 + livemedia_server_media_subsession__track_number(server_media_subsession) - 1;
				/* if dynamic */
				rtp_sink = livemedia_on_demand_server_media_subsession__create_new_rtp_sink(
						on_demand_server_media_subsession, rtp_groupsock, rtp_payload_type, media_source);
				if (rtp_sink != NULL && (*livemedia_rtp_sink__estimated_bitrate(rtp_sink)) > 0)
					stream_bitrate = (*livemedia_rtp_sink__estimated_bitrate(rtp_sink));
			}

			/* Turn off the destinations for each groupsock.  They'll get set later
			 * (unless TCP is used instead): */
			if (rtp_groupsock != NULL)
				groupsock_groupsock__remove_all_destinations(rtp_groupsock);
			if (rtcp_groupsock != NULL)
				groupsock_groupsock__remove_all_destinations(rtcp_groupsock);

			if (rtp_groupsock != NULL) {
				/* Try to use a big send buffer for RTP -  at least 0.1 second of
				 * specified bandwidth and at least 50 KB */
				rtp_buf_size = stream_bitrate * 25 /2;	/* 1 kbps * 0.1 s = 12.5 bytes */
				if (rtp_buf_size < 50 * 1024)
					rtp_buf_size = 50 * 1024;
				groupsock__increase_send_buffer_to(groupsock_socket__socket_num(
							(groupsock_socket_t *)rtp_groupsock), rtp_buf_size);
			}
		}

		/* Set up the state of the stream.  The stream will get started later: */
		*stream_token = on_demand_server_media_subsession->last_stream_token 
			= livemedia__new__stream_state(on_demand_server_media_subsession, server_rtp_port, server_rtcp_port, rtp_sink, udp_sink,
					stream_bitrate, media_source, rtp_groupsock, rtcp_groupsock);
	}

	/* Record these destinations as being for this client session id: */
	if (tcp_socket_num == NULL) { /* UDP */
		destinations = livemedia__new__destinations_by_dest_addr(&destination_addr, 
				client_rtp_port, client_rtcp_port);
	} else { /* TCP */
		destinations = livemedia__new__destinations_by_tcp_sock_num(&destination_addr, 
				client_rtp_port, client_rtcp_port);
	}
	livemedia_hash_table__add(on_demand_server_media_subsession->destination_hash_table, (char const *)client_session_id, destinations);
}

void livemedia_on_demand_server_media_subsession__start_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token,
		usage_environment__task_func *rtcp_rr_handler, void *rtcp_rr_handler_client_data,
		unsigned short *rtp_seq_num,
		unsigned int *rtp_timestamp,
		livemedia__server_request_alternative_byte_handler *server_request_alternative_byte_handler,
		void *server_request_alternative_byte_handler_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_state_t *stream_state;
	livemedia_destinations_t *destinations;
	livemedia_rtp_sink_t *rtp_sink;

	stream_state = (livemedia_stream_state_t *)stream_token;
	destinations = (livemedia_destinations_t *)(livemedia_hash_table__lookup(
				((livemedia_on_demand_server_media_subsession_t *)server_media_subsession)->
				destination_hash_table, (char const *)client_session_id));

	if (stream_state != NULL) {
		livemedia_stream_state__start_playing(stream_state, destinations,
				client_session_id, rtcp_rr_handler, rtcp_rr_handler_client_data,
				server_request_alternative_byte_handler,
				server_request_alternative_byte_handler_client_data);
		rtp_sink = livemedia_stream_state__rtp_sink(stream_state); /* alias */
		if (rtp_sink != NULL) {
			rtp_seq_num = livemedia_rtp_sink__current_seq_no(rtp_sink);
			rtp_timestamp = livemedia_rtp_sink__preset_next_time_stamp(rtp_sink);
		}
	}
}

void livemedia_on_demand_server_media_subsession__pause_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__seek_stream_relative__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, double *seek_npt,
		double stream_duration, u_int64_t *num_bytes)
{

	*num_bytes = 0; /* by default: unknown */
	livemedia_stream_state_t *stream_state;
	float *start_npt;
	livemedia_rtp_sink_t *rtp_sink;

	/* Seeking isn't allowed if multiple clients are receiving data from the same source: */
	if (((livemedia_on_demand_server_media_subsession_t *)server_media_subsession)->reuse_first_source)
		return;
	stream_state = (livemedia_stream_state_t *)stream_token;
	if (stream_state != NULL &&livemedia_stream_state__media_source(stream_state) != NULL) {
		livemedia_on_demand_server_media_subsession__seek_stream_source_by_relative(
				server_media_subsession, livemedia_stream_state__media_source(stream_state),
				seek_npt, stream_duration, num_bytes);

		start_npt = livemedia_stream_state__start_npt(stream_state);
		*start_npt = (float)*seek_npt;
		rtp_sink = livemedia_stream_state__rtp_sink(stream_state); /* alias */
		if (rtp_sink != NULL)
			livemedia_rtp_sink__reset_presentation_times(rtp_sink);
	}

}

void livemedia_on_demand_server_media_subsession__seek_stream_absolute__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void* stream_token, char **abs_start,
		char **abs_end)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__null_seek_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void *stream_token, double stream_end_time,
		u_int64_t *num_bytes)
{
	livemedia_stream_state_t *stream_state;
	double duration;
	livemedia_rtp_sink_t *rtp_sink;
	float *start_npt;

	num_bytes = 0; /* by default: unknown */

	stream_state = (livemedia_stream_state_t *)stream_token;
	if (stream_state != NULL && livemedia_stream_state__media_source(stream_state) != NULL) {
		/* Because we're not seeking here, get the current NPT, and remember it as the new 'start' NPT: */
		start_npt = livemedia_stream_state__start_npt(stream_state);
		*start_npt = livemedia_server_media_subsession__get_current_npt(server_media_subsession, stream_token);

		duration = stream_end_time - (*livemedia_stream_state__start_npt(stream_state));
		if (duration < 0.0)
			duration = 0.0;
		livemedia_on_demand_server_media_subsession__set_stream_source_duration(
				(livemedia_on_demand_server_media_subsession_t *)server_media_subsession,
				livemedia_stream_state__media_source(stream_state), duration, num_bytes);

		rtp_sink = livemedia_stream_state__rtp_sink(stream_state); /* alias */
		if (rtp_sink != NULL)
			livemedia_rtp_sink__reset_presentation_times(rtp_sink);
	}
}

void livemedia_on_demand_server_media_subsession__set_stream_scale__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned client_session_id, void *stream_token, float scale)
{
	livemedia_stream_state_t *stream_state;

	/* Changing the scale factor isn't allowed if multiple clients are receiving data
	 * from the same source: */
	stream_state = (livemedia_stream_state_t *)stream_token;
	if (stream_state != NULL && livemedia_stream_state__media_source(stream_state) != NULL) {
		livemedia_on_demand_server_media_subsession__set_stream_source_scale(
				(livemedia_on_demand_server_media_subsession_t *)server_media_subsession,
				livemedia_stream_state__media_source(stream_state), scale);
	}
}

float livemedia_on_demand_server_media_subsession__get_current_npt__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token)
{
	livemedia_stream_state_t *stream_state;
	livemedia_rtp_sink_t *rtp_sink;

	do {
		if (stream_token == NULL)
			break;
		stream_state = (livemedia_stream_state_t *)stream_token;
		rtp_sink = livemedia_stream_state__rtp_sink(stream_state);
		if (rtp_sink == NULL)
			break;

		return (*livemedia_stream_state__start_npt(stream_state))
			+ (livemedia_rtp_sink__most_recent_presentation_time(rtp_sink)->tv_sec
					- livemedia_rtp_sink__initial_presentation_time(rtp_sink)->tv_sec)
			+ (livemedia_rtp_sink__most_recent_presentation_time(rtp_sink)->tv_usec
					- livemedia_rtp_sink__initial_presentation_time(rtp_sink)->tv_usec)/1000000.0f;
	} while (0);

	return 0.0;
}

livemedia_framed_source_t *livemedia_on_demand_server_media_subsession__get_stream_source(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return NULL;
}

void livemedia_on_demand_server_media_subsession__get_rtp_sink_and_rtcp__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		void *stream_token, livemedia_rtp_sink_t const **rtp_sink,
		livemedia_rtcp_instance_t const **rtcp)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__delete_stream__impl(
		livemedia_server_media_subsession_t *server_media_subsession,
		unsigned int client_session_id, void **stream_token)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

char *livemedia_on_demand_server_media_subsession__get_aux_sdp_line__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return NULL;
}

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_relative__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double *seek_npt, 
		double stream_duration, u_int64_t *num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	*num_bytes = 0;
}

void livemedia_on_demand_server_media_subsession__seek_stream_source_by_absolute__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, char **abs_start, char **abs_end)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__set_stream_source_scale__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, float scale)
{
	/* Default implementation: DO nothing */
}

void livemedia_on_demand_server_media_subsession__set_stream_source_duration__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source, double stream_duration, u_int64_t *num_bytes)
{
#ifdef WEBGATE_DEBUG
	/*TODO*/
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__close_stream_source__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_medium__close_by_medium_ptr__static((livemedia_medium_t *)input_source);
}


groupsock_groupsock_t *livemedia_on_demand_server_media_subsession__create_groupsock__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		struct in_addr *addr, groupsock_port_t port)
{
	/* Default implementation; may be redefined by subclasses: */
	return groupsock__new__groupsock_by_port_and_ttl(addr, port, 255);
}

livemedia_rtcp_instance_t *livemedia_on_demand_server_media_subsession__create_rtcp__impl(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw, /* in kbps */
		unsigned char const *cname, livemedia_rtp_sink_t *sink)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Default implementation; may be redefined by subclasses: */
	return livemedia_rtcp_instance__create_new__static(rtcp_gs, tot_session_bw, cname, 
			sink, NULL /* we're a server */, false, NULL);
}

void livemedia_on_demand_server_media_subsession__delete__impl(livemedia_medium_t *medium)
{
	livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession;
	on_demand_server_media_subsession = (livemedia_on_demand_server_media_subsession_t *)medium;

	/* if child, get parent as parameter and cast to child */
	if (on_demand_server_media_subsession) {
		__livemedia_on_demand_server_media_subsession__deinit(on_demand_server_media_subsession);
		__livemedia_on_demand_server_media_subsession__free(on_demand_server_media_subsession);
	}
}

/*
 * Functions in header file
 */
void livemedia_on_demand_server_media_subsession__multiplex_rtcp_with_rtp(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession)
{
#ifdef WEBGATE_DEBUG
	/*TODO: */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*
 * Normal functions
 */

void livemedia_on_demand_server_media_subsession__set_rtcp_app_packet_handler(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia__rtcp_app_handler_func *handler, void *client_data)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__send_rtcp_app_packet(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		u_int8_t subtype, char const *name,
		uint8_t *app_dependent_data, unsigned int app_dependent_data_size)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_on_demand_server_media_subsession__set_sdp_lines_from_rtp_sink(
		livemedia_on_demand_server_media_subsession_t *on_demand_server_media_subsession,
		livemedia_rtp_sink_t *rtp_sink, livemedia_framed_source_t *input_source,
		unsigned int est_bitrate)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	char *media_type;
	unsigned char rtp_payload_type;
	groupsock_address_string_t *ip_address_str;
	char *rtpmap_line;
	char *rtcpmux_line;
	char *range_line;
	char *aux_sdp_line;
	char *sdp_fmt;
	unsigned int sdp_fmt_size;
	char *sdp_lines;

	if (rtp_sink == NULL)
		return;

	media_type = livemedia_rtp_sink__sdp_media_type(rtp_sink);
	rtp_payload_type = livemedia_rtp_sink__rtp_payload_type(rtp_sink);
	ip_address_str = groupsock__new__address_string_by_net_address_bits(
			((livemedia_server_media_subsession_t *)on_demand_server_media_subsession)->
			server_address_for_sdp);
	rtpmap_line = livemedia_rtp_sink__rtpmap_line(rtp_sink);
	rtcpmux_line = on_demand_server_media_subsession->multiplex_rtcp_with_rtp ? 
		"a=rtcp-mux\r\n" : "";
	range_line = livemedia_server_media_subsession__range_sdp_line(
			(livemedia_server_media_subsession_t *)on_demand_server_media_subsession);
	aux_sdp_line = livemedia_on_demand_server_media_subsession__get_aux_sdp_line(
			on_demand_server_media_subsession, rtp_sink, input_source);

	if (aux_sdp_line == NULL)
		aux_sdp_line = "";

	sdp_fmt =
		"m=%s %u RTP/AVP %d\r\n"
		"c=IN IP4 %s\r\n"
		"b=AS:%u\r\n"
		"%s"
		"%s"
		"%s"
		"%s"
		"a=control:%s\r\n";


	sdp_fmt_size = strlen(sdp_fmt)
    + strlen(media_type) + 5 /* max short len */ + 3 /* max char len */
    + strlen(groupsock_address_string__val(ip_address_str))
    + 20 /* max int len */
    + strlen(rtpmap_line)
    + strlen(rtcpmux_line)
    + strlen(range_line)
    + strlen(aux_sdp_line)
    + strlen(livemedia_server_media_subsession__track_id(
			(livemedia_server_media_subsession_t *)on_demand_server_media_subsession));

	sdp_lines = (char *)malloc(sizeof(char) * sdp_fmt_size);
	if (sdp_lines == NULL) {
		fprintf(stderr, "(char *)malloc(sizeof(char) * sdp_fmt_size)"
				"failed in %s()\n", __func__);
	}
	sprintf(sdp_lines, sdp_fmt,
			media_type, /* m= <media> */
			((livemedia_server_media_subsession_t *)on_demand_server_media_subsession)->port_num_for_sdp, /* m= <port> */
			rtp_payload_type, /* m= <fmt list> */
			groupsock_address_string__val(ip_address_str), /* c= address */
			est_bitrate, /* b=AS:<bandwidth> */
			rtpmap_line, /* a=rtpmap:... (if present) */
			rtcpmux_line, /* a=rtcp-mux:... (if present) */
			range_line, /* a=range:... (if present) */
			aux_sdp_line, /* optional extra SDP line */
			livemedia_server_media_subsession__track_id(
			(livemedia_server_media_subsession_t *)on_demand_server_media_subsession)); /* a=control:<track-id> */

	if (range_line) {
		free((char *)range_line);
		range_line = NULL;
	}
	if (rtpmap_line) {
		free(rtpmap_line);
		rtpmap_line = NULL;
	}
	if (on_demand_server_media_subsession->sdp_lines) {
		free(on_demand_server_media_subsession->sdp_lines);
		on_demand_server_media_subsession->sdp_lines = NULL;
	}
	on_demand_server_media_subsession->sdp_lines = livemedia__str_dup(sdp_lines);
	if (sdp_lines) {
		free(sdp_lines);
		sdp_lines = NULL;
	}
	

}
/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * Destinations (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_destinations_t *__livemedia_destinations__alloc(void)
{
	livemedia_destinations_t *destinations;
	destinations = (livemedia_destinations_t *)calloc(1, sizeof(livemedia_destinations_t));
	if (destinations == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_destinations_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return destinations;
}
void __livemedia_destinations__init_by_dest_addr(livemedia_destinations_t *destinations,
		struct in_addr const *dest_addr,
		groupsock_port_t const *rtp_dest_port,
		groupsock_port_t const *rtcp_dest_port)
{
#ifdef WEBGATE_DEBUG
	/* TODO: constructor */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: virtual functions configuration */ 

	destinations->delete = livemedia_destinations__delete__impl;
}
void __livemedia_destinations__init_by_tcp_sock_num(livemedia_destinations_t *destinations,
		uv_tcp_t *tcp_sock_num,
		unsigned char rtp_chan_id,
		unsigned char rtcp_chan_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_port_t *tmp_port;

	destinations->is_tcp = true;
	tmp_port = groupsock__new__port(0);
	destinations->rtp_port = *tmp_port;
	destinations->rtcp_port = *tmp_port;
	groupsock__delete__port(tmp_port);
	destinations->tcp_socket_num = tcp_sock_num;
	destinations->rtp_channel_id = rtp_chan_id;
	destinations->rtcp_channel_id = rtcp_chan_id;

	destinations->delete = livemedia_destinations__delete__impl;
}
void __livemedia_destinations__deinit(livemedia_destinations_t *destinations)
{
#ifdef WEBGATE_DEBUG
	printf("Log: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void __livemedia_destinations__free(livemedia_destinations_t *destinations)
{
	if (destinations) {
		free(destinations);
		destinations = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_destinations_t *livemedia__new__destinations_by_dest_addr(
		struct in_addr const *dest_addr,
		groupsock_port_t const *rtp_dest_port,
		groupsock_port_t const *rtcp_dest_port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_destinations_t *destinations;

	destinations = __livemedia_destinations__alloc();
	if (destinations)
		__livemedia_destinations__init_by_dest_addr(destinations, dest_addr,
				rtp_dest_port, rtcp_dest_port);

	return destinations;
}
livemedia_destinations_t *livemedia__new__destinations_by_tcp_sock_num(
		uv_tcp_t *tcp_sock_num,
		unsigned char rtp_chan_id,
		unsigned char rtcp_chan_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_destinations_t *destinations;

	destinations = __livemedia_destinations__alloc();
	if (destinations)
		__livemedia_destinations__init_by_tcp_sock_num(destinations, tcp_sock_num,
				rtp_chan_id, rtcp_chan_id);

	return destinations;
}
void livemedia__delete__destinations(livemedia_destinations_t *destinations)
{
	if (destinations && destinations->delete)
		destinations->delete(destinations);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_destinations__delete__impl(livemedia_destinations_t *destinations)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (destinations) {
		__livemedia_destinations__deinit(destinations);
		__livemedia_destinations__free(destinations);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * StreamState (Function Definitions)
 ****************************************************************/
static void livemedia__after_playing_stream_state(void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_state_t *stream_state;

	stream_state = (livemedia_stream_state_t *)client_data;
	if (livemedia_stream_state__stream_duration(stream_state) == 0.0) {
		/* When the input stream ends, tear it down.  This will cause a RTCP "BYE"
		 * to be sent to each client, teling it that the stream has ended.
		 * (Because the stream didn't have a known duration, there was no other
		 *  way for clients to know when the stream ended.) */
		livemedia_stream_state__reclaim(stream_state);
	}
	/* Otherwise, keep the stream alive, in case a client wants to
	 * subsequently re-play the stream starting from somewhere other than the end.
	 * (This can be done only on streams that have a known duration.) */
}

/*
 * Internal Functions for new() and delete() functions
 */

livemedia_stream_state_t *__livemedia_stream_state__alloc(void)
{
	livemedia_stream_state_t *stream_state;
	stream_state = (livemedia_stream_state_t *)calloc(1, sizeof(livemedia_stream_state_t));
	if (stream_state == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_stream_state_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return stream_state;
}
void __livemedia_stream_state__init(livemedia_stream_state_t *stream_state,
		livemedia_on_demand_server_media_subsession_t *master,
		groupsock_port_t const *server_rtp_port, groupsock_port_t const *server_rtcp_port,
		livemedia_rtp_sink_t *rtp_sink, livemedia_basic_udp_sink_t *udp_sink,
		unsigned int total_bw, livemedia_framed_source_t *media_source,
		groupsock_groupsock_t *rtp_gs, groupsock_groupsock_t *rtcp_gs)
{
	stream_state->master = master;
	stream_state->are_currently_playing = false;
	stream_state->reference_count = 1;
	stream_state->server_rtp_port = *server_rtp_port;
	stream_state->server_rtcp_port = *server_rtcp_port;
	stream_state->rtp_sink = rtp_sink;
	stream_state->udp_sink = udp_sink;
	stream_state->stream_duration = livemedia_server_media_subsession__duration(master);
	stream_state->total_bw = total_bw;
	stream_state->rtcp_instance = NULL; /* created later */
	stream_state->media_source = media_source;
	stream_state->start_npt = 0.0;
	stream_state->rtp_gs = rtp_gs;
	stream_state->rtcp_gs = rtcp_gs;

	stream_state->delete = livemedia_stream_state__delete__impl;
}
void __livemedia_stream_state__deinit(livemedia_stream_state_t *stream_state)
{
	livemedia_stream_state__reclaim(stream_state);
}
void __livemedia_stream_state__free(livemedia_stream_state_t *stream_state)
{
	if (stream_state) {
		free(stream_state);
		stream_state = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_stream_state_t *livemedia__new__stream_state(
		livemedia_on_demand_server_media_subsession_t *master,
		groupsock_port_t const *server_rtp_port, groupsock_port_t const *server_rtcp_port,
		livemedia_rtp_sink_t *rtp_sink, livemedia_basic_udp_sink_t *udp_sink,
		unsigned int total_bw, livemedia_framed_source_t *media_source,
		groupsock_groupsock_t *rtp_gs, groupsock_groupsock_t *rtcp_gs)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_state_t *stream_state;

	stream_state = __livemedia_stream_state__alloc();
	if (stream_state)
		__livemedia_stream_state__init(stream_state, master, server_rtp_port,
				server_rtcp_port, rtp_sink, udp_sink,
				total_bw, media_source, rtp_gs, rtcp_gs);

	return stream_state;
}
void livemedia__delete__stream_state(livemedia_stream_state_t *stream_state)
{
	if (stream_state && stream_state->delete)
		stream_state->delete(stream_state);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_stream_state__delete__impl(livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	printf("Log: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (stream_state) {
		__livemedia_stream_state__deinit(stream_state);
		__livemedia_stream_state__free(stream_state);
	}
}

/*
 * Functions in header file
 */
unsigned int *livemedia_stream_state__reference_count(livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
groupsock_port_t const *livemedia_stream_state__server_rtp_port(
		livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return NULL;
}

groupsock_port_t const *livemedia_stream_state__server_rtcp_port(
		livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return NULL;
}

livemedia_rtp_sink_t *livemedia_stream_state__rtp_sink(
		livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return stream_state->rtp_sink;
}

livemedia_rtcp_instance_t *livemedia_stream_state__rtcp_instance(
		livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return NULL;
}

float livemedia_stream_state__stream_duration(livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 0.0;
}

livemedia_framed_source_t *livemedia_stream_state__media_source(
		livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return stream_state->media_source;
}

float *livemedia_stream_state__start_npt(livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &stream_state->start_npt;
}

/*
 * Normal functions
 */

void livemedia_stream_state__start_playing(livemedia_stream_state_t *stream_state,
		livemedia_destinations_t *dests, unsigned int client_session_id,
		usage_environment__task_func *rtcp_rr_handler, void *rtcp_rr_handler_client_data,
		livemedia__server_request_alternative_byte_handler *server_request_alternative_byte_handler,
		void *server_request_alternative_byte_handler_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_port_t *port;
	int tmp_fd;
	if (dests == NULL)
		return;

	if (stream_state->rtcp_instance == NULL && stream_state->rtp_sink != NULL) {
		/* Create (and start) a 'RTCP instance' for this RTP sink: */
		stream_state->rtcp_instance = livemedia_on_demand_server_media_subsession__create_rtcp(
				stream_state->master, stream_state->rtcp_gs, stream_state->total_bw,
				(unsigned char *)stream_state->master->cname, stream_state->rtp_sink);
		/* Note: This starts RTCP running automatically */
		livemedia_rtcp_instance__set_app_handler(stream_state->rtcp_instance,
				stream_state->master->app_handler_task,
				stream_state->master->app_handler_client_data);
	}
	
	if (dests->is_tcp) {
		/* Change RTP and RTCP to use the TCP socket instead of UDP: */
		if (stream_state->rtp_sink != NULL) {
			livemedia_rtp_sink__add_stream_socket(stream_state->rtp_sink,
					dests->tcp_socket_num, dests->rtp_channel_id);
			livemedia_rtp_interface__set_server_request_alternative_byte_handler__static(
					dests->tcp_socket_num, server_request_alternative_byte_handler,
					server_request_alternative_byte_handler_client_data);
			/* So that we continue to handle RTSP commands from the client */
		}
		if (stream_state->rtcp_instance != NULL) {
			livemedia_rtcp_instance__add_stream_socket(stream_state->rtcp_instance, dests->tcp_socket_num,
					dests->rtcp_channel_id);
			port = groupsock__new__port((port_num_bits)dests->rtcp_channel_id);
			uv_fileno(dests->tcp_socket_num, &tmp_fd);
			livemedia_rtcp_instance__set_specific_rr_handler(
					stream_state->rtcp_instance, tmp_fd,
					*port, rtcp_rr_handler, rtcp_rr_handler_client_data);
			groupsock__delete__port(port);
		}
	} else {
		/* Tell the RTP and RTCP 'groupsocks' about this destination
		 * (in case they don't already have it) */
		if (stream_state->rtp_gs != NULL)
			groupsock_groupsock__add_destination(stream_state->rtp_gs, &dests->addr,
					&dests->rtp_port, client_session_id);
		if(stream_state->rtcp_gs != NULL && !(stream_state->rtcp_gs == stream_state->rtp_gs && 
					groupsock_port__num(&dests->rtcp_port) == groupsock_port__num(&dests->rtp_port))) {
			groupsock_groupsock__add_destination(stream_state->rtcp_gs, &dests->addr,
					&dests->rtcp_port, client_session_id);
		}
		if (stream_state->rtcp_instance != NULL) {
			livemedia_rtcp_instance__set_specific_rr_handler(stream_state->rtcp_instance,
					dests->addr.s_addr, dests->rtcp_port, rtcp_rr_handler, rtcp_rr_handler_client_data);
		}
	}
	
	if (stream_state->rtcp_instance != NULL) {
    /* Hack: Send an initial RTCP "SR" packet, before the initial RTP packet, so that receivers will (likely) be able to
     * get RTCP-synchronized presentation times immediately: */
		livemedia_rtcp_instance__send_report(stream_state->rtcp_instance);
	}
	if (!stream_state->are_currently_playing && stream_state->media_source != NULL) {
		if (stream_state->rtp_sink != NULL) {
			livemedia_media_sink__start_playing((livemedia_media_sink_t *)stream_state->rtp_sink,
					stream_state->media_source, livemedia__after_playing_stream_state, stream_state);
			stream_state->are_currently_playing = true;
		} else if (stream_state->udp_sink != NULL) {
			livemedia_media_sink__start_playing((livemedia_media_sink_t *)stream_state->udp_sink,
					stream_state->media_source, livemedia__after_playing_stream_state, stream_state);
			stream_state->are_currently_playing = true;
		}
	}
}

void livemedia_stream_state__pause(livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_stream_state__send_rtcp_app_packet(livemedia_stream_state_t *stream_state,
		u_int8_t subtype, char const *name, 
		u_int8_t *app_dependent_data, unsigned int app_dependent_data_size)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_stream_state__end_playing(livemedia_stream_state_t *stream_state,
		livemedia_destinations_t *destinations, unsigned int client_session_id)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_stream_state__reclaim(livemedia_stream_state_t *stream_state)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
