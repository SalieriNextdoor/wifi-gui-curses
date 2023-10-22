/*
 *  This file contains functions related to the workings
 *  of the main menu of the program.
 * */
#ifndef WIFI_MAINMENU_H
#define WIFI_MAINMENU_H
#include <curses.h>

#include "commands.h"
void draw_inner_menu(WINDOW *menuwin, int linecnt, int cnt,
                     char *entries[NENTRIES], char **connected, bool search,
                     int *highlight);
void menu_loading(WINDOW *menuwin, char *loadstr);
WINDOW *init_menuwin(void);
int handle_connection(WINDOW *menuwin, char **connected, int highlight);

#endif
