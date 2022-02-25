/* Written by Peter Edmonds 2022 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#define ARR_SIZE 8196

/* The higher the level, the lookups are required to get to the index */
#define INDIRECTION_LEVEL 512
/* 1: Use optimal strategy */
/* 0: Use suboptimal strategy */
#define CACHE_OPTIMIZED 0
/* Enabling this should slow down the program further */
#define ENABLE_BRANCH 0


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
    return 0;
}


