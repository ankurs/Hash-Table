#include "hashtable.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>

// element operations
hash_table_element_t * hash_table_element_new()
{
    INFO("creating a new hash table element");
    return calloc(1, hash_table_element_s);
}

void hash_table_element_delete(hash_table_element_t * element)
{
    INFO("Deleting an hash table element");
    free(element->key);
    free(element->value);
    free(element);
}

// hash table operations
hash_table_t * hash_table_new(hash_table_mode_t mode)
{
    INFO("Creating a new hash table");
    hash_table_t *table = calloc(1, hash_table_s);
    table->mode = mode;
    return table;
}

void hash_table_delete(hash_table_t * table)
{
    INFO("Deleating a hash table");
    size_t i=0;
    for (;i<HASH_LEN;i++)
    {
        while (table->store_house[i])
        {
            hash_table_element_t * temp = table->store_house[i];
            table->store_house[i] = table->store_house[i]->next;
            hash_table_element_delete(temp);
        }
    }
    free(table);
}

int hash_table_add(hash_table_t * table, void * key, size_t key_len, void * value, size_t value_len)
{
    size_t hash = HASH(key, key_len);
    hash_table_element_t * element = hash_table_element_new();
    if (!element)
    {
        INFO("Cannot allocate memory for element");
        return -1; // No Memory
    }
    if (table->mode == MODE_COPY)
    {
        LOG("Adding a key-value pair to the hash table with hash -> %d, in COPY MODE", (int)hash);
        element->key = malloc(key_len);
        element->value = malloc(value_len);
        if (element->key && element->value)
        {
            memcpy(element->key, key, key_len);
            memcpy(element->value, value, value_len);
        }
        else
        {
            if (element->key)
            {
                free(element->key);
                INFO("Cannot allocate memory for value");
            }
            if (element->value)
            {
                free(element->value);
                INFO("Cannot allocate memory for key");
            }
            free(element);
            return -1; //No Memory
        }
    }
    else if (table->mode == MODE_VALUEREF)
    {
        LOG("Adding a key-value pair to the hash table with hash -> %d, in VALUEREF MODE", (int)hash);
        element->key = malloc(key_len);
        if (element->key)
        {
            memcpy(element->key, key, key_len);
        }
        else
        {
            INFO("Cannot allocate memory for key");
            free(element);
            return -1; //No Memory
        }
        element->value = value;
    }
    element->key_len = key_len;
    element->value_len = value_len;
    element->next = NULL;
    // find the key position for chaining
    if (!table->store_house[hash])
    {
        LOG("No Conflicts adding the first element at %d", (int)hash);
        table->store_house[hash] = element;
        table->key_count++;
    }
    else
    {
        LOG("Conflicts adding element at %d", (int)hash);
        hash_table_element_t * temp = table->store_house[hash];
        while(temp->next)
        {
            temp = temp->next;
        }
        temp->next = element;
        table->key_count++;
    }
    return 0;
}

int hash_table_remove(hash_table_t * table, void * key, size_t key_len)
{
    INFO("Deleting a key-value pair from the hash table");
    size_t hash = HASH(key, key_len);
    if (!table->store_house[hash])
    {
        LOG("Key Not Found -> No element at %d", (int)hash);
        return -1; // key not found
    }
    hash_table_element_t *temp = table->store_house[hash];
    hash_table_element_t *prev = temp;
    while(temp)
    {
        while(temp && temp->key_len!=key_len)
        {
            prev = temp;
            temp = temp->next;
        }
        if(temp)
        {
            if (!memcmp(temp->key, key, key_len))
            {
                prev->next = temp->next;
                hash_table_element_delete(temp);
                INFO("Deleted a key-value pair from the hash table");
                table->key_count--;
                return 0;
            }
        }
    }
    INFO("Key Not Found");
    return -1; // key not found
}

void * hash_table_lookup(hash_table_t * table, void * key, size_t key_len)
{
    size_t hash = HASH(key, key_len);
    LOG("Looking up a key-value pair for hash -> %d", (int)hash);
    if (!table->store_house[hash])
    {
        LOG("Key not found at hash %d, no entries", (int)hash);
        return NULL; // key not found
    }
    hash_table_element_t *temp = table->store_house[hash];
    while(temp)
    {
        while(temp && temp->key_len!=key_len)
        {
            temp = temp->next;
        }
        if(temp)
        {
            if (!memcmp(temp->key, key, key_len))
            {
                LOG("Found Key at hash -> %d", (int)hash);
                return temp->value;
            }
            else
            {
                temp = temp->next;
            }
        }
    }
    LOG("Key not found at hash %d", (int)hash);
    return NULL; // key not found   
}

int hash_table_has_key(hash_table_t * table, void * key, size_t key_len)
{
    size_t hash = HASH(key, key_len);
    LOG("Searching for key with hash -> %d", (int)hash);
    if (!table->store_house[hash])
    {
        LOG("Key not found with hash -> %d, no entries", (int)hash);
        return 0; // key not found
    }
    hash_table_element_t *temp = table->store_house[hash];
    while(temp)
    {
        while(temp && temp->key_len!=key_len)
        {
            temp = temp->next;
        }
        if(temp)
        {
            if (!memcmp(temp->key, key, key_len))
            {
                LOG("Key Found with hash -> %d", (int)hash);
                return 1; // key found
            }
        }
    }
    LOG("Key not found with hash -> %d", (int)hash);
    return 0; // key not found   
}

int hash_table_get_keys(hash_table_t * table, void ** keys)
{
    size_t i = 0;
    size_t count = 0;
    keys = calloc(table->key_count, sizeof(void *));
    for(i=0;i<HASH_LEN;i++)
    {
        if (table->store_house[i])
        {
            keys[count++] = table->store_house[i];
            hash_table_element_t *temp = table->store_house[i];
            #ifdef DEBUG
            size_t num = 1;
            #endif
            while(temp->next)
            {
                keys[count++] = temp->next;
                temp = temp->next;
                #ifdef DEBUG
                num++;
                #endif
            }
            #ifdef DEBUG
            LOG("found %d key(s) at hash -> %d", (int)num, (int)i);
            #endif 
        }
    }
    return count;
}

