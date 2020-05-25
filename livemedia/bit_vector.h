#ifndef __LIVEMEDIA_BIT_VECTOR_H__
#define __LIVEMEDIA_BIT_VECTOR_H__

#include <stdbool.h>

/*****************************************************************
 * BitVector (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_bit_vector_t {
	unsigned char *base_byte_ptr;
	unsigned int base_bit_offset;
	unsigned int tot_num_bits;
	unsigned int cur_bit_index;

	void (*delete)(struct _livemedia_bit_vector_t *bit_vector);
} livemedia_bit_vector_t;

/*****************************************************************
 * BitVector (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_bit_vector_t *__livemedia_bit_vector__alloc(void);
void __livemedia_bit_vector__init(livemedia_bit_vector_t *bit_vector,
		unsigned char *base_byte_ptr, unsigned int base_bit_offset,
		unsigned int tot_num_bits);
void __livemedia_bit_vector__deinit(livemedia_bit_vector_t *bit_vector);
void __livemedia_bit_vector__free(livemedia_bit_vector_t *bit_vector);
/*
 * new() and delete() functions
 */
livemedia_bit_vector_t *livemedia__new__bit_vector(
		unsigned char *base_byte_ptr, unsigned int base_bit_offset,
		unsigned int tot_num_bits);
void livemedia__delete__bit_vector(livemedia_bit_vector_t *bit_vector);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_bit_vector__delete__impl(livemedia_bit_vector_t *bit_vector);

/*
 * Functions in header file
 */
bool livemedia_bit_vector__get1_bit_boolean(livemedia_bit_vector_t *bit_vector);
bool livemedia_bit_vector__cur_bit_index(livemedia_bit_vector_t *bit_vector);
bool livemedia_bit_vector__tot_num_bits(livemedia_bit_vector_t *bit_vector);
bool livemedia_bit_vector__num_bits_remaining(livemedia_bit_vector_t *bit_vector);
/*
 * Normal functions
 */
void livemedia_bit_vector__setup(livemedia_bit_vector_t *bit_vector,
		unsigned char *base_byte_ptr, unsigned int base_bit_offset,
		unsigned int tot_num_bits);

void livemedia_bit_vector__put_bits(livemedia_bit_vector_t *bit_vector,
		unsigned int from, unsigned int num_bits);
void livemedia_bit_vector__put1_bit(livemedia_bit_vector_t *bit_vector,
		unsigned int bit);

unsigned int livemedia_bit_vector__get_bits(livemedia_bit_vector_t *bit_vector,
		unsigned int num_bits); /* "numBits" <= 32 */
unsigned int livemedia_bit_vector__get1_bit(livemedia_bit_vector_t *bit_vector);

void livemedia_bit_vector__skip_bits(livemedia_bit_vector_t *bit_vector,
		unsigned int num_bits);

unsigned livemedia_bit_vector__get_exp_golomb(livemedia_bit_vector_t *bit_vector);
/* Returns the value of the next bits, assuming that they were encoded using an
 * exponential-Golomb code of order 0 */
int livemedia_bit_vector__get_exp_golomb_signed(livemedia_bit_vector_t *bit_vector);
/* signed version of the above */

void livemedia__shift_bits(unsigned char *to_base_ptr, unsigned int to_bit_offset,
		unsigned char const *from_base_ptr, unsigned int from_bit_offset,
		unsigned int num_bits);

#endif
