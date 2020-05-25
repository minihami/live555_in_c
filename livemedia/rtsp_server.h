#ifndef __LIVEMEDIA_RTSP_SERVER_H__
#define __LIVEMEDIA_RTSP_SERVER_H__
#include "generic_media_server.h"
#include "../usage_environment/usage_environment_version.h"
#include "rtsp_common.h"

#include "digest_authentication.h"

typedef struct _write_req_t {
	uv_write_t req;
	uv_buf_t buf;
} write_req_t; 
typedef struct _rtsp_server_write_ctx_t {
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection; 
	int num_bytes_remaining;
	int new_bytes_read;
} rtsp_server_write_ctx_t;

/*****************************************************************
 * RTSPServer:GenericMediaServer (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtsp_server_t {
	livemedia_generic_media_server_t generic_media_server;

	uv_tcp_t *http_server_socket; /* for optional RTSP-over-HTTP tunneling */
	groupsock_port_t http_server_port; /* ditto */
	livemedia_hash_table_t *client_connections_for_http_tunneling; 
	/* maps client-supplied 'session cookie' strings to "RTSPClientConnection"s 
	 * (used only for optional RTSP-over-HTTP tunneling) */
	livemedia_hash_table_t *tcp_streaming_database;
	/* maps TCP socket numbers to ids of sessions that are streaming over it (RTP/RTCP-over-TCP) */
	livemedia_hash_table_t *pending_register_or_deregister_requests;
	unsigned int register_or_deregister_request_counter;
	livemedia_user_authentication_database_t *auth_db;

	bool allow_streaming_rtp_over_tcp; /* by default, True */

	/* virtual function pointers */
	char const *(*allowed_command_names)(struct _livemedia_rtsp_server_t *rtsp_server);
	bool (*we_implement_register)(struct _livemedia_rtsp_server_t *rtsp_server, 
			char const *cmd /* "REGISTER" or "DEREGISTER" */,
			char const *proxy_url_suffix, char **response_str);
	/* used to implement "RTSPClientConnection::handleCmd_REGISTER()" */
	/* Note: "responseStr" is dynamically allocated (or NULL), and should be delete[]d after the call */
	void (*implement_cmd_register)(struct _livemedia_rtsp_server_t *rtsp_server, 
			char const *cmd /* "REGISTER" or "DEREGISTER" */,
			char const *url, char const *url_suffix, int socket_to_remote_server, 
			bool deliver_via_tcp, char const *proxy_url_suffix);
	/* used to implement "RTSPClientConnection::handleCmd_REGISTER()" */
	livemedia_user_authentication_database_t *(*get_authentication_database_for_command)(
			struct _livemedia_rtsp_server_t *rtsp_server, char const *cmd_name);
	bool (*special_client_access_check)(struct _livemedia_rtsp_server_t *rtsp_server,
			uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
			char const* url_suffix);
	/* a hook that allows subrtsp_server_rtsp_client_connection_params_for_registered servers to do server-specific access checking
	 * on each client (e.g., based on client ip address), without using digest authentication. */
	bool (*special_client_user_access_check)(struct _livemedia_rtsp_server_t *rtsp_server,
			uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
			char const* url_suffix, char const *username);
	/* another hook that allows subrtsp_server_rtsp_client_connection_params_for_registered servers to do server-specific access checking
	 * - this time after normal digest authentication has already taken place (and would otherwise allow access).
	 * (This test can only be used to further restrict access, not to grant additional access.) */

} livemedia_rtsp_server_t;

/*****************************************************************
 * RTSPClientConnection:GenericMediaServer::ClientConnection (Structure Declarations)
 ****************************************************************/

struct _livemedia_rtsp_server_rtsp_client_connection_params_for_register_t;

typedef struct _livemedia_rtsp_server_rtsp_client_connection_t {
	livemedia_generic_media_server_client_connection_t generic_media_server_client_connection;

	livemedia_rtsp_server_t *our_rtsp_server; /* same as ::fOurServer */
	uv_tcp_t *client_input_socket; /* aliased to ::fOurSocket */
	uv_tcp_t *client_output_socket;
	bool is_active;
	unsigned char *last_crlf;
	unsigned int *recursion_count;
	char const *current_cseq;
	livemedia_authenticator_t current_authenticator; 
	/* used if access control is needed */
	char *our_session_cookie; /* used for optional RTSP-over-HTTP tunneling */
	unsigned int base64_remainder_count; /* used for optional RTSP-over-HTTP tunneling (possible values: 0,1,2,3) */

	/* virtual function pointers */
	void (*handle_cmd_options)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_cmd_get_parameter)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			char const *full_request_str);
	void (*handle_cmd_set_parameter)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
			char const *full_request_str);
	void (*handle_cmd_describe)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
			char const *url_pre_suffix, char const *url_suffix, char const *full_request_str);
	void (*handle_cmd_register)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			char const *cmd /*"REGISTER" or "DEREGISTER"*/,
			char const *url, char const *url_suffix, char const *full_request_str,
			bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix);
	void (*handle_cmd_bad)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_cmd_not_supported)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_cmd_not_found)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_cmd_session_not_found)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_cmd_unsupported_transport)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	/* Support for optional RTSP-over-HTTP tunneling: */
	bool (*parse_http_request_string)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			char *result_cmd_name, unsigned int result_cmd_name_max_size,
			char *url_suffix, unsigned int url_suffix_max_size,
			char *session_cookie, unsigned int session_cookie_max_size,
			char *accept_str, unsigned int accept_str_max_size);
	void (*handle_http_cmd_not_supported)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_http_cmd_not_found)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_http_cmd_options)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
	void (*handle_http_cmd_tunneling_get)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			char const *session_cookie);
	bool (*handle_http_cmd_tunneling_post)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size);
	void (*handle_http_cmd_streaming_get)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			char const *url_suffix, char const *full_request_str);

	void (*continue_handling_register1)(
			struct _livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
			struct _livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params);
} livemedia_rtsp_server_rtsp_client_connection_t;

/*****************************************************************
 * ParamsForREGISTER (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtsp_server_rtsp_client_connection_params_for_register_t {

	char const *cmd;
	livemedia_rtsp_server_rtsp_client_connection_t *our_connection;
	char *url;
	char *url_suffix;
	bool reuse_connection;
	bool deliver_via_tcp;
	char *proxy_url_suffix;
	/*  virtual function pointers */
	void (*delete)(
			struct _livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);

} livemedia_rtsp_server_rtsp_client_connection_params_for_register_t;

/*****************************************************************
 * RTSPClientSession:GenericMediaServer::ClientSession (Structure Declarations)
 ****************************************************************/
/* The state of an individual client session (using one or more sequential TCP connections) handled by a RTSP server: */
typedef struct _livemedia_rtsp_server_rtsp_client_session_t {
	livemedia_generic_media_server_client_session_t generic_media_server_client_session;

	livemedia_rtsp_server_t *our_rtsp_server; /* same as ::fOurServer */
	bool is_multicast;
	bool stream_after_setup;
	unsigned char tcp_stream_id_count; /* used for (optional) RTP/TCP */
	unsigned num_stream_states;
	struct stream_state_t {
		livemedia_server_media_subsession_t *subsession;
		uv_tcp_t *tcp_socket_num;
		void *stream_token;
	} *stream_states;

	/* virtual function pointers */
	void (*handle_cmd_setup)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			char *url_pre_suffix, char *url_suffix, char const *full_request_str);
	void (*handle_cmd_within_session)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
			char const *full_request_str);
	void (*handle_cmd_tear_down)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			livemedia_server_media_subsession_t *subsession);
	void (*handle_cmd_play)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			livemedia_server_media_subsession_t *subsession, char const *full_request_str);
	void (*handle_cmd_pause)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			livemedia_server_media_subsession_t *subsession);
	void (*handle_cmd_get_parameter)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			livemedia_server_media_subsession_t *subsession, char const *full_request_str);
	void (*handle_cmd_set_parameter)(
			struct _livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
			livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
			livemedia_server_media_subsession_t *subsession, char const *full_request_str);

} livemedia_rtsp_server_rtsp_client_session_t;

/*****************************************************************
 * RTSPServerWithREGISTERProxying:RTSPServer (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtsp_server_with_register_proxying_t {
	livemedia_rtsp_server_t rtsp_server;

	bool stream_rtp_over_tcp;
	int verbosity_level_for_proxying;
	unsigned int registered_proxy_counter;
	char *allowed_command_names;
	livemedia_user_authentication_database_t *auth_db_for_register;
	char *back_end_username;
	char *back_end_password;

} livemedia_rtsp_server_with_register_proxying_t;

/*****************************************************************
 * RTSPServer:GenericMediaServer (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_t *__livemedia_rtsp_server__alloc(void);
void __livemedia_rtsp_server__init(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *our_socket, groupsock_port_t our_port, 
		livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds);
void __livemedia_rtsp_server__deinit(livemedia_rtsp_server_t *rtsp_server);
void __livemedia_rtsp_server__free(livemedia_rtsp_server_t *rtsp_server);
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_t *livemedia__new__rtsp_server(
		uv_tcp_t *our_socket, groupsock_port_t our_port, 
		livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds);
/*
 * Functions for virtual functions
 */
char const *livemedia_rtsp_server__allowed_command_names(livemedia_rtsp_server_t *rtsp_server);
bool livemedia_rtsp_server__we_implement_register(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *proxy_url_suffix, char **response_str);
/* used to implement "RTSPClientConnection::handleCmd_REGISTER()" */
/* Note: "responseStr" is dynamically allocated (or NULL), and should be delete[]d after the call */
void livemedia_rtsp_server__implement_cmd_register(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *url, char const *url_suffix, int socket_to_remote_server, 
		bool deliver_via_tcp, char const *proxy_url_suffix);
/* used to implement "RTSPClientConnection::handleCmd_REGISTER()" */
livemedia_user_authentication_database_t *livemedia_rtsp_server__get_authentication_database_for_command(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name);
bool livemedia_rtsp_server__special_client_access_check(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username);
/* a hook that allows subrtsp_server_rtsp_client_connection_params_for_registered servers to do server-specific access checking
 * on each client (e.g., based on client ip address), without using digest authentication. */
bool livemedia_rtsp_server__special_client_user_access_check(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username);

/*
 * Override functions
 */
char const *livemedia_rtsp_server__allowed_command_names__impl(livemedia_rtsp_server_t *rtsp_server);
bool livemedia_rtsp_server__we_implement_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *proxy_url_suffix, char **response_str);
/* used to implement "RTSPClientConnection::handleCmd_REGISTER()" */
/* Note: "responseStr" is dynamically allocated (or NULL), and should be delete[]d after the call */
void livemedia_rtsp_server__implement_cmd_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *url, char const *url_suffix, int socket_to_remote_server, 
		bool deliver_via_tcp, char const *proxy_url_suffix);
/* used to implement "RTSPClientConnection::handleCmd_REGISTER()" */
livemedia_user_authentication_database_t *livemedia_rtsp_server__get_authentication_database_for_command__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name);
bool livemedia_rtsp_server__special_client_access_check__impl(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix);
/* a hook that allows subrtsp_server_rtsp_client_connection_params_for_registered servers to do server-specific access checking
 * on each client (e.g., based on client ip address), without using digest authentication. */
bool livemedia_rtsp_server__special_client_user_access_check__impl(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username);
livemedia_generic_media_server_client_connection_t *livemedia_rtsp_server__create_new_client_connection__impl(
		livemedia_generic_media_server_t *generic_media_server,
		uv_tcp_t *client_socket, struct sockaddr_in client_addr);
livemedia_generic_media_server_client_session_t *livemedia_rtsp_server__create_new_client_session__impl(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id);
bool livemedia_rtsp_server__is_rtsp_server__impl(livemedia_medium_t *medium);
void livemedia_rtsp_server__delete__impl(livemedia_medium_t *medium);


/*
 * Functions in header file
 */
void livemedia_rtsp_server__disable_stream_rtp_over_tcp(livemedia_rtsp_server_t *rtsp_server);

/*
 * Normal functions
 */
livemedia_rtsp_server_t *livemedia_rtsp_server__create_new__static(
		groupsock_port_t *our_port, livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds);
/* If ourPort.num() == 0, we'll choose the port number
 * Note: The caller is responsible for reclaiming "authDatabase"
 * If "reclamationSeconds" > 0, then the "RTSPClientSession" state for
 *     each client will get reclaimed (and the corresponding RTP stream(s)
 *     torn down) if no RTSP commands - or RTCP "RR" packets - from the
 *     client are received in at least "reclamationSeconds" seconds. */
bool livemedia_rtsp_server__lookup_by_name__static(char const *name,
		livemedia_rtsp_server_t **result_server);
typedef void (response_handler_for_register)(livemedia_rtsp_server_t *rtsp_server,
		unsigned int request_id, int result_code, char *result_string);
unsigned int livemedia_rtsp_server__register_stream(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *server_media_session,
		char const *remote_client_name_or_address, port_num_bits remote_client_port_num,
		response_handler_for_register *response_handler,
		char const *username, char const *password,
		bool receive_our_stream_via_tcp,
		char const *proxy_url_suffix);
/* 'Register' the stream represented by "serverMediaSession" with the given remote client (specifed by name and port number).
 * This is done using our custom "REGISTER" RTSP command.
 * The function returns a unique number that can be used to identify the request; this number is also passed to "responseHandler".
 * When a response is received from the remote client (or the "REGISTER" request fails), the specified response handler
 *   (if non-NULL) is called.  (Note that the "resultString" passed to the handler was dynamically allocated,
 *   and should be delete[]d by the handler after use.)
 * If "receiveOurStreamViaTCP" is True, then we're requesting that the remote client access our stream using RTP/RTCP-over-TCP.
 *   (Otherwise, the remote client may choose regular RTP/RTCP-over-UDP streaming.)
 * "proxyURLSuffix" (optional) is used only when the remote client is also a proxy server.
 *   It tells the proxy server the suffix that it should use in its "rtsp://" URL (when front-end clients access the stream) */
typedef void (response_handler_for_deregister)(livemedia_rtsp_server_t *rtsp_server,
		unsigned int request_id, int result_code, char *result_string);
unsigned int livemedia_rtsp_server__deregister_stream(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *server_media_session,
		char const *remote_client_name_or_address, port_num_bits remote_client_port_num,
		response_handler_for_register *response_handler,
		char const *username, char const *password,
		char const *proxy_url_suffix);
/* Used to turn off a previous "registerStream()" - using our custom "DEREGISTER" RTSP command. */
char *livemedia_rtsp_server__rtsp_url(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *server_media_session, uv_tcp_t *client_socket);
/* returns a "rtsp://" URL that could be used to access the
 * specified session (which must already have been added to
 * us using "addServerMediaSession()".
 * This string is dynamically allocated; caller should delete[]
 * (If "clientSocket" is non-negative, then it is used (by calling "getsockname()") to determine
 *  the IP address to be used in the URL.) */
char *livemedia_rtsp_server__rtsp_url_prefix(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket);
/* like "rtspURL()", except that it returns just the common prefix used by
 * each session's "rtsp://" URL.
 * This string is dynamically allocated; caller should delete[] */
livemedia_user_authentication_database_t *livemedia_rtsp_server__set_authentication_database(
		livemedia_rtsp_server_t *rtsp_server, livemedia_user_authentication_database_t *new_db);
/* Changes the server's authentication database to "newDB", returning a pointer to the old database (if there was one).
 *"newDB" may be NULL (you can use this to disable authentication at runtime, if desired). */
bool livemedia_rtsp_server__set_up_tunneling_over_http(livemedia_rtsp_server_t *rtsp_server, 
		groupsock_port_t http_port);
/* (Attempts to) enable RTSP-over-HTTP tunneling on the specified port.
 * Returns True iff the specified port can be used in this way (i.e., it's not already being used for a separate HTTP server).
 * Note: RTSP-over-HTTP tunneling is described in
 *  http://mirror.informatimago.com/next/developer.apple.com/quicktime/icefloe/dispatch028.html
 *  and http://images.apple.com/br/quicktime/pdf/QTSS_Modules.pdf */
port_num_bits livemedia_rtsp_server__http_server_port_num(
		livemedia_rtsp_server_t *rtsp_server);	/* in host byte order.  (Returns 0 if not present.) */
void livemedia_rtsp_server__incoming_connection_handler_http__static(void *instance);
void livemedia_rtsp_server__incoming_connection_handler_http(livemedia_rtsp_server_t *rtsp_server);
void livemedia_rtsp_server__note_tcp_streaming_on_socket(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *socket_num, livemedia_rtsp_server_rtsp_client_session_t *client_session, unsigned int track_num);
void livemedia_rtsp_server__unnote_tcp_streaming_on_socket(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *socket_num, livemedia_rtsp_server_rtsp_client_session_t *client_session, unsigned int track_num);
void livemedia_rtsp_server__stop_tcp_streaming_on_socket(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *socket_num);

/*****************************************************************
 * RTSPClientConnection:GenericMediaServer::ClientConnection (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_rtsp_client_connection_t *__livemedia_rtsp_server_rtsp_client_connection__alloc(void);
void __livemedia_rtsp_server_rtsp_client_connection__init(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_t *our_server, uv_tcp_t *client_socket, struct sockaddr_in client_addr);
void __livemedia_rtsp_server_rtsp_client_connection__deinit(livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void __livemedia_rtsp_server_rtsp_client_connection__free(livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_rtsp_client_connection_t *livemedia__new__rtsp_server_rtsp_client_connection(
		livemedia_rtsp_server_t *our_server, uv_tcp_t *client_socket, struct sockaddr_in client_addr);
void livemedia__delete__rtsp_server_rtsp_client_connection(livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
/*
 * Functions for virtual functions
 */
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_options(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_get_parameter(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_set_parameter(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
		char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url, char const *url_suffix, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd /*"REGISTER" or "DEREGISTER"*/,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
/* Support for optional RTSP-over-HTTP tunneling: */
bool livemedia_rtsp_server_rtsp_client_connection__parse_http_request_string(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char *result_cmd_name, unsigned int result_cmd_name_max_size,
		char *url_suffix, unsigned int url_suffix_max_size,
		char *session_cookie, unsigned int session_cookie_max_size,
		char *accept_str, unsigned int accept_str_max_size);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie);
bool livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_suffix, char const *full_request_str);

void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params);

/*
 * Override functions
 */
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_options__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_get_parameter__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_set_parameter__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
		char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_pre_suffix, char const *url_suffix, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd /*"REGISTER" or "DEREGISTER"*/,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
/* Support for optional RTSP-over-HTTP tunneling: */
bool livemedia_rtsp_server_rtsp_client_connection__parse_http_request_string__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char *result_cmd_name, unsigned int result_cmd_name_max_size,
		char *url_suffix, unsigned int url_suffix_max_size,
		char *session_cookie, unsigned int session_cookie_max_size,
		char *accept_str, unsigned int accept_str_max_size);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie);
bool livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size);
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_suffix, char const *full_request_str);

void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params);
void livemedia_rtsp_server_rtsp_client_connection__handle_request_bytes__impl(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection, int new_bytes_read);
void livemedia_rtsp_server_rtsp_client_connection__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_rtsp_server_rtsp_client_connection__reset_request_buffer(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__close_socket_rtsp(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection);
void livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte__static(
		void *instance, u_int8_t request_byte);
void livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte1(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		u_int8_t request_byte);
bool livemedia_rtsp_server_rtsp_client_connection__authentication_ok(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd_name, char const * url_suffix, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_connection__change_client_input_socket(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		int new_socket_num, unsigned char const *extra_data, unsigned int extra_data_size);
      /* used to implement RTSP-over-HTTP tunneling */
void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register__static(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params);

    /* Shortcuts for setting up a RTSP response (prior to sending it): */
void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str);
void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_session_id(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, u_int32_t session_id);
void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_content_str(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, char const *content_str);
void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_session_id_and_content_str(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, u_int32_t session_id, char const *content_str);


/*****************************************************************
 * ParamsForREGISTER (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *__livemedia_rtsp_server_rtsp_client_connection_params_for_register__alloc(void);
void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__init(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register,
		char const *cmd/*"REGISTER" or "DEREGISTER"*/,
		livemedia_rtsp_server_rtsp_client_connection_t *our_connection,
		char const *url, char const *url_suffix,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix);

void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__deinit(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);
void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__free(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *livemedia__new__rtsp_server_rtsp_client_connection_params_for_register(
		char const *cmd/*"REGISTER" or "DEREGISTER"*/,
		livemedia_rtsp_server_rtsp_client_connection_t *our_connection,
		char const *url, char const *url_suffix,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix);
void livemedia__delete__rtsp_server_rtsp_client_connection_params_for_register(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);
/*
 * Functions for virtual functions
 */
	void livemedia__delete__rtsp_server_rtsp_client_connection_params_for_register(
			livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);

/*
 * Override functions
 */
void livemedia_rtsp_server_rtsp_client_connection_params_for_register__delete__impl(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * RTSPClientSession:GenericMediaServer::ClientSession (Function Declarations)
 ****************************************************************/
/* The state of an individual client session (using one or more sequential TCP connections) handled by a RTSP server: */
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_rtsp_client_session_t *__livemedia_rtsp_server_rtsp_client_session__alloc(void);
void __livemedia_rtsp_server_rtsp_client_session__init(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_t *our_server, u_int32_t session_id);
void __livemedia_rtsp_server_rtsp_client_session__deinit(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session);
void __livemedia_rtsp_server_rtsp_client_session__free(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session);
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_rtsp_client_session_t *livemedia__new__rtsp_server_rtsp_client_session(
		livemedia_rtsp_server_t *our_server, u_int32_t session_id);
void livemedia__delete__rtsp_server_rtsp_client_session(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session);
/*
 * Functions for virtual functions
 */
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char *url_pre_suffix, char *url_suffix, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
		char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_play(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str);

/*
 * Override functions
 */
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char *url_pre_suffix, char *url_suffix, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
		char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_play__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str);
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str);

void livemedia_rtsp_server_rtsp_client_session__delete__impl(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session);

/*
 * Functions in header file
 */
bool livemedia_rtsp_server_rtsp_client_session__is_multicast(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection);
bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str);
bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_session_id(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, u_int32_t session_id);
bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_content_str(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, char const *content_str);
bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_session_id_and_content_str(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, u_int32_t session_id, char const *content_str);
bool livemedia_rtsp_server_rtsp_client_session__uses_tcp_transport(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection);

/*
 * Normal functions
 */
void livemedia_rtsp_server_rtsp_client_session__delete_stream_by_track(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		unsigned int track_num);
void livemedia_rtsp_server_rtsp_client_session__reclaim_stream_states(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection);

/*****************************************************************
 * RTSPServerWithREGISTERProxying:RTSPServer (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_with_register_proxying_t *__livemedia_rtsp_server_with_register_proxying__alloc(void);
void __livemedia_rtsp_server_with_register_proxying__init(
		livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying,
		groupsock_port_t our_port, livemedia_user_authentication_database_t *auth_database,
		livemedia_user_authentication_database_t *auth_database_for_register,
		unsigned int reclamation_seconds,
		bool stream_rtp_over_tcp,
		int verbosity_level_for_proxying,
		char const *back_end_username,
		char const *back_end_password);
void __livemedia_rtsp_server_with_register_proxying__deinit(livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying);
void __livemedia_rtsp_server_with_register_proxying__free(livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying);
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_with_register_proxying_t *livemedia__new__rtsp_server_with_register_proxying(
		groupsock_port_t our_port, livemedia_user_authentication_database_t *auth_database,
		livemedia_user_authentication_database_t *auth_database_for_register,
		unsigned int reclamation_seconds,
		bool stream_rtp_over_tcp,
		int verbosity_level_for_proxying,
		char const *back_end_username,
		char const *back_end_password);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char const *livemedia_rtsp_server_with_register_proxying__allowed_command_names__impl(livemedia_rtsp_server_t *rtsp_server);
bool livemedia_rtsp_server_with_register_proxying__we_implement_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *proxy_url_suffix, char **response_str);
void livemedia_rtsp_server_with_register_proxying__implement_cmd_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *url, char const *url_suffix, int socket_to_remote_server, 
		bool deliver_via_tcp, char const *proxy_url_suffix);
livemedia_user_authentication_database_t *livemedia_rtsp_server_with_register_proxying__get_authentication_database_for_command__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name);
void livemedia_rtsp_server_with_register_proxying__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
static livemedia_rtsp_server_with_register_proxying_t *livemedia_rtsp_server_with_register_proxying__create_new__static(
		groupsock_port_t our_port, 
		livemedia_user_authentication_database_t *auth_database,
		livemedia_user_authentication_database_t *auth_database_for_register,
		unsigned int reclamation_seconds,
		bool stream_rtp_over_tcp,
		int verbosity_level_for_proxying,
		char const *back_end_username,
		char const *back_end_password);

/* 
 * Non-rtsp_server_rtsp_client_connection_params_for_register functions
 */

void livemedia__parse_transport_header_for_register(char const* buf, /* in */
		bool *reuse_connection, /* out */
		bool *deliver_via_tcp, /* out */
		char **procxy_url_suffix); /* out */

#endif
