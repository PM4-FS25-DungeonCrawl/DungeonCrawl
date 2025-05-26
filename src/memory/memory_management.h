/**
 * @file memory_management.h
 * @brief Exposes functions for working with the memory management.
 */
#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stdlib.h>

#define STANDARD_MEMORY_POOL_SIZE (8 * 1024 * 1024)        // 8MB
#define MIN_MEMORY_POOL_SIZE (1024 * 1024)                 // 1MB
#define MIN_MEMORY_BLOCK_SIZE (sizeof(memory_block_t) + 16)// 16 bytes for min user data

typedef struct memory_block_t {
    size_t size;                // size of the block (without the header)
    int active;                 // 1 if the block is in use, 0 if it is free
    struct memory_block_t* next;// pointer to the next block
    //here lays the user data
} memory_block_t;

typedef struct {
    size_t pool_size;// size of the memory pool
    void* memory;
    memory_block_t* first;// pointer to the first block
} memory_pool_t;


/**
 * @brief Initialize a memory pool of the given size.
 *
 * This function initializes a memory pool for dynamic memory allocation.
 * If the given size is smaller than the minimum required size, it will
 * be automatically set to the minimum size.
 *
 * @param size The size of the memory pool to initialize.
 * @return A pointer to the initialized memory pool, or NULL if the initialization failed.
 */
memory_pool_t* init_memory_pool(size_t size);
/**
 * @brief Allocate a memory pool.
 *
 * @param pool The memory pool to allocate.
 * @param size The size of the memory pool.
 */
void* memory_pool_alloc(memory_pool_t* pool, size_t size);
/**
 * @brief Free a memory pool.
 *
 * @param pool The pool to free.
 * @param ptr A pointer to who knows what.
 */
void memory_pool_free(memory_pool_t* pool, void* ptr);
/**
 * @brief Shuts down the memory pool.
 *
 * @param pool The memory pool to be shut down.
 */
void shutdown_memory_pool(memory_pool_t* pool);

#endif//MEMORY_MANAGEMENT_H
