#ifndef __LIVEMEDIA_GENERIC_MEDIA_SERVER_H__
#define __LIVEMEDIA_GENERIC_MEDIA_SERVER_H__

#include "hash_table.h"
#include "media.h"
#include "server_media_session.h"
#include "../groupsock/net_address.h"

#ifndef REQUEST_BUFFER_SIZE
#define REQUEST_BUFFER_SIZE 20000 // for incoming requests
#endif
#ifndef RESPONSE_BUFFER_SIZE
#define RESPONSE_BUFFER_SIZE 20000
#endif

struct _livemedia_generic_media_server_client_connection_t;
struct _livemedia_generic_media_server_client_session_t;

/*****************************************************************
 * GenericMediaServer:Medium (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_generic_media_server_t {
	livemedia_medium_t medium;

	uv_tcp_t *server_socket; /* int server_socket; */
	groupsock_port_t server_port;
	unsigned int reclamation_seconds;
	livemedia_hash_table_t *server_media_sessions;
	livemedia_hash_table_t *client_connections;
	livemedia_hash_table_t *client_sessions;
	u_int32_t previous_client_session_id;

	livemedia_server_media_session_t *(*lookup_server_media_session)(
			struct _livemedia_generic_media_server_t *generic_media_server,
			char const* stream_name, bool is_first_lookup_in_session);
	void (*remove_server_media_session_by_stream_name)(
			struct _livemedia_generic_media_server_t *generic_media_server,
			char const* stream_name);
	void (*close_all_client_sessions_for_server_media_session_by_stream_name)(
			struct _livemedia_generic_media_server_t *generic_media_server,
			char const* stream_name);
	void (*delete_server_media_session_by_stream_name)(
			struct _livemedia_generic_media_server_t *generic_media_server,
			char const* stream_name);
	struct _livemedia_generic_media_server_client_connection_t *(*create_new_client_connection)(
			struct _livemedia_generic_media_server_t *generic_media_server,
			uv_tcp_t *client_socket, struct sockaddr_in client_addr);
	struct _livemedia_generic_media_server_client_session_t *(*create_new_client_session)(
			struct _livemedia_generic_media_server_t *generic_media_server,
			u_int32_t session_id);

} livemedia_generic_media_server_t;

/*****************************************************************
 * ClientConnection(Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_generic_media_server_client_connection_t {
	livemedia_generic_media_server_t *our_server;
	uv_tcp_t *our_socket;
	struct sockaddr_in client_addr;
	unsigned char request_buffer[REQUEST_BUFFER_SIZE];
	unsigned char response_buffer[RESPONSE_BUFFER_SIZE];
	unsigned int request_bytes_already_seen;
	unsigned int request_buffer_bytes_left;

	void (*handle_request_bytes)(
			struct _livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection,
			int new_bytes_read);
	void (*delete)(
			struct _livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);

} livemedia_generic_media_server_client_connection_t;

/*****************************************************************
 * ClientSession (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_generic_media_server_client_session_t {

	livemedia_generic_media_server_t *our_server;
	u_int32_t our_session_id;
	livemedia_server_media_session_t *our_server_media_session;
	uv_timer_t *liveness_check_task;

	void (*delete)(
			struct _livemedia_generic_media_server_client_session_t *generic_media_server_client_session);

} livemedia_generic_media_server_client_session_t;

/*****************************************************************
 * ServerMediaSessionIterator (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_generic_media_server_server_media_session_iterator_t {
	livemedia_hash_table_iterator_t *our_iterator;

	/* TODO: virtual function pointers */
	void (*delete)(
			struct _livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_media_session_iterator_t);

} livemedia_generic_media_server_server_media_session_iterator_t;

/*****************************************************************
 * UserAuthenticationDatabase (Structure Declarations)
 ****************************************************************/
/* A data structure used for optional user/password authentication: */
typedef struct _livemedia_user_authentication_database_t {
	livemedia_hash_table_t *table;
	char *realm;
	bool passwords_are_md5;

	/* TODO: virtual function pointers */
	/* If "passwordsAreMD5" is True, then each password stored into, or removed from,
	 * the database is actually the value computed
	 * by md5(<username>:<realm>:<actual-password>) */
	void (*add_user_record)(
			struct _livemedia_user_authentication_database_t *user_authentication_database,
			char const *username, char const *password);
	void (*remove_user_record)(
			struct _livemedia_user_authentication_database_t *user_authentication_database,
			char const *username);
	char const *(*lookup_password)(
			struct _livemedia_user_authentication_database_t *user_authentication_database,
			char const *username);
      /* returns NULL if the user name was not present */
	void (*delete)(
			struct _livemedia_user_authentication_database_t *user_authentication_database);
} livemedia_user_authentication_database_t;

/*****************************************************************
 * GenericMediaServer:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_t *__livemedia_generic_media_server__alloc(void);
void __livemedia_generic_media_server__init(
		livemedia_generic_media_server_t *generic_media_server,
		uv_tcp_t *our_socket, groupsock_port_t our_port, unsigned int reclamation_seconds);
void __livemedia_generic_media_server__deinit(
		livemedia_generic_media_server_t *generic_media_server);
void __livemedia_generic_media_server__free(
		livemedia_generic_media_server_t *generic_media_server);
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_t *livemedia__new__generic_media_server(
		uv_tcp_t *our_socket, groupsock_port_t our_port, unsigned int reclamation_seconds);
/*
 * Functions for virtual functions
 */
livemedia_server_media_session_t *livemedia_generic_media_server__lookup_server_media_session(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name,
		bool is_first_lookup_in_session);
void livemedia_generic_media_server__remove_server_media_session_by_stream_name(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name);
void livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_stream_name(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name);
void livemedia_generic_media_server__delete_server_media_session_by_stream_name(	
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name);
livemedia_generic_media_server_client_connection_t *livemedia_generic_media_server__create_new_client_connection(
		livemedia_generic_media_server_t *generic_media_server, uv_tcp_t *client_socket,
		struct sockaddr_in client_addr);
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__create_new_client_session(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id);

/*
 * Override functions
 */
livemedia_server_media_session_t *livemedia_generic_media_server__lookup_server_media_session__impl(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name,
		bool is_first_lookup_in_session);
void livemedia_generic_media_server__remove_server_media_session_by_stream_name__impl(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name);
void livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_stream_name__impl(
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name);
void livemedia_generic_media_server__delete_server_media_session_by_stream_name__impl(	
		livemedia_generic_media_server_t *generic_media_server, char const *stream_name);
void livemedia_generic_media_server__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
unsigned int livemedia_generic_media_server__num_client_sessions(
		livemedia_generic_media_server_t *generic_media_server);

/*
 * Normal functions
 */
void livemedia_generic_media_server__add_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session);
void livemedia_generic_media_server__remove_server_media_session_by_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session);
void livemedia_generic_media_server__close_all_client_sessions_for_server_media_session_by_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session);
void livemedia_generic_media_server__delete_server_media_session_by_server_media_session(
		livemedia_generic_media_server_t *generic_media_server,
		livemedia_server_media_session_t *server_media_session);
void livemedia_generic_media_server__cleanup(livemedia_generic_media_server_t *generic_media_server);
uv_tcp_t *livemedia_generic_media_server__set_up_our_socket__static(groupsock_port_t *our_port);
void livemedia_generic_media_server__incoming_connection_handler__static(void *instance);
void livemedia_generic_media_server__incoming_connection_handler(
		livemedia_generic_media_server_t *generic_media_server);
void livemedia_generic_media_server__incoming_connection_handler_on_socket(
		livemedia_generic_media_server_t *generic_media_server, uv_tcp_t* server_socket);
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__create_new_client_session_with_id(
		livemedia_generic_media_server_t *generic_media_server);
/* Generates a new (unused) random session id, and calls the "createNewClientSession()"
 * virtual function with this session id as parameter.*/
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__lookup_client_session_by_session_id(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id);
livemedia_generic_media_server_client_session_t *livemedia_generic_media_server__lookup_client_session_by_session_id_str(
		livemedia_generic_media_server_t *generic_media_server, char const *session_id_str);

/*****************************************************************
 * ClientConnection (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_client_connection_t *__livemedia_generic_media_server_client_connection__alloc(
		void);
void __livemedia_generic_media_server_client_connection__init(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection,
		livemedia_generic_media_server_t *our_server, uv_tcp_t *client_socket,
		struct sockaddr_in client_addr);
void __livemedia_generic_media_server_client_connection__deinit(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);
void __livemedia_generic_media_server_client_connection__free(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_client_connection_t *livemedia__new__generic_media_server_client_connection(
		livemedia_generic_media_server_t *our_server, uv_tcp_t *client_socket,
		struct sockaddr_in client_addr);
void livemedia__delete__generic_media_server_client_connection(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);
/*
 * Functions for virtual functions
 */
void livemedia_generic_media_server_client_connection__handle_request_bytes(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection,
		int new_bytes_read);

/*
 * Override functions
 */
void livemedia_generic_media_server_client_connection__delete__impl(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_generic_media_server_client_connection__close_sockets(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);
void livemedia_generic_media_server_client_connection__incoming_request_handler__static(
		void* instance);
void livemedia_generic_media_server_client_connection__incoming_request_handler(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);
void livemedia_generic_media_server_client_connection__reset_request_buffer(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection);

/*****************************************************************
 * ClientSession (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_client_session_t *__livemedia_generic_media_server_client_session__alloc(
		void);
void __livemedia_generic_media_server_client_session__init(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session,
		livemedia_generic_media_server_t *our_server, u_int32_t session_id);
void __livemedia_generic_media_server_client_session__deinit(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session);
void __livemedia_generic_media_server_client_session__free(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session);
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_client_session_t *livemedia__new__generic_media_server_client_session(
		livemedia_generic_media_server_t *our_server, u_int32_t session_id);
void livemedia__delete__generic_media_server_client_session(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_generic_media_server_client_session__delete__impl(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_generic_media_server_client_session__note_liveness(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session);
void livemedia_generic_media_server_client_session__note_client_liveness__static(
		livemedia_generic_media_server_client_session_t *client_session);
void livemedia_generic_media_server_client_session__liveness_timeout_task__static(
		livemedia_generic_media_server_client_session_t *client_session);

/*****************************************************************
 * ServerMediaSessionIterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_generic_media_server_server_media_session_iterator_t *__livemedia_generic_media_server_server_media_session_iterator__alloc(
		void);
void __livemedia_generic_media_server_server_media_session_iterator__init(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator,
		livemedia_generic_media_server_t *server);
void __livemedia_generic_media_server_server_media_session_iterator__deinit(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator);
void __livemedia_generic_media_server_server_media_session_iterator__free(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator);
/*
 * new() and delete() functions
 */
livemedia_generic_media_server_server_media_session_iterator_t *livemedia__new__generic_media_server_server_media_session_iterator(
		livemedia_generic_media_server_t *generic_media_server);
void livemedia__delete__generic_media_server_server_media_session_iterator(livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_generic_media_server_server_media_session_iterator__delete__impl(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_server_media_session_t *livemedia_generic_media_server_server_media_session_iterator__next(
		livemedia_generic_media_server_server_media_session_iterator_t *generic_media_server_server_media_session_iterator);

/*****************************************************************
 * UserAuthenticationDatabase (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_user_authentication_database_t *__livemedia_user_authentication_database__alloc(
		void);
void __livemedia_user_authentication_database__init(
		livemedia_user_authentication_database_t *user_authentication_database,
		char const *realm, bool passwords_are_md5);
void __livemedia_user_authentication_database__deinit(
		livemedia_user_authentication_database_t *user_authentication_database);
void __livemedia_user_authentication_database__free(
		livemedia_user_authentication_database_t *user_authentication_database);
/*
 * new() and delete() functions
 */
livemedia_user_authentication_database_t *livemedia__new__user_authentication_database(
		char const *realm, bool passwords_are_md5);

void livemedia__delete__user_authentication_database(livemedia_user_authentication_database_t *user_authentication_database);
/*
 * Functions for virtual functions
 */
void livemedia_user_authentication_database__add_user_record(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username, char const *password);
void livemedia_user_authentication_database__remove_user_record(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username);
char const *livemedia_user_authentication_database__lookup_password(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username);
/* returns NULL if the user name was not present */
void livemedia_user_authentication_database__delete(
		struct _livemedia_user_authentication_database_t *user_authentication_database);

/*
 * Override functions
 */
void livemedia_user_authentication_database__add_user_record__impl(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username, char const *password);
void livemedia_user_authentication_database__remove_user_record__impl(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username);
char const *livemedia_user_authentication_database__lookup_password__impl(
		struct _livemedia_user_authentication_database_t *user_authentication_database,
		char const *username);
/* returns NULL if the user name was not present */
void livemedia_user_authentication_database__delete__impl(
		livemedia_user_authentication_database_t *user_authentication_database);

/*
 * Functions in header file
 */
char const *livemedia_user_authentication_database__realm(
		livemedia_user_authentication_database_t *user_authentication_database);
bool livemedia_user_authentication_database__passwords_are_md5(
		livemedia_user_authentication_database_t *user_authentication_database);

/*
 * Normal functions
 */

/*
 * libuv callback functions
 */

void generic_media_server_client_connection_alloc_cb(uv_handle_t *handle, 
		size_t suggested_size, uv_buf_t *buf);
void generic_media_server_client_connection_read_cb(uv_stream_t *stream,
		ssize_t nread, const uv_buf_t *buf);

#endif
