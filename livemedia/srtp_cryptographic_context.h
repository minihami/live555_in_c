#ifndef __LIVEMEDIA_SRTP_CRYPTOGRAPHIC_CONTEXT_H__
#define __LIVEMEDIA_SRTP_CRYPTOGRAPHIC_CONTEXT_H__

#ifndef __LIVEMEDIA_MIKEY_H__
#include "mikey.h"
#endif

#define SRTP_CIPHER_KEY_LENGTH (128/8) // in bytes
#define SRTP_CIPHER_SALT_LENGTH (112/8) // in bytes
#define SRTP_MKI_LENGTH 4 // in bytes
#define SRTP_AUTH_KEY_LENGTH (160/8) // in bytes
#define SRTP_AUTH_TAG_LENGTH (80/8) // in bytes

struct derived_keys {
	u_int8_t cipher_key[SRTP_CIPHER_KEY_LENGTH];
	u_int8_t salt[SRTP_CIPHER_KEY_LENGTH];
	u_int8_t auth_key[SRTP_CIPHER_KEY_LENGTH];
};

struct all_derived_keys {
	struct derived_keys srtp;
	struct derived_keys srtcp;
};

typedef enum {
	label_srtp_encryption 	= 0x00,
	label_srtp_msg_auth 	= 0x01,
	label_srtp_salt 		= 0x02,
	label_srtcp_encryption 	= 0x03,
	label_srtcp_msg_auth 	= 0x04,
	label_srtcp_salt 		= 0x05,
} srtp_key_derivation_label;

/*****************************************************************
 * SRTPCryptographicContext (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_srtp_cryptographic_context_t {

	livemedia_mikey_state_t const *mikey_state;
	/* Derived {i.e., session) keys: */
	struct all_derived_keys derived_keys;
	/* State used for handling the reception of SRTP packets: */
	bool have_received_srtp_packets;
	u_int16_t previous_high_rtp_seq_num;
	u_int32_t roc; /* rollover counter */
	/* State used for handling the sending of SRTCP packets: */
	u_int32_t srtcp_index;

	void (*delete)(
			struct _livemedia_srtp_cryptographic_context_t *srtp_ctyptographic_context);
} livemedia_srtp_cryptographic_context_t;

/*****************************************************************
 * SRTPCryptographicContext (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_srtp_cryptographic_context_t *__livemedia_srtp_cryptographic_context__alloc(void);
void __livemedia_srtp_cryptographic_context__init(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		livemedia_mikey_state_t const *mikey_state);
void __livemedia_srtp_cryptographic_context__deinit(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
void __livemedia_srtp_cryptographic_context__free(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
/*
 * new() and delete() functions
 */
livemedia_srtp_cryptographic_context_t *livemedia__new__srtp_cryptographic_context(
		livemedia_mikey_state_t const *mikey_state);
void livemedia__delete__srtp_cryptographic_context(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_srtp_cryptographic_context__delete__impl(livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);

/*
 * Functions in header file
 */
u_int8_t const *livemedia_srtp_cryptographic_context__master_key_plus_salt(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
u_int8_t const *livemedia_srtp_cryptographic_context__master_key(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
u_int8_t const *livemedia_srtp_cryptographic_context__master_salt(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
bool livemedia_srtp_cryptographic_context__we_encrypt_srtp(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
bool livemedia_srtp_cryptographic_context__we_encrypt_srtcp(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
bool livemedia_srtp_cryptographic_context__we_authenticate(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
u_int32_t livemedia_srtp_cryptographic_context__mki(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);

/*
 * Normal functions
 */
/* Authenticate (if necessary) and decrypt (if necessary) incoming SRTP and SRTCP packets.
 * Returns True iff the packet is well-formed and authenticates OK.
 * ("outPacketSize" will be <= "inPacketSize".) */
bool livemedia_srtp_cryptographic_context__process_incoming_srtp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *buffer, unsigned int in_packet_size, unsigned int *out_packet_size);
bool livemedia_srtp_cryptographic_context__process_incoming_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *buffer, unsigned int in_packet_size, unsigned int *out_packet_size);

/* Encrypt (if necessary) and add an authentication tag (if necessary) to an outgoing
 * RTCP packet.
 * Returns True iff the packet is well-formed.
 * ("outPacketSize" will be >= "inPacketSize"; there must be enough space at the end of
 * "buffer" for the extra SRTCP tags (4+4+10 bytes).) */
bool livemedia_srtp_cryptographic_context__process_outgoing_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *buffer, unsigned int in_packet_size, unsigned int *out_packet_size);

#ifndef NO_OPENSSL
  // Definitions specific to the "SRTP_AES128_CM_HMAC_SHA1_80" ciphersuite.
  // Later generalize to support more SRTP ciphersuites #####

unsigned int livemedia_srtp_cryptographic_context__generate_srtcp_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *data_to_authenticate, unsigned int num_bytes_to_authenticate,
		u_int8_t *result_authentication_tag);
/* returns the size of the resulting authentication tag */

bool livemedia_srtp_cryptographic_context__verify_srtp_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t *data_to_authenticate, unsigned int num_bytes_to_authenticate,
		u_int32_t roc, u_int8_t const *authentication_tag);
bool livemedia_srtp_cryptographic_context__verify_srtcp_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *data_to_authenticate, unsigned int num_bytes_to_authenticate,
		u_int8_t const * authentication_tag);

void livemedia_srtp_cryptographic_context__decrypt_srtp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int64_t index, u_int32_t ssrc, u_int8_t *data, unsigned int num_data_bytes);
void livemedia_srtp_cryptographic_context__decrypt_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int64_t index, u_int32_t ssrc, u_int8_t *data, unsigned int num_data_bytes);
void livemedia_srtp_cryptographic_context__encrypt_srtcp_packet(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int64_t index, u_int32_t ssrc, u_int8_t *data, unsigned int num_data_bytes);

unsigned int livemedia_srtp_cryptographic_context__generate_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		struct derived_keys *keys_to_use, u_int8_t const *data_to_authenticate,
		unsigned int num_bytes_to_authenticate, u_int8_t result_authentication_tag);
/* returns the size of the resulting authentication tag
 * "resultAuthenticationTag" must point to an array of at least SRTP_AUTH_TAG_LENGTH */
bool livemedia_srtp_cryptographic_context__verify_authentication_tag(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		struct derived_keys *keys_to_use, u_int8_t const *data_to_authenticate,
		unsigned num_bytes_to_authenticate, u_int8_t const *authentication_tag);
void livemedia_srtp_cryptographic_context__crypt_data(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		struct derived_keys *keys, u_int64_t index, u_int32_t ssrc, u_int8_t *data,
		unsigned int num_data_bytes);
void livemedia_srtp_cryptographic_context__perform_key_derivation(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context);
void livemedia_srtp_cryptographic_context__derive_keys_from_master(
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *master_key, u_int8_t const *salt, struct all_derived_keys *all_keys_result);
/* used to implement "performKeyDerivation()" */
void livemedia_srtp_cryptographic_context__derive_single_key( 
		livemedia_srtp_cryptographic_context_t *srtp_cryptographic_context,
		u_int8_t const *master_key, u_int8_t const *salt,
		srtp_key_derivation_label label, unsigned int result_key_length,
		u_int8_t *result_key);
/* used to implement "deriveKeysFromMaster()".
 * ("resultKey" must be an existing buffer, of size >= "resultKeyLength") */

#endif

#endif
