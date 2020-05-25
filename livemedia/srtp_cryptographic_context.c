#include "srtp_cryptographic_context.h"
#ifndef NO_OPENSSL
#include "hmac_sha1.h"
#include <openssl/aes.h>
#endif

#include <stdio.h>

/*****************************************************************
 * SRTPCryptographicContext (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_srtp_cryptographic_context_t *__livemedia_srtp_cryptographic_context__alloc(void)
{
	livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context;
	srtp_cryptographic_context = (livemedia_srtp_cryptographic_context_t *)calloc(1, sizeof(livemedia_srtp_cryptographic_context_t));
	if (srtp_cryptographic_context == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_srtp_cryptographic_context_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return srtp_cryptographic_context;
}
void __livemedia_srtp_cryptographic_context__init(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		livemedia_mikey_state_t const *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	srtp_cryptographic_context->delete = livemedia_srtp_cryptographic_context__delete__impl;
}
void __livemedia_srtp_cryptographic_context__deinit(livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_srtp_cryptographic_context__free(livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
	if (srtp_cryptographic_context) {
		free(srtp_cryptographic_context);
		srtp_cryptographic_context = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_srtp_cryptographic_context_t *livemedia__new__srtp_cryptographic_context(
		livemedia_mikey_state_t const *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context;

	srtp_cryptographic_context = __livemedia_srtp_cryptographic_context__alloc();
	if (srtp_cryptographic_context)
		__livemedia_srtp_cryptographic_context__init(srtp_cryptographic_context, mikey_state);

	return srtp_cryptographic_context;
}
void livemedia__delete__srtp_cryptographic_context(livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
	if (srtp_cryptographic_context && srtp_cryptographic_context->delete)
		srtp_cryptographic_context->delete(srtp_cryptographic_context);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_srtp_cryptographic_context__delete__impl(livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/*TODO: if child, get parent as parameter and cast to child */
	if (srtp_cryptographic_context) {
		__livemedia_srtp_cryptographic_context__deinit(srtp_cryptographic_context);
		__livemedia_srtp_cryptographic_context__free(srtp_cryptographic_context);
	}
}

/*
 * Functions in header file
 */
u_int8_t const *livemedia_srtp_cryptographic_context__master_key_plus_salt(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int8_t const *livemedia_srtp_cryptographic_context__master_key(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int8_t const *livemedia_srtp_cryptographic_context__master_salt(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__we_encrypt_srtp(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__we_encrypt_srtcp(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__we_authenticate(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int32_t livemedia_srtp_cryptographic_context__mki(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Normal functions
 */
bool livemedia_srtp_cryptographic_context__process_incoming_srtp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *buffer, unsigned int in_packet_size, unsigned int *out_packet_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__process_incoming_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *buffer, unsigned int in_packet_size, unsigned int *out_packet_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}
bool livemedia_srtp_cryptographic_context__process_outgoing_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *buffer, unsigned int in_packet_size, unsigned int *out_packet_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_srtp_cryptographic_context__generate_srtcp_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *data_to_authenticate, unsigned int num_bytes_to_authenticate,
		u_int8_t *result_authentication_tag)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__verify_srtp_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *data_to_authenticate, unsigned int num_bytes_to_authenticate,
		u_int32_t roc, u_int8_t const *authentication_tag)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__verify_srtcp_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *data_to_authenticate, unsigned int num_bytes_to_authenticate,
		u_int8_t const * authentication_tag)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__decrypt_srtp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int64_t index, u_int32_t ssrc, u_int8_t *data, unsigned int num_data_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__decrypt_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int64_t index, u_int32_t ssrc, u_int8_t *data, unsigned int num_data_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__encrypt_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int64_t index, u_int32_t ssrc, u_int8_t *data, unsigned int num_data_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

unsigned int livemedia_srtp_cryptographic_context__generate_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		struct derived_keys *keys_to_use, u_int8_t const *data_to_authenticate,
		unsigned int num_bytes_to_authenticate, u_int8_t result_authentication_tag)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_srtp_cryptographic_context__verify_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		struct derived_keys *keys_to_use, u_int8_t const *data_to_authenticate,
		unsigned num_bytes_to_authenticate, u_int8_t const *authentication_tag)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__crypt_data(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		struct derived_keys *keys, u_int64_t index, u_int32_t ssrc, u_int8_t *data,
		unsigned int num_data_bytes)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__perform_key_derivation(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__derive_keys_from_master(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *master_key, u_int8_t const *salt, struct all_derived_keys *all_keys_result)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_srtp_cryptographic_context__derive_single_key( 
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *master_key, u_int8_t const *salt,
		srtp_key_derivation_label label, unsigned int result_key_length,
		u_int8_t *result_key)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

/*
 * Static functions implementation for ancestor
 */
