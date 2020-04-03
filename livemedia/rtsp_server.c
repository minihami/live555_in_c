#include "rtsp_server.h"
#include "../groupsock/groupsock_helper.h"


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
		livemedia__delete__streaming_over_tcp_record(rtsp_server->tcp_streaming_database);

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
	if(rtsp_server && rtsp_server->allowed_command_names)
		rtsp_server->allowed_command_names(rtsp_server);
}
bool livemedia_rtsp_server__we_implement_register(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd, char const *proxy_url_suffix, char **response_str)
{
	if(rtsp_server && rtsp_server->we_implement_register)
		rtsp_server->we_implement_register(rtsp_server, cmd, proxy_url_suffix, 
				response_str);
}
void livemedia_rtsp_server__implement_cmd_register(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd, char const *url, char const *url_suffix, 
		int socket_to_remote_server, bool deliver_via_tcp, char const *proxy_url_suffix)
{
	if(rtsp_server && rtsp_server->implement_cmd_register)
		rtsp_server->implement_cmd_register(rtsp_server, cmd, url, url_suffix, 
				socket_to_remote_server, deliver_via_tcp, proxy_url_suffix);

}
livemedia_user_authentication_database_t *livemedia_rtsp_server__get_authentication_database_for_command(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name)
{
	if(rtsp_server && rtsp_server->get_authentication_database_for_command)
		rtsp_server->get_authentication_database_for_command(rtsp_server, cmd_name);
}
bool livemedia_rtsp_server__special_client_access_check(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix)
{
	if(rtsp_server && rtsp_server->special_client_access_check)
		rtsp_server->special_client_access_check(rtsp_server, client_socket, client_addr,
				url_suffix);
}
bool livemedia_rtsp_server__special_client_user_access_check(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username)
{
	if(rtsp_server && rtsp_server->special_client_user_access_check)
		rtsp_server->special_client_user_access_check(rtsp_server, client_socket,
				client_addr, url_suffix, username);
}

/*
 * Override functions
 */
char const *livemedia_rtsp_server__allowed_command_names__impl(livemedia_rtsp_server_t *rtsp_server)
{
	return "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER";
}
bool livemedia_rtsp_server__we_implement_register__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd, 
		char const *proxy_url_suffix, char **response_str)
{
	/* in RTSPServer, No Definition */
}
void livemedia_rtsp_server__implement_cmd_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd, char const *url, char const *url_suffix, 
		int socket_to_remote_server, bool deliver_via_tcp, 
		char const *proxy_url_suffix)
{
	/* in RTSPServer, No Definition */
}
livemedia_user_authentication_database_t *livemedia_rtsp_server__get_authentication_database_for_command__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name)
{
	/* default implementation */
	return rtsp_server->auth_db;
}

bool livemedia_rtsp_server__special_client_access_check__impl(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix)
{
	/* default implementation */
	return true;
}

bool livemedia_rtsp_server__special_client_user_access_check__impl(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket, struct sockaddr_in *client_addr,
		char const* url_suffix, char const *username)
{
	/* default implementation; no further access restrictions: */
	return true;
}

bool livemedia_rtsp_server__is_rtsp_server__impl(livemedia_medium_t *medium)
{
	return true;
}

void livemedia_rtsp_server__delete__impl(livemedia_medium_t *medium)
{
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
void livemedia_rtsp_server__diable_stream_rtp_over_tcp(livemedia_rtsp_server_t *rtsp_server)
{
	return rtsp_server->allow_streaming_rtp_over_tcp;
}

/*
 * Normal functions
 */
livemedia_rtsp_server_t *livemedia_rtsp_server__create_new__static(
		groupsock_port_t *our_port, livemedia_user_authentication_database_t *auth_database,
		unsigned int reclamation_seconds)
{
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
	*result_server = NULL; /* unless we succeed */

	livemedia_medium_t *medium;
	if (!livemedia_medium__lookup_by_name__static(name, medium)) 
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
	/* Create a new "DeregisterRequestRecord" that will send the "DEREGISTER" command.
	 * (This object will automatically get deleted after we get a response to the "DEREGISTER" command, or if we're deleted.) */
	/* TODO: RegisterRequestRecord*/
	return -1;
}

char *livemedia_rtsp_server__rtsp_url(livemedia_rtsp_server_t *rtsp_server,
		livemedia_server_media_session_t const *server_media_session, uv_tcp_t *client_socket)
{
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

	if(url_prefix) {
		free(url_prefix);
		url_prefix = NULL;
	}
	return result_url;
}

char *livemedia_rtsp_server__rtsp_url_prefix(livemedia_rtsp_server_t *rtsp_server,
		uv_tcp_t *client_socket)
{
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
	livemedia_user_authentication_database_t *old_db;
	old_db = rtsp_server->auth_db;
	rtsp_server->auth_db = new_db;
	return old_db;
}

bool livemedia_rtsp_server__set_up_tunneling_over_http(livemedia_rtsp_server_t *rtsp_server, 
		groupsock_port_t http_port)
{
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
	return ntohs(groupsock_port__num(&rtsp_server->http_server_port));
}

void livemedia_rtsp_server__incoming_connection_handler_http__static(void *instance)
{
	livemedia_rtsp_server_t *server;
	server = (livemedia_rtsp_server_t *)instance;
	livemedia_rtsp_server__incoming_connection_handler_http(server);
}

void livemedia_rtsp_server__incoming_connection_handler_http(
		livemedia_rtsp_server_t *rtsp_server)
{
	livemedia_generic_media_server__incoming_connection_handler_on_socket(
			(livemedia_generic_media_server_t *)rtsp_server,
			rtsp_server->http_server_socket);
}

void livemedia_rtsp_server__note_tcp_streaming_on_socket(
		livemedia_rtsp_server_t *rtsp_server, int socket_num,
		livemedia_rtsp_server_rtsp_client_session_t *client_session, 
		unsigned int track_num)
{
	livemedia_streaming_over_tcp_record_t *sotcp_cur;
	livemedia_streaming_over_tcp_record_t *sotcp_new;
	sotcp_cur = (livemedia_streaming_over_tcp_record_t *)livemedia_hash_table__lookup(
			rtsp_server->tcp_streaming_database, (char const *)socket_num);
	sotcp_new = livemedia__new__streaming_over_tcp_record(((livemedia_generic_media_server_client_session_t *)client_session)->
			our_session_id, track_num, sotcp_new);
	livemedia_hash_table__add(rtsp_server->tcp_streaming_database,
			(char const *)socket_num, sotcp_new);
}

void livemedia_rtsp_server__unnote_tcp_streaming_on_socket(
		livemedia_rtsp_server_t *rtsp_server, int socket_num,
		livemedia_rtsp_server_rtsp_client_session_t *client_session,
		unsigned int track_num)
{
	livemedia_streaming_over_tcp_record_t *sotcp_head;
	livemedia_streaming_over_tcp_record_t *sotcp;
	livemedia_streaming_over_tcp_record_t *sotcp_prev;

	if (socket_num < 0)
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
		livemedia_rtsp_server_t *rtsp_server, int socket_num)
{
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
			our_server, client_socket, client_addr);

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
	/*((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->
		create_new_client_connection = 
		livemedia_rtsp_server_rtsp_client_connection__create_new_client_connection__impl;*/

	rtsp_server_rtsp_client_connection->generic_media_server_client_connection.delete =
		livemedia_rtsp_server_rtsp_client_connection__delete__impl;
}
void __livemedia_rtsp_server_rtsp_client_connection__deinit(livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if (rtsp_server_rtsp_client_connection->our_session_cookie != NULL) {
		/* We were being used for RTSP-over-HTTP tunneling. Also remove ourselves from the 'session cookie' hash table before we go: */
		livemedia_hash_table__remove(
				rtsp_server_rtsp_client_connection->our_rtsp_server->client_connections_for_http_tunneling,
				rtsp_server_rtsp_client_connection->our_session_cookie);

		if(rtsp_server_rtsp_client_connection->our_session_cookie) {
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
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_options)
		rtsp_server_rtsp_client_connection->handle_cmd_options(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_get_parameter(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *full_request_str)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_get_parameter)
		rtsp_server_rtsp_client_connection->handle_cmd_get_parameter(
				rtsp_server_rtsp_client_connection, full_request_str);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_set_parameter(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection, 
		char const *full_request_str)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_set_parameter)
		rtsp_server_rtsp_client_connection->handle_cmd_set_parameter(
				rtsp_server_rtsp_client_connection, full_request_str);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_describe)
		rtsp_server_rtsp_client_connection->handle_cmd_describe(
				rtsp_server_rtsp_client_connection, url, url_suffix, full_request_str,
				reuse_connection, deliver_via_tcp, proxy_url_suffix);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd /*"REGISTER" or "DEREGISTER"*/,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_register)
		rtsp_server_rtsp_client_connection->handle_cmd_register(
				rtsp_server_rtsp_client_connection, cmd, url, url_suffix, full_request_str,
				reuse_connection, deliver_via_tcp, proxy_url_suffix);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_bad)
		rtsp_server_rtsp_client_connection->handle_cmd_bad(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_not_supported)
		rtsp_server_rtsp_client_connection->handle_cmd_not_supported(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_not_found)
		rtsp_server_rtsp_client_connection->handle_cmd_not_found(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_session_not_found)
		rtsp_server_rtsp_client_connection->handle_cmd_session_not_found(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_cmd_unsupported_transport)
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
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->parse_http_request_string)
		rtsp_server_rtsp_client_connection->parse_http_request_string(
				rtsp_server_rtsp_client_connection, result_cmd_name, result_cmd_name_max_size,
				url_suffix, url_suffix_max_size, 
				session_cookie, session_cookie_max_size,
				accept_str, accept_str_max_size);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_http_cmd_not_supported)
		rtsp_server_rtsp_client_connection->handle_http_cmd_not_supported(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_http_cmd_not_found)
		rtsp_server_rtsp_client_connection->handle_http_cmd_not_found(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_http_cmd_options)
		rtsp_server_rtsp_client_connection->handle_http_cmd_options(
				rtsp_server_rtsp_client_connection);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_get)
		rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_get(
				rtsp_server_rtsp_client_connection, session_cookie);
}
bool livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_post)
		rtsp_server_rtsp_client_connection->handle_http_cmd_tunneling_post(
				rtsp_server_rtsp_client_connection, session_cookie, extra_data, extra_data_size);
}
void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_suffix, char const *full_request_str)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->handle_http_cmd_streaming_get)
		rtsp_server_rtsp_client_connection->handle_http_cmd_streaming_get(
				rtsp_server_rtsp_client_connection, url_suffix, full_request_str);
}

void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params)
{
	if(rtsp_server_rtsp_client_connection && 
			rtsp_server_rtsp_client_connection->continue_handling_register1)
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
	/* By default, we implement "SET_PARAMETER" (on the entire server) just as a 'no op', and send back an empty response.
	 * (If you want to handle this type of "SET_PARAMETER" differently, you can do so by defining a subclass of "RTSPServer"
	 * and "RTSPServer::RTSPClientConnection", and then reimplement this virtual function in your subclass.) */
	livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
			rtsp_server_rtsp_client_connection, "200 OK");
}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_describe__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{

}
void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_register__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd /*"REGISTER" or "DEREGISTER"*/,
		char const *url, char const *url_suffix, char const *full_request_str,
		bool reuse_connection, bool deliver_via_tcp, char const *proxy_url_suffix)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_bad__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_supported__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_session_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_cmd_unsupported_transport__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

/* Support for optional RTSP-over-HTTP tunneling: */
bool livemedia_rtsp_server_rtsp_client_connection__parse_http_request_string__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char *result_cmd_name, unsigned int result_cmd_name_max_size,
		char *url_suffix, unsigned int url_suffix_max_size,
		char *session_cookie, unsigned int session_cookie_max_size,
		char *accept_str, unsigned int accept_str_max_size)
{
	return false;
}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_supported__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_not_found__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_options__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_get__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie)
{

}

bool livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_tunneling_post__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *session_cookie, unsigned char const *extra_data, unsigned int extra_data_size)
{
	return false;
}

void livemedia_rtsp_server_rtsp_client_connection__handle_http_cmd_streaming_get__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *url_suffix, char const *full_request_str)
{

}


void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register1__impl(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params)
{

}

void livemedia_rtsp_server_rtsp_client_connection__handle_request_bytes__impl(
		livemedia_generic_media_server_client_connection_t *generic_media_server_client_connection,
		int new_bytes_read)
{
	int num_bytes_remaining;
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
	bool const request_included_session_id;
	bool are_authenticated;
	char url_total_suffix[2*RTSP_PARAM_STRING_MAX];
	char *url;
	bool reuse_connection, deliver_via_tcp;
	char *proxy_url_suffix;
	char session_cookies[RTSP_PARAM_STRING_MAX];
	char accept_str[RTSP_PARAM_STRING_MAX];
	bool is_valid_http_cmd;
	unsigned char const *extra_data;
	unsigned int extra_data_size;
	unsigned request_size;

	num_bytes_remaining = 0;
	++generic_media_server_client_connection->recursion_count;
	do {
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
			((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->is_active = false
			break;
		}

		end_of_msg = false;
		ptr = &generic_media_server_client_connection->
			request_buffer[generic_media_server_client_connection->request_bytes_already_seen];
#ifdef DEBUG
		ptr[new_bytes_read] = '\0';
		fprintf(stderr, "RTSPClientConnection[%p]::handleRequestBytes() %s %d new bytes:%s\n",
				generic_media_server_client_connection, num_bytes_remaining > 0 ? "processing" : "read", 
				new_bytes_read, ptr);
#endif

#if 0
		if (((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->
				client_output_socket != 
				((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->
				client_input_socket && num_bytes_remaining == 0) {
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

		if (!end_of_msg)
			break;	/* subsequent reads will be needed to complete the request */

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
				content_length);
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
			if (ptr + new_bytes_read < tmp_ptr + 2 + content_length)
				break; /* we still need more data; subsequent reads will give it to us */

			/* If the request included a "Session:" id, and it refers to a client session that's
			 * current ongoing, then use this command to indicate 'liveness' on that client session: */
			request_included_session_id = session_id_str[0] != '\0';
			if (request_included_session_id) {
				client_session = (livemedia_rtsp_server_rtsp_client_session_t *)
					livemedia_generic_media_server__lookup_client_session_by_session_id_str(
							((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->our_server, session_id_str);
				if (client_session != NULL)
					livemedia_generic_media_server_client_session__note_liveness(
							(livemedia_generic_media_server_client_session_t *)client_session);
				/* We now have a complete RTSP request.
				 * Handle the specified command (beginning with commands that are session-independent): */
				((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->current_cseq = cseq;
				if (strcmp(cmd_name, "OPTIONS") == 0) {
					/* If the "OPTIONS" command included a "Session:" id for a session that doesn't exist,
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
					/*TODO: RTSPServer.c 751 line*/
				}
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
		send(((livemedia_rtsp_server_rtsp_client_connection_t *)
					generic_media_server_client_connection)->client_output_socket, 
				(char const *)generic_media_server_client_connection->response_buffer,
				strlen((char *)generic_media_server_client_connection->response_buffer), 0);
		/*TODO: RTSPServer.c 887*/

		request_size = 
			((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection->last_crlf
			+ 4 
			+ generic_media_server_client_connection->request_buffer)
			+ content_length;
		num_bytes_remaining = generic_media_server_client_connection->request_bytes_already_seen - request_size;
		livemedia_rtsp_server_rtsp_client_connection__reset_request_buffer(
				(livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection);
		/*to prepare for any subsequence request */

		if (num_bytes_remaining > 0) {
			memmove(generic_media_server_client_connection->request_buffer,
					&generic_media_server_client_connection->request_buffer[request_size],
					num_bytes_remaining);
			new_bytes_read = num_bytes_remaining;
		}
	} while (num_bytes_remaining >0);


	--(((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->recursion_count);
	if (((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->is_active) {
		if (((livemedia_rtsp_server_rtsp_client_connection_t *)generic_media_server_client_connection)->recursion_count > 0) {
			livemedia_generic_media_server_client_connection__close_sockets(generic_media_server_client_connection);
			/* Note: The "fRecursionCount" test is for a pathological situation where we reenter the event loop and get called recursively
			 * while handling a command (e.g., while handling a "DESCRIBE", to get a SDP description).
			 * In such a case we don't want to actually delete ourself until we leave the outermost call. */
		}
}

livemedia_generic_media_server_client_connection_t *livemedia_rtsp_server_rtsp_client_connection__create_new_client_connection__impl(
		livemedia_generic_media_server_t *generic_media_server,
		uv_tcp_t *client_socket, struct sockaddr_in client_addr)
{
	return livemedia__new__rtsp_server_rtsp_client_connection(
			(livemedia_rtsp_server_t *)generic_media_server,
			client_socket, client_addr);
}

void livemedia_rtsp_server_rtsp_client_connection__delete__impl(livemedia_medium_t *medium)
{

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
	livemedia_generic_media_server_client_connection__reset_request_buffer(
			(livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection);
	rtsp_server_rtsp_client_connection->last_crlf = &((livemedia_generic_media_server_client_connection_t *)rtsp_server_rtsp_client_connection)->request_buffer[-3];
	rtsp_server_rtsp_client_connection->base64_remainder_count = 0; 
}

void livemedia_rtsp_server_rtsp_client_connection__close_socket_rtsp(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection)
{

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
	livemedia_rtsp_server_rtsp_client_connection_t *connection;

	connection = (livemedia_rtsp_server_rtsp_client_connection_t *)instance;
	livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte1(connection, request_byte);
}

void livemedia_rtsp_server_rtsp_client_connection__handle_alternative_request_byte1(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		u_int8_t request_byte)
{
	if(request_byte == 0xFF) {
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
	/* TODO:
	   char const *fields;
	   char *parameter;
	   char *value;
	   char *p;
	   bool success;

	 * Initialize the result parameters to default values: 
	 username = realm = nonce = uri = response = NULL;

	 First, find "Authorization:" 
	 while (1) {
	 if (*buf == '\0')
	 return false;  not found 
	 if (_strncasecmp(buf, "Authorization: Digest ", 22) == 0)
	 break;
	 ++buf;
	 }

	 Then, run through each of the fields, looking for ones we handle:
	 fields = buf + 22;
	 parameter = livemedia__str_dup(fields);
	 value = livemedia__str_dup(fields);
	 do {
Parse: <parameter>="<value>"
success = false;
parameter[0] = value[0] = '\0';
SKIP_WHITESPACE;
for (p = parameter; *fields != '\0' && *fields != ' ' && *fields != '\t' && *fields != '='; )*p++ = *fields++;
SKIP_WHITESPACE;
if (*fields++ != '=')
break;

}
*/
return false;
}

bool livemedia_rtsp_server_rtsp_client_connection__authentication_ok(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *cmd_name, char const * url_suffix, char const *full_request_str)
{
#if 0
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
				rtsp_server_rtsp_client_connection->client_addr,
				url_suffix)) {
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
		/* TODO: DigestAuthentication.cpp
		   if (fCurrentAuthenticator.nonce() == NULL) break;
		   */

		/* Next, the request needs to contain an "Authorization:" header,
		 * containing a username, (our) realm, (our) nonce, uri,
		 * and response string: */
		if (!livemedia_rtsp_server_rtsp_client_connection__parse_autho

				}
#endif
				}

				void livemedia_rtsp_server_rtsp_client_connection__change_client_input_socket(
					livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
					int new_socket_num, unsigned char const *extra_data, unsigned int extra_data_size)
				{

				}

				/* used to implement RTSP-over-HTTP tunneling */
				void livemedia_rtsp_server_rtsp_client_connection__continue_handling_register__static(
					livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *params)
				{

				}


/* Shortcuts for setting up a RTSP response (prior to sending it): */
void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str)
{

}

void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_session_id(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, u_int32_t session_id)
{

}

void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_content_str(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, char const *content_str)
{

}

void livemedia_rtsp_server_rtsp_client_connection__set_rtsp_response_by_session_id_and_content_str(
		livemedia_rtsp_server_rtsp_client_connection_t *rtsp_server_rtsp_client_connection,
		char const *response_str, u_int32_t session_id, char const *content_str)
{

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
	/* TODO: parent init */

	/* TODO: constructor */

	/* TODO: virtual functions configuration */ 

	rtsp_server_rtsp_client_connection_params_for_register->delete = livemedia_rtsp_server_rtsp_client_connection_params_for_register__delete__impl;
}
void __livemedia_rtsp_server_rtsp_client_connection_params_for_register__deinit(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register)
{
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
void livemedia__delete__rtsp_server_rtsp_client_connection_params_for_register(
		livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register);

/*
 * Override functions
 */
void livemedia_rtsp_server_rtsp_client_connection_params_for_register__delete__impl(livemedia_rtsp_server_rtsp_client_connection_params_for_register_t *rtsp_server_rtsp_client_connection_params_for_register)
{
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
	/* TODO: parent init */

	/* TODO: constructor */

	/* TODO: virtual functions configuration */ 
	((livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session)->
		create_new_client_session = 
		livemedia_rtsp_server_rtsp_client_session__create_new_client_session__impl;

	((livemedia_generic_media_server_client_session_t *)rtsp_server_rtsp_client_session)->delete =
		livemedia_rtsp_server_rtsp_client_session__delete__impl;

}
void __livemedia_rtsp_server_rtsp_client_session__deinit(livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
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
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char const *url_pre_suffix, char const *url_suffix, char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
		char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_play(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
}


/*
 * Override functions
 */
void livemedia_rtsp_server_rtsp_client_session__handle_cmd_setup__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char const *url_pre_suffix, char const *url_suffix, char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_within_session__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		char const *cmd_name, char const *url_pre_suffix, char const *url_suffix,
		char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_tear_down__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_play__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_pause__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_get_parameter__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
}

void livemedia_rtsp_server_rtsp_client_session__handle_cmd_set_parameter__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connecion,
		livemedia_server_media_subsession_t *subsession, char const *full_request_str)
{
}

livemedia_generic_media_server_client_session_t *livemedia_rtsp_server_rtsp_client_session__create_new_client_session__impl(
		livemedia_generic_media_server_t *generic_media_server, u_int32_t session_id)
{

}

void livemedia_rtsp_server_rtsp_client_session__delete__impl(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
	/*TODO: if child, get parent as parameter and cast to child */
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
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str)
{
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_session_id(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, u_int32_t session_id)
{
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_content_str(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, char const *content_str)
{
}

bool livemedia_rtsp_server_rtsp_client_session__set_rtsp_response_by_session_id_and_content_str(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection,
		livemedia_rtsp_server_rtsp_client_connection_t *our_client_connection,
		char const *respons_str, u_int32_t session_id, char const *content_str)
{
}

bool livemedia_rtsp_server_rtsp_client_session__uses_tcp_transport(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_connection)
{
}


/*
 * Normal functions
 */
void livemedia_rtsp_server_rtsp_client_session__delete_stream_by_track(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session,
		unsigned int track_num)
{
}

void livemedia_rtsp_server_rtsp_client_session__reclaim_stream_states(
		livemedia_rtsp_server_rtsp_client_session_t *rtsp_server_rtsp_client_session)
{
}


/*****************************************************************
 * RTSPServerWithREGISTERProxying:RTSPServer (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtsp_server_with_register_proxying_t *__livemedia_rtsp_server_with_register_proxying__alloc(void)
{
	livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying;
	rtsp_server_with_register_proxying = (livemedia_rtsp_server_with_register_proxying_t *)calloc(1, sizeof(livemedia_rtsp_server_with_register_proxying_t));
	if (rtsp_server_with_register_proxying == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtsp_server_with_register_proxying_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtsp_server_with_register_proxying;
}
void __livemedia_rtsp_server_with_register_proxying__init(
		livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying,
		groupsock_port_t our_port, livemedia_user_authentication_database_t *auth_database,
		livemedia_user_authentication_database_t *auth_database_for_register,
		unsigned int reclamation_seconds,
		bool stream_rtp_over_tcp,
		int verbosity_level_for_proxying,
		char const *back_end_username,
		char const *back_end_password)
{
	/* TODO: parent init */

	/* TODO: constructor */

	/* TODO: virtual functions configuration */ 

	((livemedia_medium_t *)rtsp_server_with_register_proxying)->delete = livemedia_rtsp_server_with_register_proxying__delete__impl;

	rtsp_server_with_register_proxying->rtsp_server.generic_media_server.medium.delete = livemedia_rtsp_server_with_register_proxying__delete__impl;
}
void __livemedia_rtsp_server_with_register_proxying__deinit(livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_rtsp_server_with_register_proxying__free(livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying)
{
	if (rtsp_server_with_register_proxying) {
		free(rtsp_server_with_register_proxying);
		rtsp_server_with_register_proxying = NULL;
	}
}
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
		char const *back_end_password)
{
	livemedia_rtsp_server_with_register_proxying_t *rtsp_server_with_register_proxying;

	rtsp_server_with_register_proxying = __livemedia_rtsp_server_with_register_proxying__alloc();
	if (rtsp_server_with_register_proxying)
		__livemedia_rtsp_server_with_register_proxying__init(
				rtsp_server_with_register_proxying, our_port,
				auth_database, auth_database_for_register,
				reclamation_seconds, stream_rtp_over_tcp, verbosity_level_for_proxying,
				back_end_username, back_end_password);

	return rtsp_server_with_register_proxying;

}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
char const *livemedia_rtsp_server_with_register_proxying__allow_command_names__impl(livemedia_rtsp_server_t *rtsp_server)
{
}

bool livemedia_rtsp_server_with_register_proxying__we_implement_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *proxy_url_suffix, char **response_str)
{
}

void livemedia_rtsp_server_with_register_proxying__implement_cmd_register__impl(livemedia_rtsp_server_t *rtsp_server, 
		char const *cmd /* "REGISTER" or "DEREGISTER" */,
		char const *url, char const *url_suffix, int socket_to_remote_server, 
		bool deliver_via_tcp, char const *proxy_url_suffix)
{
}

livemedia_user_authentication_database_t *livemedia_rtsp_server_with_register_proxying__get_authentication_database_for_command__impl(
		livemedia_rtsp_server_t *rtsp_server, char const *cmd_name)
{
}

void livemedia_rtsp_server_with_register_proxying__delete__impl(livemedia_medium_t *medium)
{
}


/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_rtsp_server_with_register_proxying__create_new__static(
		groupsock_port_t our_port, livemedia_user_authentication_database_t *auth_database,
		livemedia_user_authentication_database_t *auth_database_for_register,
		unsigned int reclamation_seconds,
		bool stream_rtp_over_tcp,
		int verbosity_level_for_proxying,
		char const *back_end_username,
		char const *back_end_password)
{
}


/* 
 * Non-rtsp_server_rtsp_client_connection_params_for_register functions
 */

void livemedia__parse_transport_header_for_register(char const* buf, /* in */
		bool *reuse_connection, /* out */
		bool *deliver_via_tcp, /* out */
		char **procxy_url_suffix) /* out */
{

}
