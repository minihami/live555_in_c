#ifndef __LIVEMEDIA_HASH_TABLE_H__
#define __LIVEMEDIA_HASH_TABLE_H__

#include <stdbool.h>

/* Warning: The following are deliberately the same as in */
/* Tcl's hash table implementation */
#define STRING_HASH_KEYS	0
#define ONE_WORD_HASH_KEYS	1


/*****************************************************************
 * HashTable (Structure Definitions)
 ****************************************************************/
typedef struct _livemedia_hash_table_t {
	void *(*add)(struct _livemedia_hash_table_t *hash_table, char const *key, 
			void *value);
	bool (*remove)(struct _livemedia_hash_table_t *hash_table, char const *key);
	void *(*lookup)(struct _livemedia_hash_table_t *hash_table, char const *key);
	unsigned int (*num_entries)(struct _livemedia_hash_table_t *hash_table);

	void (*delete)(struct _livemedia_hash_table_t *hash_table);
} livemedia_hash_table_t;

/*****************************************************************
 * HashTable::Iterator  (Structure Definitions)
 ****************************************************************/
/* Used to iterate through the members of the table: */
typedef struct _livemedia_hash_table_iterator_t {
	void (*delete)(struct _livemedia_hash_table_iterator_t *hash_table_iterator);
	void *(*next)(struct _livemedia_hash_table_iterator_t *hash_table_iterator,
			char const **key);
} livemedia_hash_table_iterator_t;

/*****************************************************************
 * HashTable (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_hash_table_t *__livemedia_hash_table__alloc(void);
void __livemedia_hash_table__init(livemedia_hash_table_t *hash_table);
void __livemedia_hash_table__deinit(livemedia_hash_table_t *hash_table);
void __livemedia_hash_table__free(livemedia_hash_table_t *hash_table);

/*
 * new() and delete() functions
 */
livemedia_hash_table_t *livemedia__new__hash_table(void);
void livemedia__delete__hash_table(livemedia_hash_table_t *hash_table);

/*
 * Functions for virtual functions
 */
void *livemedia_hash_table__add(livemedia_hash_table_t *hash_table, char const *key, 
		void *value);
/* Returns the old value if different, otherwise 0 */
bool livemedia_hash_table__remove(livemedia_hash_table_t *hash_table, char const *key);
void *livemedia_hash_table__lookup(livemedia_hash_table_t *hash_table, char const *key);
/* Returns 0 if not found */
unsigned int livemedia_hash_table__num_entries(livemedia_hash_table_t *hash_table);

/*
 * Override functions
 */
void livemedia_hash_table__delete__impl(livemedia_hash_table_t *hash_table);

/*
 * Functions in header file
 */
bool livemedia_hash_table__is_empty(livemedia_hash_table_t *hash_table);

/*
 * Normal functions
 */
/* A shortcut that can be used to successively remove each of 
 * the entries in the table (e.g., so that their values can be 
 * deleted, if they happen to be pointers to allocated memory). */
void *livemedia_hash_table__remove_next(livemedia_hash_table_t *hash_table);
/* Returns the first entry in the table.
 * (This is useful for deleting each entry in the table, if the entry's destructor also
 * removes itself from the table.)*/
void *livemedia_hash_table__get_first(livemedia_hash_table_t *hash_table);

/* 
 * The following must be implemented by a particular implementation (subclass):
 */
livemedia_hash_table_t *livemedia_hash_table__create__static(int key_type);

/*****************************************************************
 * HashTable::Iterator (Function Declarations)
 ****************************************************************/
/*
 * Internal Functions for new() and delete() functions
 */
livemedia_hash_table_iterator_t *__livemedia_hash_table_iterator__alloc(void);
void __livemedia_hash_table_iterator__init(
		livemedia_hash_table_iterator_t *hash_table_iterator);
void __livemedia_hash_table_iterator__deinit(
		livemedia_hash_table_iterator_t *hash_table_iterator);
void __livemedia_hash_table_iterator__free(
		livemedia_hash_table_iterator_t *hash_table_iterator);

/*
 * new() and delete() functions
 */
/* abstract base class */
livemedia_hash_table_iterator_t *livemedia__new__hash_table_iterator(void);
void livemedia__delete__hash_table_iterator(
		livemedia_hash_table_iterator_t *hash_table_iterator);

/*
 * Functions for virtual functions
 */
void *livemedia_hash_table_iterator__next(
		livemedia_hash_table_iterator_t *hash_table_iterator, char const **key);

/*
 * Override functions
 */
void livemedia_hash_table_iterator__delete__impl(
		livemedia_hash_table_iterator_t *hash_table_iterator);

/*
 * Functions in header file
 */

/*
 * Normal functions
 */

/* The following must be implemented by a particular 
 * implementation (subclass): */
livemedia_hash_table_iterator_t *livemedia_hash_table_iterator__create__static(
		livemedia_hash_table_t *hash_table);
#endif
