#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include "input_types.h"

#include <stdbool.h>

// Default buffer size can be overridden at compile time
#ifndef INPUT_BUFFER_SIZE
    #define INPUT_BUFFER_SIZE 32
#endif

/**
 * @brief Circular buffer for input events
 * 
 * Thread-safe circular buffer that stores input events.
 * Used to buffer inputs between the input thread and the game thread.
 */
typedef struct input_buffer_s input_buffer_t;

/**
 * @brief Initialize the input buffer
 * 
 * @param[out] buffer Pointer to the buffer to initialize
 * @return true on success, false on failure
 */
bool input_buffer_init(input_buffer_t** buffer);

/**
 * @brief Free the input buffer
 * 
 * @param buffer The buffer to free
 */
void input_buffer_free(input_buffer_t* buffer);

/**
 * @brief Write an input event to the buffer
 * 
 * Thread-safe function to add an input event to the buffer.
 * If the buffer is full, the oldest event is overwritten.
 * 
 * @param buffer The buffer to write to
 * @param event The event to write
 * @return true if the event was written, false on error
 */
bool input_buffer_write(input_buffer_t* buffer, const input_event_t* event);

/**
 * @brief Read an input event from the buffer
 * 
 * Thread-safe function to read the next input event from the buffer.
 * The event is removed from the buffer after reading.
 * 
 * @param buffer The buffer to read from
 * @param[out] event Pointer to store the read event
 * @return true if an event was read, false if the buffer is empty
 */
bool input_buffer_read(input_buffer_t* buffer, input_event_t* event);

/**
 * @brief Clear all events from the buffer
 * 
 * Thread-safe function to remove all events from the buffer.
 * 
 * @param buffer The buffer to clear
 */
void input_buffer_clear(input_buffer_t* buffer);

#endif// INPUT_BUFFER_H