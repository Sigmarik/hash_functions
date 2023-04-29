/**
 * @file main.cpp
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Main patcher program.
 * @version 0.1
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <ctype.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/util/dbg/debug.h"
#include "lib/util/argparser.h"
#include "lib/alloc_tracker/alloc_tracker.h"
#include "lib/util/util.h"

#include "utils/config.h"
#include "utils/main_utils.h"

#include "hash/hash_functions.h"
#include "hash/hash_table.hpp"

#include "text_parser/text_parser.h"

#define MAIN

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    start_local_tracking();
    unsigned int log_threshold = STATUS_REPORTS;
    MAKE_WRAPPER(log_threshold);

    ActionTag line_tags[] = {
        #include "cmd_flags/main_flags.h"
    };
    const int number_of_tags = ARR_SIZE(line_tags);

    parse_args(argc, argv, number_of_tags, line_tags);
    log_init("program_log.html", log_threshold, &errno);
    print_label();

    const char* sample_file_name = get_input_file_name(argc, argv, DEFAULT_SAMPLE_NAME);
    log_printf(STATUS_REPORTS, "status", "Opening file %s.\n", sample_file_name);
    const char* word_list = NULL;
    size_t sample_size = read_words(sample_file_name, &word_list);

    _LOG_FAIL_CHECK_(word_list, "error", ERROR_REPORTS, return_clean(EXIT_FAILURE), NULL, ENOENT);

    HashTable table = {};
    HashTable_ctor(&table, BUCKET_COUNT, &errno);
    track_allocation(table, HashTable_dtor);

    _LOG_FAIL_CHECK_(HashTable_status(&table) == 0, "error", ERROR_REPORTS, return_clean(EXIT_FAILURE), NULL, ENOMEM);

    clock_t start_time = clock();

    for (const char* word_ptr = word_list;
        word_ptr < word_list + sample_size * MAX_WORD_LENGTH;
        word_ptr += MAX_WORD_LENGTH) {
        
        HashTable_insert(&table, TESTED_HASH(word_ptr, word_ptr + MAX_WORD_LENGTH), word_ptr, strcmp);
    }


    #ifdef DISTRIBUTION_TEST  //* DISTRIBUTION TEST CASE ==============================

    FILE* out_table = fopen(OUTPUT_TABLE_NAME, "w");
    _LOG_FAIL_CHECK_(out_table, "error", ERROR_REPORTS, return_clean(EXIT_FAILURE), NULL, ENOENT);

    fprintf(out_table, "bucket_id,size\n");

    for (unsigned bucket_id = 0; bucket_id < BUCKET_COUNT; ++bucket_id) {
        fprintf(out_table, "%u,%lu\n", bucket_id, table.contents[bucket_id].size);
    }

    if (out_table) fclose(out_table);

    #endif


    #ifdef PERFORMANCE_TEST  //* PERFORMANCE TEST CASE ==============================

    FILE* out_timetable = fopen(OUTPUT_TIMETABLE_NAME, "w");
    _LOG_FAIL_CHECK_(out_timetable, "error", ERROR_REPORTS, return_clean(EXIT_FAILURE), NULL, ENOENT);

    fprintf(out_timetable, "word_count,time\n");


    for (size_t word_id = 0; word_id < sample_size; ++word_id) {
        const char* word_ptr = word_list + word_id * MAX_WORD_LENGTH;
        HashTable_find_value(&table, TESTED_HASH(word_ptr, word_ptr + MAX_WORD_LENGTH), word_ptr, strcmp);

        if (word_id % 128 == 0) {
            fprintf(out_timetable, "%lu,%ld\n", word_id, clock() - start_time);
        }
    }

    if (out_timetable) fclose(out_timetable);

    #endif

    return_clean(errno == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
