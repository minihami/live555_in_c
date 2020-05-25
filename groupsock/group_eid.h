
#ifndef __GROUPSOCK_GROUPEID_H__
#define __GROUPSOCK_GROUPEID_H__
#include <stdbool.h>
#include "net_address.h"

/*****************************************************************
 * GroupEId (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_group_eid_t {
	struct in_addr group_address;
	struct in_addr source_filter_address;
	port_num_bits port_num;
	u_int8_t ttl;

	/* virtual function pointers */
	void (*delete)(struct _groupsock_group_eid_t *group_eid);

} groupsock_group_eid_t;

/*****************************************************************
 * GroupEId (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_group_eid_t *__groupsock_group_eid__alloc(void);
void __groupsock_group_eid__init_by_ttl(groupsock_group_eid_t *group_eid, 
		struct in_addr const *group_addr,
		port_num_bits port_num, u_int8_t ttl);
void __groupsock_group_eid__init_by_source_filter_addr(groupsock_group_eid_t *group_eid,
		struct in_addr const *group_addr, 
		struct in_addr const *source_filter_addr,
		port_num_bits port_num);
void __groupsock_group_eid__deinit(groupsock_group_eid_t *group_eid);
void __groupsock_group_eid__free(groupsock_group_eid_t *group_eid);
/*
 * new() and delete() functions
 */
groupsock_group_eid_t *groupsock__new__group_eid_by_ttl(
		struct in_addr const *group_addr,
		port_num_bits port_num, u_int8_t ttl);

/* used for a 'source-independent multicast' group */
groupsock_group_eid_t *groupsock__new__group_eid_by_source_filter_addr(
		struct in_addr const *group_addr, 
		struct in_addr const *source_filter_addr,
		port_num_bits port_num);
/* used for a 'source-specific multicast' group */
void groupsock__delete__group_eid(groupsock_group_eid_t *group_eid);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_group_eid__delete__impl(groupsock_group_eid_t *group_eid);

/*
 * Functions in header file
 */
struct in_addr const *groupsock_group_eid__group_address(groupsock_group_eid_t *group_eid);
struct in_addr const *groupsock_group_eid__source_filter_address(groupsock_group_eid_t *group_eid);
port_num_bits groupsock_group_eid__port_num(groupsock_group_eid_t *group_eid);
u_int8_t groupsock_group_eid__ttl(groupsock_group_eid_t *group_eid);

/*
 * Normal functions
 */
bool groupsock_group_eid__is_ssm(groupsock_group_eid_t *group_eid);
void groupsock_group_eid__init(groupsock_group_eid_t *group_eid,
		struct in_addr const *group_addr,
		struct in_addr const *source_filter_addr,
		port_num_bits port_num,
		u_int8_t ttl);

#endif
