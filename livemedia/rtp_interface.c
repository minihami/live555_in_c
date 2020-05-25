#include "rtp_interface.h"
#include "groupsock_helper.h"
#include <stdio.h>

/********* Helper Functions - Definition *********/

typedef struct _poll_cb_data_t {
	void *client_data;
	usage_environment__background_handler_proc *handler;
} poll_cb_data_t;

static void poll_cb(uv_poll_t *handle, int status, int events)
{
	((poll_cb_data_t *)handle->data)->handler(handle->data, events);
}


/* Helper routines and data structures, used to implement
 * sending/receiving RTP/RTCP over a TCP socket: */
/*****************************************************************
 * tcpStreamRecord (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_tcp_stream_record_t {
	struct _livemedia_tcp_stream_record_t *next;
	uv_tcp_t *stream_socket_num;
	unsigned char stream_channel_id;

	void (*delete)(struct _livemedia_tcp_stream_record_t *tcp_stream_record);
} livemedia_tcp_stream_record_t;


/* Reading RTP-over-TCP is implemented using two levels of hash tables.
 * The top-level hash table maps TCP socket numbers to a
 * "SocketDescriptor" that contains a hash table for each of the
 * sub-channels that are reading from this socket. */
/*****************************************************************
 * SocketDesriptor (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_socket_descriptor_t {
	uv_tcp_t *our_socket_num;
	livemedia_hash_table_t *sub_channel_hash_table;
	livemedia__server_request_alternative_byte_handler *server_request_alternative_byte_handler;
	void *server_request_alternative_byte_handler_client_data;
	u_int8_t stream_channel_id;
	u_int8_t size_byte1;
	bool read_error_occurred;
	bool delete_myself_next;
	bool are_in_read_handler_loop;
	enum { AWAITING_DOLLAR, AWAITING_STREAM_CHANNEL_ID, AWAITING_SIZE1, AWAITING_SIZE2, AWAITING_PACKET_DATA } tcp_reading_state;

	void (*delete)(struct _livemedia_socket_descriptor_t *socket_descriptor);

	uv_poll_t *poll_handle;
} livemedia_socket_descriptor_t;

/*****************************************************************
 * tcpStreamRecord (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_tcp_stream_record_t *__livemedia_tcp_stream_record__alloc(void);
void __livemedia_tcp_stream_record__init(livemedia_tcp_stream_record_t *tcp_stream_record,
		uv_tcp_t *stream_socket_num, unsigned char stream_channel_id, livemedia_tcp_stream_record_t *next);
void __livemedia_tcp_stream_record__deinit(livemedia_tcp_stream_record_t *tcp_stream_record);
void __livemedia_tcp_stream_record__free(livemedia_tcp_stream_record_t *tcp_stream_record);
/*
 * new() and delete() functions
 */
livemedia_tcp_stream_record_t *livemedia__new__tcp_stream_record(uv_tcp_t *stream_socket_num,
		unsigned char stream_channel_id, livemedia_tcp_stream_record_t *next);
void livemedia__delete__tcp_stream_record(livemedia_tcp_stream_record_t *tcp_stream_record);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_tcp_stream_record__delete__impl(livemedia_tcp_stream_record_t *tcp_stream_record);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * SocketDescriptor (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_socket_descriptor_t *__livemedia_socket_descriptor__alloc(void);
void __livemedia_socket_descriptor__init(livemedia_socket_descriptor_t *socket_descriptor, uv_tcp_t *socket_num);
void __livemedia_socket_descriptor__deinit(livemedia_socket_descriptor_t *socket_descriptor);
void __livemedia_socket_descriptor__free(livemedia_socket_descriptor_t *socket_descriptor);
/*
 * new() and delete() functions
 */
livemedia_socket_descriptor_t *livemedia__new__socket_descriptor(uv_tcp_t *socket_num);
void livemedia__delete__socket_descriptor(livemedia_socket_descriptor_t *socket_descriptor);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_socket_descriptor__delete__impl(livemedia_socket_descriptor_t *socket_descriptor);

/*
 * Functions in header file
 */
void livemedia_socket_descriptor__set_server_request_alternative_byte_handler(
		livemedia_socket_descriptor_t *socket_descriptor,
		livemedia__server_request_alternative_byte_handler *handler,
		void *client_data);

/*
 * Normal functions
 */
void livemedia_socket_descriptor__register_rtp_interface(
		livemedia_socket_descriptor_t *socket_descriptor, unsigned char stream_channel_id,
		livemedia_rtp_interface_t *rtp_interface);

livemedia_rtp_interface_t *livemedia_socket_descriptor__lookup_rtp_interface(
		livemedia_socket_descriptor_t *socket_descriptor, unsigned char stream_channel_id);

void livemedia_socket_descriptor__deregister_rtp_interface(
		livemedia_socket_descriptor_t *socket_descriptor, unsigned char stream_channel_id);

void livemedia_socket_descriptor__tcp_read_handler__static(
		livemedia_socket_descriptor_t *socket_descriptor, int mask);

bool livemedia_socket_descriptor__tcp_read_handler1(
		livemedia_socket_descriptor_t *socket_descriptor, int mask);


/*****************************************************************
 * tcpStreamRecord (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_tcp_stream_record_t *__livemedia_tcp_stream_record__alloc(void)
{
	livemedia_tcp_stream_record_t *tcp_stream_record;
	tcp_stream_record = (livemedia_tcp_stream_record_t *)calloc(1, sizeof(livemedia_tcp_stream_record_t));
	if (tcp_stream_record == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_tcp_stream_record_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return tcp_stream_record;
}

void __livemedia_tcp_stream_record__init(livemedia_tcp_stream_record_t *tcp_stream_record,
		uv_tcp_t *stream_socket_num, unsigned char stream_channel_id, livemedia_tcp_stream_record_t *next)
{
	/* constructor */
	tcp_stream_record->next = next;
	tcp_stream_record->stream_socket_num = stream_socket_num;
	tcp_stream_record->stream_channel_id = stream_channel_id;
	
	/* virtual functions configuration */ 
	tcp_stream_record->delete = livemedia_tcp_stream_record__delete__impl;
}

void __livemedia_tcp_stream_record__deinit(livemedia_tcp_stream_record_t *tcp_stream_record)
{
	/* destructor */
	livemedia__delete__tcp_stream_record(tcp_stream_record->next);
}

void __livemedia_tcp_stream_record__free(livemedia_tcp_stream_record_t *tcp_stream_record)
{
	if (tcp_stream_record) {
		free(tcp_stream_record);
		tcp_stream_record = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_tcp_stream_record_t *livemedia__new__tcp_stream_record(uv_tcp_t *stream_socket_num,
		unsigned char stream_channel_id, livemedia_tcp_stream_record_t *next)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_tcp_stream_record_t *tcp_stream_record;

	tcp_stream_record = __livemedia_tcp_stream_record__alloc();
	if (tcp_stream_record)
		__livemedia_tcp_stream_record__init(tcp_stream_record, stream_socket_num,
				stream_channel_id, next);

	return tcp_stream_record;
}
void livemedia__delete__tcp_stream_record(livemedia_tcp_stream_record_t *tcp_stream_record)
{
	if (tcp_stream_record && tcp_stream_record->delete)
		tcp_stream_record->delete(tcp_stream_record);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_tcp_stream_record__delete__impl(livemedia_tcp_stream_record_t *tcp_stream_record)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (tcp_stream_record) {
		__livemedia_tcp_stream_record__deinit(tcp_stream_record);
		__livemedia_tcp_stream_record__free(tcp_stream_record);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * SocketDescriptor (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
static livemedia_hash_table_t *livemedia__socket_hash_table(bool create_if_not_present /*defalut: true */)
{
	livemedia_tables_t *our_tables;

	our_tables = livemedia_tables__get_our_tables__static(create_if_not_present);
	if (our_tables == NULL)
		return NULL;

	if (our_tables->socket_table == NULL) {
		/* Create a new socket number -> SocketDescriptor mapping table: */
		our_tables->socket_table = livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	}
	return (livemedia_hash_table_t *)(our_tables->socket_table);
}
static livemedia_socket_descriptor_t *livemedia__lookup_socket_descriptor(
		uv_tcp_t *sock_num, bool create_if_not_found /* default: true */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_hash_table_t *table;
	char *key;
	int sock_fd;
	livemedia_socket_descriptor_t *socket_descriptor;
	livemedia_tables_t *our_tables;

	table = livemedia__socket_hash_table(create_if_not_found);
	if (table == NULL)
		return NULL;

	uv_fileno((uv_handle_t *)sock_num, &sock_fd);
	key = (char *)(long)(sock_fd);
	socket_descriptor = (livemedia_socket_descriptor_t *)(livemedia_hash_table__lookup(table, key));
	if (socket_descriptor == NULL) {
		if (create_if_not_found) {
			socket_descriptor = livemedia__new__socket_descriptor(sock_num);
			livemedia_hash_table__add(table, (char const *)(long)(sock_fd), socket_descriptor);
		} else if (livemedia_hash_table__is_empty(table)) {
			/* We can also delete the table (to reclaim space): */
			our_tables = livemedia_tables__get_our_tables__static(true);
			livemedia__delete__hash_table(table);
			our_tables->socket_table = NULL;
			livemedia_tables__reclaim_if_possible(our_tables);
		}
	}
	return socket_descriptor;
}

static void livemedia__remove_socket_description(uv_tcp_t *sock_num)
{
	int sock_fd;
	char *key;
	livemedia_hash_table_t *table;
	livemedia_tables_t *our_tables;

	uv_fileno((uv_handle_t *)sock_num, &sock_fd);
	key = (char *)(long)(sock_fd);
	table = livemedia__socket_hash_table(true);
	livemedia_hash_table__remove(table, key);

	if (livemedia_hash_table__is_empty(table)) {
		/* We can also delete the table (to reclaim space): */
		our_tables = livemedia_tables__get_our_tables__static(true);
		livemedia__delete__hash_table(table);
		our_tables->socket_table = NULL;
		livemedia_tables__reclaim_if_possible(our_tables);
	}
}

livemedia_socket_descriptor_t *__livemedia_socket_descriptor__alloc(void)
{
	livemedia_socket_descriptor_t *socket_descriptor;
	socket_descriptor = (livemedia_socket_descriptor_t *)calloc(1, sizeof(livemedia_socket_descriptor_t));
	if (socket_descriptor == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_socket_descriptor_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return socket_descriptor;
}
void __livemedia_socket_descriptor__init(livemedia_socket_descriptor_t *socket_descriptor, uv_tcp_t *socket_num)
{
	poll_cb_data_t *socket_descriptor_poll_cb_data;

	socket_descriptor->our_socket_num = socket_num;
	socket_descriptor->sub_channel_hash_table = livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	socket_descriptor->server_request_alternative_byte_handler = NULL;
	socket_descriptor->server_request_alternative_byte_handler_client_data = NULL;
	socket_descriptor->read_error_occurred =false;
	socket_descriptor->delete_myself_next = false;
	socket_descriptor->are_in_read_handler_loop = false;
	socket_descriptor->tcp_reading_state = AWAITING_DOLLAR;
	
	/*  virtual functions configuration */ 
	socket_descriptor->delete = livemedia_socket_descriptor__delete__impl;

	socket_descriptor_poll_cb_data = (poll_cb_data_t *)calloc(1, sizeof(poll_cb_data_t));
	if (socket_descriptor_poll_cb_data == NULL) {
		fprintf(stderr, "calloc(1, sizeof(poll_cb_data_t))"
				"failed in %s()\n", __func__);
	}
	socket_descriptor_poll_cb_data->client_data = (void *)socket_descriptor;

	socket_descriptor->poll_handle = (uv_poll_t *)calloc(1, sizeof(uv_poll_t));
	if (socket_descriptor->poll_handle == NULL) {
		fprintf(stderr, "calloc(1, sizeof(uv_poll_t))"
				"failed in %s()\n", __func__);
	}
	socket_descriptor->poll_handle->data = socket_descriptor_poll_cb_data;
}
void __livemedia_socket_descriptor__deinit(livemedia_socket_descriptor_t *socket_descriptor)
{
	livemedia_hash_table_iterator_t *iter;
	livemedia_rtp_interface_t *rtp_interface;
	char *key;
	u_int64_t stream_channel_id_long;
	unsigned char stream_channel_id;
	u_int8_t special_char;
	poll_cb_data_t *socket_descriptor_poll_cb_data;

	uv_read_stop((uv_stream_t *)socket_descriptor->our_socket_num);
	livemedia__remove_socket_description(socket_descriptor->our_socket_num);

	if (socket_descriptor->sub_channel_hash_table != NULL) {
		/* Remove knowledge of this socket from any "RTPInterface"s that are using it: */
		iter = livemedia_hash_table_iterator__create__static(socket_descriptor->sub_channel_hash_table);

		while ((rtp_interface = (livemedia_rtp_interface_t *)(livemedia_hash_table_iterator__next(iter, key))) != NULL) {
			stream_channel_id_long = (u_int64_t)key;
			stream_channel_id = (unsigned char)stream_channel_id_long;

			livemedia_rtp_interface__remove_stream_socket(rtp_interface,
					socket_descriptor->our_socket_num, stream_channel_id);
		}
		livemedia__delete__hash_table_iterator(iter);
    
		/* Then remove the hash table entries themselves, and then remove the hash table: */
		while (livemedia_hash_table__remove_next(socket_descriptor->sub_channel_hash_table) != NULL) {}
		livemedia__delete__hash_table(socket_descriptor->sub_channel_hash_table);
	}

	/* finally */
	if (socket_descriptor->server_request_alternative_byte_handler != NULL) {
    /* Hack: Pass a special character to our alternative byte handler, to tell it that either
     * - an error occurred when reading the TCP socket, or
     * - no error occurred, but it needs to take over control of the TCP socket once again. */
		special_char = socket_descriptor->read_error_occurred ? 0xFF: 0xFE;
		(*socket_descriptor->server_request_alternative_byte_handler)(
				socket_descriptor->server_request_alternative_byte_handler_client_data, special_char);
	}

	socket_descriptor_poll_cb_data = socket_descriptor->poll_handle->data;
	if (socket_descriptor_poll_cb_data) {
		free(socket_descriptor_poll_cb_data);
		socket_descriptor_poll_cb_data = NULL;
	}
	if (socket_descriptor->poll_handle) {
		free(socket_descriptor->poll_handle);
		socket_descriptor->poll_handle = NULL;
	}
}

void __livemedia_socket_descriptor__free(livemedia_socket_descriptor_t *socket_descriptor)
{
	if (socket_descriptor) {
		free(socket_descriptor);
		socket_descriptor = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_socket_descriptor_t *livemedia__new__socket_descriptor(uv_tcp_t *socket_num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_socket_descriptor_t *socket_descriptor;

	socket_descriptor = __livemedia_socket_descriptor__alloc();
	if (socket_descriptor)
		__livemedia_socket_descriptor__init(socket_descriptor, socket_num);

	return socket_descriptor;
}
void livemedia__delete__socket_descriptor(livemedia_socket_descriptor_t *socket_descriptor)
{
	if (socket_descriptor && socket_descriptor->delete)
		socket_descriptor->delete(socket_descriptor);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_socket_descriptor__delete__impl(livemedia_socket_descriptor_t *socket_descriptor)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (socket_descriptor) {
		__livemedia_socket_descriptor__deinit(socket_descriptor);
		__livemedia_socket_descriptor__free(socket_descriptor);
	}
}
/*
 * Functions in header file
 */
void livemedia_socket_descriptor__set_server_request_alternative_byte_handler(
		livemedia_socket_descriptor_t *socket_descriptor,
		livemedia__server_request_alternative_byte_handler *handler,
		void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	socket_descriptor->server_request_alternative_byte_handler = handler;
	socket_descriptor->server_request_alternative_byte_handler_client_data = client_data;
}

/*
 * Normal functions
 */
void livemedia_socket_descriptor__register_rtp_interface(
		livemedia_socket_descriptor_t *socket_descriptor, unsigned char stream_channel_id,
		livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	bool is_first_registration;
	uv_poll_t *poll_handle;
	int sock_fd;

	is_first_registration = 
		livemedia_hash_table__is_empty(socket_descriptor->sub_channel_hash_table);
#if defined(DEBUG_SEND)||defined(DEBUG_RECEIVE)
	fprintf(stderr, "SocketDescriptor(socket %d)::registerRTPInterface(channel %d): isFirstRegistration %d\n", socket_descriptor->our_socket_num, stream_channel_id, is_first_registration);
#endif
	livemedia_hash_table__add(socket_descriptor->sub_channel_hash_table, 
			(char const *)(long)stream_channel_id, rtp_interface);

	if (is_first_registration) {
		/* Arrange to handle reads on this TCP socket: */
		poll_handle = socket_descriptor->poll_handle;
		((poll_cb_data_t *)poll_handle->data)->handler = 
			livemedia_socket_descriptor__tcp_read_handler__static;

		uv_fileno((uv_handle_t *)socket_descriptor->our_socket_num, &sock_fd);
		uv_poll_init(uv_default_loop(), poll_handle, sock_fd);
		uv_poll_start(poll_handle, UV_READABLE | UV_DISCONNECT, poll_cb);
	}
}

livemedia_rtp_interface_t *livemedia_socket_descriptor__lookup_rtp_interface(
		livemedia_socket_descriptor_t *socket_descriptor, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_socket_descriptor__deregister_rtp_interface(
		livemedia_socket_descriptor_t *socket_descriptor, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_socket_descriptor__tcp_read_handler__static(
		livemedia_socket_descriptor_t *socket_descriptor, int mask)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Call the read handler until it returns false, with a limit to avoid starving other sockets */
	unsigned int count;
	
	count = 2000;
	socket_descriptor->are_in_read_handler_loop = true;
	while (!socket_descriptor->delete_myself_next && 
			livemedia_socket_descriptor__tcp_read_handler1(socket_descriptor, mask) && 
			--count > 0) {}
	socket_descriptor->are_in_read_handler_loop = false;
	if (socket_descriptor->delete_myself_next)
		livemedia__delete__socket_descriptor(socket_descriptor);
}

bool livemedia_socket_descriptor__tcp_read_handler1(
		livemedia_socket_descriptor_t *socket_descriptor, int mask)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* We expect the following data over the TCP channel:
	 *   optional RTSP command or response bytes (before the first '$' character)
	 *   a '$' character
	 *   a 1-byte channel id
	 *   a 2-byte packet size (in network byte order)
	 *   the packet data.
	 * However, because the socket is being read asynchronously, this data might arrive in pieces. */

	u_int8_t c;
	struct sockaddr_in from_address;
	int result;
	bool call_again;
	unsigned short size;
	livemedia_rtp_interface_t *rtp_interface;

	if (socket_descriptor->tcp_reading_state != AWAITING_PACKET_DATA) {
	}
}

/*****************************************************************
 * RTPInterface (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtp_interface_t *__livemedia_rtp_interface__alloc(void)
{
	livemedia_rtp_interface_t *rtp_interface;
	rtp_interface = (livemedia_rtp_interface_t *)calloc(1, sizeof(livemedia_rtp_interface_t));
	if (rtp_interface == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtp_interface_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtp_interface;
}
void __livemedia_rtp_interface__init(livemedia_rtp_interface_t *rtp_interface,
		livemedia_medium_t *owner, groupsock_groupsock_t *gs)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int sock_fd;
	poll_cb_data_t *udp_poll_cb_data;

	rtp_interface->owner = owner;
	rtp_interface->gs = gs;
	rtp_interface->tcp_streams = NULL;
	rtp_interface->next_tcp_read_size = 0;
	rtp_interface->next_tcp_read_stream_socket_num = -1;
	rtp_interface->next_tcp_read_stream_channel_id = 0xFF;
	rtp_interface->read_handler_proc = NULL;
	rtp_interface->aux_read_handler_func = NULL;
	rtp_interface->aux_read_handler_client_data = NULL;
  /* Make the socket non-blocking, even though it will be read from only asynchronously, when packets arrive.
   * The reason for this is that, in some OSs, reads on a blocking socket can (allegedly) sometimes block,
   * even if the socket was previously reported (e.g., by "select()") as having data available.
   * (This can supposedly happen if the UDP checksum fails, for example.) */

	uv_fileno((uv_handle_t *)(groupsock_socket__socket_num((groupsock_socket_t *)rtp_interface->gs)), &sock_fd);
	groupsock__make_socket_non_blocking(sock_fd);
	groupsock__increase_send_buffer_to(sock_fd, 50 * 1024);
	
	/* virtual functions configuration */ 

	rtp_interface->delete = livemedia_rtp_interface__delete__impl;

	rtp_interface->poll_handle = (uv_poll_t *)calloc(1, sizeof(uv_poll_t));
	if (rtp_interface->poll_handle == NULL) {
		fprintf(stderr, "calloc(1, sizeof(uv_poll_t))"
				"failed in %s()\n", __func__);
	}
	udp_poll_cb_data = (poll_cb_data_t *)calloc(1, sizeof(poll_cb_data_t));
	if (udp_poll_cb_data == NULL) {
		fprintf(stderr, "calloc(1, sizeof(upll_cb_data_t))"
				"failed in %s()\n", __func__);
	}
	udp_poll_cb_data->client_data = (void *)rtp_interface;

	rtp_interface->poll_handle->data = (void *)udp_poll_cb_data;
}
void __livemedia_rtp_interface__deinit(livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* destructor */
	livemedia_rtp_interface__stop_network_reading(rtp_interface);
	livemedia__delete__tcp_stream_record(rtp_interface->tcp_streams);

	if (rtp_interface->poll_handle->data) {
		free(rtp_interface->poll_handle->data);
		rtp_interface->poll_handle->data = NULL;
	}
	if (rtp_interface->poll_handle) {
		free(rtp_interface->poll_handle);
		rtp_interface->poll_handle= NULL;
	}
}
void __livemedia_rtp_interface__free(livemedia_rtp_interface_t *rtp_interface)
{
	if (rtp_interface) {
		free(rtp_interface);
		rtp_interface = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtp_interface_t *livemedia__new__rtp_interface(
		livemedia_medium_t *owner, groupsock_groupsock_t *gs)
{
#ifdef webgate_debug
	printf("log: %s() in %s: %d line\n", __func__, __file__, __line__);
#endif
	livemedia_rtp_interface_t *rtp_interface;

	rtp_interface = __livemedia_rtp_interface__alloc();
	if (rtp_interface)
		__livemedia_rtp_interface__init(rtp_interface, owner, gs);

	return rtp_interface;
}
void livemedia__delete__rtp_interface(livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtp_interface && rtp_interface->delete)
		rtp_interface->delete(rtp_interface);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtp_interface__delete__impl(livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	if (rtp_interface) {
		__livemedia_rtp_interface__deinit(rtp_interface);
		__livemedia_rtp_interface__free(rtp_interface);
	}
}

/*
 * Functions in header file
 */
groupsock_groupsock_t *livemedia_rtp_interface__gs(livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtp_interface->gs;
}

void livemedia_rtp_interface__set_auxilliary_read_handler(
		livemedia_rtp_interface_t *rtp_interface, livemedia__aux_handler_func *handler_func, void *handler_client_data)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_interface__forget_our_groupsock(livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	rtp_interface->gs = NULL;
}


/*
 * Normal functions
 */
void livemedia_rtp_interface__set_stream_socket(livemedia_rtp_interface_t *rtp_interface,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_interface__add_stream_socket(livemedia_rtp_interface_t *rtp_interface,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_tcp_stream_record_t *streams;
	livemedia_socket_descriptor_t *socket_descriptor;

	if (sock_num == NULL)
		return;

	for (streams = rtp_interface->tcp_streams; streams != NULL;
			streams = streams->next) {
		if (streams->stream_socket_num == sock_num &&
				streams->stream_channel_id == stream_channel_id) {
			return; /* we already have it */
		}
	}
	
	rtp_interface->tcp_streams = livemedia__new__tcp_stream_record(sock_num,
			stream_channel_id, rtp_interface->tcp_streams);

	/* Also, make sure this new socket is set up for receiving RTP/RTCP-over-TCP: */
	socket_descriptor = livemedia__lookup_socket_descriptor(sock_num, true);
	livemedia_socket_descriptor__register_rtp_interface(socket_descriptor, 
			stream_channel_id, rtp_interface);

}

static void livemedia__deregister_socket(uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
	livemedia_socket_descriptor_t *socket_descriptor;

	socket_descriptor = livemedia__lookup_socket_descriptor(sock_num, false);
	if(socket_descriptor != NULL) {
		livemedia_socket_descriptor__deregister_rtp_interface(socket_descriptor,
				stream_channel_id);
        /* Note: This may delete "socketDescriptor",
         * if no more interfaces are using this socket */
	}
}

void livemedia_rtp_interface__remove_stream_socket(livemedia_rtp_interface_t *rtp_interface, 
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtp_interface__set_server_request_alternative_byte_handler__static(
		uv_tcp_t *socket_num, livemedia__server_request_alternative_byte_handler *handler,
		void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_socket_descriptor_t *socket_descriptor;

	socket_descriptor = livemedia__lookup_socket_descriptor(socket_num, false);
	
	if (socket_descriptor != NULL)
		livemedia_socket_descriptor__set_server_request_alternative_byte_handler(
				socket_descriptor, handler, client_data);

}

void livemedia_rtp_interface__clear_server_request_alternative_byte_handler__static(
		int socket_num)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_rtp_interface__send_packet(livemedia_rtp_interface_t *rtp_interface,
		unsigned char *packet, unsigned int packet_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	bool success;
	livemedia_tcp_stream_record_t *next_stream;
	livemedia_tcp_stream_record_t *stream;

	success = true; /* we'll return False instead if any of the sends fail */

	/* Normal case: Send as a UDP packet: */
	if (!groupsock_groupsock__output(rtp_interface->gs, packet, packet_size, NULL))
		success = false;

	/* Also, send over each of our TCP sockets: */
	for (stream = rtp_interface->tcp_streams; stream != NULL; stream = next_stream) {
		next_stream = stream->next;
		/* Set this now, in case the following deletes "stream": */
		if (!livemedia_rtp_interface__send_rtp_or_rtcp_packet_over_tcp(rtp_interface, 
					packet, packet_size, stream->stream_socket_num, 
					stream->stream_channel_id)) {
			success = false;
		}
	}
	
	return success;
}


void livemedia_rtp_interface__start_network_reading(livemedia_rtp_interface_t *rtp_interface,
		usage_environment__background_handler_proc *handler_proc)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int udp_fd;
	uv_udp_t *udp_handle;
	
	livemedia_tcp_stream_record_t *streams;
	livemedia_socket_descriptor_t *socket_descriptor;
	uv_poll_t *udp_poll_handle;

	udp_poll_handle = rtp_interface->poll_handle;

	((poll_cb_data_t *)udp_poll_handle->data)->client_data = rtp_interface->owner;
	((poll_cb_data_t *)udp_poll_handle->data)->handler = handler_proc;

	udp_handle = groupsock_socket__socket_num(rtp_interface->gs);
	uv_fileno((uv_handle_t *)udp_handle, &udp_fd);
	if (!uv_is_active(udp_poll_handle)) {
		uv_poll_init(uv_default_loop(), udp_poll_handle, udp_fd);
	}

	/* Normal case: Arrange to read UDP packets: */
	if (uv_is_active(udp_poll_handle)) {
		uv_poll_stop(udp_poll_handle);
	}
	uv_poll_start(udp_poll_handle, UV_READABLE, poll_cb);
	/* Normal case: Arrange to read UDP packets: */

	/* Also, receive RTP over TCP, on each of our TCP connections: */
	rtp_interface->read_handler_proc = handler_proc;
	for (streams = rtp_interface->tcp_streams; streams != NULL; 
			streams = streams->next) {
		/* Get a socket descriptor for "streams->fStreamSocketNum": */
		socket_descriptor = livemedia__lookup_socket_descriptor(
				streams->stream_socket_num, true);

		/* Tell it about our subChannel: */
		livemedia_socket_descriptor__register_rtp_interface(
				socket_descriptor, streams->stream_channel_id, rtp_interface);
	}
}

bool livemedia_rtp_interface__handle_read(livemedia_rtp_interface_t *rtp_interface,
		unsigned char *buffer, unsigned int buffer_max_size,
		/* out parameters: */
		unsigned *bytes_read, struct sockaddr_in *from_address,
		int *tcp_socket_num, unsigned char *tcp_stream_channel_td,
		bool *packet_read_was_incomplete)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtp_interface__stop_network_reading(livemedia_rtp_interface_t *rtp_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_tcp_stream_record_t *streams;

	/* Normal case */
	if (rtp_interface->gs != NULL)
		uv_udp_recv_stop(groupsock_socket__socket_num((groupsock_socket_t *)rtp_interface->gs));

	/* Also trun off read handling on each of our TCP connections: */
	for (streams = rtp_interface->tcp_streams; streams != NULL; streams = streams->next) {
		livemedia__deregister_socket(streams->stream_socket_num, streams->stream_channel_id);
	}
}

bool livemedia_rtp_interface__send_rtp_or_rtcp_packet_over_tcp(
		livemedia_rtp_interface_t *rtp_interface, unsigned char *packet, unsigned packet_size,
		uv_tcp_t *socket_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int sock_fd;
	u_int8_t framing_header[4];

#ifdef DEBUG_SEND
	uv_fileno((uv_handle_t *)socke_num, &sock_fd);
	fprintf(stderr, "sendRTPorRTCPPacketOverTCP: %d bytes over channel %d (socket %d)\n",
			pakcet_size, stream_channel_id, sock_fd); 
	fflush(stderr);
#endif
	/* Send a RTP/RTCP packet over TCP, using the encoding defined in RFC 2326, section 10.12:
	 *     $<stream_channel_id><packet_size><packet>
	 * (If the initial "send()" of '$<streamChannelId><packet_size>' succeeds, then we force
	 * the subsequent "send()" for the <packet> data to succeed, even if we have to do so with
	 * a blocking "send()".) */
	do {
		framing_header[0] = '$';
		framing_header[1] = stream_channel_id;
		framing_header[2] = (u_int8_t) ((packet_size&0xFF00)>>8);
		framing_header[3] = (u_int8_t) (packet_size&0xFF);

		if (!livemedia_rtp_interface__send_data_over_tcp(rtp_interface, socket_num, framing_header, 4, false))
			break;
		if (!livemedia_rtp_interface__send_data_over_tcp(rtp_interface, socket_num, packet, packet_size, true))
			break;
#ifdef DEBUG_SEND
		fprintf(stderr, "sendRTPorRTCPPacketOverTCP: completed\n"); 
		fflush(stderr);
#endif

		return true;
	} while(0);

#ifdef DEBUG_SEND
	fprintf(stderr, "sendRTPorRTCPPacketOverTCP: failed!\n");
	fflush(stderr);
#endif
	return false;
}

#ifndef RTPINTERFACE_BLOCKING_WRITE_TIMEOUT_MS
#define RTPINTERFACE_BLOCKING_WRITE_TIMEOUT_MS 500
#endif

void send_data_over_tcp_write_cb(uv_write_t *req, int status) 
{
	livemedia_rtp_interface_t *rtp_interface;
	rtp_interface = (livemedia_rtp_interface_t *)req->data;
	if (status) {
		fprintf(stderr, "uv_write error in %s()\n", __func__);
	}

}
bool livemedia_rtp_interface__send_data_over_tcp(
		livemedia_rtp_interface_t *rtp_interface, uv_tcp_t *socket_num, u_int8_t const *data,
		unsigned int data_size, bool force_send_to_succeed)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int send_result;
	unsigned int num_bytes_remaining_to_send;
	uv_write_t *req;
	uv_buf_t *buf;
	req = (uv_write_t *)calloc(1, sizeof(uv_write_t));
	buf = (uv_buf_t *)calloc(1, sizeof(uv_buf_t));

	req->data = (void *)rtp_interface;
	buf->base = (char *)calloc(data_size, sizeof(char));
	memcpy(buf->base, data, data_size);

	uv_write(req, (uv_stream_t *)socket_num, buf, 1, send_data_over_tcp_write_cb);

	return true;
}


/*
 * Static functions implementation for ancestor
 */
