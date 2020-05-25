#include "group_eid.h"
#include <stdio.h>

/*****************************************************************
 * GroupEId: (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_group_eid_t *__groupsock_group_eid__alloc(void)
{
	groupsock_group_eid_t *group_eid;
	group_eid = (groupsock_group_eid_t *)calloc(1, sizeof(groupsock_group_eid_t));
	if (group_eid == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_group_eid_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return group_eid;
}

void __groupsock_group_eid__init_by_ttl(groupsock_group_eid_t *group_eid, 
		struct in_addr const *group_addr,
		port_num_bits port_num, u_int8_t ttl)
{
	struct in_addr source_filter_addr;
	source_filter_addr.s_addr = ~0;		/* indicates no source filter */
	groupsock_group_eid__init(group_eid, group_addr, &source_filter_addr, port_num, ttl);
	
	/* virtual functions configuration */ 
	group_eid->delete = groupsock_group_eid__delete__impl;
}

void __groupsock_group_eid__init_by_source_filter_addr(groupsock_group_eid_t *group_eid,
		struct in_addr const *group_addr, 
		struct in_addr const *source_filter_addr,
		port_num_bits port_num)
{
	groupsock_group_eid__init(group_eid, group_addr, source_filter_addr, port_num, 255);
	
	/* virtual functions configuration */ 
	group_eid->delete = groupsock_group_eid__delete__impl;
}

void __groupsock_group_eid__deinit(groupsock_group_eid_t *group_eid)
{
}

void __groupsock_group_eid__free(groupsock_group_eid_t *group_eid)
{
	if (group_eid) {
		free(group_eid);
		group_eid = NULL;
	}
}

/*
 * new() and delete() functions
 */
groupsock_group_eid_t *groupsock__new__group_eid_by_ttl(
		struct in_addr const *group_addr,
		port_num_bits port_num, u_int8_t ttl)
{
	groupsock_group_eid_t *group_eid;

	group_eid = __groupsock_group_eid__alloc();
	if (group_eid)
		__groupsock_group_eid__init_by_ttl(group_eid, group_addr,
				port_num, ttl);

	return group_eid;
}

groupsock_group_eid_t *groupsock__new__group_eid_by_source_filter_addr(
		struct in_addr const *group_addr, 
		struct in_addr const *source_filter_addr,
		port_num_bits port_num)
{
	groupsock_group_eid_t *group_eid;

	group_eid = __groupsock_group_eid__alloc();
	if (group_eid)
		__groupsock_group_eid__init_by_source_filter_addr(group_eid, group_addr,
				source_filter_addr, port_num);

	return group_eid;
}

void groupsock__delete__group_eid(groupsock_group_eid_t *group_eid)
{
	if (group_eid && group_eid->delete)
		group_eid->delete(group_eid);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_group_eid__delete__impl(groupsock_group_eid_t *group_eid)
{
	if (group_eid) {
		__groupsock_group_eid__deinit(group_eid);
		__groupsock_group_eid__free(group_eid);
	}
}

/*
 * Functions in header file
 */
struct in_addr const *groupsock_group_eid__group_address(groupsock_group_eid_t *group_eid)
{
	return &group_eid->group_address;
}
struct in_addr const *groupsock_group_eid__source_filter_address(
		groupsock_group_eid_t *group_eid)
{
	return &group_eid->source_filter_address;
}
port_num_bits groupsock_group_eid__port_num(groupsock_group_eid_t *group_eid)
{
	return group_eid->port_num;
}
u_int8_t groupsock_group_eid__ttl(groupsock_group_eid_t *group_eid)
{
	return group_eid->ttl;
}

/*
 * Normal functions
 */
bool groupsock_group_eid__is_ssm(groupsock_group_eid_t *group_eid)
{
	net_address_bits net_bits;
	net_bits = ~0;
	return ((group_eid->source_filter_address).s_addr != net_bits);
}
void groupsock_group_eid__init(groupsock_group_eid_t *group_eid,
		struct in_addr const *group_addr,
		struct in_addr const *source_filter_addr,
		port_num_bits port_num,
		u_int8_t ttl)
{
	group_eid->group_address = *group_addr;
	group_eid->source_filter_address = *source_filter_addr;
	group_eid->port_num = port_num;
	group_eid->ttl = ttl;
}
