
#ifndef __LIVEMEDIA_STREAM_PARSER_H__
#define __LIVEMEDIA_STREAM_PARSER_H__

#ifndef __LIVEMEDIA_FRAMED_SOURCE_H__
#include "framed_source.h"
#endif
#include <setjmp.h>

jmp_buf jmp_buf_e;

/*****************************************************************
 * StreamParser (Structure Declarations)
 ****************************************************************/
typedef void (livemedia__client_continue_func)(void *client_data, unsigned char *ptr,
		unsigned int size, struct timeval presentation_time);

typedef struct _livemedia_stream_parser_t {
	livemedia_framed_source_t *input_source; /* sould be a byte-stream source?? */
	livemedia_framed_source__on_close_func *client_on_input_close_func;
	void *client_on_input_close_client_data;
	livemedia__client_continue_func *client_continue_func;
	void *client_continue_client_data;

	/* Use a pair of 'banks', and swap between them as they fill up: */
	unsigned char *bank[2];
	unsigned char cur_bank_num;
	unsigned char *cur_bank;

	/* The most recent 'saved' parse position: */
	unsigned int saved_parser_index; /* <= fCurParserIndex */
	unsigned char saved_remaining_unparsed_bits;

	/* The current position of the parser within the current bank: */
	unsigned int cur_parser_index; /* <= fTotNumValidBytes */
	unsigned char remaining_unparsed_bits; /* in previous byte: [0,7] */

	/* The total number of nalid bytes stored in the current bank: */
	unsigned int tot_num_valid_bytes; /* <= BANK_SIZE */

	/* Whether we have seen EOF on the input source: */
	bool have_seen_eof;

	struct timeval last_seen_presentation_time; /* hack used for EOF handling */

	void (*flush_input)(struct _livemedia_stream_parser_t *stream_parser);
	void (*restore_saved_parser_state)(struct _livemedia_stream_parser_t *stream_parser);
	void (*delete)(struct _livemedia_stream_parser_t *stream_parser);
} livemedia_stream_parser_t;

/*****************************************************************
 * StreamParser (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_stream_parser_t *__livemedia_stream_parser__alloc(void);
void __livemedia_stream_parser__init(livemedia_stream_parser_t *stream_parser,
		livemedia_framed_source_t *input_source,
		livemedia_framed_source__on_close_func *on_input_close_func,
		void *on_input_close_client_data,
		livemedia__client_continue_func *client_continue_func,
		void *client_continue_client_data);
void __livemedia_stream_parser__deinit(livemedia_stream_parser_t *stream_parser);
void __livemedia_stream_parser__free(livemedia_stream_parser_t *stream_parser);
/*
 * new() and delete() functions
 */
livemedia_stream_parser_t *livemedia__new__stream_parser(
		livemedia_framed_source_t *input_source,
		livemedia_framed_source__on_close_func *on_input_close_func,
		void *on_input_close_client_data,
		livemedia__client_continue_func *client_continue_func,
		void *client_continue_client_data);
void livemedia__delete__stream_parser(livemedia_stream_parser_t *stream_parser);
/*
 * Functions for virtual functions
 */
void livemedia_stream_parser__flush_input(livemedia_stream_parser_t *stream_parser);
void livemedia_stream_parser__restore_saved_parser_state(
		livemedia_stream_parser_t *stream_parser);

/*
 * Override functions
 */
void livemedia_stream_parser__flush_input__impl(livemedia_stream_parser_t *stream_parser);
void livemedia_stream_parser__restore_saved_parser_state__impl(
		livemedia_stream_parser_t *stream_parser);
void livemedia_stream_parser__delete__impl(livemedia_stream_parser_t *stream_parser);

/*
 * Functions in header file
 */
u_int32_t livemedia_stream_parser__get4_bytes(livemedia_stream_parser_t *stream_parser);
u_int32_t livemedia_stream_parser__test4_bytes(livemedia_stream_parser_t *stream_parser);
u_int16_t livemedia_stream_parser__get2_bytes(livemedia_stream_parser_t *stream_parser);
u_int16_t livemedia_stream_parser__test2_bytes(livemedia_stream_parser_t *stream_parser);
u_int8_t livemedia_stream_parser__get1_byte(livemedia_stream_parser_t *stream_parser);
u_int8_t livemedia_stream_parser__test1_byte(livemedia_stream_parser_t *stream_parser);
void livemedia_stream_parser__get_bytes(livemedia_stream_parser_t *stream_parser,
		u_int8_t *to, unsigned int num_bytes);
void livemedia_stream_parser__test_bytes(livemedia_stream_parser_t *stream_parser,
		u_int8_t *to, unsigned int num_bytes); /* as above, but doesn't advance ptr */
void livemedia_stream_parser__skip_bytes(livemedia_stream_parser_t *stream_parser,
		unsigned int num_bytes);
unsigned int livemedia_stream_parser__cur_offset(livemedia_stream_parser_t *stream_parser);
unsigned int *livemedia_stream_parser__tot_num_valid_bytes(
		livemedia_stream_parser_t *stream_parser);
bool livemedia_stream_parser__have_seen_eof(livemedia_stream_parser_t *stream_parser);
unsigned char *livemedia_stream_parser__cur_bank(
		livemedia_stream_parser_t *stream_parser);
unsigned char *livemedia_stream_parser__next_to_parse(
		livemedia_stream_parser_t *stream_parser);
unsigned char *livemedia_stream_parser__last_parsed(
		livemedia_stream_parser_t *stream_parser);

/* makes sure that at leat "numBytes" valid bytes remain: */
void livemedia_stream_parser__ensure_valid_bytes(
		livemedia_stream_parser_t *stream_parser, unsigned int num_bytes_needed);

/*
 * Normal functions
 */
void livemedia_stream_parser__save_parser_state(livemedia_stream_parser_t *stream_parser);
void livemedia_stream_parser__skip_bits(livemedia_stream_parser_t *stream_parser, unsigned int num_bits);
unsigned int livemedia_stream_parser__get_bits(livemedia_stream_parser_t *stream_parser, unsigned int num_bits);
unsigned int livemedia_stream_parser__bank_size(livemedia_stream_parser_t *stream_parser);
void livemedia_stream_parser__ensure_valid_bytes1(
		livemedia_stream_parser_t *stream_parser, unsigned int num_bytes_needed);
void livemedia_stream_parser__after_getting_bytes__static(void *client_data,
		unsigned int num_bytes_read, unsigned int num_truncated_bytes,
		struct timeval presentation_time, unsigned int duration_in_microseconds);
void livemedia_stream_parser__after_getting_bytes1(
		livemedia_stream_parser_t *stream_parser, unsigned int num_bytes_read,
		struct timeval presentation_time);
void livemedia_stream_parser__on_input_closure__static(void *client_data);
void livemedia_stream_parser__on_input_closure1(livemedia_stream_parser_t *stream_parser);

#endif
