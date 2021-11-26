/*
  hashtable

  // TODO

  REFERENCES
  - (article) https://benhoyt.com/writings/hash-table-in-c/, Ben Hoyt
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define DEBUG 1

typedef struct {
	const char* key;
	void *value;
} entry_t;

// struct to maintain a hashtable (entries)
typedef struct {
	entry_t* entries;
        int capacity;
	int size;
} hash_t;

#define INITIAL_CAPACITY 32;

// private in the actual implementation
uint64_t hash_key(const char* key);

// public
int hash_size(hash_t* table);
hash_t* hash_create(void);
int hash_destroy(hash_t* table);
void* hash_search(hash_t* table, const char* key);
const char* hash_insert(hash_t* table, const char* key, void* value);


#endif /* HASHTABLE_H */
