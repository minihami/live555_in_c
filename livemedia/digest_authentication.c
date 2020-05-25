#include "digest_authentication.h"
#include "our_md5.h"
#include "str_dup.h"
#include "../groupsock/groupsock_helper.h"	/* for gettimeofday() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************
 * Authenticator: (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_authenticator_t *__livemedia_authenticator__alloc(void)
{
	livemedia_authenticator_t *authenticator;
	authenticator = (livemedia_authenticator_t *)calloc(1, sizeof(livemedia_authenticator_t));
	if (authenticator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_authenticator_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return authenticator;
}
void __livemedia_authenticator__init(livemedia_authenticator_t *authenticator)
{
	/* constructor */
	livemedia_authenticator__assign(authenticator, NULL, NULL, NULL, NULL, false);
	
	/* virtual functions configuration */ 
	authenticator->delete = livemedia_authenticator__delete__impl;
}

void __livemedia_authenticator__init_by_username_and_password(livemedia_authenticator_t *authenticator,
		char const *username, char const *password, bool password_is_md5 /* default: false */)
{
	/* constructor */
	livemedia_authenticator__assign(authenticator, NULL, NULL, username, password, password_is_md5);
	
	/* virtual functions configuration */ 
	authenticator->delete = livemedia_authenticator__delete__impl;

}

void __livemedia_authenticator__init_by_authenticator(livemedia_authenticator_t *authenticator,
		livemedia_authenticator_t *orig)
{
	/* constructor */
	livemedia_authenticator__assign(authenticator, livemedia_authenticator__realm(orig),
			livemedia_authenticator__nonce(orig),
			livemedia_authenticator__username(orig),
			livemedia_authenticator__password(orig),
			authenticator->password_is_md5);
	
	/* virtual functions configuration */ 
	authenticator->delete = livemedia_authenticator__delete__impl;
}

void __livemedia_authenticator__deinit(livemedia_authenticator_t *authenticator)
{
	livemedia_authenticator__reset(authenticator);
}
void __livemedia_authenticator__free(livemedia_authenticator_t *authenticator)
{
	if (authenticator) {
		free(authenticator);
		authenticator = NULL;
	}
}
/*
 * new() and delete() functions
 */
livemedia_authenticator_t *livemedia__new__authenticator(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator_t *authenticator;

	authenticator = __livemedia_authenticator__alloc();
	if (authenticator)
		__livemedia_authenticator__init(authenticator);

	return authenticator;
}

livemedia_authenticator_t *livemedia__new__authenticator_by_username_and_password(
		char const *username, char const *password, bool password_is_md5 /* default: false */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator_t *authenticator;

	authenticator = __livemedia_authenticator__alloc();
	if (authenticator)
		__livemedia_authenticator__init_by_username_and_password(authenticator,
				username, password, password_is_md5);

	return authenticator;
}

livemedia_authenticator_t *livemedia__new__authenticator_by_authenticator(
		livemedia_authenticator_t *orig)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator_t *authenticator;

	authenticator = __livemedia_authenticator__alloc();
	if (authenticator)
		__livemedia_authenticator__init_by_authenticator(authenticator,
				orig);

	return authenticator;
}

void livemedia__delete__authenticator(livemedia_authenticator_t *authenticator)
{
	if (authenticator && authenticator->delete)
		authenticator->delete(authenticator);
}
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_authenticator__delete__impl(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (authenticator) {
		__livemedia_authenticator__deinit(authenticator);
		__livemedia_authenticator__free(authenticator);
	}
}

/*
 * Functions in header file
 */
char const *livemedia_authenticator__realm(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return authenticator->realm;
}
char const *livemedia_authenticator__nonce(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return authenticator->nonce;
}
char const *livemedia_authenticator__username(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return authenticator->username;
}
char const *livemedia_authenticator__password(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return authenticator->password;
}

/*
 * Normal functions
 */
void livemedia_authenticator__reset(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator__reset_realm_and_nonce(authenticator);
	livemedia_authenticator__reset_username_and_password(authenticator);
}

void livemedia_authenticator__set_realm_and_nonce(livemedia_authenticator_t *authenticator,
		char const *realm, char const *nonce)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator__reset_realm_and_nonce(authenticator);
	livemedia_authenticator__assign_realm_and_nonce(authenticator, realm, nonce);
}

void livemedia_authenticator__set_realm_and_random_nonce(livemedia_authenticator_t *authenticator,
		char const *realm)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	static unsigned int counter;
	char nonce_buf[33];
	livemedia_authenticator__reset_realm_and_nonce(authenticator);

	/* Construct data to seed the random nonce: */
	struct {
		struct timeval timestamp;
		unsigned int counter;
	} seed_data;
	gettimeofday(&seed_data.timestamp, NULL);
	counter = 0;
	seed_data.counter = ++counter;
  
	/* Use MD5 to compute a 'random' nonce from this seed data: */
	livemedia__our_md5_data((unsigned char *)(&seed_data), sizeof(seed_data), nonce_buf);

	livemedia_authenticator__assign_realm_and_nonce(authenticator, realm, nonce_buf);
}

void livemedia_authenticator__set_username_and_password(livemedia_authenticator_t *authenticator,
		char const *username, char const *password, bool password_is_md5 /* default: false */)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator__reset_username_and_password(authenticator);
	livemedia_authenticator__assign_username_and_password(authenticator,
			username, password, password_is_md5);
}

char const *livemedia_authenticator__compute_digest_response(livemedia_authenticator_t *authenticator,
		char const *cmd, char const *url)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
  /* The "response" field is computed as:
   *    md5(md5(<username>:<realm>:<password>):<nonce>:md5(<cmd>:<url>))
   * or, if "fPasswordIsMD5" is True:
   *    md5(<password>:<nonce>:md5(<cmd>:<url>)) */
	char ha1_buf[33];
	char ha2_buf[33];
	unsigned int ha1_data_len;
	unsigned char * ha1_data;
	unsigned int ha2_data_len;
	unsigned char * ha2_data;
	unsigned int digest_data_len;
	unsigned char * digest_data;
	char const *result;

	if (authenticator->password_is_md5) {
		strncpy(ha1_buf, livemedia_authenticator__password(authenticator), 32);
		ha1_buf[32] = '\0';	/* just in case */
	} else {
		ha1_data_len = strlen(livemedia_authenticator__username(authenticator)) + 1
			+ strlen(livemedia_authenticator__realm(authenticator)) + 1 
			+ strlen(livemedia_authenticator__password(authenticator));
		ha1_data = (unsigned char *)malloc(sizeof(unsigned char) * (ha1_data_len + 1));
		if(ha1_data == NULL) {
			fprintf(stderr, "malloc(sizeof(unsigned char) * (ha1_data_len + 1))"
					"failed in %s()\n", __func__);
		}
		sprintf((char *)ha1_data, "%s:%s:%s", 
				livemedia_authenticator__username(authenticator),
				livemedia_authenticator__realm(authenticator),
				livemedia_authenticator__password(authenticator));
		livemedia__our_md5_data(ha1_data, ha1_data_len, ha1_buf);
		if (ha1_data) {
			free(ha1_data);
			ha1_data = NULL;
		}
	}

	ha2_data_len = strlen(cmd) + 1 + strlen(url);
	ha2_data = (unsigned char *)malloc(sizeof(unsigned char) * (ha2_data_len + 1));
	if(ha2_data == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char) * (ha2_data_len + 1))"
				"failed in %s()\n", __func__);
	}
	sprintf((char *)ha2_data, "%s:%s", cmd, url);
	livemedia__our_md5_data(ha2_data, ha2_data_len, ha2_buf);
	if (ha2_data) {
		free(ha2_data);
		ha2_data = NULL;
	}

	digest_data_len
		= 32 + 1 + strlen(livemedia_authenticator__nonce(authenticator)) + 1 + 32;
	digest_data = (unsigned char *)malloc(sizeof(unsigned char) * (digest_data_len + 1));
	if(digest_data == NULL) {
		fprintf(stderr, "malloc(sizeof(unsigned char) * (digest_data_len + 1))"
				"failed in %s()\n", __func__);
	}
	sprintf((char *)digest_data, "%s:%s:%s", 
			ha1_buf, livemedia_authenticator__nonce(authenticator), ha2_buf);
	result = livemedia__our_md5_data(digest_data, digest_data_len, NULL);

	if (digest_data) {
		free(digest_data);
		digest_data = NULL;
	}
	return result;
}

void livemedia_authenticator__reclaim_digest_response(livemedia_authenticator_t *authenticator,
		char const *response_str)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (response_str) {
		free((char *)response_str);
		response_str = NULL;
	}
}

void livemedia_authenticator__reset_realm_and_nonce(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (authenticator->realm) {
		free(authenticator->realm);
		authenticator->realm = NULL;
	}
	if (authenticator->nonce) {
		free(authenticator->nonce);
		authenticator->nonce = NULL;
	}

}

void livemedia_authenticator__reset_username_and_password(livemedia_authenticator_t *authenticator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (authenticator->username) {
		free(authenticator->username);
		authenticator->username = NULL;
	}
	if (authenticator->password) {
		free(authenticator->password);
		authenticator->password = NULL;
	}
	authenticator->password_is_md5 = false;
}

void livemedia_authenticator__assign_realm_and_nonce(livemedia_authenticator_t *authenticator,
		char const *realm, char const *nonce)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	authenticator->realm = livemedia__str_dup(realm);
	authenticator->nonce = livemedia__str_dup(nonce);
}

void livemedia_authenticator__assign_username_and_password(livemedia_authenticator_t *authenticator,
	 char const *username, char const *password, bool password_is_md5)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (username == NULL)
		username = "";
	if (password == NULL)
		password = "";

	authenticator->username = livemedia__str_dup(username);
	authenticator->password = livemedia__str_dup(password);
	authenticator->password_is_md5 = password_is_md5;
}

void livemedia_authenticator__assign(livemedia_authenticator_t *authenticator,
		char const *realm, char const *nonce,
		char const *username, char const *password, bool password_is_md5)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_authenticator__assign_realm_and_nonce(authenticator, realm, nonce);
	livemedia_authenticator__assign_username_and_password(authenticator, username,
			password, password_is_md5);
}
