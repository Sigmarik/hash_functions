/**
 * @file hash_table.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief The most basic hash table.
 * @version 0.1
 * @date 2023-04-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <x86intrin.h>

#ifndef HT_ELEM_T
typedef const char* HT_ELEM_T;
#endif

#ifndef HT_ELEM_POISON
#define HT_ELEM_POISON NULL
#endif

#define list_elem_t HT_ELEM_T
static const list_elem_t LIST_ELEM_POISON = HT_ELEM_POISON;

#include "lib/list/listworks.h"

static const size_t DFLT_HT_CELL_SIZE = 16;

typedef unsigned ht_status_t;

typedef int ht_compar_fn_t(HT_ELEM_T alpha, HT_ELEM_T beta);

enum HT_STATUS {
    HT_NULL         = 1 << 0,
    HT_NO_CONTENT   = 1 << 1,
    HT_UNINIT       = 1 << 2,
    HT_BROKEN_CELL  = 1 << 3,
};

struct HashTable {
    size_t size = 0;
    List* contents = NULL;
};

size_t test = sizeof(_ListCell);


//* DECLARATIONS

/**
 * @brief Construct hash table data structure
 * 
 * @param table pointer to the table
 * @param err_code pointer to the errno-functioning variable 
 */
void HashTable_ctor(HashTable* table, size_t size, ERROR_MARKER);

/**
 * @brief Destroy the table
 * 
 * @param table pointer to the table to destroy
 */
void HashTable_dtor(HashTable* table);

/**
 * @brief Get status of the hash table
 * 
 * @param table pointer to the table
 * @return ht_status_t
 */
ht_status_t HashTable_status(const HashTable* table);

/**
 * @brief Insert an element 
 * 
 * @param table pointer to the table
 * @param hash hash of the new element
 * @param value value of the element
 * @param err_code pointer to the errno-functioning variable
 */
void HashTable_insert(HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t comparator, ERROR_MARKER);

/**
 * @brief Get the list of elements matching specified hash from the table
 * 
 * @param table pointer to the tables
 * @param hash hash to search for
 * @return pointer to the list where all elements match specified hash
 */
List* HashTable_find(const HashTable* table, hash_t hash);

#if OPTIM_LVL < 2
/**
 * @brief Find element in hash table by its hash and value
 * 
 * @param table hash table to search in
 * @param hash hash of the element
 * @param value exact value of the element
 * @param comparator comparator function between elements (should return 0 on equality)
 * @return pointer to the element cell in table (NULL if the element was not found)
 */
HT_ELEM_T* HashTable_find_value(const HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t* comparator);
#else
extern HT_ELEM_T* HashTable_find_value(const HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t* comparator) __asm__ ("HashTable_find_value");
#endif


//* IMPLEMENTATIONS

void HashTable_ctor(HashTable* table, size_t size, err_anchor_t err_code) {
    table->contents = (List*) calloc(size, sizeof(*table->contents));

    if (!table->contents) {
        *err_code = ENOMEM;
        return;
    }

    table->size = size;

    for (size_t id = 0; id < size; ++id) {
        table->contents[id] = {};
        List_ctor(&table->contents[id], DFLT_HT_CELL_SIZE, err_code);
        if (List_status(&table->contents[id])) {
            HashTable_dtor(table);
            return;
        }
    }
}

void HashTable_dtor(HashTable* table) {
    if (HashTable_status(table)) return;

    for (size_t id = 0; id < table->size; id++) {
        List_dtor(&table->contents[id], NULL);
    }

    free(table->contents);
}

ht_status_t HashTable_status(const HashTable* table) {
    if (!table) return HT_NULL;
    if (!table->size) return HT_UNINIT;
    if (!table->contents) return HT_NO_CONTENT;

    #ifdef _DEBUG
    ht_status_t status = 0;
    for (size_t id = 0; id < table->size; ++id) {
        if (List_status(&table->contents[id])) status |= HT_BROKEN_CELL;
    }
    #endif

    return 0;
}

void HashTable_insert(HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t comparator, err_anchor_t err_code) {
    if (HashTable_status(table)) return;

    if (HashTable_find_value(table, hash, value, comparator)) return;

    List* list = &table->contents[hash % table->size];

    if (list->size >= list->capacity - 1) {
        if (List_inflate(list, list->capacity * 2, err_code)) return;
    }

    List_insert(list, value, List_find_position(list, -1), err_code);
}

List* HashTable_find(const HashTable* table, hash_t hash) {
    _LOG_FAIL_CHECK_(HashTable_status(table) == 0, "error", ERROR_REPORTS, return NULL, NULL, EINVAL);
    return &table->contents[hash % table->size];
}

#if OPTIM_LVL < 2
HT_ELEM_T* HashTable_find_value(const HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t* comparator) {
    _LOG_FAIL_CHECK_(HashTable_status(table) == 0, "error", ERROR_REPORTS, return NULL, NULL, EINVAL);

    List* bucket = &table->contents[hash % table->size];
    _ListCell* iterator = &bucket->buffer[0];

    #if OPTIM_LVL == 0
    for (size_t elem_id = 0; elem_id < bucket->size; ++elem_id, ++iterator) {
        if (iterator->content != HT_ELEM_POISON && comparator(iterator->content, value) == 0) return &iterator->content;
    }
    #endif

    #if OPTIM_LVL >= 1
    __m256i search_word_l = _mm256_loadu_si256((const __m256i_u*)value);
    __m256i search_word_r = _mm256_loadu_si256((const __m256i_u*)value + 1);

    for (size_t elem_id = 0; elem_id < bucket->size; ++elem_id, ++iterator) {
        if (iterator->content == HT_ELEM_POISON) continue;

        __m256i word_l = _mm256_loadu_si256((const __m256i_u*)iterator->content);
        __m256i word_r = _mm256_loadu_si256((const __m256i_u*)iterator->content + 1);
        if (_mm256_testc_si256(word_l, search_word_l) && _mm256_testc_si256(word_r, search_word_r)) {
            return &iterator->content;
        }
    }
    #endif

    return NULL;
}
#endif

#endif
