#include "../../src/logging/ringbuffer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

ring_buffer_t test_ringbuffer;

void test_init_ringbuffer(void) {
    assert(init_ringbuffer(&test_ringbuffer) == 0);
    assert(test_ringbuffer.head == 0);
    assert(test_ringbuffer.tail == 0);
    assert(test_ringbuffer.count == 0);
    assert(test_ringbuffer.messages != NULL);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        assert(test_ringbuffer.messages[i] != NULL);
    }
    // mutex and condition variable should be initialized

    // not free the ringbuffer here, because it is used in other tests
    printf("test_init_ringbuffer passed\n");
}

void test_read_write_ringbuffer(void) {
    const char *test_message1 = "Test Message 1";
    const char *test_message2 = "Test Message 2";
    char buffer[32];

    write_to_ringbuffer(&test_ringbuffer, test_message1);
    assert(test_ringbuffer.count == 1);

    write_to_ringbuffer(&test_ringbuffer, test_message2);
    assert(test_ringbuffer.count == 2);

    assert(read_from_ringbuffer(&test_ringbuffer, buffer) == 0);
    assert(strcmp(buffer, test_message1) == 0);
    assert(test_ringbuffer.count == 1);

    assert(read_from_ringbuffer(&test_ringbuffer, buffer) == 0);
    assert(strcmp(buffer, test_message2) == 0);
    assert(test_ringbuffer.count == 0);

    printf("test_read_write_ringbuffer passed\n");
}

void tear_down(void) {
    free_ringbuffer(&test_ringbuffer);
    printf("test_free_ringbuffer passed\n");
}



int main(void) {
    test_init_ringbuffer();
    test_read_write_ringbuffer();
    tear_down();
    return 0;
}
