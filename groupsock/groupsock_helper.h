#ifndef __GROUPSOCK_GROUPSOCK_HELPER_H__
#define __GROUPSOCK_GROUPSOCK_HELPER_H__
#include "net_address.h"
#include "uv.h"

net_address_bits groupsock__our_ip_address(void);/* in network order */

static int groupsock__create_socket(int type);

bool groupsock__make_socket_non_blocking(int sock);
bool groupsock__make_socket_non_blocking_by_writing_timeout(int sock, 
		unsigned int write_timeout_in_milliseconds);
uv_tcp_t *groupsock__setup_stream_socket(groupsock_port_t port, bool make_non_blocking,
		bool set_keep_alive);


void groupsock__ignore_sig_pipe_on_socket(uv_tcp_t *socket_num);
static unsigned int __groupsock__increase_buffer_to(int buf_opt_name, uv_tcp_t *socket, 
		unsigned int requested_size);
unsigned int groupsock__increase_send_buffer_to(uv_tcp_t *socket, unsigned int requested_size);
unsigned int groupsock__increase_receive_buffer_to(uv_tcp_t *socket, unsigned int requested_size);

bool groupsock__get_source_port(uv_tcp_t *socket, groupsock_port_t *port);
net_address_bits groupsock__our_ip_address(void);

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

#endif
