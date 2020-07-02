#include <string.h>
#include "hashtable.h"

#define HT_INITIAL_SIZE 8
#define MAX_TOLERABLE_LOAD 0.8

typedef struct ht_node
{
    void *key, *value;
    size_t ksize, vsize;
    struct ht_node *next;
} ht_node;

typedef ht_node *ht_bucket;

struct htstruct
{
    ht_bucket *buckets;
    unsigned long size;
    unsigned long nkeys;
    hash_function hasher;
};

bool DataEqual(const void *d1, size_t size1, const void *d2, size_t size2)
{
    unsigned int i;
    unsigned char *c1, *c2;

    if (size1 != size2)
        return 0;
    else
    {
        c1 = (unsigned char *) d1;
        c2 = (unsigned char *) d2;
        i = 0;
        while (i < size1 && c1[i] == c2[i])
            i++;
        return i == size1;
    }
}

void HashTableExpand(hashtable ht)
{
    int i;
    ht_bucket bucket, aux = NULL;
    unsigned long index, origsize = ht->size;

    ht->size *= 2;
    ht->buckets = realloc(ht->buckets, ht->size * sizeof(ht_bucket));
    for (i = origsize; i < ht->size; i++)
        ht->buckets[i] = NULL;
    for (i = 0; i < origsize; i++)
    {
        bucket = ht->buckets[i];
        if (bucket != NULL)
        {
            while (bucket->next != NULL)
                bucket = bucket->next;
            bucket->next = aux;
            aux = ht->buckets[i];
            ht->buckets[i] = NULL;
        }
    }
    while (aux != NULL)
    {
        bucket = aux;
        aux = aux->next;
        index = ht->hasher(bucket->key, bucket->ksize, ht->size);
        bucket->next = ht->buckets[index];
        ht->buckets[index] = bucket;
    }
}

hashtable HashTableNew(hash_function hf)
{
    hashtable ht = (hashtable) malloc(sizeof(*ht));

    ht->buckets = calloc(HT_INITIAL_SIZE, sizeof(ht_bucket));
    ht->size = HT_INITIAL_SIZE;
    ht->nkeys = 0;
    ht->hasher = hf;
    return ht;
}

bool HashTableHasKey(hashtable ht, const void *key, size_t ksize)
{
    unsigned long position = ht->hasher(key, ksize, ht->size);
    ht_bucket bucket = ht->buckets[position];

    while (bucket != NULL && !DataEqual(key, ksize, bucket->key, bucket->ksize))
        bucket = bucket->next;
    return bucket != NULL;
}

void *HashTableValueAtKey(hashtable ht, const void *key, size_t ksize)
{
    unsigned long position = ht->hasher(key, ksize, ht->size);
    ht_bucket bucket = ht->buckets[position];

    while (bucket != NULL && !DataEqual(key, ksize, bucket->key, bucket->ksize))
        bucket = bucket->next;
    return bucket != NULL ? bucket->value : NULL;
}

void HashTableValueAndItsSizeAtKey(hashtable ht, const void *key, size_t ksize, void **value, size_t *vsize)
{
    unsigned long position = ht->hasher(key, ksize, ht->size);
    ht_bucket bucket = ht->buckets[position];

    while (bucket != NULL && !DataEqual(key, ksize, bucket->key, bucket->ksize))
        bucket = bucket->next;
    if (bucket != NULL) {
        *value = bucket->value;
        *vsize = bucket->vsize;
    }
}

void HashTableInsert(hashtable ht, const void *key, size_t ksize, const void *value, size_t vsize)
{
    ht_bucket current, new;
    unsigned long position;

    if ((float) ht->nkeys / ht->size >= MAX_TOLERABLE_LOAD)
        HashTableExpand(ht);
    position = ht->hasher(key, ksize, ht->size);
    current = ht->buckets[position];
    while (current != NULL && !DataEqual(key, ksize, current->key, current->ksize))
        current = current->next;

    if (current != NULL)
    {
        if (current->vsize != vsize)
            current->value = realloc(current->value, vsize);
        memcpy(current->value, value, vsize);
    }
    else
    {
        new = (ht_bucket) malloc(sizeof(ht_node));
        new->key = malloc(ksize);
        memcpy(new->key, key, ksize);
        new->ksize = ksize;
        new->value = malloc(vsize);
        memcpy(new->value, value, vsize);
        new->vsize = vsize;
        new->next = ht->buckets[position];
        ht->buckets[position] = new;
        (ht->nkeys)++;
    }
}

void HashTableDelete(hashtable ht, const void *key, size_t ksize)
{
    unsigned long position = ht->hasher(key, ksize, ht->size);
    ht_bucket bucket, previous;

    bucket = ht->buckets[position];
    previous = NULL;
    while (bucket != NULL && !DataEqual(key, ksize, bucket->key, bucket->ksize))
    {
        previous = bucket;
        bucket = bucket->next;
    }
    if (bucket != NULL)
    {
        if (ht->buckets[position] == bucket)
            ht->buckets[position] = bucket->next;
        else
            previous->next = bucket->next;
        free(bucket);
        (ht->nkeys)--;
    }
}