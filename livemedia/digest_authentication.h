#ifndef __LIVEMEDIA_DIGEST_AUTHENTICATION_H__
#define __LIVEMEDIA_DIGEST_AUTHENTICATION_H__
#include <stdbool.h>

/*****************************************************************
 * Authenticator: (Structure Declarations)
 ****************************************************************/
/* A class used for digest authentication.
 * The "realm", and "nonce" fields are supplied by the server
 * (in a "401 Unauthorized" response).
 * The "username" and "password" fields are supplied by the client. */
typedef struct _livemedia_authenticator_t {
	char *realm;
	char *nonce;
	char *username;
	char *password;
	bool password_is_md5;

	/* virtual function pointers */
	void (*delete)(struct _livemedia_authenticator_t *authenticator);

} livemedia_authenticator_t;

/*****************************************************************
 * Authenticator: (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */

livemedia_authenticator_t *__livemedia_authenticator__alloc(void);

void __livemedia_authenticator__init(livemedia_authenticator_t *authenticator);
void __livemedia_authenticator__init_by_username_and_password(livemedia_authenticator_t *authenticator,
		char const *username, char const *password, bool password_is_md5 /* default: false */);
void __livemedia_authenticator__init_by_authenticator(livemedia_authenticator_t *authenticator,
		livemedia_authenticator_t *orig);

void __livemedia_authenticator__deinit(livemedia_authenticator_t *authenticator);
void __livemedia_authenticator__free(livemedia_authenticator_t *authenticator);
/*
 * new() and delete() functions
 */
livemedia_authenticator_t *livemedia__new__authenticator(void);
livemedia_authenticator_t *livemedia__new__authenticator_by_username_and_password(
		char const *username, char const *password, bool password_is_md5 /* default: false */);
livemedia_authenticator_t *livemedia__new__authenticator_by_authenticator(
		livemedia_authenticator_t *orig);

void livemedia__delete__authenticator(livemedia_authenticator_t *authenticator);
/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_authenticator__delete__impl(livemedia_authenticator_t *authenticator);

/*
 * Functions in header file
 */
char const *livemedia_authenticator__realm(livemedia_authenticator_t *authenticator);
char const *livemedia_authenticator__nonce(livemedia_authenticator_t *authenticator);
char const *livemedia_authenticator__username(livemedia_authenticator_t *authenticator);
char const *livemedia_authenticator__password(livemedia_authenticator_t *authenticator);

/*
 * Normal functions
 */
void livemedia_authenticator__reset(livemedia_authenticator_t *authenticator);

void livemedia_authenticator__set_realm_and_nonce(livemedia_authenticator_t *authenticator,
		char const *realm, char const *nonce);

void livemedia_authenticator__set_realm_and_random_nonce(livemedia_authenticator_t *authenticator,
		char const *realm);
      /* as above, except that the nonce is created randomly.
       * (This is used by servers.) */

void livemedia_authenticator__set_username_and_password(livemedia_authenticator_t *authenticator,
		char const *username, char const *password, bool password_is_md5 /* default: false */);
      /* If "passwordIsMD5" is True, then "password" is actually the value computed
       * by md5(<username>:<realm>:<actual-password>) */

char const *livemedia_authenticator__compute_digest_response(livemedia_authenticator_t *authenticator,
		char const *cmd, char const *url);
      /* The returned string from this function must later be freed by calling: */

void livemedia_authenticator__reclaim_digest_response(livemedia_authenticator_t *authenticator,
		char const *response_str);

void livemedia_authenticator__reset_realm_and_nonce(livemedia_authenticator_t *authenticator);

void livemedia_authenticator__reset_username_and_password(livemedia_authenticator_t *authenticator);

void livemedia_authenticator__assign_realm_and_nonce(livemedia_authenticator_t *authenticator,
		char const *realm, char const *nonce);

void livemedia_authenticator__assign_username_and_password(livemedia_authenticator_t *authenticator,
	 char const *username, char const *password, bool password_is_md5);

void livemedia_authenticator__assign(livemedia_authenticator_t *authenticator,
		char const *realm, char const *nonce,
		char const *username, char const *password, bool password_is_md5);
#endif
