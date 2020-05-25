#include "rtcp.h"
#include "../groupsock/groupsock_helper.h"
#include "rtcp_from_spec.h"

#if defined(__WIN32__) || defined(_WIN32) || defined(_QNX4)
#define snprintf _snprintf
#endif

static unsigned int const IP_UDP_HDR_SIZE = 28;
    /* overhead (bytes) of IP and UDP hdrs */

#define ADVANCE(n) pkt += (n); packet_size -= (n)

/*****************************************************************
 * RTCPMemberDatabase (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_rtcp_member_database_t {
	/*  member variables */
	livemedia_rtcp_instance_t *our_rtcp_instance;
	unsigned int num_members;
	livemedia_hash_table_t *table;

	/*  virtual function pointers */
	void (*delete)(struct _livemedia_rtcp_member_database_t *rtcp_member_database);

} livemedia_rtcp_member_database_t;

/*****************************************************************
 * RTCPMemberDatabase (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtcp_member_database_t *__livemedia_rtcp_member_database__alloc(void);
void __livemedia_rtcp_member_database__init(
		livemedia_rtcp_member_database_t *rtcp_member_database,
		livemedia_rtcp_instance_t *our_rtcp_instance);
void __livemedia_rtcp_member_database__deinit(livemedia_rtcp_member_database_t *rtcp_member_database);
void __livemedia_rtcp_member_database__free(livemedia_rtcp_member_database_t *rtcp_member_database);
/*
 * new() and delete() functions
 */
livemedia_rtcp_member_database_t *livemedia__new__rtcp_member_database(
		livemedia_rtcp_instance_t *our_rtcp_instance);
void livemedia__delete__rtcp_member_database(livemedia_rtcp_member_database_t *rtcp_member_database);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtcp_member_database__delete__impl(livemedia_rtcp_member_database_t *rtcp_member_database);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
bool livemedia_rtcp_member_database__is_member(
		livemedia_rtcp_member_database_t *rtcp_member_database, u_int32_t ssrc);

bool livemedia_rtcp_member_database__note_membership(
		livemedia_rtcp_member_database_t *rtcp_member_database, u_int32_t ssrc,
		unsigned int cur_time_count);

bool livemedia_rtcp_member_database__remove(
		livemedia_rtcp_member_database_t *rtcp_member_database, u_int32_t ssrc);

unsigned int livemedia_rtcp_member_database__num_members(
		livemedia_rtcp_member_database_t *rtcp_member_database);

void livemedia_rtcp_member_database__reap_old_members(
		livemedia_rtcp_member_database_t *rtcp_member_database, unsigned int threshold);

/*****************************************************************
 * RTCPMemberDatabase (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtcp_member_database_t *__livemedia_rtcp_member_database__alloc(void)
{
	livemedia_rtcp_member_database_t *rtcp_member_database;
	rtcp_member_database = (livemedia_rtcp_member_database_t *)calloc(1, sizeof(livemedia_rtcp_member_database_t));
	if (rtcp_member_database == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtcp_member_database_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtcp_member_database;
}
void __livemedia_rtcp_member_database__init(
		livemedia_rtcp_member_database_t *rtcp_member_database,
		livemedia_rtcp_instance_t *our_rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	rtcp_member_database->our_rtcp_instance = our_rtcp_instance;
	rtcp_member_database->num_members = 1; /* ourself */
	rtcp_member_database->table = 
		livemedia_hash_table__create__static(ONE_WORD_HASH_KEYS);
	
	/* virtual functions configuration */ 
	rtcp_member_database->delete = livemedia_rtcp_member_database__delete__impl;
}
void __livemedia_rtcp_member_database__deinit(livemedia_rtcp_member_database_t *rtcp_member_database)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia__delete__hash_table(rtcp_member_database->table);

}
void __livemedia_rtcp_member_database__free(livemedia_rtcp_member_database_t *rtcp_member_database)
{
	if (rtcp_member_database) {
		free(rtcp_member_database);
		rtcp_member_database = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtcp_member_database_t *livemedia__new__rtcp_member_database(
		livemedia_rtcp_instance_t *our_rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtcp_member_database_t *rtcp_member_database;

	rtcp_member_database = __livemedia_rtcp_member_database__alloc();
	if (rtcp_member_database)
		__livemedia_rtcp_member_database__init(rtcp_member_database,
				our_rtcp_instance);

	return rtcp_member_database;
}
void livemedia__delete__rtcp_member_database(livemedia_rtcp_member_database_t *rtcp_member_database)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtcp_member_database && rtcp_member_database->delete)
		rtcp_member_database->delete(rtcp_member_database);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_rtcp_member_database__delete__impl(livemedia_rtcp_member_database_t *rtcp_member_database)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtcp_member_database) {
		__livemedia_rtcp_member_database__deinit(rtcp_member_database);
		__livemedia_rtcp_member_database__free(rtcp_member_database);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
bool livemedia_rtcp_member_database__is_member(
		livemedia_rtcp_member_database_t *rtcp_member_database, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_hash_table__lookup(rtcp_member_database->table, (char *)(long)ssrc) != NULL;
}

bool livemedia_rtcp_member_database__note_membership(
		livemedia_rtcp_member_database_t *rtcp_member_database, u_int32_t ssrc,
		unsigned int cur_time_count)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtcp_member_database__remove(
		livemedia_rtcp_member_database_t *rtcp_member_database, u_int32_t ssrc)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtcp_member_database__num_members(
		livemedia_rtcp_member_database_t *rtcp_member_database)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtcp_member_database->num_members;
}

void livemedia_rtcp_member_database__reap_old_members(
		livemedia_rtcp_member_database_t *rtcp_member_database, unsigned int threshold)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}


/*****************************************************************
 * RTCPInstance:Medium (Function Definitions)
 ****************************************************************/

static double livemedia__d_time_now(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    return (double) (time_now.tv_sec + time_now.tv_usec/1000000.0);
}

static unsigned int const max_rtcp_packet_size = 1438;
	/* bytes (1500, minus some allowance for IP, UDP, UMTP headers; SRTCP trailers) */
static unsigned int const preferred_rtcp_packet_size = 1000; /* bytes */

/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtcp_instance_t *__livemedia_rtcp_instance__alloc(void)
{
	livemedia_rtcp_instance_t *rtcp_instance;
	rtcp_instance = (livemedia_rtcp_instance_t *)calloc(1, sizeof(livemedia_rtcp_instance_t));
	if (rtcp_instance == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_rtcp_instance_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return rtcp_instance;
}
void __livemedia_rtcp_instance__init(livemedia_rtcp_instance_t *rtcp_instance,
		groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw,
		unsigned char const *cname, livemedia_rtp_sink_t *sink, livemedia_rtp_source_t *source,
		bool is_ssm_transmitter, livemedia_srtp_cryptographic_context_t *crypto)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	double time_now;

	__livemedia_medium__init(&rtcp_instance->medium);

	__livemedia_rtp_interface__init(&rtcp_instance->rtcp_interface,
			(livemedia_medium_t *)rtcp_instance, rtcp_gs);
	rtcp_instance->tot_session_bw = tot_session_bw;
	rtcp_instance->sink = sink;
	rtcp_instance->source = source;
	rtcp_instance->is_ssm_transmitter = is_ssm_transmitter;
	rtcp_instance->is_ssm_transmitter = is_ssm_transmitter;
	rtcp_instance->crypto = crypto;
	__livemedia_sdes_item__init(&rtcp_instance->cname, RTCP_SDES_CNAME, cname);
	rtcp_instance->outgoing_report_count = 1;
	rtcp_instance->ave_rtcp_size = 0;
	rtcp_instance->is_initial = 1;
	rtcp_instance->prev_num_members = 0;
	rtcp_instance->type_of_event = EVENT_UNKNOWN;
	rtcp_instance->type_of_packet = PACKET_UNKNOWN_TYPE;
	rtcp_instance->have_just_sent_packet = false;
	rtcp_instance->last_packet_sent_size = 0;
	rtcp_instance->bye_handler_task = NULL;
	rtcp_instance->bye_with_reason_handler_task = NULL;
	rtcp_instance->bye_handler_client_data = NULL;
	rtcp_instance->sr_handler_task = NULL;
	rtcp_instance->sr_handler_client_data = NULL;
	rtcp_instance->rr_handler_task = NULL;
	rtcp_instance->rr_handler_client_data = NULL;
	rtcp_instance->specific_rr_handler_table = NULL;
	rtcp_instance->app_handler_task = NULL;
	rtcp_instance->app_handler_client_data = NULL;

#ifdef DEBUG
	fprintf(stderr, "RTCPInstance[%p]::RTCPInstance()\n", this);
#endif

	if (rtcp_instance->tot_session_bw == 0) { /* not allowed! */
		fprintf(stderr, "RTCPInstance::RTCPInstance error: totSessionBW parameter should not be zero!\n");
		rtcp_instance->tot_session_bw = 1;
	}

	if (is_ssm_transmitter)
		groupsock_groupsock__multicast_send_only(rtcp_gs); /* don't receive multicast */

	time_now = livemedia__d_time_now();

	rtcp_instance->prev_report_time = rtcp_instance->next_report_time = time_now;

	rtcp_instance->known_members = livemedia__new__rtcp_member_database(rtcp_instance);
	rtcp_instance->in_buf = (unsigned char *)calloc(max_rtcp_packet_size, sizeof(char));
	if (rtcp_instance->in_buf == NULL) {
		fprintf(stderr, "calloc(max_rtcp_packet_size, sizeof(char))"
				"failed in %s()\n", __func__);
	}
	if (rtcp_instance->known_members == NULL || rtcp_instance->in_buf == NULL)
		return;
	rtcp_instance->num_bytes_already_read = 0;

	rtcp_instance->out_buf = livemedia__new__out_packet_buffer(preferred_rtcp_packet_size,
			max_rtcp_packet_size, 1500);
	if (rtcp_instance->out_buf == NULL)
		return;

	if (rtcp_instance->source != NULL && livemedia_rtp_source__rtp_gs(rtcp_instance->source) == rtcp_gs) {
		/* We're receiving RTCP reports that are multiplexed with RTP, so ask the RTP source
		 * to give them to us: */
		livemedia_rtp_source__register_for_multiplexed_rtcp_packets(rtcp_instance->source,
				rtcp_instance);
	} else {
		/* Arrange to handle incoming reports from the network: */
		livemedia_rtp_interface__start_network_reading(&rtcp_instance->rtcp_interface,
				livemedia_rtcp_instance__incoming_report_handler__static);
	}
	
	/* Send our first report. */
	rtcp_instance->type_of_event = EVENT_REPORT;
	livemedia_rtcp_instance__on_expire__static(rtcp_instance);
	
	/* virtual functions configuration */ 
	rtcp_instance->note_arriving_rr = livemedia_rtcp_instance__note_arriving_rr__impl;
	((livemedia_medium_t *)rtcp_instance)->is_rtcp_instance = 
		livemedia_rtcp_instance__is_rtcp_instance;
	((livemedia_medium_t *)rtcp_instance)->delete = livemedia_rtcp_instance__delete__impl;
}
typedef struct _rr_handler_record_t {
	usage_environment__task_func *rr_handler_task;
	void *rr_handler_client_data;
} rr_handler_record_t;
void __livemedia_rtcp_instance__deinit(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
#ifdef DEBUG
	fprintf(stderr, "RTCPInstance[%p]::~RTCPInstance()\n", rtcp_instance);
#endif
	groupsock_address_port_lookup_table_iterator_t *iter;
	rr_handler_record_t *rr_handler;

	/* TODO: destructor */
	/* Begin by sending a BYE.  We have to do this immediately, without
	 * 'reconsideration', because "this" is going away. */
	rtcp_instance->type_of_event = EVENT_BYE;
	livemedia_rtcp_instance__send_bye(rtcp_instance, NULL);

	if (rtcp_instance->source != NULL &&
			livemedia_rtp_source__rtp_gs(rtcp_instance->source) ==
			livemedia_rtp_interface__gs(&rtcp_instance->rtcp_interface)) {
		/* We were receiving RTCP reports that were multiplexed with RTP, so tell the RTP source
		 * to stop giving them to us: */
			livemedia_rtp_source__deregister_for_multiplexed_rtcp_packets(
					rtcp_instance->source);
			livemedia_rtp_interface__forget_our_groupsock(&rtcp_instance->rtcp_interface);
			/* so that the "fRTCPInterface" destructor
			 * doesn't turn off background read handling */
	}

	if (rtcp_instance->specific_rr_handler_table != NULL) {
		iter = groupsock__new__address_port_lookup_table_iterator(
				rtcp_instance->specific_rr_handler_table);
		while ((rr_handler =
					(rr_handler_record_t *)groupsock_address_port_lookup_table_iterator__next(iter)) != NULL) {
		}
		if (rr_handler) {
			free(rr_handler);
			rr_handler = NULL;
		}
		groupsock__delete__address_port_lookup_table(
				rtcp_instance->specific_rr_handler_table);
	}
	livemedia__delete__rtcp_member_database(rtcp_instance->known_members);
	livemedia__delete__out_packet_buffer(rtcp_instance->out_buf);
	if (rtcp_instance->in_buf) {
		free(rtcp_instance->in_buf);
		rtcp_instance->in_buf = NULL;
	}
	__livemedia_rtp_interface__deinit(&rtcp_instance->rtcp_interface);
	__livemedia_sdes_item__deinit(&rtcp_instance->cname);
	/* parent deinit */
	__livemedia_medium__deinit((livemedia_medium_t *)rtcp_instance);

}
void __livemedia_rtcp_instance__free(livemedia_rtcp_instance_t *rtcp_instance)
{
	if (rtcp_instance) {
		free(rtcp_instance);
		rtcp_instance = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_rtcp_instance_t *livemedia__new__rtcp_instance(
		groupsock_groupsock_t *rtp_gs, unsigned tot_session_bw,
		unsigned char const *cname, livemedia_rtp_sink_t *sink, livemedia_rtp_source_t *source,
		bool is_ssm_transmitter, livemedia_srtp_cryptographic_context_t *crypto)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtcp_instance_t *rtcp_instance;

	rtcp_instance = __livemedia_rtcp_instance__alloc();
	if (rtcp_instance)
		__livemedia_rtcp_instance__init(rtcp_instance, rtp_gs, tot_session_bw,
				cname, sink, source, is_ssm_transmitter, crypto);

	return rtcp_instance;
}
/*
 * Functions for virtual functions
 */
void livemedia_rtcp_instance__note_arriving_rr(livemedia_rtcp_instance_t *rtcp_instance, 
	struct sockaddr_in const *from_address_and_port,
	int tcp_socket_num, unsigned char tcp_stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*
 * Override functions
 */
void livemedia_rtcp_instance__note_arriving_rr__impl(livemedia_rtcp_instance_t *rtcp_instance, 
	struct sockaddr_in const *from_address_and_port,
	int tcp_socket_num, unsigned char tcp_stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtcp_instance__is_rtcp_instance(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO: if child, get parent as parameter and cast to child 
	if (rtcp_instance) {
		__livemedia_rtcp_instance__deinit(rtcp_instance);
		__livemedia_rtcp_instance__free(rtcp_instance);
	}
	*/
}

/*
 * Functions in header file
 */
unsigned int livemedia_rtcp_instance__tot_session_bw(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

groupsock_groupsock_t *livemedia_rtcp_instance__rtcp_gs(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__remove_stream_socket(livemedia_rtcp_instance_t *rtcp_instance,
		int sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	/* TODO */
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__set_auxilliary_read_handler(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia__aux_handler_func *handler_func, void *handler_client_data)
{
	/* TODO */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

int livemedia_rtcp_instance__type_of_event(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return rtcp_instance->type_of_event;
}

int livemedia_rtcp_instance__sent_packet_size(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

int livemedia_rtcp_instance__packet_type(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

int livemedia_rtcp_instance__received_packet_size(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*
 * Normal functions
 */
livemedia_rtcp_instance_t *livemedia_rtcp_instance__create_new__static(
		groupsock_groupsock_t *rtcp_gs, unsigned tot_session_bw, 
		unsigned char const *cname, livemedia_rtp_sink_t *sink, livemedia_rtp_source_t *source,
		bool is_ssm_transmitter /*defalut: false */,
		livemedia_srtp_cryptographic_context_t *crypt /*default: NULL */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia__new__rtcp_instance(rtcp_gs, tot_session_bw, cname, sink, source,
			is_ssm_transmitter, crypt);
}

bool livemedia_rtcp_instance__lookup_by_name__static(char const *instance_name,
		livemedia_rtcp_instance_t **result_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

unsigned int livemedia_rtcp_instance__num_members(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (rtcp_instance->known_members == NULL)
		return 0;
	return livemedia_rtcp_member_database__num_members(rtcp_instance->known_members);
}


void livemedia_rtcp_instance__set_bye_handler(livemedia_rtcp_instance_t *rtcp_instance,
		usage_environment__task_func *handler_task, void *client_data, 
		bool handle_active_participants_only)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__set_bye_with_reason_handler(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia__bye_with_reson_handler_func *handler_task, void *client_data,
		bool handle_active_participants_only)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__set_sr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		usage_environment__task_func *handler_task, void *client_data)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void livemedia_rtcp_instance__set_rr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		usage_environment__task_func *handler_task, void *client_data)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__set_specific_rr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		net_address_bits from_address, groupsock_port_t from_port,
		usage_environment__task_func *handler_task, void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	rr_handler_record_t *rr_handler;
	rr_handler_record_t *existing_record;

	if (handler_task == NULL && client_data == NULL) {
		livemedia_rtcp_instance__unset_specific_rr_handler(rtcp_instance, from_address,
				from_port);
		return;
	}

	rr_handler = (rr_handler_record_t *)calloc(1, sizeof(rr_handler_record_t));
	if (rr_handler == NULL) {
		fprintf(stderr, "calloc(1, sizeof(rr_handler_record_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	rr_handler->rr_handler_task = handler_task;
	rr_handler->rr_handler_client_data = client_data;
	if (rtcp_instance->specific_rr_handler_table == NULL) {
		rtcp_instance->specific_rr_handler_table = groupsock__new__address_port_lookup_table();
	}
	existing_record = (rr_handler_record_t *)groupsock_address_port_lookup_table__add(
			rtcp_instance->specific_rr_handler_table, from_address, (~0), from_port, rr_handler);
	if (existing_record) {
		free(existing_record); /*if any */
		existing_record = NULL;
	}
}

void livemedia_rtcp_instance__unset_specific_rr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		net_address_bits from_address, groupsock_port_t from_port)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__set_app_handler(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia__rtcp_app_handler_func *handler_task, void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	rtcp_instance->app_handler_task = handler_task;
	rtcp_instance->app_handler_client_data = client_data;
}

void livemedia_rtcp_instance__send_app_packet(livemedia_rtcp_instance_t *rtcp_instance,
		u_int8_t *app_dependent_data, unsigned int app_dependent_data_size)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__set_stream_socket(livemedia_rtcp_instance_t *rtcp_instance,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void livemedia_rtcp_instance__add_stream_socket(livemedia_rtcp_instance_t *rtcp_instance,
		uv_tcp_t *sock_num, unsigned char stream_channel_id)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* First, turn off background read handling for the default (UDP) socket: */
	uv_udp_t *udp_handle;

	udp_handle = groupsock_socket__socket_num(
				livemedia_rtp_interface__gs(&rtcp_instance->rtcp_interface));
	if (uv_is_active((uv_handle_t *)udp_handle))
		uv_udp_recv_stop(udp_handle);

	/* Add the RTCP-over-TCP interface: */
	livemedia_rtp_interface__add_stream_socket(&rtcp_instance->rtcp_interface, sock_num,
			stream_channel_id);

	/* Turn on background reading for this socket (in case it's not on already): */
	livemedia_rtp_interface__start_network_reading(&rtcp_instance->rtcp_interface,
			livemedia_rtcp_instance__incoming_report_handler__static);
}

void livemedia_rtcp_instance__inject_report(livemedia_rtcp_instance_t *rtcp_instance,
		u_int8_t const *packet, unsigned int packet_size, struct sockaddr_in const *from_address)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__inject_report_handler1(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

bool livemedia_rtcp_instance__add_report(livemedia_rtcp_instance_t *rtcp_instance,
		bool always_add /*default: false */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Include a SR or a RR, depending on whether we have an associated sink or source: */
	if (rtcp_instance->sink != NULL) {
		if (!always_add) {
			if (!livemedia_rtp_sink__enable_rtcp_reports(rtcp_instance->sink))
				return false;

			/* Hack: Don't send a SR during those (brief) times when the timestamp of the
			 * next outgoing RTP packet has been preset, to ensure that that timestamp gets
			 * used for that outgoing packet. (David Bertrand, 2006.07.18) */
			if (livemedia_rtp_sink__next_timestamp_has_been_preset(rtcp_instance->sink))
				return false;
		}

		livemedia_rtcp_instance__add_sr(rtcp_instance);
	} 
	if (rtcp_instance->source != NULL) {
		if (!always_add) {
			if (!livemedia_rtp_source__enable_rtcp_reports(rtcp_instance->source))
				return false;
		}
		livemedia_rtcp_instance__add_rr(rtcp_instance);
	}

	return true;
}

void livemedia_rtcp_instance__add_sr(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* ASSERT: fSink != NULL */
	struct timeval time_now;
	double fractional_part;
	unsigned int rtp_timestamp;


	livemedia_rtcp_instance__enqueue_common_report_prefix(rtcp_instance,
			RTCP_PT_SR, livemedia_rtp_sink__ssrc(rtcp_instance->sink),
			5 /*extra words in a SR */);

	/* Now, add the 'sender info' for our sink */

	/* Insert the NTP and RTP timestamps for the 'wallclock time': */
	gettimeofday(&time_now, NULL);
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf, 
			time_now.tv_sec + 0x83AA7E80);
	/* NTP timestamp most-significant word (1970 epoch -> 1900 epoch) */
	fractional_part = (time_now.tv_usec/15625.0)*0x04000000; /* 2^32/10^6 */
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf, 
			(unsigned int)(fractional_part + 0.5));
	/* NTP timestamp least-significant word */
	rtp_timestamp = livemedia_rtp_sink__convert_to_rtp_timestamp(rtcp_instance->sink, 
			time_now);
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf, rtp_timestamp);

	/* Insert the packet and byte counts: */
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf,
			livemedia_rtp_sink__packet_count(rtcp_instance->sink));
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf,
			livemedia_rtp_sink__octet_count(rtcp_instance->sink));

	livemedia_rtcp_instance__enqueue_common_report_suffix(rtcp_instance);
}

void livemedia_rtcp_instance__add_rr(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__enqueue_common_report_prefix(livemedia_rtcp_instance_t *rtcp_instance,
		unsigned char packet_type, u_int32_t ssrc, 
		unsigned int num_extra_words)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int num_reporting_sources;
	livemedia_rtp_reception_stats_db_t *all_reception_stats;
	unsigned int rtcp_hdr;

	if (rtcp_instance->source == NULL) {
		num_reporting_sources = 0; /* we don't receive anything */
	} else {
		all_reception_stats = livemedia_rtp_source__reception_stats_db(
				rtcp_instance->source);
		num_reporting_sources = 
			livemedia_rtp_reception_stats_db__num_active_sources_since_last_reset(
					all_reception_stats);
    /* This must be <32, to fit in 5 bits: */
		if (num_reporting_sources >= 32) {
			num_reporting_sources = 32;
		}
		/* Later: support adding more reports to handle >32 sources (unlikely)##### */
	}

	rtcp_hdr = 0x80000000; /* version 2, no padding */
	rtcp_hdr |= (num_reporting_sources << 24);
	rtcp_hdr |= (packet_type << 16);
	rtcp_hdr |= (1 + num_extra_words + 6 * num_reporting_sources);
	/* each report block is 6 32-bit words long */
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf, rtcp_hdr);
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf, ssrc);
}
		

void livemedia_rtcp_instance__enqueue_common_report_suffix(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtp_reception_stats_db_t *all_reception_stats;
	livemedia_rtp_reception_stats_db_iterator_t *iterator;
	livemedia_rtp_reception_stats_t *reception_stats;

	/* Output the report blocks for each source: */
	if (rtcp_instance->source != NULL) {
		all_reception_stats = livemedia_rtp_source__reception_stats_db(
				rtcp_instance->source);	
		iterator = livemedia__new__rtp_reception_stats_db_iterator(all_reception_stats);
		while (1) {
			reception_stats = livemedia_rtp_reception_stats_db_iterator__next(iterator,
					false);
			if (reception_stats == NULL)
				break;
			livemedia_rtcp_instance__enqueue_report_block(rtcp_instance, 
					reception_stats);
		}

		livemedia__delete__rtp_reception_stats_db_iterator(iterator);
		livemedia_rtp_reception_stats_db__reset(all_reception_stats);
		/* because we have just generated a report */
	}
}

void livemedia_rtcp_instance__enqueue_report_block(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia_rtp_reception_stats_t *reception_stats)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__add_sdes(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int num_bytes;
	unsigned int num4_byte_words;
	unsigned int rtcp_hdr;
	unsigned int num_padding_bytes_needed;
	unsigned char zero;

	/* For now we support only the CNAME item; later support more #####

	 * Begin by figuring out the size of the entire SDES report: */
	num_bytes = 4;
	/* counts the SSRC, but not the header; it'll get subtracted out */
	num_bytes += livemedia_sdes_item__total_size(&rtcp_instance->cname);
	/* includes id and length */
	num_bytes += 1; /* the special END item */

	num4_byte_words = (num_bytes + 3) / 4;

	rtcp_hdr = 0x81000000; /* version 2, no padding, 1 SSRC chunk */
	rtcp_hdr |= (RTCP_PT_SDES << 16);
	rtcp_hdr |= num4_byte_words;
	livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf, rtcp_hdr);

	if (rtcp_instance->source != NULL) {
		livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf,
				livemedia_rtp_source__ssrc(rtcp_instance->source));
	} else if (rtcp_instance->sink != NULL) {
		livemedia_out_packet_buffer__enqueue_word(rtcp_instance->out_buf,
				livemedia_rtp_sink__ssrc(rtcp_instance->sink));
	}

	/* Add the CNAME: */
	livemedia_out_packet_buffer__enqueue(rtcp_instance->out_buf,
			livemedia_sdes_item__data(&rtcp_instance->cname),
			livemedia_sdes_item__total_size(&rtcp_instance->cname));

	/* Add the 'END' item (i.e., a zero byte), plus any more needed to pad: */
	num_padding_bytes_needed = 4 - (livemedia_out_packet_buffer__cur_packet_size(
				rtcp_instance->out_buf) % 4);
	zero = '\0';
	while (num_padding_bytes_needed-- > 0)
		livemedia_out_packet_buffer__enqueue(rtcp_instance->out_buf, &zero, 1);
}

void livemedia_rtcp_instance__add_bye(livemedia_rtcp_instance_t *rtcp_instance,
		char const *reason)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__send_built_packet(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *p;
	unsigned int i;
	unsigned int report_size;
	unsigned int new_report_size;

#ifdef DEBUG
	fprintf(stderr, "sending RTCP packet\n");
	p = livemedia_out_packet_buffer__packet(rtcp_instance->out_buf);
	for (i = 0; i < livemedia_out_packet_buffer__cur_packet_size(rtcp_instance->out_buf);
			++i) {
		if (i % 4 == 0)
			fprintf(stderr, " ");
		fprintf(stderr, "%02x", p[i]);
	}
	fprintf(stderr, "\n");
#endif
	report_size = livemedia_out_packet_buffer__cur_packet_size(rtcp_instance->out_buf);
	if (rtcp_instance->crypto != NULL) { /* Encrypt/tag the data before sending it: */
		if (!livemedia_srtp_cryptographic_context__process_outgoing_srtcp_packet(
					rtcp_instance->crypto,
					livemedia_out_packet_buffer__packet(rtcp_instance->out_buf),
					report_size, new_report_size))
			return;
		report_size = new_report_size;
	}
	livemedia_rtp_interface__send_packet(&rtcp_instance->rtcp_interface,
			livemedia_out_packet_buffer__packet(rtcp_instance->out_buf),
			report_size);
	livemedia_out_packet_buffer__reset_offset(rtcp_instance->out_buf);

	rtcp_instance->last_sent_size = IP_UDP_HDR_SIZE + report_size;
	rtcp_instance->have_just_sent_packet = true;
	rtcp_instance->last_packet_sent_size = report_size;
}

void livemedia_rtcp_instance__on_expire__static(livemedia_rtcp_instance_t *instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_rtcp_instance__on_expire1(instance);
}

void livemedia_rtcp_instance__on_expire1(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	double rtcp_bw;

	rtcp_bw = 0.05 * rtcp_instance->tot_session_bw * 1024 / 8; /* bytes per second */
	uv_timer_stop(livemedia_medium__next_task((livemedia_medium_t *)rtcp_instance));
	livemedia__on_expire(rtcp_instance, /* event */
			livemedia_rtcp_instance__num_members(rtcp_instance), /* member */
			(rtcp_instance->sink != NULL) ? 1 : 0, /* senders */
			rtcp_bw, /*rtcp_bw */
			(rtcp_instance->sink != NULL) ? 1 : 0, /* we_sent */
			&rtcp_instance->ave_rtcp_size, /* ave_rtcp_size */
			&rtcp_instance->is_initial, /* initial */
			livemedia__d_time_now(), /* tc */
			&rtcp_instance->prev_report_time, /* tp */
			&rtcp_instance->prev_num_members /* pmembers */
			);
}

void livemedia_rtcp_instance__incoming_report_handler__static(livemedia_rtcp_instance_t *instance, int mask)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_rtcp_instance__process_incoming_report(livemedia_rtcp_instance_t *rtcp_instance,
		unsigned int packet_size, struct sockaddr_in const *from_address_and_port,
		int tcp_socket_num, unsigned char tcp_stream_channel_id)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__on_receive(livemedia_rtcp_instance_t *rtcp_instance,
		int type_of_packet, int tot_packet_size, u_int32_t ssrc)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void schedule_timer_cb(uv_timer_t *handle) 
{
	livemedia_rtcp_instance_t *rtcp_instance;
	rtcp_instance = (livemedia_rtcp_instance_t *)handle->data;
	livemedia_rtcp_instance__on_expire__static(rtcp_instance);
}
void livemedia_rtcp_instance__schedule(livemedia_rtcp_instance_t *rtcp_instance,
		double next_time)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	double seconds_to_delay;
	int64_t us_to_go;
	uv_timer_t *timer;

	rtcp_instance->next_report_time = next_time;
	
	seconds_to_delay = next_time - livemedia__d_time_now();
	if (seconds_to_delay < 0)
		seconds_to_delay = 0;
#ifdef DEBUG
  fprintf(stderr, "schedule(%f->%f)\n", seconds_to_delay, next_time);
#endif
  us_to_go = (int64_t)(seconds_to_delay * 1000000);

  timer = livemedia_medium__next_task((livemedia_medium_t *)rtcp_instance);
  timer->data = (void *)rtcp_instance;
  uv_timer_start(timer,schedule_timer_cb, us_to_go * 1000, 0);
		  
}

void livemedia_rtcp_instance__reschedule(livemedia_rtcp_instance_t *rtcp_instance,
		double next_time)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__send_report(livemedia_rtcp_instance_t *rtcp_instance)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int membership_reap_period;
	unsigned int threshold;
#ifdef DEBUG
	fprintf(stderr, "sending REPORT\n");
#endif
	/* Begin by including a SR and/or RR report: */
	if (!livemedia_rtcp_instance__add_report(rtcp_instance, false))
		return;

	/* Then, include a SDES: */
	livemedia_rtcp_instance__add_sdes(rtcp_instance);

	/* Send the report: */
	livemedia_rtcp_instance__send_built_packet(rtcp_instance);

	/* Periodically clean out old members from our SSRC membership database: */
	membership_reap_period = 5;
	if ((++rtcp_instance->outgoing_report_count) % membership_reap_period == 0) {
		threshold = rtcp_instance->outgoing_report_count - membership_reap_period;
		livemedia_rtcp_member_database__reap_old_members(
				rtcp_instance->known_members, threshold);
	}
}

void livemedia_rtcp_instance__send_bye(livemedia_rtcp_instance_t *rtcp_instance,
		char const *reason /*defalut: NULL */)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

int livemedia_rtcp_instance__check_new_ssrc(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__remove_last_received_ssrc(livemedia_rtcp_instance_t *rtcp_instance)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

void livemedia_rtcp_instance__remove_ssrc(livemedia_rtcp_instance_t *rtcp_instance,
		u_int32_t ssrc, bool also_remove_stats)
{
	/* TODO: */
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}

/*****************************************************************
 * SDEItem (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_sdes_item_t *__livemedia_sdes_item__alloc(void)
{
	livemedia_sdes_item_t *sdes_item;
	sdes_item = (livemedia_sdes_item_t *)calloc(1, sizeof(livemedia_sdes_item_t));
	if (sdes_item == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_sdes_item_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return sdes_item;
}
void __livemedia_sdes_item__init(livemedia_sdes_item_t *sdes_item, unsigned char tag,
		unsigned char const *value)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int length;

	length = strlen((char const *)value);
	if (length > 0xFF)
		length = 0xFF;	/* maximum data length for a SDES item */

	sdes_item->data[0] = tag;
	sdes_item->data[1] = (unsigned char)length;
	memmove(&sdes_item[2], value, length);

	/* virtual functions configuration */ 

	sdes_item->delete = livemedia_sdes_item__delete__impl;
}
void __livemedia_sdes_item__deinit(livemedia_sdes_item_t *sdes_item)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void __livemedia_sdes_item__free(livemedia_sdes_item_t *sdes_item)
{
	if (sdes_item) {
		free(sdes_item);
		sdes_item = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_sdes_item_t *livemedia__new__sdes_item(unsigned char tag,
		unsigned char const *value)
{
	livemedia_sdes_item_t *sdes_item;

	sdes_item = __livemedia_sdes_item__alloc();
	if (sdes_item)
		__livemedia_sdes_item__init(sdes_item, tag, value);

	return sdes_item;
}
void livemedia__delete__sdes_item(livemedia_sdes_item_t *sdes_item)
{
	if (sdes_item && sdes_item->delete)
		sdes_item->delete(sdes_item);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_sdes_item__delete__impl(livemedia_sdes_item_t *sdes_item)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (sdes_item) {
		__livemedia_sdes_item__deinit(sdes_item);
		__livemedia_sdes_item__free(sdes_item);
	}
}

/*
 * Functions in header file
 */
unsigned char const *livemedia_sdes_item__data(livemedia_sdes_item_t *sdes_item)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return sdes_item->data;
}

/*
 * Normal functions
 */
unsigned int livemedia_sdes_item__total_size(livemedia_sdes_item_t *sdes_item)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 2 + (unsigned int)sdes_item->data[1];
}

////////// Implementation of routines imported by the "rtcp_from_spec" C code

void livemedia__schedule(double next_time, event e) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)e;
  if (instance == NULL) return;

  livemedia_rtcp_instance__schedule(instance, next_time);
}

void livemedia__reschedule(double next_time, event e) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)e;
  if (instance == NULL) return;

  livemedia_rtcp_instance__reschedule(instance, next_time);
}

void livemedia__send_rtcp_report(event e) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)e;
  if (instance == NULL) return;

  livemedia_rtcp_instance__send_report(instance);
}

void livemedia__send_bye_packet(event e) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)e;
  if (instance == NULL) return;

  livemedia_rtcp_instance__send_bye(instance, NULL);
}

int livemedia__type_of_event(event e) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)e;
  if (instance == NULL) return EVENT_UNKNOWN;

  return livemedia_rtcp_instance__type_of_event(instance);
}

int livemedia__sent_packet_size(event e) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)e;
  if (instance == NULL) return 0;

  return livemedia_rtcp_instance__sent_packet_size(instance);
}

int livemedia__packet_type(packet p) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)p;
  if (instance == NULL) return PACKET_UNKNOWN_TYPE;

  return livemedia_rtcp_instance__packet_type(instance);
}

int livemedia__received_packet_size(packet p) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)p;
  if (instance == NULL) return 0;

  return livemedia_rtcp_instance__received_packet_size(instance);
}

int livemedia__new_member(packet p) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)p;
  if (instance == NULL) return 0;

  return livemedia_rtcp_instance__check_new_ssrc(instance);
}

int livemedia__new_sender(packet p) {
  return 0; // we don't yet recognize senders other than ourselves #####
}

void livemedia__add_member(packet p) {
  // Do nothing; all of the real work was done when NewMember() was called
}

void livemedia__add_sender(packet p) {
  // we don't yet recognize senders other than ourselves #####
}

void livemedia__remove_member(packet p) {
  livemedia_rtcp_instance_t *instance = (livemedia_rtcp_instance_t *)p;
  if (instance == NULL) return;

  livemedia_rtcp_instance__remove_last_received_ssrc(instance);
}

void livemedia__remove_sender(packet p) {
  // we don't yet recognize senders other than ourselves #####
}

double livemedia__drand30() {
  unsigned int tmp;
  tmp = our_random()&0x3FFFFFFF; // a random 30-bit integer
  return tmp/(double)(1024*1024*1024);
}
