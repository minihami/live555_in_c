
#ifndef __GROUPSOCK_NET_ADDRESS_H__
#define __GROUPSOCK_NET_ADDRESS_H__ 

#include <stdbool.h>
#include <sys/types.h>
#include "net_common.h"
typedef u_int32_t net_address_bits;
/*****************************************************************
 * NetAddress (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_net_address_t {
	/* TODO: member variables */
	unsigned int length;
	u_int8_t *data;

	/* TODO: virtual function pointers */
	void (*delete)(struct _groupsock_net_address_t *net_address);

} groupsock_net_address_t;

typedef u_int16_t port_num_bits;

/*****************************************************************
 * Port(Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_port_t {
	/* TODO: parent structure */

	/* TODO: member variables */
	port_num_bits port_num;

	/* TODO: virtual function pointers */
	void (*delete)(struct _groupsock_port_t *port);

} groupsock_port_t;

/*****************************************************************
 * AddressString (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_address_string_t {
	char *val;

	void (*delete)(struct _groupsock_address_string_t *address_string);
} groupsock_address_string_t;

/*****************************************************************
 * NetAddress (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_address_t *__groupsock_net_address__alloc(void);
void __groupsock_net_address__init(groupsock_net_address_t *net_address);
void __groupsock_net_address__deinit(groupsock_net_address_t *net_address);
void __groupsock_net_address__free(groupsock_net_address_t *net_address);
/*
 * new() and delete() functions
 */
groupsock_net_address_t *groupsock__new__net_address(void);
void groupsock__delete__net_address(groupsock_net_address_t *net_address);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_address__delete__impl(groupsock_net_address_t *net_address);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */


/*****************************************************************
 * Port(Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_port_t *__groupsock_port__alloc(void);
void __groupsock_port__init(groupsock_port_t *port);
void __groupsock_port__deinit(groupsock_port_t *port);
void __groupsock_port__free(groupsock_port_t *port);
/*
 * new() and delete() functions
 */
groupsock_port_t *groupsock__new__port(void);
void groupsock__delete__port(groupsock_port_t *port);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_port__delete__impl(groupsock_port_t *port);

/*
 * Functions in header file
 */
port_num_bits groupsock_port__num(groupsock_port_t *port);

/*
 * Normal functions
 */


/*****************************************************************
 * AddressString (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_address_string_t *__groupsock_address_string__alloc(void);
void __groupsock_address_string__init_by_sockaddr_in(groupsock_address_string_t *address_string,
		struct sockaddr_in const *addr);
void __groupsock_address_string__init_by_in_addr(groupsock_address_string_t *address_string,
		struct in_addr const* addr);
void __groupsock_address_string__init_by_net_address_bits(groupsock_address_string_t *address_string,
		net_address_bits addr);
void __groupsock_address_string__deinit(groupsock_address_string_t *address_string);
void __groupsock_address_string__free(groupsock_address_string_t *address_string);
/*
 * new() and delete() functions
 */
groupsock_address_string_t *groupsock__new__address_string_by_sockaddr_in(struct sockaddr_in const *addr);
groupsock_address_string_t *groupsock__new__address_string_by_in_addr(struct in_addr const* addr);
groupsock_address_string_t *groupsock__new__address_string_by_net_address_bits(net_address_bits addr);
void groupsock__delete__address_string(groupsock_address_string_t *address_string);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_address_string__delete__impl(groupsock_address_string_t *address_string);

/*
 * Functions in header file
 */
char const* groupsock_address_string__val(groupsock_address_string_t *address_string);
/*
 * Normal functions
 */
void groupsock_address_string__init(groupsock_address_string_t *address_string, net_address_bits addr);
#endif
