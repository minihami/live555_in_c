#include "groupsock.h"
#include "groupsock_helper.h"

#if 0
TODO: 
#include "tunnel_encaps.h"
#endif

#include <stdio.h>

/*****************************************************************
 * OutputSocket:Socket (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_output_socket_t *__groupsock_output_socket__alloc(void)
{
	groupsock_output_socket_t *output_socket;
	output_socket = (groupsock_output_socket_t *)calloc(1, sizeof(groupsock_output_socket_t));
	if (output_socket == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_output_socket_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return output_socket;
}
void __groupsock_output_socket__init(groupsock_output_socket_t *output_socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_port_t *port;

	port = groupsock__new__port(0);
	__groupsock_socket__init(&output_socket->socket, *port);

	output_socket->source_port = *port;
	groupsock__delete__port(port);
	output_socket->last_sent_ttl = 256; /* hack: a deliberately invalid value */
	
	output_socket->write = groupsock_output_socket__write__impl;
	((groupsock_socket_t *)output_socket)->handle_read = groupsock_output_socket__handle_read__impl;
	((groupsock_net_interface_t *)output_socket)->delete = groupsock_output_socket__delete__impl;

}
void __groupsock_output_socket__init_by_port(groupsock_output_socket_t *output_socket,
		groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_port_t *source_port;
	source_port = groupsock__new__port(0);

	__groupsock_socket__init(&output_socket->socket, port);

	output_socket->source_port = *source_port;
	groupsock__delete__port(source_port);
	output_socket->last_sent_ttl = 256;	/*hack: a deliberately invalid value*/

	output_socket->write = groupsock_output_socket__write__impl;
	((groupsock_socket_t *)output_socket)->handle_read = groupsock_output_socket__handle_read__impl;
	((groupsock_net_interface_t *)output_socket)->delete = groupsock_output_socket__delete__impl;

}
void __groupsock_output_socket__deinit(groupsock_output_socket_t *output_socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* parent deinit */
	__groupsock_socket__deinit(&output_socket->socket);
}
void __groupsock_output_socket__free(groupsock_output_socket_t *output_socket)
{
	if (output_socket) {
		free(output_socket);
		output_socket = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_output_socket_t *groupsock__new__output_socket(void)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_output_socket_t *output_socket;

	output_socket = __groupsock_output_socket__alloc();
	if (output_socket)
		__groupsock_output_socket__init(output_socket);

	return output_socket;
}
groupsock_output_socket_t *groupsock__new__output_socket_by_port(groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_output_socket_t *output_socket;

	output_socket = __groupsock_output_socket__alloc();
	if (output_socket)
		__groupsock_output_socket__init_by_port(output_socket, port);

	return output_socket;
}
/*
 * Functions for virtual functions
 */
bool groupsock_output_socket__write(groupsock_output_socket_t *output_socket,
		net_address_bits address, port_num_bits port_num,
		u_int8_t ttl, unsigned char *buffer, unsigned int buffer_size)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Override functions
 */
bool groupsock_output_socket__write__impl(groupsock_output_socket_t *output_socket,
		net_address_bits address, port_num_bits port_num/* in network order */,
		u_int8_t ttl, unsigned char *buffer, unsigned int buffer_size)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool groupsock_output_socket__handle_read__impl(groupsock_socket_t *socket,
		unsigned char *buffer, unsigned int buffer_max_size, unsigned int *bytes_read,
		struct sockaddr_in *from_address_and_port)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_output_socket__delete__impl(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_output_socket_t *output_socket;

	output_socket = (groupsock_output_socket_t *)net_interface;
	if (output_socket) {
		__groupsock_output_socket__deinit(output_socket);
		__groupsock_output_socket__free(output_socket);
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
 * destRecord (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_dest_record_t *__groupsock_dest_record__alloc(void)
{
	groupsock_dest_record_t *dest_record;
	dest_record = (groupsock_dest_record_t *)calloc(1, sizeof(groupsock_dest_record_t));
	if (dest_record == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_dest_record_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return dest_record;
}
void __groupsock_dest_record__init(groupsock_dest_record_t *dest_record,
		struct in_addr *addr, groupsock_port_t *port, u_int8_t ttl,
		unsigned int session_id, groupsock_dest_record_t *next)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_group_eid_t *tmp_group_eid;
	dest_record->next = next;
	tmp_group_eid = (groupsock__new__group_eid_by_ttl(addr, groupsock_port__num(port), ttl));
	dest_record->group_eid = *tmp_group_eid;
	groupsock__delete__group_eid(tmp_group_eid);
	dest_record->session_id = session_id;
	
	dest_record->delete = groupsock_dest_record__delete__impl;
}
void __groupsock_dest_record__deinit(groupsock_dest_record_t *dest_record)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (dest_record->next)
		groupsock__delete__dest_record(dest_record->next);
}
void __groupsock_dest_record__free(groupsock_dest_record_t *dest_record)
{
	if (dest_record) {
		free(dest_record);
		dest_record = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_dest_record_t *groupsock__new__dest_record(
		struct in_addr *addr, groupsock_port_t *port, u_int8_t ttl,
		unsigned int session_id, groupsock_dest_record_t *next)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_dest_record_t *dest_record;

	dest_record = __groupsock_dest_record__alloc();
	if (dest_record)
		__groupsock_dest_record__init(dest_record, addr, port, ttl, session_id, next);

	return dest_record;
}
void groupsock__delete__dest_record(groupsock_dest_record_t *dest_record)
{
	if (dest_record && dest_record->delete)
		dest_record->delete(dest_record);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_dest_record__delete__impl(groupsock_dest_record_t *dest_record)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (dest_record) {
		__groupsock_dest_record__deinit(dest_record);
		__groupsock_dest_record__free(dest_record);
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
 * Groupsock:OutputSocket (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_groupsock_t *__groupsock_groupsock__alloc(void)
{
	groupsock_groupsock_t *groupsock;
	groupsock = (groupsock_groupsock_t *)calloc(1, sizeof(groupsock_groupsock_t));
	if (groupsock == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_groupsock_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return groupsock;
}
void __groupsock_groupsock__init_by_port_and_ttl(groupsock_groupsock_t *groupsock,
		struct in_addr *group_addr, groupsock_port_t port, u_int8_t ttl)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	__groupsock_output_socket__init_by_port(&groupsock->output_socket, port);

	groupsock->delete_if_no_members = false;
	groupsock->is_slave = false;
	groupsock->dests = groupsock__new__dest_record(group_addr, &port, ttl, 0, NULL);
	groupsock->incoming_group_eid = *(groupsock__new__group_eid_by_ttl(group_addr,
			groupsock_port__num(&port), ttl));
	__groupsock_directed_net_interface_set__init(&groupsock->members);

	if (!groupsock__socket_join_group(groupsock_socket__socket_num(
					(groupsock_socket_t *)groupsock), (*group_addr).s_addr)) {
		if (*((groupsock_socket_t *)groupsock)->debug_level >= 1) {
			fprintf(stderr, "%p: failed to join group: in %s()\n", groupsock, __func__);
		}
	}

	/* Make sure we can get our source address: */
	if (groupsock__our_ip_address() == 0) {
		if (*((groupsock_socket_t *)groupsock)->debug_level >= 0) { /* this is a fatal error */
			fprintf(stderr, "Unable to determine our source address: in %s()\n", __func__);
		}
	}

	if (*((groupsock_socket_t *)groupsock)->debug_level >=2)
			fprintf(stderr, "%p: created in %s()\n", groupsock, __func__);

	
	groupsock->create_new_dest_record = groupsock_groupsock__create_new_dest_record__impl;
	groupsock->add_destination = groupsock_groupsock__add_destination__impl;
	groupsock->remove_destination = groupsock_groupsock__remove_destination__impl;
	groupsock->output = groupsock_groupsock__output__impl;
	((groupsock_socket_t *)groupsock)->handle_read = groupsock_groupsock__handle_read__impl;
	((groupsock_net_interface_t *)groupsock)->delete = groupsock_groupsock__delete__impl;

	groupsock->stats_incoming = &groupsock_stats_incoming;
	groupsock->stats_outgoing = &groupsock_stats_outgoing;
	groupsock->stats_relayed_incoming = &groupsock_stats_relayed_incoming;
	groupsock->stats_relayed_outgoing = &groupsock_stats_relayed_outgoing;
}
void __groupsock_groupsock__init_by_source_filter_addr_and_port(
		groupsock_groupsock_t *groupsock, struct in_addr *group_addr,
		struct in_addr *source_filter_addr, groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */

	/*TODO - members is not initialized
	  __groupsock_directed_net_interface_set__init(&groupsock->members);
	  */
	
	/* TODO: virtual functions configuration */ 

	/* TODO: static variable in class pointer */

	((groupsock_net_interface_t *)groupsock)->delete = groupsock_groupsock__delete__impl;
}
void __groupsock_groupsock__deinit(groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* destructor */
	if (groupsock_groupsock__is_ssm(groupsock)) {
		if (!groupsock__socket_leave_group_ssm(groupsock_socket__socket_num((groupsock_socket_t *)groupsock),
					groupsock_groupsock__group_address(groupsock)->s_addr, 
					groupsock_groupsock__source_filter_address(groupsock)->s_addr)) {
			groupsock__socket_leave_group(groupsock_socket__socket_num((groupsock_socket_t *)groupsock),
					groupsock_groupsock__group_address(groupsock)->s_addr);
		} 
	} else {
		groupsock__socket_leave_group(groupsock_socket__socket_num(
					(groupsock_socket_t *)groupsock),
				groupsock_groupsock__group_address(groupsock)->s_addr);
	}

	groupsock__delete__dest_record(groupsock->dests);
	if (*((groupsock_socket_t *)groupsock)->debug_level >=2)
			fprintf(stderr, "%p: deleting in %s()\n", groupsock, __func__);

	__groupsock_directed_net_interface_set__deinit(&groupsock->members);
	/* parent deinit */
	__groupsock_output_socket__deinit(&groupsock->output_socket);

}
void __groupsock_groupsock__free(groupsock_groupsock_t *groupsock)
{
	if (groupsock) {
		free(groupsock);
		groupsock = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_groupsock_t *groupsock__new__groupsock_by_port_and_ttl(
		struct  in_addr *group_addr, groupsock_port_t port, u_int8_t ttl)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_groupsock_t *groupsock;

	groupsock = __groupsock_groupsock__alloc();
	if (groupsock)
		__groupsock_groupsock__init_by_port_and_ttl(groupsock, group_addr, port, ttl);

	return groupsock;
}
groupsock_groupsock_t *groupsock__new__groupsock_by_source_filter_addr_and_port(
		struct in_addr *group_addr, struct in_addr *source_filter_addr,
		groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_groupsock_t *groupsock;

	groupsock = __groupsock_groupsock__alloc();
	if (groupsock)
		__groupsock_groupsock__init_by_source_filter_addr_and_port(groupsock, group_addr,
				source_filter_addr, port);

	return groupsock;
}
/*
 * Functions for virtual functions
 */
groupsock_dest_record_t *groupsock_groupsock__create_new_dest_record(
		groupsock_groupsock_t *groupsock, struct in_addr const *addr,
		groupsock_port_t const *port, u_int8_t ttl, unsigned int session_id,
		groupsock_dest_record_t *next)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_groupsock__add_destination(groupsock_groupsock_t *groupsock,
		struct in_addr const *addr, groupsock_port_t const *port, unsigned int session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_groupsock__remove_destination(groupsock_groupsock_t *groupsock,
		unsigned int session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool groupsock_groupsock__output(groupsock_groupsock_t *groupsock,
		unsigned char *buffer, unsigned int buffer_size,
		groupsock_directed_net_interface_t *interface_not_to_fwd_back_to /* default: NULL */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	bool write_success;
	groupsock_dest_record_t *dests;
	int num_members;
	struct in_addr *group_address;
	
	do {
		/* First, do the datagram send, to each destination: */
		write_success = true;
		for (dests = groupsock->dests; dests != NULL; dests = dests->next) {
			group_address = groupsock_group_eid__group_address(&dests->group_eid);
			if (!groupsock_output_socket__write((groupsock_output_socket_t *)groupsock,
						group_address->s_addr, groupsock_group_eid__port_num(&dests->group_eid),
						groupsock_group_eid__ttl(&dests->group_eid), buffer, buffer_size)) {
				write_success = false;
				break;
			}
		}
		if (!write_success)
			break;
		groupsock_net_interface_traffic_stats__count_packet(groupsock->stats_outgoing, buffer_size);
		groupsock_net_interface_traffic_stats__count_packet(&groupsock->stats_group_outgoing, buffer_size);

		/* Then, forward to our members: */
		num_members = 0;
		if (!groupsock_directed_net_interface_set__is_empty(groupsock_groupsock__members(groupsock))) {
			num_members = groupsock_groupsock__output_to_all_members_except(groupsock,
					interface_not_to_fwd_back_to, groupsock_groupsock__ttl(groupsock), buffer,
					buffer_size, groupsock__our_ip_address());
			if (num_members < 0)
				break;
		}

		if (*((groupsock_socket_t *)groupsock)->debug_level >= 3) {
			fprintf(stderr, "%p : wrote %d bytes, ttl %d", groupsock, buffer_size,
					(unsigned int)groupsock_groupsock__ttl(groupsock));
			if (num_members > 0) {
				fprintf(stderr, "; relayed to %d members", num_members);
			}
			fprintf(stderr, "\n");
		}
		return true;
	} while (0);

	if (((groupsock_socket_t *)groupsock)->debug_level >= 0) { /* this is a fatal error */
		fprintf(stderr, "Groupsock write failed in %s", __func__);
	}
	return false;
}


/*
 * Override functions
 */
groupsock_dest_record_t *groupsock_groupsock__create_new_dest_record__impl(
		groupsock_groupsock_t *groupsock, struct in_addr const *addr,
		groupsock_port_t const *port, u_int8_t ttl, unsigned int session_id,
		groupsock_dest_record_t *next)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_groupsock__add_destination__impl(groupsock_groupsock_t *groupsock,
		struct in_addr const *addr, groupsock_port_t const *port, unsigned int session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_groupsock__remove_destination__impl(groupsock_groupsock_t *groupsock,
		unsigned int session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool groupsock_groupsock__output__impl(groupsock_groupsock_t *groupsock,
		unsigned char *buffer, unsigned int buffer_size,
		groupsock_directed_net_interface_t *interface_not_to_fwd_back_to)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool groupsock_groupsock__handle_read__impl(groupsock_socket_t *socket,
		unsigned char *buffer, unsigned int buffer_max_size, unsigned *bytes_read,
		struct sockaddr_in *from_address_and_port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_groupsock__delete__impl(groupsock_net_interface_t *net_interface)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_groupsock_t *groupsock;

	groupsock = (groupsock_groupsock_t *)net_interface;

	if (groupsock) {
		__groupsock_groupsock__deinit(groupsock);
		__groupsock_groupsock__free(groupsock);
	}
}

/*
 * Functions in header file
 */
bool groupsock_groupsock__has_multiple_destinations(groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return groupsock->dests && groupsock->dests->next != NULL;
}
struct in_addr const *groupsock_groupsock__group_address(groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return groupsock_group_eid__group_address(&groupsock->incoming_group_eid);
}
struct in_addr const *groupsock_groupsock__source_filter_address(
		groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return groupsock_group_eid__source_filter_address(&groupsock->incoming_group_eid);
}
bool groupsock_groupsock__is_ssm(groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return groupsock_group_eid__is_ssm(&groupsock->incoming_group_eid);
}
u_int8_t groupsock_groupsock__ttl(groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
groupsock_directed_net_interface_set_t *groupsock_groupsock__members(
		groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &groupsock->members;

}

/*
 * Normal functions
 */
void groupsock_groupsock__change_destination_parameters(
		struct in_addr const *new_dest_addr, groupsock_port_t new_dest_port,
		int new_dest_ttl, unsigned int session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif


}

unsigned int groupsock_groupsock__lookup_session_id_from_destination(
		groupsock_groupsock_t *groupsock, struct sockaddr_in const *dest_addr_and_port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


void groupsock_groupsock__remove_all_destinations(groupsock_groupsock_t *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock__delete__dest_record(groupsock->dests);
	groupsock->dests = NULL;
}

void groupsock_groupsock__multicast_send_only(groupsock_groupsock_t *groupsock)
{

#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool groupsock_groupsock__was_looped_back_from_us(groupsock_groupsock_t *groupsock,
		struct sockaddr_in *from_address_and_port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

groupsock_dest_record_t *groupsock_groupsock__lookup_dest_record_from_destination(
		groupsock_groupsock_t *groupsock, struct sockaddr_in const *dest_addr_and_port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_groupsock__remove_destination_from(groupsock_groupsock_t *groupsock,
		groupsock_dest_record_t **dests, unsigned int session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

int groupsock_groupsock__output_to_all_members_except(groupsock_groupsock_t *groupsock,
		groupsock_directed_net_interface_t *except_interface,
		u_int8_t ttl_to_fwd, unsigned char *data, unsigned int size,
		net_address_bits source_addr)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * GroupsockLookupTable (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_groupsock_lookup_table_t *__groupsock_groupsock_lookup_table__alloc(void)
{
	groupsock_groupsock_lookup_table_t *groupsock_lookup_table;
	groupsock_lookup_table = (groupsock_groupsock_lookup_table_t *)calloc(1, sizeof(groupsock_groupsock_lookup_table_t));
	if (groupsock_lookup_table == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_groupsock_lookup_table_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return groupsock_lookup_table;
}
void __groupsock_groupsock_lookup_table__init(groupsock_groupsock_lookup_table_t *groupsock_lookup_table)
{
	/* TODO: parent init */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	groupsock_lookup_table->delete = groupsock_groupsock_lookup_table__delete__impl;
}
void __groupsock_groupsock_lookup_table__deinit(groupsock_groupsock_lookup_table_t *groupsock_lookup_table)
{
	/* TODO: destructor */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: parent deinit */
}
void __groupsock_groupsock_lookup_table__free(groupsock_groupsock_lookup_table_t *groupsock_lookup_table)
{
	if (groupsock_lookup_table) {
		free(groupsock_lookup_table);
		groupsock_lookup_table = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_groupsock_lookup_table_t *groupsock__new__groupsock_lookup_table(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_groupsock_lookup_table_t *groupsock_lookup_table;

	groupsock_lookup_table = __groupsock_groupsock_lookup_table__alloc();
	if (groupsock_lookup_table)
		__groupsock_groupsock_lookup_table__init(groupsock_lookup_table);

	return groupsock_lookup_table;
}
void groupsock__delete__groupsock_lookup_table(groupsock_groupsock_lookup_table_t *groupsock_lookup_table)
{
	if (groupsock_lookup_table && groupsock_lookup_table->delete)
		groupsock_lookup_table->delete(groupsock_lookup_table);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_groupsock_lookup_table__delete__impl(groupsock_groupsock_lookup_table_t *groupsock_lookup_table)
{
	/*TODO: if child, get parent as parameter and cast to child */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (groupsock_lookup_table) {
		__groupsock_groupsock_lookup_table__deinit(groupsock_lookup_table);
		__groupsock_groupsock_lookup_table__free(groupsock_lookup_table);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_groupsock_t *groupsock_groupsock_lookup_table__fetch_by_ttl(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, groupsock_port_t port, u_int8_t ttl, bool *is_new)
{

#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

groupsock_groupsock_t *groupsock_groupsock_lookup_table__fetch_by_source_filter_addr(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, net_address_bits source_filter_addr,
		groupsock_port_t port, bool *is_new)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

groupsock_groupsock_t *groupsock_groupsock_lookup_table__lookup(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

groupsock_groupsock_t *groupsock_groupsock_lookup_table__lookup_by_source_filter_addr(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, net_address_bits source_filter_addr,
		groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

groupsock_groupsock_t *groupsock_groupsock_lookup_table__lookup_by_sock(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table, int sock)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool groupsock_groupsock_lookup_table__remove(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		groupsock_groupsock_t const *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

groupsock_groupsock_t *groupsock_groupsock_lookup_table__add_new(
		groupsock_groupsock_lookup_table_t *groupsock_lookup_table,
		net_address_bits group_address, net_address_bits source_filter_address,
		groupsock_port_t port, u_int8_t ttl)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

livemedia_hash_table_t **groupsock__get_socket_table(void)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	
}

bool groupsock__unset_groupsock_by_socket(groupsock_groupsock_t const *groupsock)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * GroupsockLookupTable::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_groupsock_lookup_table_iterator_t *__groupsock_groupsock_lookup_table_iterator__alloc(void)
{
	groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator;
	groupsock_lookup_table_iterator = (groupsock_groupsock_lookup_table_iterator_t *)calloc(1, sizeof(groupsock_groupsock_lookup_table_iterator_t));
	if (groupsock_lookup_table_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_groupsock_lookup_table_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return groupsock_lookup_table_iterator;
}
void __groupsock_groupsock_lookup_table_iterator__init(
		groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator,
		groupsock_groupsock_lookup_table_t *groupsocks)
{
	/* TODO: parent init */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	groupsock_lookup_table_iterator->delete = groupsock_groupsock_lookup_table_iterator__delete__impl;
}
void __groupsock_groupsock_lookup_table_iterator__deinit(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator)
{
	/* TODO: destructor */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* TODO: parent deinit */
}
void __groupsock_groupsock_lookup_table_iterator__free(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator)
{
	if (groupsock_lookup_table_iterator) {
		free(groupsock_lookup_table_iterator);
		groupsock_lookup_table_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_groupsock_lookup_table_iterator_t *groupsock__new__groupsock_lookup_table_iterator(
		groupsock_groupsock_lookup_table_t *groupsocks)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator;

	groupsock_lookup_table_iterator = __groupsock_groupsock_lookup_table_iterator__alloc();
	if (groupsock_lookup_table_iterator)
		__groupsock_groupsock_lookup_table_iterator__init(groupsock_lookup_table_iterator,
				groupsocks);

	return groupsock_lookup_table_iterator;
}
void groupsock__delete__groupsock_lookup_table_iterator(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator)
{
	if (groupsock_lookup_table_iterator && groupsock_lookup_table_iterator->delete)
		groupsock_lookup_table_iterator->delete(groupsock_lookup_table_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_groupsock_lookup_table_iterator__delete__impl(groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator)
{
	/*TODO: if child, get parent as parameter and cast to child */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (groupsock_lookup_table_iterator) {
		__groupsock_groupsock_lookup_table_iterator__deinit(groupsock_lookup_table_iterator);
		__groupsock_groupsock_lookup_table_iterator__free(groupsock_lookup_table_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_groupsock_t *groupsock_groupsock_lookup_table_iterator__next(
		groupsock_groupsock_lookup_table_iterator_t *groupsock_lookup_table_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */
