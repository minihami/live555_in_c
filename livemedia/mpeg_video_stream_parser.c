#include "mpeg_video_stream_parser.h"

/*****************************************************************
 * MPEGVideoStreamParser:StreamParser (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mpeg_video_stream_parser_t *__livemedia_mpeg_video_stream_parser__alloc(void)
{
	livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser;
	mpeg_video_stream_parser = (livemedia_mpeg_video_stream_parser_t *)calloc(1, sizeof(livemedia_mpeg_video_stream_parser_t));
	if (mpeg_video_stream_parser == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_mpeg_video_stream_parser_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return mpeg_video_stream_parser;
}
void __livemedia_mpeg_video_stream_parser__init(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser,
		livemedia_mpeg_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	__livemedia_stream_parser__init(&mpeg_video_stream_parser->stream_parser,
			input_source, livemedia_framed_source__handle_closure__static, using_source, 
			&livemedia_mpeg_video_stream_framer__continue_read_processing__static, using_source);

	/* constructor */
	mpeg_video_stream_parser->using_source = using_source;
	
	/* virtual functions configuration */ 

	((livemedia_stream_parser_t *)mpeg_video_stream_parser)->restore_saved_parser_state = 
	livemedia_mpeg_video_stream_parser__restore_saved_parser_state__impl;
	((livemedia_stream_parser_t *)mpeg_video_stream_parser)->delete = 
		livemedia_mpeg_video_stream_parser__delete__impl;
}
void __livemedia_mpeg_video_stream_parser__deinit(livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	__livemedia_stream_parser__deinit(&mpeg_video_stream_parser->stream_parser);
}
void __livemedia_mpeg_video_stream_parser__free(livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
	if (mpeg_video_stream_parser) {
		free(mpeg_video_stream_parser);
		mpeg_video_stream_parser = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_mpeg_video_stream_parser_t *livemedia__new__mpeg_video_stream_parser(
		livemedia_mpeg_video_stream_framer_t *using_source,
		livemedia_framed_source_t *input_source)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser;

	mpeg_video_stream_parser = __livemedia_mpeg_video_stream_parser__alloc();
	if (mpeg_video_stream_parser)
		__livemedia_mpeg_video_stream_parser__init(mpeg_video_stream_parser,
				using_source, input_source);

	return mpeg_video_stream_parser;
}

/*
 * Functions for virtual functions
 */
unsigned int livemedia_mpeg_video_stream_parser__parse(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
	if (mpeg_video_stream_parser && mpeg_video_stream_parser->parse) 
		return mpeg_video_stream_parser->parse(mpeg_video_stream_parser);
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return 0;
}

/*
 * Override functions
 */
void livemedia_mpeg_video_stream_parser__restore_saved_parser_state__impl(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser;
	mpeg_video_stream_parser =
		(livemedia_mpeg_video_stream_parser_t *)stream_parser;

	livemedia_stream_parser__restore_saved_parser_state__impl(stream_parser);
	mpeg_video_stream_parser->to = mpeg_video_stream_parser->saved_to;
	mpeg_video_stream_parser->num_truncated_bytes =
		mpeg_video_stream_parser->saved_num_truncated_bytes;
}

void livemedia_mpeg_video_stream_parser__delete__impl(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser;

	mpeg_video_stream_parser = (livemedia_mpeg_video_stream_parser_t *)stream_parser;
	if (mpeg_video_stream_parser) {
		__livemedia_mpeg_video_stream_parser__deinit(mpeg_video_stream_parser);
		__livemedia_mpeg_video_stream_parser__free(mpeg_video_stream_parser);
	}
}

/*
 * Functions in header file
 */
unsigned int livemedia_mpeg_video_stream_parser__num_truncated_bytes(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return mpeg_video_stream_parser->num_truncated_bytes;
}

void livemedia_mpeg_video_stream_parser__set_parse_state(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	mpeg_video_stream_parser->saved_to = mpeg_video_stream_parser->to;
	mpeg_video_stream_parser->saved_num_truncated_bytes =
		mpeg_video_stream_parser->num_truncated_bytes;
	livemedia_stream_parser__save_parser_state(
			(livemedia_stream_parser_t *)mpeg_video_stream_parser);
}

/* Record "byte" in the current output frame: */
void livemedia_mpeg_video_stream_parser__save_byte(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int8_t byte)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (mpeg_video_stream_parser->to >= mpeg_video_stream_parser->limit) {
		/* there's no space left */
		++mpeg_video_stream_parser->num_truncated_bytes;
		return;
	}

	*mpeg_video_stream_parser->to++ = byte;
}

void livemedia_mpeg_video_stream_parser__save4_bytes(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int32_t word)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (mpeg_video_stream_parser->to + 4 > mpeg_video_stream_parser->limit) {
		/* there's no space left */
		mpeg_video_stream_parser->num_truncated_bytes += 4;
		return;
	}

	*mpeg_video_stream_parser->to++ = word >> 24;
	*mpeg_video_stream_parser->to++ = word >> 16;
	*mpeg_video_stream_parser->to++ = word >> 8;
	*mpeg_video_stream_parser->to++ = word;
}

/* Save data until we see a sync word (0x000001xx): */
void livemedia_mpeg_video_stream_parser__save_to_next_code(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int32_t *cur_word)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser_t *stream_parser;
	stream_parser = (livemedia_stream_parser_t *)mpeg_video_stream_parser;
	unsigned char new_byte;

	livemedia_mpeg_video_stream_parser__save_byte(mpeg_video_stream_parser, (*cur_word)>>24);
	*cur_word = ((*cur_word) << 8) | livemedia_stream_parser__get1_byte(stream_parser);
	while (((*cur_word)&0xFFFFFF00) != 0x00000100) {
		if ((unsigned int)((*cur_word)&0xFF) > 1) {
			/* a sync word definitely doesn't begin anywhere in "curWord" */
			*cur_word = livemedia_stream_parser__get4_bytes(stream_parser);
		} else {
			/* a sync word might begin in "curWord", although not at its start */
			new_byte = livemedia_stream_parser__get1_byte(stream_parser);
			*cur_word = ((*cur_word) << 8) | new_byte;
		}
	}
}

/* Skip data until we see a sync word (0x000001xx): */
void livemedia_mpeg_video_stream_parser__skip_to_next_code(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser, u_int32_t *cur_word)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser_t *stream_parser;
	stream_parser = (livemedia_stream_parser_t *)mpeg_video_stream_parser;

	unsigned char new_byte;

	*cur_word = ((*cur_word) << 8) | livemedia_stream_parser__get1_byte(stream_parser);
	while (((*cur_word)&0xFFFFFF00) != 0x00000100) {
		if ((unsigned int)((*cur_word)&0xFF) > 1) {
			/* a sync word definitely doesn't begin anywhere in "curWord" */
			*cur_word = livemedia_stream_parser__get4_bytes(stream_parser);
		} else {
			/* a sync word might begin in "curWord", although not at its start */
			new_byte = livemedia_stream_parser__get1_byte(stream_parser);
			*cur_word = ((*cur_word) << 8) | new_byte;
		}
	}
}


unsigned int livemedia_mpeg_video_stream_parser__cur_frame_size(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return mpeg_video_stream_parser->to - mpeg_video_stream_parser->start_of_frame;
}


/*
 * Normal functions
 */
void livemedia_mpeg_video_stream_parser__register_read_interest(
		livemedia_mpeg_video_stream_parser_t *mpeg_video_stream_parser,
		unsigned char *to, unsigned int max_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	mpeg_video_stream_parser->start_of_frame = mpeg_video_stream_parser->to
		= mpeg_video_stream_parser->saved_to = to;
	mpeg_video_stream_parser->limit = to + max_size;
	mpeg_video_stream_parser->num_truncated_bytes
		= mpeg_video_stream_parser->saved_num_truncated_bytes = 0;
}

/*
 * Static functions implementation for ancestor
 */
