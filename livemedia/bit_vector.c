#include "bit_vector.h"
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************
 * BitVector (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_bit_vector_t *__livemedia_bit_vector__alloc(void)
{
	livemedia_bit_vector_t *bit_vector;
	bit_vector = (livemedia_bit_vector_t *)calloc(1, sizeof(livemedia_bit_vector_t));
	if (bit_vector == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_bit_vector_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return bit_vector;
}
void __livemedia_bit_vector__init(livemedia_bit_vector_t *bit_vector,
		unsigned char *base_byte_ptr, unsigned base_bit_offset,
		unsigned int tot_num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* constructor */
	livemedia_bit_vector__setup(bit_vector, base_byte_ptr, base_bit_offset, tot_num_bits);
	
	/* virtual functions configuration */ 
	bit_vector->delete = livemedia_bit_vector__delete__impl;
}
void __livemedia_bit_vector__deinit(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
}
void __livemedia_bit_vector__free(livemedia_bit_vector_t *bit_vector)
{
	if (bit_vector) {
		free(bit_vector);
		bit_vector = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_bit_vector_t *livemedia__new__bit_vector(
		unsigned char *base_byte_ptr, unsigned int base_bit_offset,
		unsigned int tot_num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_bit_vector_t *bit_vector;

	bit_vector = __livemedia_bit_vector__alloc();
	if (bit_vector)
		__livemedia_bit_vector__init(bit_vector, base_byte_ptr, base_bit_offset,
				tot_num_bits);

	return bit_vector;
}
void livemedia__delete__bit_vector(livemedia_bit_vector_t *bit_vector)
{
	if (bit_vector && bit_vector->delete)
		bit_vector->delete(bit_vector);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_bit_vector__delete__impl(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (bit_vector) {
		__livemedia_bit_vector__deinit(bit_vector);
		__livemedia_bit_vector__free(bit_vector);
	}
}

/*
 * Functions in header file
 */
bool livemedia_bit_vector__get1_bit_boolean(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return livemedia_bit_vector__get1_bit(bit_vector) != 0;

}

bool livemedia_bit_vector__cur_bit_index(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return bit_vector->cur_bit_index;
}

bool livemedia_bit_vector__tot_num_bits(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return bit_vector->tot_num_bits;
}

bool livemedia_bit_vector__num_bits_remaining(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return bit_vector->tot_num_bits - bit_vector->cur_bit_index;
}


/*
 * Normal functions
 */
void livemedia_bit_vector__setup(livemedia_bit_vector_t *bit_vector,
		unsigned char *base_byte_ptr, unsigned int base_bit_offset,
		unsigned int tot_num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

	bit_vector->base_byte_ptr = base_byte_ptr;
	bit_vector->base_bit_offset = base_bit_offset;
	bit_vector->tot_num_bits = tot_num_bits;
	bit_vector->cur_bit_index = 0;
}

static unsigned char const single_bit_mask[8]
    = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

#define MAX_LENGTH 32

void livemedia_bit_vector__put_bits(livemedia_bit_vector_t *bit_vector,
		unsigned int from, unsigned int num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char tmp_buf[4];
	unsigned int overflowing_bits;

	if (num_bits == 0)
		return 0;

	overflowing_bits = 0;

	if (num_bits > MAX_LENGTH) {
		num_bits = MAX_LENGTH;
	}

	if (num_bits > bit_vector->tot_num_bits - bit_vector->cur_bit_index) {
		overflowing_bits = 
			num_bits - (bit_vector->tot_num_bits - bit_vector->cur_bit_index);
	}

	tmp_buf[0] = (unsigned char)(from>>24);
	tmp_buf[1] = (unsigned char)(from>>16);
	tmp_buf[2] = (unsigned char)(from>>8);
	tmp_buf[3] = (unsigned char)(from);

	livemedia__shift_bits(bit_vector->base_byte_ptr, 
			bit_vector->base_bit_offset + bit_vector->cur_bit_index, /* to */
			tmp_buf, MAX_LENGTH - num_bits, /* from */
			num_bits - overflowing_bits /* num bits */);
	bit_vector->cur_bit_index += num_bits - overflowing_bits;
}

void livemedia_bit_vector__put1_bit(livemedia_bit_vector_t *bit_vector,
		unsigned int bit)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* The following is equivalent to "putBits(..., 1)", except faster: */
	unsigned int tot_bit_offset;
	unsigned char mask;

	if (bit_vector->cur_bit_index >= bit_vector->tot_num_bits) { /*overflow */
		return;
	} else {
		tot_bit_offset = bit_vector->base_bit_offset + bit_vector->cur_bit_index++;
		mask = single_bit_mask[tot_bit_offset % 8];
		if (bit) {
			bit_vector->base_byte_ptr[tot_bit_offset/8] |= mask;
		} else {
			bit_vector->base_byte_ptr[tot_bit_offset/8] &=~ mask;
		}
	}
}


unsigned int livemedia_bit_vector__get_bits(livemedia_bit_vector_t *bit_vector,
		unsigned int num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char tmp_buf[4];
	unsigned int overflowing_bits;
	unsigned int result;
	if (num_bits == 0)
		return 0;

	overflowing_bits = 0;
	if (num_bits > MAX_LENGTH) {
		num_bits = MAX_LENGTH;
	}

	if (num_bits > bit_vector->tot_num_bits - bit_vector->cur_bit_index) {
		overflowing_bits = 
			num_bits - (bit_vector->tot_num_bits - bit_vector->cur_bit_index);
	}

	livemedia__shift_bits(tmp_buf, 0, /* to */
			bit_vector->base_byte_ptr, 
			bit_vector->base_bit_offset + bit_vector->cur_bit_index, /* from */
			num_bits - overflowing_bits /* num bits */);
	bit_vector->cur_bit_index += num_bits - overflowing_bits;

	result = (tmp_buf[0] << 24) | (tmp_buf[1] << 16) |
		(tmp_buf[2] << 8) | (tmp_buf[3]);
	result >>= (MAX_LENGTH - num_bits); /* move into low-order part of word */
	result &= (0xFFFFFFFF << overflowing_bits); /* so any overflow bits are 0 */
	return result;
}
/* "numBits" <= 32 */
unsigned int livemedia_bit_vector__get1_bit(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* The following is equivalent to "getBits(1)", except faster: */
	unsigned int tot_bit_offset;
	unsigned char cur_from_byte;
	unsigned int result;

	if (bit_vector->cur_bit_index >= bit_vector->tot_num_bits) { /* overflow */
		return 0;
	} else {
		tot_bit_offset = bit_vector->base_bit_offset + bit_vector->cur_bit_index++;
		cur_from_byte = bit_vector->base_byte_ptr[tot_bit_offset / 8];
		result = (cur_from_byte >> (7 - (tot_bit_offset % 8))) & 0x01;
		return result;
	}
}


void livemedia_bit_vector__skip_bits(livemedia_bit_vector_t *bit_vector,
		unsigned int num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (num_bits > bit_vector->tot_num_bits - bit_vector->cur_bit_index) { /* overflow */
		bit_vector->cur_bit_index = bit_vector->tot_num_bits;
	} else {
		bit_vector->cur_bit_index += num_bits;
	}
}


unsigned livemedia_bit_vector__get_exp_golomb(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int num_leading_zero_bits;
	unsigned int code_start;

	num_leading_zero_bits = 0;
	code_start = 1;

	while (livemedia_bit_vector__get1_bit(bit_vector) == 0 &&
			bit_vector->cur_bit_index < bit_vector->tot_num_bits) {
		++num_leading_zero_bits;
		code_start *= 2;
	}

	return code_start - 1 + livemedia_bit_vector__get_bits(bit_vector, num_leading_zero_bits);
}

int livemedia_bit_vector__get_exp_golomb_signed(livemedia_bit_vector_t *bit_vector)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int code_num;

	code_num = livemedia_bit_vector__get_exp_golomb(bit_vector);
	
	if ((code_num & 1) == 0) { /* even */
		return -(int)(code_num / 2);
	} else { /* odd */
		return (code_num + 1) / 2;
	}
}


void livemedia__shift_bits(unsigned char *to_base_ptr, unsigned int to_bit_offset,
		unsigned char const *from_base_ptr, unsigned int from_bit_offset,
		unsigned int num_bits)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned char *from_byte_ptr;
	unsigned int from_bit_rem;
	unsigned char *to_byte_ptr;
	unsigned int to_bit_rem;
	unsigned char from_bit_mask;
	unsigned char from_bit;
	unsigned char to_bit_mask;

	if (num_bits == 0)
		return;

	from_byte_ptr = from_base_ptr + from_bit_offset / 8;
	from_bit_rem = from_bit_offset % 8;
	to_byte_ptr = to_base_ptr + to_bit_offset / 8;
	to_bit_rem = to_bit_offset % 8;

	while (num_bits-- > 0) {
		from_bit_mask = single_bit_mask[from_bit_rem];
		from_bit = (*from_byte_ptr)&from_bit_mask;
		to_bit_mask = single_bit_mask[to_bit_rem];

		if (from_bit != 0) {
			*to_byte_ptr |= to_bit_mask;
		} else {
			*to_byte_ptr &=~ to_bit_mask;
		}
		if (++from_bit_rem == 8) {
			++from_byte_ptr;
			from_bit_rem = 0;
		}
		if (++to_bit_rem == 8) {
			++to_byte_ptr;
			to_bit_rem = 0;
		}
	}
}


/*
 * Static functions implementation for ancestor
 */
