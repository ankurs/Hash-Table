#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include<sys/types.h>

#define HASH_LEN 256
#define HASH(x,y) (size_t)(((*(int *)x)+(int)y)%HASH_LEN)

typedef struct hash_table_element hash_table_element_t;

struct hash_table_element
{
    size_t key_len;
    size_t value_len;
    void * key;
    void * value;
    hash_table_element_t * next;
};
#define hash_table_element_s sizeof(hash_table_element_t)

typedef enum hash_table_mode{
    MODE_COPY,
    MODE_VALUEREF
} hash_table_mode_t;

typedef struct hash_table
{
    hash_table_element_t  * store_house[HASH_LEN];
    hash_table_mode_t mode;
    size_t key_count;
} hash_table_t;
#define hash_table_s sizeof(hash_table_t)

// element operations
hash_table_element_t * hash_table_element_new();
void hash_table_element_delete(hash_table_element_t *);


// hash table operations
hash_table_t * hash_table_new(hash_table_mode_t);
void hash_table_delete(hash_table_t *);
#define HT_ADD(table, key, value) hash_table_add(table, (void *) key, sizeof(*key), (void *) value, sizeof(*value))
int hash_table_add(hash_table_t *, void *, size_t, void *, size_t);
#define HT_REMOVE(table, key) hash_table_remove(table, key, sizeof(*key))
int hash_table_remove(hash_table_t *, void *, size_t);
#define HT_LOOKUP(table, key) hash_table_lookup(table, key, sizeof(*key))
void * hash_table_lookup(hash_table_t *, void *, size_t);
#define HT_HAS_KEY(table, key) hash_table_has_key(table, key, sizeof(*key))
int hash_table_has_key(hash_table_t *, void *, size_t);
int hash_table_get_keys(hash_table_t *, void **);

#endif
