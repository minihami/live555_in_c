
#ifndef __LIVEMEDIA_H264_VIDEO_RTP_SOURCE_H__
#define __LIVEMEDIA_H264_VIDEO_RTP_SOURCE_H__

#include "multi_framed_rtp_source.h"

/*****************************************************************
 * H264VideoRTPSource:MultiFramedRTPSource (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264_video_rtp_source_t {
	livemedia_multi_framed_rtp_source_t multi_framed_rtp_source;

	/* member variables */
	unsigned char cur_packet_nal_unit_type;


} livemedia_h264_video_rtp_source_t;

/*****************************************************************
 * SPropRecord (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_s_prop_record_t {
	/* TODO: member variables */
	unsigned int s_prop_length;	/* in bytes */
	unsigned char *s_prop_bytes;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_s_prop_record_t *s_prop_record);

} livemedia_s_prop_record_t;

/*****************************************************************
 * H264BufferedPacket:BufferdPacket (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264_buffered_packet_t {
	livemedia_buffered_packet_t buffered_packet;

	livemedia_h264_video_rtp_source_t *our_source;

} livemedia_h264_buffered_packet_t;

/*****************************************************************
 * H264BufferedPacketFactory:BufferedPacketFactory (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264_buffered_packet_factory_t {
	livemedia_buffered_packet_factory_t buffered_packet_factory;

} livemedia_h264_buffered_packet_factory_t;

/*****************************************************************
 * H264VideoRTPSource:MultiFramedRTPSource (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_rtp_source_t *__livemedia_h264_video_rtp_source__alloc(void);
void __livemedia_h264_video_rtp_source__init(
		livemedia_h264_video_rtp_source_t *h264_video_rtp_source,
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency);
void __livemedia_h264_video_rtp_source__deinit(livemedia_h264_video_rtp_source_t *h264_video_rtp_source);
void __livemedia_h264_video_rtp_source__free(livemedia_h264_video_rtp_source_t *h264_video_rtp_source);
/*
 * new() and delete() functions
 */
livemedia_h264_video_rtp_source_t *livemedia__new__h264_video_rtp_source(
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency);
      /* called only by createNew() */
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_h264_video_rtp_source__process_special_header__impl(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		livemedia_buffered_packet_t *packet,
		unsigned int *result_special_header_size);

char const *livemedia_h264_video_rtp_source__mime_type(
		livemedia_media_source_t *media_source);

void livemedia_h264_video_rtp_source__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_h264_video_rtp_source_t *livemedia_h264_video_rtp_source__create_new__static(
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format, 
		unsigned int rtp_timestamp_frequency);

/*****************************************************************
 * SPropRecord (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_s_prop_record_t *__livemedia_s_prop_record__alloc(void);
void __livemedia_s_prop_record__init(livemedia_s_prop_record_t *s_prop_record);
void __livemedia_s_prop_record__deinit(livemedia_s_prop_record_t *s_prop_record);
void __livemedia_s_prop_record__free(livemedia_s_prop_record_t *s_prop_record);
/*
 * new() and delete() functions
 */
livemedia_s_prop_record_t *livemedia__new__s_prop_record(void);
void livemedia__delete__s_prop_record(livemedia_s_prop_record_t *s_prop_record);
livemedia_s_prop_record_t *livemedia__new__s_prop_record_array(unsigned int size);
void livemedia__delete__s_prop_record_array(
		livemedia_s_prop_record_t *s_prop_record_array);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_s_prop_record__delete__impl(livemedia_s_prop_record_t *s_prop_record);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_s_prop_record_t *livemedia__parse_s_prop_parameter_sets(
		char const *s_prop_parameter_sets_str,
		/* result parameter */
		unsigned int *num_s_prop_records);
    /* Returns the binary value of each 'parameter set' specified in a
     * "sprop-parameter-sets" string (in the SDP description for a H.264/RTP stream).
     * The value is returned as an array (length "numSPropRecords") of "SPropRecord"s.
     * This array is dynamically allocated by this routine, and must be delete[]d by the caller. */

/*****************************************************************
 * H264BufferedPacket:BufferdPacket (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_h264_buffered_packet_t *__livemedia_h264_buffered_packet__alloc(void);

void __livemedia_h264_buffered_packet__init(
		livemedia_h264_buffered_packet_t *h264_buffered_packet,
		livemedia_h264_video_rtp_source_t *our_source);

void __livemedia_h264_buffered_packet__deinit(
		livemedia_h264_buffered_packet_t *h264_buffered_packet);

void __livemedia_h264_buffered_packet__free(
		livemedia_h264_buffered_packet_t *h264_buffered_packet);
/*
 * new() and delete() functions
 */
livemedia_h264_buffered_packet_t *livemedia__new__h264_buffered_packet(
		livemedia_h264_video_rtp_source_t *our_source);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264_buffered_packet__next_enclosed_frame_size__impl(
		livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size);

void livemedia_h264_buffered_packet__delete__impl(
		livemedia_buffered_packet_t *buffered_packet);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * H264BufferedPacketFactory:BufferedPacketFactory (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_buffered_packet_factory_t *__livemedia_h264_buffered_packet_factory__alloc(
		void);

void __livemedia_h264_buffered_packet_factory__init(
		livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory);

void __livemedia_h264_buffered_packet_factory__deinit(
		livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory);

void __livemedia_h264_buffered_packet_factory__free(
		livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory);

/*
 * new() and delete() functions
 */
livemedia_h264_buffered_packet_factory_t *livemedia__new__h264_buffered_packet_factory(
		void);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264_buffered_packet_factory__create_new_packet__impl(
		livemedia_buffered_packet_factory_t *buffered_packet_factory,
		livemedia_multi_framed_rtp_source_t *our_source);

void livemedia_h264_buffered_packet_factory__delete__impl(
		livemedia_buffered_packet_factory_t *buffered_packet_factory);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
#endif
