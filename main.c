/* Written by Peter Edmonds 2022 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#define ARR_SIZE 8196

/* The higher the level, the lookups are required to get to the index */
#define INDIRECTION_LEVEL 512

/* Determines whether a linear or randomized access pattern is used
 * 1 means linear, 0 means randomized */
#define CACHE_OPTIMIZED 0

/* Determines whether a branch is used based on the value read
 * This will cause many branch predictions failures if CACHE_OPTIMIZED = 1*/
#define ENABLE_BRANCH 0

/* Determines the number of elements are linearly searched in a single lookup. */
#define SUMMATION_LEVEL 0




uint32_t * generate_indexes(int size) {
    uint32_t* arr = malloc(sizeof(uint32_t) * size);
    for (uint32_t i = 0; i < size; i++) {
        arr[i] = i;
    }
    return arr;
}

/* Shuffles an array */
void shuffle(uint32_t* arr, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        uint32_t random = rand() % size;
        uint32_t temp = arr[i];
        arr[i] = arr[random];
        arr[random] = temp;
    }
}

/* Generates a lookup table with a certain depth. */
uint32_t** generate_lookup_table(uint32_t depth, bool do_shuffle) {
    uint32_t** lookup_table = malloc(sizeof(uint32_t*) * depth);
    for (uint32_t i = 0; i < depth; ++i) {
        /* Generate a series of in-order indexes */
        uint32_t* indexes = generate_indexes(ARR_SIZE);

        /* Randomize the sequence */
        if(do_shuffle) {
            shuffle(indexes, ARR_SIZE);
        }
        lookup_table[i] = indexes;
    }
    return lookup_table;
}

/* Reads down the tree of a lookup table */
uint32_t read_down_tree(uint32_t** lookup_table, uint32_t depth, uint32_t index, uint32_t* data) {
    if (depth == 0) {
        return data[index];
    }
    uint32_t sum = 0;
    /* Sum the next SUMMATION_LEVEL elements in data */
    for (uint32_t i = 0; i+index < ARR_SIZE && i<SUMMATION_LEVEL; ++i) {
        sum += data[index+i];
    }

    return read_down_tree(lookup_table, depth - 1, lookup_table[depth - 1][index], data);
}

uint32_t traverse_direct(uint32_t* data, uint32_t** lookup_table){
    uint32_t prev = 0;
    uint32_t count = 0;
    for (int i = 0; i < ARR_SIZE; i++) {
        uint32_t value = read_down_tree(lookup_table, INDIRECTION_LEVEL, i, data);
        if(ENABLE_BRANCH){
            if(prev < value) {
                count++;
            }
        }
        prev = value;
    }
    return count;
}

void free_lookup_table(uint32_t** lookup_table, uint32_t depth) {
    for (uint32_t i = 0; i < depth; ++i) {
        free(lookup_table[i]);
    }
    free(lookup_table);
}

int main() {
    uint32_t* data = generate_indexes(ARR_SIZE);
    uint32_t** lookup_table = NULL;
    if(CACHE_OPTIMIZED) {
        lookup_table = generate_lookup_table(INDIRECTION_LEVEL, false);
    } else {
        lookup_table = generate_lookup_table(INDIRECTION_LEVEL, true);
    }
    uint32_t result = traverse_direct(data, lookup_table);
    printf("%d\n", result);
    free_lookup_table(lookup_table, INDIRECTION_LEVEL);
    free(data);
    return 0;
}


