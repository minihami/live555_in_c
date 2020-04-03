
#include "generic_media_server.h"
#include "uv.h"
#include "../groupsock/groupsock_helper.h"
#include "../groupsock/inet.h"

static void on_close_cb(uv_handle_t *handle)
{
	fprintf(stderr, "server_socket_closed %s()\n", __func__);
	uv_tcp_t *tcp_handle;
	
	tcp_handle = (uv_tcp_t *)handle;

	if (tcp_handle) {
		free(tcp_handle);
		tcp_handle = NULL;
	}
}
/*****************************************************************
 * GenericMediaServer:Medium (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_t *__livemedia_generic_media_server__alloc(void)
{
	livemedia_generic_media_server_t *generic_media_server;
	generic_media_server = (livemedia_generic_media_server_t *)calloc(1, sizeof(livemedia_generic_media_server_t));
	if (generic_media_server == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_generic_media_server_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return generic_media_server;
}
void __livemedia_generic_media_server__init(
		livemedia_generic_media_server_t *generic_media_server,
		uv_tcp_t *our_socket, groupsock_port_t our_port, unsigned int reclamation_seconds)
{
	__livemedia_medium__init(&generic_media_server->medium);

	generic_media_server->server_socket = our_socket;
	our_socket->data = (void *)generic_media_server;
	generic_media_server->server_port = our_port;
	generic_media_server->reclamation_seconds = reclamation_seconds;
	generic_media_server->server_media_sessions = livemedia_hash_table__create__static(
			STRING_HASH_KEYS);
	generic_media_server->client_connections = livemedia_hash_table__create__static(
			ONE_WORD_HASH_KEYS);
	generic_media_server->client_sessions = livemedia_hash_table__create__static(
			STRING_HASH_KEYS);
	generic_media_server->previous_client_session_id = 0;
	
	groupsock__ignore_sig_pipe_on_socket(generic_media_server->server_socket);
	/* so that clients on the same host that are killed don't also kill us */
	/* TODO: background handling??
	 * env.taskScheduler().turnOnBackgroundReadHandling(
	 * fServerSocket, incomingConnectionHandler, this); */
	
	generic_media_server->lookup_server_media_session = 
		livemedia_generic_media_server__lookup_server_media_session__impl;
	generic_media_server->remove_server_media_session_by_stream_name = 
		livemedia_generic_media_server__remove_server_media_session_by_stream_name__impl;
	generic_media_server->close_all_client_sessions_for_server_media_session_by_stream_name = 
		livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_stream_name__impl;
	generic_media_server->delete_server_media_session_by_stream_name = 
		livemedia_generic_media_server__delete_server_media_session_by_stream_name__impl;
	generic_media_server->medium.delete = livemedia_generic_media_server__delete__impl;
}
void __livemedia_generic_media_server__deinit(livemedia_generic_media_server_t *generic_media_server)
{
	/* TODO: background handling??
	 * envir().taskScheduler().turnOffBackgroundReadHandling(fServerSocket); */

	uv_close((uv_handle_t *)generic_media_server->server_socket, on_close_cb);

	__livemedia_medium__deinit(&generic_media_server->medium);
}
void __livemedia_generic_media_server__free(livemedia_generic_media_server_t *generic_media_server)
{
	if (generic_media_server) {
		free(generic_media_server);
		generic_media_server = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_t *livemedia__new__generic_media_server(
		uv_tcp_t *our_socket, groupsock_port_t our_port, unsigned int reclamation_seconds)
{
	livemedia_generic_media_server_t *generic_media_server;

	generic_media_server = __livemedia_generic_media_server__alloc();
	if (generic_media_server)
		__livemedia_generic_media_server__init(generic_media_server, our_socket, our_port, reclamation_seconds);

	return generic_media_server;
}
/*
 * Functions for virtual functions
 */
livemedia_server_media_session_t *livemedia_generic_media_server__lookup_server_media_session(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name,
		bool is_first_lookup_in_session)
{
	if(generic_media_server->lookup_server_media_session)
		return generic_media_server->lookup_server_media_session(generic_media_server,
				stream_name, is_first_lookup_in_session);
	return NULL;
}
void livemedia_generic_media_server__remove_server_media_session_by_stream_name(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name)
{
	if(generic_media_server->remove_server_media_session_by_stream_name)
		return generic_media_server->remove_server_media_session_by_stream_name(
				generic_media_server, stream_name);
	return ;
}
void livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_stream_name(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name) 
{
	if(generic_media_server->close_all_client_sessions_for_server_media_session_by_stream_name)
		return generic_media_server->close_all_client_sessions_for_server_media_session_by_stream_name(
				generic_media_server, stream_name);
	return ;
}
void livemedia_generic_media_server__delete_server_media_session_by_stream_name(	
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name)
{
	if(generic_media_server->delete_server_media_session_by_stream_name)
		return generic_media_server->delete_server_media_session_by_stream_name(
				generic_media_server, stream_name);
	return ;
}
livemedia_generic_media_server_client_connection_t *livemedia_generic_media_server__create_new_client_connection(
		livemedia_generic_media_server_t *generic_media_server, uv_tcp_t *client_socket,
		struct sockaddr_in client_addr)
{
	if(generic_media_server->create_new_client_connection)
		return generic_media_server->create_new_client_connection(generic_media_server,
				client_socket, client_addr);
	return NULL;
}
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__create_new_client_session(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id)
{
	if(generic_media_server->create_new_client_session)
		return generic_media_server->create_new_client_session(generic_media_server,
				session_id);
	return NULL;
}


/*
 * Override functions
 */
livemedia_server_media_session_t *livemedia_generic_media_server__lookup_server_media_session__impl(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name,
		bool is_first_lookup_in_session)
{
	/*TODO:
	return (livemedia_server_media_session_t *)livemedia_server_media_session__lookup(
			stream_name);
			*/
	return NULL;
}
void livemedia_generic_media_server__remove_server_media_session_by_stream_name__impl(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name)
{
	/*TODO:	
	livemedia_generic_media_server__remove_server_media_session_by_server_media_session(
			livemedia_server_media_session__lookup_server_media_session(stream_name));
			*/
	return;
}
void livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_stream_name__impl(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name)
{
	/*TODO:
	livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_server_media_session(
			livemedia_generic_media_server__lookup_server_media_session(stream_name));
			*/
	return;
}
void livemedia_generic_media_server__delete_server_media_session_by_stream_name__impl(	
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name)
{
	/*TODO:
	  livemedia_generic_media_server__delete_server_media_session_by_server_media_session(
	  livemedia_generic_media_server__lookup_server_media_session(stream_name));
	  */
	return;
}
void livemedia_generic_media_server__delete__impl(livemedia_medium_t *medium)
{
	livemedia_generic_media_server_t *generic_media_server;

	generic_media_server = (livemedia_generic_media_server_t *)medium;
	if (generic_media_server) {
		__livemedia_generic_media_server__deinit(generic_media_server);
		__livemedia_generic_media_server__free(generic_media_server);
	}
	return;
}

/*
 * Functions in header file
 */
unsigned int livemedia_generic_media_server__num_client_sessions(
		livemedia_generic_media_server_t *generic_media_server)
{
	return livemedia_hash_table__num_entries(generic_media_server->client_sessions);
}

/*
 * Normal functions
 */
void livemedia_generic_media_server__add_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session)
{
	char const *session_name;
	session_name = livemedia_server_media_session__stream_name(server_media_session);
	if (session_name == NULL)
		session_name = "";
	livemedia_generic_media_server__remove_server_media_session_by_stream_name(
			generic_media_server, session_name);
	livemedia_hash_table__add(generic_media_server->server_media_sessions, session_name,
			(void *)server_media_session);
}
void livemedia_generic_media_server__remove_server_media_session_by_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session)
{
	if (server_media_session == NULL)
		return;
	livemedia_hash_table__remove(generic_media_server->server_media_sessions,
			livemedia_server_media_session__stream_name(server_media_session));
	if (livemedia_server_media_session__reference_count(server_media_session) == 0)
		livemedia_medium__close_by_medium_ptr__static(&server_media_session->medium);
	else
	/*	livemedia_server_media_session__delete_when_unreferenced(server_media_session)
			= true; */
	 server_media_session->delete_when_unreferenced = true;
}
void livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session)
{
	livemedia_hash_table_iterator_t *iter;
	livemedia_generic_media_server_client_session_t *client_session;
	char const *key;

	if (server_media_session == NULL)
		return;
	iter = livemedia_hash_table_iterator__create__static(
			generic_media_server->client_sessions);
	while ((client_session = (livemedia_generic_media_server_client_session_t *)(
					livemedia_hash_table_iterator__next(iter, &key))) != NULL) {
		if (client_session->our_server_media_session == server_media_session) 
			livemedia__delete__generic_media_server_client_session(client_session);
	}
	livemedia__delete__hash_table_iterator(iter);
}
void livemedia_generic_media_server__delete_server_media_session_by_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session)
{
	if (server_media_session == NULL)
		return;

	livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_server_media_session(
			generic_media_server, server_media_session);
	livemedia_generic_media_server__remove_server_media_session_by_server_media_session(
			generic_media_server, server_media_session);
}
void livemedia_generic_media_server__cleanup(livemedia_generic_media_server_t *generic_media_server)
{
	livemedia_generic_media_server_client_session_t *client_session;
	livemedia_generic_media_server_client_connection_t *connection;
	livemedia_server_media_session_t *server_media_session;

	while ((client_session = (livemedia_generic_media_server_client_session_t *)livemedia_hash_table__get_first(generic_media_server->client_sessions)) != NULL) {
		livemedia__delete__generic_media_server_client_session(client_session);
	}
	livemedia__delete__hash_table(generic_media_server->client_sessions);

	while ((connection = (livemedia_generic_media_server_client_connection_t *)livemedia_hash_table__get_first(generic_media_server->client_connections)) != NULL) {
		livemedia__delete__generic_media_server_client_connection(connection);
	}
	livemedia__delete__hash_table(generic_media_server->client_connections);

	while ((server_media_session = (livemedia_server_media_session_t *)livemedia_hash_table__get_first(generic_media_server->server_media_sessions)) != NULL) {
		livemedia_generic_media_server__remove_server_media_session_by_server_media_session(generic_media_server, server_media_session);
	}
	livemedia__delete__hash_table(generic_media_server->server_media_sessions);
}

static void on_connection_cb(uv_stream_t *server, int status)
{
	livemedia_generic_media_server__incoming_connection_handler__static(server->data);
}

#define LISTEN_BACKLOG_SIZE 20
uv_tcp_t *livemedia_generic_media_server__set_up_our_socket__static(groupsock_port_t *our_port)
{
	uv_tcp_t *our_socket;

	do {
		our_socket = groupsock__setup_stream_socket(*our_port, true, true);
		if (our_socket == NULL)
			break;
		if (!groupsock__increase_send_buffer_to(our_socket, 50*1024))
			break;
		if (uv_listen((uv_stream_t *)our_socket, LISTEN_BACKLOG_SIZE,
					on_connection_cb) < 0) {
			fprintf(stderr, "listen() failed: %s()", __func__);
			break;
		}
		if (groupsock_port__num(our_port) == 0) {
      /* bind() will have chosen a port for us; return it also: */
			if (!groupsock__get_source_port(our_socket, our_port))
				break; 
		}
		return our_socket;
	} while (0);

	if (our_socket != NULL)
		uv_close((uv_handle_t *)our_socket, on_close_cb);
	return NULL;
}
void livemedia_generic_media_server__incoming_connection_handler__static(void *instance)
{
	livemedia_generic_media_server_t *server;
	server = (livemedia_generic_media_server_t *)instance;
	livemedia_generic_media_server__incoming_connection_handler(server);
}
void livemedia_generic_media_server__incoming_connection_handler(
		livemedia_generic_media_server_t *generic_media_server)
{
	livemedia_generic_media_server__incoming_connection_handler_on_socket(
			generic_media_server, generic_media_server->server_socket);
}
void livemedia_generic_media_server__incoming_connection_handler_on_socket(
		livemedia_generic_media_server_t *generic_media_server, uv_tcp_t *server_socket)
{
	struct sockaddr_in client_addr;
	/*SOCKLEN_T client_addr_len; */
	/*client_addr_len = sizeof(client_addr); */
	uv_tcp_t *client_socket;
	client_socket = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
	if (client_socket == NULL) {
		fprintf(stderr, "malloc(sizeof(uv_tcp_t))"
				"failed in %s()\n", __func__);
		uv_close(client_socket, on_close_cb);
		return;
	}
	uv_tcp_init(uv_default_loop(), client_socket);


	if (uv_accept((uv_stream_t *)server_socket, (uv_stream_t *)client_socket) < 0) {
		fprintf(stderr, "accept faild in %s()\n", __func__);
		return;
	}
	groupsock__ignore_sig_pipe_on_socket(client_socket);
	/* so that clients on the same host that are killed don't also kill us */
	groupsock__make_socket_non_blocking(client_socket);
	groupsock__increase_send_buffer_to(client_socket, 50*1024);
#ifdef DEBUG
	fprintf(stdout, "uv_accept()ed connection form %s in %s()\n",
			groupsock_address_string__val(client_addr), __func__);
#endif

	/* Create a new object for handling this connection: */
	(void)livemedia_generic_media_server__create_new_client_connection(
			generic_media_server, client_socket, client_addr);
}
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__create_new_client_session_with_id(
		livemedia_generic_media_server_t *generic_media_server)
{
	u_int32_t session_id;
	char session_id_str[8+1];
	livemedia_generic_media_server_client_session_t *client_session;

  /* Choose a random (unused) 32-bit integer for the session id
  * (it will be encoded as a 8-digit hex number).  (We avoid choosing session id 0,
  * because that has a special use by some servers.  Similarly, we avoid choosing the same
  * session id twice in a row.) */

	do {
		session_id = (u_int32_t)groupsock__our_random32();
		snprintf(session_id_str, sizeof(session_id_str), "%08X", session_id);
	} while (session_id == 0 || session_id == generic_media_server->previous_client_session_id 
			|| livemedia_generic_media_server__lookup_client_session_by_session_id_str(generic_media_server,
				session_id_str));

	generic_media_server->previous_client_session_id = session_id;
	client_session = livemedia_generic_media_server__create_new_client_session(generic_media_server, session_id);
	if (client_session != NULL)
		livemedia_hash_table__add(generic_media_server->client_sessions, session_id_str, client_session);

	return client_session;
}
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__lookup_client_session_by_session_id(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id)
{
	char session_id_str[8+1];
	snprintf(session_id_str, sizeof(session_id_str), "%08X", session_id);
	return livemedia_generic_media_server__lookup_client_session_by_session_id_str(generic_media_server, session_id_str);
}
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__lookup_client_session_by_session_id_str(
		livemedia_generic_media_server_t *generic_media_server, char const *session_id_str)
{
	return (livemedia_generic_media_server_client_session_t *)livemedia_hash_table__lookup(
			generic_media_server->client_sessions, session_id_str);
}

/*****************************************************************
 * ClientConnection (Function Definition)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_client_connection_t *__livemedia_generic_media_server_client_connection__alloc(
		void)
{
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;
	generic_media_server_client_connection = (livemedia_generic_media_server_client_connection_t *)calloc(1,
			sizeof(livemedia_generic_media_server_client_connection_t));
	if (generic_media_server_client_connection == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_generic_media_server_client_connection_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return generic_media_server_client_connection;
}

void generic_media_server_client_connection_alloc_cb(uv_handle_t *handle, 
		size_t suggested_size, uv_buf_t *buf)
{
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;
	generic_media_server_client_connection = 
		(livemedia_generic_media_server_client_connection_t *)handle->data;

	suggested_size = generic_media_server_client_connection->request_buffer_bytes_left;;
	buf->base = (char *)malloc(suggested_size);
	if (buf->base == NULL) {
		fprintf(stderr, "malloc(1, sizeof(suggested_size))"
				"failed in %s()\n", __func__);
		uv_close(handle, on_close_cb);
	}
	buf->len = suggested_size;
}
void generic_media_server_client_connection_read_cb(uv_stream_t *stream,
		ssize_t nread, const uv_buf_t *buf)
{
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;
	unsigned char *ptr;

	generic_media_server_client_connection = 
		(livemedia_generic_media_server_client_connection_t *)stream->data;

	ptr = &generic_media_server_client_connection->
		request_buffer[generic_media_server_client_connection->request_bytes_already_seen];
	memcpy(ptr, buf->base, nread);

	livemedia_generic_media_server_client_connection__handle_request_bytes(
			generic_media_server_client_connection, nread);

	free(buf->base);
}
void __livemedia_generic_media_server_client_connection__init(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection, livemedia_generic_media_server_t *our_server, uv_tcp_t *client_socket,
		struct sockaddr_in client_addr)
{
	generic_media_server_client_connection->our_server = our_server;
	generic_media_server_client_connection->our_socket = client_socket;
	generic_media_server_client_connection->client_addr = client_addr;

	livemedia_hash_table__add(generic_media_server_client_connection->our_server->client_connections, 
			(char const *)generic_media_server_client_connection, generic_media_server_client_connection);
	
	livemedia_generic_media_server_client_connection__reset_request_buffer(
			generic_media_server_client_connection);
	generic_media_server_client_connection->our_socket->data = (void *)generic_media_server_client_connection;
	uv_read_start((uv_stream_t *)generic_media_server_client_connection->our_socket,
			generic_media_server_client_connection_alloc_cb,
			generic_media_server_client_connection_read_cb);

	/* virtual functions configuration */ 

	generic_media_server_client_connection->delete = livemedia_generic_media_server_client_connection__delete__impl;
}
void __livemedia_generic_media_server_client_connection__deinit(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	/* Remove ourself from the server's 'client connections' hash table before we go: */
	livemedia_hash_table__remove(generic_media_server_client_connection->our_server->client_connections,
			(char const *)generic_media_server_client_connection);

	livemedia_generic_media_server_client_connection__close_sockets(
			generic_media_server_client_connection);
}
void __livemedia_generic_media_server_client_connection__free(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	if (generic_media_server_client_connection) {
		free(generic_media_server_client_connection);
		generic_media_server_client_connection = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_client_connection_t *livemedia__new__generic_media_server_client_connection(
		livemedia_generic_media_server_t *our_server, uv_tcp_t *client_socket,
		struct sockaddr_in client_addr)
{
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;

	generic_media_server_client_connection = __livemedia_generic_media_server_client_connection__alloc();
	if (generic_media_server_client_connection)
		__livemedia_generic_media_server_client_connection__init(generic_media_server_client_connection, our_server, client_socket, client_addr);

	return generic_media_server_client_connection;

}
void livemedia__delete__generic_media_server_client_connection(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	if (generic_media_server_client_connection && generic_media_server_client_connection->delete)
		generic_media_server_client_connection->delete(generic_media_server_client_connection);
}
/*
 * Functions for virtual functions
 */
void livemedia_generic_media_server_client_connection__handle_request_bytes(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection,
		int new_bytes_read)
{
	if (generic_media_server_client_connection->handle_request_bytes)
		generic_media_server_client_connection->handle_request_bytes(
				generic_media_server_client_connection, new_bytes_read);
}

/*
 * Override functions
 */
void livemedia_generic_media_server_client_connection__delete__impl(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	if (generic_media_server_client_connection) {
		__livemedia_generic_media_server_client_connection__deinit(
				generic_media_server_client_connection);
		__livemedia_generic_media_server_client_connection__free(
				generic_media_server_client_connection);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_generic_media_server_client_connection__close_sockets(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	uv_read_stop((uv_stream_t *)generic_media_server_client_connection->our_socket);
	uv_close((uv_handle_t *)generic_media_server_client_connection->our_socket, on_close_cb);
	generic_media_server_client_connection->our_socket = NULL;

}
void livemedia_generic_media_server_client_connection__incoming_request_handler__static(
		void* instance)
{
	/* TODO: in read_cb
	livemedia_generic_media_server_client_connection_t *connection;
	connection = (livemedia_generic_media_server_client_connection_t *)instance;
	livemedia_generic_media_server_client_connection__incoming_request_handler(connection);
	*/
}
void livemedia_generic_media_server_client_connection__incoming_request_handler(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	/* TODO: in read_cb */
}
void livemedia_generic_media_server_client_connection__reset_request_buffer(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection)
{
	generic_media_server_client_connection->request_bytes_already_seen = 0;
	generic_media_server_client_connection->request_buffer_bytes_left =
		sizeof(generic_media_server_client_connection->request_buffer);
	
}

/*****************************************************************
 * ClientSession (Function Definition)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_client_session_t *__livemedia_generic_media_server_client_session__alloc(
		void)
{
	livemedia_generic_media_server_client_session_t *generic_media_server_client_session;
	generic_media_server_client_session = (livemedia_generic_media_server_client_session_t *)calloc(1, sizeof(livemedia_generic_media_server_client_session_t));
	if (generic_media_server_client_session == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_generic_media_server_client_session_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return generic_media_server_client_session;
}
void __livemedia_generic_media_server_client_session__init(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session,
		livemedia_generic_media_server_t *our_server, u_int32_t session_id)
{

	generic_media_server_client_session->our_server = our_server;
	generic_media_server_client_session->our_session_id = session_id;
	generic_media_server_client_session->our_server_media_session = NULL;
	generic_media_server_client_session->liveness_check_task = NULL;
	livemedia_generic_media_server_client_session__note_liveness(
			generic_media_server_client_session);

	
	/* virtual functions configuration */ 

	generic_media_server_client_session->delete = livemedia_generic_media_server_client_session__delete__impl;

}
void __livemedia_generic_media_server_client_session__deinit(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session)
{
	char session_id_str[8+1];
	/* TODO: destructor */
	
	/* TODO: uv_timer_t */
	/* Turn off any liveness checking: */
	/* envir().taskScheduler().unscheduleDelayedTask(fLivenessCheckTask); */

	sprintf(session_id_str, "%08X", generic_media_server_client_session->our_session_id);
	livemedia_hash_table__remove(generic_media_server_client_session->our_server->client_sessions, session_id_str);

	if (generic_media_server_client_session->our_server_media_session != NULL) {
		livemedia_server_media_session__decrement_reference_count(
				generic_media_server_client_session->our_server_media_session);
			if (livemedia_server_media_session__reference_count(
						generic_media_server_client_session->our_server_media_session) == 0 &&
					livemedia_server_media_session__delete_when_unreferenced(
						generic_media_server_client_session->our_server_media_session)) {
				livemedia_generic_media_server__remove_server_media_session_by_server_media_session(
						generic_media_server_client_session->our_server,
						generic_media_server_client_session->our_server_media_session);
				generic_media_server_client_session->our_server_media_session = NULL;
			}
	}
}
void __livemedia_generic_media_server_client_session__free(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session)
{
	if (generic_media_server_client_session) {
		free(generic_media_server_client_session);
		generic_media_server_client_session = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_client_session_t *livemedia__new__generic_media_server_client_session(
		livemedia_generic_media_server_t *our_server, u_int32_t session_id)
{
	livemedia_generic_media_server_client_session_t *generic_media_server_client_session;

	generic_media_server_client_session = __livemedia_generic_media_server_client_session__alloc();
	if (generic_media_server_client_session)
		__livemedia_generic_media_server_client_session__init(generic_media_server_client_session,
				our_server, session_id);

	return generic_media_server_client_session;
}
void livemedia__delete__generic_media_server_client_session(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session)
{
	if (generic_media_server_client_session && generic_media_server_client_session->delete)
		generic_media_server_client_session->delete(generic_media_server_client_session);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_generic_media_server_client_session__delete__impl(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (generic_media_server_client_session) {
		__livemedia_generic_media_server_client_session__deinit(generic_media_server_client_session);
		__livemedia_generic_media_server_client_session__free(generic_media_server_client_session);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_generic_media_server_client_session__note_liveness(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session)
{
#ifdef DEBUG
	char const* stream_name;

	stream_name  = (generic_media_server_client_session->our_server_media_session == NULL) ? "???" : 
		livemedia_server_media_session__stream_name(generic_media_server_client_session->our_server_media_session);
	fprintf(stderr, "Client session (id \"%08X\", stream name \"%s\"): Liveness indication\n",
			generic_media_server_client_session->our_session_id, stream_name);
#endif
	if (generic_media_server_client_session->our_server_media_session != NULL)
		livemedia_server_media_session__note_liveness(generic_media_server_client_session->our_server_media_session);
	/*TODO: uv_timer_t*/
	if (generic_media_server_client_session->our_server->reclamation_seconds > 0) {
	}
}
void livemedia_generic_media_server_client_session__note_client_liveness__static(
		livemedia_generic_media_server_client_session_t *client_session)
{
	livemedia_generic_media_server_client_session__note_liveness(client_session);
}
void livemedia_generic_media_server_client_session__liveness_timeout_task__static(
		livemedia_generic_media_server_client_session_t *client_session)
{
  /* If this gets called, the client session is assumed to have timed out, so delete it: */
#ifdef DEBUG
	char const* stream_name;

	stream_name  = (generic_media_server_client_session->our_server_media_session == NULL) ? "???" : 
		livemedia_server_media_session__stream_name(client_session->our_server_media_session);
	fprintf(stderr, "Client session (id \"%08X\", stream name \"%s\") has timed out (due to inactivity)\n",
			client_session->our_session_id, stream_name);
#endif
	client_session->liveness_check_task = NULL;
	livemedia__delete__generic_media_server_client_session(client_session);
}

/*****************************************************************
 * ServerMediaSessionIterator (Function Definition)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_server_media_session_iterator_t *__livemedia_generic_media_server_server_media_session_iterator__alloc(
		void)
{
	livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator;
	generic_media_server_server_media_session_iterator = (livemedia_generic_media_server_server_media_session_iterator_t *)calloc(1, sizeof(livemedia_generic_media_server_server_media_session_iterator_t));
	if (generic_media_server_server_media_session_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_generic_media_server_server_media_session_iterator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return generic_media_server_server_media_session_iterator;
}
void __livemedia_generic_media_server_server_media_session_iterator__init(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator,
		livemedia_generic_media_server_t *server)
{
	generic_media_server_server_media_session_iterator->our_iterator = 
		(server->server_media_sessions == NULL) ? NULL : livemedia_hash_table_iterator__create__static(
				server->server_media_sessions);
	
	/* virtual functions configuration */ 

	generic_media_server_server_media_session_iterator->delete = livemedia_generic_media_server_server_media_session_iterator__delete__impl;
}
void __livemedia_generic_media_server_server_media_session_iterator__deinit(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator)
{
	livemedia__delete__hash_table_iterator(
			generic_media_server_server_media_session_iterator->our_iterator);
}
void __livemedia_generic_media_server_server_media_session_iterator__free(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator)
{
	if (generic_media_server_server_media_session_iterator) {
		free(generic_media_server_server_media_session_iterator);
		generic_media_server_server_media_session_iterator = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_server_media_session_iterator_t *livemedia__new__generic_media_server_server_media_session_iterator(
		livemedia_generic_media_server_t *generic_media_server)
{
	livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator;

	generic_media_server_server_media_session_iterator = __livemedia_generic_media_server_server_media_session_iterator__alloc();
	if (generic_media_server_server_media_session_iterator)
		__livemedia_generic_media_server_server_media_session_iterator__init(
				generic_media_server_server_media_session_iterator, generic_media_server);

	return generic_media_server_server_media_session_iterator;
}
void livemedia__delete__generic_media_server_server_media_session_iterator(livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator)
{
	if (generic_media_server_server_media_session_iterator && generic_media_server_server_media_session_iterator->delete)
		generic_media_server_server_media_session_iterator->delete(generic_media_server_server_media_session_iterator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_generic_media_server_server_media_session_iterator__delete__impl(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator)
{
	if (generic_media_server_server_media_session_iterator) {
		__livemedia_generic_media_server_server_media_session_iterator__deinit(generic_media_server_server_media_session_iterator);
		__livemedia_generic_media_server_server_media_session_iterator__free(generic_media_server_server_media_session_iterator);
	}
}
/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_server_media_session_t *livemedia_generic_media_server_server_media_session_iterator__next(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator)
{
	char const *key;
	if (generic_media_server_server_media_session_iterator->our_iterator == NULL)
		return NULL;

	return (livemedia_server_media_session_t *)(livemedia_hash_table_iterator__next(
				generic_media_server_server_media_session_iterator->our_iterator, &key));
}

/*****************************************************************
 * UserAuthenticationDatabase (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_user_authentication_database_t *__livemedia_user_authentication_database__alloc(void)
{
	livemedia_user_authentication_database_t *user_authentication_database;
	user_authentication_database = (livemedia_user_authentication_database_t *)calloc(1, sizeof(livemedia_user_authentication_database_t));
	if (user_authentication_database == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_user_authentication_database_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return user_authentication_database;
}
void __livemedia_user_authentication_database__init(
		livemedia_user_authentication_database_t *user_authentication_database,
		char const *realm, bool passwords_are_md5)
{

	user_authentication_database->table =
		livemedia_hash_table__create__static(STRING_HASH_KEYS);
	user_authentication_database->realm =
		livemedia__str_dup(realm == NULL ? "WEBGATE Streaming Media" : realm);
	user_authentication_database->passwords_are_md5 = passwords_are_md5;
	/* TODO: virtual functions configuration */ 
	user_authentication_database->add_user_record =
		livemedia_user_authentication_database__add_user_record__impl;
	user_authentication_database->remove_user_record =
		livemedia_user_authentication_database__remove_user_record__impl;
	user_authentication_database->lookup_password =
		livemedia_user_authentication_database__lookup_password__impl;
	user_authentication_database->delete =
		livemedia_user_authentication_database__delete__impl;
}
void __livemedia_user_authentication_database__deinit(livemedia_user_authentication_database_t *user_authentication_database)
{
	char *password;

	if (user_authentication_database->realm) {
		free(user_authentication_database->realm);
		user_authentication_database->realm = NULL;
	}
	/* Delete the allocated 'password' strings that we stored in the table,
	 * and then the table itself: */
	while ((password = (char *)livemedia_hash_table__remove_next(
					user_authentication_database->table)) != NULL) {
		if (password) {
			free(password);
			password = NULL;
		}
	}
	livemedia__delete__hash_table(user_authentication_database->table);
}
void __livemedia_user_authentication_database__free(livemedia_user_authentication_database_t *user_authentication_database)
{
	if (user_authentication_database) {
		free(user_authentication_database);
		user_authentication_database = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_user_authentication_database_t *livemedia__new__user_authentication_database(
		char const *realm, bool passwords_are_md5)
{
	livemedia_user_authentication_database_t *user_authentication_database;

	user_authentication_database = __livemedia_user_authentication_database__alloc();
	if (user_authentication_database)
		__livemedia_user_authentication_database__init(user_authentication_database,
				realm, passwords_are_md5);

	return user_authentication_database;
}
void livemedia__delete__user_authentication_database(livemedia_user_authentication_database_t *user_authentication_database)
{
	if (user_authentication_database && user_authentication_database->delete)
		user_authentication_database->delete(user_authentication_database);
}
/*
 * Functions for virtual functions
 */
void livemedia_user_authentication_database__add_user_record(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username, char const *password)
{
	if (user_authentication_database && user_authentication_database->add_user_record)
		user_authentication_database->add_user_record(user_authentication_database,
				username, password);
}
void livemedia_user_authentication_database__remove_user_record(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username)
{
	if (user_authentication_database && user_authentication_database->remove_user_record)
		user_authentication_database->remove_user_record(user_authentication_database,
				username);
}
char const *livemedia_user_authentication_database__lookup_password(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username)
{
	if (user_authentication_database && user_authentication_database->lookup_password)
		user_authentication_database->lookup_password(user_authentication_database,
				username);
}
/* returns NULL if the user name was not present */
void livemedia_user_authentication_database__delete(
		struct _livemedia_user_authentication_database_t *user_authentication_database)
{
	if (user_authentication_database && user_authentication_database->delete)
		user_authentication_database->delete(user_authentication_database);
}

/*
 * Override functions
 */
void livemedia_user_authentication_database__add_user_record__impl(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username, char const *password)
{
	char *old_password;
	old_password = (char *)livemedia_hash_table__add(user_authentication_database->table,
			username, (void *)(livemedia__str_dup(password)));
	if (old_password) {
		free(old_password);
		old_password = NULL;
	}
}
void livemedia_user_authentication_database__remove_user_record__impl(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username)
{
	char *password;
	password = (char *)livemedia_hash_table__lookup(user_authentication_database->table,
			username);
	livemedia_hash_table__remove(user_authentication_database->table, username);
	if (password) {
		free(password);
		password = NULL;
	}
}
char const *livemedia_user_authentication_database__lookup_password__impl(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username)
{
	return (char const *)livemedia_hash_table__lookup(user_authentication_database->table,
			username);
}
/* returns NULL if the user name was not present */
void livemedia_user_authentication_database__delete__impl(livemedia_user_authentication_database_t *user_authentication_database)
{
	if (user_authentication_database) {
		__livemedia_user_authentication_database__deinit(user_authentication_database);
		__livemedia_user_authentication_database__free(user_authentication_database);
	}
}

/*
 * Functions in header file
 */
char const *livemedia_user_authentication_database__realm(
		livemedia_user_authentication_database_t *user_authentication_database)
{
	return user_authentication_database->realm;
}
bool livemedia_user_authentication_database__passwords_are_md5(
		livemedia_user_authentication_database_t *user_authentication_database)
{
	return user_authentication_database->passwords_are_md5;
}

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */
