#ifndef __GROUPSOCK_GROUPSOCK_H__
#define __GROUPSOCK_GROUPSOCK_H__

#if 0
#ifndef __GROUPSOCK_GROUPSOCK_VERSION_H__
#include "groupsock_version.h"
#endif
#endif

#ifndef __GROUPSOCK_NET_INTERFACE_H__
#include "net_interface.h"
#endif

#ifndef __GROUPSOCK_GROUPEID_H__
#include "group_eid.h"
#endif

/* An "OutputSocket" is (by default) used only to send packets.
 * No packets are received on it (unless a suboutput_socket arranges this) */

/*****************************************************************
 * OutputSocket:Socket (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_output_socket_t {
	groupsock_socket_t socket;

	groupsock_port_t source_port;
	unsigned int last_sent_ttl;

	bool (*write)(struct _groupsock_output_socket_t *output_socket, 
			net_address_bits address, port_num_bits port_num/* in network_order */,
			u_int8_t ttl, unsigned char *buffer, unsigned int buffer_size);
} groupsock_output_socket_t;

/*****************************************************************
 * destRecord (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_dest_record_t {
	struct _groupsock_dest_record_t *next;
	groupsock_group_eid_t group_eid;
	unsigned int session_id;

	void (*delete)(struct _groupsock_dest_record_t *dest_record);
} groupsock_dest_record_t;

/*****************************************************************
 * Groupsock:OutputSocket (Structure Declarations)
 ****************************************************************/
static groupsock_net_interface_traffic_stats_t groupsock_stats_incoming;
static groupsock_net_interface_traffic_stats_t groupsock_stats_outgoing;
static groupsock_net_interface_traffic_stats_t groupsock_stats_relayed_incoming;
static groupsock_net_interface_traffic_stats_t groupsock_stats_relayed_outgoing;

typedef struct _groupsock_groupsock_t {
	groupsock_output_socket_t output_socket;

	bool delete_if_no_members;
	bool is_slave; /* for tunneling */
	groupsock_net_interface_traffic_stats_t *stats_incoming;
	groupsock_net_interface_traffic_stats_t *stats_outgoing;
	groupsock_net_interface_traffic_stats_t *stats_relayed_incoming;
	groupsock_net_interface_traffic_stats_t *stats_relayed_outgoing;
	groupsock_net_interface_traffic_stats_t stats_group_incoming;
	groupsock_net_interface_traffic_stats_t stats_group_outgoing;
	groupsock_net_interface_traffic_stats_t stats_group_relayed_incoming;
	groupsock_net_interface_traffic_stats_t stats_group_relayed_outgoing;

	groupsock_dest_record_t *dests;
	groupsock_group_eid_t incoming_group_eid;
	groupsock_directed_net_interface_set_t members;

	groupsock_dest_record_t *(*create_new_dest_record)(
			struct _groupsock_groupsock_t *groupsock, struct in_addr const *addr,
			groupsock_port_t const *port, u_int8_t ttl, unsigned int session_id,
			groupsock_dest_record_t *next);
	void (*add_destination)(struct _groupsock_groupsock_t *groupsock,
			struct in_addr const *addr, groupsock_port_t const *port,
			unsigned int session_id);
	void (*remove_destination)(struct _groupsock_groupsock_t *groupsock,
			unsigned int session_id);
	bool (*output)(struct _groupsock_groupsock_t *groupsock, unsigned char *buffer,
			unsigned int buffer_size,
			groupsock_directed_net_interface_t *interface_not_to_fwd_back_to);
} groupsock_groupsock_t;

/*****************************************************************
 * GroupsockLookupTable (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_groupsock_lookup_table_t {
	groupsock_address_port_lookup_table_t table;

	void (*delete)(struct _groupsock_groupsock_lookup_table_t * groupsock_lookup_table);
} groupsock_groupsock_lookup_table_t;

/*****************************************************************
 * GroupsockLookupTable::Iterator (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_groupsock_lookup_table_iterator_t {
	groupsock_address_port_lookup_table_iterator_t iter;

	void (*delete)(
			struct _groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator);
} groupsock_groupsock_lookup_table_iterator_t;

/*****************************************************************
 * OutputSocket:Socket (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_output_socket_t *__groupsock_output_socket__alloc(void);
void __groupsock_output_socket__init(groupsock_output_socket_t *output_socket);
void __groupsock_output_socket__init_by_port(groupsock_output_socket_t *output_socket,
		groupsock_port_t port);
void __groupsock_output_socket__deinit(groupsock_output_socket_t *output_socket);
void __groupsock_output_socket__free(groupsock_output_socket_t *output_socket);
/*
 * new() and delete() functions
 */
groupsock_output_socket_t *groupsock__new__output_socket(void);
groupsock_output_socket_t *groupsock__new__output_socket_by_port(groupsock_port_t port);
void groupsock__delete__output_socket(groupsock_output_socket_t *output_socket);
/*
 * Functions for virtual functions
 */
bool groupsock_output_socket__write(groupsock_output_socket_t *output_socket,
		net_address_bits address, port_num_bits port_num/* in network order */,
		u_int8_t ttl, unsigned char *buffer, unsigned int buffer_size);
/*
 * Override functions
 */
bool groupsock_output_socket__write__impl(groupsock_output_socket_t *output_socket,
		net_address_bits address, port_num_bits port_num/* in network order */,
		u_int8_t ttl, unsigned char *buffer, unsigned int buffer_size);
bool groupsock_output_socket__handle_read__impl(groupsock_socket_t *socket,
		unsigned char *buffer, unsigned int buffer_max_size, unsigned int *bytes_read,
		struct sockaddr_in *from_address_and_port);
void groupsock_output_socket__delete__impl(groupsock_net_interface_t *net_interface);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * destRecord (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_dest_record_t *__groupsock_dest_record__alloc(void);
void __groupsock_dest_record__init(groupsock_dest_record_t *dest_record,
		struct in_addr *addr, groupsock_port_t *port, u_int8_t ttl,
		unsigned int session_id, groupsock_dest_record_t *next);
void __groupsock_dest_record__deinit(groupsock_dest_record_t *dest_record);
void __groupsock_dest_record__free(groupsock_dest_record_t *dest_record);
/*
 * new() and delete() functions
 */
groupsock_dest_record_t *groupsock__new__dest_record(
		struct in_addr *addr, groupsock_port_t *port, u_int8_t ttl,
		unsigned int session_id, groupsock_dest_record_t *next);
void groupsock__delete__dest_record(groupsock_dest_record_t *dest_record);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_dest_record__delete__impl(groupsock_dest_record_t *dest_record);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * Groupsock:OutputSocket (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_groupsock_t *__groupsock_groupsock__alloc(void);
void __groupsock_groupsock__init_by_port_and_ttl(groupsock_groupsock_t *groupsock,
		struct in_addr *group_addr, groupsock_port_t port, u_int8_t ttl);
void __groupsock_groupsock__init_by_source_filter_addr_and_port(
		groupsock_groupsock_t *groupsock, struct in_addr *group_addr,
		struct in_addr *source_filter_addr, groupsock_port_t port);
void __groupsock_groupsock__deinit(groupsock_groupsock_t *groupsock);
void __groupsock_groupsock__free(groupsock_groupsock_t *groupsock);
/*
 * new() and delete() functions
 */
groupsock_groupsock_t *groupsock__new__groupsock_by_port_and_ttl(
		struct  in_addr *group_addr, groupsock_port_t port, u_int8_t ttl);
/* used for a 'source-independent multicast' group */
groupsock_groupsock_t *groupsock__new__groupsock_by_source_filter_addr_and_port(
		struct in_addr *group_addr, struct in_addr *source_filter_addr,
		groupsock_port_t port);
/* used for a 'source-specific multicast' group */

/*
 * Functions for virtual functions
 */
groupsock_dest_record_t *groupsock_groupsock__create_new_dest_record(
		groupsock_groupsock_t *groupsock, struct in_addr const *addr,
		groupsock_port_t const *port, u_int8_t ttl, unsigned int session_id,
		groupsock_dest_record_t *next);
/* Can be redefined by subgroupsock_lookup_table_iteratores that also subcalss "destRecord" */
void groupsock_groupsock__add_destination(groupsock_groupsock_t *groupsock,
		struct in_addr const *addr, groupsock_port_t const *port, unsigned int session_id);
void groupsock_groupsock__remove_destination(groupsock_groupsock_t *groupsock,
		unsigned int session_id);
bool groupsock_groupsock__output(groupsock_groupsock_t *groupsock,
		unsigned char *buffer, unsigned int buffer_size,
		groupsock_directed_net_interface_t *interface_not_to_fwd_back_to);

/*
 * Override functions
 */
groupsock_dest_record_t *groupsock_groupsock__create_new_dest_record__impl(
		groupsock_groupsock_t *groupsock, struct in_addr const *addr,
		groupsock_port_t const *port, u_int8_t ttl, unsigned int session_id,
		groupsock_dest_record_t *next);
void groupsock_groupsock__add_destination__impl(groupsock_groupsock_t *groupsock,
		struct in_addr const *addr, groupsock_port_t const *port, unsigned int session_id);
void groupsock_groupsock__remove_destination__impl(groupsock_groupsock_t *groupsock,
		unsigned int session_id);
bool groupsock_groupsock__output__impl(groupsock_groupsock_t *groupsock,
		unsigned char *buffer, unsigned int buffer_size,
		groupsock_directed_net_interface_t *interface_not_to_fwd_back_to);
bool groupsock_groupsock__handle_read__impl(groupsock_socket_t *socket,
		unsigned char *buffer, unsigned int buffer_max_size, unsigned *bytes_read,
		struct sockaddr_in *from_address_and_port);
void groupsock_groupsock__delete__impl(groupsock_net_interface_t *net_interface);

/*
 * Functions in header file
 */
bool groupsock_groupsock__has_multiple_destinations(groupsock_groupsock_t *groupsock);
struct in_addr const *groupsock_groupsock__group_address(groupsock_groupsock_t *groupsock);
struct in_addr const *groupsock_groupsock__source_filter_address(
		groupsock_groupsock_t *groupsock);
bool groupsock_groupsock__is_ssm(groupsock_groupsock_t *groupsock);
u_int8_t groupsock_groupsock__ttl(groupsock_groupsock_t *groupsock);
groupsock_directed_net_interface_set_t *groupsock_groupsock__members(
		groupsock_groupsock_t *groupsock);

/*
 * Normal functions
 */
void groupsock_groupsock__change_destination_parameters(
		struct in_addr const *new_dest_addr, groupsock_port_t new_dest_port,
		int new_dest_ttl, unsigned int session_id);
/* By default, the destination address, port and ttl for
 * outgoing packets are those that were specified in
 * the constructor.  This works OK for multicast sockets,
 * but for unicast we usually want the destination port
 * number, at least, to be different from the source port.
 * (If a parameter is 0 (or ~0 for ttl), then no change is made to that parameter.)
 * (If no existing "destRecord" exists with this "sessionId", then we add a new "destRecord".) */
unsigned int groupsock_groupsock__lookup_session_id_from_destination(
		groupsock_groupsock_t *groupsock, struct sockaddr_in const *dest_addr_and_port);
/* returns 0 if not found */

void groupsock_groupsock__remove_all_destinations(groupsock_groupsock_t *groupsock);
void groupsock_groupsock__multicast_send_only(groupsock_groupsock_t *groupsock);
bool groupsock_groupsock__was_looped_back_from_us(groupsock_groupsock_t *groupsock,
		struct sockaddr_in *from_address_and_port);
groupsock_dest_record_t *groupsock_groupsock__lookup_dest_record_from_destination(
		groupsock_groupsock_t *groupsock, struct sockaddr_in const *dest_addr_and_port);
void groupsock_groupsock__remove_destination_from(groupsock_groupsock_t *groupsock,
		groupsock_dest_record_t **dests, unsigned int session_id);
/* used to implement (the public) "removeDestination()", and "changeDestinationParameters()" */
int groupsock_groupsock__output_to_all_members_except(groupsock_groupsock_t *groupsock,
		groupsock_directed_net_interface_t *except_interface,
		u_int8_t ttl_to_fwd, unsigned char *data, unsigned int size,
		net_address_bits source_addr);

/*****************************************************************
 * GroupsockLookupTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_groupsock_lookup_table_t *__groupsock_groupsock_lookup_table__alloc(void);
void __groupsock_groupsock_lookup_table__init(groupsock_groupsock_lookup_table_t *groupsock_lookup_table);
void __groupsock_groupsock_lookup_table__deinit(groupsock_groupsock_lookup_table_t *groupsock_lookup_table);
void __groupsock_groupsock_lookup_table__free(groupsock_groupsock_lookup_table_t *groupsock_lookup_table);
/*
 * new() and delete() functions
 */
groupsock_groupsock_lookup_table_t *groupsock__new__groupsock_lookup_table(void);
void groupsock__delete__groupsock_lookup_table(groupsock_groupsock_lookup_table_t *groupsock_lookup_table);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_groupsock_lookup_table__delete__impl(groupsock_groupsock_lookup_table_t *groupsock_lookup_table);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_groupsock_t *groupsock_groupsock_lookup_table__fetch_by_ttl(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, groupsock_port_t port, u_int8_t ttl, bool *is_new);
/* Creates a new Groupsock if none already exists */
groupsock_groupsock_t *groupsock_groupsock_lookup_table__fetch_by_source_filter_addr(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, net_address_bits source_filter_addr,
		groupsock_port_t port, bool *is_new);
/* Creates a new Groupsock if none already exists */
groupsock_groupsock_t *groupsock_groupsock_lookup_table__lookup(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, groupsock_port_t port);
/* Returns NULL if none already exists */
groupsock_groupsock_t *groupsock_groupsock_lookup_table__lookup_by_source_filter_addr(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, net_address_bits source_filter_addr,
		groupsock_port_t port);
/* Returns NULL if none already exists */
groupsock_groupsock_t *groupsock_groupsock_lookup_table__lookup_by_sock(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table, int sock);
/* Returns NULL if none already exists */
bool groupsock_groupsock_lookup_table__remove(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		groupsock_groupsock_t const *groupsock);
groupsock_groupsock_t *groupsock_groupsock_lookup_table__add_new(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, net_address_bits source_filter_address,
		groupsock_port_t port, u_int8_t ttl);
		
/*****************************************************************
 * GroupsockLookupTable::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_groupsock_lookup_table_iterator_t *__groupsock_groupsock_lookup_table_iterator__alloc(void);
void __groupsock_groupsock_lookup_table_iterator__init(
		groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator,
		groupsock_groupsock_lookup_table_t *groupsocks);
void __groupsock_groupsock_lookup_table_iterator__deinit(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator);
void __groupsock_groupsock_lookup_table_iterator__free(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator);
/*
 * new() and delete() functions
 */
groupsock_groupsock_lookup_table_iterator_t *groupsock__new__groupsock_lookup_table_iterator(
		groupsock_groupsock_lookup_table_t *groupsocks);
void groupsock__delete__groupsock_lookup_table_iterator(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_groupsock_lookup_table_iterator__delete__impl(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_groupsock_t *groupsock_groupsock_lookup_table_iterator__next(
		groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator);

#endif
