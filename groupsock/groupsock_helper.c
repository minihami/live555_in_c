#include "groupsock_helper.h"
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(__MINGW32__)
#include <time.h>
extern "C" int initializeWinsockIfNecessary();
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

static int groupsock__create_socket(int type)
{
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

bool groupsock__make_socket_non_blocking(int sock)
{
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

bool groupsock__set_socket_keep_alive(int sock) {
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
			close(socket);
			return NULL;
		}

	}
  /* Set the keep alive mechanism for the TCP socket, to avoid "ghost sockets" 
   *that remain after an interrupted communication. */
	if (set_keep_alive) {
		if (!groupsock__set_socket_keep_alive(new_socket)) {
			fprintf(stderr, "failed to set keep alive in %s()\n", __func__);
			close(new_socket);
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
void groupsock__ignore_sig_pipe_on_socket(uv_tcp_t *socket_num)
{
	return ;
}
static unsigned int __groupsock__increase_buffer_to(int buf_opt_name, uv_tcp_t *socket, 
		unsigned int requested_size)
{
	return 0;
}
unsigned int groupsock__increase_send_buffer_to(uv_tcp_t *socket, unsigned int requested_size)
{
	return 0;
}
unsigned int groupsock__increase_receive_buffer_to(uv_tcp_t *socket, unsigned int requested_size)
{
	return 0;
}
static bool groupsock__get_source_port0(uv_tcp_t *socket, 
		port_num_bits* result_port_num/*host order*/) {
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
bool groupsock__get_source_port(uv_tcp_t *socket, groupsock_port_t *port)
{
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
net_address_bits groupsock__our_ip_address(void)
{
	return 0;
}

