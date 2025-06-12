/**
 * @file language_menu.h
 * @brief Exposes functions for the language menu.
 */
#ifndef LANGUAGE_MENU_H
#define LANGUAGE_MENU_H

#include "menu.h"

/**
 * @brief Initializes the language menu with the needed data.
 */
int init_language_menu(void);

/**
 * @brief Draws the language menu to the screen.
 *
 * @return A menu_result_t enum for the next language menu state.
 */
menu_result_t show_language_menu(void);

/**
 * @brief Shuts down the language menu and frees all associated data.
 */
void shutdown_language_menu(void);

#endif//LANGUAGE_MENU_H
