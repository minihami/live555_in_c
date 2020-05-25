#ifndef __LIVEMEDIA_RTCP_H__
#define __LIVEMEDIA_RTCP_H__

#include "rtp_sink.h"
#include "rtp_source.h"
#include "srtp_cryptographic_context.h"

/* RTCP packet types:  */
#define RTCP_PT_SR 			 200
#define RTCP_PT_RR 			 201
#define RTCP_PT_SDES 		 202
#define RTCP_PT_BYE 		 203
#define RTCP_PT_APP 		 204
#define RTCP_PT_RTPFB 		 205 // Generic RTP Feedback [RFC4585]
#define RTCP_PT_PSFB 		 206 // Payload-specific [RFC4585]
#define RTCP_PT_XR			 207 // extended report [RFC3611]
#define RTCP_PT_AVB 		 208 // AVB RTCP packet ["Standard for Layer 3 Transport Protocol for Time Sensitive Applications in Local Area Networks." Work in progress.]
#define RTCP_PT_RSI 		 209 // Receiver Summary Information [RFC5760]
#define RTCP_PT_TOKEN 		 210 // Port Mapping [RFC6284]
#define RTCP_PT_IDMS 		 211 // IDMS Settings [RFC7272]

/* SDES tags: */
#define RTCP_SDES_END 		 0
#define RTCP_SDES_CNAME 	 1
#define RTCP_SDES_NAME 		 2
#define RTCP_SDES_EMAIL 	 3
#define RTCP_SDES_PHONE 	 4
#define RTCP_SDES_LOC 		 5
#define RTCP_SDES_TOOL 		 6
#define RTCP_SDES_NOTE 		 7
#define RTCP_SDES_PRIV 		 8

/*****************************************************************
 * SDEItem (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_sdes_item_t {
	/*  member variables */
	unsigned char data[2 + 0xFF];	/* first 2 bytes are tag and length	 */

	/*  virtual function pointers */
	void (*delete)(struct _livemedia_sdes_item_t *sdes_item);

} livemedia_sdes_item_t;


/*****************************************************************
 * RTCPInstance:Medium (Structure Declarations)
 ****************************************************************/
typedef void (livemedia__rtcp_app_handler_func)(void *client_data,
		u_int8_t subtype, u_int32_t name_bytes/*big endian order*/,
		u_int8_t app_dependent_data, unsigned int app_dependent_data_size);

struct _livemedia_rtcp_member_database_t;

typedef void (livemedia__bye_with_reson_handler_func)(void *client_data, char const *reason);

typedef struct _livemedia_rtcp_instance_t {
	livemedia_medium_t medium;

	/* member variables */
	u_int8_t *in_buf;
	unsigned int num_bytes_already_read;
	livemedia_out_packet_buffer_t *out_buf;
	livemedia_rtp_interface_t rtcp_interface;
	unsigned int tot_session_bw;
	livemedia_rtp_sink_t *sink;
	livemedia_rtp_source_t *source;
	bool is_ssm_transmitter;
	livemedia_srtp_cryptographic_context_t *crypto;

	livemedia_sdes_item_t cname;
	struct _livemedia_rtcp_member_database_t *known_members;
	unsigned int outgoing_report_count;	/* used for SSRC member aging */

	double ave_rtcp_size;
	int is_initial;
	double prev_report_time;
	double next_report_time;
	int prev_num_members;

	int last_sent_size;
	int last_received_size;
	u_int32_t last_received_ssrc;
	int type_of_event;
	int type_of_packet;
	bool have_just_sent_packet;
	unsigned int last_packet_sent_size;
	usage_environment__task_func *bye_handler_task;
	livemedia__bye_with_reson_handler_func *bye_with_reason_handler_task;
	void *bye_handler_client_data;
	bool bye_handle_active_participants_only;
	usage_environment__task_func *sr_handler_task;
	void *sr_handler_client_data;
	usage_environment__task_func *rr_handler_task;
	void *rr_handler_client_data;
	groupsock_address_port_lookup_table_t *specific_rr_handler_table;
	livemedia__rtcp_app_handler_func *app_handler_task;
	void *app_handler_client_data;

	/*  virtual function pointers */
	void (*note_arriving_rr)(struct _livemedia_rtcp_instance_t *rtcp_instance, 
			struct sockaddr_in const *from_address_and_port,
			int tcp_socket_num, unsigned char tcp_stream_channel_id);

} livemedia_rtcp_instance_t;

/*****************************************************************
 * SDEItem (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_sdes_item_t *__livemedia_sdes_item__alloc(void);
void __livemedia_sdes_item__init(livemedia_sdes_item_t *sdes_item, unsigned char tag,
		unsigned char const *value);
void __livemedia_sdes_item__deinit(livemedia_sdes_item_t *sdes_item);
void __livemedia_sdes_item__free(livemedia_sdes_item_t *sdes_item);
/*
 * new() and delete() functions
 */
livemedia_sdes_item_t *livemedia__new__sdes_item(unsigned char tag,
		unsigned char const *value);
void livemedia__delete__sdes_item(livemedia_sdes_item_t *sdes_item);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_sdes_item__delete__impl(livemedia_sdes_item_t *sdes_item);

/*
 * Functions in header file
 */
unsigned char const *livemedia_sdes_item__data(livemedia_sdes_item_t *sdes_item);
/*
 * Normal functions
 */
unsigned int livemedia_sdes_item__total_size(livemedia_sdes_item_t *sdes_item);

/*****************************************************************
 * RTCPInstance:Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_rtcp_instance_t *__livemedia_rtcp_instance__alloc(void);
void __livemedia_rtcp_instance__init(livemedia_rtcp_instance_t *rtcp_instance,
		groupsock_groupsock_t *rtcp_gs, unsigned int tot_session_bw,
		unsigned char const *cname, livemedia_rtp_sink_t *sink, livemedia_rtp_source_t *source,
		bool is_ssm_transmitter, livemedia_srtp_cryptographic_context_t *crypto);
      /* called only by createNew() */

void __livemedia_rtcp_instance__deinit(livemedia_rtcp_instance_t *rtcp_instance);
void __livemedia_rtcp_instance__free(livemedia_rtcp_instance_t *rtcp_instance);
/*
 * new() and delete() functions
 */
livemedia_rtcp_instance_t *livemedia__new__rtcp_instance(
		groupsock_groupsock_t *rtp_gs, unsigned tot_session_bw,
		unsigned char const *cname, livemedia_rtp_sink_t *sink, livemedia_rtp_source_t *source,
		bool is_ssm_transmitter, livemedia_srtp_cryptographic_context_t *crypto);
/*
 * Functions for virtual functions
 */
void livemedia_rtcp_instance__note_arriving_rr(livemedia_rtcp_instance_t *rtcp_instance, 
	struct sockaddr_in const *from_address_and_port,
	int tcp_socket_num, unsigned char tcp_stream_channel_id);

/*
 * Override functions
 */
void livemedia_rtcp_instance__note_arriving_rr__impl(livemedia_rtcp_instance_t *rtcp_instance, 
	struct sockaddr_in const *from_address_and_port,
	int tcp_socket_num, unsigned char tcp_stream_channel_id);
  /* redefined virtual functions: */
bool livemedia_rtcp_instance__is_rtcp_instance(livemedia_medium_t *medium);
void livemedia_rtcp_instance__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
unsigned int livemedia_rtcp_instance__tot_session_bw(livemedia_rtcp_instance_t *rtcp_instance);

groupsock_groupsock_t *livemedia_rtcp_instance__rtcp_gs(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__remove_stream_socket(livemedia_rtcp_instance_t *rtcp_instance,
		int sock_num, unsigned char stream_channel_id);
/* hacks to allow sending RTP over TCP (RFC 2236, section 10.12) */

void livemedia_rtcp_instance__set_auxilliary_read_handler(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia__aux_handler_func *handler_func, void *handler_client_data);

int livemedia_rtcp_instance__type_of_event(livemedia_rtcp_instance_t *rtcp_instance);

int livemedia_rtcp_instance__sent_packet_size(livemedia_rtcp_instance_t *rtcp_instance);

int livemedia_rtcp_instance__packet_type(livemedia_rtcp_instance_t *rtcp_instance);

int livemedia_rtcp_instance__received_packet_size(livemedia_rtcp_instance_t *rtcp_instance);

/*
 * Normal functions
 */
livemedia_rtcp_instance_t *livemedia_rtcp_instance__create_new__static(
		groupsock_groupsock_t *rtcp_gs, unsigned tot_session_bw, /* in kbps */
		unsigned char const *cname, livemedia_rtp_sink_t *sink, livemedia_rtp_source_t *source,
		bool is_ssm_transmitter, livemedia_srtp_cryptographic_context_t *crypt);

bool livemedia_rtcp_instance__lookup_by_name__static(char const *instance_name,
		livemedia_rtcp_instance_t **result_instance);

unsigned int livemedia_rtcp_instance__num_members(livemedia_rtcp_instance_t *rtcp_instance);


void livemedia_rtcp_instance__set_bye_handler(livemedia_rtcp_instance_t *rtcp_instance,
		usage_environment__task_func *handler_task, void *client_data, 
		bool handle_active_participants_only);
/* Assigns a handler routine to be called if a "BYE" arrives.
 * The handler is called once only; for subsequent "BYE"s,
 * "setByeHandler()" would need to be called again.
 * If "handleActiveParticipantsOnly" is True, then the handler is called
 * only if the SSRC is for a known sender (if we have a "RTPSource"),
 * or if the SSRC is for a known receiver (if we have a "RTPSink").
 * This prevents (for example) the handler for a multicast receiver being
 * called if some other multicast receiver happens to exit.
 * If "handleActiveParticipantsOnly" is False, then the handler is called
 * for any incoming RTCP "BYE".
 * (To remove an existing "BYE" handler, call "setByeHandler()" again, with a "handlerTask" of NULL.) */

void livemedia_rtcp_instance__set_bye_with_reason_handler(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia__bye_with_reson_handler_func *handler_task, void *client_data,
		bool handle_active_participants_only);
      /* Like "setByeHandler()", except that a string 'reason for the bye' (received as part of
      * the RTCP "BYE" packet) is passed to the handler function (along with "clientData").
      * (The 'reason' parameter to the handler function will be a dynamically-allocated string,
      * or NULL, and should be delete[]d by the handler function.) */

void livemedia_rtcp_instance__set_sr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		usage_environment__task_func *handler_task, void *client_data);
void livemedia_rtcp_instance__set_rr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		usage_environment__task_func *handler_task, void *client_data);
      /* Assigns a handler routine to be called if a "SR" or "RR" packet
      * (respectively) arrives.  Unlike "setByeHandler()", the handler will
      * be called once for each incoming "SR" or "RR".  (To turn off handling,
      * call the function again with "handlerTask" (and "clientData") as NULL.) */

void livemedia_rtcp_instance__set_specific_rr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		net_address_bits from_address, groupsock_port_t from_port,
		usage_environment__task_func *handler_task, void *client_data);
      /* Like "setRRHandler()", but applies only to "RR" packets that come from
       * a specific source address and port.  (Note that if both a specific
       * and a general "RR" handler function is set, then both will be called.) */

void livemedia_rtcp_instance__unset_specific_rr_handler(livemedia_rtcp_instance_t *rtcp_instance,
		net_address_bits from_address, groupsock_port_t from_port);
/* equivalent to setSpecificRRHandler(..., NULL, NULL); */

void livemedia_rtcp_instance__set_app_handler(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia__rtcp_app_handler_func *handler_task, void *client_data);
      /* Assigns a handler routine to be called whenever an "APP" packet arrives.  (To turn off
       * handling, call the function again with "handlerTask" (and "clientData") as NULL.) */

void livemedia_rtcp_instance__send_app_packet(livemedia_rtcp_instance_t *rtcp_instance,
		u_int8_t *app_dependent_data, unsigned int app_dependent_data_size);
      /* Sends a custom RTCP "APP" packet to the peer(s).  The parameters correspond to their
       * respective fields as described in the RTP/RTCP definition (RFC 3550).
       * Note that only the low-order 5 bits of "subtype" are used, and only the first 4 bytes
       * of "name" are used.  (If "name" has fewer than 4 bytes, or is NULL,
       * then the remaining bytes are '\0'.) */

void livemedia_rtcp_instance__set_stream_socket(livemedia_rtcp_instance_t *rtcp_instance,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);
void livemedia_rtcp_instance__add_stream_socket(livemedia_rtcp_instance_t *rtcp_instance,
		uv_tcp_t *sock_num, unsigned char stream_channel_id);

void livemedia_rtcp_instance__inject_report(livemedia_rtcp_instance_t *rtcp_instance,
		u_int8_t const *packet, unsigned int packet_size, struct sockaddr_in const *from_address);
    /* Allows an outside party to inject an RTCP report (from other than the network interface) */

void livemedia_rtcp_instance__inject_report_handler1(livemedia_rtcp_instance_t *rtcp_instance);

bool livemedia_rtcp_instance__add_report(livemedia_rtcp_instance_t *rtcp_instance,
		bool always_add);

void livemedia_rtcp_instance__add_sr(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__add_rr(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__enqueue_common_report_prefix(livemedia_rtcp_instance_t *rtcp_instance,
		unsigned char packet_type, u_int32_t ssrc, 
		unsigned int num_extra_words);

void livemedia_rtcp_instance__enqueue_common_report_suffix(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__enqueue_report_block(livemedia_rtcp_instance_t *rtcp_instance,
		livemedia_rtp_reception_stats_t *reception_stats);

void livemedia_rtcp_instance__add_sdes(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__add_bye(livemedia_rtcp_instance_t *rtcp_instance,
		char const *reason);

void livemedia_rtcp_instance__send_built_packet(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__on_expire__static(livemedia_rtcp_instance_t *instance);

void livemedia_rtcp_instance__on_expire1(livemedia_rtcp_instance_t *instance);

void livemedia_rtcp_instance__incoming_report_handler__static(livemedia_rtcp_instance_t *instance, int mask);

void livemedia_rtcp_instance__process_incoming_report(livemedia_rtcp_instance_t *instance,
		unsigned int packet_size, struct sockaddr_in const *from_address_and_port,
		int tcp_socket_num, unsigned char tcp_stream_channel_id);

void livemedia_rtcp_instance__on_receive(livemedia_rtcp_instance_t *instance,
		int type_of_packet, int tot_packet_size, u_int32_t ssrc);

void livemedia_rtcp_instance__schedule(livemedia_rtcp_instance_t *instance,
		double next_time);

void livemedia_rtcp_instance__reschedule(livemedia_rtcp_instance_t *instance,
		double next_time);

void livemedia_rtcp_instance__send_report(livemedia_rtcp_instance_t *instance);

void livemedia_rtcp_instance__send_bye(livemedia_rtcp_instance_t *instance,
		char const *reason);

int livemedia_rtcp_instance__check_new_ssrc(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__remove_last_received_ssrc(livemedia_rtcp_instance_t *rtcp_instance);

void livemedia_rtcp_instance__remove_ssrc(livemedia_rtcp_instance_t *rtcp_instance,
		u_int32_t ssrc, bool also_remove_stats);

#if 0
TODO: const char changed to #define
/* RTCP packet types:  */
const unsigned char RTCP_PT_SR = 200;
const unsigned char RTCP_PT_RR = 201;
const unsigned char RTCP_PT_SDES = 202;
const unsigned char RTCP_PT_BYE = 203;
const unsigned char RTCP_PT_APP = 204;
const unsigned char RTCP_PT_RTPFB = 205; // Generic RTP Feedback [RFC4585]
const unsigned char RTCP_PT_PSFB = 206; // Payload-specific [RFC4585]
const unsigned char RTCP_PT_XR = 207; // extended report [RFC3611]
const unsigned char RTCP_PT_AVB = 208; // AVB RTCP packet ["Standard for Layer 3 Transport Protocol for Time Sensitive Applications in Local Area Networks." Work in progress.]
const unsigned char RTCP_PT_RSI = 209; // Receiver Summary Information [RFC5760]
const unsigned char RTCP_PT_TOKEN = 210; // Port Mapping [RFC6284]
const unsigned char RTCP_PT_IDMS = 211; // IDMS Settings [RFC7272]

/* SDES tags: */
const unsigned char RTCP_SDES_END = 0;
const unsigned char RTCP_SDES_CNAME = 1;
const unsigned char RTCP_SDES_NAME = 2;
const unsigned char RTCP_SDES_EMAIL = 3;
const unsigned char RTCP_SDES_PHONE = 4;
const unsigned char RTCP_SDES_LOC = 5;
const unsigned char RTCP_SDES_TOOL = 6;
const unsigned char RTCP_SDES_NOTE = 7;
const unsigned char RTCP_SDES_PRIV = 8;
#endif

#endif
