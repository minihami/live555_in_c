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
void __groupsock_net_address__init_by_data_and_length(groupsock_net_address_t *net_address,
		u_int8_t *data, unsigned int length /* default: 32 bits, 4bytes */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address__assign(net_address, data, length);

	net_address->delete = groupsock_net_address__delete__impl;
}
void __groupsock_net_address__init_by_length(groupsock_net_address_t *net_address,
		unsigned int length)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int i;
	net_address->data = (u_int8_t *)malloc(sizeof(u_int8_t) * length);
	if (net_address == NULL) {
		fprintf(stderr, "malloc(sizeof(u_int8_t * length))"
				"failed in %s()\n", __func__);
	}
	if (net_address->data == NULL) {
		net_address->length = 0;
		return ;
	}
	for (i = 0; i < length; ++i)
		net_address->data[i] = 0;
	net_address->length = length;
	
	net_address->delete = groupsock_net_address__delete__impl;
}
void __groupsock_net_address__init_by_orig(groupsock_net_address_t *net_address,
		groupsock_net_address_t *orig)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address__assign(net_address, groupsock_net_address__data(orig), 
			groupsock_net_address__length(orig));
	
	net_address->delete = groupsock_net_address__delete__impl;
}
void __groupsock_net_address__deinit(groupsock_net_address_t *net_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*  destructor */
	groupsock_net_address__clean(net_address);
}
void __groupsock_net_address__free(groupsock_net_address_t *net_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_address) {
		free(net_address);
		net_address = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_net_address_t *groupsock__new__net_address_by_data_and_length(
		u_int8_t *data, unsigned int length)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address_t *net_address;

	net_address = __groupsock_net_address__alloc();
	if (net_address)
		__groupsock_net_address__init_by_data_and_length(net_address, data, length);

	return net_address;
}
groupsock_net_address_t *groupsock__new__net_address_by_length(
		unsigned int length)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address_t *net_address;

	net_address = __groupsock_net_address__alloc();
	if (net_address)
		__groupsock_net_address__init_by_length(net_address, length);

	return net_address;
}
groupsock_net_address_t *groupsock__new__net_address_by_orig(
		groupsock_net_address_t *orig)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address_t *net_address;

	net_address = __groupsock_net_address__alloc();
	if (net_address)
		__groupsock_net_address__init_by_orig(net_address, orig);

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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_address) {
		__groupsock_net_address__deinit(net_address);
		__groupsock_net_address__free(net_address);
	}
}

/*
 * Functions in header file
 */
unsigned int groupsock_net_address__length(groupsock_net_address_t *net_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return net_address->length;
}

u_int8_t *groupsock_net_address__data(groupsock_net_address_t *net_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return net_address->data;
}


/*
 * Normal functions
 */
void groupsock_net_address__assign(groupsock_net_address_t *net_address,
		u_int8_t *data, unsigned int length)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int i;
	net_address->data = (u_int8_t *)malloc(sizeof(u_int8_t) * length);
	if (net_address == NULL) {
		fprintf(stderr, "malloc(sizeof(u_int8_t * length))"
				"failed in %s()\n", __func__);
	}
	if (net_address->data == NULL) {
		net_address->length = 0;
		return ;
	}
	for (i = 0; i < length; ++i)
		net_address->data[i] = data[i];
	net_address->length = length;
}

void groupsock_net_address__clean(groupsock_net_address_t *net_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_address->data) {
		free(net_address->data);
		net_address->data = NULL;
		net_address->length = 0;
	}
}


/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * NetAddressList (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_address_list_t *__groupsock_net_address_list__alloc(void)
{
	groupsock_net_address_list_t *net_address_list;
	net_address_list = (groupsock_net_address_list_t *)calloc(1, sizeof(groupsock_net_address_list_t));
	if (net_address_list == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_net_address_list_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return net_address_list;
}
void __groupsock_net_address_list__init_by_hostname(
		groupsock_net_address_list_t *net_address_list, char const *hostname)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	net_address_bits addr;
	u_int8_t **h_addr_ptr;
	u_int8_t **h_addr_ptr1;
	unsigned int i;
	struct addrinfo addrinfo_hints;
	struct addrinfo *addrinfo_result_ptr;
	int result;
	struct addrinfo *p;

	/* constructor */
	net_address_list->num_addresses = 0;
	net_address_list->address_array = NULL;
	
	/* First, check whether "hostname" is an IP address string: */
	addr = our_inet_addr((char *)hostname);
	if (addr != INADDR_NONE) {
    /* Yes, it was an IP address string.  Return a 1-element list with this address: */
		net_address_list->num_addresses = 1;
		
		net_address_list->address_array = 
			(groupsock_net_address_t **)malloc(
					sizeof(groupsock_net_address_t *)*net_address_list->num_addresses);
		if (net_address_list->address_array == NULL) {
			fprintf(stderr, "malloc(sizeof(groupsock_net_address_t *)"
				"*net_address_list->num_addresses)"
					"failed in %s()\n", __func__);
			return;
		}
		net_address_list->address_array[0] = 
			groupsock__new__net_address_by_data_and_length((u_int8_t *)&addr, 
					sizeof(net_address_bits));
		return;
	}
	/* "hostname" is not an IP address string; try resolving it
	 * as a real host name instead: */
#if defined(USE_GETHOSTBYNAME) || defined(VXWORKS)
	struct hostent* host;
#if defined(VXWORKS)
	char hostent_buf[512];

	host = (struct hostent *)resolvGetHostByName((char *)hostname,
			(char *)&hostent_buf, sizeof(hostent_buf));
#else
	host = gethostbyname((char*)hostname);
#endif
	if (host == NULL || host->h_length != 4 || host->h_addr_list == NULL)
		return; /* no luck */

	h_addr_ptr = (u_int8_t **)host->h_addr_list;
	/* First, count the number of addresses: */
	h_addr_ptr1 = h_addr_ptr;
	while (*h_addr_ptr1 != NULL) {
		++net_address_list->num_addresses;
		++h_addr_ptr1;
	}

	/* Next, set up the list: */
	net_address_list->address_array = 
		(groupsock_net_address_t **)malloc(
				sizeof(groupsock_net_address_t *) * net_address_list->num_addresses);
	if (net_address_list->address_array == NULL) {
		fprintf(stderr, "malloc(sizeof(groupsock_net_address_t *)"
				"*net_address_list->num_addresses)"
				"failed in %s()\n", __func__);
		return;
	}

	for (i = 0; i < net_address_list->num_addresses; ++i) {
		net_address_list->address_array[i] = 
			groupsock__new__net_address_by_data_and_length(h_addr_ptr[i], 
					host->h_length);
	}
#else
  /* Use "getaddrinfo()" (rather than the older, deprecated "gethostbyname()"): */
	memset(&addrinfo_hints, 0, sizeof(addrinfo_hints));
	addrinfo_hints.ai_family = AF_INET; 
	/* For now, we're interested in IPv4 addresses only */
	addrinfo_result_ptr = NULL;
	result = getaddrinfo(hostname, NULL, &addrinfo_hints, &addrinfo_result_ptr);
	if (result != 0 || addrinfo_result_ptr == NULL)
		return; /* no luck */
	
	/* First, count the number of addresses: */
	p = addrinfo_result_ptr;
	while (p != NULL) {
		if (p->ai_addrlen < 4)
			continue; /* sanity check: skip over addresses that are too small */
		++net_address_list->num_addresses;
		p = p->ai_next;
	}

	/* Next, set up the list: */
	net_address_list->address_array = 
		(groupsock_net_address_t **)malloc(
				sizeof(groupsock_net_address_t *) * net_address_list->num_addresses);
	if (net_address_list->address_array == NULL) {
		fprintf(stderr, "malloc(sizeof(groupsock_net_address_t *)"
				"*net_address_list->num_addresses)"
				"failed in %s()\n", __func__);
		return;
	}
	i = 0;
	p = addrinfo_result_ptr;
	while (p != NULL) {
		if (p->ai_addrlen < 4)
			continue;
		net_address_list->address_array[i++] = 
			groupsock__new__net_address_by_data_and_length((u_int8_t const*)&(((struct sockaddr_in*)p->ai_addr)->sin_addr.s_addr), 4);
		p = p->ai_next;
	}
	
	/* Finally, free the data that we had allocated by calling "getaddrinfo()": */
	freeaddrinfo(addrinfo_result_ptr);
#endif

	/* virtual functions configuration */ 
	net_address_list->delete = groupsock_net_address_list__delete__impl;
}
void __groupsock_net_address_list__init_by_orig(
		groupsock_net_address_list_t *net_address_list,
		groupsock_net_address_list_t const *orig)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	net_address_list->delete = groupsock_net_address_list__delete__impl;
}
void __groupsock_net_address_list__deinit(groupsock_net_address_list_t *net_address_list)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* destructor */
	groupsock_net_address_list__clean(net_address_list);

}
void __groupsock_net_address_list__free(groupsock_net_address_list_t *net_address_list)
{
	if (net_address_list) {
		free(net_address_list);
		net_address_list = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_net_address_list_t *groupsock__new__net_address_list_by_hostname(
		char const *hostname)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address_list_t *net_address_list;

	net_address_list = __groupsock_net_address_list__alloc();
	if (net_address_list)
		__groupsock_net_address_list__init_by_hostname(net_address_list, hostname);

	return net_address_list;
}
groupsock_net_address_list_t *groupsock__new__net_address_list_by_orig(
		groupsock_net_address_list_t const *orig)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address_list_t *net_address_list;

	net_address_list = __groupsock_net_address_list__alloc();
	if (net_address_list)
		__groupsock_net_address_list__init_by_orig(net_address_list, orig);

	return net_address_list;
}
void groupsock__delete__net_address_list(groupsock_net_address_list_t *net_address_list)
{
	if (net_address_list && net_address_list->delete)
		net_address_list->delete(net_address_list);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_address_list__delete__impl(groupsock_net_address_list_t *net_address_list)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_address_list) {
		__groupsock_net_address_list__deinit(net_address_list);
		__groupsock_net_address_list__free(net_address_list);
	}
}

/*
 * Functions in header file
 */
unsigned int groupsock_net_address_list__num_addresses(
		groupsock_net_address_list_t *net_address_list)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return net_address_list->num_addresses;

}

/*
 * Normal functions
 */
groupsock_net_address_t const *groupsock_net_address_list__first_address(
		groupsock_net_address_list_t *net_address_list)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_net_address_list__assign(groupsock_net_address_list_t *net_address_list,
		net_address_bits num_addresses, groupsock_net_address_t **address_array)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void groupsock_net_address_list__clean(groupsock_net_address_list_t *net_address_list)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	while (net_address_list->num_addresses-- > 0) {
		groupsock__delete__net_address(net_address_list->address_array[
				net_address_list->num_addresses]);
	}
	if (net_address_list->address_array) {
		free(net_address_list->address_array);
		net_address_list->address_array = NULL;
	}
}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * NetAddressList::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_net_address_list_iterator_t *__groupsock_net_address_list_iterator__alloc(void)
{
	groupsock_net_address_list_iterator_t *net_address_list_iterator;
	net_address_list_iterator = (groupsock_net_address_list_iterator_t *)calloc(1, sizeof(groupsock_net_address_list_iterator_t));
	if (net_address_list_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_net_address_list_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return net_address_list_iterator;
}
void __groupsock_net_address_list_iterator__init(
		groupsock_net_address_list_iterator_t *net_address_list_iterator,
		groupsock_net_address_list_t const *address_list)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* constructor */
	net_address_list_iterator->address_list = address_list;
	net_address_list_iterator->next_index = 0;
	
	/* virtual functions configuration */ 
	net_address_list_iterator->delete = groupsock_net_address_list_iterator__delete__impl;
}
void __groupsock_net_address_list_iterator__deinit(groupsock_net_address_list_iterator_t *net_address_list_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void __groupsock_net_address_list_iterator__free(groupsock_net_address_list_iterator_t *net_address_list_iterator)
{
	if (net_address_list_iterator) {
		free(net_address_list_iterator);
		net_address_list_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_net_address_list_iterator_t *groupsock__new__net_address_list_iterator(
		groupsock_net_address_list_t const *address_list)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_net_address_list_iterator_t *net_address_list_iterator;

	net_address_list_iterator = __groupsock_net_address_list_iterator__alloc();
	if (net_address_list_iterator)
		__groupsock_net_address_list_iterator__init(net_address_list_iterator, address_list);

	return net_address_list_iterator;
}
void groupsock__delete__net_address_list_iterator(groupsock_net_address_list_iterator_t *net_address_list_iterator)
{
	if (net_address_list_iterator && net_address_list_iterator->delete)
		net_address_list_iterator->delete(net_address_list_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_net_address_list_iterator__delete__impl(groupsock_net_address_list_iterator_t *net_address_list_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_address_list_iterator) {
		__groupsock_net_address_list_iterator__deinit(net_address_list_iterator);
		__groupsock_net_address_list_iterator__free(net_address_list_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
groupsock_net_address_t *groupsock_net_address_list_iterator__next_address(
		groupsock_net_address_list_iterator_t *net_address_list_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (net_address_list_iterator->next_index  >= 
			groupsock_net_address_list__num_addresses(
				net_address_list_iterator->address_list))
		return NULL; /* no more */
	return net_address_list_iterator->address_list->address_array[
		net_address_list_iterator->next_index++];
}

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
void __groupsock_port__init(groupsock_port_t *port, port_num_bits num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	port->port_num = htons(num);
	
	/* virtual functions configuration */ 
	port->delete = groupsock_port__delete__impl;
}
void __groupsock_port__deinit(groupsock_port_t *port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
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
groupsock_port_t *groupsock__new__port(port_num_bits num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_port_t *port;

	port = __groupsock_port__alloc();
	if (port)
		__groupsock_port__init(port, num);

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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return port->port_num;
}

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * AddressPortLookupTable (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_address_port_lookup_table_t *__groupsock_address_port_lookup_table__alloc(void)
{
	groupsock_address_port_lookup_table_t *address_port_lookup_table;
	address_port_lookup_table = (groupsock_address_port_lookup_table_t *)calloc(1, sizeof(groupsock_address_port_lookup_table_t));
	if (address_port_lookup_table == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_address_port_lookup_table_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return address_port_lookup_table;
}
void __groupsock_address_port_lookup_table__init(groupsock_address_port_lookup_table_t *address_port_lookup_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	address_port_lookup_table->table = livemedia_hash_table__create__static(3);
	
	/* virtual functions configuration */ 

	address_port_lookup_table->delete = groupsock_address_port_lookup_table__delete__impl;
}
void __groupsock_address_port_lookup_table__deinit(groupsock_address_port_lookup_table_t *address_port_lookup_table)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __groupsock_address_port_lookup_table__free(groupsock_address_port_lookup_table_t *address_port_lookup_table)
{
	if (address_port_lookup_table) {
		free(address_port_lookup_table);
		address_port_lookup_table = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_address_port_lookup_table_t *groupsock__new__address_port_lookup_table(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_port_lookup_table_t *address_port_lookup_table;

	address_port_lookup_table = __groupsock_address_port_lookup_table__alloc();
	if (address_port_lookup_table)
		__groupsock_address_port_lookup_table__init(address_port_lookup_table);

	return address_port_lookup_table;
}
void groupsock__delete__address_port_lookup_table(groupsock_address_port_lookup_table_t *address_port_lookup_table)
{
	if (address_port_lookup_table && address_port_lookup_table->delete)
		address_port_lookup_table->delete(address_port_lookup_table);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_address_port_lookup_table__delete__impl(groupsock_address_port_lookup_table_t *address_port_lookup_table)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO: if child, get parent as parameter and cast to child */
	if (address_port_lookup_table) {
		__groupsock_address_port_lookup_table__deinit(address_port_lookup_table);
		__groupsock_address_port_lookup_table__free(address_port_lookup_table);
	}
}

/*
 * Functions in header file
 */
void *groupsock_address_port_lookup_table__remove_next(
		groupsock_address_port_lookup_table_t *address_port_lookup_table)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Normal functions
 */
void *groupsock_address_port_lookup_table__add(
		groupsock_address_port_lookup_table_t *address_port_lookup_table,
		net_address_bits address1, net_address_bits address2, groupsock_port_t port, void *value)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int key[3];
	key[0] = (int)address1;
	key[1] = (int)address2;
	key[2] = (int)groupsock_port__num(&port);
	return livemedia_hash_table__add(address_port_lookup_table->table, (char *)key, value);

}

bool groupsock_address_port_lookup_table__remove(
		groupsock_address_port_lookup_table_t *address_port_lookup_table,
		net_address_bits address1, net_address_bits address2, groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void *groupsock_address_port_lookup_table__lookup(
		groupsock_address_port_lookup_table_t *address_port_lookup_table,
		net_address_bits address1, net_address_bits address2, groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * AddressPortLookupTable::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_address_port_lookup_table_iterator_t *__groupsock_address_port_lookup_table_iterator__alloc(void)
{
	groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator;
	address_port_lookup_table_iterator = (groupsock_address_port_lookup_table_iterator_t *)calloc(1, sizeof(groupsock_address_port_lookup_table_iterator_t));
	if (address_port_lookup_table_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_address_port_lookup_table_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return address_port_lookup_table_iterator;
}
void __groupsock_address_port_lookup_table_iterator__init(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator,
		groupsock_address_port_lookup_table_t *table)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	address_port_lookup_table_iterator->delete = groupsock_address_port_lookup_table_iterator__delete__impl;
}
void __groupsock_address_port_lookup_table_iterator__deinit(groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __groupsock_address_port_lookup_table_iterator__free(groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator)
{
	if (address_port_lookup_table_iterator) {
		free(address_port_lookup_table_iterator);
		address_port_lookup_table_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_address_port_lookup_table_iterator_t *groupsock__new__address_port_lookup_table_iterator(
		groupsock_address_port_lookup_table_t *table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator;

	address_port_lookup_table_iterator = __groupsock_address_port_lookup_table_iterator__alloc();
	if (address_port_lookup_table_iterator)
		__groupsock_address_port_lookup_table_iterator__init(
				address_port_lookup_table_iterator, table);

	return address_port_lookup_table_iterator;
}
void groupsock__delete__address_port_lookup_table_iterator(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator)
{
	if (address_port_lookup_table_iterator && address_port_lookup_table_iterator->delete)
		address_port_lookup_table_iterator->delete(address_port_lookup_table_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_address_port_lookup_table_iterator__delete__impl(groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO: if child, get parent as parameter and cast to child */
	if (address_port_lookup_table_iterator) {
		__groupsock_address_port_lookup_table_iterator__deinit(address_port_lookup_table_iterator);
		__groupsock_address_port_lookup_table_iterator__free(address_port_lookup_table_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void *groupsock_address_port_lookup_table_iterator__next(
		groupsock_address_port_lookup_table_iterator_t *address_port_lookup_table_iterator)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */

/* not class function */
bool groupsock__is_multicast_address(net_address_bits address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Note: We return False for addresses in the range 224.0.0.0
	* through 224.0.0.255, because these are non-routable
	* Note: IPv4-specific ##### */
	net_address_bits address_in_network_order = htonl(address);
	return address_in_network_order >  0xE00000FF &&
		address_in_network_order <= 0xEFFFFFFF;
}

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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_string__init(address_string, addr->sin_addr.s_addr);
	
	address_string->delete = groupsock_address_string__delete__impl;
}

void __groupsock_address_string__init_by_in_addr(groupsock_address_string_t *address_string,
		struct in_addr const* addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_string__init(address_string, addr->s_addr);
	
	address_string->delete = groupsock_address_string__delete__impl;
}

void __groupsock_address_string__init_by_net_address_bits(groupsock_address_string_t *address_string,
		net_address_bits addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_string__init(address_string, addr);
	
	address_string->delete = groupsock_address_string__delete__impl;
}

void __groupsock_address_string__deinit(groupsock_address_string_t *address_string)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (address_string->val) {
		free(address_string->val);
		address_string->val = NULL;
	}
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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_string_t *address_string;

	address_string = __groupsock_address_string__alloc();
	if (address_string)
		__groupsock_address_string__init_by_sockaddr_in(address_string, addr);

	return address_string;
}

groupsock_address_string_t *groupsock__new__address_string_by_in_addr(struct in_addr const* addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_address_string_t *address_string;

	address_string = __groupsock_address_string__alloc();
	if (address_string)
		__groupsock_address_string__init_by_in_addr(address_string, addr);

	return address_string;
}

groupsock_address_string_t *groupsock__new__address_string_by_net_address_bits(net_address_bits addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
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
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return address_string->val;
}

/*
 * Normal functions
 */
void groupsock_address_string__init(groupsock_address_string_t *address_string, net_address_bits addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	net_address_bits addr_nbo;

	address_string->val = (char *)malloc(sizeof(char) * 16);
	/* large enough for "abc.def.ghi.jkl" */
	addr_nbo = htonl(addr); /* make sure we have a value in a known byte order: big endian */
  sprintf(address_string->val, "%u.%u.%u.%u", (addr_nbo>>24)&0xFF, (addr_nbo>>16)&0xFF, (addr_nbo>>8)&0xFF, addr_nbo&0xFF);
}

/*
 * Static functions implementation for ancestor
 */
