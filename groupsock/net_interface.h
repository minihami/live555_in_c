#ifndef __GROUPSOCK_NET_INTERFACE_H__
#define __GROUPSOCK_NET_INTERFACE_H__

#ifndef __GROUPSOCK_NET_ADDRESS_H__
#include "net_address.h"
#endif
#include "uv.h"


/*****************************************************************
 * NetInterface (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_net_interface_t {
	void (*delete)(struct _groupsock_net_interface_t *net_interface);
} groupsock_net_interface_t;

/*****************************************************************
 * DirectedNetInterface:NetInterface (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_directed_net_interface_t {
	groupsock_net_interface_t net_interface;

	bool (*write)(struct _groupsock_directed_net_interface_t *directed_net_interface,
			unsigned char *data, unsigned int num_bytes);
	bool (*source_addr_ok_for_relaying)(struct _groupsock_directed_net_interface_t *directed_net_interface,
			unsigned int addr);
} groupsock_directed_net_interface_t;

/*****************************************************************
 * DirectedNetInterfaceSet (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_directed_net_interface_set_t {
	livemedia_hash_table_t *table;

	void (*delete)(struct _groupsock_directed_net_interface_set_t *directed_net_interface_set);
} groupsock_directed_net_interface_set_t;

/*****************************************************************
 * DirectedNetInterfaceSet::Iterator (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_directed_net_interface_set_iterator_t {
	livemedia_hash_table_iterator_t *iter;

	void (*delete)(
			struct _groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator);
} groupsock_directed_net_interface_set_iterator_t;

/*****************************************************************
 * Socket:NetInterface (Structure Declarations)
 ****************************************************************/

typedef struct _groupsock_socket_t {
	groupsock_net_interface_t net_interface;

	int *debug_level;
	uv_udp_t *socket_num;
	groupsock_port_t port;

	bool (*handle_read)(struct _groupsock_socket_t *socket, unsigned char *buffer,
			unsigned int buffer_max_size, unsigned int *bytes_read,
			struct sockaddr_in *from_address);
		/* returns false on error; resultData == NULL if data ignored */

} groupsock_socket_t;

/*****************************************************************
 * SocketLookupTable (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_socket_lookup_table_t {
	livemedia_hash_table_t *table;

	groupsock_socket_t (*create_new)(
			struct _groupsock_socket_lookup_table_t *socket_lookup_table,
			groupsock_port_t port);
	
	void (*delete)(struct _groupsock_socket_lookup_table_t *socket_lookup_table);
} groupsock_socket_lookup_table_t;

/*****************************************************************
 * NetInterfaceTrafficStats (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_net_interface_traffic_stats_t {
	float tot_num_packets;
	float tot_num_bytes;

	void (*delete)(struct _groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);
} groupsock_net_interface_traffic_stats_t;

/*****************************************************************
 * NetInterface (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_interface_t *__groupsock_net_interface__alloc(void);
void __groupsock_net_interface__init(groupsock_net_interface_t *net_interface);
void __groupsock_net_interface__deinit(groupsock_net_interface_t *net_interface);
void __groupsock_net_interface__free(groupsock_net_interface_t *net_interface);
/*
 * new() and delete() functions
 */
groupsock_net_interface_t *groupsock__new__net_interface(void);
void groupsock__delete__net_interface(groupsock_net_interface_t *net_interface);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_interface__delete__impl(groupsock_net_interface_t *net_interface);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * DirectedNetInterface:NetInterface (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_directed_net_interface_t *__groupsock_directed_net_interface__alloc(void);
void __groupsock_directed_net_interface__init(
		groupsock_directed_net_interface_t *directed_net_interface);
void __groupsock_directed_net_interface__deinit(
		groupsock_directed_net_interface_t *directed_net_interface);
void __groupsock_directed_net_interface__free(
		groupsock_directed_net_interface_t *directed_net_interface);
/*
 * new() and delete() functions
 */
groupsock_directed_net_interface_t *groupsock__new__directed_net_interface(void);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_directed_net_interface__delete__impl(groupsock_net_interface_t *net_interface);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * DirectedNetInterfaceSet (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_directed_net_interface_set_t *__groupsock_directed_net_interface_set__alloc(void);
void __groupsock_directed_net_interface_set__init(
		groupsock_directed_net_interface_set_t *directed_net_interface_set);
void __groupsock_directed_net_interface_set__deinit(
		groupsock_directed_net_interface_set_t *directed_net_interface_set);
void __groupsock_directed_net_interface_set__free(
		groupsock_directed_net_interface_set_t *directed_net_interface_set);
/*
 * new() and delete() functions
 */
groupsock_directed_net_interface_set_t *groupsock__new__directed_net_interface_set(void);
void groupsock__delete__directed_net_interface_set(
		groupsock_directed_net_interface_set_t *directed_net_interface_set);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_directed_net_interface_set__delete__impl(
		groupsock_directed_net_interface_set_t *directed_net_interface_set);

/*
 * Functions in header file
 */
bool groupsock_directed_net_interface_set__is_empty(
		groupsock_directed_net_interface_set_t *directed_net_interface_set);

/*
 * Normal functions
 */
groupsock_directed_net_interface_t *groupsock_directed_net_interface_set__add(
		groupsock_directed_net_interface_set_t *directed_net_interface_set,
		groupsock_directed_net_interface_t const *interf);
bool groupsock_directed_net_interface_set__remove(
		groupsock_directed_net_interface_set_t *directed_net_interface_set,
		groupsock_directed_net_interface_t const *interf);

/*****************************************************************
 * DirectedNetInterfaceSet::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_directed_net_interface_set_iterator_t *__groupsock_directed_net_interface_set_iterator__alloc(void);
void __groupsock_directed_net_interface_set_iterator__init(
		groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator,
		groupsock_directed_net_interface_set_t *interfaces);
void __groupsock_directed_net_interface_set_iterator__deinit(
		groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator);
void __groupsock_directed_net_interface_set_iterator__free(
		groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator);
/*
 * new() and delete() functions
 */
groupsock_directed_net_interface_set_iterator_t *groupsock__new__directed_net_interface_set_iterator(
		groupsock_directed_net_interface_set_t *interfaces);
void groupsock__delete__directed_net_interface_set_iterator(groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_directed_net_interface_set_iterator__delete__impl(groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_directed_net_interface_t *groupsock_directed_net_interface_set_iterator__next(
		groupsock_directed_net_interface_set_iterator_t *directed_net_interface_set_iterator);

/*****************************************************************
 * Socket:NetInterface (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_socket_t *__groupsock_socket__alloc(void);
void __groupsock_socket__init(groupsock_socket_t *socket, groupsock_port_t port);
void __groupsock_socket__deinit(groupsock_socket_t *socket);
void __groupsock_socket__free(groupsock_socket_t *socket);
/*
 * new() and delete() functions
 */
groupsock_socket_t *groupsock__new__socket(groupsock_port_t port);
/*
 * Functions for virtual functions
 */
bool groupsock_socket__handle_read(groupsock_socket_t *socket, unsigned char *buffer,
		unsigned int buffer_max_size, unsigned *bytes_read, struct sockaddr_in *from_address);
      // Returns False on error; resultData == NULL if data ignored

/*
 * Override functions
 */
void groupsock_socket__delete__impl(groupsock_net_interface_t *net_interface);

/*
 * Functions in header file
 */
uv_udp_t *groupsock_socket__socket_num(groupsock_socket_t *socket);
groupsock_port_t groupsock_socket__port(groupsock_socket_t *socket);

/*
 * Normal functions
 */
void groupsock_socket__reset(groupsock_socket_t *socket);
/* closes the socket, and sets "fSocketNum" to -1 */
bool groupsock_socket__change_port(groupsock_socket_t *socket, groupsock_port_t new_port);
/* will also cause socketNum() to change */


/*****************************************************************
 * SocketLookupTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_socket_lookup_table_t *__groupsock_socket_lookup_table__alloc(void);
void __groupsock_socket_lookup_table__init(
		groupsock_socket_lookup_table_t *socket_lookup_table);
void __groupsock_socket_lookup_table__deinit(
		groupsock_socket_lookup_table_t *socket_lookup_table);
void __groupsock_socket_lookup_table__free(
		groupsock_socket_lookup_table_t *socket_lookup_table);
/*
 * new() and delete() functions
 */
groupsock_socket_lookup_table_t *groupsock__new__socket_lookup_table(void);
void groupsock__delete__socket_lookup_table(
		groupsock_socket_lookup_table_t *socket_lookup_table);
/*
 * Functions for virtual functions
 */
groupsock_socket_t *groupsock_socket_lookup_table__create_new(
		groupsock_socket_lookup_table_t *socket_lookup_table, groupsock_port_t port);

/*
 * Override functions
 */
void groupsock_socket_lookup_table__delete__impl(groupsock_socket_lookup_table_t *socket_lookup_table);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_socket_t *groupsock_socket_lookup_table__fetch(
		groupsock_socket_lookup_table_t *socket_lookup_table, groupsock_port_t port,
		bool *is_new);
/* Creates a new Socket if none already exists */
bool groupsock_socket_lookup_table__remove(
		groupsock_socket_lookup_table_t *socket_lookup_table,
		groupsock_socket_t *sock);

/*****************************************************************
 * NetInterfaceTrafficStats (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_interface_traffic_stats_t *__groupsock_net_interface_traffic_stats__alloc(void);
void __groupsock_net_interface_traffic_stats__init(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);
void __groupsock_net_interface_traffic_stats__deinit(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);
void __groupsock_net_interface_traffic_stats__free(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);
/*
 * new() and delete() functions
 */
groupsock_net_interface_traffic_stats_t *groupsock__new__net_interface_traffic_stats(void);
void groupsock__delete__net_interface_traffic_stats(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_interface_traffic_stats__delete__impl(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);

/*
 * Functions in header file
 */
float groupsock_net_interface_traffic_stats__tot_num_packets(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);
float groupsock_net_interface_traffic_stats__tot_num_bytes(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);

/*
 * Normal functions
 */
void groupsock_net_interface_traffic_stats__count_packet(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats,
		unsigned int packet_size);
bool groupsock_net_interface_traffic_stats__have_seen_traffic(
		groupsock_net_interface_traffic_stats_t *net_interface_traffic_stats);

#endif
