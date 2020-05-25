#ifndef __LIVEMEDIA_BASIC_HASH_TABLE_H__
#define __LIVEMEDIA_BASIC_HASH_TABLE_H__

#ifndef __LIVEMEDIA_HASH_TABLE_H__
#include "hash_table.h"
#endif
#include <stdint.h>

/* A simple hash table implementation, inspired by the hash table */
/* implementation used in Tcl 7.6: <http://www.tcl.tk/> */

#define SMALL_HASH_TABLE_SIZE	4

/*****************************************************************
 * BasicHashTable (Structure Definitions)
 ****************************************************************/
struct _livemedia_basic_hash_table_table_entry_t; /* forward */

typedef struct _livemedia_basic_hash_table_t {
	livemedia_hash_table_t hash_table;

	struct _livemedia_basic_hash_table_table_entry_t **buckets; /* pointer to bucket array */
	/* used for small tables */
	struct _livemedia_basic_hash_table_table_entry_t *static_buckets[SMALL_HASH_TABLE_SIZE];
	unsigned int num_buckets, num_entries, rebuild_size, down_shift, mask;
	int key_type;
} livemedia_basic_hash_table_t;

/*****************************************************************
 * BasicHashTable::Iterator  (Structure Definitions)
 ****************************************************************/
/* Used to iterate through the members of the table: */
typedef struct _livemedia_basic_hash_table_iterator_t {
	livemedia_hash_table_iterator_t hash_table_iterator;

	livemedia_basic_hash_table_t *table;
	unsigned int next_index; /* index of next bucket to be enumerated after this */
	struct _livemedia_basic_hash_table_table_entry_t *next_entry;
	/* next entry in the current bucket */
} livemedia_basic_hash_table_iterator_t;

/*****************************************************************
 * BasicHashTable::TableEntry  (Structure Definitions)
 ****************************************************************/
typedef struct _livemedia_basic_hash_table_table_entry_t {
	struct _livemedia_basic_hash_table_table_entry_t *next;
	char const *key;
	void *value;

	void (*delete)(
			struct _livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry);
} livemedia_basic_hash_table_table_entry_t;

/*****************************************************************
 * BasicHashTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_basic_hash_table_t *__livemedia_basic_hash_table__alloc(void);
void __livemedia_basic_hash_table__init(livemedia_basic_hash_table_t *basic_hash_table,
		int key_type);
void __livemedia_basic_hash_table__deinit(livemedia_basic_hash_table_t *basic_hash_table);
void __livemedia_basic_hash_table__free(livemedia_basic_hash_table_t *basic_hash_table);

/*
 * new() and delete() functions
 */
livemedia_basic_hash_table_t *livemedia__new__basic_hash_table(int key_type);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
/* implementation of inherited pure virtual functions */
void *livemedia_basic_hash_table__add__impl(livemedia_hash_table_t *hash_table,
		char const *key, void *value);
/* Returns the old value if different, otherwise 0 */
bool livemedia_basic_hash_table__remove__impl(livemedia_hash_table_t *hash_table,
		char const *key);
void *livemedia_basic_hash_table__lookup__impl(livemedia_hash_table_t *hash_table, 
		char const *key);
/* Returns 0 if not found */
unsigned int livemedia_basic_hash_table__num_entries__impl(
		livemedia_hash_table_t *hash_table);

void livemedia_basic_hash_table__delete__impl(livemedia_hash_table_t *hash_table);
/*
 * Functions in header file
 */
unsigned int livemedia_basic_hash_table__random_index(
		livemedia_basic_hash_table_t *basic_hash_table, uintptr_t i);

/*
 * Normal functions
 */
livemedia_basic_hash_table_table_entry_t *livemedia_basic_hash_table__lookup_key(
		livemedia_basic_hash_table_t *basic_hash_table, char const *key, unsigned int *index);
/* returns entry matching "key", or NULL if none */
bool livemedia_basic_hash_table__key_matches(
		livemedia_basic_hash_table_t *basic_hash_table, char const *key1, char const *key2);
/* used to implement "lookupKey()" */
livemedia_basic_hash_table_table_entry_t *livemedia_basic_hash_table__insert_new_entry(
		livemedia_basic_hash_table_t *basic_hash_table, unsigned int index, char const *key);
/* creates a new entry, and inserts it in the table */
void livemedia_basic_hash_table__assign_key(
		livemedia_basic_hash_table_t *basic_hash_table,
		livemedia_basic_hash_table_table_entry_t *entry, char const *key);
/* used to implement "insertNewEntry()" */
void livemedia_basic_hash_table__delete_entry(
		livemedia_basic_hash_table_t *basic_hash_table, unsigned int index,
		livemedia_basic_hash_table_table_entry_t *entry);
void livemedia_basic_hash_table__delete_key(
		livemedia_basic_hash_table_t *basic_hash_table, 
		livemedia_basic_hash_table_table_entry_t *entry);
/* used to implement "deleteEntry()" */
void livemedia_basic_hash_table__rebuild(livemedia_basic_hash_table_t *basic_hash_table);
/* rebuilds the table as its size increases */
unsigned int livemedia_basic_hash_table__hash_index_from_key(
		livemedia_basic_hash_table_t *basic_hash_table, char const *key);
/* used to implement many of the routines above */

/*****************************************************************
 * BasicHashTable::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_basic_hash_table_iterator_t *__livemedia_basic_hash_table_iterator__alloc(void);
void __livemedia_basic_hash_table_iterator__init(
		livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator,
		livemedia_basic_hash_table_t *table);
void __livemedia_basic_hash_table_iterator__deinit(
		livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator);
void __livemedia_basic_hash_table_iterator__free(
		livemedia_basic_hash_table_iterator_t *basic_hash_table_iterator);

/*
 * new() and delete() functions
 */
livemedia_basic_hash_table_iterator_t *livemedia__new__basic_hash_table_iterator(
		livemedia_basic_hash_table_t *basic_hash_table);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
/* implementation of ingerited pure virtual functions */
/* returns 0 if none */
void *livemedia_basic_hash_table_iterator__next__impl(
		livemedia_hash_table_iterator_t *hash_table_iterator, char const** key);
void livemedia_basic_hash_table_iterator__delete__impl(
		livemedia_hash_table_iterator_t *hash_table_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/*****************************************************************
 * BasicHashTable::TableEntry (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_basic_hash_table_table_entry_t *__livemedia_basic_hash_table_table_entry__alloc(void);
void __livemedia_basic_hash_table_table_entry__init(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry);
void __livemedia_basic_hash_table_table_entry__deinit(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry);
void __livemedia_basic_hash_table_table_entry__free(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry);

/*
 * new() and delete() functions
 */
livemedia_basic_hash_table_table_entry_t *livemedia__new__basic_hash_table_table_entry(void);
void livemedia__delete__basic_hash_table_table_entry(
		livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry);
livemedia_basic_hash_table_table_entry_t *livemedia__new__basic_hash_table_table_entry_array( 
		unsigned int size);
void livemedia__delete__basic_hash_table_table_entry_array( 
		livemedia_basic_hash_table_table_entry_t *entry_array);

/*
 * Functions for virtual functions
 */

/*
 * Override functions
 */
void livemedia_basic_hash_table_table_entry__delete__impl(livemedia_basic_hash_table_table_entry_t *basic_hash_table_table_entry);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */
#endif
