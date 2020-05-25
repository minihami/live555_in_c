#include "groupsock_helper.h"
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(__MINGW32__)
#include <time.h>
int initializeWinsockIfNecessary();
#else
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#if !defined(_WIN32)
#include <netinet/tcp.h>
#ifdef __ANDROID_NDK__
#include <android/ndk-version.h>
#define ANDROID_OLD_NDK __NDK_MAJOR__ < 17
#endif
#endif
#include <fcntl.h>
#define initializeWinsockIfNecessary() 1
#endif
#if defined(__WIN32__) || defined(_WIN32) || defined(_QNX4)
#else
#include <signal.h>
#define USE_SIGNALS 1
#endif
#include <stdio.h>

/* By default, use INADDR_ANY for the sending and receiving interfaces: */
net_address_bits sending_interface_addr = INADDR_ANY;
net_address_bits receiving_interface_addr = INADDR_ANY;

static void groupsock__socket_err(char const *error_msg)
{
	fprintf(stderr, "%s by %s()\n", error_msg, __func__);
}

/*****************************************************************
 * NoReuse (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

groupsock_no_reuse_t *__groupsock_no_reuse__alloc(void)
{
	groupsock_no_reuse_t *no_reuse;
	no_reuse = (groupsock_no_reuse_t *)calloc(1, sizeof(groupsock_no_reuse_t));
	if (no_reuse == NULL) {
		fprintf(stderr, "calloc(1, sizeof(groupsock_no_reuse_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return no_reuse;
}
void __groupsock_no_reuse__init(groupsock_no_reuse_t *no_reuse)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock__groupsock_priv()->reuse_flag = 0;

	no_reuse->delete = groupsock_no_reuse__delete__impl;
}
void __groupsock_no_reuse__deinit(groupsock_no_reuse_t *no_reuse)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock__groupsock_priv()->reuse_flag = 1;
	groupsock__reclaim_groupsock_priv();
}
void __groupsock_no_reuse__free(groupsock_no_reuse_t *no_reuse)
{
	if (no_reuse) {
		free(no_reuse);
		no_reuse = NULL;
	}
}
/*
 * new() and delete() functions
 */
groupsock_no_reuse_t *groupsock__new__no_reuse(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	groupsock_no_reuse_t *no_reuse;

	no_reuse = __groupsock_no_reuse__alloc();
	if (no_reuse)
		__groupsock_no_reuse__init(no_reuse);

	return no_reuse;
}
void groupsock__delete__no_reuse(groupsock_no_reuse_t *no_reuse)
{
	if (no_reuse && no_reuse->delete)
		no_reuse->delete(no_reuse);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void groupsock_no_reuse__delete__impl(groupsock_no_reuse_t *no_reuse)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (no_reuse) {
		__groupsock_no_reuse__deinit(no_reuse);
		__groupsock_no_reuse__free(no_reuse);
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




struct _groupsock_priv *groupsock__groupsock_priv(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct _groupsock_priv *result;

	if (usage_environment__groupsock_priv == NULL) { /* We need to create it */
		result = (struct _groupsock_priv *)malloc(sizeof(struct _groupsock_priv));
		result->socket_table = NULL;
		result->reuse_flag = 1; /* default value => allow reuse of socket numbers */
		usage_environment__groupsock_priv = result;
	}
	return (struct _groupsock_priv *)usage_environment__groupsock_priv;
}

void groupsock__reclaim_groupsock_priv(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct _groupsock_priv *priv;

	priv = (struct _groupsock_priv *)(usage_environment__groupsock_priv);
	if (priv->socket_table == NULL && priv->reuse_flag == 1 /* default value */) {
		/* We can delete the structure (to save space); it will get created again, if needed: */
		if(priv) {
			free(priv);
			priv = NULL;
		}
		usage_environment__groupsock_priv = NULL;
	}
}

static int groupsock__create_socket(int type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Call "socket()" to create a (IPv4) socket of the specified type.
	 * But also set it to have the 'close on exec' property (if we can) */
	int sock;

#ifdef SOCK_CLOEXEC
	sock = socket(AF_INET, type|SOCK_CLOEXEC, 0);
	if (sock != -1 || errno != EINVAL) return sock;
	// An "errno" of EINVAL likely means that the system wasn't happy with the SOCK_CLOEXEC; fall through and try again without it:
#endif

	sock = socket(AF_INET, type, 0);
#ifdef FD_CLOEXEC
	if (sock != -1) fcntl(sock, F_SETFD, FD_CLOEXEC);
#endif
	return sock;
}

uv_udp_t *groupsock__setup_datagram_socket(groupsock_port_t port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int new_socket;
	int reuse_flag;
	struct _groupsock_priv *groupsock_priv;
	u_int8_t loop;
	char tmp_buffer[100];
	uv_udp_t *udp_sock;

	if (!initializeWinsockIfNecessary()) {
		fprintf(stderr, "Failed to initialize 'winsock' by %s()\n", __func__);
		return NULL;
	}

	new_socket = groupsock__create_socket(SOCK_DGRAM);
	if (new_socket < 0) {
		fprintf(stderr, "unable to create datagram socket by %s()\n", __func__);
		return NULL;
	}

	groupsock_priv = groupsock__groupsock_priv();
	reuse_flag = groupsock_priv->reuse_flag;
	groupsock__reclaim_groupsock_priv();
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR,
				(const char *)&reuse_flag, sizeof(reuse_flag)) < 0) {
		fprintf(stderr, "setsockopt(SO_REUSEADDR) error: ");
		close(new_socket);
		return NULL;
	}
#if defined(__WIN32__) || defined(_WIN32)
	// Windoze doesn't properly handle SO_REUSEPORT or IP_MULTICAST_LOOP
#else
#ifdef SO_REUSEPORT
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEPORT,
				(const char*)&reuse_flag, sizeof(reuse_flag)) < 0) {
		fprintf(stderr, "setsockopt(SO_REUSEPORT) error: ");
		close(new_socket);
		return NULL;
	}
#endif

#ifdef IP_MULTICAST_LOOP
	loop = 1;
	if (setsockopt(new_socket, IPPROTO_IP, IP_MULTICAST_LOOP,
				(const char*)&loop, sizeof (loop)) < 0) {
		fprintf(stderr, "setsockopt(IP_MULTICAST_LOOP) error: ");
		close(new_socket);
		return NULL;
	}
#endif
#endif

	/* Note: Windoze requires binding, even if the port number is 0 */
	net_address_bits addr = INADDR_ANY;
#if defined(__WIN32__) || defined(_WIN32)
#else
	if (groupsock_port__num(&port) != 0 || receiving_interface_addr != INADDR_ANY) {
#endif
		if (groupsock_port__num(&port) == 0)
			addr = receiving_interface_addr;
		MAKE_SOCKADDR_IN(name, addr, groupsock_port__num(&port));
		if (bind(new_socket, (struct sockaddr*)&name, sizeof(name)) != 0) {
			sprintf(tmp_buffer, "bind() error (port number: %d): ",
					ntohs(groupsock_port__num(&port)));
			fprintf(stderr, "%s\n", tmp_buffer);
			close(new_socket);
			return NULL;
		}
#if defined(__WIN32__) || defined(_WIN32)
#else
	}
#endif

	/* Set the sending interface for multicasts, if it's not the default: */
	if (sending_interface_addr != INADDR_ANY) {
		struct in_addr addr;
		addr.s_addr = sending_interface_addr;

		if (setsockopt(new_socket, IPPROTO_IP, IP_MULTICAST_IF,
					(const char*)&addr, sizeof(addr)) < 0) {
			fprintf(stderr, "error setting outgoing multicast interface: ");
			close(new_socket);
			return NULL;
		}
	}

	udp_sock = (uv_udp_t *)malloc(sizeof(uv_udp_t));
	if (udp_sock == NULL) {
		fprintf(stderr, "malloc(sizeof(uv_udp_t)) fail in %s()\n", __func__);
		return NULL;
	}
	uv_udp_init(uv_default_loop(), udp_sock);
	uv_udp_open(udp_sock, new_socket);

	return udp_sock;
}

bool groupsock__make_socket_non_blocking(int sock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
#if defined(__WIN32__) || defined(_WIN32)
	unsigned long arg = 1;
	return ioctlsocket(sock, FIONBIO, &arg) == 0;
#elif defined(VXWORKS)
	int arg = 1;
	return ioctl(sock, FIONBIO, (int)&arg) == 0;
#else
	int cur_flag = fcntl(sock, F_GETFL, 0);
	return fcntl(sock, F_SETFL, cur_flag|O_NONBLOCK) >= 0;
#endif
}
bool groupsock__make_socket_non_blocking_by_writing_timeout(int sock, 
		unsigned int write_timeout_in_milliseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	bool result;
#if defined(__WIN32__) || defined(_WIN32)
	unsigned long arg = 0;
	result = ioctlsocket(sock, FIONBIO, &arg) == 0;
#elif defined(VXWORKS)
	int arg = 0;
	result = ioctl(sock, FIONBIO, (int)&arg) == 0;
#else
	int cur_flag = fcntl(sock, F_GETFL, 0);
	result = fcntl(sock, F_SETFL, cur_flag&(~O_NONBLOCK)) >= 0;
#endif

	if (write_timeout_in_milliseconds > 0) {
#ifdef SO_SNDTIMEO
#if defined(__WIN32__) || defined(_WIN32)
		DWORD msto = (DWORD)write_timeout_in_milliseconds;
		setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&msto, sizeof(msto) );
#else
		struct timeval tv;
		tv.tv_sec = write_timeout_in_milliseconds/1000;
		tv.tv_usec = (write_timeout_in_milliseconds%1000)*1000;
		setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
#endif
#endif
	}

	return result;
}

bool groupsock__set_socket_keep_alive(int sock)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
#if defined(__WIN32__) || defined(_WIN32)
	// How do we do this in Windows?  For now, just make this a no-op in Windows:
#else
	int const keepalive_enabled = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepalive_enabled, 
				sizeof(keepalive_enabled)) < 0) {
		return false;
	}

#ifdef TCP_KEEPIDLE
	int const keepalive_time = 180;
	if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepalive_time, 
				sizeof(keepalive_time)) < 0) {
		return false;
	}
#endif

#ifdef TCP_KEEPCNT
	int const keepalive_count = 5;
	if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepalive_count, 
				sizeof(keepalive_count)) < 0) {
		return false;
	}
#endif

#ifdef TCP_KEEPINTVL
	int const keepalive_interval = 20;
	if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepalive_interval,
				sizeof(keepalive_interval)) < 0) {
		return false;
	}
#endif
#endif

	return true;
}
#ifndef SOCKET_REUSE_FLAG
#define SOCKET_REUSE_FLAG 1
#endif
uv_tcp_t *groupsock__setup_stream_socket(groupsock_port_t port, bool make_non_blocking,
		bool set_keep_alive)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	int new_socket;
	uv_tcp_t *new_tcp_handle;
	int reuse_flag;

	new_socket = groupsock__create_socket(SOCK_STREAM);
	if (new_socket < 0) {
		fprintf(stderr, "unable to create stream socket: ");
		return NULL;
	}
	reuse_flag = SOCKET_REUSE_FLAG;
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&reuse_flag, sizeof(reuse_flag)) < 0) {
		fprintf(stderr, "setsockopt(SO_REUSEADDR) error: ");
		close(new_socket);
		return NULL;
	}
	/* SO_REUSEPORT doesn't really make sense for TCP sockets, so we
	 * normally don't set them.  However, if you really want to do this
	 * #define REUSE_FOR_TCP */
#ifdef REUSE_FOR_TCP
#if defined(__WIN32__) || defined(_WIN32)
	// Windoze doesn't properly handle SO_REUSEPORT
#else
#ifdef SO_REUSEPORT
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEPORT,
				(const char*)&reuse_flag, sizeof (reuse_flag)) < 0) {
		fprintf(stderr, "setsockopt(SO_REUSEPORT) error: ");
		close(newSocket);
		return NULL;
	}
#endif
#endif
#endif

	/* Note: Windoze requires binding, even if the port number is 0 */
#if defined(__WIN32__) || defined(_WIN32)
#else
	if (groupsock_port__num(&port) != 0 || receiving_interface_addr != INADDR_ANY) {
#endif
		MAKE_SOCKADDR_IN(name, receiving_interface_addr, groupsock_port__num(&port));

		SET_SOCKADDR_SIN_LEN(name);

		if (bind(new_socket, (struct sockaddr*)&name, sizeof(name)) != 0) {
			fprintf(stderr, "bind() error (port number: %d) in %s()\n",
					ntohs(groupsock_port__num(&port)), __func__);
			close(new_socket);
			return NULL;
		}

#if defined(__WIN32__) || defined(_WIN32)
#else
	}
#endif


	if (make_non_blocking) {
		if (!groupsock__make_socket_non_blocking(new_socket)){
			fprintf(stderr, "failed to make non-blocking in %s()\n", __func__);
			//close(socket);
			return NULL;
		}

	}
	/* Set the keep alive mechanism for the TCP socket, to avoid "ghost sockets" 
	 *that remain after an interrupted communication. */
	if (set_keep_alive) {
		if (!groupsock__set_socket_keep_alive(new_socket)) {
			fprintf(stderr, "failed to set keep alive in %s()\n", __func__);
			//close(new_socket);
			return NULL;
		}
	}

	new_tcp_handle = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
	if (new_tcp_handle == NULL) {
		fprintf(stderr, "malloc(sizeof(uv_tcp_t)) fail in %s()\n", __func__);
		return NULL;
	}
	uv_tcp_init(uv_default_loop(), new_tcp_handle);
	uv_tcp_open(new_tcp_handle, new_socket);
	return new_tcp_handle;
}

int groupsock__read_socket(int socket, unsigned char *buffer, unsigned int buffer_size,
		struct sockaddr_in *from_address)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO*/
}

bool groupsock__write_socket_by_ttl_arg(int socket, struct in_addr address,
		port_num_bits port_num, u_int8_t ttl_arg, unsigned char *buffer,
		unsigned int buffer_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Before sending, set the socket's TTL: */
#if defined(__WIN32__) || defined(_WIN32)
#define TTL_TYPE int
#else
#define TTL_TYPE u_int8_t
#endif
	TTL_TYPE ttl;

	ttl  = (TTL_TYPE)ttl_arg;
	if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
				(const char *)&ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "setsockopt(IP_MULTICAST_TTL) error: ");
		return false;
	}
	return groupsock__write_socket(socket, address, port_num, buffer, buffer_size);
}

void write_socket_send_cb(uv_udp_send_t *req, int status)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO*/

}

bool groupsock__write_socket(int socket, struct in_addr address,
		port_num_bits port_num, unsigned char *buffer,
		unsigned int buffer_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/*TODO*/
}


void groupsock__ignore_sig_pipe_on_socket(int socket_num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
#ifdef USE_SIGNALS
#ifdef SO_NOSIGPIPE
	int set_option = 1;
	setsockopt(socket_num, SOL_SOCKET, SO_NOSIGPIPE, &set_option, sizeof(set_option));
#else
	signal(SIGPIPE, SIG_IGN);
#endif
#endif
}

static unsigned int groupsock__get_buffer_size(int buf_opt_name, int socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int cur_size;
	SOCKLEN_T size_size = sizeof(cur_size);
	if (getsockopt(socket, SOL_SOCKET, buf_opt_name,
				(char *)&cur_size, &size_size) < 0) {
		fprintf(stderr, "%s error\n", __func__);
		return 0;
	}
	return cur_size;
}

unsigned int groupsock__get_send_buffer_size(int socket)
{
	/*TODO*/
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int groupsock__get_receive_buffer_size(int socket)
{
	/*TODO*/
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

static unsigned int groupsock__set_buffer_to(int buf_opt_name)
{
	/*TODO*/
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int groupsock__set_send_buffer_to(int socket,
		unsigned int requested_size)
{
	/*TODO*/
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int groupsock__set_receive_buffer_to(int socket,
		unsigned int requested_size)
{
	/*TODO*/
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

static unsigned int groupsock__increase_buffer_to(int buf_opt_name, int socket, 
		unsigned int requested_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* First, get the current buffer size.  If it's already at least
	 * as big as what we're requesting, do nothing. */
	unsigned int cur_size;
	
	cur_size = groupsock__get_buffer_size(buf_opt_name, socket);
	/* Next, try to increase the buffer to the requested size,
	 * or to some smaller size, if that's not possible: */
	
	while(requested_size > cur_size) {
		SOCKLEN_T size_size = sizeof(requested_size);
		if (setsockopt(socket, SOL_SOCKET, buf_opt_name, (char*)&requested_size, size_size) >= 0) {
			/* success */
			return requested_size;
		}
		requested_size = (requested_size + cur_size) / 2;
	}
	return groupsock__get_buffer_size(buf_opt_name, socket);
}
unsigned int groupsock__increase_send_buffer_to(int socket, unsigned int requested_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return groupsock__increase_buffer_to(SO_SNDBUF, socket, requested_size);
}
unsigned int groupsock__increase_receive_buffer_to(int socket, unsigned int requested_size)
{
	/*TODO*/
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

static void groupsock__clear_multicast_all_socket_option(int socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
#ifdef IP_MULTICAST_ALL
	/* This option is defined in modern versions of Linux to overcome a bug in the Linux kernel's default behavior.
	 * When set to 0, it ensures that we receive only packets that were sent to the specified IP multicast address,
	 * even if some other process on the same system has joined a different multicast group with the same port number. */
	int multicast_all;
	multicast_all = 0;
	(void)setsockopt(socket, IPPROTO_IP, IP_MULTICAST_ALL, (void*)&multicast_all, sizeof(multicast_all));
	/* Ignore the call's result.  Should it fail, we'll still receive packets (just perhaps more than intended) */
#endif
}
/* BasicUsageEnvironment::GetErrno function */
static int get_errorno() {
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	return WSAGetLastError();
#else
	return errno;
#endif
}

bool groupsock__socket_join_group(uv_udp_t *socket,
		net_address_bits group_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct ip_mreq imr;
	int socket_fd;
	uv_fileno((uv_handle_t *)socket, &socket_fd);

	imr.imr_multiaddr.s_addr = group_address;
	imr.imr_interface.s_addr = receiving_interface_addr;
	if (setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				(const char *)&imr, sizeof(struct ip_mreq)) < 0) {
#if defined(__WIN32__) || defined(_WIN32)
		if (get_errno() != 0) {
			/* That piece-of-shit toy operating system (Windows) sometimes lies
			 * about setsockopt() failing! */
#endif
			fprintf(stderr, "setsockopt(IP_ADD_MEMBERSHIP) error: ");
			return false;
#if defined(__WIN32__) || defined(_WIN32)
		}
#endif
	}

	groupsock__clear_multicast_all_socket_option(socket_fd);
	return true;
}

bool groupsock__socket_leave_group(uv_udp_t *socket, net_address_bits group_address)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct ip_mreq imr;
	int socket_fd;
	uv_fileno((uv_handle_t *)socket, &socket_fd);

	if (!groupsock__is_multicast_address(group_address)) {
		/* ignore this case */
		return true;
	}
	imr.imr_multiaddr.s_addr = group_address;
	imr.imr_interface.s_addr = receiving_interface_addr;
	if (setsockopt(socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
				(const char*)&imr, sizeof (struct ip_mreq)) < 0) {
		return false;
	}

	return true;
}

/* The source-specific join/leave operations require special setsockopt()
 * commands, and a special structure (ip_mreq_source).  If the include files
 * didn't define these, we do so here: */
#if !defined(IP_ADD_SOURCE_MEMBERSHIP)
struct ip_mreq_source {
	struct  in_addr imr_multiaddr;  /* IP multicast address of group */
	struct  in_addr imr_sourceaddr; /* IP address of source */
	struct  in_addr imr_interface;  /* local IP address of interface */
};
#endif

#ifndef IP_ADD_SOURCE_MEMBERSHIP

#ifdef LINUX
#define IP_ADD_SOURCE_MEMBERSHIP   39
#define IP_DROP_SOURCE_MEMBERSHIP 40
#else
#define IP_ADD_SOURCE_MEMBERSHIP   25
#define IP_DROP_SOURCE_MEMBERSHIP 26
#endif

#endif

bool groupsock__socket_join_group_ssm(uv_udp_t *socket,
		net_address_bits group_address, net_address_bits source_filter_addr)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool groupsock__socket_leave_group_ssm(uv_udp_t *socket,
		net_address_bits group_address, net_address_bits source_filter_addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct ip_mreq_source imr;
	int socket_fd;
	uv_fileno((uv_handle_t *)socket, &socket_fd);
	
	if (!groupsock__is_multicast_address(group_address)) {
		/* ignore this case */
		return true;
	}
#if ANDROID_OLD_NDK
	imr.imr_multiaddr = group_address;
	imr.imr_sourceaddr = source_filter_addr;
	imr.imr_interface = receiving_interface_addr;
#else
	imr.imr_multiaddr.s_addr = group_address;
	imr.imr_sourceaddr.s_addr = source_filter_addr;
	imr.imr_interface.s_addr = receiving_interface_addr;
#endif
	if (setsockopt(socket_fd, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP,
				(const char*)&imr, sizeof (struct ip_mreq_source)) < 0) {
		return false;
	}
  return true;
}

static bool groupsock__get_source_port0(int socket, 
		port_num_bits* result_port_num/*host order*/) {
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO 
	  sockaddr_in test; 
	  test.sin_port = 0;
	  SOCKLEN_T len = sizeof test;
	  if (getsockname(socket, (struct sockaddr*)&test, &len) < 0) return false;

	 *result_port_num = ntohs(test.sin_port);
	 return true;
	 */
	return true;
}
bool groupsock__get_source_port(int socket, groupsock_port_t *port)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO 
	  port_num_bits port_num;
	  port_num = 0;
	  if (!getSourcePort0(socket, portNum) || portNum == 0) {
	// Hack - call bind(), then try again:
	MAKE_SOCKADDR_IN(name, INADDR_ANY, 0);
	bind(socket, (struct sockaddr*)&name, sizeof name);

	if (!getSourcePort0(socket, portNum) || portNum == 0) {
	socketErr(env, "getsockname() error: ");
	return False;
	}
	}

	port = Port(portNum);
	return True;
	*/
	return true;
}

static bool groupsock__bad_address_for_us(net_address_bits addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* Check for some possible erroneous addresses: */
  net_address_bits n_addr = htonl(addr);
  return (n_addr == 0x7F000001 /* 127.0.0.1 */
	  || n_addr == 0
	  || n_addr == (net_address_bits)(~0));
}
bool loopback_works = 1;

net_address_bits groupsock__our_ip_address(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 0x9fd9a8c0;
	static net_address_bits our_address = 0;
	struct sockaddr_in from_addr;
	char hostname[100];
	int result;
	groupsock_net_address_list_t *addresses;
	groupsock_net_address_list_iterator_t *iter;
	groupsock_net_address_t *address;
	net_address_bits addr;
	net_address_bits a;
	net_address_bits from;
	groupsock_address_string_t *address_string;
    struct timeval time_now;
	unsigned int seed;

	loopback_works = 0;

	if (receiving_interface_addr != INADDR_ANY) {
		/* Hack: If we were told to receive on a specific interface address, then 
		 * define this to be our ip address: */
		our_address = receiving_interface_addr;
	}

		if (our_address == 0) {
			/* We need to find our source address */
			from_addr.sin_addr.s_addr = 0;

			if (!loopback_works) {
				do {
					/* We couldn't find our address using multicast loopback,
					 * so try instead to look it up directly - 
					 * by first getting our host name, and then resolving this host name */
					hostname[0] = '\0';
					result = gethostname(hostname, sizeof(hostname));
					if (result != 0 || hostname[0] == '\0') {
						fprintf(stderr, "initial gethostname() failed\n");
						break;
					}

					/* Try to resolve "hostname" to an IP address: */
					addresses = groupsock__new__net_address_list_by_hostname(hostname);
					iter = groupsock__new__net_address_list_iterator(addresses);

					/* Take the first address that's not bad: */
					addr = 0;
					while ((address = groupsock_net_address_list_iterator__next_address(iter)) 
							!= NULL) {
						a = *(net_address_bits *)(groupsock_net_address__data(address));
						if (!groupsock__bad_address_for_us(a)) {
							addr = a;
							break;
						}
					}

					/* Assign the address that we found to "fromAddr" (as if the 'loopback' method had worked), to simplify the code below: */
					from_addr.sin_addr.s_addr = addr;

				} while(0);

				groupsock__delete__net_address_list(addresses);
				groupsock__delete__net_address_list_iterator(iter);

				/* Make sure we have a good address: */
				from = from_addr.sin_addr.s_addr;
				if (groupsock__bad_address_for_us(from)) {
					address_string = groupsock__new__address_string_by_net_address_bits(from);
					fprintf(stderr, "This computer has an invalid IP address: %s\n",
							groupsock_address_string__val(address_string));
					groupsock__delete__address_string(address_string);
					from = 0;
				}

				our_address = from;

				/* Use our newly-discovered IP address, and the current time,
				 * to initialize the random number generator's seed: */
				gettimeofday(&time_now, NULL);
				seed = our_address^time_now.tv_sec^time_now.tv_usec;
				our_srandom(seed);
			}
		}
		return our_address;
#if 0

	/*TODO: our_ip_address - not using loopback:loopback_works = 0 */
	static net_address_bits our_address;
	int sock;
	struct in_addr test_addr;
	struct sockaddr_in from_addr;
	groupsock_port_t test_port;
	unsigned char *test_string;
	unsigned int test_string_length;
	struct ip_mreq imr;
#if defined(__WIN32__) || defined(_WIN32)
#define TTL_TYPE int
#else
#define TTL_TYPE u_int8_t
#endif
	TTL_TYPE ttl;
	int bytes_sent;
	char tmp_buf[100];
	fd_set rd_set;
	unsigned int num_fds;
	struct timeval timeout;
	unsigned char read_buffer[20];
	int bytes_read;
	char hostname[100];
	in result;

	sock = -1;

	if (receiving_interface_addr != INADDR_ANY) {
		/* Hack: If we were told to receive on a specific interface address, then 
		 * define this to be our ip address: */
		our_address = receiving_interface_addr;
	}

	if (our_address == 0) {
		/* We need to find our source address */
		from_addr.sin_addr.s_addr = 0;
		/* Get our address by sending a (0-TTL) multicast packet,
		 * receiving it, and looking at the source address used.
		 * (This is kinda bogus, but it provides the best guarantee
		 * that other nodes will think our address is the same as we do.) */
		do {
			loopback_works = 0; /* until we learn otherwise */

#ifndef DISABLE_LOOPBACK_IP_ADDRESS_CHECK
			test_addr.s_addr = our_inet_addr("228.67.43.91"); /* arbitrary */
			test_port = groupsock__new__port(15947); /* ditto */

			sock = groupsock__setup_datagram_socket(test_port);
			if (!initializeWinsockIfNecessary()) {
				fprintf(stderr, "Failed to initialize 'winsock' by %s()\n", __func__);
				sock = -1;
			}

			sock = groupsock__create_socket(SOCK_DGRAM);
			if (sock < 0) {
				fprintf(stderr, "unable to create datagram socket by %s()\n", __func__);
			}

			groupsock_priv = groupsock__groupsock_priv();
			reuse_flag = groupsock_priv->reuse_flag;
			groupsock__reclaim_groupsock_priv();
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
						(const char *)&reuse_flag, sizeof(reuse_flag)) < 0) {
				fprintf(stderr, "setsockopt(SO_REUSEADDR) error: ");
				close(sock);
				sock = -1;
			}
#if defined(__WIN32__) || defined(_WIN32)
			// Windoze doesn't properly handle SO_REUSEPORT or IP_MULTICAST_LOOP
#else
#ifdef SO_REUSEPORT
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
						(const char*)&reuse_flag, sizeof(reuse_flag)) < 0) {
				fprintf(stderr, "setsockopt(SO_REUSEPORT) error: ");
				close(sock);
				sock  = -1;
			}
#endif

#ifdef IP_MULTICAST_LOOP
			loop = 1;
			if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
						(const char*)&loop, sizeof (loop)) < 0) {
				fprintf(stderr, "setsockopt(IP_MULTICAST_LOOP) error: ");
				close(sock);
				sock = -1;
			}
#endif
#endif

			/* Note: Windoze requires binding, even if the port number is 0 */
			net_address_bits addr = INADDR_ANY;
#if defined(__WIN32__) || defined(_WIN32)
#else
			if (groupsock_port__num(port) != 0 || receiving_interface_addr != INADDR_ANY) {
#endif
				if (groupsock_port__num(port) == 0)
					addr = receiving_interface_addr;
				MAKE_SOCKADDR_IN(name, addr, groupsock_port__num(port));
				if (bind(sock, (struct sockaddr*)&name, sizeof(name)) != 0) {
					sprintf(tmp_buffer, "bind() error (port number: %d): ",
							ntohs(groupsock_port__num(port)));
					fprintf(stderr, "%s\n", tmp_buffer);
					close(sock);
					sock = -1;
				}
#if defined(__WIN32__) || defined(_WIN32)
#else
			}
#endif

			/* Set the sending interface for multicasts, if it's not the default: */
			if (sending_interface_addr != INADDR_ANY) {
				addr.s_addr = sending_interface_addr;

				if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF,
							(const char*)&addr, sizeof(addr)) < 0) {
					fprintf(stderr, "error setting outgoing multicast interface: ");
					close(sock);
					sock = -1;
				}
			}
			if (sock < 0)
				break;

			imr.imr_multiaddr.s_addr = test_addr.s_addr;
			imr.imr_interface.s_addr = receiving_interface_addr;
			if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
						(const char *)&imr, sizeof(struct ip_mreq)) < 0) {
#if defined(__WIN32__) || defined(_WIN32)
				if (get_errno() != 0) {
					/* That piece-of-shit toy operating system (Windows) sometimes lies
					 * about setsockopt() failing! */
#endif
					fprintf(stderr, "setsockopt(IP_ADD_MEMBERSHIP) error: ");
					break;
#if defined(__WIN32__) || defined(_WIN32)
				}
#endif
			}
			groupsock__clear_multicast_all_socket_option(sock);

			test_string = "hostIdTest";
			test_string_length = sizeof(test_string);

			/* Before sending, set the socket's TTL: */

			ttl  = (TTL_TYPE)0;
			if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,
						(const char *)&ttl, sizeof(ttl)) < 0) {
				fprintf(stderr, "setsockopt(IP_MULTICAST_TTL) error: ");
				break;
			}
			do {
				MAKE_SOCKADDR_IN(dest, address.s_addr, portNum);
				bytes_sent = sendto(socket, (char*)test_string, test_string_length, 0,
						(struct sockaddr*)&dest, sizeof(dest));
				if (bytes_sent != (int)test_string_length) {
					sprintf(tmp_buf, "writeSocket(%d), sendTo() error: wrote %d bytes instead of %u: ", socket, bytes_sent, bufferSize);
					sprintf(stderr, "%s", tmp_buf);
					break;
				}
			} while (0);

			/* Block until the socket is readable (with a 5-second timeout): */


#endif
			FD_ZERO(&rd_set);
			FD_SET((unsigned)sock, &rd_set);
			num_fds = sock+1;

		} while(0);
		return 0;

	}
#endif
#if 0
TODO: to get our_ip_address using libuv is difficult -
		  using select like live555

		  static net_address_bits our_address;
	  uv_udp_t *sock;
	  struct in_addr test_addr;
	  struct sockaddr_in from_addr;
	  groupsock_port_t test_port;
	  unsigned char *test_string;
	  unsigned int test_string_length;

	  if (receiving_interface_addr != INADDR_ANY) {
		  /* Hack: If we were told to receive on a specific interface address, then 
		   * define this to be our ip address: */
		  our_address = receiving_interface_addr;
	  }

	  if (our_address == 0) {
		  /* We need to find our source address */
		  from_addr.sin_addr.s_addr = 0;

		  /* Get our address by sending a (0-TTL) multicast packet,
		   * receiving it, and looking at the source address used.
		   * (This is kinda bogus, but it provides the best guarantee
		   * that other nodes will think our address is the same as we do.) */
		  do {
			  loopback_works = 0; /* until we learn otherwise */

#ifndef DISABLE_LOOPBACK_IP_ADDRESS_CHECK
			  test_addr.s_addr = our_inet_addr("228.67.43.91"); /* arbitrary */
			  test_port = groupsock__new__port(15947); /* ditto */

			  sock = groupsock__setup_datagram_socket(test_port);
			  if (sock == NULL)
				  break;

			  if (!groupsock__socket_join_group(sock, test_addr.s_addr))
				  break;

			  test_string = "hostIdTest";
			  test_string_length = sizeof(test_string);

			  if (!groupsock__write_socket_by_ttl_arg(sock, test_addr, 
						  groupsock_port__num(test_port), 0,
						  test_string, test_string_length))
				  break;


#endif

		  } while(0);

	  }
	  return 0;
#endif
}


net_address_bits groupsock__choose_random_ip_v4_ssm_address(void)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

char const *groupsock__timestamp_string(void)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(__MINGW32__)
/* For Windoze, we need to implement our own gettimeofday() */

/* used to make sure that static variables in gettimeofday() aren't initialized simultaneously by multiple threads */
static LONG initializeLock_gettimeofday = 0;  

#if !defined(_WIN32_WCE)
#include <sys/timeb.h>
#endif

/*TODO: gettimeofday */

#endif
#undef ANDROID_OLD_NDK
