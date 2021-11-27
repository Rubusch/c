/*
  hashtable

  on base of an allocated memory with fixed size, similar to an array,
  values are placed basically in a +/- random order

  the hash function used by search guarantees direc O(1) access to the
  specific element (given a random access memory with direct access by
  address + offset i.e. index)

  performance then depends on the performance of the hash algorithm,
  and/or the search function
 */

#include "hashtable.h"

#include <stdint.h>
#include <stdbool.h>


/* private */

// "The offset and prime are carefully chosen by people with PhDs."
#define FNV_OFFSET 14695981039346656037ULL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t _hash_key(const uint64_t* key)
{
	uint64_t hash = FNV_OFFSET;
	const uint8_t *ptr = (uint8_t*)key;

	// "for each byte_of_data to be hashed do"
	for (int idx = 0; idx < sizeof(*key); ptr++, idx++) {
		hash ^= (uint64_t)(*ptr);
		hash *= FNV_PRIME;
	}
	return hash;
}
uint64_t hash_key(const uint64_t* key) /* provided for testing */
{
	return _hash_key(key);
}


/* public */

int hash_size(hash_t* table)
{
	return table->size;
}

hash_t* hash_create(void)
{
	hash_t *table = malloc(sizeof(*table));
	if (!table) {
		perror("allocation failed");
		exit(EXIT_FAILURE); // -ENOMEM
	}
	table->size = 0;
	table->capacity = INITIAL_CAPACITY;

	table->entries = calloc(table->capacity, sizeof(*table->entries));
	if (!table->entries) {
		perror("allocation failed, again");
		exit(EXIT_FAILURE);
	}
	return table;
}


int hash_destroy(hash_t* table)
{
	// keys
	for (int idx = 0; idx < table->capacity; idx++) {
		if (NULL != table->entries[idx].key) {
			free((void*) table->entries[idx].key);
		}
	}

	// entries
	free(table->entries);

	// table
	free(table);
	return 0;
}


void* hash_search(hash_t* table, const uint64_t* key)
{
	uint64_t hash = _hash_key(key);
	int index = (int)(hash & (uint64_t)(table->capacity - 1));

#ifdef DEBUG
	fprintf(stderr, "%s(): index %d, key '%lld'",
		__func__, index, *key);
#endif

	// loop until we find an empty entry
	while (NULL != table->entries[index].key) {
		if (*key == *(table->entries[index].key)) {

#ifdef DEBUG
	fprintf(stderr, ", value %c\n", *(uint8_t*) table->entries[index].value);
#endif
			// key found, return value
			return table->entries[index].value;
		}

		index++;

		if (index >= table->capacity) {
			index = 0;
		}
	};

#ifdef DEBUG
	fprintf(stderr, "\n");
#endif

	return NULL;
}


const uint64_t* hash_insert(hash_t* table, const uint64_t* key, void* value)
{
	if (!value) {
		return NULL;
	}

	// now obtain an index within the allocated random access
	// memory locations, by the hash function
	uint64_t hash = _hash_key(key);
	int index = (int)(hash & (uint64_t)(table->capacity - 1));

#ifdef DEBUG
	fprintf(stderr, "%s(): index %d, key '%lld', value %c\n",
		__func__, index, *key, *(uint8_t*) value);
#endif
	// find the key
	while (table->entries[index].key != NULL) {
		if (*key == *(table->entries[index].key)) {
			// update value
			table->entries[index].value = value;
			return table->entries[index].key;
		}

		// key was not found
		index++;
		if (index >= table->capacity) {
			index = 0;
		}
	}

	table->size++;

	uint64_t *key_dup = malloc(sizeof(*key));
	if (!key_dup) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	*key_dup = *key; /* due to const, copy content to dup pointer memory */
	table->entries[index].key = key_dup; /* assign dup pointer to const */

	table->entries[index].value = value;

	return key;
}
