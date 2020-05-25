#include "net_interface.h"
#include "groupsock_helper.h"
#include "uv.h"

#if 0
TODO: C++ library delete
#ifndef NO_SSTREAM
#include <sstream>
#endif
#endif
#include <stdio.h>


/*****************************************************************
 * NetInterface (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_interface_t *__groupsock_net_interface__alloc(void)
{
	groupsock_net_interface_t *net_interface;
	net_interface = (groupsock_net_interface_t *)calloc(1, sizeof(groupsock_net_interface_t));
	if (net_interface == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_net_interface_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return net_interface;
}
void __groupsock_net_interface__init(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	net_interface->delete = groupsock_net_interface__delete__impl;
}

void __groupsock_net_interface__deinit(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void __groupsock_net_interface__free(groupsock_net_interface_t *net_interface)
{
	if (net_interface) {
		free(net_interface);
		net_interface = NULL;
	}
}

/*
 * new() and delete() functions
 */
groupsock_net_interface_t *groupsock__new__net_interface(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_interface_t *net_interface;

	net_interface = __groupsock_net_interface__alloc();
	if (net_interface)
		__groupsock_net_interface__init(net_interface);

	return net_interface;
}
void groupsock__delete__net_interface(groupsock_net_interface_t *net_interface)
{
	if (net_interface && net_interface->delete)
		net_interface->delete(net_interface);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_interface__delete__impl(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_interface) {
		__groupsock_net_interface__deinit(net_interface);
		__groupsock_net_interface__free(net_interface);
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

/*****************************************************************
 * DirectedNetInterface:NetInterface (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_directed_net_interface_t *__groupsock_directed_net_interface__alloc(void)
{
	groupsock_directed_net_interface_t *directed_net_interface;
	directed_net_interface = (groupsock_directed_net_interface_t *)calloc(1, sizeof(groupsock_directed_net_interface_t));
	if (directed_net_interface == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_directed_net_interface_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return directed_net_interface;
}
void __groupsock_directed_net_interface__init(groupsock_directed_net_interface_t *directed_net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	__groupsock_net_interface__init(&directed_net_interface->net_interface);

	((groupsock_net_interface_t *)directed_net_interface)->delete = groupsock_directed_net_interface__delete__impl;
}
void __groupsock_directed_net_interface__deinit(groupsock_directed_net_interface_t *directed_net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	__groupsock_net_interface__deinit(&directed_net_interface->net_interface);
}
void __groupsock_directed_net_interface__free(groupsock_directed_net_interface_t *directed_net_interface)
{
	if (directed_net_interface) {
		free(directed_net_interface);
		directed_net_interface = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_directed_net_interface_t *groupsock__new__directed_net_interface(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_directed_net_interface_t *directed_net_interface;

	directed_net_interface = __groupsock_directed_net_interface__alloc();
	if (directed_net_interface)
		__groupsock_directed_net_interface__init(directed_net_interface);

	return directed_net_interface;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_directed_net_interface__delete__impl(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_directed_net_interface_t *directed_net_interface;

	directed_net_interface = (groupsock_directed_net_interface_t *)net_interface;

	if (directed_net_interface) {
		__groupsock_directed_net_interface__deinit(directed_net_interface);
		__groupsock_directed_net_interface__free(directed_net_interface);
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

/*****************************************************************
 * DirectedNetInterfaceSet (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_directed_net_interface_set_t *__groupsock_directed_net_interface_set__alloc(void)
{
	groupsock_directed_net_interface_set_t *directed_net_interface_set;
	directed_net_interface_set = (groupsock_directed_net_interface_set_t *)calloc(1, sizeof(groupsock_directed_net_interface_set_t));
	if (directed_net_interface_set == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_directed_net_interface_set_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return directed_net_interface_set;
}

void __groupsock_directed_net_interface_set__init(groupsock_directed_net_interface_set_t *directed_net_interface_set)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	directed_net_interface_set->table = livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	
	directed_net_interface_set->delete = groupsock_directed_net_interface_set__delete__impl;
}

void __groupsock_directed_net_interface_set__deinit(groupsock_directed_net_interface_set_t *directed_net_interface_set)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia__delete__hash_table(directed_net_interface_set->table);
}

void __groupsock_directed_net_interface_set__free(groupsock_directed_net_interface_set_t *directed_net_interface_set)
{
	if (directed_net_interface_set) {
		free(directed_net_interface_set);
		directed_net_interface_set = NULL;
	}
}

/*
 * new() and delete() functions
 */
groupsock_directed_net_interface_set_t *groupsock__new__directed_net_interface_set(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_directed_net_interface_set_t *directed_net_interface_set;

	directed_net_interface_set = __groupsock_directed_net_interface_set__alloc();
	if (directed_net_interface_set)
		__groupsock_directed_net_interface_set__init(directed_net_interface_set);

	return directed_net_interface_set;
}
void groupsock__delete__directed_net_interface_set(groupsock_directed_net_interface_set_t *directed_net_interface_set)
{
	if (directed_net_interface_set && directed_net_interface_set->delete)
		directed_net_interface_set->delete(directed_net_interface_set);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_directed_net_interface_set__delete__impl(groupsock_directed_net_interface_set_t *directed_net_interface_set)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (directed_net_interface_set) {
		__groupsock_directed_net_interface_set__deinit(directed_net_interface_set);
		__groupsock_directed_net_interface_set__free(directed_net_interface_set);
	}
}

/*
 * Functions in header file
 */
bool groupsock_directed_net_interface_set__is_empty(
		groupsock_directed_net_interface_set_t *directed_net_interface_set)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_hash_table__is_empty(directed_net_interface_set->table);
}

/*
 * Normal functions
 */
groupsock_directed_net_interface_t *groupsock_directed_net_interface_set__add(
		groupsock_directed_net_interface_set_t *directed_net_interface_set,
		groupsock_directed_net_interface_t const *interf)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (groupsock_directed_net_interface_t *)livemedia_hash_table__add(
			directed_net_interface_set->table, (char *)interf, (void *)interf);
}

bool groupsock_directed_net_interface_set__remove(
		groupsock_directed_net_interface_set_t *directed_net_interface_set,
		groupsock_directed_net_interface_t const *interf)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_hash_table__remove(directed_net_interface_set->table, (char *)interf);
}

/*****************************************************************
 * DirectedNetInterfaceSet::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_directed_net_interface_set_iterator_t *__groupsock_directed_net_interface_set_iterator__alloc(void)
{
	groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator;
	directed_net_interface_set_iterator = (groupsock_directed_net_interface_set_iterator_t *)calloc(1, sizeof(groupsock_directed_net_interface_set_iterator_t));
	if (directed_net_interface_set_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_directed_net_interface_set_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return directed_net_interface_set_iterator;
}

void __groupsock_directed_net_interface_set_iterator__init(
		groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator,
		groupsock_directed_net_interface_set_t *interfaces)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	directed_net_interface_set_iterator->iter =
		livemedia_hash_table_iterator__create__static(interfaces->table);

	directed_net_interface_set_iterator->delete = groupsock_directed_net_interface_set_iterator__delete__impl;
}

void __groupsock_directed_net_interface_set_iterator__deinit(groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia__delete__hash_table_iterator(directed_net_interface_set_iterator->iter);
}

void __groupsock_directed_net_interface_set_iterator__free(groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator)
{
	if (directed_net_interface_set_iterator) {
		free(directed_net_interface_set_iterator);
		directed_net_interface_set_iterator = NULL;
	}
}

/*
 * new() and delete() functions
 */
groupsock_directed_net_interface_set_iterator_t *groupsock__new__directed_net_interface_set_iterator(
		groupsock_directed_net_interface_set_t *interfaces)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator;

	directed_net_interface_set_iterator = __groupsock_directed_net_interface_set_iterator__alloc();
	if (directed_net_interface_set_iterator)
		__groupsock_directed_net_interface_set_iterator__init(
				directed_net_interface_set_iterator, interfaces);

	return directed_net_interface_set_iterator;
}

void groupsock__delete__directed_net_interface_set_iterator(groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator)
{
	if (directed_net_interface_set_iterator && directed_net_interface_set_iterator->delete)
		directed_net_interface_set_iterator->delete(directed_net_interface_set_iterator);
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_directed_net_interface_set_iterator__delete__impl(groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (directed_net_interface_set_iterator) {
		__groupsock_directed_net_interface_set_iterator__deinit(directed_net_interface_set_iterator);
		__groupsock_directed_net_interface_set_iterator__free(directed_net_interface_set_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_directed_net_interface_t *groupsock_directed_net_interface_set_iterator__next(
		groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	char const *key; /* dummy */

	return (groupsock_directed_net_interface_t *)livemedia_hash_table_iterator__next(
			directed_net_interface_set_iterator->iter, &key);
}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * Socket:NetInterface (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_socket_t *__groupsock_socket__alloc(void)
{
	groupsock_socket_t *socket;
	socket = (groupsock_socket_t *)calloc(1, sizeof(groupsock_socket_t));
	if (socket == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_socket_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return socket;
}


static int socket__debug_level = 1;
void __groupsock_socket__init(groupsock_socket_t *socket, groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	__groupsock_net_interface__init(&socket->net_interface);

	socket->debug_level = &socket__debug_level;
	socket->port = port;
	socket->socket_num = groupsock__setup_datagram_socket(port);
	
	((groupsock_net_interface_t *)socket)->delete = groupsock_socket__delete__impl;
}
void __groupsock_socket__deinit(groupsock_socket_t *socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_socket__reset(socket);

	__groupsock_net_interface__deinit(&socket->net_interface);
}
void __groupsock_socket__free(groupsock_socket_t *socket)
{
	if (socket) {
		free(socket);
		socket = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_socket_t *groupsock__new__socket(groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_socket_t *socket;

	socket = __groupsock_socket__alloc();
	if (socket)
		__groupsock_socket__init(socket, port);

	return socket;
}
/*
 * Functions for virtual functions
 */
bool groupsock_socket__handle_read(groupsock_socket_t *socket, unsigned char *buffer,
		unsigned buffer_max_size, unsigned int *bytes_read, struct sockaddr_in *from_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (socket && socket->handle_read) 
		socket->handle_read(socket, buffer, buffer_max_size, bytes_read, from_address);
}

/*
 * Override functions
 */
void groupsock_socket__delete__impl(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_socket_t *socket;

	socket = (groupsock_socket_t *)net_interface;

	if (socket) {
		__groupsock_socket__deinit(socket);
		__groupsock_socket__free(socket);
	}
}

/*
 * Functions in header file
 */
uv_udp_t *groupsock_socket__socket_num(groupsock_socket_t *socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return socket->socket_num;
}

groupsock_port_t groupsock_socket__port(groupsock_socket_t *socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return socket->port;
}


/*
 * Normal functions
 */
void groupsock_socket__reset(groupsock_socket_t *socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (socket->socket_num)
		uv_close((uv_handle_t *)socket->socket_num, NULL);
	socket->socket_num = NULL;
}

void groupsock_socket__change_port_close_cb(uv_handle_t *handle)
{
	/*TODO: callback definition */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
bool groupsock_socket__change_port(groupsock_socket_t *socket, groupsock_port_t new_port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	uv_udp_t *old_socket_num;
	int socket_fd;
	unsigned int old_receive_buffer_size;
	unsigned int old_send_buffer_size;

	old_socket_num = socket->socket_num;
	uv_fileno((uv_handle_t *)socket->socket_num, &socket_fd);

	old_receive_buffer_size = groupsock__get_receive_buffer_size(socket_fd);
	old_send_buffer_size = groupsock__get_send_buffer_size(socket_fd);
	uv_close((uv_handle_t *)socket->socket_num, groupsock_socket__change_port_close_cb);

	socket->socket_num = groupsock__setup_datagram_socket(new_port);
	if (socket->socket_num == NULL) {
		uv_read_stop((uv_stream_t *)old_socket_num);
		return false;
	}

	groupsock__set_receive_buffer_to(socket_fd, old_receive_buffer_size);
	groupsock__set_send_buffer_to(socket_fd, old_send_buffer_size);
	if (socket->socket_num != old_socket_num) {
		/* the socket number has changed, so move any event handling for it: */
		/* TODO: moveSocketHandling */
	}
	return true;
}


/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * SocketLookupTable (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_socket_lookup_table_t *__groupsock_socket_lookup_table__alloc(void)
{
	groupsock_socket_lookup_table_t *socket_lookup_table;
	socket_lookup_table = (groupsock_socket_lookup_table_t *)calloc(1, sizeof(groupsock_socket_lookup_table_t));
	if (socket_lookup_table == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_socket_lookup_table_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return socket_lookup_table;
}
void __groupsock_socket_lookup_table__init(groupsock_socket_lookup_table_t *socket_lookup_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	socket_lookup_table->table = livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	
	socket_lookup_table->delete = groupsock_socket_lookup_table__delete__impl;
}
void __groupsock_socket_lookup_table__deinit(groupsock_socket_lookup_table_t *socket_lookup_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia__delete__hash_table(socket_lookup_table->table);
}
void __groupsock_socket_lookup_table__free(groupsock_socket_lookup_table_t *socket_lookup_table)
{
	if (socket_lookup_table) {
		free(socket_lookup_table);
		socket_lookup_table = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_socket_lookup_table_t *groupsock__new__socket_lookup_table(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_socket_lookup_table_t *socket_lookup_table;

	socket_lookup_table = __groupsock_socket_lookup_table__alloc();
	if (socket_lookup_table)
		__groupsock_socket_lookup_table__init(socket_lookup_table);

	return socket_lookup_table;
}
void groupsock__delete__socket_lookup_table(groupsock_socket_lookup_table_t *socket_lookup_table)
{
	if (socket_lookup_table && socket_lookup_table->delete)
		socket_lookup_table->delete(socket_lookup_table);
}
/*
 * Functions for virtual functions
 */
groupsock_socket_t *groupsock_socket_lookup_table__create_new(
		groupsock_socket_lookup_table_t *socket_lookup_table, groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (socket_lookup_table && socket_lookup_table->create_new)
		socket_lookup_table->create_new(socket_lookup_table, port);
}

/*
 * Override functions
 */
void groupsock_socket_lookup_table__delete__impl(groupsock_socket_lookup_table_t *socket_lookup_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (socket_lookup_table) {
		__groupsock_socket_lookup_table__deinit(socket_lookup_table);
		__groupsock_socket_lookup_table__free(socket_lookup_table);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_socket_t *groupsock_socket_lookup_table__fetch(
		groupsock_socket_lookup_table_t *socket_lookup_table, groupsock_port_t port,
		bool *is_new)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_socket_t *sock;

	*is_new = false;
	do {
		sock = (groupsock_socket_t *)livemedia_hash_table__lookup(
				socket_lookup_table->table, (char *)(long)(groupsock_port__num(&port)));
		if (sock == NULL) { /* we need to create one */
			sock = groupsock_socket_lookup_table__create_new(socket_lookup_table, port);
			if (sock == NULL || groupsock_socket__socket_num(sock) == NULL)
				break;

			livemedia_hash_table__add(socket_lookup_table->table,
					(char *)(long)(groupsock_port__num(&port)), (void *)sock);
			*is_new = true;
		}

		return sock;
	} while (0);

	groupsock__delete__net_interface((groupsock_net_interface_t *)sock);
	return NULL;
}

bool groupsock_socket_lookup_table__remove(
		groupsock_socket_lookup_table_t *socket_lookup_table,
		groupsock_socket_t *sock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_port_t port;

	port = groupsock_socket__port(sock);
	
	return livemedia_hash_table__remove(socket_lookup_table->table,
			(char *)(long)(groupsock_port__num(&port)));
}


/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * NetInterfaceTrafficStats (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_interface_traffic_stats_t *__groupsock_net_interface_traffic_stats__alloc(void)
{
	groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats;
	net_interface_traffic_stats = (groupsock_net_interface_traffic_stats_t *)calloc(1, sizeof(groupsock_net_interface_traffic_stats_t));
	if (net_interface_traffic_stats == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_net_interface_traffic_stats_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return net_interface_traffic_stats;
}
void __groupsock_net_interface_traffic_stats__init(groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	net_interface_traffic_stats->tot_num_packets =
		net_interface_traffic_stats->tot_num_bytes = 0.0;
	
	net_interface_traffic_stats->delete = groupsock_net_interface_traffic_stats__delete__impl;
}
void __groupsock_net_interface_traffic_stats__deinit(groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void __groupsock_net_interface_traffic_stats__free(groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
	if (net_interface_traffic_stats) {
		free(net_interface_traffic_stats);
		net_interface_traffic_stats = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_net_interface_traffic_stats_t *groupsock__new__net_interface_traffic_stats(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats;

	net_interface_traffic_stats = __groupsock_net_interface_traffic_stats__alloc();
	if (net_interface_traffic_stats)
		__groupsock_net_interface_traffic_stats__init(net_interface_traffic_stats);

	return net_interface_traffic_stats;
}
void groupsock__delete__net_interface_traffic_stats(groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
	if (net_interface_traffic_stats && net_interface_traffic_stats->delete)
		net_interface_traffic_stats->delete(net_interface_traffic_stats);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_interface_traffic_stats__delete__impl(groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_interface_traffic_stats) {
		__groupsock_net_interface_traffic_stats__deinit(net_interface_traffic_stats);
		__groupsock_net_interface_traffic_stats__free(net_interface_traffic_stats);
	}
}

/*
 * Functions in header file
 */
float groupsock_net_interface_traffic_stats__tot_num_packets(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return net_interface_traffic_stats->tot_num_packets;
}

float groupsock_net_interface_traffic_stats__tot_num_bytes(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return net_interface_traffic_stats->tot_num_bytes;
}

/*
 * Normal functions
 */
void groupsock_net_interface_traffic_stats__count_packet(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats,
		unsigned int packet_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	net_interface_traffic_stats->tot_num_packets += 1.0;
	net_interface_traffic_stats->tot_num_bytes += packet_size;
}

bool groupsock_net_interface_traffic_stats__have_seen_traffic(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return net_interface_traffic_stats->tot_num_packets != 0.0;
}

/*
 * Static functions implementation for ancestor
 */
