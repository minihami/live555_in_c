#include "rtsp_server.h"
#include "../groupsock/groupsock_helper.h"

#define DEBUG

extern net_address_bits sending_interface_addr;
extern net_address_bits receiving_interface_addr;

/*****************************************************************
 * streamingOverTCPRecord (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_streaming_over_tcp_record_t {
	struct _livemedia_streaming_over_tcp_record_t *next;
	u_int32_t session_id;
	unsigned int track_num;
	/* virtual function pointers */
	void (*delete)(
			struct _livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record);
} livemedia_streaming_over_tcp_record_t;
/*****************************************************************
 * streamingOverTCPRecord (Function Definitions)
 ****************************************************************/
void livemedia_streaming_over_tcp_record__delete__impl(livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record); /* forward */
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_streaming_over_tcp_record_t *__livemedia_streaming_over_tcp_record__alloc(void)
{
	livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record;
	streaming_over_tcp_record = (livemedia_streaming_over_tcp_record_t *)calloc(1, sizeof(livemedia_streaming_over_tcp_record_t));
	if (streaming_over_tcp_record == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_streaming_over_tcp_record_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return streaming_over_tcp_record;
}
void __livemedia_streaming_over_tcp_record__init(
		livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record,
		u_int32_t session_id, unsigned int track_num,
		livemedia_streaming_over_tcp_record_t *next)
{
	streaming_over_tcp_record->next = next;
	streaming_over_tcp_record->session_id = session_id;
	streaming_over_tcp_record->track_num = track_num;

	/* virtual functions configuration */ 
	streaming_over_tcp_record->delete = livemedia_streaming_over_tcp_record__delete__impl;
}
void __livemedia_streaming_over_tcp_record__deinit(livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record)
{
}
void __livemedia_streaming_over_tcp_record__free(livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record)
{
	if (streaming_over_tcp_record) {
		free(streaming_over_tcp_record);
		streaming_over_tcp_record = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_streaming_over_tcp_record_t *livemedia__new__streaming_over_tcp_record(
		u_int32_t session_id, unsigned int track_num,
		livemedia_streaming_over_tcp_record_t *next)
{
	livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record;

	streaming_over_tcp_record = __livemedia_streaming_over_tcp_record__alloc();
	if (streaming_over_tcp_record)
		__livemedia_streaming_over_tcp_record__init(streaming_over_tcp_record,
				session_id, track_num, next);

	return streaming_over_tcp_record;
}
void livemedia__delete__streaming_over_tcp_record(livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record)
{
	if (streaming_over_tcp_record && streaming_over_tcp_record->delete)
		streaming_over_tcp_record->delete(streaming_over_tcp_record);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_streaming_over_tcp_record__delete__impl(livemedia_streaming_over_tcp_record_t *streaming_over_tcp_record)
{
	if (streaming_over_tcp_record) {
		__livemedia_streaming_over_tcp_record__deinit(streaming_over_tcp_record);
		__livemedia_streaming_over_tcp_record__free(streaming_over_tcp_record);
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


/*****************************************************************
 * RTSPServer:GenericMediaServer (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_t *__livemedia_rtsp_server__alloc(void)
{
	livemedia_rtsp_server_t *rtsp_server;
	rtsp_server = (livemedia_rtsp_server_t *)calloc(1, sizeof(livemedia_rtsp_server_t));
	if (rtsp_server == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtsp_server_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtsp_server;
}
static void on_close_cb(uv_handle_t *handle)
{
	fprintf(stderr, "socket_closed %s()\n", __func__);
	uv_tcp_t *tcp_handle;

	tcp_handle = (uv_tcp_t *)handle;

	if (tcp_handle) {
		free(tcp_handle);
		tcp_handle = NULL;
	}
}
void __livemedia_rtsp_server__init(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *our_socket, groupsock_port_t our_port, 
		livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds)
{
	__livemedia_generic_media_server__init(&rtsp_server->generic_media_server,
			our_socket, our_port, reclamation_seconds);

	rtsp_server->http_server_socket = NULL;
	/* TODO: rtsp_server->http_server_port = NULL; */
	rtsp_server->client_connections_for_http_tunneling = NULL;
	rtsp_server->tcp_streaming_database = livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	rtsp_server->pending_register_or_deregister_requests = livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	rtsp_server->register_or_deregister_request_counter = 0;
	rtsp_server->auth_db = auth_database;
	rtsp_server->allow_streaming_rtp_over_tcp = true;

	/* virtual functions configuration */ 
	rtsp_server->allowed_command_names = livemedia_rtsp_server__allowed_command_names__impl;
	rtsp_server->we_implement_register = livemedia_rtsp_server__we_implement_register__impl;
	rtsp_server->implement_cmd_register = livemedia_rtsp_server__implement_cmd_register__impl;
	rtsp_server->get_authentication_database_for_command = livemedia_rtsp_server__get_authentication_database_for_command__impl;
	rtsp_server->special_client_access_check = livemedia_rtsp_server__special_client_access_check__impl;
	rtsp_server->special_client_user_access_check = livemedia_rtsp_server__special_client_user_access_check__impl;
	((livemedia_medium_t *)rtsp_server)->is_rtsp_server = livemedia_rtsp_server__is_rtsp_server__impl;
	((livemedia_generic_media_server_t *)rtsp_server)->create_new_client_connection =
		livemedia_rtsp_server__create_new_client_connection__impl;
	((livemedia_generic_media_server_t *)rtsp_server)->create_new_client_session =
		livemedia_rtsp_server__create_new_client_session__impl;
	rtsp_server->generic_media_server.medium.delete = livemedia_rtsp_server__delete__impl;
}
void __livemedia_rtsp_server__deinit(livemedia_rtsp_server_t *rtsp_server) {
	/* TODO: destructor */
	uv_close((uv_handle_t *)rtsp_server->http_server_socket, on_close_cb);
	livemedia_generic_media_server__cleanup((livemedia_generic_media_server_t *)rtsp_server);
	/* Removes all "ClientSession" and "ClientConnection" objects, and their tables. */
	livemedia__delete__hash_table(rtsp_server->client_connections_for_http_tunneling);

	/* Delete any pending REGISTER requests: */
	/* TODO: RTSPRegisterOrDeregisterSender* r; */
	livemedia_hash_table_t *r;
	while ((r = livemedia_hash_table__get_first(rtsp_server->pending_register_or_deregister_requests)) != NULL)
		livemedia__delete__hash_table(r);
	livemedia__delete__hash_table(rtsp_server->pending_register_or_deregister_requests);

	/* Empty out and close "fTCPStreamingDatabase": */
	livemedia_streaming_over_tcp_record_t *sotcp;
	while ((sotcp = 
				(livemedia_streaming_over_tcp_record_t *)livemedia_hash_table__get_first(
					rtsp_server->tcp_streaming_database)) != NULL) 
		livemedia__delete__hash_table(rtsp_server->tcp_streaming_database);

	__livemedia_generic_media_server__deinit(&rtsp_server->generic_media_server);
}
void __livemedia_rtsp_server__free(livemedia_rtsp_server_t *rtsp_server)
{
	if (rtsp_server) {
		free(rtsp_server);
		rtsp_server = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_t *livemedia__new__rtsp_server(
		uv_tcp_t *our_socket, groupsock_port_t our_port, 
		livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_t *rtsp_server;

	rtsp_server = __livemedia_rtsp_server__alloc();
	if (rtsp_server)
		__livemedia_rtsp_server__init(rtsp_server, our_socket, our_port,
				auth_database, reclamation_seconds);

	return rtsp_server;
}
/*
 * Functions for virtual functions
 */
char const *livemedia_rtsp_server__allowed_command_names(livemedia_rtsp_server_t *rtsp_server)
{
	if (rtsp_server && rtsp_server->allowed_command_names)
		rtsp_server->allowed_command_names(rtsp_server);
}
bool livemedia_rtsp_server__we_implement_register(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd, char const *proxy_url_suffix, char **response_str)
{
	if (rtsp_server && rtsp_server->we_implement_register)
		rtsp_server->we_implement_register(rtsp_server, cmd, proxy_url_suffix, 
				response_str);
}
void livemedia_rtsp_server__implement_cmd_register(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd, char const *url, char const *url_suffix, 
		int socket_to_remote_server, bool deliver_via_tcp, char const *proxy_url_suffix)
{
	if (rtsp_server && rtsp_server->implement_cmd_register)
		rtsp_server->implement_cmd_register(rtsp_server, cmd, url, url_suffix, 
				socket_to_remote_server, deliver_via_tcp, proxy_url_suffix);

}
livemedia_user_authentication_database_t *livemedia_rtsp_server__get_authentication_database_for_command(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name)
{
	if (rtsp_server && rtsp_server->get_authentication_database_for_command)
		rtsp_server->get_authentication_database_for_command(rtsp_server, cmd_name);
}
bool livemedia_rtsp_server__special_client_access_check(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username)
{
	if (rtsp_server && rtsp_server->special_client_access_check)
		rtsp_server->special_client_access_check(rtsp_server, client_socket, client_addr,
				url_suffix);
}
bool livemedia_rtsp_server__special_client_user_access_check(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username)
{
	if (rtsp_server && rtsp_server->special_client_user_access_check)
		rtsp_server->special_client_user_access_check(rtsp_server, client_socket,
				client_addr, url_suffix, username);
}

/*
 * Override functions
 */
char const *livemedia_rtsp_server__allowed_command_names__impl(livemedia_rtsp_server_t *rtsp_server)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER";
}
bool livemedia_rtsp_server__we_implement_register__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd, 
		char const *proxy_url_suffix, char **response_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* in RTSPServer, No Definition */
}
void livemedia_rtsp_server__implement_cmd_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd, char const *url, char const *url_suffix, 
		int socket_to_remote_server, bool deliver_via_tcp, 
		char const *proxy_url_suffix)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* in RTSPServer, No Definition */
}
livemedia_user_authentication_database_t *livemedia_rtsp_server__get_authentication_database_for_command__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation */
	return rtsp_server->auth_db;
}

bool livemedia_rtsp_server__special_client_access_check__impl(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation */
	return true;
}

bool livemedia_rtsp_server__special_client_user_access_check__impl(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* default implementation; no further access restrictions: */
	return true;
}

bool livemedia_rtsp_server__is_rtsp_server__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return true;
}

livemedia_generic_media_server_client_connection_t *livemedia_rtsp_server__create_new_client_connection__impl(
		livemedia_generic_media_server_t *generic_media_server,
		uv_tcp_t *client_socket, struct sockaddr_in client_addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (livemedia_generic_media_server_client_connection_t *)livemedia__new__rtsp_server_rtsp_client_connection(
			(livemedia_rtsp_server_t *)generic_media_server,
			client_socket, client_addr);
}

livemedia_generic_media_server_client_session_t *livemedia_rtsp_server__create_new_client_session__impl(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (livemedia_generic_media_server_client_session_t *)livemedia__new__rtsp_server_rtsp_client_session(
			(livemedia_rtsp_server_t *)generic_media_server, session_id);
}

void livemedia_rtsp_server__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_t *rtsp_server;
	rtsp_server = (livemedia_rtsp_server_t *)medium;
	if (rtsp_server) {
		__livemedia_rtsp_server__deinit(rtsp_server);
		__livemedia_rtsp_server__free(rtsp_server);
	}
}


/*
 * Functions in header file
 */
void livemedia_rtsp_server__disable_stream_rtp_over_tcp(livemedia_rtsp_server_t *rtsp_server)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	rtsp_server->allow_streaming_rtp_over_tcp = false;
}

/*
 * Normal functions
 */
livemedia_rtsp_server_t *livemedia_rtsp_server__create_new__static(
		groupsock_port_t *our_port, livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	uv_tcp_t *our_socket;
	our_socket = livemedia_generic_media_server__set_up_our_socket__static(
			our_port);
	if (our_socket == NULL)
		return NULL;
	return livemedia__new__rtsp_server(our_socket, *our_port, auth_database,
			reclamation_seconds);
}
bool livemedia_rtsp_server__lookup_by_name__static(char const *name,
		livemedia_rtsp_server_t **result_server)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	*result_server = NULL; /* unless we succeed */

	livemedia_medium_t *medium;
	if (!livemedia_medium__lookup_by_name__static(name, &medium)) 
		return false;
	if (!livemedia_medium__is_rtsp_server(medium)) {
		fprintf(stderr, "%s is not a RTSP server in %s()\n", name, __func__);
		return false;
	}

	*result_server = (livemedia_rtsp_server_t *)medium;
	return true;
}
unsigned int livemedia_rtsp_server__register_stream(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *server_media_session,
		char const *remote_client_name_or_address, port_num_bits remote_client_port_num,
		response_handler_for_register *response_handler,
		char const *username, char const *password,
		bool receive_our_stream_via_tcp,
		char const *proxy_url_suffix)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Create a new "RegisterRequestRecord" that will send the "REGISTER" command.
	 * (This object will automatically get deleted after we get a response to the "REGISTER" command, or if we're deleted.) */
	/* TODO: RegisterRequestRecord*/
	return -1;
}

unsigned int livemedia_rtsp_server__deregister_stream(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *server_media_session,
		char const *remote_client_name_or_address, port_num_bits remote_client_port_num,
		response_handler_for_register *response_handler,
		char const *username, char const *password,
		char const *proxy_url_suffix)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Create a new "DeregisterRequestRecord" that will send the "DEREGISTER" command.
	 * (This object will automatically get deleted after we get a response to the "DEREGISTER" command, or if we're deleted.) */
	/* TODO: RegisterRequestRecord*/
	/* TODO */
	return -1;
}

char *livemedia_rtsp_server__rtsp_url(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t *server_media_session, uv_tcp_t *client_socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	char *url_prefix;
	char const *session_name;
	char *result_url;
	url_prefix = livemedia_rtsp_server__rtsp_url_prefix(rtsp_server, client_socket);
	session_name = livemedia_server_media_session__stream_name(server_media_session);
	result_url = (char *)malloc(strlen(url_prefix) + strlen(session_name) + 1);
	if (result_url == NULL) {
		fprintf(stderr, "malloc(strlen(url_prefix) + strlen(session_name) + 1)"
				"failed in %s()\n", __func__);
		return NULL;
	}
	sprintf(result_url, "%s%s", url_prefix, session_name);

	if (url_prefix) {
		free(url_prefix);
		url_prefix = NULL;
	}
	return result_url;
}

char *livemedia_rtsp_server__rtsp_url_prefix(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	struct sockaddr_in our_address;
	SOCKLEN_T namelen;
	char url_buffer[100]; /* more than big enough for "rtsp://<ip-address>:<port>/" */
	port_num_bits port_num_host_order;

	groupsock_address_string_t *address_string;

	if (client_socket == NULL) {
		our_address.sin_addr.s_addr = sending_interface_addr != 0
			? receiving_interface_addr
			: groupsock__our_ip_address(); /* hack */
	} else {
		namelen = sizeof(our_address);
		uv_tcp_getsockname(client_socket, (struct sockaddr*)&our_address, &namelen);
	}
	port_num_host_order = ntohs(groupsock_port__num(&((livemedia_generic_media_server_t *)rtsp_server)->server_port));
	address_string = groupsock__new__address_string_by_sockaddr_in(&our_address);
	if (port_num_host_order == 554 /* the default port number */) {
		sprintf(url_buffer, "rtsp://%s/", groupsock_address_string__val(address_string));
	} else {
		address_string = groupsock__new__address_string_by_sockaddr_in(&our_address);
		sprintf(url_buffer, "rtsp://%s:%hu/", groupsock_address_string__val(
					address_string), port_num_host_order);
	}

	return livemedia__str_dup(url_buffer);
}

livemedia_user_authentication_database_t *livemedia_rtsp_server__set_authentication_database(
		livemedia_rtsp_server_t *rtsp_server, livemedia_user_authentication_database_t *new_db)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_user_authentication_database_t *old_db;
	old_db = rtsp_server->auth_db;
	rtsp_server->auth_db = new_db;
	return old_db;
}

bool livemedia_rtsp_server__set_up_tunneling_over_http(livemedia_rtsp_server_t *rtsp_server, 
		groupsock_port_t http_port)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*setup_our_socket__static is set up uv_tcp_t *,
	 *so, uv_tcp_t * malloc and init and listen
	 * */
	rtsp_server->http_server_socket = 
		livemedia_generic_media_server__set_up_our_socket__static(&http_port);
	if (rtsp_server->http_server_socket != NULL) {
		rtsp_server->http_server_port = http_port;
		return true;
	}
	return false;
}

port_num_bits livemedia_rtsp_server__http_server_port_num(
		livemedia_rtsp_server_t *rtsp_server)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return ntohs(groupsock_port__num(&rtsp_server->http_server_port));
}

void livemedia_rtsp_server__incoming_connection_handler_http__static(void *instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_t *server;
	server = (livemedia_rtsp_server_t *)instance;
	livemedia_rtsp_server__incoming_connection_handler_http(server);
}

void livemedia_rtsp_server__incoming_connection_handler_http(
		livemedia_rtsp_server_t *rtsp_server)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_generic_media_server__incoming_connection_handler_on_socket(
			(livemedia_generic_media_server_t *)rtsp_server,
			rtsp_server->http_server_socket);
}

void livemedia_rtsp_server__note_tcp_streaming_on_socket(
		livemedia_rtsp_server_t *rtsp_server, uv_tcp_t *socket_num,
		livemedia_rtsp_server_rtsp_client_session_t *client_session, 
		unsigned int track_num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_streaming_over_tcp_record_t *sotcp_cur;
	livemedia_streaming_over_tcp_record_t *sotcp_new;
	sotcp_cur = (livemedia_streaming_over_tcp_record_t *)livemedia_hash_table__lookup(
			rtsp_server->tcp_streaming_database, (char const *)socket_num);
	sotcp_new = livemedia__new__streaming_over_tcp_record(((livemedia_generic_media_server_client_session_t *)client_session)->
			our_session_id, track_num, sotcp_cur);
	livemedia_hash_table__add(rtsp_server->tcp_streaming_database,
			(char const *)socket_num, sotcp_new);
}

void livemedia_rtsp_server__unnote_tcp_streaming_on_socket(
		livemedia_rtsp_server_t *rtsp_server, uv_tcp_t *socket_num,
		livemedia_rtsp_server_rtsp_client_session_t *client_session,
		unsigned int track_num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_streaming_over_tcp_record_t *sotcp_head;
	livemedia_streaming_over_tcp_record_t *sotcp;
	livemedia_streaming_over_tcp_record_t *sotcp_prev;

	if (socket_num == NULL)
		return;
	sotcp_head = (livemedia_streaming_over_tcp_record_t *)livemedia_hash_table__lookup(
			rtsp_server->tcp_streaming_database, (char const *)socket_num);
	if (sotcp_head == NULL)
		return;

	/* Look for a record of the (session,track); remove it if found: */
	sotcp = sotcp_head;
	sotcp_prev = sotcp_head;
	do {
		if (sotcp->session_id == ((livemedia_generic_media_server_client_session_t *)client_session)->our_session_id && 
				sotcp->track_num == track_num)
			break;
		sotcp_prev = sotcp;
		sotcp = sotcp->next;
	} while (sotcp != NULL);
	if (sotcp == NULL)
		return; /* not found */

	if (sotcp == sotcp_head) {
		/* We found it at the head of the list.
		 * Remove it and reinsert the tail into the hash table: */
		sotcp_head = sotcp->next;
		sotcp->next = NULL;
		livemedia__delete__streaming_over_tcp_record(sotcp);

		if (sotcp_head == NULL) {
			/* There were no more entries on the list.
			 * Remove the original entry from the hash table: */
			livemedia_hash_table__remove(rtsp_server->tcp_streaming_database,
					(char const *)socket_num);
		} else {
			/* Add the rest of the list into the hash table (replacing the original): */
			livemedia_hash_table__add(rtsp_server->tcp_streaming_database,
					(char const *)socket_num, sotcp_head);
		}
	} else {
		/* We found it on the list, but not at the head.  Unlink it: */
		sotcp_prev->next = sotcp->next;
		sotcp->next = NULL;
		livemedia__delete__streaming_over_tcp_record(sotcp);
	}
}

void livemedia_rtsp_server__stop_tcp_streaming_on_socket(
		livemedia_rtsp_server_t *rtsp_server, uv_tcp_t *socket_num)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Close any stream that is streaming over "socketNum" (using RTP/RTCP-over-TCP streaming): */
	livemedia_streaming_over_tcp_record_t *sotcp;
	livemedia_rtsp_server_rtsp_client_session_t * client_session;
	livemedia_streaming_over_tcp_record_t *sotcp_next;

	sotcp = (livemedia_streaming_over_tcp_record_t *)livemedia_hash_table__lookup(
			rtsp_server->tcp_streaming_database, (char const *)socket_num);
	if (sotcp != NULL) {
		do {
			client_session = (livemedia_rtsp_server_rtsp_client_session_t *)
				livemedia_generic_media_server__lookup_client_session_by_session_id(
						(livemedia_generic_media_server_t *)rtsp_server, sotcp->session_id);
			if (client_session != NULL)
				livemedia_rtsp_server_rtsp_client_session__delete_stream_by_track(
						client_session, sotcp->track_num);

			sotcp_next = sotcp->next;
			sotcp->next = NULL;
			livemedia__delete__streaming_over_tcp_record(sotcp);
			sotcp = sotcp_next;
		} while (sotcp != NULL);
		livemedia_hash_table__remove(rtsp_server->tcp_streaming_database,
				(char const *)socket_num);
	}
}

/*****************************************************************
 * RTSPClientConnection:GenericMediaServer::ClientConnection (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_rtsp_client_connection_t *__livemedia_rtsp_server_rtsp_client_connection__alloc(void)
{
	livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection;
	rtsp_server_rtsp_client_connection = (livemedia_rtsp_server_rtsp_client_connection_t *)calloc(1, sizeof(livemedia_rtsp_server_rtsp_client_connection_t));
	if (rtsp_server_rtsp_client_connection == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtsp_server_rtsp_client_connection_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtsp_server_rtsp_client_connection;
}
void __livemedia_rtsp_server_rtsp_client_connection__init(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_t *our_server, uv_tcp_t *client_socket, struct sockaddr_in client_addr)
{
	__livemedia_generic_media_server_client_connection__init(
			&rtsp_server_rtsp_client_connection->generic_media_server_client_connection,
			(livemedia_generic_media_server_t *)our_server, client_socket, client_addr);

	rtsp_server_rtsp_client_connection->our_rtsp_server = our_server;
	rtsp_server_rtsp_client_connection->client_input_socket = 
		((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->our_socket;
	rtsp_server_rtsp_client_connection->client_output_socket = 
		((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->our_socket;
	rtsp_server_rtsp_client_connection->is_active = true;
	rtsp_server_rtsp_client_connection->recursion_count = 0;
	rtsp_server_rtsp_client_connection->our_session_cookie = NULL;

	livemedia_rtsp_server_rtsp_client_connection__reset_request_buffer(rtsp_server_rtsp_client_connection);

	/* virtual functions configuration */ 
	rtsp_server_rtsp_client_connection->handle_cmd_options = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_options__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_get_parameter = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_get_parameter__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_set_parameter = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_set_parameter__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_describe = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_register = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_bad = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_not_supported = 
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_not_found =
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_session_not_found =
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found__impl;
	rtsp_server_rtsp_client_connection->handle_cmd_unsupported_transport =
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport__impl;
	rtsp_server_rtsp_client_connection->parse_http_request_string =
		livemedia_rtsp_server_rtsp_client_connection__parse_http_request_string__impl;
	rtsp_server_rtsp_client_connection->handle_http_cmd_not_supported =
		livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported__impl;
	rtsp_server_rtsp_client_connection->handle_http_cmd_not_found =
		livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found__impl;
	rtsp_server_rtsp_client_connection-> handle_http_cmd_options = 
		livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options__impl;
	rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_get =
		livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get__impl;
	rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_post = 
		livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post__impl;
	rtsp_server_rtsp_client_connection->handle_http_cmd_streaming_get = 
		livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get__impl;
	rtsp_server_rtsp_client_connection->continue_handling_register1 = 
		livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1__impl;
	rtsp_server_rtsp_client_connection->generic_media_server_client_connection.handle_request_bytes = 
		livemedia_rtsp_server_rtsp_client_connection__handle_request_bytes__impl;

	((livemedia_medium_t *)rtsp_server_rtsp_client_connection)->delete =
		livemedia_rtsp_server_rtsp_client_connection__delete__impl;
}
void __livemedia_rtsp_server_rtsp_client_connection__deinit(livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection->our_session_cookie != NULL) {
		/* We were being used for RTSP-over-HTTP tunneling. Also remove ourselves from the 'session cookie' hash table before we go: */
		livemedia_hash_table__remove(
				rtsp_server_rtsp_client_connection->our_rtsp_server->client_connections_for_http_tunneling,
				rtsp_server_rtsp_client_connection->our_session_cookie);

		if (rtsp_server_rtsp_client_connection->our_session_cookie) {
			free(rtsp_server_rtsp_client_connection->our_session_cookie);
			rtsp_server_rtsp_client_connection->our_session_cookie = NULL;
		}
	}
	livemedia_rtsp_server_rtsp_client_connection__close_socket_rtsp(rtsp_server_rtsp_client_connection);

	__livemedia_generic_media_server_client_connection__deinit(
			&rtsp_server_rtsp_client_connection->generic_media_server_client_connection);

}
void __livemedia_rtsp_server_rtsp_client_connection__free(livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection) {
		free(rtsp_server_rtsp_client_connection);
		rtsp_server_rtsp_client_connection = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_rtsp_client_connection_t *livemedia__new__rtsp_server_rtsp_client_connection(
		livemedia_rtsp_server_t *our_server, uv_tcp_t *client_socket, struct sockaddr_in client_addr)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection;

	rtsp_server_rtsp_client_connection = __livemedia_rtsp_server_rtsp_client_connection__alloc();
	if (rtsp_server_rtsp_client_connection)
		__livemedia_rtsp_server_rtsp_client_connection__init(rtsp_server_rtsp_client_connection,
				our_server, client_socket, client_addr);

	return rtsp_server_rtsp_client_connection;
}

/*
 * Functions for virtual functions
 */
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_options(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_options(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_get_parameter(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *full_request_str)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_get_parameter(
				rtsp_server_rtsp_client_connection, full_request_str);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_set_parameter(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
		char const *full_request_str)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_set_parameter(
				rtsp_server_rtsp_client_connection, full_request_str);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_pre_suffix, char const *url_suffix, char const *full_request_str)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_describe(
				rtsp_server_rtsp_client_connection, url_pre_suffix, url_suffix, full_request_str);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd /*"REGISTER" or "DEREGISTER"*/,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_register(
				rtsp_server_rtsp_client_connection, cmd, url, url_suffix, full_request_str,
				reuse_connection, deliver_via_tcp, proxy_url_suffix);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_bad(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_not_supported(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_not_found(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_session_not_found(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_cmd_unsupported_transport(
				rtsp_server_rtsp_client_connection);
}
/* Support for optional RTSP-over-HTTP tunneling: */
bool livemedia_rtsp_server_rtsp_client_connection__parse_http_request_string(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char *result_cmd_name, unsigned int result_cmd_name_max_size,
		char *url_suffix, unsigned int url_suffix_max_size,
		char *session_cookie, unsigned int session_cookie_max_size,
		char *accept_str, unsigned int accept_str_max_size)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->parse_http_request_string(
				rtsp_server_rtsp_client_connection, result_cmd_name, result_cmd_name_max_size,
				url_suffix, url_suffix_max_size, 
				session_cookie, session_cookie_max_size,
				accept_str, accept_str_max_size);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_http_cmd_not_supported(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_http_cmd_not_found(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_http_cmd_options(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_get(
				rtsp_server_rtsp_client_connection, session_cookie);
}
bool livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_post(
				rtsp_server_rtsp_client_connection, session_cookie, extra_data, extra_data_size);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_suffix, char const *full_request_str)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->handle_http_cmd_streaming_get(
				rtsp_server_rtsp_client_connection, url_suffix, full_request_str);
}

void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params)
{
	if (rtsp_server_rtsp_client_connection)
		rtsp_server_rtsp_client_connection->continue_handling_register1(
				rtsp_server_rtsp_client_connection, params);
}
/*
 * Override functions
 */

/* Handler routines for specific RTSP commands: */
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_options__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	snprintf((char*)((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->response_buffer, 
			sizeof(((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->response_buffer),
			"RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sPublic: %s\r\n\r\n",
			rtsp_server_rtsp_client_connection->current_cseq, livemedia__date_header(), 
			livemedia_rtsp_server__allowed_command_names(rtsp_server_rtsp_client_connection->our_rtsp_server));
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_get_parameter__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* By default, we implement "GET_PARAMETER" (on the entire server) just as a 'no op', and send back a dummy response.
	 * (If you want to handle this type of "GET_PARAMETER" differently, you can do so by defining a subclass of "RTSPServer"
	 * and "RTSPServer::RTSPClientConnection", and then reimplement this virtual function in your subclass.) */

	livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_content_str(
			rtsp_server_rtsp_client_connection, "200 OK", WEBGATE_LIBRARY_VERSION_STRING);

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_set_parameter__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
		char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* By default, we implement "SET_PARAMETER" (on the entire server) just as a 'no op', and send back an empty response.
	 * (If you want to handle this type of "SET_PARAMETER" differently, you can do so by defining a subclass of "RTSPServer"
	 * and "RTSPServer::RTSPClientConnection", and then reimplement this virtual function in your subclass.) */
	livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
			rtsp_server_rtsp_client_connection, "200 OK");
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_pre_suffix, char const *url_suffix, char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_session_t *session;
	char *sdp_description;
	char *rtsp_url;
	char url_total_suffix[2*RTSP_PARAM_STRING_MAX];
	unsigned int sdp_description_size;

	session = NULL;
	sdp_description = NULL;
	rtsp_url = NULL;
	do {
		url_total_suffix[0] = '\0';
		if (url_pre_suffix[0] != '\0') {
			strcat(url_total_suffix, url_pre_suffix);
			strcat(url_total_suffix, "/");
		}
		strcat(url_total_suffix, url_suffix);
		printf("in %s(), url_total_suffix: %s\n", __func__, url_total_suffix);

		if (!livemedia_rtsp_server_rtsp_client_connection__authentication_ok(
					rtsp_server_rtsp_client_connection, "DESCRIBE", url_total_suffix, full_request_str)) {
			break;
		}
		/* We should really check that the request contains an "Accept:" #####
		 * for "application/sdp", because that's what we're sending back #####

		 * Begin by looking up the "ServerMediaSession" object for the specified "urlTotalSuffix": */
		session = livemedia_generic_media_server__lookup_server_media_session(
				(livemedia_generic_media_server_t *)((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->our_server,
				url_total_suffix, true /* empty parameter */);
		if (session == NULL) {
			livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(
					rtsp_server_rtsp_client_connection);
			break;
		}
		/* Increment the "ServerMediaSession" object's reference count, in case someone removes it
		 * while we're using it: */
		livemedia_server_media_session__increment_reference_count(session);

		/* Then, assemble a SDP description for this session: */
		sdp_description = livemedia_server_media_session__generate_sdp_description(
				session);
		printf("in %s(), sdp_description: %s\n", __func__, sdp_description);
		if (sdp_description == NULL) {
			/* This usually means that a file name that was specified for a
			 * "ServerMediaSubsession" does not exist. */
			livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
					rtsp_server_rtsp_client_connection, "404 File Not Found, Or In Incorrect Format");
			break;
		}
		sdp_description_size = strlen(sdp_description);

		/* Also, generate our RTSP URL, for the "Content-Base:" header
		 * (which is necessary to ensure that the correct URL gets used in subsequent "SETUP" requests). */
		rtsp_url = livemedia_rtsp_server__rtsp_url(
				rtsp_server_rtsp_client_connection->our_rtsp_server, session,
				rtsp_server_rtsp_client_connection->client_input_socket);

		snprintf((char*)(((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->response_buffer),
				sizeof(((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->response_buffer),
				"RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
				"%s"
				"Content-Base: %s/\r\n"
				"Content-Type: application/sdp\r\n"
				"Content-Length: %d\r\n\r\n"
				"%s",
				rtsp_server_rtsp_client_connection->current_cseq,
				livemedia__date_header(),
				rtsp_url,
				sdp_description_size,
				sdp_description);
	} while (0);

	if (session != NULL) {
		/* Decrement its reference count, now that we're done using it: */
		livemedia_server_media_session__decrement_reference_count(session);

		if (livemedia_server_media_session__reference_count(session) == 0 &&
				*livemedia_server_media_session__delete_when_unreferenced(session)) {
			livemedia_generic_media_server__remove_server_media_session_by_server_media_session(
					(livemedia_generic_media_server_t *)
					((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->our_server,
					session);
		}
	}
	if (sdp_description) {
		free(sdp_description);
		sdp_description = NULL;
	}
	if (rtsp_url) {
		free(rtsp_url);
		rtsp_url = NULL;
	}
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd /*"REGISTER" or "DEREGISTER"*/,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;
	generic_media_server_client_connection = 
		(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection;

	/* Don't do anything with "fCurrentCSeq", because it might be nonsense */
	snprintf((char*)generic_media_server_client_connection->response_buffer, 
			sizeof(generic_media_server_client_connection->response_buffer),
			"RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
			livemedia__date_header(), 
			livemedia_rtsp_server__allowed_command_names(rtsp_server_rtsp_client_connection->our_rtsp_server));
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;
	generic_media_server_client_connection = 
		(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection;

	snprintf((char*)generic_media_server_client_connection->response_buffer, 
			sizeof(generic_media_server_client_connection->response_buffer),
			"RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n%sAllow: %s\r\n\r\n",
			rtsp_server_rtsp_client_connection->current_cseq,
			livemedia__date_header(), 
			livemedia_rtsp_server__allowed_command_names(rtsp_server_rtsp_client_connection->our_rtsp_server));
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(rtsp_server_rtsp_client_connection, 
			"404 Session Not Found");
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(rtsp_server_rtsp_client_connection, 
			"454 Session Not Found");
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(rtsp_server_rtsp_client_connection, 
			"461 Session Not Found");
}

/* Support for optional RTSP-over-HTTP tunneling: */
bool livemedia_rtsp_server_rtsp_client_connection__parse_http_request_string__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char *result_cmd_name, unsigned int result_cmd_name_max_size,
		char *url_suffix, unsigned int url_suffix_max_size,
		char *session_cookie, unsigned int session_cookie_max_size,
		char *accept_str, unsigned int accept_str_max_size)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false;
}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return false;
}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_suffix, char const *full_request_str)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void rtsp_server_write_cb(uv_write_t *req, int status)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_session_t *client_session;
	bool end_of_msg;
	unsigned char *ptr;
	unsigned int to_index;
	char c;
	int from_index;
	unsigned int num_bytes_to_decode;
	unsigned int new_base64_remainder_count;
	unsigned int decoded_size;
	unsigned char *decoded_bytes;
	unsigned int k;
	unsigned char *to;
	unsigned int i;
	unsigned int j;
	unsigned char *tmp_ptr;
	char cmd_name[RTSP_PARAM_STRING_MAX];
	char url_pre_suffix[RTSP_PARAM_STRING_MAX];
	char url_suffix[RTSP_PARAM_STRING_MAX];
	char cseq[RTSP_PARAM_STRING_MAX];
	char session_id_str[RTSP_PARAM_STRING_MAX];
	unsigned int content_length;
	bool play_after_setup;
	bool parse_succeeded;
	bool request_included_session_id;
	bool are_authenticated;
	char url_total_suffix[2*RTSP_PARAM_STRING_MAX];
	/* enough space for urlPreSuffix/urlSuffix'\0' */
	char *url;
	bool reuse_connection, deliver_via_tcp;
	char *proxy_url_suffix;
	char session_cookies[RTSP_PARAM_STRING_MAX];
	char accept_str[RTSP_PARAM_STRING_MAX];
	bool is_valid_http_cmd;
	unsigned char const *extra_data;
	unsigned int extra_data_size;
	unsigned request_size;
	rtsp_server_write_ctx_t *ctx;
	int new_bytes_read;

	bool break_flag;

	/* TODO: break_flag set before break */
	break_flag = false;
	ctx = (rtsp_server_write_ctx_t *)req->data;
	write_req_t *write_req;
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;
	generic_media_server_client_connection = ctx->generic_media_server_client_connection;
	new_bytes_read = ctx->new_bytes_read;
#if 0
	printf("response buffer: %s\n", generic_media_server_client_connection->response_buffer);
	printf("request buffer: %s\n", generic_media_server_client_connection->request_buffer);
	printf("status: %d num_bytes_remaining: %d\n", status, ctx->num_bytes_remaining);
#endif
	do {
		if (status != 1) {
			/*TODO: RTSPServer.c 887*/

			request_size = 
				(((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->last_crlf
				 + 4 
				 - generic_media_server_client_connection->request_buffer)
				+ content_length;
			ctx->num_bytes_remaining = generic_media_server_client_connection->request_bytes_already_seen - request_size;
			livemedia_rtsp_server_rtsp_client_connection__reset_request_buffer(
					(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection);
			/*to prepare for any subsequence request */

			if (ctx->num_bytes_remaining > 0) {
				memmove(generic_media_server_client_connection->request_buffer,
						&generic_media_server_client_connection->request_buffer[request_size],
						ctx->num_bytes_remaining);
				new_bytes_read = ctx->num_bytes_remaining;
			}
		}
		if (ctx->num_bytes_remaining != 0 || status == 1) {
			client_session = NULL;

			if (new_bytes_read < 0 || (unsigned int )new_bytes_read >=
					generic_media_server_client_connection->request_buffer_bytes_left) {
				// Either the client socket has died, or the request was too big for us.
				// Terminate this connection:
#ifdef DEBUG
				fprintf(stderr, "RTSPClientConnection[%p]::handleRequestBytes() read %d new bytes (of %d);"
						"terminating connection!\n", generic_media_server_client_connection, new_bytes_read, 
						generic_media_server_client_connection->request_buffer_bytes_left);
#endif
				((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->is_active = false;
				break_flag = true;
				break;
			}

			end_of_msg = false;
			ptr = &generic_media_server_client_connection->
				request_buffer[generic_media_server_client_connection->request_bytes_already_seen];
#ifdef DEBUG
			ptr[new_bytes_read] = '\0';
			fprintf(stderr, "RTSPClientConnection[%p]::handleRequestBytes() %s %d new bytes:%s\n",
					generic_media_server_client_connection, ctx->num_bytes_remaining > 0 ? "processing" : "read", 
					new_bytes_read, ptr);
#endif

#if 0
			if (((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->
					client_output_socket != 
					((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->
					client_input_socket && ctx->num_bytes_remaining == 0) {
				/* We're doing RTSP-over-HTTP tunneling, and input commands are assumed to have been Base64-encoded.
				 * We therefore Base64-decode as much of this new data as we can (i.e., up to a multiple of 4 bytes). */

				/* But first, we remove any whitespace that may be in the input data: */
				to_index = 0;
				for (from_index = 0; from_indwx < new_bytes_read; ++from_index) {
					c = ptr[from_index];
					if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n'))  /* not 'whitespace': space,tab,CR,NL */
						ptr[to_index++] = c;
				}
				new_bytes_read = to_index;

				num_bytes_to_decode = ((livemedia_rtsp_server_rtsp_client_connection_t *)
						generic_media_server_client_connection)->base64_remainder_count + new_bytes_read;
				new_base64_remainder_count = num_bytes_to_decode%4;
				num_bytes_to_decode -= new_base64_remainder_count;
				if (num_bytes_to_decode > 0) {
					ptr[new_bytes_read] = '\0';
					decoded_bytes = base64_decode((char const *)(ptr - (
									(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->base64_remainder_count), 
							num_bytes_to_decode, decoded_size);
#ifdef DEBUG
					fprintf(stderr, "Base64-decoded %d input bytes into %d new bytes:", num_bytes_to_decode, decoded_size);
					for (k = 0; k < decoded_size; ++k)
						fprintf(stderr, "%c", decoded_bytes[k]);
					fprintf(stderr, "\n");
#endif

					/* Copy the new decoded bytes in place of the old ones (we can do this because there are fewer decoded bytes than original): */
					to = ptr - ((livemedia_rtsp_server_rtsp_client_connection_t *)
							generic_media_server_client_connection)->base64_remainder_count;
					for (i = 0; i < decoded_size; ++i) 
						*to++ = decoded_bytes[i];

					/* Then copy any remaining (undecoded) bytes to the end: */
					for (j = 0; j < new_base64_remainder_count; ++j)
						*to++ = (ptr - 
								(((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->base64_remainder_count) + num_bytes_to_decode)[j];

					new_bytes_read = decoded_size -
						(((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->base64_remainder_count) + new_base64_remainder_count;
					/* adjust to allow for the size of the new decoded data (+ remainder) */


				}
			}
			/*TODO: RTSP over HTTP  for minimum set */
#endif
			tmp_ptr = ((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->last_crlf + 2;

			if (((livemedia_rtsp_server_rtsp_client_connection_t *)
						generic_media_server_client_connection)->base64_remainder_count == 0) {
				/* no more Base-64 bytes remain to be read/decoded 
				 * Look for the end of the message: <CR><LF><CR><LF> */
				if (tmp_ptr < generic_media_server_client_connection->request_buffer)
					tmp_ptr = generic_media_server_client_connection->request_buffer;
				while (tmp_ptr < &ptr[new_bytes_read - 1]) {
					if (*tmp_ptr == '\r' && *(tmp_ptr + 1) == '\n') {
						if (tmp_ptr - ((livemedia_rtsp_server_rtsp_client_connection_t *)
									generic_media_server_client_connection)->last_crlf == 2) { /* This is it */
							end_of_msg = true;
							break;
						}
						((livemedia_rtsp_server_rtsp_client_connection_t *)
						 generic_media_server_client_connection)->last_crlf = tmp_ptr;
					}
					++tmp_ptr;
				}
			}
			generic_media_server_client_connection->request_buffer_bytes_left -= new_bytes_read;	
			generic_media_server_client_connection->request_bytes_already_seen += new_bytes_read;	

			if (!end_of_msg) {
				break_flag = true;
				break;	/* subsequent reads will be needed to complete the request */
			}

			/* Parse the request string into command name and 'CSeq', then handle the command: */
			generic_media_server_client_connection->
				request_buffer[generic_media_server_client_connection->request_bytes_already_seen] == '\0';
			content_length = 0;
			play_after_setup = false;
			((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->last_crlf[2] = '\0';
			/* temporarily, for parsing */
			parse_succeeded = livemedia__parse_rtsp_request_string(
					(char *)generic_media_server_client_connection->request_buffer,
					((livemedia_rtsp_server_rtsp_client_connection_t *)
					 generic_media_server_client_connection)->last_crlf + 2 - 
					generic_media_server_client_connection->request_buffer,
					cmd_name, sizeof(cmd_name),
					url_pre_suffix, sizeof(url_pre_suffix),
					url_suffix, sizeof(url_suffix),
					cseq, sizeof(cseq),
					session_id_str, sizeof(session_id_str),
					&content_length);
			((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->last_crlf[2] = '\r';
			/* restore its value
			 * Check first for a bogus "Content-Length" value that would cause a pointer wraparound: */
			if (tmp_ptr + 2 + content_length < tmp_ptr + 2) {
#ifdef DEBUG
				fprintf(stderr, "parseRTSPRequestString() returned a bogus \"Content-Length:\" value: 0x%x (%d)\n", content_length, (int)content_length);
#endif
				content_length = 0;
				parse_succeeded = false;
			}
			if (parse_succeeded) {
#ifdef DEBUG
				fprintf(stderr, "parseRTSPRequestString() succeeded, returning cmdName \"%s\", urlPreSuffix \"%s\", urlSuffix \"%s\", CSeq \"%s\", Content-Length %u, with %d bytes following the message.\n", 
						cmd_name, url_pre_suffix, url_suffix, cseq, content_length, ptr + new_bytes_read - (tmp_ptr + 2));
#endif
				/* If there was a "Content-Length:" header, then make sure we've received all of the data that it specified: */
				if (ptr + new_bytes_read < tmp_ptr + 2 + content_length) {
					break_flag = true;
					break; /* we still need more data; subsequent reads will give it to us */
				}

				/* If the request included a "Session:" id, and it refers to a client session that's
				 * current ongoing, then use this command to indicate 'liveness' on that client session: */
				request_included_session_id = session_id_str[0] != '\0';
				if (request_included_session_id) {
					client_session = (livemedia_rtsp_server_rtsp_client_session_t *)
						livemedia_generic_media_server__lookup_client_session_by_session_id_str(
								(livemedia_generic_media_server_t *)((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->our_rtsp_server, session_id_str);
					if (client_session != NULL)
						livemedia_generic_media_server_client_session__note_liveness(
								(livemedia_generic_media_server_client_session_t *)client_session);
				}

				/* We now have a complete RTSP request.
				 * Handle the specified command (beginning with commands that are session-independent): */
				((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->current_cseq = cseq;
				if (strcmp(cmd_name, "OPTIONS") == 0) { /* If the "OPTIONS" command included a "Session:" id for a session that doesn't exist,
					 * then treat this as an error: */
					if (request_included_session_id && client_session == NULL) {
#ifdef DEBUG
						fprintf(stderr, "Calling handleCmd_sessionNotFound() (case 1)\n");
#endif
						livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(
								(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection);
					} else {
						/* Normal case: */
						livemedia_rtsp_server_rtsp_client_connection__handle_cmd_options(
								(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection);
					} 
				} else if (url_pre_suffix[0] == '\0' && url_suffix[0] == '*' && url_suffix[1] == '\0') {
					/* The special "*" URL means: an operation on the entire server.  This works only for GET_PARAMETER and SET_PARAMETER: */
					/*TODO: 752. GET_PARAMETER, SET_PARAMETER*/
				} else if (strcmp(cmd_name, "DESCRIBE") == 0) {
					livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe(
							(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection,
							url_pre_suffix, url_suffix, 
							(char const*)generic_media_server_client_connection->request_buffer);
				} else if (strcmp(cmd_name, "SETUP") == 0) {
					are_authenticated = true;

					if (!request_included_session_id) {
						/* No session id was present in the request.
						 * So create a new "RTSPClientSession" object for this request.

						 * But first, make sure that we're authenticated to perform this command: */
						url_total_suffix[0] = '\0';
						if (url_pre_suffix[0] != '\0') {
							strcat(url_total_suffix, url_pre_suffix);
							strcat(url_total_suffix, "/");
						}
						strcat(url_total_suffix, url_suffix);
						if (livemedia_rtsp_server_rtsp_client_connection__authentication_ok(
									(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection,
									"SETUP", url_total_suffix, 
									(char const*)generic_media_server_client_connection->request_buffer)) {
							client_session = (livemedia_rtsp_server_rtsp_client_session_t *)
								livemedia_generic_media_server__create_new_client_session_with_id(
										(livemedia_generic_media_server_t *)((livemedia_rtsp_server_rtsp_client_connection_t *)
										 generic_media_server_client_connection)->our_rtsp_server);
						} else {
							are_authenticated = false;
						}
					}
					if (client_session != NULL) {
						livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup(client_session, 
								(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection,
								url_pre_suffix, url_suffix, 
								(char const *)generic_media_server_client_connection->request_buffer);
						play_after_setup = client_session->stream_after_setup;
					} else if (are_authenticated) {
#ifdef DEBUG
						fprintf(stderr, "Calling handleCmd_sessionNotFound() (case 2)\n");
#endif
						livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found(
								(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection);
					}

				} else if (strcmp(cmd_name, "TEARDOWN") == 0
						|| strcmp(cmd_name, "PLAY") == 0
						|| strcmp(cmd_name, "PAUSE") == 0
						|| strcmp(cmd_name, "GET_PARAMETER") == 0
						|| strcmp(cmd_name, "SET_PARAMETER") == 0) {
					if (client_session != NULL) {
						livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session(
								client_session,
								(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection,
								cmd_name, url_pre_suffix, url_suffix, 
								(char const *)generic_media_server_client_connection->request_buffer);
					} else {
#ifdef DEBUG
						fprintf(stderr, "Calling handleCmd_sessionNotFound() (case 3)\n");
#endif
						livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found(
								(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection);
					}
					/* TODO: RTSPServer.cpp 807 line */
				} else {

				}
			} else {
#ifdef DEBUG
				fprintf(stderr, "parseRTSPRequestString() failed; checking now for HTTP commands (for RTSP-over-HTTP tunneling)...\n");
#endif
				/* The request was not (valid) RTSP, but check for a special case: HTTP commands (for setting up RTSP-over-HTTP tunneling): */
				/*TODO: RTSPServer.c 832 line*/

			}
#ifdef DEBUG
			fprintf(stderr, "sending response: %s", generic_media_server_client_connection->response_buffer);
#endif
			write_req = (write_req_t *)malloc(sizeof(write_req_t));
			write_req->req.data = (void *)req->data;

			write_req->buf.base = (char *)generic_media_server_client_connection->response_buffer;
			write_req->buf.len = strlen((char *)generic_media_server_client_connection->response_buffer);
			uv_write((uv_write_t *)write_req, (uv_stream_t *)((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)
					->client_output_socket, &write_req->buf, 1, rtsp_server_write_cb);
		} 
	} while (0); 
	if ((ctx->num_bytes_remaining == 0 && status != 1) || (break_flag && status != 1) ) {
		--(((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->recursion_count);
		if (!((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->is_active) {
			if (((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->recursion_count > 0) {
				livemedia_generic_media_server_client_connection__close_sockets(generic_media_server_client_connection);
				/* Note: The "fRecursionCount" test is for a pathological situation where we reenter the event loop and get called recursively
				 * while handling a command (e.g., while handling a "DESCRIBE", to get a SDP description).
				 * In such a case we don't want to actually delete ourself until we leave the outermost call. */
			} else {
				livemedia__delete__generic_media_server_client_connection(generic_media_server_client_connection);	
			}
		}
		if (ctx) {
			free(ctx);
			ctx = NULL;
		}
	}

	if (req) {
		free(req);
		req = NULL;
	}
}

void livemedia_rtsp_server_rtsp_client_connection__handle_request_bytes__impl(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection,
		int new_bytes_read)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int num_bytes_remaining;
	rtsp_server_write_ctx_t *ctx;
	uv_write_t *write_req;

	ctx = (rtsp_server_write_ctx_t *)malloc(sizeof(rtsp_server_write_ctx_t));

	ctx->num_bytes_remaining = 0;
	ctx->new_bytes_read = new_bytes_read;
	ctx->generic_media_server_client_connection = 
		generic_media_server_client_connection;

	++((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->recursion_count;

	write_req = (uv_write_t *)malloc(sizeof(uv_write_t));
	write_req->data = ctx;
	rtsp_server_write_cb(write_req, 1);
}


void livemedia_rtsp_server_rtsp_client_connection__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_rtsp_server_rtsp_client_connection__reset_request_buffer(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_generic_media_server_client_connection__reset_request_buffer(
			(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection);
	rtsp_server_rtsp_client_connection->last_crlf = &((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->request_buffer[-3];
	rtsp_server_rtsp_client_connection->base64_remainder_count = 0; 
}

void livemedia_rtsp_server_rtsp_client_connection__close_socket_rtsp(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	/* First, tell our server to stop any streaming that it might be doing over our output socket: */
	livemedia_rtsp_server__stop_tcp_streaming_on_socket(rtsp_server_rtsp_client_connection->our_rtsp_server, 
			rtsp_server_rtsp_client_connection->client_output_socket);

	/* Turn off background handling on our input socket (and output socket, if different); then close it (or them): */
	if (rtsp_server_rtsp_client_connection->client_output_socket != 
			rtsp_server_rtsp_client_connection->client_input_socket) 
		uv_close((uv_handle_t *)rtsp_server_rtsp_client_connection->client_output_socket, on_close_cb);
	rtsp_server_rtsp_client_connection->client_output_socket = NULL;
	livemedia_generic_media_server_client_connection__close_sockets(
			(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection);
	/* closes fClientInputSocket */

}

void livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte__static(
		void *instance, u_int8_t request_byte)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_connection_t *connection;

	connection = (livemedia_rtsp_server_rtsp_client_connection_t *)instance;
	livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte1(connection, request_byte);
}

void livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte1(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		u_int8_t request_byte)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (request_byte == 0xFF) {
		/* Hack: The new handler of the input TCP socket encountered an error reading it.  Indicate this: */
		livemedia_generic_media_server_client_connection__handle_request_bytes(
				(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection, -1);
	} else if (request_byte == 0xFE) {
		/* Another hack: The new handler of the input TCP socket no longer needs it, so take back control of it: */
		uv_read_start((uv_stream_t *)rtsp_server_rtsp_client_connection->client_input_socket, 
				generic_media_server_client_connection_alloc_cb,
				generic_media_server_client_connection_read_cb);
	} else {
		/* Normal case: Add this character to our buffer; then try to handle the data that we have buffered so far: */
		if (((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->request_buffer_bytes_left == 0 ||
				((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->request_bytes_already_seen >= REQUEST_BUFFER_SIZE)
			return;
		((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->
			request_buffer[((livemedia_generic_media_server_client_connection_t *)
					rtsp_server_rtsp_client_connection)->request_bytes_already_seen] = request_byte;
		livemedia_generic_media_server_client_connection__handle_request_bytes(
				(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection, 1);
	}
}

#define SKIP_WHITESPACE while (*fields != '\0' && (*fields == ' ' || *fields == '\t')) ++fields
static bool livemedia__parse_authorization_header(char const *buf,
		char const **username, char const **realm,
		char const **nonce, char const **uri,
		char const **response)
{
	char const *fields;
	char *parameter;
	char *value;
	char *p;
	bool success;

	/* Initialize the result parameters to default values:  */
	*username = *realm = *nonce = *uri = *response = NULL;

	/*First, find "Authorization:"  */
	while (1) {
		if (*buf == '\0')
			return false; /* not found  */
		if (_strncasecmp(buf, "Authorization: Digest ", 22) == 0)
			break;
		++buf;
	}

	/* Then, run through each of the fields, looking for ones we handle: */
	fields = buf + 22;
	parameter = livemedia__str_dup_size(fields);
	value = livemedia__str_dup(fields);
	do {
		/* Parse: <parameter>="<value>" */
		   success = false;
	   parameter[0] = value[0] = '\0';
	   SKIP_WHITESPACE;
	   for (p = parameter; *fields != '\0' && *fields != ' ' && *fields != '\t' && *fields != '='; )*p++ = *fields++;
	   SKIP_WHITESPACE;	
	   if (*fields++ != '=')	/* parsing failed */
		   break;
	   *p = '\0'; /* complete parsing <parameter> */
	   SKIP_WHITESPACE;
	   if (*fields++ != '"')
		   break; /* parsing failed */
	   for (p = value; *fields != '\0' && *fields != '"'; )
		   *p++ = *fields++;
	   if (*fields++ != '"')
		   break; /* parsing failed */
	   *p = '\0'; /* complete parsing <value>*/
	   SKIP_WHITESPACE;
	   success = true;

	   /* Copy values for parameters that we understand: */
	   if (strcmp(parameter, "username") == 0) {
		   *username = livemedia__str_dup(value);
	   } else if (strcmp(parameter, "realm") == 0) {
		   *realm = livemedia__str_dup(value);
	   } else if (strcmp(parameter, "nonce") == 0) {
		   *nonce = livemedia__str_dup(value);
	   } else if (strcmp(parameter, "uri") == 0) {
		   *uri = livemedia__str_dup(value);
	   } else if (strcmp(parameter, "response") == 0) {
		   *response = livemedia__str_dup(value);
	   }

    // Check for a ',', indicating that more <parameter>="<value>" pairs follow:
	} while (*fields++ == ',');

	if (parameter) {
		free(parameter);
		parameter = NULL;
	}
	if (value) {
		free(value);
		value = NULL;
	}
	return success;
}

bool livemedia_rtsp_server_rtsp_client_connection__authentication_ok(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd_name, char const * url_suffix, char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_user_authentication_database_t *auth_db;
	char const *username;
	char const *realm;
	char const *nonce;
	char const *uri;
	char const *response;
	bool success;
	char const *password;
	char const *our_response;



	if (!livemedia_rtsp_server__special_client_access_check(
				rtsp_server_rtsp_client_connection->our_rtsp_server,
				rtsp_server_rtsp_client_connection->client_input_socket,
				&((livemedia_generic_media_server_client_connection_t *)
					rtsp_server_rtsp_client_connection)->client_addr,
				url_suffix, username)) {
		livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
				rtsp_server_rtsp_client_connection, "401 Unauthorized");
		return false;
	}

	/* If we weren't set up with an authentication database, we're OK: */
	auth_db = livemedia_rtsp_server__get_authentication_database_for_command(
			rtsp_server_rtsp_client_connection->our_rtsp_server, cmd_name);
	if (auth_db == NULL)
		return true;
	username = NULL;
	realm = NULL;
	nonce = NULL;
	uri = NULL;
	response = NULL;
	success = false;

	do {
		/* To authenticate, we first need to have a nonce set up
		 * from a previous attempt: */
		if (livemedia_authenticator__nonce(
					&rtsp_server_rtsp_client_connection->current_authenticator) == NULL)
			break;

		/* Next, the request needs to contain an "Authorization:" header,
		 * containing a username, (our) realm, (our) nonce, uri,
		 * and response string: */
		if (livemedia__parse_authorization_header(full_request_str,
					&username, &realm, &nonce, &uri, &response)
				|| username == NULL
				|| realm == NULL 
				|| strcmp(realm, livemedia_authenticator__realm(
						&rtsp_server_rtsp_client_connection->current_authenticator))
				|| nonce == NULL 
				|| strcmp(nonce, livemedia_authenticator__nonce(
						&rtsp_server_rtsp_client_connection->current_authenticator))
				|| uri == NULL || response == NULL) {
			break;
		}

		/* Next, the username has to be known to us: */
		password = livemedia_user_authentication_database__lookup_password(auth_db, 
				username);
#ifdef DEBUG
    fprintf(stderr, "lookupPassword(%s) returned password %s\n", username, password);
#endif
	if (password == NULL)
		break;
	livemedia_authenticator__set_username_and_password(
			&rtsp_server_rtsp_client_connection->current_authenticator, 
			username, password, 
			livemedia_user_authentication_database__passwords_are_md5(auth_db));

    /* Finally, compute a digest response from the information that we have,
     * and compare it to the one that we were given: */
	our_response = livemedia_authenticator__compute_digest_response(
			&rtsp_server_rtsp_client_connection->current_authenticator, 
			cmd_name, uri);
	success = (strcmp(our_response, response) == 0);
	livemedia_authenticator__reclaim_digest_response(
			&rtsp_server_rtsp_client_connection->current_authenticator, 
			our_response);
	} while (0);

	if (realm) {
		free((char *)realm);
		realm = NULL;
	}
	if (nonce) {
		free((char *)nonce);
		nonce = NULL;
	}
	if (uri) {
		free((char *)uri);
		uri = NULL;
	}
	if (response) {
		free((char *)response);
		response = NULL;
	}

	if ((char *)success) {
    /* The user has been authenticated.
     * Now allow subclasses a chance to validate the user against the IP address and/or URL suffix. */
		if (!livemedia_rtsp_server__special_client_access_check(
					rtsp_server_rtsp_client_connection->our_rtsp_server,
					rtsp_server_rtsp_client_connection->client_input_socket,
					&((livemedia_generic_media_server_client_connection_t *)
					 rtsp_server_rtsp_client_connection)->client_addr,
					url_suffix, username)) {
      /* Note: We don't return a "WWW-Authenticate" header here, because the user is valid,
	   * even though the server has decided that they should not have access. */
			livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
					rtsp_server_rtsp_client_connection, "401 Unauthorized");
			if (username) {
				free((char *)username);
				username = NULL;
			}
			return false;
		}
	}
	if (username) {
		free((char *)username);
		username = NULL;
	}
	if (success)
		return true;

  /* If we get here, we failed to authenticate the user.
   * Send back a "401 Unauthorized" response, with a new random nonce: */
	livemedia_authenticator__set_realm_and_random_nonce(
			&rtsp_server_rtsp_client_connection->current_authenticator, 
			livemedia_user_authentication_database__realm(auth_db));

  snprintf((char*)((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->response_buffer,
		  sizeof(((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->response_buffer),
	   "RTSP/1.0 401 Unauthorized\r\n"
	   "CSeq: %s\r\n"
	   "%s"
	   "WWW-Authenticate: Digest realm=\"%s\", nonce=\"%s\"\r\n\r\n",
	   rtsp_server_rtsp_client_connection->current_cseq,
	   livemedia__date_header(),
	   livemedia_authenticator__realm(
		   &rtsp_server_rtsp_client_connection->current_authenticator),
	   livemedia_authenticator__nonce(
		   &rtsp_server_rtsp_client_connection->current_authenticator));
  return false;
}

void livemedia_rtsp_server_rtsp_client_connection__change_client_input_socket(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		int new_socket_num, unsigned char const *extra_data, unsigned int extra_data_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/* used to implement RTSP-over-HTTP tunneling */
void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register__static(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/* Shortcuts for setting up a RTSP response (prior to sending it): */
void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection;

	generic_media_server_client_connection = 
		(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection;
	snprintf((char*)generic_media_server_client_connection->response_buffer, 
			sizeof(generic_media_server_client_connection->response_buffer),
			"RTSP/1.0 %s\r\n"
			"CSeq: %s\r\n"
			"%s\r\n",
			response_str,
			rtsp_server_rtsp_client_connection->current_cseq,
			livemedia__date_header());

}

void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_session_id(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, u_int32_t session_id)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_content_str(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, char const *content_str)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_session_id_and_content_str(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, u_int32_t session_id, char const *content_str)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*****************************************************************
 * ParamsForREGISTER (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *__livemedia_rtsp_server_rtsp_client_connection_params_for_register__alloc(void)
{
	livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register;
	rtsp_server_rtsp_client_connection_params_for_register = (livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *)calloc(1, sizeof(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t));
	if (rtsp_server_rtsp_client_connection_params_for_register == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtsp_server_rtsp_client_connection_params_for_register;
}
void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__init(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register,
		char const *cmd/*"REGISTER" or "DEREGISTER"*/,
		livemedia_rtsp_server_rtsp_client_connection_t *our_connection,
		char const *url, char const *url_suffix,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */

	/* TODO: virtual functions configuration */ 

	rtsp_server_rtsp_client_connection_params_for_register->delete = livemedia_rtsp_server_rtsp_client_connection_params_for_register__delete__impl;
}
void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__deinit(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__free(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register)
{
	if (rtsp_server_rtsp_client_connection_params_for_register) {
		free(rtsp_server_rtsp_client_connection_params_for_register);
		rtsp_server_rtsp_client_connection_params_for_register = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *livemedia__new__rtsp_server_rtsp_client_connection_params_for_register(
		char const *cmd/*"REGISTER" or "DEREGISTER"*/,
		livemedia_rtsp_server_rtsp_client_connection_t *our_connection,
		char const *url, char const *url_suffix,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register;

	rtsp_server_rtsp_client_connection_params_for_register = __livemedia_rtsp_server_rtsp_client_connection_params_for_register__alloc();
	if (rtsp_server_rtsp_client_connection_params_for_register)
		__livemedia_rtsp_server_rtsp_client_connection_params_for_register__init(rtsp_server_rtsp_client_connection_params_for_register,
				cmd, our_connection, url, url_suffix, reuse_connection, deliver_via_tcp, proxy_url_suffix);

	return rtsp_server_rtsp_client_connection_params_for_register;
}
void livemedia__delete__rtsp_server_rtsp_client_connection_params_for_register(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register)
{
	if (rtsp_server_rtsp_client_connection_params_for_register && rtsp_server_rtsp_client_connection_params_for_register->delete)
		rtsp_server_rtsp_client_connection_params_for_register->delete(rtsp_server_rtsp_client_connection_params_for_register);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtsp_server_rtsp_client_connection_params_for_register__delete__impl(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO: if child, get parent as parameter and cast to child */
	if (rtsp_server_rtsp_client_connection_params_for_register) {
		__livemedia_rtsp_server_rtsp_client_connection_params_for_register__deinit(rtsp_server_rtsp_client_connection_params_for_register);
		__livemedia_rtsp_server_rtsp_client_connection_params_for_register__free(rtsp_server_rtsp_client_connection_params_for_register);
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

/*****************************************************************
 * RTSPClientSession:GenericMediaServer::ClientSession (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_rtsp_client_session_t *__livemedia_rtsp_server_rtsp_client_session__alloc(void)
{
	livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session;
	rtsp_server_rtsp_client_session = (livemedia_rtsp_server_rtsp_client_session_t *)calloc(1, sizeof(livemedia_rtsp_server_rtsp_client_session_t));
	if (rtsp_server_rtsp_client_session == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtsp_server_rtsp_client_session_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtsp_server_rtsp_client_session;
}
void __livemedia_rtsp_server_rtsp_client_session__init(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_t *our_server, u_int32_t session_id)
{
	__livemedia_generic_media_server_client_session__init(
			&rtsp_server_rtsp_client_session->generic_media_server_client_session, our_server, session_id);

	rtsp_server_rtsp_client_session->our_rtsp_server = our_server;
	rtsp_server_rtsp_client_session->is_multicast = false;
	rtsp_server_rtsp_client_session->stream_after_setup = false;
	rtsp_server_rtsp_client_session->tcp_stream_id_count = 0;
	rtsp_server_rtsp_client_session->num_stream_states = 0;
	rtsp_server_rtsp_client_session->stream_states = NULL;

	/* virtual functions configuration */ 
	rtsp_server_rtsp_client_session->handle_cmd_setup = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup__impl;

	rtsp_server_rtsp_client_session->handle_cmd_within_session = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session__impl;

	rtsp_server_rtsp_client_session->handle_cmd_tear_down = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down__impl;

	rtsp_server_rtsp_client_session->handle_cmd_play = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_play__impl;

	rtsp_server_rtsp_client_session->handle_cmd_pause = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause__impl;

	rtsp_server_rtsp_client_session->handle_cmd_get_parameter = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter__impl;

	rtsp_server_rtsp_client_session->handle_cmd_set_parameter = 
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter__impl;

	((livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session)->delete =
		livemedia_rtsp_server_rtsp_client_session__delete__impl;

}
void __livemedia_rtsp_server_rtsp_client_session__deinit(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
	/* destructor */
	livemedia_rtsp_server_rtsp_client_session__reclaim_stream_states(rtsp_server_rtsp_client_session);

	/* parent deinit */
	__livemedia_generic_media_server_client_session__deinit(
			&rtsp_server_rtsp_client_session->generic_media_server_client_session);
}
void __livemedia_rtsp_server_rtsp_client_session__free(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
	if (rtsp_server_rtsp_client_session) {
		free(rtsp_server_rtsp_client_session);
		rtsp_server_rtsp_client_session = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtsp_server_rtsp_client_session_t *livemedia__new__rtsp_server_rtsp_client_session(
		livemedia_rtsp_server_t *our_server, u_int32_t session_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session;

	rtsp_server_rtsp_client_session = __livemedia_rtsp_server_rtsp_client_session__alloc();
	if (rtsp_server_rtsp_client_session)
		__livemedia_rtsp_server_rtsp_client_session__init(rtsp_server_rtsp_client_session,
				our_server, session_id);

	return rtsp_server_rtsp_client_session;
}
void livemedia__delete__rtsp_server_rtsp_client_session(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
	if (rtsp_server_rtsp_client_session && 
			((livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session)->delete)
		((livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session)->delete(
			(livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session);
}
/*
 * Functions for virtual functions
 */
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char *url_pre_suffix, char *url_suffix, char const *full_request_str)
{
	if (rtsp_server_rtsp_client_session &&
			rtsp_server_rtsp_client_session->handle_cmd_setup) {
		rtsp_server_rtsp_client_session->handle_cmd_setup(
				rtsp_server_rtsp_client_session, our_client_connection, url_pre_suffix,
				url_suffix, full_request_str);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
		char const *full_request_str)
{
	if (rtsp_server_rtsp_client_session) {
		rtsp_server_rtsp_client_session->handle_cmd_within_session(
				rtsp_server_rtsp_client_session, our_client_connection, cmd_name,
				url_pre_suffix, url_suffix, full_request_str);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession)
{
	if (rtsp_server_rtsp_client_session) {
		rtsp_server_rtsp_client_session->handle_cmd_tear_down(
				rtsp_server_rtsp_client_session, our_client_connection, subsession);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_play(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
	if (rtsp_server_rtsp_client_session) {
		rtsp_server_rtsp_client_session->handle_cmd_play(
				rtsp_server_rtsp_client_session, our_client_connection, subsession,
				full_request_str);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession)
{
	if (rtsp_server_rtsp_client_session) {
		rtsp_server_rtsp_client_session->handle_cmd_pause(
				rtsp_server_rtsp_client_session, our_client_connection, subsession);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
	if (rtsp_server_rtsp_client_session) {
		rtsp_server_rtsp_client_session->handle_cmd_get_parameter(
				rtsp_server_rtsp_client_session, our_client_connection, subsession,
				full_request_str);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
	if (rtsp_server_rtsp_client_session) {
		rtsp_server_rtsp_client_session->handle_cmd_set_parameter(
				rtsp_server_rtsp_client_session, our_client_connection, subsession,
				full_request_str);
	}
}


/*
 * Override functions
 */
typedef enum _streaming_mode_e {
	RTP_UDP,
	RTP_TCP,
	RAW_UDP
} streaming_mode_e;

static void livemedia__parse_transport_header(char const *buf,
		streaming_mode_e *streaming_mode,
		char **streaming_mode_string,
		char **destination_address_str,
		u_int8_t *destination_ttl,
		port_num_bits *client_rtp_port_num,	/* if UDP */
		port_num_bits *client_rtcp_port_num,	/* if UDP */
		unsigned char *rtp_channel_id,	/* if TCP */
		unsigned char *rtcp_channel_id	/* if TCP */
		)
{
	port_num_bits p1, p2;
	unsigned int ttl, rtp_cid, rtcp_cid;
	char *fields;
	char *field;

	/* Initialize the result parameters to default values: */
	*streaming_mode = RTP_UDP;
	*streaming_mode_string = NULL;
	*destination_address_str = NULL;
	*destination_ttl = 255;
	*client_rtp_port_num = 0;
	*client_rtcp_port_num = 1;
	*rtp_channel_id = *rtcp_channel_id = 0xFF;

	/* First, find "Transport:" */
	while (1) {
		if (*buf == '\0')
			return; /* not found */
		if (*buf == '\r' && *(buf+1) == '\n' && *(buf+2) == '\r')
			return; /* end of the headers => not found */
		if (_strncasecmp(buf, "Transport:", 10) == 0)
			break;
		++buf;
	}

	/* Then, run through each of the fields, looking for ones we handle: */
	fields = buf + 10;
	while (*fields == ' ')
		++fields;
	field = livemedia__str_dup_size(fields);
	while (sscanf(fields, "%[^;\r\n]", field) == 1) {
		if (strcmp(field, "RTP/AVP/TCP") == 0) {
			*streaming_mode = RTP_TCP;
		} else if (strcmp(field, "RAW/RAW/UDP") == 0 ||
				strcmp(field, "MP2T/H2221/UDP") == 0) {
			*streaming_mode = RAW_UDP;
			*streaming_mode_string = livemedia__str_dup(field);
		} else if (_strncasecmp(field, "destination=", 12) == 0) {
			if (*destination_address_str) {
				free(*destination_address_str);
				*destination_address_str = NULL;
			}
			*destination_address_str = livemedia__str_dup(field+12);
		} else if (sscanf(field, "ttl%u", &ttl) == 1) {
			*destination_ttl = (u_int8_t)ttl;
		} else if (sscanf(field, "client_port=%hu-%hu", &p1, &p2) == 2) {
			*client_rtp_port_num = p1;
			*client_rtcp_port_num = *streaming_mode == RAW_UDP ? 0 : p2;
			/* ignore the second port number if the client asked for raw UDP */
		} else if (sscanf(field, "client_port=%hu", &p1) == 1) {
			*client_rtp_port_num = p1;
			*client_rtcp_port_num = *streaming_mode == RAW_UDP ? 0 : p1 + 1;
		} else if (sscanf(field, "interleaved=%u-%u", &rtp_cid, &rtcp_cid) == 2) {
			*rtp_channel_id = (unsigned char)rtp_cid;
			*rtcp_channel_id = (unsigned char)rtcp_cid;
		}

		fields += strlen(field);
		while (*fields == ';' || *fields == ' ' || *fields == '\t')
			++fields;
		/* skip over separating ';' chars or whitespace */
		if (*fields == '\0' || *fields == '\r' || *fields == '\n')
			break;
	}
	if (field) {
		free(field);
		field = NULL;
	}
}

static bool livemedia__parse_play_now_header(char const *buf)
{
	/* Find "x-playNow:" header, if present */
	while (1) {
		if (*buf == '\0')
			return false; /* not found */
		if (_strncasecmp(buf, "x-playNow:", 10) == 0)
			break;
		++buf;
	}

	return true;
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char *url_pre_suffix, char *url_suffix, char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_generic_media_server_client_session_t *generic_media_server_client_session;

  /* Normally, "urlPreSuffix" should be the session (stream) name, and "urlSuffix" should be the subsession (track) name.
   * However (being "liberal in what we accept"), we also handle 'aggregate' SETUP requests (i.e., without a track name),
   * in the special case where we have only a single track.  I.e., in this case, we also handle:
   *    "urlPreSuffix" is empty and "urlSuffix" is the session (stream) name, or
   *    "urlPreSuffix" concatenated with "urlSuffix" (with "/" inbetween) is the session (stream) name. */
	char *stream_name;
	char *track_id;
	char *concatenated_stream_name;
	livemedia_server_media_session_t *sms;
	livemedia_server_media_subsession_iterator_t *iter;
	livemedia_server_media_subsession_t *subsession;
	unsigned int i;
	unsigned int track_num;
	void **token;
	char *streaming_mode_string;
	char *clients_destination_address_str;
	u_int8_t clients_destination_ttl;
	port_num_bits client_rtp_port_num;
	port_num_bits client_rtcp_port_num;
	unsigned char rtp_channel_id;
	unsigned char rtcp_channel_id;
	double range_start;
	double range_end;
	char *abs_start;
	char *abs_end;;
	bool start_time_is_now;
	net_address_bits destination_address;
	u_int8_t destination_ttl;
	struct sockaddr_in source_addr;
	SOCKLEN_T namelen;
	net_address_bits orig_sending_interface_addr;
	net_address_bits orig_receiving_interface_addr;
	char timeout_parameter_string[100];
	streaming_mode_e streaming_mode;
	groupsock_port_t *client_rtp_port;
	groupsock_port_t *client_rtcp_port;
	groupsock_port_t *server_rtp_port;
	groupsock_port_t *server_rtcp_port;
	groupsock_address_string_t *dest_addr_str;
	groupsock_address_string_t *source_addr_str;
	int client_input_socket_fd;

	generic_media_server_client_session = 
		(livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session; 

	stream_name = url_pre_suffix; /* in normal case */
	track_id = url_suffix; /* in nurmal case */
	concatenated_stream_name = NULL; /* in nurmal case */
	

	do {
    /* First, make sure the specified stream name exists: */
		sms = livemedia_generic_media_server__lookup_server_media_session(
				generic_media_server_client_session->our_server,
				stream_name, generic_media_server_client_session->our_server_media_session == NULL);
		if (sms == NULL) {
			/* Check for the special case (noted above), before we give up: */
			if (url_pre_suffix[0] == '\0') {
				stream_name = url_suffix;
			} else {
				concatenated_stream_name = (char *)malloc(sizeof(char) * 
						(strlen(url_pre_suffix) + strlen(url_suffix) + 2));
				/* allow for the "/" and the trailing '\0' */
				sprintf(concatenated_stream_name, "%s/%s", url_pre_suffix, url_suffix);
				stream_name = concatenated_stream_name;
			}
			track_id = NULL;
			/* Check again: */
			sms = livemedia_generic_media_server__lookup_server_media_session(
					generic_media_server_client_session->our_server,
					stream_name, generic_media_server_client_session->our_server_media_session == NULL);
		}
		if (sms == NULL) {
			if (generic_media_server_client_session->
					our_server_media_session == NULL) {
				/* The client asked for a stream that doesn't exist (and this session descriptor has not been used before): */
				livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(our_client_connection);
			} else {
	/* The client asked for a stream that doesn't exist, but using a stream id for a stream that does exist. Bad request: */
				livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad(our_client_connection);
			}
			break;
		} else {
			if (generic_media_server_client_session->
					our_server_media_session == NULL) {
				/* We're accessing the "ServerMediaSession" for the first time. */
				generic_media_server_client_session->our_server_media_session = sms;
				livemedia_server_media_session__increment_reference_count(
						generic_media_server_client_session->
					our_server_media_session);
			} else if (sms != generic_media_server_client_session->
					our_server_media_session) {
	/* The client asked for a stream that's different from the one originally requested for this stream id.  Bad request: */
				livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad(our_client_connection);
				break;
			}
		}
		
		if (rtsp_server_rtsp_client_session->stream_states == NULL) {
			/* This is the first "SETUP" for this session.  Set up our array of states for all of this session's subsessions (tracks): */
			rtsp_server_rtsp_client_session->num_stream_states = livemedia_server_media_session__num_subsessions(
					generic_media_server_client_session->our_server_media_session);
			rtsp_server_rtsp_client_session->stream_states = (struct stream_state_t *)malloc(
					sizeof(struct stream_state_t) * rtsp_server_rtsp_client_session->num_stream_states);
			iter = livemedia__new__server_media_subsession_iterator(
				generic_media_server_client_session->our_server_media_session);

			for (i = 0; i < rtsp_server_rtsp_client_session->num_stream_states; ++i) {
				subsession = livemedia_server_media_subsession_iterator__next(iter);
				rtsp_server_rtsp_client_session->stream_states[i].subsession = subsession;
				rtsp_server_rtsp_client_session->stream_states[i].tcp_socket_num = NULL;
				/* for now; may get set for RTP-over-TCP streaming */
				rtsp_server_rtsp_client_session->stream_states[i].stream_token = NULL;
				/* for now; it may be changed by the "getStreamParameters()" call that comes later */
			}
		}

		/* Look up information for the specified subsession (track): */
		subsession = NULL;
		if (track_id != NULL && track_id[0] != '\0') { /* normal case */
			for (track_num = 0; track_num < rtsp_server_rtsp_client_session->num_stream_states; ++track_num) {
				subsession = rtsp_server_rtsp_client_session->stream_states[track_num].subsession;
				if (subsession != NULL && strcmp(track_id, livemedia_server_media_subsession__track_id(subsession)) == 0)
					break;
			}
			if (track_num >= rtsp_server_rtsp_client_session->num_stream_states) {
				/* The specified track id doesn't exist, so this request fails: */
				livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(our_client_connection);
				break;
			}
		} else {
			/* Weird case: there was no track id in the URL.
			 * This works only if we have only one subsession: */
			if (rtsp_server_rtsp_client_session->num_stream_states != 1 ||
					rtsp_server_rtsp_client_session->stream_states[0].subsession == NULL) {
				livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad(our_client_connection);
				break;
			}
			track_num = 0;
			subsession = rtsp_server_rtsp_client_session->stream_states[track_num].subsession;
		}
		/* ASSERT: subsession != NULL */
		token = rtsp_server_rtsp_client_session->stream_states[track_num].stream_token; /* alias */
		if (token != NULL) {
			/* We already handled a "SETUP" for this track (to the same client),
			 * so stop any existing streaming of it, before we set it up again: */
			livemedia_server_media_subsession__pause_stream(subsession, 
					generic_media_server_client_session->our_session_id, token);
			livemedia_rtsp_server__unnote_tcp_streaming_on_socket(rtsp_server_rtsp_client_session->our_rtsp_server,
					rtsp_server_rtsp_client_session->stream_states[track_num].tcp_socket_num,
					rtsp_server_rtsp_client_session, track_num);
			livemedia_server_media_subsession__delete_stream(subsession, 
					generic_media_server_client_session->our_session_id, token);
		}

		/* Look for a "Transport:" header in the request string, to extract client parameters: */
		streaming_mode_string = NULL;	/* set when RAW_UDP streaming is specified */

		livemedia__parse_transport_header(full_request_str, &streaming_mode, &streaming_mode_string,
				&clients_destination_address_str, &clients_destination_ttl, 
				&client_rtp_port_num, &client_rtcp_port_num,
				&rtp_channel_id, &rtcp_channel_id);
		if ((streaming_mode == RTP_TCP && rtp_channel_id == 0xFF) ||
				(streaming_mode != RTP_TCP && 
				 our_client_connection->client_output_socket != our_client_connection->client_input_socket)) {
      /* An anomolous situation, caused by a buggy client.  Either:
       *     1/ TCP streaming was requested, but with no "interleaving=" fields.  (QuickTime Player sometimes does this.), or
       *     2/ TCP streaming was not requested, but we're doing RTSP-over-HTTP tunneling (which implies TCP streaming).
       * In either case, we assume TCP streaming, and set the RTP and RTCP channel ids to proper values: */
			streaming_mode = RTP_TCP;
			rtp_channel_id = rtsp_server_rtsp_client_session->tcp_stream_id_count;
			rtcp_channel_id = rtsp_server_rtsp_client_session->tcp_stream_id_count + 1;
		}
		if (streaming_mode == RTP_TCP)
			rtsp_server_rtsp_client_session->tcp_stream_id_count += 2;
			
		client_rtp_port = groupsock__new__port(client_rtp_port_num);
		client_rtcp_port = groupsock__new__port(client_rtcp_port_num);
		/* Next, check whether a "Range:" or "x-playNow:" header is present in the request.
		 * This isn't legal, but some clients do this to combine "SETUP" and "PLAY": */

		range_start = 0.0;
		range_end = 0.0;
		abs_start = NULL;
		abs_end = NULL;
		if (livemedia__parse_range_header(full_request_str, &range_start, &range_end, &abs_start, &abs_end, &start_time_is_now)) {
			if (abs_start) {
				free(abs_start);
				abs_start = NULL;
			}
			if (abs_end) {
				free(abs_end);
				abs_end = NULL;
			}
			rtsp_server_rtsp_client_session->stream_after_setup = true;
		} else if (livemedia__parse_play_now_header(full_request_str)) {
			rtsp_server_rtsp_client_session->stream_after_setup = true;
		} else {
			rtsp_server_rtsp_client_session->stream_after_setup = false;
		}

		/* Then, get server parameters from the 'subsession': */
		if (streaming_mode == RTP_TCP) {
			/* Note that we'll be streaming over the RTSP TCP connection: */
			rtsp_server_rtsp_client_session->stream_states[track_num].tcp_socket_num =
				our_client_connection->client_output_socket;
			livemedia_rtsp_server__note_tcp_streaming_on_socket(rtsp_server_rtsp_client_session->our_rtsp_server,
					rtsp_server_rtsp_client_session->stream_states[track_num].tcp_socket_num,
					rtsp_server_rtsp_client_session, track_num);
		}
		destination_address = 0;
		destination_ttl = 255;
#ifdef RTSP_ALLOW_CLIENT_DESTINATION_SETTING
		if (clients_destination_address_str != NULL) {
			/* Use the client-provided "destination" address.
			 * Note: This potentially allows the server to be used in denial-of-service
			 * attacks, so don't enable this code unless you're sure that clients are
			 * trusted. */
			destination_address = our_inet_addr(clients_destination_address_str);
		}
		/* Also use the client-provided TTL. */
		destination_ttl = clients_destination_ttl;
#endif
		if (clients_destination_address_str) {
			free(clients_destination_address_str);
			clients_destination_address_str = NULL;
		}
		server_rtp_port = groupsock__new__port(0);
		server_rtcp_port = groupsock__new__port(0);

		/* Make sure that we transmit on the same interface that's used by the client (in case we're a multi-homed server): */
		namelen = sizeof(source_addr);
		uv_fileno((uv_handle_t *)our_client_connection->client_input_socket, &client_input_socket_fd);
		getsockname(client_input_socket_fd, (struct sockaddr *)&source_addr, &namelen);
		orig_sending_interface_addr = sending_interface_addr;
		orig_receiving_interface_addr = receiving_interface_addr;
		/* NOTE: The following might not work properly, so we ifdef it out for now: */
#ifdef HACK_FOR_MULTIHOMED_SERVERS
		receiving_interface_addr = sending_interface_addr = source_addr.sin_addr.s_addr;
#endif

		livemedia_server_media_subsession__get_stream_parameters(subsession,
				generic_media_server_client_session->our_session_id,
				((livemedia_generic_media_server_client_connection_t *)our_client_connection)->client_addr.sin_addr.s_addr,
				client_rtp_port, client_rtcp_port, rtsp_server_rtsp_client_session->stream_states[track_num].tcp_socket_num,
				rtp_channel_id, rtcp_channel_id, &destination_address, &destination_ttl,
				&rtsp_server_rtsp_client_session->is_multicast, server_rtp_port, server_rtcp_port,
				&(rtsp_server_rtsp_client_session->stream_states)[track_num].stream_token);
		sending_interface_addr = orig_sending_interface_addr;
		receiving_interface_addr = orig_receiving_interface_addr;

		dest_addr_str = groupsock__new__address_string_by_net_address_bits(destination_address);
		source_addr_str = groupsock__new__address_string_by_sockaddr_in(&source_addr);
		if (((livemedia_generic_media_server_t *)rtsp_server_rtsp_client_session->our_rtsp_server)->reclamation_seconds > 0) {
			sprintf(timeout_parameter_string, ";timeout=%u", 
					((livemedia_generic_media_server_t *)rtsp_server_rtsp_client_session->our_rtsp_server)->reclamation_seconds);
		} else {
			timeout_parameter_string[0] = '\0';
		}
		if (rtsp_server_rtsp_client_session->is_multicast) {
			switch(streaming_mode) {
				case RTP_UDP:
				{
					snprintf((char*)((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
							response_buffer,
							sizeof(((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
								response_buffer),
							"RTSP/1.0 200 OK\r\n"
							"CSeq: %s\r\n"
							"%s"
							"Transport: RTP/AVP;multicast;destination=%s;source=%s;port=%d-%d;ttl=%d\r\n"
							"Session: %08X%s\r\n\r\n",
							our_client_connection->current_cseq,
							livemedia__date_header(),
							groupsock_address_string__val(dest_addr_str), groupsock_address_string__val(source_addr_str),
							ntohs(groupsock_port__num(server_rtp_port)), ntohs(groupsock_port__num(server_rtcp_port)), 
							destination_ttl, generic_media_server_client_session->our_session_id, timeout_parameter_string);
					break;
				}
				case RTP_TCP:
				{
					/* multicast streams can't be sent via TCP */ 
					livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport(our_client_connection);
					break;
				}
				case RAW_UDP:
				{
					snprintf((char*)((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
							response_buffer,
							sizeof(((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
								response_buffer),
							"RTSP/1.0 200 OK\r\n"
							"CSeq: %s\r\n"
							"%s"
							"Transport: %s;multicast;destination=%s;source=%s;port=%d;ttl=%d\r\n"
							"Session: %08X%s\r\n\r\n",
							our_client_connection->current_cseq,
							livemedia__date_header(),
							streaming_mode_string, 
							groupsock_address_string__val(dest_addr_str), groupsock_address_string__val(source_addr_str),
							ntohs(groupsock_port__num(server_rtp_port)), 
							destination_ttl, generic_media_server_client_session->our_session_id, timeout_parameter_string);
					break;
				}
			}
		} else {
			switch (streaming_mode) {
				case RTP_UDP:
				{
					snprintf((char*)((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
							response_buffer,
							sizeof(((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
								response_buffer),
							"RTSP/1.0 200 OK\r\n"
							"CSeq: %s\r\n"
							"%s"
							"Transport: RTP/AVP;unicast;destination=%s;source=%s;client_port=%d-%d;server_port=%d-%d\r\n"
							"Session: %08X%s\r\n\r\n",
							our_client_connection->current_cseq,
							livemedia__date_header(),
							groupsock_address_string__val(dest_addr_str), groupsock_address_string__val(source_addr_str),
							ntohs(groupsock_port__num(client_rtp_port)), ntohs(groupsock_port__num(client_rtcp_port)), 
							ntohs(groupsock_port__num(server_rtp_port)), ntohs(groupsock_port__num(server_rtcp_port)), 
							generic_media_server_client_session->our_session_id, timeout_parameter_string);
					break;
				}
				case RTP_TCP:
				{
					if (!rtsp_server_rtsp_client_session->our_rtsp_server->allow_streaming_rtp_over_tcp) {
						livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport(
								our_client_connection);
					} else {
						snprintf((char*)((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
								response_buffer,
								sizeof(((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
									response_buffer),
								"RTSP/1.0 200 OK\r\n"
								"CSeq: %s\r\n"
								"%s"
								"Transport: RTP/AVP/TCP;unicast;destination=%s;source=%s;interleaved=%d-%d\r\n"
								"Session: %08X%s\r\n\r\n",
								our_client_connection->current_cseq,
								livemedia__date_header(),
								groupsock_address_string__val(dest_addr_str), groupsock_address_string__val(source_addr_str),
								rtp_channel_id, rtcp_channel_id, 
								generic_media_server_client_session->our_session_id, timeout_parameter_string);
					}
					break;
				}
				case RAW_UDP:
				{
					snprintf((char*)((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
							response_buffer,
							sizeof(((livemedia_generic_media_server_client_connection_t *)our_client_connection)->
								response_buffer),
							"RTSP/1.0 200 OK\r\n"
							"CSeq: %s\r\n"
							"%s"
							"Transport: %s;unicast;destination=%s;source=%s;client_port=%d;server_port=%d\r\n"
							"Session: %08X%s\r\n\r\n",
							our_client_connection->current_cseq,
							livemedia__date_header(),
							streaming_mode_string, 
							groupsock_address_string__val(dest_addr_str), groupsock_address_string__val(source_addr_str),
							ntohs(groupsock_port__num(client_rtp_port)), ntohs(groupsock_port__num(server_rtp_port)), 
							generic_media_server_client_session->our_session_id, timeout_parameter_string);
					break;
				}
			}
		}
		if (streaming_mode_string) {
			free(streaming_mode_string);
			streaming_mode_string = NULL;
		}

		groupsock__delete__port(client_rtp_port);
		groupsock__delete__port(client_rtcp_port);
		groupsock__delete__port(server_rtp_port);
		groupsock__delete__port(server_rtcp_port);
		groupsock__delete__address_string(dest_addr_str);
		groupsock__delete__address_string(source_addr_str);

	} while(0);

	if (concatenated_stream_name) {
		free(concatenated_stream_name);
		concatenated_stream_name = NULL;
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
		char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_server_media_subsession_t *subsession;
	unsigned int url_pre_suffix_len;
	livemedia_server_media_subsession_iterator_t *iter;

	livemedia_generic_media_server_client_session_t *generic_media_server_client_session;
	generic_media_server_client_session = 
		(livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session;

	/* This will either be:
	 * - a non-aggregated operation, if "urlPreSuffix" is the session (stream)
	 *   name and "urlSuffix" is the subsession (track) name, or
	 * - an aggregated operation, if "urlSuffix" is the session (stream) name,
	 *   or "urlPreSuffix" is the session (stream) name, and "urlSuffix" is empty,
	 *   or "urlPreSuffix" and "urlSuffix" are both nonempty, but when concatenated, (with "/") form the session (stream) name.
	 * Begin by figuring out which of these it is: */

	if (generic_media_server_client_session->our_server_media_session == NULL) { /* There wasn't a previous SETUP! */
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported(our_client_connection);
		return;
	} else if (url_suffix[0] != '\0' &&
			strcmp(livemedia_server_media_session__stream_name(
					generic_media_server_client_session->our_server_media_session), url_pre_suffix) == 0) {
		/* Non-aggregated operation.
		 * Look up the media subsession whose track id is "urlSuffix": */
		iter = livemedia__new__server_media_subsession_iterator(
				generic_media_server_client_session->our_server_media_session);
		while ((subsession = livemedia_server_media_subsession_iterator__next(iter)) != NULL) {
			if (strcmp(livemedia_server_media_subsession__track_id(subsession), url_suffix) == 0)
				break; /* success */
		}
		if (subsession == NULL) { /* no such track! */
			livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(our_client_connection);
			return;
		}
	} else if (strcmp(livemedia_server_media_session__stream_name(
					generic_media_server_client_session->our_server_media_session), url_suffix) == 0 ||
			(url_suffix[0] == '\0' &&
			 strcmp(livemedia_server_media_session__stream_name(
					 generic_media_server_client_session->our_server_media_session), url_pre_suffix) == 0)) {
		/* Aggregated operation */
		subsession = NULL;
	} else if (url_pre_suffix[0] != '\0' && url_suffix[0] != '\0') {
		/* Aggregated operation, if <urlPreSuffix>/<urlSuffix> is the session (stream) name: */
		url_pre_suffix_len = strlen(url_pre_suffix);
		if (strncmp(livemedia_server_media_session__stream_name(
						generic_media_server_client_session->our_server_media_session), url_pre_suffix, url_pre_suffix_len) == 0 &&
				(livemedia_server_media_session__stream_name(
					generic_media_server_client_session->our_server_media_session))[url_pre_suffix_len] == '/' &&
				strcmp(&(livemedia_server_media_session__stream_name(
							generic_media_server_client_session->our_server_media_session))[url_pre_suffix_len+1], url_suffix) == 0) {
			subsession = NULL;
		} else {
			livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(our_client_connection);
			return;
		}
	} else { /* the request doesn't match a known stream and/or track at all! */
		livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(our_client_connection);
		return;
	}

	if (strcmp(cmd_name, "TEARDOWN") == 0) {
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down(rtsp_server_rtsp_client_session,
				our_client_connection, subsession);
	} else if (strcmp(cmd_name, "PLAY") == 0) {
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_play(rtsp_server_rtsp_client_session,
				our_client_connection, subsession, full_request_str);
	} else if (strcmp(cmd_name, "PAUSE") == 0) {
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause(rtsp_server_rtsp_client_session,
				our_client_connection, subsession);
	} else if (strcmp(cmd_name, "GET_PARAMETER") == 0) {
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter(rtsp_server_rtsp_client_session,
				our_client_connection, subsession, full_request_str);
	} else if (strcmp(cmd_name, "SET_PARAMETER") == 0) {
		livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter(rtsp_server_rtsp_client_session,
				our_client_connection, subsession, full_request_str);
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_play__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	char *rtsp_url;
	unsigned int rtsp_url_size;
	float scale;
	bool saw_scale_header;
	char buf[100];
	char *scale_header;
	float duration;
	double range_start;
	double range_end;
	char *abs_start;
	char *abs_end;
	bool start_time_is_now;
	bool saw_range_header;
	double tmp;
	char *rtp_info_fmt;
	unsigned int rtp_info_fmt_size;
	char *rtp_info;
	unsigned int i;
	unsigned int num_rtp_info_items;
	double stream_duration;
	float cur_npt;
	float npt;
	char *range_header;
	unsigned short rtp_seq_num;
	unsigned int rtp_timestamp;
	char *url_suffix;
	char *prev_rtp_info;
	unsigned int rtp_info_size;
	unsigned int rtp_info_len;
	u_int64_t num_bytes;


	livemedia_generic_media_server_client_session_t *generic_media_server_client_session;

	generic_media_server_client_session = 
		(livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session;

	rtsp_url = livemedia_rtsp_server__rtsp_url(rtsp_server_rtsp_client_session->our_rtsp_server,
			generic_media_server_client_session->our_server_media_session,
			our_client_connection->client_input_socket);
	rtsp_url_size = strlen(rtsp_url);
  
	/* Parse the client's "Scale:" header, if any: */
	saw_scale_header = livemedia__parse_scale_header(full_request_str, &scale);
	
	/* Try to set the stream's scale factor to this value: */
	if (subsession == NULL /*aggregate op*/) {
		livemedia_server_media_session__test_scale_factor(
				generic_media_server_client_session->our_server_media_session, &scale);
	} else {
		livemedia_server_media_subsession__test_scale_factor(subsession, &scale);
	}

	if (!saw_scale_header) {
		buf[0] =  '\0';	/* Because we didn't see a Scale: header, don't send one back */
	} else {
		sprintf(buf, "Scale: %f\r\n", scale);
	}
	scale_header = livemedia__str_dup(buf);

	/* Parse the client's "Range:" header, if any: */
	duration = 0.0;
	range_start = 0.0;
	range_end = 0.0;
	abs_start = NULL;
	abs_end = NULL;
	saw_range_header = livemedia__parse_range_header(full_request_str, 
			&range_start, &range_end, &abs_start, &abs_end, &start_time_is_now);

	if (saw_range_header && abs_start == NULL /* not seeking by 'absolute' time*/) {
		/* Use this information, plus the stream's duration (if known), to create our own "Range:" header, for the response: */
		duration = subsession == NULL /*aggregate op*/
			? livemedia_server_media_session__duration(
					generic_media_server_client_session->our_server_media_session) :
			livemedia_server_media_subsession__duration(subsession);
		if (duration < 0.0) {
			/* We're an aggregate PLAY, but the subsessions have different durations.
			 * Use the largest of these durations in our header */
			duration = -duration;
		}
    
		/* Make sure that "rangeStart" and "rangeEnd" (from the client's "Range:" header)
		 * have sane values, before we send back our own "Range:" header in our response: */
		if (range_start < 0.0)
			range_start = 0.0;
		else if (range_start > duration)
			range_start = duration;
		if (range_end < 0.0)
			range_end = 0.0;
		else if (range_end > duration)
			range_end = duration;
		if ((scale > 0.0 &&  range_start > range_end && range_end > 0.0) ||
				(scale < 0.0 && range_start < range_end)) {
			/* "rangeStart" and "rangeEnd" were the wrong way around; swap them: */ 
			tmp = range_start;
			range_start = range_end;
			range_end = tmp;
		}
	}


	/* Create a "RTP-Info:" line.  It will get filled in from each subsession's state: */
	rtp_info_fmt = 
    "%s" /* "RTP-Info:", plus any preceding rtpInfo items */
    "%s" /* comma separator, if needed */
    "url=%s/%s"
    ";seq=%d"
    ";rtptime=%u"
    ;
	rtp_info_fmt_size = strlen(rtp_info_fmt);
	rtp_info = livemedia__str_dup("RTP-Info: ");
	num_rtp_info_items = 0;
	
	/* Do any required seeking/scaling on each subsession, before starting streaming.
	 * (However, we don't do this if the "PLAY" request was for just a single subsession
	 * of a multiple-subsession stream; for such streams, seeking/scaling can be done
	 * only with an aggregate "PLAY".) */
	for (i = 0; i < rtsp_server_rtsp_client_session->num_stream_states; ++i) {
		if (subsession == NULL /* means: aggregated operation */ || rtsp_server_rtsp_client_session->num_stream_states == 1) {
			if (rtsp_server_rtsp_client_session->stream_states[i].subsession != NULL) {
				if (saw_scale_header) {
					livemedia_server_media_subsession__set_stream_scale(
							rtsp_server_rtsp_client_session->stream_states[i].subsession,
							generic_media_server_client_session->our_session_id,
							rtsp_server_rtsp_client_session->stream_states[i].stream_token, scale);
				}
				if (abs_start != NULL) {
					/* Special case handling for seeking by 'absolute' time: */

					livemedia_server_media_subsession__seek_stream_absolute(
							rtsp_server_rtsp_client_session->stream_states[i].subsession,
							generic_media_server_client_session->our_session_id,
							rtsp_server_rtsp_client_session->stream_states[i].stream_token,
							&abs_start, &abs_end);
				} else {
					/* Seeking by relative (NPT) time: */

					if (!saw_range_header || start_time_is_now) {
						/* We're resuming streaming without seeking, so we just do a 'null' seek
						 * (to get our NPT, and to specify when to end streaming): */
						livemedia_server_media_subsession__null_seek_stream(
								rtsp_server_rtsp_client_session->stream_states[i].subsession,
								generic_media_server_client_session->our_session_id,
								rtsp_server_rtsp_client_session->stream_states[i].stream_token,
								range_end, &num_bytes);
					} else {
						/* We do a real 'seek': */
						stream_duration = 0.0; /* by default; means: stream until the end of the media */
						if (range_end > 0.0 && (range_end + 0.001) < duration) {
							/* the 0.001 is because we limited the values to 3 decimal places
							 * We want the stream to end early.  Set the duration we want: */
							stream_duration = range_end - range_start;
							if (stream_duration < 0.0)
								stream_duration = -stream_duration;
							/* should happen only if scale < 0.0 */
						}
						livemedia_server_media_subsession__seek_stream_relative(
								rtsp_server_rtsp_client_session->stream_states[i].subsession,
								generic_media_server_client_session->our_session_id,
								rtsp_server_rtsp_client_session->stream_states[i].stream_token,
								&range_start, stream_duration, &num_bytes);
					}
				}
			}
		}
	}

	/* Create the "Range:" header that we'll send back in our response.
	 * (Note that we do this after seeking, in case the seeking operation changed the range start time.) */
	if (abs_start != NULL) {
		/* We're seeking by 'absolute' time: */
		 if (abs_end == NULL) {
			 sprintf(buf, "Range: clock=%s-\r\n", abs_start);
		 } else {
			 sprintf(buf, "Range: clock=%s-%s\r\n", abs_start, abs_end);
		 }
		 if (abs_start) {
			 free(abs_start);
			 abs_start = NULL;
		 }
		 if (abs_end) {
			 free(abs_end);
			 abs_end = NULL;
		 }
	} else {
		/* We're seeking by relative (NPT) time: */
		if (!saw_range_header || start_time_is_now) {
			/* We didn't seek, so in our response, begin the range with the current NPT (normal play time): */
			cur_npt = 0.0;
			for (i = 0; i < rtsp_server_rtsp_client_session->num_stream_states; ++i) {
				if (subsession == NULL /* means: aggregated operation */
						|| subsession == rtsp_server_rtsp_client_session->stream_states[i].subsession) {
					npt = livemedia_server_media_subsession__get_current_npt(
							rtsp_server_rtsp_client_session->stream_states[i].subsession,
							rtsp_server_rtsp_client_session->stream_states[i].stream_token);
					if (npt > cur_npt)
						cur_npt = npt;
					/* Note: If this is an aggregate "PLAY" on a multi-subsession stream,
					 * then it's conceivable that the NPTs of each subsession may differ
					 * (if there has been a previous seek on just one subsession).
					 * In this (unusual) case, we just return the largest NPT; I hope that turns out OK... */
				}
			}
			range_start = cur_npt;
		}

		if (range_end == 0.0 && scale >= 0.0) {
			sprintf(buf, "Range: npt=%.3f-\r\n", range_start);
		} else {
			sprintf(buf, "Range: npt=%.3f-%.3f\r\n", range_start, range_end);
		}
	}
	range_header = livemedia__str_dup(buf);

	/* Now, start streaming: */
	for (i = 0; i < rtsp_server_rtsp_client_session->num_stream_states; ++i) {
		if (subsession == NULL /*means: aggregated operation */
				|| subsession == rtsp_server_rtsp_client_session->stream_states[i].subsession) {
			rtp_seq_num = 0;
			rtp_timestamp = 0;
			if (rtsp_server_rtsp_client_session->stream_states[i].subsession == NULL)
				continue;
			livemedia_server_media_subsession__start_stream(
					rtsp_server_rtsp_client_session->stream_states[i].subsession,
					generic_media_server_client_session->our_session_id,
					(void *)rtsp_server_rtsp_client_session->stream_states[i].stream_token,
					(usage_environment__task_func *)livemedia_generic_media_server_client_session__note_client_liveness__static,
					(void *)rtsp_server_rtsp_client_session, &rtp_seq_num, &rtp_timestamp,
					livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte__static,
					(void *)our_client_connection);
			url_suffix = livemedia_server_media_subsession__track_id(rtsp_server_rtsp_client_session->stream_states[i].subsession);
			prev_rtp_info = rtp_info;
			rtp_info_size = rtp_info_fmt_size
				+ strlen(prev_rtp_info)
				+ 1
				+ rtsp_url_size + strlen(url_suffix)
				+ 5 /*max unsigned short len*/
				+ 10 /*max unsigned (32-bit) len*/
				+ 2 /*allows for trailing \r\n at final end of string*/;
			rtp_info = (char *)malloc(sizeof(char)*rtp_info_size);
			if (rtp_info == NULL) {
				fprintf(stderr, "malloc(sizeof(char)*rtp_info_size)"
						"failed in %s()\n", __func__);
			}
			sprintf(rtp_info, rtp_info_fmt,
					prev_rtp_info,
					num_rtp_info_items++ == 0 ? "" : ",",
					rtsp_url, url_suffix,
					rtp_seq_num,
					rtp_timestamp
				   );
			if (prev_rtp_info) {
				free(prev_rtp_info);
				prev_rtp_info = NULL;
			}
		}
	}
	if (num_rtp_info_items == 0) {
		rtp_info[0] = '\0';
	} else {
		rtp_info_len = strlen(rtp_info);
		rtp_info[rtp_info_len] = '\r';
		rtp_info[rtp_info_len + 1] = '\n';
		rtp_info[rtp_info_len + 2] = '\0';
	}

	/* Fill in the response: */
	snprintf((char*)((livemedia_generic_media_server_client_connection_t *)our_client_connection)->response_buffer,
			sizeof(((livemedia_generic_media_server_client_connection_t *)our_client_connection)->response_buffer),
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %s\r\n"
			"%s"
			"%s"
			"%s"
			"Session: %08X\r\n"
			"%s\r\n",
			our_client_connection->current_cseq,
			livemedia__date_header(),
			scale_header,
			range_header,
			generic_media_server_client_session->our_session_id,
			rtp_info);
	if (rtp_info) {
		free(rtp_info);
		rtp_info = NULL;
	}
	if (range_header) {
		free(range_header);
		range_header = NULL;
	}
	if (scale_header) {
		free(scale_header);
		scale_header = NULL;
	}
	if (rtsp_url) {
		free(rtsp_url);
		rtsp_url = NULL;
	}
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}


void livemedia_rtsp_server_rtsp_client_session__delete__impl(
		livemedia_generic_media_server_client_session_t *generic_media_server_client_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session;

	rtsp_server_rtsp_client_session = 
		(livemedia_rtsp_server_rtsp_client_session_t *)generic_media_server_client_session;

	  if (rtsp_server_rtsp_client_session) {
	  __livemedia_rtsp_server_rtsp_client_session__deinit(rtsp_server_rtsp_client_session);
	  __livemedia_rtsp_server_rtsp_client_session__free(rtsp_server_rtsp_client_session);
	  }
}

/*
 * Functions in header file
 */
bool livemedia_rtsp_server_rtsp_client_session__is_multicast(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_session_id(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, u_int32_t session_id)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_content_str(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, char const *content_str)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_session_id_and_content_str(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, u_int32_t session_id, char const *content_str)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtsp_server_rtsp_client_session__uses_tcp_transport(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}


/*
 * Normal functions
 */
void livemedia_rtsp_server_rtsp_client_session__delete_stream_by_track(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		unsigned int track_num)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtsp_server_rtsp_client_session__reclaim_stream_states(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int i;

	for (i = 0; i <rtsp_server_rtsp_client_session->num_stream_states; ++i) {
		if (rtsp_server_rtsp_client_session->stream_states[i].subsession != NULL) {
			livemedia_rtsp_server__unnote_tcp_streaming_on_socket(
					rtsp_server_rtsp_client_session->our_rtsp_server,
					rtsp_server_rtsp_client_session->stream_states[i].tcp_socket_num,
					rtsp_server_rtsp_client_session, i);
		}
	}
	if (rtsp_server_rtsp_client_session->stream_states){
		free(rtsp_server_rtsp_client_session->stream_states);
		rtsp_server_rtsp_client_session->stream_states = NULL;
	}
	rtsp_server_rtsp_client_session->num_stream_states = 0;

}


