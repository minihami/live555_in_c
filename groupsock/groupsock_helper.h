#ifndef __GROUPSOCK_GROUPSOCK_HELPER_H__
#define __GROUPSOCK_GROUPSOCK_HELPER_H__
#include "net_address.h"
#include "uv.h"

uv_udp_t *groupsock__setup_datagram_socket(groupsock_port_t port);
uv_tcp_t *groupsock__setup_stream_socket(groupsock_port_t port, bool make_non_blocking,
		bool set_keep_alive);

int groupsock__read_socket(int socket, unsigned char *buffer,
		unsigned int buffer_size, struct sockaddr_in *from_address);
bool groupsock__write_socket_by_ttl_arg(int socket, struct in_addr address,
		port_num_bits port_num /*network byte order */,
		u_int8_t ttl_arg, unsigned char *buffer, unsigned int buffer_size);
bool groupsock__write_socket(int socket, struct in_addr address,
		port_num_bits port_num /*network byte order */,
		unsigned char *buffer, unsigned int buffer_size);
    /* An optimized version of "writeSocket" that omits the "setsockopt()" call to set the TTL. */

void groupsock__ignore_sig_pipe_on_socket(int socket_num);

unsigned int groupsock__get_send_buffer_size(int socket);
unsigned int groupsock__get_receive_buffer_size(int socket);
unsigned int groupsock__set_send_buffer_to(int socket, unsigned int requested_size);
unsigned int groupsock__set_receive_buffer_to(int socket, unsigned int requested_size);
unsigned int groupsock__increase_send_buffer_to(int socket, unsigned int requested_size);
unsigned int groupsock__increase_receive_buffer_to(int socket, unsigned int requested_size);

bool groupsock__make_socket_non_blocking(int sock);
bool groupsock__make_socket_non_blocking_by_writing_timeout(int sock, 
		unsigned int write_timeout_in_milliseconds);
  /* A "writeTimeoutInMilliseconds" value of 0 means: Don't timeout */
bool groupsock__set_socket_keep_alive(int socket);

bool groupsock__socket_join_group(uv_udp_t *socket, net_address_bits group_address);
bool groupsock__socket_leave_group(uv_udp_t *socket, net_address_bits group_address);

/* source-specific multicast join/leave */
bool groupsock__socket_join_group_ssm(uv_udp_t *socket, net_address_bits group_address,
		net_address_bits source_filter_addr);
bool groupsock__socket_leave_group_ssm(uv_udp_t *socket, net_address_bits group_address,
		net_address_bits source_filter_addr);

bool groupsock__get_source_port(int socket, groupsock_port_t *port);

net_address_bits groupsock__our_ip_address(void);/* in network order */

/* IP addresses of our sending and receiving interfaces.  (By default, these */
/* are INADDR_ANY (i.e., 0), specifying the default interface.) */
extern net_address_bits sending_interface_addr;
extern net_address_bits receiving_interface_addr;

/* Allocates a randomly-chosen IPv4 SSM (multicast) address: */
net_address_bits groupsock__choose_random_ip_v4_ssm_address(void);

/* returns a simple "hh:mm:ss"string, for use in debugging output (e.g.) */
char const *groupsock__timestamp_string(void);




#ifdef HAVE_SOCKADDR_LEN
#define SET_SOCKADDR_SIN_LEN(var) var.sin_len = sizeof var
#else
#define SET_SOCKADDR_SIN_LEN(var)
#endif

#define MAKE_SOCKADDR_IN(var,adr,prt) /*adr,prt must be in network order*/\
    struct sockaddr_in var;\
    var.sin_family = AF_INET;\
    var.sin_addr.s_addr = (adr);\
    var.sin_port = (prt);\
    SET_SOCKADDR_SIN_LEN(var);


/* By default, we create sockets with the SO_REUSE_* flag set.
* If, instead, you want to create sockets without the SO_REUSE_* flags,
* Then enclose the creation code with:
*          {
*            NoReuse dummy;
*            ...
*          } */
/*****************************************************************
 * NoReuse (Structure Declarations)
 ****************************************************************/
typedef struct _groupsock_no_reuse_t {
	/* TODO: parent structure */

	/* TODO: member variables */

	/* TODO: virtual function pointers */

	void (*delete)(struct _groupsock_no_reuse_t *no_reuse);
} groupsock_no_reuse_t;

/*****************************************************************
 * Class:Superno_reuse (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_no_reuse_t *__groupsock_no_reuse__alloc(void);
void __groupsock_no_reuse__init(groupsock_no_reuse_t *no_reuse);
void __groupsock_no_reuse__deinit(groupsock_no_reuse_t *no_reuse);
void __groupsock_no_reuse__free(groupsock_no_reuse_t *no_reuse);
/*
 * new() and delete() functions
 */
groupsock_no_reuse_t *groupsock__new__no_reuse(void);
void groupsock__delete__no_reuse(groupsock_no_reuse_t *no_reuse);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_no_reuse__delete__impl(groupsock_no_reuse_t *no_reuse);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */


struct _groupsock_priv { /*There should be only one of these allocated */
	livemedia_hash_table_t *socket_table;
	int reuse_flag;
};
struct _groupsock_priv *groupsock__groupsock_priv(void); /* allocates it if necessary */
void groupsock__reclaim_groupsock_priv(void);

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(__MINGW32__)
// For Windoze, we need to implement our own gettimeofday()
extern int gettimeofday(struct timeval*, int*);
#else
#include <sys/time.h>
#endif

/* the following are implemented in inet.c: */
net_address_bits our_inet_addr(char const *);
void our_srandom(int x);
long our_random(void);
u_int32_t our_random32(void);
/* because "our_random()" returns a 31-bit number */

#endif
