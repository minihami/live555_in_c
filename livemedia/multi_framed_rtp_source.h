#ifndef __LIVEMEDIA_MULTI_FRAMED_RTP_SOURCE_H__
#define __LIVEMEDIA_MULTI_FRAMED_RTP_SOURCE_H__

#include "rtp_source.h"

/* forward */
struct _livemedia_buffered_packet_t;
struct _livemedia_buffered_packet_factory_t;
struct _livemedia_reordering_packet_buffer_t;
/*****************************************************************
 * MultiFramedRTPSource:RTPSource (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_multi_framed_rtp_source_t {
	livemedia_rtp_source_t rtp_source;

	/* member variables */
	bool current_packet_begins_frame;
	bool current_packet_completes_frame;
	bool are_doing_network_reads;
	struct _livemedia_buffered_packet_t *packet_read_in_progress;
	bool need_delivery;
	bool packet_loss_in_fragmented_frame;
	unsigned char *saved_to;
	unsigned int saved_max_size;

	struct _livemedia_reordering_packet_buffer_t *reordering_buffer;
	/* TODO: virtual function pointers */
	bool (*process_special_header)(
			struct _livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
			struct _livemedia_buffered_packet_t *packet,
			unsigned int *result_special_header_size);
      /* Subclasses redefine this to handle any special, payload format
       * specific header that follows the RTP header. */
	bool (*packet_is_usable_in_jitter_calculation)(
			struct _livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
			unsigned char *packet,
			unsigned int packet_size);
      /* The default implementation returns True, but this can be redefined */

} livemedia_multi_framed_rtp_source_t;

/*****************************************************************
 * BufferedPacket (Structure Declarations)
 ****************************************************************/
/* A 'packet data' class that's used to implement the above.
 * Note that this can be subclassed - if desired - to redefine
 * "nextEnclosedFrameParameters()". */
typedef struct _livemedia_buffered_packet_t {
	/* member variables */
	unsigned int packet_size;
	unsigned char *buf;
	unsigned int head;
	unsigned int tail;

	struct _livemedia_buffered_packet_t *next_packet;	/*used to link together packets*/

	unsigned int use_count;
	unsigned short rtp_seq_no;
	unsigned int rtp_timestamp;
	struct timeval presentation_time;	/* corresponding to "fRTPTimestamp"*/
	bool has_been_synced_using_rtcp;
	bool rtp_maker_bit;
	bool is_first_packet;
	struct timeval time_received;

	/* virtual function pointers */
	void (*reset)(struct _livemedia_buffered_packet_t *buffered_packet);
	unsigned int (*next_enclosed_frame_size)(
			struct _livemedia_buffered_packet_t *buffered_packet,
			unsigned char **frame_ptr,
			unsigned int data_size);
	/* The above function has been deprecated.  Instead, new subclasses should use: */
	void (*get_next_enclosed_frame_parameters)(
			struct _livemedia_buffered_packet_t *buffered_packet,
			unsigned char **frame_ptr,
			unsigned int data_size,
			unsigned int *frame_size,
			unsigned int *frame_duration_in_microseconds);
	void (*delete)(struct _livemedia_buffered_packet_t *buffered_packet);

} livemedia_buffered_packet_t;

/*****************************************************************
 * BufferedPacketFactory (Structure Declarations)
 ****************************************************************/
/* A 'factory' class for creating "BufferedPacket" objects.
 * If you want to subclass "BufferedPacket", then you'll also
 * want to subclass this, to redefine createNewPacket() */

typedef struct _livemedia_buffered_packet_factory_t {

	livemedia_buffered_packet_t *(*create_new_packet)(
			struct _livemedia_buffered_packet_factory_t *buffered_packet_factory,
			livemedia_multi_framed_rtp_source_t *our_source);

	void (*delete)(struct _livemedia_buffered_packet_factory_t *buffered_packet_factory);
} livemedia_buffered_packet_factory_t;

/*****************************************************************
 * ReorderingPacketBuffer (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_reordering_packet_buffer_t {

	livemedia_buffered_packet_factory_t *packet_factory;
	unsigned int thresgold_time;	/* uSeconds */
	bool have_seen_first_packet;	/* used to set initial "fNextExpectedSeqNo" */
	unsigned short next_expected_seq_no;
	livemedia_buffered_packet_t *head_packet;
	livemedia_buffered_packet_t *tail_packet;
	livemedia_buffered_packet_t *saved_packet;
	/* to avoid calling new/free in the common case */
	bool saved_packet_free;

	/* virtual function pointers */
	void (*delete)(
			struct _livemedia_reordering_packet_buffer_t *reordering_packet_buffer);

} livemedia_reordering_packet_buffer_t;


/*****************************************************************
 * MultiFramedRTPSource:RTPSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_multi_framed_rtp_source_t *__livemedia_multi_framed_rtp_source__alloc(void);

void __livemedia_multi_framed_rtp_source__init(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency,
		livemedia_buffered_packet_factory_t *packet_factory);

void __livemedia_multi_framed_rtp_source__deinit(livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source);

void __livemedia_multi_framed_rtp_source__free(livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source);
/*
 * new() and delete() functions
 */
livemedia_multi_framed_rtp_source_t *livemedia__new__multi_framed_rtp_source(
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency,
		livemedia_buffered_packet_factory_t *packet_factory);

/*
 * Functions for virtual functions
 */
bool livemedia_multi_framed_rtp_source__process_special_header(
			livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
			livemedia_buffered_packet_t *packet,
			unsigned int *result_special_header_size);

bool livemedia_multi_framed_rtp_source__packet_is_usable_in_jitter_calculation(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		unsigned char *packet,
		unsigned int packet_size);

/*
 * Override functions
 */
bool livemedia_multi_framed_rtp_source__process_special_header__impl(
			livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
			livemedia_buffered_packet_t *packet,
			unsigned int *result_special_header_size);

bool livemedia_multi_framed_rtp_source__packet_is_usable_in_jitter_calculation__impl(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		unsigned char *packet,
		unsigned int packet_size);

/* redefined virtual functions: */
void livemedia_multi_framed_rtp_source__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source);

void livemedia_multi_framed_rtp_source__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source);

void livemedia_multi_framed_rtp_source__set_packet_reordering_threshold_time(
		livemedia_rtp_source_t *rtp_source);

void livemedia_multi_framed_rtp_source__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_multi_framed_rtp_source__reset(
		livemedia_multi_framed_rtp_source_t *framed_source);

void livemedia_multi_framed_rtp_source__do_get_next_frame1(
		livemedia_multi_framed_rtp_source_t *framed_source);

void livemedia_multi_framed_rtp_source__network_read_handler__static(
		livemedia_multi_framed_rtp_source_t *source, int mask);

void livemedia_multi_framed_rtp_source__network_read_handler1(
		livemedia_multi_framed_rtp_source_t *source);


/*****************************************************************
 * BufferedPacket (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_buffered_packet_t *__livemedia_buffered_packet__alloc(void);

void __livemedia_buffered_packet__init(livemedia_buffered_packet_t *buffered_packet);

void __livemedia_buffered_packet__deinit(livemedia_buffered_packet_t *buffered_packet);

void __livemedia_buffered_packet__free(livemedia_buffered_packet_t *buffered_packet);
/*
 * new() and delete() functions
 */
livemedia_buffered_packet_t *livemedia__new__buffered_packet(void);

void livemedia__delete__buffered_packet(livemedia_buffered_packet_t *buffered_packet);

/*
 * Functions for virtual functions
 */
void livemedia_buffered_packet__reset(struct _livemedia_buffered_packet_t *buffered_packet);
unsigned int livemedia_buffered_packet__next_enclosed_frame_size(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size);
void livemedia_buffered_packet__get_next_enclosed_frame_parameters(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size,
		unsigned int *frame_size,
		unsigned int *frame_duration_in_microseconds);

/*
 * Override functions
 */
void livemedia_buffered_packet__reset__impl(livemedia_buffered_packet_t *buffered_packet);

unsigned int livemedia_buffered_packet__next_enclosed_frame_size__impl(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size);

void livemedia_buffered_packet__get_next_enclosed_frame_parameters__impl(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size,
		unsigned int *frame_size,
		unsigned int *frame_duration_in_microseconds);

void livemedia_buffered_packet__delete__impl(livemedia_buffered_packet_t *buffered_packet);

/*
 * Functions in header file
 */
bool livemedia_buffered_packet__has_usable_data(
		livemedia_buffered_packet_t *buffered_packet);

unsigned int livemedia_buffered_packet__use_count(
		livemedia_buffered_packet_t *buffered_packet);

livemedia_buffered_packet_t **livemedia_buffered_packet__next_packet(
		livemedia_buffered_packet_t *buffered_packet);

unsigned short livemedia_buffered_packet__rtp_seq_no(
		livemedia_buffered_packet_t *buffered_packet);

struct timeval const *livemedia_buffered_packet__time_received(
		livemedia_buffered_packet_t *buffered_packet);

unsigned char *livemedia_buffered_packet__data(
		livemedia_buffered_packet_t *buffered_packet);

unsigned int livemedia_buffered_packet__data_size(
		livemedia_buffered_packet_t *buffered_packet);

bool livemedia_buffered_packet__rtp_maker_bit(
		livemedia_buffered_packet_t *buffered_packet);

bool livemedia_buffered_packet__is_first_packet(
		livemedia_buffered_packet_t *buffered_packet);

unsigned int livemedia_buffered_packet__bytes_available(
		livemedia_buffered_packet_t *buffered_packet);

/*
 * Normal functions
 */
bool livemedia_buffered_packet__fill_in_data(livemedia_buffered_packet_t *buffered_packet,
		livemedia_rtp_interface_t *rtp_interface, struct sockaddr_in *from_address,
		bool *packet_read_was_incomplete);

void livemedia_buffered_packet__assign_misc_params(
		livemedia_buffered_packet_t *buffered_packet,
		unsigned short rtp_seq_no, unsigned int rtp_timestamp,
		struct timeval presentation_time,
		bool has_been_synced_using_rtcp,
		bool rtp_maker_bit, struct timeval time_received);

void livemedia_buffered_packet__skip(livemedia_buffered_packet_t *buffered_packet,
		unsigned int num_bytes);	// used to skip over an initial header

void livemedia_buffered_packet__remove_padding(
		livemedia_buffered_packet_t *buffered_packet, unsigned int num_bytes);	

void livemedia_buffered_packet__append_data(livemedia_buffered_packet_t *buffered_packet,
		unsigned char *new_data, unsigned int num_bytes);

void livemedia_buffered_packet__use(livemedia_buffered_packet_t *buffered_packet,
		unsigned char *to, unsigned int to_size,
		unsigned int *byte_used, unsigned int *bytes_trucated,
		unsigned short *rtp_seq_no, unsigned int *rtp_timestamp,
		struct timeval *presentation_time,
		bool *has_been_synced_using_rtcp, bool *rtp_maker_bit);


/*****************************************************************
 * BufferedPacketFactory (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_buffered_packet_factory_t *__livemedia_buffered_packet_factory__alloc(void);
void __livemedia_buffered_packet_factory__init(livemedia_buffered_packet_factory_t *buffered_packet_factory);
void __livemedia_buffered_packet_factory__deinit(livemedia_buffered_packet_factory_t *buffered_packet_factory);
void __livemedia_buffered_packet_factory__free(livemedia_buffered_packet_factory_t *buffered_packet_factory);
/*
 * new() and delete() functions
 */
livemedia_buffered_packet_factory_t *livemedia__new__buffered_packet_factory(void);
void livemedia__delete__buffered_packet_factory(livemedia_buffered_packet_factory_t *buffered_packet_factory);
/*
 * Functions for virtual functions
 */
livemedia_buffered_packet_t *livemedia_buffered_packet_factory__create_new_packet(
		livemedia_buffered_packet_factory_t *buffered_packet_factory,
		livemedia_multi_framed_rtp_source_t *our_source);

/*
 * Override functions
 */
livemedia_buffered_packet_t *livemedia_buffered_packet_factory__create_new_packet__impl(
		livemedia_buffered_packet_factory_t *buffered_packet_factory,
		livemedia_multi_framed_rtp_source_t *our_source);

void livemedia_buffered_packet_factory__delete__impl(livemedia_buffered_packet_factory_t *buffered_packet_factory);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * ReorderingPacketBuffer (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_reordering_packet_buffer_t *__livemedia_reordering_packet_buffer__alloc(void);

void __livemedia_reordering_packet_buffer__init(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_factory_t *packet_factory);

void __livemedia_reordering_packet_buffer__deinit(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);

void __livemedia_reordering_packet_buffer__free(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);
/*
 * new() and delete() functions
 */
livemedia_reordering_packet_buffer_t *livemedia__new__reordering_packet_buffer(
		livemedia_buffered_packet_factory_t *packet_factory);

void livemedia__delete__reordering_packet_buffer(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_reordering_packet_buffer__delete__impl(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);

/*
 * Functions in header file
 */

void livemedia_reordering_packet_buffer__free_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_t *packet);

bool livemedia_reordering_packet_buffer__is_empty(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);

void livemedia_reordering_packet_buffer__set_threshold_time(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		unsigned int u_seconds);

void livemedia_reordering_packet_buffer__reset_have_seen_first_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);
/*
 * Normal functions
 */
void livemedia_reordering_packet_buffer__reset(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer);

livemedia_buffered_packet_t *livemedia_reordering_packet_buffer__get_free_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_multi_framed_rtp_source_t *our_source);

bool livemedia_reordering_packet_buffer__store_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_t *b_packet);

livemedia_buffered_packet_t *livemedia_reordering_packet_buffer__get_next_completed_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		bool *packet_loss_preceded);

void livemedia_reordering_packet_buffer__release_used_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_t *packet);


#endif
