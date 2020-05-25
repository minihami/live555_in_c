#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

/*****************************************************************
 * HashTable (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_hash_table_t *__livemedia_hash_table__alloc(void)
{
	livemedia_hash_table_t *hash_table;

	hash_table = (livemedia_hash_table_t *)calloc(1, sizeof(livemedia_hash_table_t));
	if (hash_table == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_hash_table_t)) failed in %s()\n", 
				__func__);
		return NULL;
	}

	return hash_table;
}
void __livemedia_hash_table__init(livemedia_hash_table_t *hash_table)
{
	if (hash_table) {
		hash_table->add = NULL;
		hash_table->remove = NULL;
		hash_table->lookup = NULL;
		hash_table->num_entries = NULL;
		hash_table->delete = livemedia_hash_table__delete__impl;
	}
}

void __livemedia_hash_table__deinit(livemedia_hash_table_t *hash_table)
{
}

void __livemedia_hash_table__free(livemedia_hash_table_t *hash_table)
{
	if (hash_table) {
		free(hash_table);
		hash_table = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_hash_table_t *livemedia__new__hash_table(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_hash_table_t *hash_table;

	hash_table = __livemedia_hash_table__alloc();
	if (hash_table)
		__livemedia_hash_table__init(hash_table);

	return hash_table;
}

void livemedia__delete__hash_table(livemedia_hash_table_t *hash_table)
{
	if (hash_table->delete)
		hash_table->delete(hash_table);
}

/*
 * Functions for virtual functions
 */
void *livemedia_hash_table__add(livemedia_hash_table_t *hash_table, char const *key, 
		void *value)
{
	if (hash_table->add)
		return hash_table->add(hash_table, key, value);
	return NULL;
}

bool livemedia_hash_table__remove(livemedia_hash_table_t *hash_table, char const *key)
{
	if (hash_table->remove)
	{
		printf("key: %s entry removed!\n", key);
		return hash_table->remove(hash_table, key);
	}
	return false;
}

void *livemedia_hash_table__lookup(livemedia_hash_table_t *hash_table, char const *key)
{
	if (hash_table->lookup)
		return hash_table->lookup(hash_table, key);
	return NULL;
}

unsigned int livemedia_hash_table__num_entries(livemedia_hash_table_t *hash_table)
{
	if (hash_table->num_entries)
		return hash_table->num_entries(hash_table);
	return 0;
}

/*
 * Override functions
 */
void livemedia_hash_table__delete__impl(livemedia_hash_table_t *hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (hash_table) {
		__livemedia_hash_table__deinit(hash_table);
		__livemedia_hash_table__free(hash_table);
	}
}

/*
 * Functions in header file
 */
bool livemedia_hash_table__is_empty(livemedia_hash_table_t *hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (livemedia_hash_table__num_entries(hash_table) == 0);
}

/*
 * Normal functions
 */
void *livemedia_hash_table__remove_next(livemedia_hash_table_t *hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_hash_table_iterator_t *iter;
	char const *key;
	void *removed_value;

	iter = livemedia_hash_table_iterator__create__static(hash_table);
	removed_value = livemedia_hash_table_iterator__next(iter, &key);
	if (removed_value != 0)
		livemedia_hash_table__remove(hash_table, key);

	livemedia__delete__hash_table_iterator(iter);
	return removed_value;
}

void *livemedia_hash_table__get_first(livemedia_hash_table_t *hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_hash_table_iterator_t *iter;
	char const *key;
	void *first_value;

	iter = livemedia_hash_table_iterator__create__static(hash_table);
	first_value = livemedia_hash_table_iterator__next(iter, &key);

	livemedia__delete__hash_table_iterator(iter);
	return first_value;
}

/*****************************************************************
 * HashTable::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_hash_table_iterator_t *__livemedia_hash_table_iterator__alloc(void)
{
	livemedia_hash_table_iterator_t *hash_table_iterator;
	hash_table_iterator = (livemedia_hash_table_iterator_t *)calloc(1, sizeof(livemedia_hash_table_iterator_t));
	if (hash_table_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_hash_table_iterator_t)) failed in" 
				" %s()\n", __func__);
		return NULL;
	}

	return hash_table_iterator;
}
void __livemedia_hash_table_iterator__init(
		livemedia_hash_table_iterator_t *hash_table_iterator)
{
	if (hash_table_iterator) {
		hash_table_iterator->delete = livemedia_hash_table_iterator__delete__impl;
		hash_table_iterator->next = NULL;
	}
}
void __livemedia_hash_table_iterator__deinit(
		livemedia_hash_table_iterator_t *hash_table_iterator)
{
}
void __livemedia_hash_table_iterator__free(
		livemedia_hash_table_iterator_t *hash_table_iterator)
{
	if (hash_table_iterator) {
		free(hash_table_iterator);
		hash_table_iterator = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_hash_table_iterator_t *livemedia__new__hash_table_iterator(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_hash_table_iterator_t *hash_table_iterator;

	hash_table_iterator = __livemedia_hash_table_iterator__alloc();
	if (hash_table_iterator)
		__livemedia_hash_table_iterator__init(hash_table_iterator);

	return hash_table_iterator;
}
void livemedia__delete__hash_table_iterator(
		livemedia_hash_table_iterator_t *hash_table_iterator)
{
	if (hash_table_iterator->delete)
		hash_table_iterator->delete(
				(livemedia_hash_table_iterator_t*)hash_table_iterator);
}

/*
 * Functions for virtual functions
 */
void *livemedia_hash_table_iterator__next(
		livemedia_hash_table_iterator_t *hash_table_iterator, char const **key)
{
	if (hash_table_iterator->next)
		return hash_table_iterator->next(hash_table_iterator, key);
	return NULL;
}

/*
 * Override functions
 */
void livemedia_hash_table_iterator__delete__impl(
		livemedia_hash_table_iterator_t *hash_table_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (hash_table_iterator) {
		__livemedia_hash_table_iterator__deinit(hash_table_iterator);
		__livemedia_hash_table_iterator__free(hash_table_iterator);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
