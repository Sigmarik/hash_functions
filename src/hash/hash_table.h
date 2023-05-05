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

#include "src/utils/config.h"

#if OPTIMIZATION_LEVEL < 1
typedef const char* HT_ELEM_T;
const HT_ELEM_T HT_ELEM_POISON = NULL;
typedef HT_ELEM_T list_elem_t;
#else
typedef __m256i HT_ELEM_T __attribute__((__aligned__(32)));
const HT_ELEM_T HT_ELEM_POISON = _mm256_set1_epi8(0);
typedef HT_ELEM_T list_elem_t __attribute__((__aligned__(32)));
#endif

static const list_elem_t LIST_ELEM_POISON = HT_ELEM_POISON;

#include "lib/list/listworks.h"

static const size_t DFLT_HT_CELL_SIZE = 256;

typedef unsigned ht_status_t;

typedef int ht_compar_fn_t(HT_ELEM_T alpha, HT_ELEM_T beta);

enum HT_STATUS {
    HT_NULL         = 1 << 0,
    HT_NO_CONTENT   = 1 << 1,
    HT_BROKEN_CELL  = 1 << 3,
};

struct HashTable {
    size_t size = 0;
    List* contents = NULL;
};


//* DECLARATIONS

/**
 * @brief Construct hash table data structure
 * 
 * @param table pointer to the table
 * @param err_code pointer to the errno-functioning variable 
 */
void HashTable_ctor(HashTable* table, ERROR_MARKER);

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

#if OPTIMIZATION_LEVEL < 2
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

#endif
