#include "mikey.h"
#include "../groupsock/groupsock_helper.h" /* for our_random32() */


/*****************************************************************
 * MIKEYState (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mikey_state_t *__livemedia_mikey_state__alloc(void)
{
	livemedia_mikey_state_t *mikey_state;
	mikey_state = (livemedia_mikey_state_t *)calloc(1, sizeof(livemedia_mikey_state_t));
	if (mikey_state == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_mikey_state_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return mikey_state;
}
void __livemedia_mikey_state__init(livemedia_mikey_state_t *mikey_state,
		u_int8_t const *message_to_parse, unsigned int message_size, bool *parsed_ok)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	mikey_state->delete = livemedia_mikey_state__delete__impl;
}
void __livemedia_mikey_state__deinit(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_mikey_state__free(livemedia_mikey_state_t *mikey_state)
{
	if (mikey_state) {
		free(mikey_state);
		mikey_state = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_mikey_state_t *livemedia__new__mikey_state(
		u_int8_t const *message_to_parse, unsigned int message_size, bool *parsed_ok)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mikey_state_t *mikey_state;

	mikey_state = __livemedia_mikey_state__alloc();
	if (mikey_state)
		__livemedia_mikey_state__init(mikey_state, message_to_parse,
				message_size, parsed_ok);

	return mikey_state;
}
void livemedia__delete__mikey_state(livemedia_mikey_state_t *mikey_state)
{
	if (mikey_state && mikey_state->delete)
		mikey_state->delete(mikey_state);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_mikey_state__delete__impl(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (mikey_state) {
		__livemedia_mikey_state__deinit(mikey_state);
		__livemedia_mikey_state__free(mikey_state);
	}
}

/*
 * Functions in header file
 */
bool livemedia_mikey_state__encrypt_srtp(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_mikey_state__encrypt_srtcp(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int8_t const *livemedia_mikey_state__key_data(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int32_t livemedia_mikey_state__mki(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_mikey_state__use_authentication(livemedia_mikey_state_t *mikey_state)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Normal functions
 */
livemedia_mikey_state_t *livemedia_mikey_state__create_new__static(
		u_int8_t *message_to_parse, unsigned int message_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

u_int8_t *livemedia_mikey_state__generate_message(livemedia_mikey_state_t *mikey_state,
		unsigned int *message_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

void livemedia_mikey_state__add_new_payload(livemedia_mikey_state_t *mikey_state,
		livemedia_mikey_payload_t *new_payload)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_mikey_state__parse_hdr_payload(livemedia_mikey_state_t *mikey_state,
		u_int8_t const **ptr, u_int8_t const *end_ptr, u_int8_t *next_payload_type)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}

bool livemedia_mikey_state__parse_non_hdr_payload(livemedia_mikey_state_t *mikey_state,
		u_int8_t const **ptr, u_int8_t const *end_ptr, u_int8_t *next_payload_type)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif

}


/*
 * Static functions implementation for ancestor
 */
/*****************************************************************
 * MIKEYPayload (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_mikey_payload_t *__livemedia_mikey_payload__alloc(void)
{
	livemedia_mikey_payload_t *mikey_payload;
	mikey_payload = (livemedia_mikey_payload_t *)calloc(1, sizeof(livemedia_mikey_payload_t));
	if (mikey_payload == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_mikey_payload_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return mikey_payload;
}
void __livemedia_mikey_payload__init(livemedia_mikey_payload_t *mikey_payload,
		livemedia_mikey_state_t *our_mikey_state, u_int8_t payload_type)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	mikey_payload->delete = livemedia_mikey_payload__delete__impl;
}
void __livemedia_mikey_payload__init_by_data(livemedia_mikey_payload_t *mikey_payload,
		livemedia_mikey_state_t *our_mikey_state, u_int8_t payload_type,
		u_int8_t const *data, unsigned int data_size)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: parent init */

	/* TODO: constructor */
	
	/* TODO: virtual functions configuration */ 

	mikey_payload->delete = livemedia_mikey_payload__delete__impl;
}
void __livemedia_mikey_payload__deinit(livemedia_mikey_payload_t *mikey_payload)
{
#ifdef WEBGATE_DEBUG
	printf("TODO: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* TODO: destructor */

	/* TODO: parent deinit */
}
void __livemedia_mikey_payload__free(livemedia_mikey_payload_t *mikey_payload)
{
	if (mikey_payload) {
		free(mikey_payload);
		mikey_payload = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_mikey_payload_t *livemedia__new__mikey_payload(
		livemedia_mikey_state_t *our_mikey_state, u_int8_t payload_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mikey_payload_t *mikey_payload;

	mikey_payload = __livemedia_mikey_payload__alloc();
	if (mikey_payload)
		__livemedia_mikey_payload__init(mikey_payload, our_mikey_state,
				payload_type);

	return mikey_payload;
}
livemedia_mikey_payload_t *livemedia__new__mikey_payload_by_data(
		livemedia_mikey_state_t *our_mikey_state, u_int8_t payload_type,
		u_int8_t const *data, unsigned int data_size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_mikey_payload_t *mikey_payload;

	mikey_payload = __livemedia_mikey_payload__alloc();
	if (mikey_payload)
		__livemedia_mikey_payload__init_by_data(mikey_payload, our_mikey_state,
				payload_type, data, data_size);

	return mikey_payload;
}
void livemedia__delete__mikey_payload(livemedia_mikey_payload_t *mikey_payload)
{
	if (mikey_payload && mikey_payload->delete)
		mikey_payload->delete(mikey_payload);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_mikey_payload__delete__impl(livemedia_mikey_payload_t *mikey_payload)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (mikey_payload) {
		__livemedia_mikey_payload__deinit(mikey_payload);
		__livemedia_mikey_payload__free(mikey_payload);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*
 * Static functions implementation for ancestor
 */

