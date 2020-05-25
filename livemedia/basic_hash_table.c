#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basic_hash_table.h"
#include "str_dup.h"
#ifdef WEBGATE_DEBUG
#undef WEBGATE_DEBUG
#endif

/* When there are this many entries per bucket, on average, rebuild
 * the table to increase the number of buckets */
#define REBUILD_MULTIPLIER	3

/*****************************************************************
 * BasicHashTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_basic_hash_table_t *__livemedia_basic_hash_table__alloc(void)
{
	livemedia_basic_hash_table_t *basic_hash_table;

	basic_hash_table = (livemedia_basic_hash_table_t *)calloc(1, sizeof(livemedia_basic_hash_table_t));
	if (basic_hash_table == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_basic_hash_table_t)) failed in %s()\n", __func__);
		return NULL;
	}

	return basic_hash_table;
}
void __livemedia_basic_hash_table__init(livemedia_basic_hash_table_t *basic_hash_table,
		int key_type)
{
	unsigned int i;

	__livemedia_hash_table__init(&basic_hash_table->hash_table);

	basic_hash_table->buckets = basic_hash_table->static_buckets;
	basic_hash_table->num_buckets = SMALL_HASH_TABLE_SIZE;
	basic_hash_table->num_entries = 0;
	basic_hash_table->rebuild_size = SMALL_HASH_TABLE_SIZE * REBUILD_MULTIPLIER;
	basic_hash_table->down_shift = 28;
	basic_hash_table->mask = 0x3;
	basic_hash_table->key_type = key_type;

	for (i = 0; i < SMALL_HASH_TABLE_SIZE; ++i)
		basic_hash_table->static_buckets[i] = NULL;

	basic_hash_table->hash_table.add = livemedia_basic_hash_table__add__impl;
	basic_hash_table->hash_table.remove = livemedia_basic_hash_table__remove__impl;
	basic_hash_table->hash_table.lookup = livemedia_basic_hash_table__lookup__impl;
	basic_hash_table->hash_table.num_entries =
		livemedia_basic_hash_table__num_entries__impl;
	basic_hash_table->hash_table.delete = livemedia_basic_hash_table__delete__impl;
}

void __livemedia_basic_hash_table__deinit(livemedia_basic_hash_table_t *basic_hash_table)
{
	unsigned int i;
	livemedia_basic_hash_table_table_entry_t *entry;

	/* Free all the entries in the table: */
	for (i = 0; i < basic_hash_table->num_buckets; ++i) 
		while ((entry = basic_hash_table->buckets[i]) != NULL) 
			livemedia_basic_hash_table__delete_entry(basic_hash_table, i, entry);
	

	/* Also free the bucket array, if it was dynamically allocated: */
	if (basic_hash_table->buckets != basic_hash_table->static_buckets) {
		free(basic_hash_table->buckets);
		basic_hash_table->buckets = NULL;
	}

	__livemedia_hash_table__deinit(&basic_hash_table->hash_table);
}

void __livemedia_basic_hash_table__free(livemedia_basic_hash_table_t *basic_hash_table)
{
	if (basic_hash_table) {
		free(basic_hash_table);
		basic_hash_table = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_basic_hash_table_t *livemedia__new__basic_hash_table(int key_type)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_t *basic_hash_table;

	basic_hash_table = __livemedia_basic_hash_table__alloc();
	if (basic_hash_table)
		__livemedia_basic_hash_table__init(basic_hash_table, key_type);

	return basic_hash_table;
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void *livemedia_basic_hash_table__add__impl(livemedia_hash_table_t *hash_table, 
		char const *key, void *value)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_t *basic_hash_table;
	void *old_value;
	unsigned int index;
	livemedia_basic_hash_table_table_entry_t *entry;

	basic_hash_table = (livemedia_basic_hash_table_t *)hash_table;
	entry = livemedia_basic_hash_table__lookup_key(basic_hash_table, key, &index);
	if (entry != NULL) {
		/* There's already an item with this key */
		old_value = entry->value;
	} else {
		/* There's no existing entry; create a new one: */
		entry = livemedia_basic_hash_table__insert_new_entry(basic_hash_table, index, 
				key);
		old_value = NULL;
	}
	entry->value = value;

	/* If the table has become too large, rebuild it with more buckets: */
	if (basic_hash_table->num_entries >= basic_hash_table->rebuild_size)
		livemedia_basic_hash_table__rebuild(basic_hash_table);

	return old_value;
}
bool livemedia_basic_hash_table__remove__impl(livemedia_hash_table_t *hash_table, 
		char const *key)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_t *basic_hash_table;
	unsigned int index;
	livemedia_basic_hash_table_table_entry_t *entry;

	basic_hash_table = (livemedia_basic_hash_table_t *)hash_table;
	entry = livemedia_basic_hash_table__lookup_key(basic_hash_table, key, &index);
	if (entry == NULL)
		return false; /* no such entry */

	livemedia_basic_hash_table__delete_entry(basic_hash_table, index, entry);

	return true;
}
void *livemedia_basic_hash_table__lookup__impl(livemedia_hash_table_t *hash_table,
		char const *key)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_t *basic_hash_table;
	unsigned int index;
	livemedia_basic_hash_table_table_entry_t *entry;

	basic_hash_table = (livemedia_basic_hash_table_t *)hash_table;
	entry = livemedia_basic_hash_table__lookup_key(basic_hash_table, key, &index);
	if (entry == NULL)
		return NULL; /* no such entry */

	return entry->value;
}
unsigned int livemedia_basic_hash_table__num_entries__impl(
		livemedia_hash_table_t *hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_t *basic_hash_table;

	basic_hash_table = (livemedia_basic_hash_table_t *)hash_table;

	return basic_hash_table->num_entries;
}
void livemedia_basic_hash_table__delete__impl(livemedia_hash_table_t *hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_t *basic_hash_table;

	basic_hash_table = (livemedia_basic_hash_table_t *)hash_table;
	if (basic_hash_table) {
		__livemedia_basic_hash_table__deinit(basic_hash_table);
		__livemedia_basic_hash_table__free(basic_hash_table);
	}
}
/*
 * Functions in header file
 */
unsigned int livemedia_basic_hash_table__random_index(
		livemedia_basic_hash_table_t *basic_hash_table, uintptr_t i)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	return (unsigned int)(((i * 1103515245) >> basic_hash_table->down_shift) & 
			basic_hash_table->mask);
}

/*
 * Normal functions
 */
livemedia_basic_hash_table_table_entry_t *livemedia_basic_hash_table__lookup_key(
		livemedia_basic_hash_table_t *basic_hash_table, char const *key, 
		unsigned int *index)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_table_entry_t *entry;

	*index = livemedia_basic_hash_table__hash_index_from_key(basic_hash_table, key);

	for (entry = basic_hash_table->buckets[*index]; entry != NULL; entry = entry->next) 
		if (livemedia_basic_hash_table__key_matches(basic_hash_table, key, entry->key))
			break;

	return entry;
}
bool livemedia_basic_hash_table__key_matches(
		livemedia_basic_hash_table_t *basic_hash_table, char const *key1, 
		char const *key2)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int *k1;
	unsigned int *k2;
	int i;

	/* The way we check the keys for a match depends upon their type: */
	if (basic_hash_table->key_type == STRING_HASH_KEYS) {
		return (strcmp(key1, key2) == 0);
	} else if (basic_hash_table->key_type == ONE_WORD_HASH_KEYS) {
		return (key1 == key2);
	} else {
		k1 = (unsigned int *)key1;
		k2 = (unsigned int *)key2;

		for (i = 0; i < basic_hash_table->key_type; ++i)
			if (k1[i] != k2[i])
				return false; /* keys differ */
		return true;
	}
}
livemedia_basic_hash_table_table_entry_t *livemedia_basic_hash_table__insert_new_entry(
		livemedia_basic_hash_table_t *basic_hash_table, unsigned index, char const *key)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_table_entry_t *entry;

	entry = livemedia__new__basic_hash_table_table_entry();
	entry->next = basic_hash_table->buckets[index];
	basic_hash_table->buckets[index] = entry;

	++basic_hash_table->num_entries;
	livemedia_basic_hash_table__assign_key(basic_hash_table, entry, key);

	return entry;
}
void livemedia_basic_hash_table__assign_key(
		livemedia_basic_hash_table_t *basic_hash_table, 
		livemedia_basic_hash_table_table_entry_t *entry, char const *key)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int *key_from;
	unsigned int *key_to;
	int i;

	/* The way we assign the key depends upon its type: */
	if (basic_hash_table->key_type == STRING_HASH_KEYS) {
		entry->key = livemedia__str_dup(key);
	} else if (basic_hash_table->key_type == ONE_WORD_HASH_KEYS) {
		entry->key = key;
	} else if (basic_hash_table->key_type > 0) {
		key_from = (unsigned int *)key;
		key_to = (unsigned int *)malloc(sizeof(unsigned int) * basic_hash_table->key_type);
		if (key_to == NULL) {
			fprintf(stderr, "malloc(sizeof(unsigned int) * basic_hash_table->key_type)"
					" failed in %s()\n", __func__);
		}
		for (i = 0; i < basic_hash_table->key_type; ++i)
			key_to[i] = key_from[i];

		entry->key = (char const *)key_to;
	}
}
void livemedia_basic_hash_table__delete_entry(
		livemedia_basic_hash_table_t *basic_hash_table, unsigned int index, 
		livemedia_basic_hash_table_table_entry_t *entry)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_table_entry_t **ep;
	bool found_it;

	ep = &basic_hash_table->buckets[index];

	found_it = false;
	while (*ep != NULL) {
		if (*ep == entry) {
			found_it = true;
			*ep = entry->next;
			break;
		}
		ep = &((*ep)->next);
	}

	if (!found_it) { /* shouldn't happen */
#ifdef DEBUG
		fprintf(stderr, "BasicHashTable[%p]::deleteEntry(%d,%p): internal error - not found (first entry %p", basic_hash_table, index, entry, basic_hash_table->buckets[index]);
		if (basic_hash_table->buckets[index] != NULL) fprintf(stderr, ", next entry %p", basic_hash_table->buckets[index]->next);
		fprintf(stderr, ")\n");
#endif
	}

	--basic_hash_table->num_entries;
	livemedia_basic_hash_table__delete_key(basic_hash_table, entry);
	livemedia__delete__basic_hash_table_table_entry(entry);
}
void livemedia_basic_hash_table__delete_key(
		livemedia_basic_hash_table_t *basic_hash_table, 
		livemedia_basic_hash_table_table_entry_t *entry)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* The way we delete the key depends upon its type: */
	if (basic_hash_table->key_type == ONE_WORD_HASH_KEYS) {
		entry->key = NULL;
	} else {
		free((char *)entry->key);
		entry->key = NULL;
	}
}
void livemedia_basic_hash_table__rebuild(livemedia_basic_hash_table_t *basic_hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	/* Remember the existing table size: */
	unsigned int old_size;
	livemedia_basic_hash_table_table_entry_t **old_buckets;
	livemedia_basic_hash_table_table_entry_t **old_chain_ptr;
	livemedia_basic_hash_table_table_entry_t *h_ptr;
	unsigned int index;
	unsigned int i;

	old_size = basic_hash_table->num_buckets;
	old_buckets = basic_hash_table->buckets;

	/* Create the new sized table: */
	basic_hash_table->num_buckets *= 4;
	basic_hash_table->buckets = (livemedia_basic_hash_table_table_entry_t **)malloc(sizeof(livemedia_basic_hash_table_table_entry_t *) *
			basic_hash_table->num_buckets);

	if (basic_hash_table->buckets == NULL) {
		fprintf(stderr, "malloc(sizeof(livemedia_basic_hash_table_table_entry_t *) *"
				" basic_hash_table->num_buckets) failed in %s()\n", __func__);
	}
	for (i = 0; i < basic_hash_table->num_buckets; ++i) 
		basic_hash_table->buckets[i] = NULL;
	basic_hash_table->rebuild_size *= 4;
	basic_hash_table->down_shift -= 2;
	basic_hash_table->mask = (basic_hash_table->mask << 2) | 0x3;

	/* Rehash the existing entries into the new table: */
	for (old_chain_ptr = old_buckets; old_size > 0; --old_size, ++old_chain_ptr) {
		for (h_ptr = *old_chain_ptr; h_ptr != NULL; h_ptr = *old_chain_ptr) {
			*old_chain_ptr = h_ptr->next;

			index = livemedia_basic_hash_table__hash_index_from_key(basic_hash_table, 
					h_ptr->key);

			h_ptr->next = basic_hash_table->buckets[index];
			basic_hash_table->buckets[index] = h_ptr;
		}
	}

	/* Free the old bucket array, if it was dynamically allocated: */
	if (old_buckets != basic_hash_table->static_buckets) {
		free(old_buckets);
		old_buckets = NULL;
	}
}
unsigned int livemedia_basic_hash_table__hash_index_from_key(
		livemedia_basic_hash_table_t *basic_hash_table, char const *key)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	unsigned int result;
	unsigned int *k;
	uintptr_t sum;
	int i;
	char c;

	result = 0;

	if (basic_hash_table->key_type == STRING_HASH_KEYS) {
		while (1) {
			c = *key++;
			if (c == 0)
				break;
			result += (result << 3) + (unsigned int)c;
		}
		result &= basic_hash_table->mask;
	} else if (basic_hash_table->key_type == ONE_WORD_HASH_KEYS) {
		result = livemedia_basic_hash_table__random_index(basic_hash_table, 
				(uintptr_t)key);
	} else {
		k = (unsigned int *)key;
		sum = 0;
		for (i = 0; i< basic_hash_table->key_type; ++i)
			sum += k[i];
		
		result = livemedia_basic_hash_table__random_index(basic_hash_table, sum);
	}

	return result;
}

/*
 * Static functions implementation for super class
 */
livemedia_hash_table_t *livemedia_hash_table__create__static(int key_type)
{
	return (livemedia_hash_table_t *)livemedia__new__basic_hash_table(key_type);
}
/*****************************************************************
 * BasicHashTable::Iterator (Function Definitions)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_basic_hash_table_iterator_t *__livemedia_basic_hash_table_iterator__alloc(void)
{
	livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator;

	basic_hash_table_iterator = (livemedia_basic_hash_table_iterator_t *)calloc(1, sizeof(livemedia_basic_hash_table_iterator_t));
	if (basic_hash_table_iterator == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_basic_hash_table_iterator_t)) failed"
				" in %s()\n", __func__);
		return NULL;
	}

	return basic_hash_table_iterator;
}
void __livemedia_basic_hash_table_iterator__init(
		livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator, 
		livemedia_basic_hash_table_t *table)
{
	__livemedia_hash_table_iterator__init(&basic_hash_table_iterator->hash_table_iterator);

	basic_hash_table_iterator->table = table;
	basic_hash_table_iterator->next_index = 0;
	basic_hash_table_iterator->next_entry = NULL;

	basic_hash_table_iterator->hash_table_iterator.next = livemedia_basic_hash_table_iterator__next__impl;
	basic_hash_table_iterator->hash_table_iterator.delete = livemedia_basic_hash_table_iterator__delete__impl;
}
void __livemedia_basic_hash_table_iterator__deinit(
		livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator)
{
	__livemedia_hash_table_iterator__deinit(
			&basic_hash_table_iterator->hash_table_iterator);
}
void __livemedia_basic_hash_table_iterator__free(
		livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator)
{
	if (basic_hash_table_iterator) {
		free(basic_hash_table_iterator);
		basic_hash_table_iterator = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_basic_hash_table_iterator_t *livemedia__new__basic_hash_table_iterator(
		livemedia_basic_hash_table_t *basic_hash_table)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator;

	basic_hash_table_iterator = __livemedia_basic_hash_table_iterator__alloc();
	if (basic_hash_table_iterator)
		__livemedia_basic_hash_table_iterator__init(basic_hash_table_iterator, 
				basic_hash_table);

	return basic_hash_table_iterator;
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_basic_hash_table_iterator__delete__impl(
		livemedia_hash_table_iterator_t *hash_table_iterator)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator;

	basic_hash_table_iterator = 
		(livemedia_basic_hash_table_iterator_t *)hash_table_iterator;
	
	if (basic_hash_table_iterator) {
		__livemedia_basic_hash_table_iterator__deinit(basic_hash_table_iterator);
		__livemedia_basic_hash_table_iterator__free(basic_hash_table_iterator);
	}
}
void *livemedia_basic_hash_table_iterator__next__impl(
		livemedia_hash_table_iterator_t *hash_table_iterator, char const **key)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator;
	livemedia_basic_hash_table_table_entry_t *entry;	

	basic_hash_table_iterator = 
		(livemedia_basic_hash_table_iterator_t *)hash_table_iterator;

	while (basic_hash_table_iterator->next_entry == NULL) {
		if (basic_hash_table_iterator->next_index >= 
				basic_hash_table_iterator->table->num_buckets)
			return NULL;

		basic_hash_table_iterator->next_entry = 
			basic_hash_table_iterator->table->buckets[basic_hash_table_iterator->next_index++];
	}

	entry = basic_hash_table_iterator->next_entry;
	basic_hash_table_iterator->next_entry = entry->next;

	*key = entry->key;
	return entry->value;
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*
 * Static functions implementation for super class
 */
livemedia_hash_table_iterator_t *livemedia_hash_table_iterator__create__static(
		livemedia_hash_table_t *hash_table)
{
	/* "hashTable" is assumed to be a BasicHashTable */
	return (livemedia_hash_table_iterator_t *)livemedia__new__basic_hash_table_iterator(
			(livemedia_basic_hash_table_t *)hash_table);
}

/*****************************************************************
 * BasicHashTable::TableEntry (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_basic_hash_table_table_entry_t 
*__livemedia_basic_hash_table_table_entry__alloc(void)
{
	livemedia_basic_hash_table_table_entry_t* basic_hash_table_table_entry;

	basic_hash_table_table_entry = (livemedia_basic_hash_table_table_entry_t *)calloc(1,
			sizeof(livemedia_basic_hash_table_table_entry_t));
	if (basic_hash_table_table_entry == NULL) {
		fprintf(stderr, "calloc(1, sizeof(livemedia_basic_hash_table_table_entry_t))"
				" failed in %s()\n", __func__);
		return NULL;
	}

	return basic_hash_table_table_entry;
}
void __livemedia_basic_hash_table_table_entry__init(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry)
{
	if (basic_hash_table_table_entry)
		basic_hash_table_table_entry->delete = 
			livemedia_basic_hash_table_table_entry__delete__impl;
}
void __livemedia_basic_hash_table_table_entry__deinit(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry)
{
}
void __livemedia_basic_hash_table_table_entry__free(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry)
{
	if (basic_hash_table_table_entry) {
		free(basic_hash_table_table_entry);
		basic_hash_table_table_entry = NULL;
	}
}

/*
 * new() and delete() functions
 */
livemedia_basic_hash_table_table_entry_t *livemedia__new__basic_hash_table_table_entry(void)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry;

	basic_hash_table_table_entry = __livemedia_basic_hash_table_table_entry__alloc();
	if (basic_hash_table_table_entry)
		__livemedia_basic_hash_table_table_entry__init(basic_hash_table_table_entry);

	return basic_hash_table_table_entry;
}
void livemedia__delete__basic_hash_table_table_entry(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry)
{
	if (basic_hash_table_table_entry) 
		basic_hash_table_table_entry->delete(basic_hash_table_table_entry);
}
#define TABLE_ENTRY_END_MARKER 0x00000000
livemedia_basic_hash_table_table_entry_t *livemedia__new__basic_hash_table_table_entry_array(
		unsigned int size)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	int i;
	
	livemedia_basic_hash_table_table_entry_t *entry_array;
	entry_array = (livemedia_basic_hash_table_table_entry_t *)malloc(sizeof(livemedia_basic_hash_table_table_entry_t) * size + sizeof(uint32_t *));
	for (i = 0; i < size; ++i) 
		__livemedia_basic_hash_table_table_entry__init(&entry_array[i]);
	//(uint32_t *)entry_array[i] = TABLE_ENTRY_END_MARKER;
	*((uint32_t * )(entry_array + i)) = TABLE_ENTRY_END_MARKER;
	return entry_array;

}
void livemedia__delete__basic_hash_table_table_entry_array( 
		livemedia_basic_hash_table_table_entry_t *entry_array)
{
	int i;
	for (i = 0; ; ++i) {
		if (*((uint32_t *)(entry_array + i)) == TABLE_ENTRY_END_MARKER)
			break;
		__livemedia_basic_hash_table_table_entry__deinit(&entry_array[i]);
	}
	free(entry_array);
}

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_basic_hash_table_table_entry__delete__impl(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry)
{
#ifdef WEBGATE_DEBUG
	printf("LOG: %s() in %s: %d line\n", __func__, __FILE__, __LINE__);
#endif
	if (basic_hash_table_table_entry) {
		__livemedia_basic_hash_table_table_entry__deinit(basic_hash_table_table_entry);
		__livemedia_basic_hash_table_table_entry__free(basic_hash_table_table_entry);
	}
}

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
