#include "h264_video_rtp_source.h"
#include "base64.h"

/*****************************************************************
 * H264VideoRTPSource:MultiFramedRTPSource (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_video_rtp_source_t *__livemedia_h264_video_rtp_source__alloc(void)
{
	livemedia_h264_video_rtp_source_t *h264_video_rtp_source;
	h264_video_rtp_source = (livemedia_h264_video_rtp_source_t *)calloc(1, sizeof(livemedia_h264_video_rtp_source_t));
	if (h264_video_rtp_source == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264_video_rtp_source_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264_video_rtp_source;
}
void __livemedia_h264_video_rtp_source__init(
		livemedia_h264_video_rtp_source_t *h264_video_rtp_source,
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	((livemedia_medium_t *)h264_video_rtp_source)->delete = 
		livemedia_h264_video_rtp_source__delete__impl;
}
void __livemedia_h264_video_rtp_source__deinit(livemedia_h264_video_rtp_source_t *h264_video_rtp_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_h264_video_rtp_source__free(livemedia_h264_video_rtp_source_t *h264_video_rtp_source)
{
	if (h264_video_rtp_source) {
		free(h264_video_rtp_source);
		h264_video_rtp_source = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264_video_rtp_source_t *livemedia__new__h264_video_rtp_source(
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format,
		unsigned int rtp_timestamp_frequency)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_rtp_source_t *h264_video_rtp_source;

	h264_video_rtp_source = __livemedia_h264_video_rtp_source__alloc();
	if (h264_video_rtp_source)
		__livemedia_h264_video_rtp_source__init(h264_video_rtp_source,
				rtp_gs, rtp_payload_format, rtp_timestamp_frequency);

	return h264_video_rtp_source;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
bool livemedia_h264_video_rtp_source__process_special_header__impl(
		livemedia_multi_framed_rtp_source_t *multi_framed_rtp_source,
		livemedia_buffered_packet_t *packet,
		unsigned int *result_special_header_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */
}

char const *livemedia_h264_video_rtp_source__mime_type(
		livemedia_media_source_t *media_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */
}

void livemedia_h264_video_rtp_source__delete__impl(livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_video_rtp_source_t *h264_video_rtp_source;
	h264_video_rtp_source = (livemedia_h264_video_rtp_source_t *)medium;

	if (h264_video_rtp_source) {
		__livemedia_h264_video_rtp_source__deinit(h264_video_rtp_source);
		__livemedia_h264_video_rtp_source__free(h264_video_rtp_source);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_h264_video_rtp_source_t *livemedia_h264_video_rtp_source__create_new__static(
		groupsock_groupsock_t *rtp_gs,
		unsigned char rtp_payload_format, 
		unsigned int rtp_timestamp_frequency)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */
}

/*****************************************************************
 * SPropRecord (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_s_prop_record_t *__livemedia_s_prop_record__alloc(void)
{
	livemedia_s_prop_record_t *s_prop_record;
	s_prop_record = (livemedia_s_prop_record_t *)calloc(1, sizeof(livemedia_s_prop_record_t));
	if (s_prop_record == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_s_prop_record_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return s_prop_record;
}
void __livemedia_s_prop_record__init(livemedia_s_prop_record_t *s_prop_record)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	s_prop_record->delete = livemedia_s_prop_record__delete__impl;
}
void __livemedia_s_prop_record__deinit(livemedia_s_prop_record_t *s_prop_record)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_s_prop_record__free(livemedia_s_prop_record_t *s_prop_record)
{
	if (s_prop_record) {
		free(s_prop_record);
		s_prop_record = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_s_prop_record_t *livemedia__new__s_prop_record(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_s_prop_record_t *s_prop_record;

	s_prop_record = __livemedia_s_prop_record__alloc();
	if (s_prop_record)
		__livemedia_s_prop_record__init(s_prop_record);

	return s_prop_record;
}
void livemedia__delete__s_prop_record(livemedia_s_prop_record_t *s_prop_record)
{
	if (s_prop_record && s_prop_record->delete)
		s_prop_record->delete(s_prop_record);
}

#define S_PROP_RECORD_ENDMARKER 0x00000000
livemedia_s_prop_record_t *livemedia__new__s_prop_record_array(unsigned int size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int i;

	livemedia_s_prop_record_t *s_prop_record_array;
	s_prop_record_array = (livemedia_s_prop_record_t *)malloc(
			sizeof(livemedia_s_prop_record_t) * size + sizeof(uint32_t *));
	for (i = 0; i < size; ++i)
		__livemedia_s_prop_record__init(&s_prop_record_array[i]);
	*((uint32_t *)(s_prop_record_array + i)) = S_PROP_RECORD_ENDMARKER;
	return s_prop_record_array;
}
void livemedia__delete__s_prop_record_array(
		livemedia_s_prop_record_t *s_prop_record_array)
{
	int i;
	for (i = 0; ; ++i) {
		if (*((uint32_t *)(s_prop_record_array + i)) == S_PROP_RECORD_ENDMARKER)
			break;
		__livemedia_s_prop_record__deinit(&s_prop_record_array[i]);
	}
	free(s_prop_record_array);
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_s_prop_record__delete__impl(livemedia_s_prop_record_t *s_prop_record)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (s_prop_record) {
		__livemedia_s_prop_record__deinit(s_prop_record);
		__livemedia_s_prop_record__free(s_prop_record);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

livemedia_s_prop_record_t *livemedia__parse_s_prop_parameter_sets(
		char const *s_prop_parameter_sets_str,
		unsigned int *num_s_prop_records)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Make a copy of the input string, so we can replace the commas with '\0's: */
	char *in_str;
	char *s;
	unsigned int i;
	livemedia_s_prop_record_t *result_array;

	in_str = livemedia__str_dup(s_prop_parameter_sets_str);
	if (in_str == NULL) {
		*num_s_prop_records = 0;
		return NULL;
	}

	/* Count the number of commas (and thus the number of parameter sets): */
	*num_s_prop_records = 1;
	for (s = in_str; *s != '\0'; ++s) {
		if (*s == ',') {
			++(*num_s_prop_records);
			*s = '\0';
		}
	}

	/* Allocate and fill in the result array: */
	result_array = livemedia__new__s_prop_record_array(*num_s_prop_records);
	s = in_str;
	for (i = 0; i < *num_s_prop_records; ++i) {
		result_array[i].s_prop_bytes = livemedia__base64_decode(
				s, &result_array[i].s_prop_length, true);
		s += strlen(s) + 1;
	}
	free(in_str);
	return result_array;
}

/*****************************************************************
 * H264BufferedPacket:BufferdPacket (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264_buffered_packet_t *__livemedia_h264_buffered_packet__alloc(void)
{
	livemedia_h264_buffered_packet_t *h264_buffered_packet;
	h264_buffered_packet = (livemedia_h264_buffered_packet_t *)calloc(1, sizeof(livemedia_h264_buffered_packet_t));
	if (h264_buffered_packet == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264_buffered_packet_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264_buffered_packet;
}
void __livemedia_h264_buffered_packet__init(
		livemedia_h264_buffered_packet_t *h264_buffered_packet,
		livemedia_h264_video_rtp_source_t *our_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	((livemedia_buffered_packet_t *)h264_buffered_packet)->delete = 
		livemedia_h264_buffered_packet__delete__impl;
}
void __livemedia_h264_buffered_packet__deinit(livemedia_h264_buffered_packet_t *h264_buffered_packet)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_h264_buffered_packet__free(livemedia_h264_buffered_packet_t *h264_buffered_packet)
{
	if (h264_buffered_packet) {
		free(h264_buffered_packet);
		h264_buffered_packet = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264_buffered_packet_t *livemedia__new__h264_buffered_packet(
		livemedia_h264_video_rtp_source_t *our_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_buffered_packet_t *h264_buffered_packet;

	h264_buffered_packet = __livemedia_h264_buffered_packet__alloc();
	if (h264_buffered_packet)
		__livemedia_h264_buffered_packet__init(h264_buffered_packet, our_source);

	return h264_buffered_packet;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264_buffered_packet__next_enclosed_frame_size__impl(
		livemedia_buffered_packet_t *buffered_packet,
		unsigned char **frame_ptr,
		unsigned int data_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */
}

void livemedia_h264_buffered_packet__delete__impl(
		livemedia_buffered_packet_t *buffered_packet)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_buffered_packet_t *h264_buffered_packet;
	h264_buffered_packet = (livemedia_h264_buffered_packet_t *)buffered_packet;

	if (h264_buffered_packet) {
		__livemedia_h264_buffered_packet__deinit(h264_buffered_packet);
		__livemedia_h264_buffered_packet__free(h264_buffered_packet);
	}
}

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

livemedia_h264_buffered_packet_factory_t *__livemedia_h264_buffered_packet_factory__alloc(void)
{
	livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory;
	h264_buffered_packet_factory = (livemedia_h264_buffered_packet_factory_t *)calloc(1, sizeof(livemedia_h264_buffered_packet_factory_t));
	if (h264_buffered_packet_factory == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264_buffered_packet_factory_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264_buffered_packet_factory;
}
void __livemedia_h264_buffered_packet_factory__init(livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	((livemedia_buffered_packet_factory_t *)h264_buffered_packet_factory)->delete = 
		livemedia_h264_buffered_packet_factory__delete__impl;
}
void __livemedia_h264_buffered_packet_factory__deinit(livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_h264_buffered_packet_factory__free(livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory)
{
	if (h264_buffered_packet_factory) {
		free(h264_buffered_packet_factory);
		h264_buffered_packet_factory = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264_buffered_packet_factory_t *livemedia__new__h264_buffered_packet_factory(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory;

	h264_buffered_packet_factory = __livemedia_h264_buffered_packet_factory__alloc();
	if (h264_buffered_packet_factory)
		__livemedia_h264_buffered_packet_factory__init(h264_buffered_packet_factory);

	return h264_buffered_packet_factory;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264_buffered_packet_factory__create_new_packet__impl(
		livemedia_buffered_packet_factory_t *buffered_packet_factory,
		livemedia_multi_framed_rtp_source_t *our_source)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO */
}

void livemedia_h264_buffered_packet_factory__delete__impl(
		livemedia_buffered_packet_factory_t *buffered_packet_factory)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264_buffered_packet_factory_t *h264_buffered_packet_factory;
	h264_buffered_packet_factory = 
		(livemedia_h264_buffered_packet_factory_t *)buffered_packet_factory;

	if (h264_buffered_packet_factory) {
		__livemedia_h264_buffered_packet_factory__deinit(h264_buffered_packet_factory);
		__livemedia_h264_buffered_packet_factory__free(h264_buffered_packet_factory);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
