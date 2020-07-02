#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdlib.h>
#include <stdbool.h>

typedef unsigned long (*hash_function)(const void *, size_t, unsigned long);
typedef struct htstruct *hashtable;

hashtable HashTableNew(hash_function hf);
bool HashTableHasKey(hashtable ht, const void *key, size_t ksize);
void *HashTableValueAtKey(hashtable ht, const void *key, size_t ksize);
void HashTableValueAndItsSizeAtKey(hashtable ht, const void *key, size_t ksize, void **value, size_t *vsize);
void HashTableInsert(hashtable ht, const void *key, size_t ksize, const void *value, size_t vsize);
void HashTableDelete(hashtable ht, const void *key, size_t ksize);

#endif // HASHTABLE_H_