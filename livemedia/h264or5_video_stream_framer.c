#include "h264or5_video_stream_framer.h"
#include "mpeg_video_stream_parser.h"
#include "bit_vector.h"
#include "../groupsock/groupsock_helper.h" // for "gettimeofday()"

/*****************************************************************
 * H264or5VideoStreamParser:MPEGVideoStreamParser (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_h264or5_video_stream_parser_t {
	livemedia_mpeg_video_stream_parser_t mpeg_video_stream_parser;

	/* member variables */
	int h_number; /* 264 or 265 */
	unsigned int output_start_code_size;
	bool have_seen_first_start_code, have_seen_first_byte_of_nal_unit;
	u_int8_t first_byte_of_nal_unit;
	double parsed_frame_rate;
	unsigned int cpb_removal_delay_length_minus1;
	unsigned int dpb_output_delay_length_minus1;
	bool cpb_dpb_delays_present_flag, pic_struct_present_flag;
	double delta_tfi_divisor;
} livemedia_h264or5_video_stream_parser_t;


/*****************************************************************
 * H264or5VideoStreamParser:MPEGVideoStreamParser (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_video_stream_parser_t *__livemedia_h264or5_video_stream_parser__alloc(void);
void __livemedia_h264or5_video_stream_parser__init(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		int h_number, livemedia_h264or5_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source, bool include_start_code_in_output);
void __livemedia_h264or5_video_stream_parser__deinit(livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser);
void __livemedia_h264or5_video_stream_parser__free(livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser);
/*
 * new() and delete() functions
 */
livemedia_h264or5_video_stream_parser_t *livemedia__new__h264or5_video_stream_parser(
		int h_number, livemedia_h264or5_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source, bool include_start_code_in_output);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264or5_video_stream_parser__flush_input__impl(
		livemedia_stream_parser_t *stream_parser);
unsigned int livemedia_h264or5_video_stream_parser__parse__impl(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser);
void livemedia_h264or5_video_stream_parser__delete__impl(
		livemedia_stream_parser_t *stream_parser);

/*
 * Functions in header file
 */
livemedia_h264or5_video_stream_framer_t *livemedia_h264or5_video_stream_parser__using_source(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser);
bool livemedia_h264or5_video_stream_parser__is_vps(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_parser__is_sps(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_parser__is_pps(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_parser__is_vcl(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);


/*
 * Normal functions
 */
bool livemedia_h264or5_video_stream_parser__is_sei(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_parser__is_eof(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);
bool livemedia_h264or5_video_stream_parser__usually_begin_access_unit(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);

void livemedia_h264or5_video_stream_parser__remove_emulation_bytes(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t *nal_unit_copy, unsigned int max_size, unsigned int *nal_unit_copy_size);

void livemedia_h264or5_video_stream_parser__analyze_video_parameter_set_data(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		unsigned int *num_units_in_tick, unsigned int *time_scale);
void livemedia_h264or5_video_stream_parser__analyze_seq_parameter_set_data(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		unsigned int *num_units_in_tick, unsigned int *time_scale);
void livemedia_h264or5_video_stream_parser__profile_tier_level(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		livemedia_bit_vector_t *bv, unsigned int max_sub_layers_minus1);
void livemedia_h264or5_video_stream_parser__analyze_vui_parameters(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		livemedia_bit_vector_t *bv, unsigned int *num_units_in_tick,
		unsigned int *time_scale);
void livemedia_h264or5_video_stream_parser__analyze_hrd_parameters(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		livemedia_bit_vector_t *bv);
void livemedia_h264or5_video_stream_parser__analyze_sei_data(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type);
void livemedia_h264or5_video_stream_parser__analyze_sei_payload(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		unsigned int paylead_type, unsigned int payload_size, u_int8_t *payload);

/*****************************************************************
 * H264or5VideoStreamFramer:MPEGVideoStreamFramer (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_video_stream_framer_t *__livemedia_h264or5_video_stream_framer__alloc(void)
{
	livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer;
	h264or5_video_stream_framer = (livemedia_h264or5_video_stream_framer_t *)calloc(1, sizeof(livemedia_h264or5_video_stream_framer_t));
	if (h264or5_video_stream_framer == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264or5_video_stream_framer_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264or5_video_stream_framer;
}
void __livemedia_h264or5_video_stream_framer__init(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		int number /* 264 or 265 */, livemedia_framed_source_t *input_source,
		bool create_parser, bool include_start_code_in_output,
		bool insert_access_unit_delimiters)
{
	__livemedia_mpeg_video_stream_framer__init(
			&h264or5_video_stream_framer->mpeg_video_stream_framer, input_source);

	h264or5_video_stream_framer->h_number = number;
	h264or5_video_stream_framer->include_start_code_in_output =
		include_start_code_in_output;
	h264or5_video_stream_framer->insert_access_unit_delimiters =
		insert_access_unit_delimiters;
	h264or5_video_stream_framer->last_seen_vps = NULL;
	h264or5_video_stream_framer->last_seen_vps_size = 0;
	h264or5_video_stream_framer->last_seen_sps = NULL;
	h264or5_video_stream_framer->last_seen_sps_size = 0;
	h264or5_video_stream_framer->last_seen_pps = NULL;
	h264or5_video_stream_framer->last_seen_pps_size = 0;

	((livemedia_mpeg_video_stream_framer_t *)h264or5_video_stream_framer)->parser =
		create_parser ? (livemedia_mpeg_video_stream_parser_t *)livemedia__new__h264or5_video_stream_parser(number,
				h264or5_video_stream_framer, input_source, include_start_code_in_output)
		: NULL;
	((livemedia_mpeg_video_stream_framer_t *)h264or5_video_stream_framer)->frame_rate = 25.0;
	/* We assume a frame rate of 25 fps, unless we learn otherwise
	 * (from parsing a VPS or SPS NAL unit) */

	((livemedia_framed_source_t *)h264or5_video_stream_framer)->do_get_next_frame =
		livemedia_h264or5_video_stream_framer__do_get_next_frame__impl;

	((livemedia_medium_t *)h264or5_video_stream_framer)->delete = 
		livemedia_h264or5_video_stream_framer__delete__impl;
}
void __livemedia_h264or5_video_stream_framer__deinit(livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer)
{
	__livemedia_mpeg_video_stream_framer__deinit(
			&h264or5_video_stream_framer->mpeg_video_stream_framer);
}
void __livemedia_h264or5_video_stream_framer__free(livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer)
{
	if (h264or5_video_stream_framer) {
		free(h264or5_video_stream_framer);
		h264or5_video_stream_framer = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264or5_video_stream_framer_t *livemedia__new__h264or5_video_stream_framer(
		int number /* 264 or 265 */, livemedia_framed_source_t *input_source,
		bool create_parser, bool include_start_code_in_output,
		bool insert_access_unit_delimiters)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer;

	h264or5_video_stream_framer = __livemedia_h264or5_video_stream_framer__alloc();
	if (h264or5_video_stream_framer)
		__livemedia_h264or5_video_stream_framer__init(h264or5_video_stream_framer,
				number, input_source, create_parser, include_start_code_in_output,
				insert_access_unit_delimiters);

	return h264or5_video_stream_framer;
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264or5_video_stream_framer__do_get_next_frame__impl(
		livemedia_framed_source_t *framed_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int start_code_size;
	unsigned int aud_nal_size;
	bool *picture_end_marker;

	livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer;
	h264or5_video_stream_framer =
		(livemedia_h264or5_video_stream_framer_t *)framed_source;

	if (h264or5_video_stream_framer->insert_access_unit_delimiters &&
			*livemedia_mpeg_video_stream_framer__picture_end_marker(
				(livemedia_mpeg_video_stream_framer_t *)framed_source)) {
		/* Deliver an "access_unit_delimiter" NAL unit instead: */
		start_code_size =
			h264or5_video_stream_framer->include_start_code_in_output ? 4 : 0;
		aud_nal_size = h264or5_video_stream_framer->h_number == 264 ? 2 : 3;

		framed_source->frame_size = start_code_size + aud_nal_size;
		if (framed_source->frame_size > framed_source->max_size) { /* there's no space */
			framed_source->num_truncated_bytes = framed_source->frame_size -
				framed_source->max_size;
			framed_source->frame_size = framed_source->max_size;
			livemedia_framed_source__handle_closure(framed_source);
			return;
		}

		if (h264or5_video_stream_framer->include_start_code_in_output) {
			*framed_source->to++ = 0x00;
			*framed_source->to++ = 0x00;
			*framed_source->to++ = 0x00;
			*framed_source->to++ = 0x01;
		}
		if (h264or5_video_stream_framer->h_number == 264) {
			*framed_source->to++ = 9;
			/* "Access unit delimiter" nal_unit_type */
			*framed_source->to++ = 0xF0;
			/* "primary_pic_type" (7); "rbsp_trailing_bits()" */
		} else { /* H.265 */
			*framed_source->to++ = 35<<1; /* "Access unit delimiter" nal_unit_type */
			*framed_source->to++ = 0;
			/* "nuh_layer_id" (0); "nuh_temporal_id_plus1" (0) (Is this correct??) */
			*framed_source->to++ = 0x50;
			/* "pic_type" (2); "rbsp_trailing_bits()" (Is this correct??) */
		}

		picture_end_marker = livemedia_mpeg_video_stream_framer__picture_end_marker(
				(livemedia_mpeg_video_stream_framer_t *)framed_source);
		*picture_end_marker = false; /* for next time */
		livemedia_framed_source__after_getting__static(framed_source);
	} else {
		/* Do the normal delivery of a NAL unit from the parser: */
		livemedia_mpeg_video_stream_framer__do_get_next_frame__impl(framed_source);
	}
}

void livemedia_h264or5_video_stream_framer__delete__impl(
		livemedia_medium_t *medium)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer;
	h264or5_video_stream_framer = (livemedia_h264or5_video_stream_framer_t *)medium;
	if (h264or5_video_stream_framer) {
		__livemedia_h264or5_video_stream_framer__deinit(h264or5_video_stream_framer);
		__livemedia_h264or5_video_stream_framer__free(h264or5_video_stream_framer);
	}
}

/*
 * Functions in header file
 */
void livemedia_h264or5_video_stream_framer__get_vps_and_sps_and_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t **vps, unsigned int *vps_size, u_int8_t **sps, unsigned int *sps_size,
		u_int8_t **pps, unsigned int *pps_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Returns pointers to copies of the most recently seen VPS (video parameter set)
	 * SPS (sequence parameter set) and PPS (picture parameter set) NAL units.
	 * (NULL pointers are returned if the NAL units have not yet been seen.) */
	*vps = h264or5_video_stream_framer->last_seen_vps;
	*vps_size = h264or5_video_stream_framer->last_seen_vps_size;
	*sps = h264or5_video_stream_framer->last_seen_sps;
	*sps_size = h264or5_video_stream_framer->last_seen_sps_size;
	*pps = h264or5_video_stream_framer->last_seen_pps;
	*pps_size = h264or5_video_stream_framer->last_seen_pps_size;
}

void livemedia_h264or5_video_stream_framer__set_vps_and_sps_and_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *vps, unsigned int vps_size, u_int8_t *sps, unsigned int sps_size,
		u_int8_t *pps, unsigned int pps_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_h264or5_video_stream_framer__set_presentation_time(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	((livemedia_framed_source_t *)h264or5_video_stream_framer)->presentation_time = 
		h264or5_video_stream_framer->next_presentation_time;
}

/*
 * Normal functions
 */
#define VPS_MAX_SIZE 1000 /* larger than the largest possible VPS (Video Parameter Set) NAL unit */
void livemedia_h264or5_video_stream_framer__save_copy_of_vps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *from, unsigned int size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (from == NULL)
		return;
	if (h264or5_video_stream_framer->last_seen_vps) {
		free(h264or5_video_stream_framer->last_seen_vps);
		h264or5_video_stream_framer->last_seen_vps = NULL;
	}
	h264or5_video_stream_framer->last_seen_vps = (u_int8_t *)calloc(size, sizeof(u_int8_t));
	memmove(h264or5_video_stream_framer->last_seen_vps, from, size);

	h264or5_video_stream_framer->last_seen_vps_size = size;
}

#define SPS_MAX_SIZE 1000 /* larger than the largest possible SPS (Sequence Parameter Set) NAL unit */
void livemedia_h264or5_video_stream_framer__save_copy_of_sps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *from, unsigned int size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (from == NULL)
		return;
	if (h264or5_video_stream_framer->last_seen_sps) {
		free(h264or5_video_stream_framer->last_seen_sps);
		h264or5_video_stream_framer->last_seen_sps = NULL;
	}
	h264or5_video_stream_framer->last_seen_sps = (u_int8_t *)calloc(size, sizeof(u_int8_t));
	memmove(h264or5_video_stream_framer->last_seen_sps, from, size);

	h264or5_video_stream_framer->last_seen_sps_size = size;
}

void livemedia_h264or5_video_stream_framer__save_copy_of_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t *from, unsigned int size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (from == NULL)
		return;
	if (h264or5_video_stream_framer->last_seen_pps) {
		free(h264or5_video_stream_framer->last_seen_pps);
		h264or5_video_stream_framer->last_seen_pps = NULL;
	}
	h264or5_video_stream_framer->last_seen_pps = (u_int8_t *)calloc(size, sizeof(u_int8_t));
	memmove(h264or5_video_stream_framer->last_seen_pps, from, size);

	h264or5_video_stream_framer->last_seen_pps_size = size;

}




bool livemedia_h264or5_video_stream_framer__is_vps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* VPS NAL units occur in H.265 only: */
  return h264or5_video_stream_framer->h_number == 265 && nal_unit_type == 32;

}

bool livemedia_h264or5_video_stream_framer__is_sps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  return h264or5_video_stream_framer->h_number == 264 ? 
	  nal_unit_type == 7 : nal_unit_type == 33;
}

bool livemedia_h264or5_video_stream_framer__is_pps(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  return h264or5_video_stream_framer->h_number == 264 ? 
	  nal_unit_type == 8 : nal_unit_type == 34;

}

bool livemedia_h264or5_video_stream_framer__is_vcl(
		livemedia_h264or5_video_stream_framer_t *h264or5_video_stream_framer,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  return h264or5_video_stream_framer->h_number == 264
    ? (nal_unit_type <= 5 && nal_unit_type > 0)
    : (nal_unit_type <= 31);
}

unsigned int livemedia__remove_h264or5_emulation_bytes(u_int8_t *to, unsigned int to_max_size,
		u_int8_t *from, unsigned int from_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int to_size;
	unsigned int i;
	to_size = 0;
	i = 0;

	while (i < from_size && to_size+1 < to_max_size) {
		if (i+2 < from_size && from[i] == 0 && from[i+1] == 0 && from[i+2] == 3) {
			to[to_size] = to[to_size+1] = 0;
			to_size += 2;
			i += 3;
		} else {
			to[to_size] = from[i];
			to_size += 1;
			i += 1;
		}
	}

	return to_size;
}

/*
 * Static functions implementation for ancestor
 */

/*****************************************************************
 * H264or5VideoStreamParser:MPEGVideoStreamParser (Function Definitions)
 ****************************************************************/
#ifdef DEBUG
char const* nal_unit_type_description_h264[32] = {
	"Unspecified", //0
	"Coded slice of a non-IDR picture", //1
	"Coded slice data partition A", //2
	"Coded slice data partition B", //3
	"Coded slice data partition C", //4
	"Coded slice of an IDR picture", //5
	"Supplemental enhancement information (SEI)", //6
	"Sequence parameter set", //7
	"Picture parameter set", //8
	"Access unit delimiter", //9
	"End of sequence", //10
	"End of stream", //11
	"Filler data", //12
	"Sequence parameter set extension", //13
	"Prefix NAL unit", //14
	"Subset sequence parameter set", //15
	"Reserved", //16
	"Reserved", //17
	"Reserved", //18
	"Coded slice of an auxiliary coded picture without partitioning", //19
	"Coded slice extension", //20
	"Reserved", //21
	"Reserved", //22
	"Reserved", //23
	"Unspecified", //24
	"Unspecified", //25
	"Unspecified", //26
	"Unspecified", //27
	"Unspecified", //28
	"Unspecified", //29
	"Unspecified", //30
	"Unspecified" //31
};
char const* nal_unit_type_description_h265[64] = {
	"Coded slice segment of a non-TSA, non-STSA trailing picture", //0
	"Coded slice segment of a non-TSA, non-STSA trailing picture", //1
	"Coded slice segment of a TSA picture", //2
	"Coded slice segment of a TSA picture", //3
	"Coded slice segment of a STSA picture", //4
	"Coded slice segment of a STSA picture", //5
	"Coded slice segment of a RADL picture", //6
	"Coded slice segment of a RADL picture", //7
	"Coded slice segment of a RASL picture", //8
	"Coded slice segment of a RASL picture", //9
	"Reserved", //10
	"Reserved", //11
	"Reserved", //12
	"Reserved", //13
	"Reserved", //14
	"Reserved", //15
	"Coded slice segment of a BLA picture", //16
	"Coded slice segment of a BLA picture", //17
	"Coded slice segment of a BLA picture", //18
	"Coded slice segment of an IDR picture", //19
	"Coded slice segment of an IDR picture", //20
	"Coded slice segment of a CRA picture", //21
	"Reserved", //22
	"Reserved", //23
	"Reserved", //24
	"Reserved", //25
	"Reserved", //26
	"Reserved", //27
	"Reserved", //28
	"Reserved", //29
	"Reserved", //30
	"Reserved", //31
	"Video parameter set", //32
	"Sequence parameter set", //33
	"Picture parameter set", //34
	"Access unit delimiter", //35
	"End of sequence", //36
	"End of bitstream", //37
	"Filler data", //38
	"Supplemental enhancement information (SEI)", //39
	"Supplemental enhancement information (SEI)", //40
	"Reserved", //41
	"Reserved", //42
	"Reserved", //43
	"Reserved", //44
	"Reserved", //45
	"Reserved", //46
	"Reserved", //47
	"Unspecified", //48
	"Unspecified", //49
	"Unspecified", //50
	"Unspecified", //51
	"Unspecified", //52
	"Unspecified", //53
	"Unspecified", //54
	"Unspecified", //55
	"Unspecified", //56
	"Unspecified", //57
	"Unspecified", //58
	"Unspecified", //59
	"Unspecified", //60
	"Unspecified", //61
	"Unspecified", //62
	"Unspecified", //63
};
#endif

#ifdef DEBUG
static unsigned num_debug_tabs = 1;
#define DEBUG_PRINT_TABS for (unsigned _i = 0; _i < num_debug_tabs; ++_i) fprintf(stderr, "\t")
#define DEBUG_PRINT(x) do { DEBUG_PRINT_TABS; fprintf(stderr, "%s: %d\n", #x, x); } while (0)
#define DEBUG_STR(x) do { DEBUG_PRINT_TABS; fprintf(stderr, "%s\n", x); } while (0)
void livemedia__new__debug_tab(void)
{
	++num_debug_tabs;
}
void livemedia__delete__debug_tab(void)
{
	--num_debug_tabs;
}
#define DEBUG_TAB DebugTab dummy
#else
#define DEBUG_PRINT(x) do {x = x;} while (0)
/* Note: the "x=x;" statement is intended to eliminate "unused variable" compiler warning messages */
#define DEBUG_STR(x) do {} while (0)
#define DEBUG_TAB do {} while (0)
#endif

/*
 * Internal Functions for new() and delete() functions
 */

livemedia_h264or5_video_stream_parser_t *__livemedia_h264or5_video_stream_parser__alloc(void)
{
	livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser;
	h264or5_video_stream_parser = (livemedia_h264or5_video_stream_parser_t *)calloc(1, sizeof(livemedia_h264or5_video_stream_parser_t));
	if (h264or5_video_stream_parser == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_h264or5_video_stream_parser_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return h264or5_video_stream_parser;
}
void __livemedia_h264or5_video_stream_parser__init(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		int h_number, livemedia_h264or5_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source, bool include_start_code_in_output)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* parent init */
	__livemedia_mpeg_video_stream_parser__init(&h264or5_video_stream_parser->mpeg_video_stream_parser,
			(livemedia_mpeg_video_stream_framer_t *)using_source, input_source);
	/* constructor */
	h264or5_video_stream_parser->h_number = h_number;
	h264or5_video_stream_parser->output_start_code_size = (include_start_code_in_output ? 4 : 0);
	h264or5_video_stream_parser->have_seen_first_start_code = false;
	h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit = false;
	h264or5_video_stream_parser->parsed_frame_rate = 0.0;
	h264or5_video_stream_parser->cpb_removal_delay_length_minus1 = 23;
	h264or5_video_stream_parser->dpb_output_delay_length_minus1 = 23;
	h264or5_video_stream_parser->cpb_dpb_delays_present_flag = 0;
	h264or5_video_stream_parser->pic_struct_present_flag = 0;
	h264or5_video_stream_parser->delta_tfi_divisor = 2.0;

	/*  virtual functions configuration */ 
	((livemedia_stream_parser_t *)h264or5_video_stream_parser)->flush_input = 
		livemedia_h264or5_video_stream_parser__flush_input__impl;
	((livemedia_mpeg_video_stream_parser_t *)h264or5_video_stream_parser)->parse = 
		livemedia_h264or5_video_stream_parser__parse__impl;
	((livemedia_stream_parser_t *)h264or5_video_stream_parser)->delete = 
		livemedia_h264or5_video_stream_parser__delete__impl;
}
void __livemedia_h264or5_video_stream_parser__deinit(livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* parent deinit */
	__livemedia_mpeg_video_stream_parser__deinit(&h264or5_video_stream_parser->mpeg_video_stream_parser);
}
void __livemedia_h264or5_video_stream_parser__free(livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser)
{
	if (h264or5_video_stream_parser) {
		free(h264or5_video_stream_parser);
		h264or5_video_stream_parser = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_h264or5_video_stream_parser_t *livemedia__new__h264or5_video_stream_parser(
		int h_number, livemedia_h264or5_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source, bool include_start_code_in_output)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser;

	h264or5_video_stream_parser = __livemedia_h264or5_video_stream_parser__alloc();
	if (h264or5_video_stream_parser)
		__livemedia_h264or5_video_stream_parser__init(
				h264or5_video_stream_parser, h_number, using_source, input_source,
				include_start_code_in_output);

	return h264or5_video_stream_parser;
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_h264or5_video_stream_parser__flush_input__impl(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser;

	h264or5_video_stream_parser = (livemedia_h264or5_video_stream_parser_t *)stream_parser;
	h264or5_video_stream_parser->have_seen_first_start_code = false;
	h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit = false;
	
	livemedia_stream_parser__flush_input__impl(stream_parser);
}

unsigned int livemedia_h264or5_video_stream_parser__parse__impl(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int32_t first_4bytes;
	unsigned int remaining_data_size;
	unsigned int trailing_nal_unit_size;
	u_int8_t next_byte;
	u_int8_t nal_ref_idc;
	u_int8_t nal_unit_type;
	u_int32_t next4_bytes;
	unsigned int num_units_in_tick;
	unsigned int time_scale;
	unsigned long secs;
	unsigned int u_secs;
	bool this_nal_unit_ends_access_unit;
	u_int8_t first_bytes_of_next_nal_unit[3];
	u_int8_t next_nal_unit_type;
	u_int8_t byte_after_nal_unit_header;
	struct timeval next_pt;
	double next_fraction;
	unsigned next_secs_increment;
	livemedia_stream_parser_t *stream_parser;
	livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser;
	livemedia_h264or5_video_stream_framer_t *using_source;

	stream_parser = (livemedia_stream_parser_t *)mpeg_video_stream_parser;
	h264or5_video_stream_parser = (livemedia_h264or5_video_stream_parser_t *)mpeg_video_stream_parser;

	if (!setjmp(jmp_buf_e)) {
		/* The stream must start with a 0x00000001: */
		if (!h264or5_video_stream_parser->have_seen_first_start_code) {
			/* Skip over any input bytes that precede the first 0x00000001: */
			while ((first_4bytes = livemedia_stream_parser__test4_bytes(stream_parser)) 
					!= 0x00000001) {
				livemedia_stream_parser__get1_byte(stream_parser);
				livemedia_mpeg_video_stream_parser__set_parse_state(
						mpeg_video_stream_parser);
			}
			/* ensures that we progress over bad data */
			livemedia_stream_parser__skip_bytes(stream_parser, 4);
			/* skip this initial code */

			livemedia_mpeg_video_stream_parser__set_parse_state(
					mpeg_video_stream_parser);
			h264or5_video_stream_parser->have_seen_first_start_code = true;
			/* from now on */
		}

		if (h264or5_video_stream_parser->output_start_code_size > 0 &&
				livemedia_mpeg_video_stream_parser__cur_frame_size(
					mpeg_video_stream_parser) == 0 &&
				livemedia_stream_parser__have_seen_eof(stream_parser)) {
			/* Include a start code in the output: */
			livemedia_mpeg_video_stream_parser__save4_bytes(
					mpeg_video_stream_parser, 0x00000001);
		}	

		/* Then save everything up until the next 0x00000001 (4 bytes)
		 * or 0x000001 (3 bytes), or we hit EOF.
		 * Also make note of the first byte,
		 * because it contains the "nal_unit_type": */
		if (livemedia_stream_parser__have_seen_eof(stream_parser)) {
			/* We hit EOF the last time that we tried to parse this data,
			 * so we know that any remaining unparsed data forms a complete NAL unit,
			 * and that there's no 'start code' at the end: */
			remaining_data_size = *livemedia_stream_parser__tot_num_valid_bytes(
					stream_parser) - livemedia_stream_parser__cur_offset(
						stream_parser);
#ifdef DEBUG
			trailing_nal_unit_size = remaining_data_size;
#endif
			while (remaining_data_size > 0) {
				next_byte = livemedia_stream_parser__get1_byte(stream_parser);
				if (!h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit) {
					h264or5_video_stream_parser->first_byte_of_nal_unit = next_byte;
					h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit
						= true;
				}
				livemedia_mpeg_video_stream_parser__save_byte(
						mpeg_video_stream_parser, next_byte);
				--remaining_data_size;
			}

#ifdef DEBUG
			if (h264or5_video_stream_parser->h_number == 264) {
				nal_ref_idc = 
					(h264or5_video_stream_parser->first_byte_of_nal_unit&0x60)>>5;
				nal_unit_type = 
					h264or5_video_stream_parser->first_byte_of_nal_unit&0x1F;
				fprintf(stderr, "Parsed trailing %d-byte NAL-unit (nal_ref_idc: %d, nal_unit_type: %d (\"%s\"))\n",
						trailing_nal_unit_size, nal_ref_idc, nal_unit_type, nal_unit_type_description_h264[nal_unit_type]);
			} else { // 265
				u_int8_t nal_unit_type = 
					(h264or5_video_stream_parser->first_byte_of_nal_unit&0x7E)>>1;
				fprintf(stderr, "Parsed trailing %d-byte NAL-unit (nal_unit_type: %d (\"%s\"))\n",
						trailing_nal_unit_size, nal_unit_type, nal_unit_type_description_h265[nal_unit_type]);
			}
#endif
			(void)livemedia_stream_parser__get1_byte(stream_parser);
			/* forces another read, 
			 * which will cause EOF to get handled for real this time */
			return 0;
		} else {
			next4_bytes = livemedia_stream_parser__test4_bytes(stream_parser);
			if (!h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit) {
				h264or5_video_stream_parser->first_byte_of_nal_unit = next4_bytes>>24;
				h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit = true;
			}
			while (next4_bytes != 0x00000001 && 
					(next4_bytes&0xFFFFFF00) != 0x00000100) {
				/* We save at least some of "next4_bytes". */
				if ((unsigned int)(next4_bytes&0xFF) > 1) {
					/* Common case: 0x00000001 or 0x000001 definitely doesn't begin
					 * anywhere in "next4Bytes", so we save all of it: */
					livemedia_mpeg_video_stream_parser__save4_bytes(
							mpeg_video_stream_parser, next4_bytes);
					livemedia_stream_parser__skip_bytes(stream_parser, 4);
				} else {
					/* Save the first byte, and continue testing the rest: */
					livemedia_mpeg_video_stream_parser__save4_bytes(
							mpeg_video_stream_parser, next4_bytes>>24);
					livemedia_stream_parser__skip_bytes(stream_parser, 1);
				}
				livemedia_mpeg_video_stream_parser__set_parse_state(
						mpeg_video_stream_parser); /* ensures forward progress */
				next4_bytes = livemedia_stream_parser__test4_bytes(stream_parser);
			}
			/* Assert: next4Bytes starts with 0x00000001 or 0x000001,
			 * and we've saved all previous bytes (forming a complete NAL unit).
			 * Skip over these remaining bytes, up until the start
			 * of the next NAL unit: */
			if (next4_bytes == 0x00000001) {
				livemedia_stream_parser__skip_bytes(stream_parser, 4);
			} else {
				livemedia_stream_parser__skip_bytes(stream_parser, 3);
			}
		}

		h264or5_video_stream_parser->have_seen_first_byte_of_nal_unit = false;
		/* for the next NAL unit that we'll parse */
		if (h264or5_video_stream_parser->h_number == 264) {
			nal_unit_type = h264or5_video_stream_parser->first_byte_of_nal_unit&0x1F;
#ifdef DEBUG
			nal_ref_idc =
				(h264or5_video_stream_parser->first_byte_of_nal_unit&0x60)>>5;
			fprintf(stderr, "Parsed %d-byte NAL-unit (nal_ref_idc: %d, nal_unit_type: %d (\"%s\"))\n",
					livemedia_mpeg_video_stream_parser__cur_frame_size(
						mpeg_video_stream_parser) - 
					h264or5_video_stream_parser->output_start_code_size, 
					nal_ref_idc, nal_unit_type, nal_unit_type_description_h264[nal_unit_type]);
#endif
		} else {/* 265 */
			nal_unit_type = (h264or5_video_stream_parser->first_byte_of_nal_unit&0x7E)>>1;
#ifdef DEBUG
			fprintf(stderr, "Parsed %d-byte NAL-unit (nal_unit_type: %d (\"%s\"))\n",
					livemedia_mpeg_video_stream_parser__cur_frame_size(mpeg_video_stream_parser)-
					h264or5_video_stream_parser->output_start_code_size, nal_unit_type, nal_unit_type_description_h265[nal_unit_type]);
#endif
		}


		/* Now that we have found (& copied) a NAL unit, process it if it's of special interest to us: */
		if (livemedia_h264or5_video_stream_parser__is_vps(h264or5_video_stream_parser, nal_unit_type)) { /* Video parameter set */
			/* First, save a copy of this NAL unit, in case the downstream object wants to see it: */
			livemedia_h264or5_video_stream_framer__save_copy_of_vps(
					livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
					mpeg_video_stream_parser->start_of_frame + h264or5_video_stream_parser->output_start_code_size,
					livemedia_mpeg_video_stream_parser__cur_frame_size(mpeg_video_stream_parser)-
					h264or5_video_stream_parser->output_start_code_size);

			if (h264or5_video_stream_parser->parsed_frame_rate == 0.0) {
				/* We haven't yet parsed a frame rate from the stream.
				 * So parse this NAL unit to check whether frame rate information is present: */
				livemedia_h264or5_video_stream_parser__analyze_video_parameter_set_data(h264or5_video_stream_parser,
						&num_units_in_tick, &time_scale);
				if (time_scale > 0 && num_units_in_tick > 0) {
					using_source = livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser);
					((livemedia_mpeg_video_stream_framer_t *)using_source)->frame_rate = h264or5_video_stream_parser->parsed_frame_rate
						= time_scale / (h264or5_video_stream_parser->delta_tfi_divisor * num_units_in_tick);
#ifdef DEBUG
					fprintf(stderr, "Set frame rate to %f fps\n", using_source->frame_rate);
#endif
				} else {
#ifdef DEBUG
					using_source = livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser);
					fprintf(stderr, "\tThis \"Video Parameter Set\" NAL unit contained no frame rate information, so we use a default frame rate of %f fps\n",
							using_source->frame_rate);
#endif
				}
			}
		} else if (livemedia_h264or5_video_stream_parser__is_sps(h264or5_video_stream_parser, nal_unit_type)) { /*sequence parameter set */
			/* First, save a copy of this NAL unit, in case the downstream object wants to see it: */
			livemedia_h264or5_video_stream_framer__save_copy_of_sps( 
					livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
					mpeg_video_stream_parser->start_of_frame + h264or5_video_stream_parser->output_start_code_size,
					livemedia_mpeg_video_stream_parser__cur_frame_size(mpeg_video_stream_parser)-
					h264or5_video_stream_parser->output_start_code_size);
			if (h264or5_video_stream_parser->parsed_frame_rate == 0.0) {
				/* We haven't yet parsed a frame rate from the stream.
				 * So parse this NAL unit to check whether frame rate information is present: */
				livemedia_h264or5_video_stream_parser__analyze_seq_parameter_set_data(h264or5_video_stream_parser, 
						&num_units_in_tick, &time_scale);
				if (time_scale > 0 && num_units_in_tick > 0) {
					using_source = livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser);
					((livemedia_mpeg_video_stream_framer_t *)using_source)->frame_rate = h264or5_video_stream_parser->parsed_frame_rate
						= time_scale / (h264or5_video_stream_parser->delta_tfi_divisor * num_units_in_tick);
#ifdef DEBUG
					fprintf(stderr, "Set frame rate to %f fps\n", using_source->frame_rate);
#endif
				} else {
#ifdef DEBUG
					using_source = livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser);
					fprintf(stderr, "\tThis \"Sequence Parameter Set\" NAL unit contained no frame rate information, so we use a default frame rate of %f fps\n",
							using_source->frame_rate);
#endif
				}
			}
		} else if (livemedia_h264or5_video_stream_parser__is_pps(h264or5_video_stream_parser, nal_unit_type)) { /* Picture parameter set */
			/* Save a copy of this NAL unit, in case the downstream object wants to see it: */
			livemedia_h264or5_video_stream_framer__save_copy_of_pps( 
					livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
					mpeg_video_stream_parser->start_of_frame + h264or5_video_stream_parser->output_start_code_size,
					livemedia_mpeg_video_stream_parser__cur_frame_size(mpeg_video_stream_parser)-
					h264or5_video_stream_parser->output_start_code_size);
		} else if (livemedia_h264or5_video_stream_parser__is_sei(h264or5_video_stream_parser, nal_unit_type)) { /* supplemental enhancement information(SEI) */
			livemedia_h264or5_video_stream_parser__analyze_sei_data(h264or5_video_stream_parser, nal_unit_type);
			/* Later, perhaps adjust "fPresentationTime" if we saw a "pic_timing" SEI payload??? ##### */
		}
		livemedia_h264or5_video_stream_framer__set_presentation_time(
				livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser));	
#ifdef DEBUG
		using_source = livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser);
		secs = (unsigned long)(using_source->presentation_time.tv_sec);
		u_secs = (unsigned)(using_source->presentation_time.tv_usec);
		fprintf(stderr, "\tPresentation time: %lu.%06u\n", secs, u_secs);
#endif

		/* Now, check whether this NAL unit ends an 'access unit'.
		 * (RTP streamers need to know this in order to figure out whether or not to set the "M" bit.) */
		if (livemedia_stream_parser__have_seen_eof(stream_parser) ||
				livemedia_h264or5_video_stream_parser__is_eof(h264or5_video_stream_parser, nal_unit_type)) {
			/* There is no next NAL unit, so we assume that this one ends the current 'access unit': */
			this_nal_unit_ends_access_unit = true;
		} else if (livemedia_h264or5_video_stream_parser__usually_begin_access_unit(
					h264or5_video_stream_parser, nal_unit_type)) {
			/* These NAL units usually *begin* an access unit, so assume that they don't end one here: */
			this_nal_unit_ends_access_unit = false;
		} else {
			/* We need to check the *next* NAL unit to figure out whether
			 * the current NAL unit ends an 'access unit': */
			livemedia_stream_parser__test_bytes(stream_parser, first_bytes_of_next_nal_unit, 3);

			next_nal_unit_type = (h264or5_video_stream_parser->h_number == 264)
				? (first_bytes_of_next_nal_unit[0]&0x1F) : ((first_bytes_of_next_nal_unit[0]&0x7E)>>1);
			if (livemedia_h264or5_video_stream_parser__is_vcl(h264or5_video_stream_parser, next_nal_unit_type)) {
				/* The high-order bit of the byte after the "nal_unit_header" tells us whether it's
				 * the start of a new 'access unit' (and thus the current NAL unit ends an 'access unit'): */
				byte_after_nal_unit_header = h264or5_video_stream_parser->h_number == 264 ?
					first_bytes_of_next_nal_unit[1] : first_bytes_of_next_nal_unit[2];
				this_nal_unit_ends_access_unit = (byte_after_nal_unit_header&0x80) != 0;
			} else if (livemedia_h264or5_video_stream_parser__usually_begin_access_unit(
						h264or5_video_stream_parser, next_nal_unit_type)) {
				/* The next NAL unit's type is one that usually appears at the start of an 'access unit',
				 * so we assume that the current NAL unit ends an 'access unit': */
				this_nal_unit_ends_access_unit = true;
			} else {
				/* The next NAL unit definitely doesn't start a new 'access unit',
				 * which means that the current NAL unit doesn't end one: */
				this_nal_unit_ends_access_unit = false;
			}
		}

		if (this_nal_unit_ends_access_unit) {
#ifdef DEBUG
			fprintf(stderr, "*****This NAL unit ends the current access unit*****\n");
#endif
			using_source = livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser);
			((livemedia_mpeg_video_stream_framer_t *)using_source)->picture_end_marker = true;
			++((livemedia_mpeg_video_stream_framer_t *)using_source)->picture_count;

			/* Note that the presentation time for the next NAL unit will be different: */
			next_pt = using_source->next_presentation_time; /* alias */
			next_pt = ((livemedia_framed_source_t *)using_source)->presentation_time; /* alias */
			next_fraction = next_pt.tv_usec/1000000.0 + 1/((livemedia_mpeg_video_stream_framer_t *)using_source)->frame_rate;
			next_secs_increment = (long)next_fraction;
			next_pt.tv_sec += (long)next_secs_increment;
			next_pt.tv_usec = (long)((next_fraction - next_secs_increment)*1000000);
		}
		livemedia_mpeg_video_stream_parser__set_parse_state(mpeg_video_stream_parser);

		return livemedia_mpeg_video_stream_parser__cur_frame_size(mpeg_video_stream_parser);

	} else {
#ifdef DEBUG
		fprintf(stderr, "H264or5VideoStreamParser::parse() EXCEPTION (This is normal behavior - *not* an error)\n");
#endif
		return 0;  /* the parsing got interrupted */
	}
}

void livemedia_h264or5_video_stream_parser__delete__impl(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser;
	h264or5_video_stream_parser = (livemedia_h264or5_video_stream_parser_t *)stream_parser;
	if (h264or5_video_stream_parser) {
		__livemedia_h264or5_video_stream_parser__deinit(h264or5_video_stream_parser);
		__livemedia_h264or5_video_stream_parser__free(h264or5_video_stream_parser);
	}
}

/*
 * Functions in header file
 */
livemedia_h264or5_video_stream_framer_t *livemedia_h264or5_video_stream_parser__using_source(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (livemedia_h264or5_video_stream_framer_t *)
		((livemedia_mpeg_video_stream_parser_t *)h264or5_video_stream_parser)->using_source;

}

bool livemedia_h264or5_video_stream_parser__is_vps(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_h264or5_video_stream_framer__is_vps(
				livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
				nal_unit_type);

}

bool livemedia_h264or5_video_stream_parser__is_sps(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_h264or5_video_stream_framer__is_sps(
				livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
				nal_unit_type);

}

bool livemedia_h264or5_video_stream_parser__is_pps(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_h264or5_video_stream_framer__is_pps(
				livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
				nal_unit_type);

}

bool livemedia_h264or5_video_stream_parser__is_vcl(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_h264or5_video_stream_framer__is_vcl(
				livemedia_h264or5_video_stream_parser__using_source(h264or5_video_stream_parser),
				nal_unit_type);

}



/*
 * Normal functions
 */
#define PREFIX_SEI_NUT 39 // for H.265
#define SUFFIX_SEI_NUT 40 // for H.265
bool livemedia_h264or5_video_stream_parser__is_sei(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return h264or5_video_stream_parser->h_number == 264
		? nal_unit_type == 6
		: (nal_unit_type == PREFIX_SEI_NUT || nal_unit_type == SUFFIX_SEI_NUT);

}

bool livemedia_h264or5_video_stream_parser__is_eof(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* "end of sequence" or "end of (bit)stream" */
	return h264or5_video_stream_parser->h_number == 264
		? (nal_unit_type == 10 || nal_unit_type == 11)
		: (nal_unit_type == 36 || nal_unit_type == 37);

}

bool livemedia_h264or5_video_stream_parser__usually_begin_access_unit(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return h264or5_video_stream_parser->h_number == 264
		? (nal_unit_type >= 6 && nal_unit_type <= 9) || (nal_unit_type >= 14 && nal_unit_type <= 18)
		: (nal_unit_type >= 32 && nal_unit_type <= 35) || (nal_unit_type == 39)
		|| (nal_unit_type >= 41 && nal_unit_type <= 44)
		|| (nal_unit_type >= 48 && nal_unit_type <= 55);
}


void livemedia_h264or5_video_stream_parser__remove_emulation_bytes(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t *nal_unit_copy, unsigned int max_size, unsigned int *nal_unit_copy_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int8_t *nal_unit_orig;
	unsigned int num_bytes_in_nal_unit;
	livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser;
	mpeg_video_stream_parser = 
		(livemedia_mpeg_video_stream_parser_t *)h264or5_video_stream_parser;

	nal_unit_orig = mpeg_video_stream_parser->start_of_frame + 
		h264or5_video_stream_parser->output_start_code_size; 
	num_bytes_in_nal_unit = mpeg_video_stream_parser->to - nal_unit_orig;

	*nal_unit_copy_size = livemedia__remove_h264or5_emulation_bytes(nal_unit_copy,
			max_size, nal_unit_orig, num_bytes_in_nal_unit);
}


void livemedia_h264or5_video_stream_parser__analyze_video_parameter_set_data(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		unsigned int *num_units_in_tick, unsigned int *time_scale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int8_t vps[VPS_MAX_SIZE];
	unsigned int vps_size;
	livemedia_bit_vector_t *bv;
	unsigned int i;
  	unsigned int vps_max_sub_layers_minus1;
	bool vps_sub_layer_ordering_info_present_flag;
	unsigned int vps_max_layer_id;
	unsigned int vps_num_layer_sets_minus1;
	bool vps_timing_info_present_flag;
	bool vps_poc_proportional_to_timing_flag;
	unsigned int vps_num_ticks_poc_diff_one_minus1;
	bool vps_extension_flag;

	*num_units_in_tick = *time_scale = 0; /* default values */

	/* Begin by making a copy of the NAL unit data, removing any 'emulation prevention' bytes: */
	livemedia_h264or5_video_stream_parser__remove_emulation_bytes(h264or5_video_stream_parser, vps,
			sizeof(vps), &vps_size);
	bv = livemedia__new__bit_vector(vps, 0, 8 * vps_size);

	/* Assert: fHNumber == 265 (because this function is called only when parsing H.265) */

	livemedia_bit_vector__skip_bits(bv, 28);
	/* nal_unit_header, vps_video_parameter_set_id, vps_reserved_three_2bits, vps_max_layers_minus1 */
	vps_max_sub_layers_minus1 = livemedia_bit_vector__get_bits(bv, 3);
	DEBUG_PRINT(vps_max_sub_layers_minus1);
	livemedia_bit_vector__skip_bits(bv, 17);
	/* vps_temporal_id_nesting_flag, vps_reserved_0xffff_16bits */
	livemedia_h264or5_video_stream_parser__profile_tier_level(h264or5_video_stream_parser, bv,
			vps_max_sub_layers_minus1);
	vps_sub_layer_ordering_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
	DEBUG_PRINT(vps_sub_layer_ordering_info_present_flag);

	for (i = vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1;
			i <= vps_max_sub_layers_minus1; ++i) {
		(void)livemedia_bit_vector__get_exp_golomb(bv);	/* vps_max_dec_pic_buffering_minus1[i] */
		(void)livemedia_bit_vector__get_exp_golomb(bv);	/* vps_max_num_reorder_pics[i] */
		(void)livemedia_bit_vector__get_exp_golomb(bv);	/* vps_max_latency_increase_plus1[i] */
	}
	vps_max_layer_id = livemedia_bit_vector__get_bits(bv, 6);
	DEBUG_PRINT(vps_max_layer_id);
	vps_num_layer_sets_minus1 = livemedia_bit_vector__get_exp_golomb(bv);
	DEBUG_PRINT(vps_num_layer_sets_minus1);
	for (i = 1; i <= vps_num_layer_sets_minus1; ++i) {
		livemedia_bit_vector__skip_bits(bv, vps_max_layer_id + 1); /* layer_id_included_flag[i][0..vps_max_layer_id] */
	}
	vps_timing_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
	DEBUG_PRINT(vps_timing_info_present_flag);
	if (vps_timing_info_present_flag) {
		fprintf(stderr, "\t");
		*num_units_in_tick = livemedia_bit_vector__get_bits(bv, 32);
		DEBUG_PRINT(num_units_in_tick);
		*time_scale = livemedia_bit_vector__get_bits(bv, 32);
		DEBUG_PRINT(time_scale);
		vps_poc_proportional_to_timing_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(vps_poc_proportional_to_timing_flag);
		if (vps_poc_proportional_to_timing_flag) {
			vps_num_ticks_poc_diff_one_minus1 = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(vps_num_ticks_poc_diff_one_minus1);
		}
	}
	vps_extension_flag = livemedia_bit_vector__get1_bit_boolean(bv);
	DEBUG_PRINT(vps_extension_flag);
	livemedia__delete__bit_vector(bv);
}

void livemedia_h264or5_video_stream_parser__analyze_seq_parameter_set_data(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		unsigned int *num_units_in_tick, unsigned int *time_scale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	u_int8_t sps[SPS_MAX_SIZE];
	unsigned int sps_size;
	livemedia_bit_vector_t *bv;
	unsigned int profile_idc;
	unsigned int constraint_set_n_flag;
	unsigned int level_idc;
	unsigned int seq_parameter_set_id;
	unsigned int chroma_format_idc;
	bool separate_colour_plane_flag;
	bool seq_scaling_matrix_present_flag;
	int i;
	bool seq_scaling_list_present_flag;
	unsigned int size_of_scaling_list;
	unsigned int last_scale;
	unsigned int next_scale;
	unsigned int j;
	int delta_scale;
	unsigned int log2_max_frame_num_minus4;
	unsigned int pic_order_cnt_type;
	unsigned int log2_max_pic_order_cnt_lsb_minus4;
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;
	unsigned int max_num_ref_frames;
	bool gaps_in_frame_num_value_allowed_flag;
	unsigned int pic_width_in_mbs_minus1;
    unsigned pic_height_in_map_units_minus1;
	bool frame_mbs_only_flag;
	bool frame_cropping_flag;
	bool vui_parameters_present_flag;
	unsigned int sps_max_sub_layers_minus1;
	unsigned int pic_width_in_luma_samples;
	unsigned int pic_height_in_luma_samples;
	bool conformance_window_flag;
	unsigned int conf_win_left_offset;
	unsigned int conf_win_right_offset;
	unsigned int conf_win_top_offset;
	unsigned int conf_win_bottom_offset;
	bool sps_sub_layer_ordering_info_present_flag;
	bool scaling_list_enabled_flag;
	bool sps_scaling_list_data_present_flag;
	unsigned int size_id;
	unsigned int matrix_id;
	bool scaling_list_pred_mode_flag;
	unsigned int c;
	unsigned int coef_num;
	bool pcm_enabled_flag;
	unsigned int num_short_term_ref_pic_sets;
	unsigned int num_negative_pics;
	unsigned int prev_num_negative_pics;
	unsigned int num_positive_pics;
	unsigned int prev_num_positive_pics;
	bool inter_ref_pic_set_prediction_flag;
	unsigned int num_delta_pocs;
	bool used_by_curr_pic_flag;
	unsigned int k;
	bool long_term_ref_pics_present_flag;
	unsigned int num_long_term_ref_pics_sps;
	bool sps_extension_flag;

	*num_units_in_tick = *time_scale = 0; /* default values */
	/* Begin by making a copy of the NAL unit data,
	 * removing any 'emulation prevention' bytes: */
	livemedia_h264or5_video_stream_parser__remove_emulation_bytes(
			h264or5_video_stream_parser, sps, sizeof(sps), &sps_size);

	bv = livemedia__new__bit_vector(sps, 0, 8 * sps_size);

	if (h264or5_video_stream_parser->h_number == 264) {
		livemedia_bit_vector__skip_bits(bv, 8); 
		/* forbidden_zero_bit; nal_ref_idc; nal_unit_type */
		profile_idc = livemedia_bit_vector__get_bits(bv, 8);
		DEBUG_PRINT(profile_idc);
		constraint_set_n_flag = livemedia_bit_vector__get_bits(bv, 8);
		/* also "reserved_zero_2bits" at end */
		DEBUG_PRINT(constraint_set_n_flag);
		level_idc = livemedia_bit_vector__get_bits(bv, 8);
		DEBUG_PRINT(level_idc);
		seq_parameter_set_id = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(seq_parameter_set_id);
		if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 || profile_idc == 44 || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128 ) {
			DEBUG_TAB;
			chroma_format_idc = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(chroma_format_idc);
			if (chroma_format_idc == 3) {
				DEBUG_TAB;
				separate_colour_plane_flag = livemedia_bit_vector__get1_bit_boolean(bv);
				DEBUG_PRINT(separate_colour_plane_flag);
			}
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* bit_depth_luma_minus8 */
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* bit_depth_chroma_minus8 */
			livemedia_bit_vector__skip_bits(bv, 1); 
			/* qpprime_y_zero_transform_bypass_flag */
			seq_scaling_matrix_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
			DEBUG_PRINT(seq_scaling_matrix_present_flag);
			if (seq_scaling_matrix_present_flag) {
				for (i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); ++i) {
					DEBUG_TAB;
					DEBUG_PRINT(j);
					seq_scaling_list_present_flag = 
						livemedia_bit_vector__get1_bit_boolean(bv);
					DEBUG_PRINT(seq_scaling_list_present_flag);
					if (seq_scaling_list_present_flag) {
						DEBUG_TAB;
						size_of_scaling_list = i < 6 ? 16: 64;
						last_scale = 8;
						next_scale = 8;
						for (j = 0; j < size_of_scaling_list; ++j) {
							DEBUG_TAB;
							DEBUG_PRINT(j);
							DEBUG_PRINT(next_scale);
							if (next_scale != 0) {
								DEBUG_TAB;
								delta_scale = 
									livemedia_bit_vector__get_exp_golomb_signed(bv);
								DEBUG_PRINT(delta_scale);
								next_scale = (last_scale + delta_scale + 256) % 256;
							}
							last_scale = (next_scale == 0) ? last_scale : next_scale;
							DEBUG_PRINT(last_scale);
						}
					}
				}
			}
		}                                                  
		log2_max_frame_num_minus4 = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(log2_max_frame_num_minus4);
		pic_order_cnt_type = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(pic_order_cnt_type);
		if (pic_order_cnt_type == 0) {
			DEBUG_TAB;
			log2_max_pic_order_cnt_lsb_minus4 = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(log2_max_pic_order_cnt_lsb_minus4);
		} else if (pic_order_cnt_type == 1) {
			DEBUG_TAB;
			livemedia_bit_vector__skip_bits(bv, 1); /*delta_pic_order_always_zero_flag*/
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* offset_for_non_ref_pic */
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* offset_for_top_to_bottom_field */
			num_ref_frames_in_pic_order_cnt_cycle = 
				livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(num_ref_frames_in_pic_order_cnt_cycle);
			for (i = 0; i <num_ref_frames_in_pic_order_cnt_cycle; ++i) {
				livemedia_bit_vector__get_exp_golomb_signed(bv); 
				/* offset_for_ref_frame[i] */
			}
		}
		max_num_ref_frames = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(max_num_ref_frames);
		gaps_in_frame_num_value_allowed_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(gaps_in_frame_num_value_allowed_flag);
		pic_width_in_mbs_minus1 = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(pic_width_in_mbs_minus1);
		pic_height_in_map_units_minus1 = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(pic_height_in_map_units_minus1);
		frame_mbs_only_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(frame_mbs_only_flag);
		if (!frame_mbs_only_flag){
			livemedia_bit_vector__skip_bits(bv, 1); /* mb_adaptive_frame_field_flag */
		}
		livemedia_bit_vector__skip_bits(bv, 1); /* direct_8x8_inference_flag */
		frame_cropping_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(frame_cropping_flag);
		if (frame_cropping_flag) {
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* frame_crop_left_offset */
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* frame_crop_right_offset */
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* frame_crop_top_offset */
			(void)livemedia_bit_vector__get_exp_golomb(bv); /* frame_crop_bottom_offset*/
		}
		vui_parameters_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(vui_parameters_present_flag);
		if (vui_parameters_present_flag) {
			DEBUG_TAB;
			livemedia_h264or5_video_stream_parser__analyze_vui_parameters(
					h264or5_video_stream_parser, bv, num_units_in_tick, time_scale);
		}
	} else { /* 265 */
		livemedia_bit_vector__skip_bits(bv, 16); /* nal_unit_header */
		livemedia_bit_vector__skip_bits(bv, 4);	 /* sps_video_parameter_set_id */
		sps_max_sub_layers_minus1 = livemedia_bit_vector__get_bits(bv, 3);
		DEBUG_PRINT(sps_max_sub_layers_minus1);
		livemedia_bit_vector__skip_bits(bv, 1);	/* sps_temporal_id_nesting_flag */
		livemedia_h264or5_video_stream_parser__profile_tier_level(
				h264or5_video_stream_parser, bv, sps_max_sub_layers_minus1);
		(void)livemedia_bit_vector__get_exp_golomb(bv); /* sps_seq_parameter_set_id */
		chroma_format_idc = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(chroma_format_idc);
		if (chroma_format_idc == 3)
			livemedia_bit_vector__skip_bits(bv, 3); /* separate_colour_plane_flag */
		pic_width_in_luma_samples = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(pic_width_in_luma_samples);
		pic_height_in_luma_samples = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(pic_height_in_luma_samples);
		conformance_window_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(conformance_window_flag);
		if (conformance_window_flag) {
			DEBUG_TAB;
			conf_win_left_offset = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(conf_win_left_offset);
			conf_win_right_offset = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(conf_win_right_offset);
			conf_win_top_offset = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(conf_win_top_offset);
			conf_win_bottom_offset = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(conf_win_bottom_offset);
		}
		(void)livemedia_bit_vector__get_exp_golomb(bv); /* bit_depth_luma_minus8 */
		(void)livemedia_bit_vector__get_exp_golomb(bv); /* bit_depth_chroma_minus8 */
		log2_max_pic_order_cnt_lsb_minus4 = livemedia_bit_vector__get_exp_golomb(bv);
		sps_sub_layer_ordering_info_present_flag = 
			livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(sps_sub_layer_ordering_info_present_flag);
		for (i = (sps_sub_layer_ordering_info_present_flag ?
					0 : sps_max_sub_layers_minus1);
				i <= sps_max_sub_layers_minus1; ++i) {
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* sps_max_dec_pic_buffering_minus1[i] */
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* sps_max_num_reorder_pics[i] */
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* sps_max_latency_increase[i] */
		}
		(void)livemedia_bit_vector__get_exp_golomb(bv); 
		/* log2_min_luma_coding_block_size_minus3 */
		(void)livemedia_bit_vector__get_exp_golomb(bv); 
		/* log2_diff_max_min_luma_coding_block_size */
		(void)livemedia_bit_vector__get_exp_golomb(bv); 
		/* log2_min_transform_block_size_minus2 */
		(void)livemedia_bit_vector__get_exp_golomb(bv); 
		/* log2_diff_max_min_transform_block_size */
		(void)livemedia_bit_vector__get_exp_golomb(bv); 
		/* max_transform_hierarchy_depth_inter */
		(void)livemedia_bit_vector__get_exp_golomb(bv); 
		/* max_transform_hierarchy_depth_intra */
		scaling_list_enabled_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(scaling_list_enabled_flag);
		if (scaling_list_enabled_flag) {
			DEBUG_TAB;
			sps_scaling_list_data_present_flag = 
				livemedia_bit_vector__get1_bit_boolean(bv);
			DEBUG_PRINT(sps_scaling_list_data_present_flag);
			if (sps_scaling_list_data_present_flag) {
				/* scaling_list_data() */
				DEBUG_TAB;
				for (size_id = 0; size_id < 4; ++size_id) {
					DEBUG_PRINT(size_id);
					for (matrix_id = 0; matrix_id < (size_id == 3 ? 2: 6); ++matrix_id) {
						DEBUG_TAB;
						DEBUG_PRINT(matrix_id);
						scaling_list_pred_mode_flag =
							livemedia_bit_vector__get1_bit_boolean(bv);
						DEBUG_PRINT(scaling_list_pred_mode_flag);
						if (!scaling_list_pred_mode_flag) {
							(void)livemedia_bit_vector__get_exp_golomb(bv); 
							/* scaling_list_pred_matrix_id_delta[size_id][matric_id] */
						} else {
							c = 1 << (4+(size_id << 1));
							coef_num = c < 64 ? c: 64;
							if (size_id > 1) {
								(void)livemedia_bit_vector__get_exp_golomb(bv); 
								/* scaling_list_dc_coef_minus8[size_id][matrix_id] */
							}
							for (i = 0; i < coef_num; ++i) {
								(void)livemedia_bit_vector__get_exp_golomb(bv); 
								/* scaling_list_delta_coef */
							}
						}
					}
				}
			}
		}
		livemedia_bit_vector__skip_bits(bv, 2); 
		/* amp_enabled_flag, sample_adaptive_offset_enabled_flag */
		pcm_enabled_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(pcm_enabled_flag);
		if (pcm_enabled_flag) {
			livemedia_bit_vector__skip_bits(bv, 8);
			/* pcm_sample_bit_depth_luma_minus1, pcm_sample_bit_depth_chroma_minus1 */
			(void)livemedia_bit_vector__get_exp_golomb(bv); 
			/* log2_min_pcm_luma_coding_block_size_minus3 */
			(void)livemedia_bit_vector__get_exp_golomb(bv); 
			/* log2_diff_max_min_pcm_luma_coding_block_size */
			livemedia_bit_vector__skip_bits(bv, 1); /* pcm_loop_filter_disabled_flag */
		}
		num_short_term_ref_pic_sets = livemedia_bit_vector__get_exp_golomb(bv);
		DEBUG_PRINT(num_short_term_ref_pic_sets);
		num_negative_pics = 0;
		prev_num_negative_pics = 0;
		num_positive_pics = 0;
		prev_num_positive_pics = 0;
		for (i = 0; i < num_short_term_ref_pic_sets; ++i) {
			/* short_term_ref_pic_set(i): */
			DEBUG_TAB;
			DEBUG_PRINT(i);
			inter_ref_pic_set_prediction_flag = false;
			if (i != 0) {
				inter_ref_pic_set_prediction_flag =
					livemedia_bit_vector__get1_bit_boolean(bv);
			}
			DEBUG_PRINT(inter_ref_pic_set_prediction_flag);
			if (inter_ref_pic_set_prediction_flag) {
				DEBUG_TAB;
				if (i == num_short_term_ref_pic_sets) {
					/* This can't happen here, but it's in the spec, 
					 * so we include it for completeness */
					(void)livemedia_bit_vector__get_exp_golomb(bv);/* delta_idx_minus1 */
				}
				livemedia_bit_vector__skip_bits(bv, 1); /* delta_rps_sign */
				(void)livemedia_bit_vector__get_exp_golomb(bv);/* abs_delta_rps_minus1 */
				num_delta_pocs = prev_num_negative_pics + prev_num_positive_pics;/* correct??? */

				for (j = 0; j < num_delta_pocs; ++j) {
					DEBUG_PRINT(j);
					used_by_curr_pic_flag = livemedia_bit_vector__get1_bit_boolean(bv);
					DEBUG_PRINT(used_by_curr_pic_flag);
					if (!used_by_curr_pic_flag) 
						livemedia_bit_vector__skip_bits(bv, 1); /* use_delta_flag[j] */
				}
			} else {
				prev_num_negative_pics = num_negative_pics;
				num_negative_pics = livemedia_bit_vector__get_exp_golomb(bv);
				DEBUG_PRINT(num_negative_pics);
				prev_num_positive_pics = num_positive_pics;
				num_positive_pics = livemedia_bit_vector__get_exp_golomb(bv);
				DEBUG_PRINT(num_positive_pics);
				for (k = 0; k < num_negative_pics; ++k) {
					(void)livemedia_bit_vector__get_exp_golomb(bv);
					/* delta_poc_s0_minus1[k] */
					livemedia_bit_vector__skip_bits(bv, 1);
					/* used_by_curr_pic_s0_flag[k] */
				}
				for (k = 0; k < num_positive_pics; ++k) {
					(void)livemedia_bit_vector__get_exp_golomb(bv);
					/* delta_poc_s1_minus1[k] */
					livemedia_bit_vector__skip_bits(bv, 1);
					/* used_by_curr_pic_s1_flag[k] */
				}
			}
		}
		long_term_ref_pics_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(long_term_ref_pics_present_flag);
		if (long_term_ref_pics_present_flag) {
			DEBUG_TAB;
			num_long_term_ref_pics_sps = livemedia_bit_vector__get_exp_golomb(bv);
			DEBUG_PRINT(num_long_term_ref_pics_sps);
			for (i = 0; i < num_long_term_ref_pics_sps; ++i) { 
				livemedia_bit_vector__skip_bits(bv, log2_max_pic_order_cnt_lsb_minus4);
				 /* lt_ref_pic_poc_lsb_sps[i] */
				livemedia_bit_vector__skip_bits(bv, 1);
				/* used_by_curr_pic_lt_sps_flag[1] */
			}
		}
		livemedia_bit_vector__skip_bits(bv, 2);
		/* sps_temporal_mvp_enabled_flag, strong_intra_smoothing_enabled_flag */
		vui_parameters_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(vui_parameters_present_flag);
		if (vui_parameters_present_flag) {
			DEBUG_TAB;
			livemedia_h264or5_video_stream_parser__analyze_vui_parameters(
					h264or5_video_stream_parser, bv, num_units_in_tick, time_scale);
		}
		sps_extension_flag = livemedia_bit_vector__get1_bit_boolean(bv);
		DEBUG_PRINT(sps_extension_flag);
	}
	livemedia__delete__bit_vector(bv);
}

void livemedia_h264or5_video_stream_parser__profile_tier_level(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		livemedia_bit_vector_t *bv, unsigned int max_sub_layers_minus1)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_h264or5_video_stream_parser__analyze_vui_parameters(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		livemedia_bit_vector_t *bv, unsigned int *num_units_in_tick,
		unsigned int *time_scale)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	bool aspect_ratio_info_present_flag;
	unsigned int aspect_ratio_idc;
	bool overscan_info_present_flag;
	bool video_signal_type_present_flag;
	bool colour_description_present_flag;
	bool chroma_loc_info_present_flag;
	bool frame_field_info_present_flag;
	bool default_display_window_flag;
	bool timing_info_present_flag;
	bool fixed_frame_rate_flag;
	bool vui_poc_proportional_to_timing_flag;
	unsigned int vui_num_ticks_poc_diff_one_minus1;
	bool nal_hrd_parameters_present_flag;
	bool vcl_hrd_parameters_present_flag;

	aspect_ratio_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
	DEBUG_PRINT(aspect_ratio_info_present_flag);
	if (aspect_ratio_info_present_flag) {
		DEBUG_TAB;
		aspect_ratio_idc = livemedia_bit_vector__get_bits(bv, 8);
		DEBUG_PRINT(aspect_ratio_idc);
		if (aspect_ratio_idc == 255 /*Extended_SAR*/) {
			livemedia_bit_vector__skip_bits(bv, 32); /* sar_width; sar_height */
		}
	}
	overscan_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv);
	DEBUG_PRINT(overscan_info_present_flag);
	if (overscan_info_present_flag) {
		livemedia_bit_vector__skip_bits(bv, 1); /* overscan_appropriate_flag */
	}
	video_signal_type_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
	DEBUG_PRINT(video_signal_type_present_flag);
	if (video_signal_type_present_flag) {
		DEBUG_TAB;
		livemedia_bit_vector__skip_bits(bv, 4); /* video_format; video_full_range_flag */
		colour_description_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
		DEBUG_PRINT(colour_description_present_flag);
		if (colour_description_present_flag) {
			livemedia_bit_vector__skip_bits(bv, 24);
			/* colour_primaries; transfer_characteristics; matrix_coefficients */
		}
	}
	chroma_loc_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
	DEBUG_PRINT(chroma_loc_info_present_flag);
	if (chroma_loc_info_present_flag) {
		(void)livemedia_bit_vector__get_exp_golomb(bv);
		/* chroma_sample_loc_type_top_field */
		(void)livemedia_bit_vector__get_exp_golomb(bv);
		/* chroma_sample_loc_type_bottom_field */
	}
	if (h264or5_video_stream_parser->h_number == 265) {
		livemedia_bit_vector__skip_bits(bv, 2);
		/* neutral_chroma_indication_flag, field_seq_flag */
		frame_field_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
		DEBUG_PRINT(frame_field_info_present_flag);
		h264or5_video_stream_parser->pic_struct_present_flag = 
			livemedia_bit_vector__get1_bit_boolean(bv); 
		/* hack to make H.265 like H.264 */

		default_display_window_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
		DEBUG_PRINT(default_display_window_flag);
		if (default_display_window_flag) {
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* def_disp_win_left_offset */
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* def_disp_win_right_offset */
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* def_disp_win_top_offset */
			(void)livemedia_bit_vector__get_exp_golomb(bv);
			/* def_disp_win_bottom_offset */
		}
	}
	timing_info_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
	DEBUG_PRINT(timing_info_present_flag);
	if (timing_info_present_flag) {
		DEBUG_TAB;
		*num_units_in_tick = livemedia_bit_vector__get_bits(bv, 32);
		DEBUG_PRINT(*num_units_in_tick);
		*time_scale = livemedia_bit_vector__get_bits(bv, 32);
		DEBUG_PRINT(*time_scale);
		if (h264or5_video_stream_parser->h_number == 264) {
			fixed_frame_rate_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
			DEBUG_PRINT(fixed_frame_rate_flag);
		} else { /* 265 */
			vui_poc_proportional_to_timing_flag = 
				livemedia_bit_vector__get1_bit_boolean(bv); 
			DEBUG_PRINT(vui_poc_proportional_to_timing_flag);
			if (vui_poc_proportional_to_timing_flag) {
				vui_num_ticks_poc_diff_one_minus1 = 
					livemedia_bit_vector__get_exp_golomb(bv);
				DEBUG_PRINT(vui_num_ticks_poc_diff_one_minus1);
			}
			return; /* For H.265, don't bother parsing any more of this ##### */
		}
	}
	/* The following is H.264 only: ##### */
	nal_hrd_parameters_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
	DEBUG_PRINT(nal_hrd_parameters_present_flag);
	if (nal_hrd_parameters_present_flag)
		livemedia_h264or5_video_stream_parser__analyze_hrd_parameters(
				h264or5_video_stream_parser, bv);
	vcl_hrd_parameters_present_flag = livemedia_bit_vector__get1_bit_boolean(bv); 
	DEBUG_PRINT(vcl_hrd_parameters_present_flag);
	if (vcl_hrd_parameters_present_flag)
		livemedia_h264or5_video_stream_parser__analyze_hrd_parameters(
				h264or5_video_stream_parser, bv);
	h264or5_video_stream_parser->cpb_dpb_delays_present_flag = 
		nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag;
	if (h264or5_video_stream_parser->cpb_dpb_delays_present_flag) {
		livemedia_bit_vector__skip_bits(bv, 1); /* low_delay_hrd_flag */
	}
	h264or5_video_stream_parser->pic_struct_present_flag = 
		livemedia_bit_vector__get1_bit_boolean(bv); 
	DEBUG_PRINT(h264or5_video_stream_parser->pic_struct_present_flag);
}

void livemedia_h264or5_video_stream_parser__analyze_hrd_parameters(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		livemedia_bit_vector_t *bv)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_h264or5_video_stream_parser__analyze_sei_data(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		u_int8_t nal_unit_type)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_h264or5_video_stream_parser__analyze_sei_payload(
		livemedia_h264or5_video_stream_parser_t *h264or5_video_stream_parser,
		unsigned int paylead_type, unsigned int payload_size, u_int8_t *payload)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Static functions implementation for ancestor
 */
