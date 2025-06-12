/**
 * @file memory_management.c
 * @brief Implements memory management functionality.
 */
#include "memory_management.h"

#include "../logging/logger.h"

/**
 * @brief Initialize a memory pool of the given size.
 * @param size the size of the memory pool to initialize,
 * when the given size is smaller than 1MB, the size will be automatically set to 1MB
 * @return the pointer to the memory pool. When NULL, the initialization failed
 */
memory_pool_t* init_memory_pool(size_t size) {
    if (size < MIN_MEMORY_POOL_SIZE) {
        //set the size to the minimum
        size = MIN_MEMORY_POOL_SIZE;
    }

    memory_pool_t* pool = malloc(sizeof(memory_pool_t));
    if (!pool) {
        log_msg(ERROR, "Memory", "Failed to allocate memory for the pool base structure");
        return NULL;
    }

    pool->memory = malloc(size);
    if (!pool->memory) {
        log_msg(ERROR, "Memory", "Failed to allocate memory for the pool");
        free(pool);
        return NULL;
    }

    pool->pool_size = size;
    pool->first = (memory_block_t*) pool->memory;
    pool->first->size = size - sizeof(memory_block_t);
    pool->first->active = 0; // mark the block as free
    pool->first->next = NULL;// no next block

    return pool;
}

/**
 * @brief Allocates memory on the given memory pool.
 * If the remaining memory space is lager enough, creates a new unused block for the remaining memory.
 *
 * @param pool the pool to allocate memory from
 * @param size the size of the memory to allocate
 * @return the pointer to the reserved memory space, or NULL if there is no free space on the pool
 */
void* memory_pool_alloc(memory_pool_t* pool, size_t size) {
    memory_block_t* current = pool->first;

    while (current) {
        if (!current->active && current->size >= size) {
            // found a free block that is large enough

            const size_t remaining = current->size - size;
            if (remaining > MIN_MEMORY_BLOCK_SIZE) {
                // remaining is large enough
                // create a new block for the remaining memory
                memory_block_t* new_block = (memory_block_t*) ((char*) current + sizeof(memory_block_t) + size);

                new_block->size = remaining - sizeof(memory_block_t);
                new_block->active = 0;          // mark the new block as free
                new_block->next = current->next;// link to the next block

                current->size = size;// set the size of the current block

                current->next = new_block;// link to the new block
            } else {
                log_msg(WARNING, "Memory", "No more space left in the block, using the whole block");
            }

            //the remaining memory space is too small, so the current block will be used entirely
            current->active = 1;
            return (void*) (current + 1);// return pointer to user data
        }
        current = current->next;// move to the next block
    }

    log_msg(ERROR, "Memory", "No free block found for allocation");
    return NULL;
}

/**
 * @brief Sets the given data pointer to not active in the given memory pool.
 * But first checks if the pointer is contained in the memory pool.
 * If needed it will defragment the memory pool and merge free blocks.
 *
 * @param pool the pool to free memory from
 * @param ptr the pointer to the memory to free
 */
void memory_pool_free(memory_pool_t* pool, void* ptr) {
    if (!ptr) {
        log_msg(ERROR, "Memory", "Pointer is NULL");
        return;
    }

    memory_block_t* block = (memory_block_t*) ptr - 1;
    if (block < pool->first || block >= (memory_block_t*) ((char*) pool->first + pool->pool_size)) {
        log_msg(ERROR, "Memory", "Pointer is not in the memory pool");
        return;
    }
    block->active = 0;

    // when needed, defragmentation of the memory blocks
    memory_block_t* current = pool->first;
    while (current && current->next) {
        if (!current->active && !current->next->active) {
            // merge with the next block
            current->size += sizeof(memory_block_t) + current->next->size;
            current->next = current->next->next;// link to the next block
        } else {
            current = current->next;// move to the next block
        }
    }
}

void shutdown_memory_pool(memory_pool_t* pool) {
    if (!pool) {
        log_msg(ERROR, "Memory", "Pool is NULL");
        return;
    }

    free(pool->memory);
    free(pool);
}
