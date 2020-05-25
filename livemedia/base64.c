#include "base64.h"
#include "str_dup.h"
#include <string.h>
#include <stdio.h>

static char base64_decode_table[256];

static void livemedia__init_base64_decode_table() 
{

	int i;
	for (i = 0; i < 256; ++i) 
		base64_decode_table[i] = (char)0x80;
	/* default value: invalid */

	for (i = 'A'; i <= 'Z'; ++i)
		base64_decode_table[i] = 0 + (i - 'A');
	for (i = 'a'; i <= 'z'; ++i) 
		base64_decode_table[i] = 26 + (i - 'a');
	for (i = '0'; i <= '9'; ++i) 
		base64_decode_table[i] = 52 + (i - '0');

	base64_decode_table[(unsigned char)'+'] = 62;
	base64_decode_table[(unsigned char)'/'] = 63;
	base64_decode_table[(unsigned char)'='] = 0;
}


unsigned char *livemedia__base64_decode(char const *in, unsigned int *result_size,
		bool trim_trailing_zeros/* default: true */)
{
	if (in == NULL)
		return NULL;	/* sanity check */
	return livemedia__base64_decode_by_input_size(in, strlen(in), result_size,
			trim_trailing_zeros);
}

unsigned char *livemedia__base64_decode_by_input_size(char const *in, 
		unsigned int in_size,
		unsigned int *result_size,
		bool trim_trailing_zeros)
{
	static bool have_initialized_base64_decode_table = false;
	unsigned char *out;
	int i,j,k;
	int padding_count;
	int j_max;
	char in_tmp[4];
	char out_tmp[4];
	unsigned char *result;

	if (!have_initialized_base64_decode_table) {
		livemedia__init_base64_decode_table();
		have_initialized_base64_decode_table = true;
	}
	out = (unsigned char *)malloc(sizeof(unsigned char) * (in_size + 1));
	/* ensures we have enough space */
	if (out == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char) * (in_size + 1)) failed in %s()\n", __func__);
	}
	k = 0;
	padding_count = 0;
	j_max = in_size - 3;
	/* in case "inSize" is not a multiple of 4 (although it should be) */
	for(j = 0; j < j_max; j += 4) {
		for (i = 0; i < 4; ++i) {
			in_tmp[i] = in[i+j];
			if (in_tmp[i] == '=')
				++padding_count;
			out_tmp[i] = base64_decode_table[(unsigned char)in_tmp[i]];
			if ((out_tmp[i] & 0x80) != 0)
				out_tmp[i] = 0;
			/* this happens only if there was an invalid character; pretend that it was 'A' */
		}

		out[k++] = (out_tmp[0] << 2) | (out_tmp[1] >> 4);
		out[k++] = (out_tmp[1] << 4) | (out_tmp[2] >> 2);
		out[k++] = (out_tmp[2] << 6) | out_tmp[3];
	}

	if (trim_trailing_zeros) {
		while(padding_count > 0 && k > 0 && out[k-1] == '\0') {
			--k;
			--padding_count;
		}
	}
	*result_size = k;
	result = (unsigned char *)malloc(sizeof(unsigned char)*(*result_size));
	memmove(result, out, *result_size);
	if (out) {
		free(out);
		out = NULL;
	}
	return result;
}

static const char* base64_char = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *livemedia__base64_encode(char const *orig_signed, unsigned int orig_length)
{
	unsigned char const *orig;
	unsigned int num_orig24_bit_values;
	bool have_padding;
	bool have_padding2;
	unsigned int num_result_bytes;
	char *result;
	unsigned int i;

	orig = (unsigned char const *)orig_signed;
	/* in case any input bytes have the MSB set */
	if(orig == NULL)
		return NULL;

	num_orig24_bit_values = orig_length/3;
	have_padding = orig_length > num_orig24_bit_values*3;
	have_padding2 = orig_length == num_orig24_bit_values*3 + 2;
	num_result_bytes = 4*(num_orig24_bit_values + have_padding);
	result = (char *)malloc(sizeof(char)*(num_result_bytes + 1));
	/* allow for trailing '\0' */

	/* Map each full group of 3 input bytes into 4 output base-64 characters: */
	for (i = 0; i < num_orig24_bit_values; ++i) {
		result[4*i+0] = base64_char[(orig[3*i]>>2)&0x3F];
		result[4*i+1] = base64_char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
		result[4*i+2] = base64_char[((orig[3*i+1]<<2) | (orig[3*i+2]>>6))&0x3F];
		result[4*i+3] = base64_char[orig[3*i+2]&0x3F];
	}

	/* Now, take padding into account.  (Note: i == numOrig24BitValues) */
	if (have_padding) {
		result[4*i+0] = base64_char[(orig[3*i]>>2)&0x3F];
		if (have_padding2) {
			result[4*i+1] = base64_char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
			result[4*i+2] = base64_char[(orig[3*i+1]<<2)&0x3F];
		} else {
			result[4*i+1] = base64_char[((orig[3*i]&0x3)<<4)&0x3F];
			result[4*i+2] = '=';
		}
		result[4*i+3] = '=';
	}

	result[num_result_bytes] = '\0';
	return result;
}

