#include <stdio.h>
#include <stdlib.h>

#include "media.h"
#include "hash_table.h"

livemedia_tables_t *livemedia__global_tables;

/*****************************************************************
 * Medium (Function Definition)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() function
 */
livemedia_medium_t *__livemedia_medium__alloc(void)
{
	livemedia_medium_t *medium;
	medium = (livemedia_medium_t *)calloc(1, sizeof(livemedia_medium_t));
	if (medium == NULL) {
		fprintf(stderr, "calloc(sizeof(livemedia_medium_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return medium;
}
void __livemedia_medium__init(livemedia_medium_t *medium)
{
	livemedia_media_lookup_table_t *media_lookup_table;
	medium->next_task = (uv_timer_t *)calloc(1, sizeof(uv_timer_t));
	if (medium->next_task == NULL) {
		fprintf(stderr, "calloc(sizeof(uv_timer_t))"
				"failed in %s()\n", __func__);
		return;
	}
	uv_timer_init(uv_default_loop(), medium->next_task);
	/* First generate a name for the new medium: */
	media_lookup_table = livemedia_media_lookup_table__our_media__static();
	livemedia_media_lookup_table__generate_new_name(media_lookup_table,
			medium->medium_name, LIVEMEDIA_MEDIUM_NAME_MAX_LEN);

	/* Then add it to our table: */
	media_lookup_table = livemedia_media_lookup_table__our_media__static();
	livemedia_media_lookup_table__add_new(media_lookup_table, medium, 
		medium->medium_name);

	medium->is_source = livemedia_medium__is_source__impl;
	medium->is_sink = livemedia_medium__is_sink__impl;
	medium->is_rtcp_instance = livemedia_medium__is_rtcp_instance__impl;
	medium->is_rtsp_client = livemedia_medium__is_rtsp_client__impl;
	medium->is_rtsp_server = livemedia_medium__is_rtsp_server__impl;
	medium->is_media_session = livemedia_medium__is_media_session__impl;
	medium->is_server_media_session = livemedia_medium__is_server_media_session__impl;
	medium->delete = livemedia_medium__delete__impl;
}
static void on_close_cb(uv_handle_t *handle) {
	uv_timer_t *timer = (uv_timer_t *)handle;
	free(timer);
}
void __livemedia_medium__deinit(livemedia_medium_t *medium)
{
	if (medium->next_task) {
		uv_timer_stop(medium->next_task);
		uv_close((uv_handle_t *)medium->next_task, on_close_cb);
	}
}
void __livemedia_medium__free(livemedia_medium_t *medium)
{
	if (medium) {
		free(medium);
		medium = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_medium_t *livemedia__new__medium(void)
{
	livemedia_medium_t *medium;

	medium = __livemedia_medium__alloc();
	if (medium)
		__livemedia_medium__init(medium);
	return medium;
}
void livemedia__delete__medium(livemedia_medium_t *medium)
{
	if (medium)
		medium->delete(medium);
}

/*
 * Functions for virtual functions
 */
bool livemedia_medium__is_source(livemedia_medium_t *medium)
{
	if (medium->is_source)
		return medium->is_source(medium);
	return false;
}
bool livemedia_medium__is_sink(livemedia_medium_t *medium)
{
	if (medium->is_sink)
		return medium->is_sink(medium);
	return false;
}
bool livemedia_medium__is_rtcp_instance(livemedia_medium_t *medium)
{
	if (medium->is_rtcp_instance)
		return medium->is_rtcp_instance(medium);
	return false;
}
bool livemedia_medium__is_rtsp_client(livemedia_medium_t *medium)
{
	if (medium->is_rtsp_client)
		return medium->is_rtsp_client(medium);
	return false;
}
bool livemedia_medium__is_rtsp_server(livemedia_medium_t *medium)
{
	if (medium->is_rtsp_server)
		return medium->is_rtsp_server(medium);
	return false;
}
bool livemedia_medium__is_media_session(livemedia_medium_t *medium)
{
	if (medium->is_media_session)
		return medium->is_media_session(medium);
	return false;
}
bool livemedia_medium__is_server_media_session(livemedia_medium_t *medium)
{
	if (medium->is_server_media_session)
		return medium->is_server_media_session(medium);
	return false;
}

/*
 * Override functions
 */
bool livemedia_medium__is_source__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
bool livemedia_medium__is_sink__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
bool livemedia_medium__is_rtcp_instance__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
bool livemedia_medium__is_rtsp_client__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
bool livemedia_medium__is_rtsp_server__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
bool livemedia_medium__is_media_session__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
bool livemedia_medium__is_server_media_session__impl(livemedia_medium_t *medium)
{
	return false; /* default implementation */
}
void livemedia_medium__delete__impl(livemedia_medium_t *medium)
{
	if (medium) {
		__livemedia_medium__deinit(medium);
		__livemedia_medium__free(medium);
	}
}

/*
 * Functions in header file
 */
char const* livemedia_medium__name(livemedia_medium_t *medium)
{
	return medium->medium_name;
}

/*
 * Normal functions
 */
bool livemedia_medium__lookup_by_name__static(char const* medium_name,
		livemedia_medium_t **result_medium)
{
	livemedia_media_lookup_table_t *media_lookup_table;
	media_lookup_table = livemedia_media_lookup_table__our_media__static();
	*result_medium = livemedia_media_lookup_table__lookup(media_lookup_table, 
			medium_name);
	if (*result_medium == NULL) {
		fprintf(stderr, "Medium %s does not exist\n", medium_name);
		return false;
	}

	return true;
}
void livemedia_medium__close_by_medium_name__static(char const* medium_name)
{
	livemedia_media_lookup_table_t *media_lookup_table;
	media_lookup_table = livemedia_media_lookup_table__our_media__static();
	livemedia_media_lookup_table__remove(media_lookup_table, medium_name);
}
void livemedia_medium__close_by_medium_ptr__static(livemedia_medium_t* medium)
{
	if(medium == NULL)
		return;
	livemedia_medium__close_by_medium_name__static(livemedia_medium__name(medium));
}
/* alternate close() method using ptrs
 * (has no effect if medium == NULL) */

/*****************************************************************
 * MediaLookupTable (Function Definition)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_media_lookup_table_t *__livemedia_media_lookup_table__alloc(void)
{
	livemedia_media_lookup_table_t *media_lookup_table;
	media_lookup_table = (livemedia_media_lookup_table_t *)calloc(1,
			sizeof(livemedia_media_lookup_table_t));
	if (media_lookup_table == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_media_lookup_table_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return media_lookup_table;
}
void __livemedia_media_lookup_table__init(
		livemedia_media_lookup_table_t *media_lookup_table)
{
	media_lookup_table->table = livemedia_hash_table__create__static(STRING_HASH_KEYS);
	media_lookup_table->name_generator = 0;
}
void __livemedia_media_lookup_table__deinit(
		livemedia_media_lookup_table_t *media_lookup_table)
{
	if (media_lookup_table->table) 
		livemedia__delete__hash_table(media_lookup_table->table);
}
void __livemedia_media_lookup_table__free(
		livemedia_media_lookup_table_t *media_lookup_table)
{
	if (media_lookup_table) {
		free(media_lookup_table);
		media_lookup_table = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_media_lookup_table_t *livemedia__new__media_lookup_table(void)
{
	livemedia_media_lookup_table_t *media_lookup_table;
	media_lookup_table = __livemedia_media_lookup_table__alloc();
	if (media_lookup_table)
		__livemedia_media_lookup_table__init(media_lookup_table);
	return media_lookup_table;
}
void livemedia__delete__media_lookup_table(
		livemedia_media_lookup_table_t *media_lookup_table)
{
	if (media_lookup_table)
		media_lookup_table->delete(media_lookup_table);
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_media_lookup_table__delete__impl(
		livemedia_media_lookup_table_t *media_lookup_table)
{
	if (media_lookup_table) {
		__livemedia_media_lookup_table__deinit(media_lookup_table);
		__livemedia_media_lookup_table__free(media_lookup_table);
	}
}

/*
 * Functions in header file
 */

livemedia_hash_table_t const *livemedia_media_lookup_table__get_table(
		livemedia_media_lookup_table_t *media_lookup_table)
{
	return media_lookup_table->table;
}
/*
 * Normal functions
 */
livemedia_media_lookup_table_t *livemedia_media_lookup_table__our_media__static(void)
{
	livemedia_tables_t* our_tables;
	
	our_tables = livemedia_tables__get_our_tables__static(true);
	if (our_tables->media_table == NULL) {
    /* Create a new table to record the media that are to be created in 
     * this environment: */
		our_tables->media_table = livemedia__new__media_lookup_table();
	}
	return our_tables->media_table;
}
livemedia_medium_t *livemedia_media_lookup_table__lookup(
		livemedia_media_lookup_table_t *media_lookup_table, char const* name)
{
	return (livemedia_medium_t *)livemedia_hash_table__lookup(media_lookup_table->table,
			name);
}
void livemedia_media_lookup_table__add_new(
		livemedia_media_lookup_table_t* media_lookup_table, livemedia_medium_t* medium,
		char* medium_name)
{
	livemedia_hash_table__add(media_lookup_table->table, medium_name, (void *)medium);
}
void livemedia_media_lookup_table__remove(
		livemedia_media_lookup_table_t* media_lookup_table,	char const *name)
{
	livemedia_medium_t *medium;
	livemedia_tables_t *our_tables;

	medium = livemedia_media_lookup_table__lookup(media_lookup_table, name);
	if (medium != NULL) {
		livemedia_hash_table__remove(media_lookup_table->table, name);
		if (livemedia_hash_table__is_empty(media_lookup_table->table)) {
			/* We can also delete ourselves (to reclaim space): */
			our_tables = livemedia_tables__get_our_tables__static(true);
			livemedia__delete__media_lookup_table(media_lookup_table);
			our_tables->media_table = NULL;
			livemedia_tables__reclaim_if_possible(our_tables);
		}

		livemedia__delete__medium(medium);	
	}
}
void livemedia_media_lookup_table__generate_new_name(
		livemedia_media_lookup_table_t* media_lookup_table, char *medium_name,
		unsigned int max_len)
{
  /* We should really use snprintf() here, but not all systems have it */
  sprintf(medium_name, "livemedia%d", media_lookup_table->name_generator++);
}


/*****************************************************************
 * _Tables (Function Definition)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_tables_t *__livemedia_tables__alloc(void)
{
	livemedia_tables_t *tables;
	tables = (livemedia_tables_t *)calloc(1, sizeof(livemedia_tables_t));
	if (tables == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_tables_t))"
				"failed in %s()\n", __func__);
		return NULL;
	}
	return tables;
}
void __livemedia_tables__init(livemedia_tables_t *tables)
{
	tables->media_table = NULL;
	tables->socket_table = NULL;
}
void __livemedia_tables__deinit(livemedia_tables_t *tables)
{
}
void __livemedia_tables__free(livemedia_tables_t *tables)
{
	if (tables) {
		free(tables);
		tables = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_tables_t *livemedia__new__tables(void)
{
	livemedia_tables_t *tables;
	tables = __livemedia_tables__alloc();
	if (tables) 
		__livemedia_tables__init(tables);
	return tables;
}
void livemedia__delete__tables(livemedia_tables_t *tables)
{
	if (tables) 
		tables->delete(tables);
}


/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_tables__delete__impl(livemedia_tables_t *tables)
{
	if (tables) {
		__livemedia_tables__deinit(tables);
		__livemedia_tables__free(tables);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
livemedia_tables_t* livemedia_tables__get_our_tables__static(bool create_if_not_present)
{
	if (livemedia__global_tables == NULL && create_if_not_present)
		livemedia__global_tables = livemedia__new__tables();
	return livemedia__global_tables;
}
void livemedia_tables__reclaim_if_possible(livemedia_tables_t *tables)
{
	if (tables->media_table == NULL && tables->socket_table == NULL) {
		livemedia__global_tables = NULL;
		livemedia__delete__tables(tables);
	}
}
