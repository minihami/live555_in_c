#include "stream_parser.h"

#include <string.h>
#include <stdlib.h>

#define BANK_SIZE 150000

/*****************************************************************
 * StreamParser (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_stream_parser_t *__livemedia_stream_parser__alloc(void)
{
	livemedia_stream_parser_t *stream_parser;
	stream_parser = (livemedia_stream_parser_t *)calloc(1, sizeof(livemedia_stream_parser_t));
	if (stream_parser == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_stream_parser_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return stream_parser;
}
void __livemedia_stream_parser__init(livemedia_stream_parser_t *stream_parser,
		livemedia_framed_source_t *input_source,
		livemedia_framed_source__on_close_func *on_input_close_func,
		void *on_input_close_client_data,
		livemedia__client_continue_func *client_continue_func,
		void *client_continue_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* constructor */
	stream_parser->input_source = input_source;
	stream_parser->client_on_input_close_func = on_input_close_func;
	stream_parser->client_on_input_close_client_data = on_input_close_client_data;
	stream_parser->client_continue_func = client_continue_func;
	stream_parser->client_continue_client_data = client_continue_client_data;
	stream_parser->saved_parser_index = 0;
	stream_parser->saved_remaining_unparsed_bits = 0;
	stream_parser->cur_parser_index = 0;
	stream_parser->remaining_unparsed_bits = 0;
	stream_parser->tot_num_valid_bytes = 0;
	stream_parser->have_seen_eof = false;

	stream_parser->bank[0] = (unsigned char *)malloc(sizeof(unsigned char)*BANK_SIZE);
	if (stream_parser->bank[0] == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char)*BANK_SIZE)"
				"failed in %s()\n", __func__);
	}
	stream_parser->bank[1] = (unsigned char *)malloc(sizeof(unsigned char)*BANK_SIZE);
	if (stream_parser->bank[1] == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char)*BANK_SIZE)"
				"failed in %s()\n", __func__);
	}
	stream_parser->cur_bank_num = 0;
	stream_parser->cur_bank = stream_parser->bank[stream_parser->cur_bank_num];

	stream_parser->last_seen_presentation_time.tv_sec = 0;
	stream_parser->last_seen_presentation_time.tv_usec = 0;
	
	/* virtual functions configuration */ 
	stream_parser->flush_input = livemedia_stream_parser__flush_input__impl;
	stream_parser->restore_saved_parser_state = 
		livemedia_stream_parser__restore_saved_parser_state__impl;


	stream_parser->delete = livemedia_stream_parser__delete__impl;
}
void __livemedia_stream_parser__deinit(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int i;
	for(i = 0; i < 2; i++) {
		if(stream_parser->bank[i]) {
			free(stream_parser->bank[i]);
			stream_parser->bank[i] = NULL;
		}
	}
}
void __livemedia_stream_parser__free(livemedia_stream_parser_t *stream_parser)
{
	if (stream_parser) {
		free(stream_parser);
		stream_parser = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_stream_parser_t *livemedia__new__stream_parser(
		livemedia_framed_source_t *input_source,
		livemedia_framed_source__on_close_func *on_input_close_func,
		void *on_input_close_client_data,
		livemedia__client_continue_func *client_continue_func,
		void *client_continue_client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser_t *stream_parser;

	stream_parser = __livemedia_stream_parser__alloc();
	if (stream_parser)
		__livemedia_stream_parser__init(stream_parser, input_source, on_input_close_func,
				on_input_close_client_data, client_continue_func,
				client_continue_client_data);

	return stream_parser;
}
void livemedia__delete__stream_parser(livemedia_stream_parser_t *stream_parser)
{
	if (stream_parser && stream_parser->delete)
		stream_parser->delete(stream_parser);
}
/*
 * Functions for virtual functions
 */
void livemedia_stream_parser__flush_input(livemedia_stream_parser_t *stream_parser)
{
	if (stream_parser && stream_parser->flush_input) {
		stream_parser->flush_input(stream_parser);
		return;
	}
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_stream_parser__restore_saved_parser_state(
		livemedia_stream_parser_t *stream_parser)
{
	if (stream_parser && stream_parser->restore_saved_parser_state) {
		stream_parser->restore_saved_parser_state(stream_parser);
		return;
	}
#ifdef WEBGATE_DEBUG
	printf("ERROR: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}


/*
 * Override functions
 */
void livemedia_stream_parser__flush_input__impl(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	stream_parser->cur_parser_index = stream_parser->saved_parser_index = 0;
	stream_parser->saved_remaining_unparsed_bits =
		stream_parser->remaining_unparsed_bits = 0;
	stream_parser->tot_num_valid_bytes = 0;
}

void livemedia_stream_parser__restore_saved_parser_state__impl(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	stream_parser->cur_parser_index = stream_parser->saved_parser_index;
	stream_parser->remaining_unparsed_bits = stream_parser->saved_remaining_unparsed_bits;
}

void livemedia_stream_parser__delete__impl(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (stream_parser) {
		__livemedia_stream_parser__deinit(stream_parser);
		__livemedia_stream_parser__free(stream_parser);
	}
}

/*
 * Functions in header file
 */
u_int32_t livemedia_stream_parser__get4_bytes(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* byte-aligned; returned in big-endian order */
	u_int32_t result;
	result = livemedia_stream_parser__test4_bytes(stream_parser);
	stream_parser->cur_parser_index += 4;
	stream_parser->remaining_unparsed_bits = 0;
	
	return result;
}

u_int32_t livemedia_stream_parser__test4_bytes(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* as above, but doesn't advance ptr */
	unsigned char *ptr;

	livemedia_stream_parser__ensure_valid_bytes(stream_parser, 4);
	ptr = livemedia_stream_parser__next_to_parse(stream_parser);
    return (ptr[0]<<24)|(ptr[1]<<16)|(ptr[2]<<8)|ptr[3];

}

u_int16_t livemedia_stream_parser__get2_bytes(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *ptr;
	u_int16_t result;

	livemedia_stream_parser__ensure_valid_bytes(stream_parser, 2);
	ptr = livemedia_stream_parser__next_to_parse(stream_parser);
    result = (ptr[0]<<8)|ptr[1];
	stream_parser->cur_parser_index += 2;
	stream_parser->remaining_unparsed_bits = 0;

	return result;
}

u_int16_t livemedia_stream_parser__test2_bytes(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *ptr;

	livemedia_stream_parser__ensure_valid_bytes(stream_parser, 2);
	ptr = livemedia_stream_parser__next_to_parse(stream_parser);
    return (ptr[0]<<8)|ptr[1];

}

u_int8_t livemedia_stream_parser__get1_byte(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* byte-aligned */
	unsigned char *cur_bank;

	livemedia_stream_parser__ensure_valid_bytes(stream_parser, 1);
	stream_parser->remaining_unparsed_bits = 0;

	cur_bank = livemedia_stream_parser__cur_bank(stream_parser);
	return cur_bank[stream_parser->cur_parser_index++];
}

u_int8_t livemedia_stream_parser__test1_byte(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* as above, but doesn't advance ptr */
	unsigned char *next_to_parse;

	livemedia_stream_parser__ensure_valid_bytes(stream_parser, 1);
	next_to_parse = livemedia_stream_parser__next_to_parse(stream_parser);
	return next_to_parse[0];
}

void livemedia_stream_parser__get_bytes(livemedia_stream_parser_t *stream_parser,
		u_int8_t *to, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser__test_bytes(stream_parser, to, num_bytes);
	stream_parser->cur_parser_index += num_bytes;
	stream_parser->remaining_unparsed_bits = 0;
}

void livemedia_stream_parser__test_bytes(livemedia_stream_parser_t *stream_parser,
		u_int8_t *to, unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* as above, but doesn't advance ptr */
	livemedia_stream_parser__ensure_valid_bytes(stream_parser, num_bytes);
	memmove(to, livemedia_stream_parser__next_to_parse(stream_parser), num_bytes);
}

void livemedia_stream_parser__skip_bytes(livemedia_stream_parser_t *stream_parser,
		unsigned int num_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser__ensure_valid_bytes(stream_parser, num_bytes);
	stream_parser->cur_parser_index += num_bytes;
}

unsigned int livemedia_stream_parser__cur_offset(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return stream_parser->cur_parser_index;
}

unsigned int *livemedia_stream_parser__tot_num_valid_bytes(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return &stream_parser->tot_num_valid_bytes;
}

bool livemedia_stream_parser__have_seen_eof(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return stream_parser->have_seen_eof;
}

unsigned char *livemedia_stream_parser__cur_bank(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return stream_parser->cur_bank;
}

unsigned char *livemedia_stream_parser__next_to_parse(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *cur_bank;

	cur_bank = livemedia_stream_parser__cur_bank(stream_parser);
	return &cur_bank[stream_parser->cur_parser_index];
}

unsigned char *livemedia_stream_parser__last_parsed(
		livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *cur_bank;

	cur_bank = livemedia_stream_parser__cur_bank(stream_parser);
	return &cur_bank[stream_parser->cur_parser_index - 1];
}


/* makes sure that at leat "numBytes" valid bytes remain: */
void livemedia_stream_parser__ensure_valid_bytes(
		livemedia_stream_parser_t *stream_parser, unsigned int num_bytes_needed)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
    /* common case: inlined: */
	if (stream_parser->cur_parser_index + num_bytes_needed <= stream_parser->tot_num_valid_bytes)
		return;
	livemedia_stream_parser__ensure_valid_bytes1(stream_parser, num_bytes_needed);
}


/*
 * Normal functions
 */
void livemedia_stream_parser__save_parser_state(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	stream_parser->saved_parser_index = stream_parser->cur_parser_index;
	stream_parser->saved_remaining_unparsed_bits = stream_parser->remaining_unparsed_bits;
}

void livemedia_stream_parser__skip_bits(livemedia_stream_parser_t *stream_parser,
		unsigned int num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int num_bytes_to_examine;

	if (num_bits <= stream_parser->remaining_unparsed_bits) {
		stream_parser->remaining_unparsed_bits -= num_bits;
	} else {
		num_bits -= stream_parser->remaining_unparsed_bits;
		num_bytes_to_examine = (num_bits + 7) / 8; /* round up */
		livemedia_stream_parser__ensure_valid_bytes(stream_parser, num_bytes_to_examine);
		stream_parser->remaining_unparsed_bits = 8 * num_bytes_to_examine - num_bits;
	}
}

unsigned int livemedia_stream_parser__get_bits(livemedia_stream_parser_t *stream_parser, unsigned int num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char last_byte;
	unsigned int remaining_bits;
	unsigned int result;
	unsigned int num_remaining_bytes;

	if (num_bits <= stream_parser->remaining_unparsed_bits) {
		last_byte = *livemedia_stream_parser__last_parsed(stream_parser);
		last_byte >>= (stream_parser->remaining_unparsed_bits = num_bits);
		stream_parser->remaining_unparsed_bits -= num_bits;

		return (unsigned int)last_byte &~ ((~0u)<<num_bits);
	} else {
		if (stream_parser->remaining_unparsed_bits > 0) {
			last_byte = *livemedia_stream_parser__last_parsed(stream_parser);
		} else {
			last_byte = 0;
		}

		remaining_bits = num_bits - stream_parser->remaining_unparsed_bits; /* > 0 */

		/* For simplicity, read the next 4 bytes, even though we might not
		 * need all of them here: */

		result = livemedia_stream_parser__test4_bytes(stream_parser);

		result >>= (32 - remaining_bits);
		result |= (last_byte << remaining_bits);
		if (num_bits < 32) result &=~ ((~0u)<<num_bits);

		num_remaining_bytes = (remaining_bits+7)/8;
		stream_parser->cur_parser_index += num_remaining_bytes;
		stream_parser->remaining_unparsed_bits = 8 * num_remaining_bytes - remaining_bits;

		return result;
	}
}

unsigned int livemedia_stream_parser__bank_size(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return BANK_SIZE;
}

#define NO_MORE_BUFFERED_INPUT 1

void livemedia_stream_parser__ensure_valid_bytes1(
		livemedia_stream_parser_t *stream_parser, unsigned int num_bytes_needed)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int max_input_frame_size;
	unsigned int num_bytes_to_save;
	unsigned char *from;
	unsigned int max_num_bytes_to_read;
	unsigned char *cur_bank;

	/* We need to read some more bytes from the input source.
	 * First, clarify how much data to ask for: */
	max_input_frame_size = livemedia_framed_source__max_frame_size(stream_parser->input_source);
	if (max_input_frame_size > num_bytes_needed)
		num_bytes_needed = max_input_frame_size;

	/* First, check whether these new bytes would overflow the current
	 * bank.  If so, start using a new bank now. */
	if (stream_parser->cur_parser_index + num_bytes_needed > BANK_SIZE) {
		/* Swap banks, but save any still-needed bytes from the old bank: */
		num_bytes_to_save = stream_parser->tot_num_valid_bytes - stream_parser->saved_parser_index;
		cur_bank = livemedia_stream_parser__cur_bank(stream_parser);
		from = &cur_bank[stream_parser->saved_parser_index];
		
		stream_parser->cur_bank_num = (stream_parser->cur_bank_num + 1) % 2;
		stream_parser->cur_bank = (stream_parser->bank)[stream_parser->cur_bank_num];
		memmove(livemedia_stream_parser__cur_bank(stream_parser), from, num_bytes_to_save);
		stream_parser->cur_parser_index =
			stream_parser->cur_parser_index - stream_parser->saved_parser_index;
		stream_parser->saved_parser_index = 0;
		stream_parser->tot_num_valid_bytes = num_bytes_to_save;
	}

	/* ASSERT: fCurParserIndex + numBytesNeeded > fTotNumValidBytes
	 *      && fCurParserIndex + numBytesNeeded <= BANK_SIZE */
	if (stream_parser->cur_parser_index + num_bytes_needed > BANK_SIZE) {
		/* If this happens, it means that we have too much saved parser state.
		 * To fix this, increase BANK_SIZE as appropriate. */
		fprintf(stderr, "StreamParser internal error (%u + %u > %d )\n",
				stream_parser->cur_parser_index, num_bytes_needed, BANK_SIZE);
		abort();
	}

	/* Try to read as many new bytes as will fit in the current bank: */
	max_num_bytes_to_read= BANK_SIZE - stream_parser->tot_num_valid_bytes;
	livemedia_framed_source__get_next_frame(stream_parser->input_source,
			&(livemedia_stream_parser__cur_bank(stream_parser))[stream_parser->tot_num_valid_bytes],
			max_num_bytes_to_read, livemedia_stream_parser__after_getting_bytes__static, stream_parser,
			livemedia_stream_parser__on_input_closure__static, stream_parser);

	longjmp(jmp_buf_e, NO_MORE_BUFFERED_INPUT);


}

void livemedia_stream_parser__after_getting_bytes__static(void *client_data,
		unsigned int num_bytes_read, unsigned int num_truncated_bytes,
		struct timeval presentation_time, unsigned int duration_in_microseconds)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser_t *parser;

	parser = (livemedia_stream_parser_t *)client_data;
	if (parser != NULL)
		livemedia_stream_parser__after_getting_bytes1(parser, num_bytes_read, presentation_time);
}

void livemedia_stream_parser__after_getting_bytes1(
		livemedia_stream_parser_t *stream_parser, unsigned int num_bytes_read,
		struct timeval presentation_time)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *ptr;
	unsigned char *cur_bank;

	/* Sanity check: Make sure we didn't get too many bytes for our bank: */
	if (stream_parser->tot_num_valid_bytes + num_bytes_read > BANK_SIZE) {
		fprintf(stderr, "StreamParser::afterGettingBytes() warning: read %d bytes; expected no more than %d\n",
				num_bytes_read, BANK_SIZE - stream_parser->tot_num_valid_bytes);
	}

	stream_parser->last_seen_presentation_time = presentation_time;
	
	cur_bank = livemedia_stream_parser__cur_bank(stream_parser);
	ptr = &cur_bank[stream_parser->tot_num_valid_bytes];
	stream_parser->tot_num_valid_bytes += num_bytes_read;

	/* Continue our original calling source where it left off: */
	livemedia_stream_parser__restore_saved_parser_state(stream_parser);
      /* Sigh... this is a crock; things would have been a lot simpler */
      /* here if we were using threads, with synchronous I/O... */
	stream_parser->client_continue_func(stream_parser->client_continue_client_data, ptr,
			num_bytes_read, presentation_time);
}

void livemedia_stream_parser__on_input_closure__static(void *client_data)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_stream_parser_t *parser;

	parser = (livemedia_stream_parser_t *)client_data;
	if (parser != NULL)
		livemedia_stream_parser__on_input_closure1(parser);

}

void livemedia_stream_parser__on_input_closure1(livemedia_stream_parser_t *stream_parser)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (!stream_parser->have_seen_eof) {
		/* We're hitting EOF for the first time.
		 * Set our 'EOF' flag, and continue parsing, as if we'd just read 0 bytes of data.
		 * This allows the parser to re-parse any remaining unparsed data
		 * (perhaps while testing for EOF at the end): */
		stream_parser->have_seen_eof = true;
		livemedia_stream_parser__after_getting_bytes1(stream_parser, 0,
				stream_parser->last_seen_presentation_time);
	} else {
		/* We're hittin EOF for the second time. Now, we handle the source input closure: */
		stream_parser->have_seen_eof = false;
		if (stream_parser->client_on_input_close_func != NULL)
			stream_parser->client_on_input_close_func(stream_parser->client_on_input_close_client_data);
	}
}


/*
 * Static functions implementation for ancestor
 */
