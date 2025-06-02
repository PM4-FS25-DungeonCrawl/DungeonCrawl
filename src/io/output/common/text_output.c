
#include "text_output.h"

#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../input/input_handler.h"
#include "../../input/input_types.h"
#include "../../io_handler.h"

#include <notcurses/nckeys.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <unistd.h>// for usleep
#else
    #include <windows.h>
#endif

void print_text(int y, int x, const char* text, uint64_t ncchannel) {
    if (!gio->stdplane || !text) {
        log_msg(ERROR, "output_handler", "Output handler not initialized or null text");
        return;
    }

    // Set the channels and print the text
    ncplane_set_channels(gio->stdplane, ncchannel);
    ncplane_putstr_yx(gio->stdplane, y, x, text);
}

void print_text_default(int y, int x, const char* text) {
    print_text(y, x, text, DEFAULT_COLORS);
}

void print_text_multi_line(int y, int x, const char* text, int max_width, uint64_t ncchannel) {
    if (!gio->stdplane || !text || max_width <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for print_text_multi_line");
        return;
    }

    // Set the channels
    ncplane_set_channels(gio->stdplane, ncchannel);

    // Handle line wrapping
    const char* ptr = text;
    int current_y = y;
    int line_len = 0;
    char line_buffer[256];// Assuming max line length

    while (*ptr) {
        // Copy characters until max_width or newline
        line_len = 0;
        while (*ptr && *ptr != '\n' && line_len < max_width) {
            line_buffer[line_len++] = *ptr++;
        }
        line_buffer[line_len] = '\0';

        // Print this line
        ncplane_putstr_yx(gio->stdplane, current_y, x, line_buffer);
        current_y++;

        // Handle newline character
        if (*ptr == '\n') {
            ptr++;
        }
    }
}

void print_text_multi_line_default(int y, int x, const char* text, int max_width) {
    print_text_multi_line(y, x, text, max_width, DEFAULT_COLORS);
}

void print_text_multi_strings(int y, int x, const char* text[], int count, uint64_t ncchannel) {
    if (!gio->stdplane || !text || count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for print_text_multi_strings");
        return;
    }

    // Set the channels
    ncplane_set_channels(gio->stdplane, ncchannel);

    // Print each string on a new line
    for (int i = 0; i < count; i++) {
        if (text[i]) {
            ncplane_putstr_yx(gio->stdplane, y + i, x, text[i]);
        }
    }
}

void print_text_multi_strings_default(int y, int x, const char* text[], int count) {
    print_text_multi_strings(y, x, text, count, DEFAULT_COLORS);
}

void print_menu(const char* title, const char** options, int option_count,
                int selected_index, int y, int x,
                uint64_t title_channel,
                uint64_t option_channel,
                uint64_t selected_channel) {
    if (!gio->stdplane || !title || !options || option_count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid menu parameters");
        return;
    }

    // Print title
    ncplane_set_channels(gio->stdplane, title_channel);
    ncplane_putstr_yx(gio->stdplane, y, x, title);

    // Print options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option
            ncplane_set_channels(gio->stdplane, selected_channel);
        } else {
            ncplane_set_channels(gio->stdplane, option_channel);
        }
        ncplane_putstr_yx(gio->stdplane, y + i + 1, x, options[i]);
    }
}

void print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x) {
    print_menu(title, options, option_count, selected_index, y, x,
               DEFAULT_COLORS, DEFAULT_COLORS, INVERTED_COLORS);
}

bool get_text_input(const char* prompt, char* buffer, int buffer_size,
                    const char* confirm_msg, int y, int x) {
    if (!gio->stdplane || !buffer || buffer_size <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for get_text_input");
        return false;
    }

    // Clear the buffer
    memset(buffer, 0, buffer_size);
    int text_length = 0;
    bool input_active = true;
    bool confirmed = false;

    while (input_active) {
        // Clear screen
        clear_screen();

        // Display prompt
        print_text_default(y, x, prompt);

        // Display current input
        print_text_default(y + 2, x, buffer);

        // Display confirm message if provided
        if (confirm_msg) {
            print_text_default(y + 4, x, confirm_msg);
        }

        // Render the frame
        render_frame();

        // Get input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Process input
        uint32_t key_id = input_event.raw_input.id;

        if (input_event.type == INPUT_CONFIRM && text_length > 0) {
            // Enter was pressed and we have input
            input_active = false;
            confirmed = true;
        } else if (input_event.type == INPUT_CANCEL) {
            // Escape was pressed
            input_active = false;
            confirmed = false;
        } else if (key_id == NCKEY_BACKSPACE && text_length > 0) {
            // Backspace was pressed and we have characters to delete
            buffer[--text_length] = '\0';
        } else if (key_id != 0 && text_length < buffer_size - 1 &&
                   !(input_event.type == INPUT_UP ||
                     input_event.type == INPUT_DOWN ||
                     input_event.type == INPUT_LEFT ||
                     input_event.type == INPUT_RIGHT)) {
            // A printable character was typed
            buffer[text_length++] = key_id;
            buffer[text_length] = '\0';
        }
    }

    return confirmed;
}

void show_message_screen(const char* message, const char* continue_message, int y, int x) {
    if (!gio->stdplane || !message) {
        log_msg(ERROR, "output_handler", "Invalid parameters for show_message_screen");
        return;
    }

    // Clear screen
    clear_screen();

    // Display message
    print_text_default(y, x, message);

    // Display continue message if provided
    if (continue_message) {
        print_text_default(y + 2, x, continue_message);
    }

    // Render the frame
    render_frame();

    // Wait for any input to continue
    input_event_t input_event;
    while (!get_input_blocking(&input_event));
}

static uint64_t apply_transparency(uint64_t channel, text_transparency_t transparency) {
    switch (transparency) {
        case TEXT_TRANSPARENCY_TEXT_ONLY:
            // Make background transparent
            channel = ncchannels_set_bg_alpha(&channel, NCALPHA_TRANSPARENT);
            break;
        case TEXT_TRANSPARENCY_BACKGROUND_LIGHT:
            // 25% background visibility
            channel = ncchannels_set_bg_alpha(&channel, NCALPHA_BLEND);
            break;
        case TEXT_TRANSPARENCY_BACKGROUND_MEDIUM:
            // 50% background visibility
            channel = ncchannels_set_bg_alpha(&channel, NCALPHA_BLEND);
            break;
        case TEXT_TRANSPARENCY_BACKGROUND_HEAVY:
            // 75% background visibility  
            channel = ncchannels_set_bg_alpha(&channel, NCALPHA_BLEND);
            break;
        case TEXT_TRANSPARENCY_NONE:
        default:
            // No transparency changes
            break;
    }
    return channel;
}

static int calculate_aligned_x(int base_x, const char* text, text_align_t alignment, int width) {
    if (!text || width <= 0) {
        return base_x;
    }
    
    int text_len = strlen(text);
    
    switch (alignment) {
        case TEXT_ALIGN_CENTER:
            return base_x + (width - text_len) / 2;
        case TEXT_ALIGN_RIGHT:
            return base_x + width - text_len;
        case TEXT_ALIGN_LEFT:
        default:
            return base_x;
    }
}

void print_text_formatted(int y, int x, const char* text, uint64_t ncchannel,
                          text_align_t alignment, text_style_t style,
                          text_transparency_t transparency, int width) {
    if (!gio->stdplane || !text) {
        log_msg(ERROR, "output_handler", "Output handler not initialized or null text");
        return;
    }

    // Apply transparency to channel
    uint64_t modified_channel = apply_transparency(ncchannel, transparency);
    
    // Calculate aligned X position
    int aligned_x = (width > 0) ? calculate_aligned_x(x, text, alignment, width) : x;
    
    // Set the channels and styles
    ncplane_set_channels(gio->stdplane, modified_channel);
    ncplane_set_styles(gio->stdplane, style);
    
    // Print the text
    if (width > 0 && alignment != TEXT_ALIGN_LEFT) {
        // Use notcurses alignment function for non-left alignment - cast to suppress warning
        ncplane_putstr_aligned(gio->stdplane, y, (ncalign_e)alignment, text);
    } else {
        ncplane_putstr_yx(gio->stdplane, y, aligned_x, text);
    }
    
    // Reset styles to normal
    ncplane_set_styles(gio->stdplane, TEXT_STYLE_NORMAL);
}

void print_text_aligned(int y, int x, const char* text, text_align_t alignment, int width) {
    print_text_formatted(y, x, text, DEFAULT_COLORS, alignment, TEXT_STYLE_NORMAL, TEXT_TRANSPARENCY_NONE, width);
}

void print_text_styled(int y, int x, const char* text, text_style_t style, uint64_t ncchannel) {
    print_text_formatted(y, x, text, ncchannel, TEXT_ALIGN_LEFT, style, TEXT_TRANSPARENCY_NONE, 0);
}

void print_text_transparent(int y, int x, const char* text, text_transparency_t transparency, uint64_t ncchannel) {
    print_text_formatted(y, x, text, ncchannel, TEXT_ALIGN_LEFT, TEXT_STYLE_NORMAL, transparency, 0);
}