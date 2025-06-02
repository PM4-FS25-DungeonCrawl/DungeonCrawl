/**
 * @file ascii.h
 * @brief Exposes ascii art strings and metadata for those strings.
 */
#ifndef ASCII_H
#define ASCII_H

#define HELMET_WIDTH 29
#define HELMET_HEIGHT 19

#define GOBLIN_WIDTH 30
#define GOBLIN_HEIGHT 11

static const char ascii_helmet[HELMET_HEIGHT][HELMET_WIDTH] = {
        "                _           ",
        "                ,''/., _    ",
        "        `.-._\\`/. ( //'/'`. ",
        "      _.-`-. ``' ` `(   -. \\",
        "    ,'  ,    ,-:._ _..-.. \\/",
        "   / ,'/ ,`.( _.'-'.     )/ ",
        "   `.\\ '(   ,'      `.      ",
        "      `._\\ /'       \\ \\     ",
        "          /:         \\ \\-.  ",
        "        ,;':._______...-'_) ",
        "        \\:/-.._______..-_|  ",
        "         : :\\   `----'|'-;  ",
        "          \\ :\\    : : ;:/   ",
        "           \\ ``.   ; /;/    ",
        "            )   `.  /,'     ",
        "          ,'      `-' \\     ",
        "         /  .--.       )    ",
        "        /_.---._`._   /     ",
        "                `.__.'      "};

static const char* ascii_goblin __attribute__((unused)) = {
        "             ,      ,       \n"
        "            /(.-\"\"-.)\\    \n"
        "        |\\  \\/      \\/  /|  \n"
        "        | \\ / =.  .= \\ / |  \n"
        "        \\( \\   o\\/o   / )/  \n"
        "         \\_, '-/  \\-' ,_/   \n"
        "           /   \\__/   \\     \n"
        "           \\ \\__/\\__/ /     \n"
        "         ___\\ \\|--|/ /___   \n"
        "       /`    \\      /    `\\ \n"
        "      /       '----'       \\\n"};

#endif//ASCII_H
