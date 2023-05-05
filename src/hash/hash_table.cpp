#include "lib/list/listworks_.h"
#define LISTWORKS_HPP

#include "hash_table.h"

void HashTable_ctor(HashTable* table, err_anchor_t err_code) {
    table->contents = (List*) calloc(BUCKET_COUNT, sizeof(*table->contents));

    if (!table->contents) {
        *err_code = ENOMEM;
        return;
    }

    table->size = 0;

    for (size_t id = 0; id < BUCKET_COUNT; ++id) {
        table->contents[id] = {};
        log_printf(STATUS_REPORTS, "status", "Initialising bucket with id %lu.\n", id);
        List_ctor(&table->contents[id], DFLT_HT_CELL_SIZE, err_code);
        if (List_status(&table->contents[id]) != 0) {
            for (size_t rem_id = 0; rem_id < id; ++rem_id) List_dtor(table->contents + rem_id);
            *table = {};
            return;
        }
    }
}

void HashTable_dtor(HashTable* table) {
    if (HashTable_status(table)) return;

    for (size_t id = 0; id < BUCKET_COUNT; id++) {
        List_dtor(&table->contents[id], NULL);
    }

    free(table->contents);
}

ht_status_t HashTable_status(const HashTable* table) {
    if (!table) return HT_NULL;
    if (!table->contents) return HT_NO_CONTENT;

    #ifdef _DEBUG
    ht_status_t status = 0;
    for (size_t id = 0; id < BUCKET_COUNT; ++id) {
        if (List_status(&table->contents[id])) status |= HT_BROKEN_CELL;
    }
    #endif

    return 0;
}

void HashTable_insert(HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t comparator, err_anchor_t err_code) {
    _LOG_FAIL_CHECK_(HashTable_status(table) == 0, "error", ERROR_REPORTS, return, NULL, EINVAL);

    if (HashTable_find_value(table, hash, value, comparator)) return;

    List* list = &table->contents[hash % BUCKET_COUNT];

    List_push(list, value, err_code);

    ++table->size;
}

List* HashTable_find(const HashTable* table, hash_t hash) {
    _LOG_FAIL_CHECK_(HashTable_status(table) == 0, "error", ERROR_REPORTS, return NULL, NULL, EINVAL);
    return &table->contents[hash % BUCKET_COUNT];
}

#if OPTIMIZATION_LEVEL < 2
HT_ELEM_T* HashTable_find_value(const HashTable* table, hash_t hash, HT_ELEM_T value, ht_compar_fn_t* comparator) {
    _LOG_FAIL_CHECK_(HashTable_status(table) == 0, "error", ERROR_REPORTS, return NULL, NULL, EINVAL);

    List* bucket = &table->contents[hash % BUCKET_COUNT];
    _ListCell* iterator = &bucket->buffer[0];

    #if OPTIMIZATION_LEVEL == 0
    for (size_t elem_id = 0; elem_id < bucket->size; ++elem_id, ++iterator) {
        if (iterator->content != HT_ELEM_POISON && comparator(iterator->content, value) == 0) return &iterator->content;
    }
    #endif

    #if OPTIMIZATION_LEVEL >= 1
    __m256i search_word = value;

    for (size_t elem_id = 0; elem_id < bucket->size; ++elem_id, ++iterator) {
        __m256i word = iterator->content;
        if (_mm256_testc_si256(word, search_word)) {
            return &iterator->content;
        }
    }
    #endif

    return NULL;
}
#endif
