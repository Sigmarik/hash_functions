#include "hash_functions.h"

#include <string.h>
#include <inttypes.h>

#include "lib/util/dbg/debug.h"
#include "src/utils/config.h"

static hash_t cycle_left(hash_t num, unsigned short shift) {
    hash_t prefix = (num >> (sizeof(hash_t) * 8 - shift));

    return (num << shift) + prefix;
}

static hash_t cycle_right(hash_t num, unsigned short shift) {
    hash_t suffix = (num & ((1ull << shift) - 1)) << (sizeof(hash_t) * 8 - shift);

    return (num >> shift) + suffix;
}

hash_t constant_hash(const void* begin, const void* end)    { SILENCE_UNUSED(begin); SILENCE_UNUSED(end); return 1; }
hash_t first_char_hash(const void* begin, const void* end)  { SILENCE_UNUSED(end); return (hash_t) *(char*)begin; }
hash_t length_hash(const void* begin, const void* end)      { return strnlen((char*) begin, (size_t) ((char*) end - (char*) begin)); }

hash_t sum_hash(const void* begin, const void* end) {
    hash_t sum = 0;
    for (char* ptr = (char*) begin; ptr < (char*) end; ++ptr) {
        sum += (hash_t) *ptr;
    }
    return sum;
}

hash_t left_shift_hash(const void* begin, const void* end) {
    hash_t sum = 0;
    for (char* ptr = (char*) begin; ptr < (char*) end; ++ptr) {
        sum = cycle_left(sum, 1) ^ (hash_t) *ptr;
    }
    return sum;
}

hash_t right_shift_hash(const void* begin, const void* end) {
    hash_t sum = 0;
    for (char* ptr = (char*) begin; ptr < (char*) end; ++ptr) {
        sum = cycle_right(sum, 1) ^ (hash_t) *ptr;
    }
    return sum;
}

hash_t murmur_hash(const void* begin, const void* end) {
    hash_t value = 0xBAADF00DDEADBEEF;
    for (const hash_t* segment = (hash_t*) begin; segment < (hash_t*) end; ++segment) {
        hash_t current = *segment;
        current = cycle_left(current * 0xDED15DED, 31) * 0xCADAB8A9;
        current ^= value;
        current = cycle_left(current, 15) * 0x112C13AB + 0x314159265358979;
        value = current;
    }
    return value;
}
