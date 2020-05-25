#include "hmac_sha1.h"

#ifndef NO_OPENSSL
#if defined(__APPLE__)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#endif
#include <openssl/evp.h>


static void livemedia__sha1(u_int8_t* result_digest/*must be SHA1_DIGEST_LEN bytes in size*/,
		 u_int8_t const* data1, unsigned int data1_length,
		 u_int8_t const* data2, unsigned int data2_length) 
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia__hmac_sha1(u_int8_t const *key, unsigned int key_length, u_int8_t const *text, unsigned int text_length,
		u_int8_t *result_digest/*must be SHA1_DIGEST_LEN bytes in size*/)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}
#endif
