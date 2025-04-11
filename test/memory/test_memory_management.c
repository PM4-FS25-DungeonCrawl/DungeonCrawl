#include "../../src/memory/memory_management.h"

#include <assert.h>
#include <stdio.h>

memory_pool_t* pool1;
memory_pool_t* pool2;

void test_init_memory_pool(void) {
    // initialize regular memory pool with 1MB size
    pool1 = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    assert(pool1 != NULL);
    assert(pool1->pool_size == MIN_MEMORY_POOL_SIZE);
    assert(pool1->memory != NULL);
    assert(pool1->first != NULL);

    assert(pool1->first->size == MIN_MEMORY_POOL_SIZE - sizeof(memory_block_t));
    assert(pool1->first->active == 0);
    assert(pool1->first->next == NULL);

    // initialize memory pool with 0 size
    pool2 = init_memory_pool(0);
    assert(pool2 != NULL);
    assert(pool2->pool_size == MIN_MEMORY_POOL_SIZE);

    printf("test_init_memory_pool passed\n");
}

void test_memory_alloc_free(void) {
    size_t alloc_size1 = 1024; // 1KB
    void* ptr = memory_pool_alloc(pool1, alloc_size1);
    assert(ptr != NULL);

    // test basic functionality of alloc and free
    memory_block_t* block = (memory_block_t*)ptr - 1;
    assert(block->size == alloc_size1);
    assert(block->active == 1);

    memory_block_t* next_block = block->next;
    assert(next_block != NULL);
    assert(next_block->size == (pool1->pool_size - 2 * sizeof(memory_block_t) - alloc_size1));
    assert(next_block->active == 0);

    memory_pool_free(pool1, ptr);
    assert(block->active == 0);
    assert(pool1->first->size == pool1->pool_size - sizeof(memory_block_t));
    printf("Test: \"basic alloc/free calls\" passed\n");

    // test if larger size memory request are placed correctly in the pool
    size_t alloc_size2 = 2048; // 2KB
    void* ptr1 = memory_pool_alloc(pool1, alloc_size1);
    void* ptr2 = memory_pool_alloc(pool1, alloc_size1);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    //free the first memory block
    memory_pool_free(pool1, ptr1);

    ptr1 = memory_pool_alloc(pool1, alloc_size2);
    assert(ptr1 != NULL);

    memory_block_t* block1 = pool1->first;
    //the first block was freed, from 1KB of data
    assert(block1->size == alloc_size1);
    assert(block1->active == 0);
    memory_block_t* block2 = block1->next;
    //the second block is still used, with 1KB of data
    assert(block2->size == alloc_size1);
    assert(block2->active == 1);
    memory_block_t* block3 = block2->next;
    //the third block is the newly allocated block, with 2KB of data
    //which means the first block was skipped and space after the second block was used
    assert(block3->size == alloc_size2);
    assert(block3->active == 1);
    memory_block_t* block4 = block3->next;
    //the size of the block 4 is the remaining space in the pool
    //|                                    size of pool                                    |
    //| header | 1KB (free) | header | 1KB (used) | header | 2KB (used) | header | rest ...|
    assert(block4->size == (pool1->pool_size - 4 * sizeof(memory_block_t) - 2 * alloc_size1 - alloc_size2));
    printf("Test: \"skip too small memory block\" passed\n");

    //test if the defragmentation works
    memory_pool_free(pool1, ptr2);
    //now the block1 & block2 should be merged
    //|                                    size of pool                                    |
    //| header |     2KB + size(header) (free)    | header | 2KB (used) | header | rest ...|
    assert(block1->size == alloc_size2 + sizeof(memory_block_t));
    assert(block1->active == 0);
    printf("Test: \"defragmentation works\" passed\n");

    //pointer out of pool bound can will not be freed
    void* ptr_pool2 = memory_pool_alloc(pool2, alloc_size1);
    memory_pool_free(pool1, ptr_pool2);
    memory_block_t* block_pool2 = pool2->first;
    assert(block_pool2->size == alloc_size1);
    assert(block_pool2->active == 1);
    printf("Test: \"pointers out of bound will be ignored\" passed\n");

    memory_pool_free(pool2, ptr_pool2);
    memory_pool_free(pool1, ptr1);

    printf("test_memory_alloc_free passed\n");
}

void tear_down(void) {
    shutdown_memory_pool(pool1);
    shutdown_memory_pool(pool2);
}

int main(void) {
    test_init_memory_pool();
    test_memory_alloc_free();
    tear_down();
    return 0;
}
