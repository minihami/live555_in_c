#ifndef __LIVEMEDIA_RTP_INTERFACE_H__
#define __LIVEMEDIA_RTP_INTERFACE_H__

#ifndef __LIVEMEDIA_MEDIA_H__
#include "media.h"
#endif

#ifndef __GROUPSOCK_GROUPSOCK_H__
#include "groupsock.h"
#endif


/* Typedef for an optional auxilliary handler fuction, to be called
 * when each new packet is read: */
typedef void (livemedia__aux_handler_func)(void *client_data, unsigned char *packet,
			    unsigned *packet_size);

typedef void (livemedia__server_request_alternative_byte_handler)(void *instance,
		u_int8_t request_byte);
/* A hack that allows a handler for RTP/RTCP packets received over TCP to process RTSP commands that may also appear within
* the same TCP connection.  A RTSP server implementation would supply a function like this - as a parameter to
* "ServerMediaSubsession::startStream()". */

/*****************************************************************
 * RTPInterface (Structure Declarations)
 ****************************************************************/
struct _livemedia_tcp_stream_record_t;
typedef struct _livemedia_rtp_interface_t {
	livemedia_medium_t *owner;
	groupsock_groupsock_t *gs;
	struct _livemedia_tcp_stream_record_t *tcp_streams;
	/* optional, for RTP-over-TCP streaming/receiving */
	unsigned short next_tcp_read_size;
    /* how much data (if any) is available to be read from the TCP stream */
	int next_tcp_read_stream_socket_num;
	unsigned char next_tcp_read_stream_channel_id;
	usage_environment__background_handler_proc *read_handler_proc;
	livemedia__aux_handler_func *aux_read_handler_func;
	void *aux_read_handler_client_data;

	void (*delete)(struct _livemedia_rtp_interface_t *rtp_interface);

	uv_poll_t *poll_handle;
} livemedia_rtp_interface_t;

/*****************************************************************
 * RTPInterface (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_rtp_interface_t *__livemedia_rtp_interface__alloc(void);
void __livemedia_rtp_interface__init(livemedia_rtp_interface_t *rtp_interface,
		livemedia_medium_t *owner, groupsock_groupsock_t *gs);
void __livemedia_rtp_interface__deinit(livemedia_rtp_interface_t *rtp_interface);
void __livemedia_rtp_interface__free(livemedia_rtp_interface_t *rtp_interface);

/*
 * new() and delete() functions
 */
livemedia_rtp_interface_t *livemedia__new__rtp_interface(
		livemedia_medium_t *owner, groupsock_groupsock_t *gs);
void livemedia__delete__rtp_interface(livemedia_rtp_interface_t *rtp_interface);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_interface__delete__impl(livemedia_rtp_interface_t *rtp_interface);

/*
 * Functions in header file
 */
groupsock_groupsock_t *livemedia_rtp_interface__gs(livemedia_rtp_interface_t *rtp_interface);
void livemedia_rtp_interface__set_auxilliary_read_handler(
		livemedia_rtp_interface_t *rtp_interface, livemedia__aux_handler_func *handler_func, void *handler_client_data);
void livemedia_rtp_interface__forget_our_groupsock(livemedia_rtp_interface_t *rtp_interface);

/*
 * Normal functions
 */
void livemedia_rtp_interface__set_stream_socket(livemedia_rtp_interface_t *rtp_interface,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);
void livemedia_rtp_interface__add_stream_socket(livemedia_rtp_interface_t *rtp_interface,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);
void livemedia_rtp_interface__remove_stream_socket(livemedia_rtp_interface_t *rtp_interface, 
		uv_tcp_t *sock_num, unsigned char stream_channel_id); 
void livemedia_rtp_interface__set_server_request_alternative_byte_handler__static(
		uv_tcp_t *socket_num, livemedia__server_request_alternative_byte_handler *handler,
		void *client_data);
void livemedia_rtp_interface__clear_server_request_alternative_byte_handler__static(
		int socket_num);
bool livemedia_rtp_interface__send_packet(livemedia_rtp_interface_t *rtp_interface,
		unsigned char *packet, unsigned int packet_size);
void livemedia_rtp_interface__start_network_reading(livemedia_rtp_interface_t *rtp_interface,
		usage_environment__background_handler_proc *handler_proc);
bool livemedia_rtp_interface__handle_read(livemedia_rtp_interface_t *rtp_interface,
		unsigned char *buffer, unsigned int buffer_max_size,
		/* out parameters: */
		unsigned *bytes_read, struct sockaddr_in *from_address,
		int *tcp_socket_num, unsigned char *tcp_stream_channel_td,
		bool *packet_read_was_incomplete);
void livemedia_rtp_interface__stop_network_reading(livemedia_rtp_interface_t *rtp_interface);
bool livemedia_rtp_interface__send_rtp_or_rtcp_packet_over_tcp(
		livemedia_rtp_interface_t *rtp_interface, unsigned char *packet, unsigned packet_size,
		uv_tcp_t *socket_num, unsigned char stream_channel_id);
bool livemedia_rtp_interface__send_data_over_tcp(
		livemedia_rtp_interface_t *rtp_interface, uv_tcp_t *socket_num, u_int8_t const *data,
		unsigned int data_size, bool force_send_to_succeed);


#endif
