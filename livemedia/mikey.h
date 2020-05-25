#ifndef __LIVEMEDIA_MIKEY_H__
#define __LIVEMEDIA_MIKEY_H__

#ifndef __GROUPSOCK_NET_COMMON_H__
#include "../groupsock/net_common.h"
#endif
#include <stdbool.h>

struct _livemedia_mikey_payload_t;
/*****************************************************************
 * MIKEYState (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_mikey_state_t {
	/* Encryption/authentication parameters, either set by default
	 * (if the first (parameterless) constructor is used), or set by parsing an input message
	 * (if the second constructor is used): */
	bool encrypt_srtp;
	bool encrypt_srtcp;
	u_int8_t key_data[16 + 14]; /* encryption key + salt */
	u_int32_t mki; /*used only if encryption is used. (We assume a MKI length of 4. ) */
	bool use_authentication;
	/* Our internal binary representation of the MIKEY payloads: */
	struct _livemedia_mikey_payload_t *header_payload;
	struct _livemedia_mikey_payload_t *tail_payload;
	unsigned int total_payload_byte_count;

	void (*delete)(struct _livemedia_mikey_state_t *mikey_state);
} livemedia_mikey_state_t;

/*****************************************************************
 * MIKEYPayload (Structure Declarations)
 ****************************************************************/
typedef struct _livemedia_mikey_payload_t {
	livemedia_mikey_state_t *our_mikey_state;
	u_int8_t payload_type;
	u_int8_t *data;
	unsigned int data_size;
	struct _livemedia_mikey_payload_t *next;

	void (*delete)(struct _livemedia_mikey_payload_t *mikey_payload);
} livemedia_mikey_payload_t;


/*****************************************************************
 * MIKEYState (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mikey_state_t *__livemedia_mikey_state__alloc(void);
void __livemedia_mikey_state__init(livemedia_mikey_state_t *mikey_state,
		u_int8_t const *message_to_parse, unsigned int message_size, bool *parsed_ok);
void __livemedia_mikey_state__deinit(livemedia_mikey_state_t *mikey_state);
void __livemedia_mikey_state__free(livemedia_mikey_state_t *mikey_state);
/*
 * new() and delete() functions
 */
livemedia_mikey_state_t *livemedia__new__mikey_state(
		u_int8_t const *message_to_parse, unsigned int message_size, bool *parsed_ok);
void __livemedia_mikey_state__deinit(livemedia_mikey_state_t *mikey_state);
void livemedia__delete__mikey_state(livemedia_mikey_state_t *mikey_state);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_mikey_state__delete__impl(livemedia_mikey_state_t *mikey_state);

/*
 * Functions in header file
 */
/* Accessors for the encryption/authentication parameters: */
bool livemedia_mikey_state__encrypt_srtp(livemedia_mikey_state_t *mikey_state);
bool livemedia_mikey_state__encrypt_srtcp(livemedia_mikey_state_t *mikey_state);
u_int8_t const *livemedia_mikey_state__key_data(livemedia_mikey_state_t *mikey_state);
u_int32_t livemedia_mikey_state__mki(livemedia_mikey_state_t *mikey_state);
bool livemedia_mikey_state__use_authentication(livemedia_mikey_state_t *mikey_state);

/*
 * Normal functions
 */
livemedia_mikey_state_t *livemedia_mikey_state__create_new__static(
		u_int8_t *message_to_parse, unsigned int message_size);
      /* (Attempts to) parse a binary MIKEY message, returning a new "MIKEYState" if successful
      * (or NULL if unsuccessful).
      * ("messageToParse" is assumed to have been dynamically allocated;
      * this function will delete[] it.) */

u_int8_t *livemedia_mikey_state__generate_message(livemedia_mikey_state_t *mikey_state,
		unsigned int *message_size);
/* Returns a binary message representing the current MIKEY state, of size "messageSize" bytes.
 * This array is dynamically allocated by this routine, and must be delete[]d by the caller. */

void livemedia_mikey_state__add_new_payload(livemedia_mikey_state_t *mikey_state,
		livemedia_mikey_payload_t *new_payload);
bool livemedia_mikey_state__parse_hdr_payload(livemedia_mikey_state_t *mikey_state,
		u_int8_t const **ptr, u_int8_t const *end_ptr, u_int8_t *next_payload_type);
bool livemedia_mikey_state__parse_non_hdr_payload(livemedia_mikey_state_t *mikey_state,
		u_int8_t const **ptr, u_int8_t const *end_ptr, u_int8_t *next_payload_type);

/*****************************************************************
 * MIKEYPayload (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mikey_payload_t *__livemedia_mikey_payload__alloc(void);
void __livemedia_mikey_payload__init(livemedia_mikey_payload_t *mikey_payload,
		livemedia_mikey_state_t *our_mikey_state, u_int8_t payload_type);
void __livemedia_mikey_payload__deinit(livemedia_mikey_payload_t *mikey_payload);
void __livemedia_mikey_payload__free(livemedia_mikey_payload_t *mikey_payload);
/*
 * new() and delete() functions
 */
livemedia_mikey_payload_t *livemedia__new__mikey_payload(
		livemedia_mikey_state_t *our_mikey_state, u_int8_t payload_type);
void livemedia__delete__mikey_payload(livemedia_mikey_payload_t *mikey_payload);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_mikey_payload__delete__impl(livemedia_mikey_payload_t *mikey_payload);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */


#endif
