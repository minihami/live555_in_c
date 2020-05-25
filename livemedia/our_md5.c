#include "our_md5.h"
#include "../groupsock/net_common.h" // for u_int32_t, u_int64_t
#include <string.h>
#include <stdio.h>

#define DIGEST_SIZE_IN_BYTES 16
#define DIGEST_SIZE_IN_HEX_DIGITS (2*DIGEST_SIZE_IN_BYTES)
#define DIGEST_SIZE_AS_STRING (DIGEST_SIZE_IN_HEX_DIGITS+1)


/*****************************************************************
 * MD5Context (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_md5_context_t {
	u_int32_t state[4]; /* ABCD */
	u_int64_t bit_count; /*number of bits, modulo 2^64 */
	unsigned char working_buffer[64];

	void (*delete)(struct _livemedia_md5_context_t *md5_context);
} livemedia_md5_context_t;

/*****************************************************************
 * MD5Context (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_md5_context_t *__livemedia_md5_context__alloc(void);
void __livemedia_md5_context__init(livemedia_md5_context_t *md5_context);
void __livemedia_md5_context__deinit(livemedia_md5_context_t *md5_context);
void __livemedia_md5_context__free(livemedia_md5_context_t *md5_context);
/*
 * new() and delete() functions
 */
livemedia_md5_context_t *livemedia__new__md5_context(void);
void livemedia__delete__md5_context(livemedia_md5_context_t *md5_context);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_md5_context__delete__impl(livemedia_md5_context_t *md5_context);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_md5_context__add_data(livemedia_md5_context_t *md5_context,
		unsigned char const *input_data, unsigned int input_data_size);

void livemedia_md5_context__end(livemedia_md5_context_t *md5_context,
		char *output_digest /*must point to an array of size DIGEST_SIZE_AS_STRING */);
void livemedia_md5_context__finalize(livemedia_md5_context_t *md5_context,
		unsigned char * output_digest_in_bytes);
/* Like "end()", except that the argument is a byte array, of size DIGEST_SIZE_IN_BYTES.
 *This function is used to implement "end()". */

void livemedia_md5_context__zeroize(livemedia_md5_context_t *md5_context);
/* to remove potentially sensitive information */
void livemedia_md5_context__transform64_bytes(livemedia_md5_context_t *md5_context,
		unsigned char const block[64]); /* does the actual MD5 transform */



char *livemedia__our_md5_data(unsigned char const *data, unsigned int data_size,
		char *output_digest)
{
	livemedia_md5_context_t ctx;
	
	livemedia_md5_context__add_data(&ctx, data, data_size);

	if (output_digest == NULL) {
		output_digest =
			(char *)malloc(sizeof(char) * DIGEST_SIZE_IN_BYTES);
	}
	livemedia_md5_context__end(&ctx, output_digest);

	return output_digest;
}
unsigned char *livemedia__our_md5_data_raw(unsigned char const *data, unsigned int data_size,
		unsigned char *output_digest)
{
	livemedia_md5_context_t ctx;
	
	livemedia_md5_context__add_data(&ctx, data, data_size);

	if (output_digest == NULL) {
		output_digest =
			(unsigned char *)malloc(sizeof(unsigned char) * DIGEST_SIZE_IN_BYTES);
	}
	livemedia_md5_context__finalize(&ctx, output_digest);

	return output_digest;
}

/*****************************************************************
 * MD5Context (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_md5_context_t *__livemedia_md5_context__alloc(void)
{
	livemedia_md5_context_t *md5_context;
	md5_context = (livemedia_md5_context_t *)calloc(1, sizeof(livemedia_md5_context_t));
	if (md5_context == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_md5_context_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return md5_context;
}
void __livemedia_md5_context__init(livemedia_md5_context_t *md5_context)
{
	md5_context->bit_count = 0;
	md5_context->state[0] = 0x67452301;
	md5_context->state[1] = 0xefcdab89;
	md5_context->state[2] = 0x98badcfe;
	md5_context->state[3] = 0x10325476;
	
	md5_context->delete = livemedia_md5_context__delete__impl;
}
void __livemedia_md5_context__deinit(livemedia_md5_context_t *md5_context)
{
	livemedia_md5_context__zeroize(md5_context);
}
void __livemedia_md5_context__free(livemedia_md5_context_t *md5_context)
{
	if (md5_context) {
		free(md5_context);
		md5_context = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_md5_context_t *livemedia__new__md5_context(void)
{
	livemedia_md5_context_t *md5_context;

	md5_context = __livemedia_md5_context__alloc();
	if (md5_context)
		__livemedia_md5_context__init(md5_context);

	return md5_context;
}
void livemedia__delete__md5_context(livemedia_md5_context_t *md5_context)
{
	if (md5_context && md5_context->delete)
		md5_context->delete(md5_context);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_md5_context__delete__impl(livemedia_md5_context_t *md5_context)
{
	/*TODO: if child, get parent as parameter and cast to child */
	if (md5_context) {
		__livemedia_md5_context__deinit(md5_context);
		__livemedia_md5_context__free(md5_context);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
void livemedia_md5_context__add_data(livemedia_md5_context_t *md5_context,
		unsigned char const *input_data, unsigned int input_data_size)
{
  /* Begin by noting how much of our 64-byte working buffer remains unfilled: */
	u_int64_t byte_count;
	unsigned int buffer_bytes_in_use;
	unsigned int buffer_bytes_remaining;
	unsigned int i;

	byte_count = md5_context->bit_count>>3;
	buffer_bytes_in_use = (unsigned int)(byte_count & 0x3F);
	buffer_bytes_remaining = 64 - buffer_bytes_in_use;

	/* Then update our bit count: */
	md5_context->bit_count += input_data_size<<3;

	i = 0;
	if (input_data_size >= buffer_bytes_remaining) {
		/* We have enough input data to do (64-byte) MD5 transforms.
		 * Do this now, starting with a transform on our working buffer, then with
		 * (as many as possible) transforms on rest of the input data. */

		memcpy((unsigned char *)&md5_context->working_buffer[buffer_bytes_in_use],
				(unsigned char *)input_data, buffer_bytes_remaining);
		livemedia_md5_context__transform64_bytes(md5_context, md5_context->working_buffer);
		buffer_bytes_in_use = 0;

		for (i = buffer_bytes_remaining; i + 63 < input_data_size; i += 64) 
			livemedia_md5_context__transform64_bytes(md5_context, &input_data[i]);

	}

	/* Copy any remaining (and currently un-transformed) input data into our working buffer: */
	if (i < input_data_size) {
		memcpy((unsigned char *)&md5_context->working_buffer[buffer_bytes_in_use],
				(unsigned char *)&input_data[i], input_data_size - i);
	}
}

void livemedia_md5_context__end(livemedia_md5_context_t *md5_context,
		char *output_digest)
{
	unsigned char digest_in_bytes[DIGEST_SIZE_IN_BYTES];
	static char const *hex;
	unsigned int i;

	livemedia_md5_context__finalize(md5_context, digest_in_bytes);

	/* Convert the digest from bytes (binary) to hex digits: */
	hex = "0123456789abcdef";
	for (i = 0; i < DIGEST_SIZE_IN_BYTES; ++i) {
		output_digest[2 * i] = hex[digest_in_bytes[i] >> 4];
		output_digest[2 * i + 1] = hex[digest_in_bytes[i] & 0x0F];
	}
	output_digest[2 * i] = '\0';
}

/* Routines that unpack 32 and 64-bit values into arrays of bytes (in little-endian order).
/ (These are used to implement "finalize()".) */

static void unpack32(unsigned char out[4], u_int32_t in) {
	unsigned int i;
	for (i = 0; i < 4; ++i) {
		out[i] = (unsigned char)((in>>(8*i))&0xFF);
	}
}

static void unpack64(unsigned char out[8], u_int64_t in) {
	unsigned int i;
	for (i = 0; i < 8; ++i) {
		out[i] = (unsigned char)((in>>(8*i))&0xFF);
	}
}

static unsigned char const PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
void livemedia_md5_context__finalize(livemedia_md5_context_t *md5_context,
		unsigned char * output_digest_in_bytes)
{
	unsigned char bit_count_in_bytes[8];
	unsigned int buffer_bytes_in_use;
	unsigned int num_padding_bytes;

	/* Unpack out bit count: */
	unpack64(bit_count_in_bytes, md5_context->bit_count);

	/* Before 'finalizing', make sure that we transform any remaining bytes in our working buffer: */
	u_int64_t const byte_count = md5_context->bit_count >> 3;
	buffer_bytes_in_use = (unsigned int)(byte_count & 0x3F);
	num_padding_bytes = (buffer_bytes_in_use < 56) ? (56 - buffer_bytes_in_use) :
		(64 + 56 - buffer_bytes_in_use);
	livemedia_md5_context__add_data(md5_context, PADDING, num_padding_bytes);

	livemedia_md5_context__add_data(md5_context, bit_count_in_bytes, 8);

	/* Unpack our 'state' into the output digest: */
	unpack32(&output_digest_in_bytes[0], md5_context->state[0]);
	unpack32(&output_digest_in_bytes[4], md5_context->state[1]);
	unpack32(&output_digest_in_bytes[8], md5_context->state[2]);
	unpack32(&output_digest_in_bytes[12], md5_context->state[3]);

	livemedia_md5_context__zeroize(md5_context);
}

void livemedia_md5_context__zeroize(livemedia_md5_context_t *md5_context)
{
	unsigned int i;

	md5_context->state[0] = md5_context->state[1] = md5_context->state[2] =
		md5_context->state[3] = 0;
	md5_context->bit_count = 0;
	for (i = 0; i < 64; ++i)
		md5_context->working_buffer[i] = 0;
}

/* Constants for the transform: */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* Basic MD5 functions: */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* Rotate "x" left "n" bits: */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* Other transforms: */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT((a), (s)); \
 (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT((a), (s)); \
 (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT((a), (s)); \
 (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
 (a) += I((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT((a), (s)); \
 (a) += (b); \
}
void livemedia_md5_context__transform64_bytes(livemedia_md5_context_t *md5_context,
		unsigned char const block[64])
{

}

/*
 * Static functions implementation for ancestor
 */
