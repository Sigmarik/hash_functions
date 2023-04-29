/**
 * @file firstchar_hash.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief Hash function definitions
 * @version 0.1
 * @date 2023-04-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include "hash.h"

HASH_FUNCTION(constant_hash);
HASH_FUNCTION(first_char_hash);
HASH_FUNCTION(length_hash);
HASH_FUNCTION(sum_hash);
HASH_FUNCTION(left_shift_hash);
HASH_FUNCTION(right_shift_hash);

HASH_FUNCTION(murmur_hash);

#endif
