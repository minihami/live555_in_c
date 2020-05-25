#ifndef __LIVEMEDIA_HMAC_SHA1_H__
#define __LIVEMEDIA_HMAC_SHA1_H__

#ifndef NO_OPENSSL
#ifndef __LIVEMEDIA_HMAC_HASH_H__
#include "hmac_hash.h"
#endif

#define SHA1_DIGEST_LEN 20

livemedia__hmac_hash livemedia_hmac_sha1;
#endif
#endif
