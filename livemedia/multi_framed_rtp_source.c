#include "multi_framed_rtp_source.h"

/*****************************************************************
 * MultiFramedRTPSource:RTPSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_multi_framed_rtp_source_t *__livemedia_multi_framed_rtp_source__alloc(void)
{
	livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source;
	multi_framed_rtp_source = (livemedia_multi_framed_rtp_source_t *)calloc(1, sizeof(livemedia_multi_framed_rtp_source_t));
	if (multi_framed_rtp_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_multi_framed_rtp_source_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return multi_framed_rtp_source;
}
void __livemedia_multi_framed_rtp_source__init(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency,
		livemedia_buffered_packet_factory_t *packet_factory)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	((livemedia_medium_t *)multi_framed_rtp_source)->delete = 
		livemedia_multi_framed_rtp_source__delete__impl;
}
void __livemedia_multi_framed_rtp_source__deinit(livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_multi_framed_rtp_source__free(livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source)
{
	if (multi_framed_rtp_source) {
		free(multi_framed_rtp_source);
		multi_framed_rtp_source = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_multi_framed_rtp_source_t *livemedia__new__multi_framed_rtp_source(
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency,
		livemedia_buffered_packet_factory_t *packet_factory)
{
	livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source;

	multi_framed_rtp_source = __livemedia_multi_framed_rtp_source__alloc();
	if (multi_framed_rtp_source)
		__livemedia_multi_framed_rtp_source__init(multi_framed_rtp_source,
				rtp_gs, rtp_payload_format, rtp_timestamp_frequency, packet_factory);

	return multi_framed_rtp_source;
}
/*
 * Functions for virtual functions
 */
bool livemedia_multi_framed_rtp_source__process_special_header(
			livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
			livemedia_buffered_packet_t *packet,
			unsigned int *result_special_header_size)
{
	/* TODO */
}

bool livemedia_multi_framed_rtp_source__packet_is_usable_in_jitter_calculation(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		unsigned char *packet,
		unsigned int packet_size)
{
	/* TODO */
}

/*
 * Override functions
 */
bool livemedia_multi_framed_rtp_source__process_special_header__impl(
			livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
			livemedia_buffered_packet_t *packet,
			unsigned int *result_special_header_size)
{
	/* TODO */
}
bool livemedia_multi_framed_rtp_source__packet_is_usable_in_jitter_calculation__impl(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		unsigned char *packet,
		unsigned int packet_size)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__do_stop_getting_frames__impl(
		livemedia_framed_source_t *framed_source)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__set_packet_reordering_threshold_time(
		livemedia_rtp_source_t *rtp_source)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__delete__impl(livemedia_medium_t *medium)
{
	livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source;
	multi_framed_rtp_source = (livemedia_multi_framed_rtp_source_t *)medium;

	/* if child, get parent as parameter and cast to child */
	if (multi_framed_rtp_source) {
		__livemedia_multi_framed_rtp_source__deinit(multi_framed_rtp_source);
		__livemedia_multi_framed_rtp_source__free(multi_framed_rtp_source);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

void livemedia_multi_framed_rtp_source__reset(
		livemedia_multi_framed_rtp_source_t *framed_source)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__do_get_next_frame1(
		livemedia_multi_framed_rtp_source_t *framed_source)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__network_read_handler__static(
		livemedia_multi_framed_rtp_source_t *source, int mask)
{
	/* TODO */
}

void livemedia_multi_framed_rtp_source__network_read_handler1(
		livemedia_multi_framed_rtp_source_t *source)
{
	/* TODO */
}

/*****************************************************************
 * BufferedPacket (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_buffered_packet_t *__livemedia_buffered_packet__alloc(void)
{
	livemedia_buffered_packet_t *buffered_packet;
	buffered_packet = (livemedia_buffered_packet_t *)calloc(1, sizeof(livemedia_buffered_packet_t));
	if (buffered_packet == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_buffered_packet_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return buffered_packet;
}
void __livemedia_buffered_packet__init(livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	buffered_packet->delete = livemedia_buffered_packet__delete__impl;
}
void __livemedia_buffered_packet__deinit(livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_buffered_packet__free(livemedia_buffered_packet_t *buffered_packet)
{
	if (buffered_packet) {
		free(buffered_packet);
		buffered_packet = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_buffered_packet_t *livemedia__new__buffered_packet(void)
{
	livemedia_buffered_packet_t *buffered_packet;

	buffered_packet = __livemedia_buffered_packet__alloc();
	if (buffered_packet)
		__livemedia_buffered_packet__init(buffered_packet);

	return buffered_packet;
}
void livemedia__delete__buffered_packet(livemedia_buffered_packet_t *buffered_packet)
{
	if (buffered_packet && buffered_packet->delete)
		buffered_packet->delete(buffered_packet);
}
/*
 * Functions for virtual functions
 */
void livemedia_buffered_packet__reset(struct _livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}
unsigned int livemedia_buffered_packet__next_enclosed_frame_size(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size)
{
	/* TODO */
}
void livemedia_buffered_packet__get_next_enclosed_frame_parameters(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size,
		unsigned int *frame_size,
		unsigned int *frame_duration_in_microseconds)
{
	/* TODO */
}

/*
 * Override functions
 */
void livemedia_buffered_packet__reset__impl(livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

unsigned int livemedia_buffered_packet__next_enclosed_frame_size__impl(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size)
{
	/* TODO */
}

void livemedia_buffered_packet__get_next_enclosed_frame_parameters__impl(
		struct _livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size,
		unsigned int *frame_size,
		unsigned int *frame_duration_in_microseconds)
{
	/* TODO */
}

void livemedia_buffered_packet__delete__impl(livemedia_buffered_packet_t *buffered_packet)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (buffered_packet) {
		__livemedia_buffered_packet__deinit(buffered_packet);
		__livemedia_buffered_packet__free(buffered_packet);
	}
}

/*
 * Functions in header file
 */

bool livemedia_buffered_packet__has_usable_data(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

unsigned int livemedia_buffered_packet__use_count(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

livemedia_buffered_packet_t **livemedia_buffered_packet__next_packet(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

unsigned short livemedia_buffered_packet__rtp_seq_no(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

struct timeval const *livemedia_buffered_packet__time_received(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

unsigned char *livemedia_buffered_packet__data(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

unsigned int livemedia_buffered_packet__data_size(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

bool livemedia_buffered_packet__rtp_maker_bit(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

bool livemedia_buffered_packet__is_first_packet(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

unsigned int livemedia_buffered_packet__bytes_available(
		livemedia_buffered_packet_t *buffered_packet)
{
	/* TODO */
}

/*
 * Normal functions
 */
bool livemedia_buffered_packet__fill_in_data(livemedia_buffered_packet_t *buffered_packet,
		livemedia_rtp_interface_t *rtp_interface, struct sockaddr_in *from_address,
		bool *packet_read_was_incomplete)
{
	/* TODO */
}

void livemedia_buffered_packet__assign_misc_params(
		livemedia_buffered_packet_t *buffered_packet,
		unsigned short rtp_seq_no, unsigned int rtp_timestamp,
		struct timeval presentation_time,
		bool has_been_synced_using_rtcp,
		bool rtp_maker_bit, struct timeval time_received)
{
	/* TODO */
}

void livemedia_buffered_packet__skip(livemedia_buffered_packet_t *buffered_packet,
		unsigned int num_bytes)
{
	/* TODO */
}

void livemedia_buffered_packet__remove_padding(
		livemedia_buffered_packet_t *buffered_packet, unsigned int num_bytes)
{
	/* TODO */
}

void livemedia_buffered_packet__append_data(livemedia_buffered_packet_t *buffered_packet,
		unsigned char *new_data, unsigned int num_bytes)
{
	/* TODO */
}

void livemedia_buffered_packet__use(livemedia_buffered_packet_t *buffered_packet,
		unsigned char *to, unsigned int to_size,
		unsigned int *byte_used, unsigned int *bytes_trucated,
		unsigned short *rtp_seq_no, unsigned int *rtp_timestamp,
		struct timeval *presentation_time,
		bool *has_been_synced_using_rtcp, bool *rtp_maker_bit)
{
	/* TODO */
}


/*****************************************************************
 * BufferedPacketFactory (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_buffered_packet_factory_t *__livemedia_buffered_packet_factory__alloc(void)
{
	livemedia_buffered_packet_factory_t *buffered_packet_factory;
	buffered_packet_factory = (livemedia_buffered_packet_factory_t *)calloc(1, sizeof(livemedia_buffered_packet_factory_t));
	if (buffered_packet_factory == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_buffered_packet_factory_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return buffered_packet_factory;
}
void __livemedia_buffered_packet_factory__init(livemedia_buffered_packet_factory_t *buffered_packet_factory)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	buffered_packet_factory->delete = livemedia_buffered_packet_factory__delete__impl;
}
void __livemedia_buffered_packet_factory__deinit(livemedia_buffered_packet_factory_t *buffered_packet_factory)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_buffered_packet_factory__free(livemedia_buffered_packet_factory_t *buffered_packet_factory)
{
	if (buffered_packet_factory) {
		free(buffered_packet_factory);
		buffered_packet_factory = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_buffered_packet_factory_t *livemedia__new__buffered_packet_factory(void)
{
	livemedia_buffered_packet_factory_t *buffered_packet_factory;

	buffered_packet_factory = __livemedia_buffered_packet_factory__alloc();
	if (buffered_packet_factory)
		__livemedia_buffered_packet_factory__init(buffered_packet_factory);

	return buffered_packet_factory;
}
void livemedia__delete__buffered_packet_factory(livemedia_buffered_packet_factory_t *buffered_packet_factory)
{
	if (buffered_packet_factory && buffered_packet_factory->delete)
		buffered_packet_factory->delete(buffered_packet_factory);
}
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

void livemedia_buffered_packet_factory__delete__impl(livemedia_buffered_packet_factory_t *buffered_packet_factory)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (buffered_packet_factory) {
		__livemedia_buffered_packet_factory__deinit(buffered_packet_factory);
		__livemedia_buffered_packet_factory__free(buffered_packet_factory);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * ReorderingPacketBuffer (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_reordering_packet_buffer_t *__livemedia_reordering_packet_buffer__alloc(void)
{
	livemedia_reordering_packet_buffer_t *reordering_packet_buffer;
	reordering_packet_buffer = (livemedia_reordering_packet_buffer_t *)calloc(1, sizeof(livemedia_reordering_packet_buffer_t));
	if (reordering_packet_buffer == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_reordering_packet_buffer_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return reordering_packet_buffer;
}
void __livemedia_reordering_packet_buffer__init(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_factory_t *packet_factory)
{
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	reordering_packet_buffer->delete = livemedia_reordering_packet_buffer__delete__impl;
}
void __livemedia_reordering_packet_buffer__deinit(livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_reordering_packet_buffer__free(livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	if (reordering_packet_buffer) {
		free(reordering_packet_buffer);
		reordering_packet_buffer = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_reordering_packet_buffer_t *livemedia__new__reordering_packet_buffer(
		livemedia_buffered_packet_factory_t *packet_factory)
{
	livemedia_reordering_packet_buffer_t *reordering_packet_buffer;

	reordering_packet_buffer = __livemedia_reordering_packet_buffer__alloc();
	if (reordering_packet_buffer)
		__livemedia_reordering_packet_buffer__init(reordering_packet_buffer,
				packet_factory);

	return reordering_packet_buffer;
}
void livemedia__delete__reordering_packet_buffer(livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	if (reordering_packet_buffer && reordering_packet_buffer->delete)
		reordering_packet_buffer->delete(reordering_packet_buffer);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_reordering_packet_buffer__delete__impl(livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (reordering_packet_buffer) {
		__livemedia_reordering_packet_buffer__deinit(reordering_packet_buffer);
		__livemedia_reordering_packet_buffer__free(reordering_packet_buffer);
	}
}

/*
 * Functions in header file
 */
void livemedia_reordering_packet_buffer__free_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_t *packet)
{
	/* TODO */
}

bool livemedia_reordering_packet_buffer__is_empty(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	/* TODO */
}

void livemedia_reordering_packet_buffer__set_threshold_time(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		unsigned int u_seconds)
{
	/* TODO */
}

void livemedia_reordering_packet_buffer__reset_have_seen_first_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	/* TODO */
}
/*
 * Normal functions
 */
void livemedia_reordering_packet_buffer__reset(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer)
{
	/* TODO */
}

livemedia_buffered_packet_t *livemedia_reordering_packet_buffer__get_free_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_multi_framed_rtp_source_t *our_source)
{
	/* TODO */
}

bool livemedia_reordering_packet_buffer__store_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_t *b_packet)
{
	/* TODO */
}

livemedia_buffered_packet_t *livemedia_reordering_packet_buffer__get_next_completed_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		bool *packet_loss_preceded)
{
	/* TODO */
}

void livemedia_reordering_packet_buffer__release_used_packet(
		livemedia_reordering_packet_buffer_t *reordering_packet_buffer,
		livemedia_buffered_packet_t *packet)
{
	/* TODO */
}


