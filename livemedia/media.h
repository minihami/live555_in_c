#ifndef __LIVEMEDIA_MEDIA_H__
#define __LIVEMEDIA_MEDIA_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uv.h"
#ifndef __LIVEMEDIA_HASH_TABLE_H__
#include "hash_table.h"
#endif



#define LIVEMEDIA_MEDIUM_NAME_MAX_LEN	30



/*****************************************************************
 * Medium (Structure Definitions)
 ****************************************************************/
typedef struct _livemedia_medium_t {
	char medium_name[LIVEMEDIA_MEDIUM_NAME_MAX_LEN];
	uv_timer_t *next_task;

	/* Test for specific types of media: */
	bool (*is_source)(struct _livemedia_medium_t *);
	bool (*is_sink)(struct _livemedia_medium_t *);
	bool (*is_rtcp_instance)(struct _livemedia_medium_t *);
	bool (*is_rtsp_client)(struct _livemedia_medium_t *);
	bool (*is_rtsp_server)(struct _livemedia_medium_t *);
	bool (*is_media_session)(struct _livemedia_medium_t *);
	bool (*is_server_media_session)(struct _livemedia_medium_t *);
	void (*delete)(struct _livemedia_medium_t *);
} livemedia_medium_t;

/*****************************************************************
 * MediaLookupTable (Structure Definitions)
 ****************************************************************/
/* A data structure for looking up a Medium by its string name. */
/* (It is used only to implement "Medium", but we make it visible here, in case developers
 * want to use it to iterate over */
/* the whole set of "Medium" objects that we've created.) */
typedef struct _livemedia_media_lookup_table_t {
	livemedia_hash_table_t *table;
	unsigned int name_generator;

	void (*delete)(struct _livemedia_media_lookup_table_t *media_lookup_table);
} livemedia_media_lookup_table_t;

/*****************************************************************
 * _Tables (Structure Definitions)
 * The structure pointed to by the "liveMediaPriv" UsageEnvironment field:
 ****************************************************************/
typedef struct _livemedia_tables_t {
	livemedia_media_lookup_table_t *media_table;
	void *socket_table;

	void (*delete)(struct _livemedia_tables_t *tables);
} livemedia_tables_t;

/*****************************************************************
 * Medium (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_medium_t *__livemedia_medium__alloc(void);
void __livemedia_medium__init(livemedia_medium_t *medium);
void __livemedia_medium__deinit(livemedia_medium_t *medium);
void __livemedia_medium__free(livemedia_medium_t *medium);

/*
 * new() and delete() functions
 */
livemedia_medium_t *livemedia__new__medium(void);
void livemedia__delete__medium(livemedia_medium_t *medium);

/*
 * Functions for virtual functions
 */
bool livemedia_medium__is_source(livemedia_medium_t *medium);
bool livemedia_medium__is_sink(livemedia_medium_t *medium);
bool livemedia_medium__is_rtcp_instance(livemedia_medium_t *medium);
bool livemedia_medium__is_rtsp_client(livemedia_medium_t *medium);
bool livemedia_medium__is_rtsp_server(livemedia_medium_t *medium);
bool livemedia_medium__is_media_session(livemedia_medium_t *medium);
bool livemedia_medium__is_server_media_session(livemedia_medium_t *medium);

/*
 * Override functions
 */
bool livemedia_medium__is_source__impl(livemedia_medium_t *medium);
bool livemedia_medium__is_sink__impl(livemedia_medium_t *medium);
bool livemedia_medium__is_rtcp_instance__impl(livemedia_medium_t *medium);
bool livemedia_medium__is_rtsp_client__impl(livemedia_medium_t *medium);
bool livemedia_medium__is_rtsp_server__impl(livemedia_medium_t *medium);
bool livemedia_medium__is_media_session__impl(livemedia_medium_t *medium);
bool livemedia_medium__is_server_media_session__impl(livemedia_medium_t *medium);
void livemedia_medium__delete__impl(livemedia_medium_t *medium);

/*
 * Functions in header file
 */
char const* livemedia_medium__name(livemedia_medium_t *medium);

/*
 * Normal functions
 */
bool livemedia_medium__lookup_by_name__static(char const* medium_name,
		livemedia_medium_t **result_medium);
void livemedia_medium__close_by_medium_name__static(char const* medium_name);
void livemedia_medium__close_by_medium_ptr__static(livemedia_medium_t* medium);
/* alternate close() method using ptrs
 * (has no effect if medium == NULL) */

/*****************************************************************
 * MediaLookupTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_media_lookup_table_t *__livemedia_media_lookup_table__alloc(void);
void __livemedia_media_lookup_table__init(
		livemedia_media_lookup_table_t *media_lookup_table);
void __livemedia_media_lookup_table__deinit(
		livemedia_media_lookup_table_t *media_lookup_table);
void __livemedia_media_lookup_table__free(
		livemedia_media_lookup_table_t *media_lookup_table);

/*
 * new() and delete() functions
 */
livemedia_media_lookup_table_t *livemedia__new__media_lookup_table(void);
void livemedia__delete__media_lookup_table(
		livemedia_media_lookup_table_t *media_lookup_table);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_media_lookup_table__delete__impl(
		livemedia_media_lookup_table_t *media_lookup_table);

/*
 * Functions in header file
 */

livemedia_hash_table_t const *livemedia_media_lookup_table__get_table(
		livemedia_media_lookup_table_t *media_lookup_table);
/*
 * Normal functions
 */
livemedia_media_lookup_table_t *livemedia_media_lookup_table__our_media__static(void);
livemedia_medium_t *livemedia_media_lookup_table__lookup(
		livemedia_media_lookup_table_t *media_lookup_table, char const* name);
/* Returns NULL if none already exists */
void livemedia_media_lookup_table__add_new(
		livemedia_media_lookup_table_t* media_lookup_table, livemedia_medium_t* medium,
		char* medium_name);
void livemedia_media_lookup_table__remove(
		livemedia_media_lookup_table_t* media_lookup_table,	char const *name);
void livemedia_media_lookup_table__generate_new_name(
		livemedia_media_lookup_table_t* media_lookup_table, char *medium_name,
		unsigned int max_len);


/*****************************************************************
 * _Tables (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_tables_t *__livemedia_tables__alloc(void);
void __livemedia_tables__init(livemedia_tables_t *tables);
void __livemedia_tables__deinit(livemedia_tables_t *tables);
void __livemedia_tables__free(livemedia_tables_t *tables);

/*
 * new() and delete() functions
 */
livemedia_tables_t *livemedia__new__tables(void);
void livemedia__delete__tables(livemedia_tables_t *tables);


/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_tables__delete__impl(livemedia_tables_t *tables);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_tables_t* livemedia_tables__get_our_tables__static(bool create_if_not_present);
/* returns a pointer to a "_Tables" structure (creating it if necessary) */
void livemedia_tables__reclaim_if_possible(livemedia_tables_t *tables);
/* used to delete ourselves when we're no longer used */
#endif
