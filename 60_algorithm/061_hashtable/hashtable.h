/*
  hashtable

  on base of an allocated memory with fixed size, similar to an array,
  values are placed basically in a +/- random order

  the hash function used by search guarantees direc O(1) access to the
  specific element (given a random access memory with direct access by
  address + offset i.e. index);

  NB: collisions can be allowed, avoided,... depending also on the
  hash function

  performance then depends on the performance of the hash algorithm,
  and/or the search function


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

// debug printing
//#define DEBUG 1

typedef struct {
	const uint64_t* key;
	void *value;
} entry_t;

// struct to maintain a hashtable (entries)
typedef struct {
	entry_t* entries;
        int capacity;
	int size;
} hash_t;

#define INITIAL_CAPACITY 32;

// private in the actual implementation (for testing)
uint64_t hash_key(const uint64_t* key);

// public
int hash_size(hash_t* table);
hash_t* hash_create(void);
int hash_destroy(hash_t* table);
void* hash_search(hash_t* table, const uint64_t* key);
const uint64_t* hash_insert(hash_t* table, const uint64_t* key, void* value);


#endif /* HASHTABLE_H */
