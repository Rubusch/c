/*
  hashtable implementation
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


static const uint64_t* _hash_set_entry(entry_t* entries, int capacity,
				  const uint64_t* key, void* value,
				  int* size)
{
	// now obtain an index within the allocated random access
	// memory locations, by the hash function
	uint64_t hash = _hash_key(key);
	int index = (int) (hash & (uint64_t)(capacity - 1));

#ifdef DEBUG
	fprintf(stderr, "%s(): index %d, key '%lld', value %c\n",
		__func__, index, *key, *(uint8_t*) value);
#endif

	// find the key
	while (entries[index].key != NULL) {
		if (*key == *(entries[index].key)) {
			// update value
			entries[index].value = value;
			return entries[index].key;
		}

		// key was not found
		index++;
		if (index >= capacity) {
			index = 0;
		}
	}

	// didn't find the key, copy and allocate if needed, then insert it
	if (size != NULL) {
		// not 'expand' - a new entry
		uint64_t *key_dup = malloc(sizeof(*key));
		if (!key_dup) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}
		*key_dup = *key; /* due to const, copy content to dup
				    pointer memory */
		entries[index].key = key_dup; /* assign dup pointer to const */

		// new entry, thus increase size
		(*size)++;
	} else {
		entries[index].key = key; /* 'expand' - entries are just copied */
	}
	entries[index].value = value;

	return key;
}


static bool _hash_expand(hash_t* table)
{
	int new_capacity = table->capacity * 2;
	if (new_capacity < table->capacity) {
		fprintf(stderr, "capacity overflow\n"); // overflow!
		exit(EXIT_FAILURE);
	}

	entry_t *new_entries = calloc(new_capacity, sizeof(*new_entries));
	if (new_entries == NULL) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	// move all non-empty entries to new ones
	for (int idx = 0; idx < table->capacity; idx++) {
		entry_t entry = table->entries[idx];
		if (entry.key != NULL) {
			_hash_set_entry(new_entries, new_capacity,
				       entry.key, entry.value, NULL);
		}
	}

	free(table->entries);
	table->entries = new_entries;
	table->capacity = new_capacity;

	return true;
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
			fprintf(stderr, ", value %c\n",
				*(uint8_t*) table->entries[index].value);
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

	// memory
	if (table->size >= table->capacity / 2) {
		if (!_hash_expand(table)) {
			return NULL;
		}
	}

	return _hash_set_entry(table->entries, table->capacity,
			      key, value, &table->size);
}

