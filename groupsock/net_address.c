#include "net_address.h"
#include <stdlib.h>
#include <stdio.h>


/*****************************************************************
 * NetAddress: (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_address_t *__groupsock_net_address__alloc(void)
{
	groupsock_net_address_t *net_address;
	net_address = (groupsock_net_address_t *)calloc(1, sizeof(groupsock_net_address_t));
	if (net_address == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_net_address_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return net_address;
}
void __groupsock_net_address__init(groupsock_net_address_t *net_address)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	net_address->delete = groupsock_net_address__delete__impl;
}
void __groupsock_net_address__deinit(groupsock_net_address_t *net_address)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __groupsock_net_address__free(groupsock_net_address_t *net_address)
{
	if (net_address) {
		free(net_address);
		net_address = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_net_address_t *groupsock__new__net_address(void)
{
	groupsock_net_address_t *net_address;

	net_address = __groupsock_net_address__alloc();
	if (net_address)
		__groupsock_net_address__init(net_address);

	return net_address;
}
void groupsock__delete__net_address(groupsock_net_address_t *net_address)
{
	if (net_address && net_address->delete)
		net_address->delete(net_address);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_address__delete__impl(groupsock_net_address_t *net_address)
{
	if (net_address) {
		__groupsock_net_address__deinit(net_address);
		__groupsock_net_address__free(net_address);
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
 * Port (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_port_t *__groupsock_port__alloc(void)
{
	groupsock_port_t *port;
	port = (groupsock_port_t *)calloc(1, sizeof(groupsock_port_t));
	if (port == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_port_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return port;
}
void __groupsock_port__init(groupsock_port_t *port)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	port->delete = groupsock_port__delete__impl;
}
void __groupsock_port__deinit(groupsock_port_t *port)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __groupsock_port__free(groupsock_port_t *port)
{
	if (port) {
		free(port);
		port = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_port_t *groupsock__new__port(void)
{
	groupsock_port_t *port;

	port = __groupsock_port__alloc();
	if (port)
		__groupsock_port__init(port);

	return port;
}
void groupsock__delete__port(groupsock_port_t *port)
{
	if (port && port->delete)
		port->delete(port);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_port__delete__impl(groupsock_port_t *port)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (port) {
		__groupsock_port__deinit(port);
		__groupsock_port__free(port);
	}
}

/*
 * Functions in header file
 */
port_num_bits groupsock_port__num(groupsock_port_t *port)
{
	return port->port_num;
}

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * AddressString (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_address_string_t *__groupsock_address_string__alloc(void)
{
	groupsock_address_string_t *address_string;
	address_string = (groupsock_address_string_t *)calloc(1, sizeof(groupsock_address_string_t));
	if (address_string == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_address_string_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return address_string;
}
void __groupsock_address_string__init_by_sockaddr_in(groupsock_address_string_t *address_string,
		struct sockaddr_in const *addr)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	address_string->delete = groupsock_address_string__delete__impl;
}

void __groupsock_address_string__init_by_in_addr(groupsock_address_string_t *address_string,
		struct in_addr const* addr)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	address_string->delete = groupsock_address_string__delete__impl;
}

void __groupsock_address_string__init_by_net_address_bits(groupsock_address_string_t *address_string,
		net_address_bits addr)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	address_string->delete = groupsock_address_string__delete__impl;
}

void __groupsock_address_string__deinit(groupsock_address_string_t *address_string)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __groupsock_address_string__free(groupsock_address_string_t *address_string)
{
	if (address_string) {
		free(address_string);
		address_string = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_address_string_t *groupsock__new__address_string_by_sockaddr_in(struct sockaddr_in const *addr)
{
	groupsock_address_string_t *address_string;

	address_string = __groupsock_address_string__alloc();
	if (address_string)
		__groupsock_address_string__init_by_sockaddr_in(address_string, addr);

	return address_string;
}

groupsock_address_string_t *groupsock__new__address_string_by_in_addr(struct in_addr const* addr)
{
	groupsock_address_string_t *address_string;

	address_string = __groupsock_address_string__alloc();
	if (address_string)
		__groupsock_address_string__init_by_in_addr(address_string, addr);

	return address_string;
}

groupsock_address_string_t *groupsock__new__address_string_by_net_address_bits(net_address_bits addr)
{
	groupsock_address_string_t *address_string;

	address_string = __groupsock_address_string__alloc();
	if (address_string)
		__groupsock_address_string__init_by_net_address_bits(address_string, addr);

	return address_string;
}

void groupsock__delete__address_string(groupsock_address_string_t *address_string)
{
	if (address_string && address_string->delete)
		address_string->delete(address_string);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_address_string__delete__impl(groupsock_address_string_t *address_string)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (address_string) {
		__groupsock_address_string__deinit(address_string);
		__groupsock_address_string__free(address_string);
	}
}

/*
 * Functions in header file
 */
char const* groupsock_address_string__val(groupsock_address_string_t *address_string)
{
	return address_string->val;
}

/*
 * Normal functions
 */
void groupsock_address_string__init(groupsock_address_string_t *address_string, net_address_bits addr)
{

}

/*
 * Static functions implementation for ancestor
 */
