
#ifndef __GROUPSOCK_NET_ADDRESS_H__
#define __GROUPSOCK_NET_ADDRESS_H__ 

#include <stdbool.h>
#include <sys/types.h>
#include "../livemedia/hash_table.h"
#include "net_common.h"
#include "../usage_environment/usage_environment.h"

typedef u_int32_t net_address_bits;
/*****************************************************************
 * NetAddress (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_net_address_t {
	unsigned int length;
	u_int8_t *data;

	/* virtual function pointers */
	void (*delete)(struct _groupsock_net_address_t *net_address);

} groupsock_net_address_t;


/*****************************************************************
 * NetAddressList (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_net_address_list_t {
	unsigned int num_addresses;
	groupsock_net_address_t **address_array;

	void (*delete)(struct _groupsock_net_address_list_t *net_address_list);
} groupsock_net_address_list_t;

/*****************************************************************
 * NetAddressList::Iterator (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_net_address_list_iterator_t {
	groupsock_net_address_list_t *address_list;
	unsigned int next_index;

	void (*delete)(
			struct _groupsock_net_address_list_iterator_t *net_address_list_iterator);
} groupsock_net_address_list_iterator_t;

/*****************************************************************
 * Port(Structure Declarations)
 ****************************************************************/
typedef u_int16_t port_num_bits;
typedef struct _groupsock_port_t {

	port_num_bits port_num;		/* stored in network byte order */
#ifdef IRIX
	port_num_bits filler;		/* hack to overcome a bug in IRIX C++ compiler */
#endif

	/* virtual function pointers */
	void (*delete)(struct _groupsock_port_t *port);
} groupsock_port_t;

/*****************************************************************
 * AddressPortLookupTable (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_address_port_lookup_table_t {
	livemedia_hash_table_t *table;

	void (*delete)(struct _groupsock_address_port_lookup_table_t *address_port_lookup_table);
} groupsock_address_port_lookup_table_t;

/*****************************************************************
 * AddressPortLookupTable::Iterator (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_address_port_lookup_table_iterator_t {
	livemedia_hash_table_iterator_t *iter;

	void (*delete)(
			struct _groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator);
} groupsock_address_port_lookup_table_iterator_t;

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
void __groupsock_net_address__init_by_data_and_length(groupsock_net_address_t *net_address,
		u_int8_t *data, unsigned int length /* default: 32 bits, 4bytes */);
void __groupsock_net_address__init_by_length(groupsock_net_address_t *net_address,
		unsigned int length); /* sets address data to all-zeros */
void __groupsock_net_address__init_by_orig(groupsock_net_address_t *net_address,
		groupsock_net_address_t *orig);
void __groupsock_net_address__deinit(groupsock_net_address_t *net_address);
void __groupsock_net_address__free(groupsock_net_address_t *net_address);
/*
 * new() and delete() functions
 */
groupsock_net_address_t *groupsock__new__net_address_by_data_and_length(
		u_int8_t *data, unsigned int length);
groupsock_net_address_t *groupsock__new__net_address_by_length(
		unsigned int length);
groupsock_net_address_t *groupsock__new__net_address_by_orig(
		groupsock_net_address_t *orig);
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
unsigned int groupsock_net_address__length(groupsock_net_address_t *net_address);
u_int8_t *groupsock_net_address__data(groupsock_net_address_t *net_address);

/*
 * Normal functions
 */
void groupsock_net_address__assign(groupsock_net_address_t *net_address,
		u_int8_t *data, unsigned int length);
void groupsock_net_address__clean(groupsock_net_address_t *net_address);

/*****************************************************************
 * NetAddressList (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_address_list_t *__groupsock_net_address_list__alloc(void);
void __groupsock_net_address_list__init_by_hostname(
		groupsock_net_address_list_t *net_address_list, char const *hostname);
void __groupsock_net_address_list__init_by_orig(
		groupsock_net_address_list_t *net_address_list,
		groupsock_net_address_list_t const *orig);
void __groupsock_net_address_list__deinit(groupsock_net_address_list_t *net_address_list);
void __groupsock_net_address_list__free(groupsock_net_address_list_t *net_address_list);
/*
 * new() and delete() functions
 */
groupsock_net_address_list_t *groupsock__new__net_address_list_by_hostname(
		char const *hostname);
groupsock_net_address_list_t *groupsock__new__net_address_list_by_orig(
		groupsock_net_address_list_t const *orig);
void groupsock__delete__net_address_list(groupsock_net_address_list_t *net_address_list);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_address_list__delete__impl(groupsock_net_address_list_t *net_address_list);

/*
 * Functions in header file
 */
unsigned int groupsock_net_address_list__num_addresses(
		groupsock_net_address_list_t *net_address_list);

/*
 * Normal functions
 */
groupsock_net_address_t const *groupsock_net_address_list__first_address(
		groupsock_net_address_list_t *net_address_list);
void groupsock_net_address_list__assign(groupsock_net_address_list_t *net_address_list,
		net_address_bits num_addresses, groupsock_net_address_t **address_array);
void groupsock_net_address_list__clean(groupsock_net_address_list_t *net_address_list);

/*****************************************************************
 * NetAddressList::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_address_list_iterator_t *__groupsock_net_address_list_iterator__alloc(void);
void __groupsock_net_address_list_iterator__init(
		groupsock_net_address_list_iterator_t *net_address_list_iterator,
		groupsock_net_address_list_t const *address_list);
void __groupsock_net_address_list_iterator__deinit(groupsock_net_address_list_iterator_t *net_address_list_iterator);
void __groupsock_net_address_list_iterator__free(groupsock_net_address_list_iterator_t *net_address_list_iterator);
/*
 * new() and delete() functions
 */
groupsock_net_address_list_iterator_t *groupsock__new__net_address_list_iterator(
		groupsock_net_address_list_t const *address_list);
void groupsock__delete__net_address_list_iterator(groupsock_net_address_list_iterator_t *net_address_list_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_address_list_iterator__delete__impl(groupsock_net_address_list_iterator_t *net_address_list_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_net_address_t *groupsock_net_address_list_iterator__next_address(
		groupsock_net_address_list_iterator_t *net_address_list_iterator); /* NULL if none */

/*****************************************************************
 * Port(Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_port_t *__groupsock_port__alloc(void);
void __groupsock_port__init(groupsock_port_t *port, port_num_bits num);
void __groupsock_port__deinit(groupsock_port_t *port);
void __groupsock_port__free(groupsock_port_t *port);
/*
 * new() and delete() functions
 */
groupsock_port_t *groupsock__new__port(port_num_bits num);
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
 * AddressPortLookupTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_address_port_lookup_table_t *__groupsock_address_port_lookup_table__alloc(void);
void __groupsock_address_port_lookup_table__init(
		groupsock_address_port_lookup_table_t *address_port_lookup_table);
void __groupsock_address_port_lookup_table__deinit(
		groupsock_address_port_lookup_table_t *address_port_lookup_table);
void __groupsock_address_port_lookup_table__free(
		groupsock_address_port_lookup_table_t *address_port_lookup_table);
/*
 * new() and delete() functions
 */
groupsock_address_port_lookup_table_t *groupsock__new__address_port_lookup_table(void);
void groupsock__delete__address_port_lookup_table(
		groupsock_address_port_lookup_table_t *address_port_lookup_table);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_address_port_lookup_table__delete__impl(
		groupsock_address_port_lookup_table_t *address_port_lookup_table);

/*
 * Functions in header file
 */
void *groupsock_address_port_lookup_table__remove_next(
		groupsock_address_port_lookup_table_t *address_port_lookup_table);

/*
 * Normal functions
 */
void *groupsock_address_port_lookup_table__add(
		groupsock_address_port_lookup_table_t *address_port_lookup_table,
		net_address_bits address1, net_address_bits address2, groupsock_port_t port, void *value);
/* Returns the old value if different, otherwise 0 */
bool groupsock_address_port_lookup_table__remove(
		groupsock_address_port_lookup_table_t *address_port_lookup_table,
		net_address_bits address1, net_address_bits address2, groupsock_port_t port);
void *groupsock_address_port_lookup_table__lookup(
		groupsock_address_port_lookup_table_t *address_port_lookup_table,
		net_address_bits address1, net_address_bits address2, groupsock_port_t port);

/*****************************************************************
 * AddressPortLookupTable::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_address_port_lookup_table_iterator_t *__groupsock_address_port_lookup_table_iterator__alloc(void);
void __groupsock_address_port_lookup_table_iterator__init(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator,
		groupsock_address_port_lookup_table_t *table);
void __groupsock_address_port_lookup_table_iterator__deinit(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator);
void __groupsock_address_port_lookup_table_iterator__free(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator);
/*
 * new() and delete() functions
 */
groupsock_address_port_lookup_table_iterator_t *groupsock__new__address_port_lookup_table_iterator(
		groupsock_address_port_lookup_table_t *table);
void groupsock__delete__address_port_lookup_table_iterator(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_address_port_lookup_table_iterator__delete__impl(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void *groupsock_address_port_lookup_table_iterator__next(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator);

/* not class function */
bool groupsock__is_multicast_address(net_address_bits address);

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
