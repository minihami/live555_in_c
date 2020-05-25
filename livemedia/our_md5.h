#ifndef __LIVEMEDIA_OUR_MD5_H__
#define __LIVEMEDIA_OUR_MD5_H__

extern char *livemedia__our_md5_data(unsigned char const *data, unsigned int data_size,
		char *output_digest);
    /* "outputDigest" must be either NULL (in which case this function returns a heap-allocated
    * buffer, which should be later delete[]d by the caller), or else it must point to
    * a (>=)33-byte buffer (which this function will also return). */

extern unsigned char *livemedia__our_md5_data_raw(unsigned char const *data,
		unsigned int data_size, unsigned char *output_digest);
    /* Like "ourMD5Data()", except that it returns the digest in 'raw' binary form, rather than
    * as an ASCII hex string.
    * "outputDigest" must be either NULL (in which case this function returns a heap-allocated
    * buffer, which should be later delete[]d by the caller), or else it must point to
    * a (>=)16-byte buffer (which this function will also return). */

#endif
