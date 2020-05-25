#ifndef __LIVEMEDIA_HMAC_HASH_H__
#define __LIVEMEDIA_HMAC_HASH_H__

#ifndef __GROUPSOCK_NET_COMMON_H__
#include "../groupsock/net_common.h"
#endif

/* All HMAC hash functions have the following signature: */
typedef void livemedia__hmac_hash(u_int8_t const *key, unsigned int key_length,
		       u_int8_t const *text, unsigned int text_length,
		       u_int8_t *result_digest);
    /* "resultDigest" must point to an array of sufficient size to hold the digest */

#define HMAC_BLOCK_SIZE 64

#endif
