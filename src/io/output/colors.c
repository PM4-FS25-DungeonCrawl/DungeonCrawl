#include "colors.h"
#include <notcurses/notcurses.h>

uint64_t make_channels(uint32_t fg, uint32_t bg) {
    uint64_t channels = 0;
    
    // Set the foreground RGB values
    ncchannels_set_fg_rgb8(&channels, 
                           COLOR_R(fg),
                           COLOR_G(fg),
                           COLOR_B(fg));
    
    // Set the background RGB values
    ncchannels_set_bg_rgb8(&channels, 
                           COLOR_R(bg),
                           COLOR_G(bg),
                           COLOR_B(bg));
    
    return channels;
}