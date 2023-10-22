/*
 *  This file contains functions related to the workings
 *  of the main menu of the program.
 * */
#include <curses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "paswin.h"

bool forced_exit = false;
char *names[100];
int namecnt = 0;

/*
 *  Draws the inner part of the main menu (everything except
 *  for the surrounding box).
 * */
void draw_inner_menu(WINDOW *menuwin, int linecnt, int cnt,
                     char *entries[NENTRIES], char **connected, bool search,
                     int *highlight) {
  bool change_connect = false;
  char *str;
  if (cnt == *highlight) wattron(menuwin, A_REVERSE);
  wmove(menuwin, linecnt, 1);
  for (int i = 0; i < NENTRIES; i++) {
    if (i == 0) {
      if (*entries[i] == '*') {
        if (*connected != NULL) free(*connected);
        change_connect = true;
        waddstr(menuwin, "* ");
      } else
        waddstr(menuwin, "  ");
      free(entries[i]);
    } else {
      str = strsep(&entries[i], "\n");
      waddstr(menuwin, str);
      if (i == 1) {
        if (change_connect) {
          *connected = strdup(str);
          change_connect = false;
        }
        // this call to mbstowcs counts the amount of characters
        // in UTF-8 encoding properly
        for (int j = 0; j < 20 - mbstowcs(NULL, str, 0); j++)
          waddch(menuwin, ' ');
        if (search) names[namecnt++] = strdup(str);
      } else if (i == 2) {
        for (int j = 0; j < 4 - mbstowcs(NULL, str, 0); j++)
          waddch(menuwin, ' ');
        waddstr(menuwin, " Mbit/s ");
      } else
        waddch(menuwin, ' ');
      free(str);
    }
  }
  if (cnt == *highlight) wattroff(menuwin, A_REVERSE);
}

/*
 *  Draws the menu for when it is in a loading state.
 * */
void menu_loading(WINDOW *menuwin, char *loadstr) {
  int y __attribute__((unused)), x;
  getmaxyx(menuwin, y, x);
  werase(menuwin);
  mvwaddstr(menuwin, 1, (x - strlen(loadstr)) / 2, loadstr);
  box(menuwin, 0, 0);
  wrefresh(menuwin);
}

/*
 * Initializes the menu window.
 * Returns NULL in case of bad malloc,
 * otherwise returns the window pointer.
 */
WINDOW *init_menuwin(void) {
  WINDOW *menuwin = newwin(LINES - 4, COLS / 2, 2, COLS / 4);
  if (menuwin == NULL) {
    forced_exit = true;
    return NULL;
  }
  wtimeout(menuwin, 0);
  keypad(menuwin, TRUE);
  menu_loading(menuwin, "Searching...");
  return menuwin;
}

/*
 * Does the frontend handling for when the user
 * selects a network and inserts the password.
 * Returns 1 in case of errors,
 * otherwise returns 0.
 * */
int handle_connection(WINDOW *menuwin, char **connected, int highlight) {
  bool same = false;
  char *pas = malloc(50 * sizeof(char));
  if (pas == NULL) return 1;
  int rv = 0;
  if (*connected != NULL) {
    if (strcmp(*connected, names[highlight - 1]) == 0) same = true;
    rv = wifi_disconnect(*connected);
    free(*connected);
    *connected = NULL;
    menu_loading(menuwin, "Disconnecting...");
  }
  if (!same) {
    werase(menuwin);
    wrefresh(menuwin);
    if (init_paswin() || get_pas(pas))
      rv = 1;
    else
      rv = wifi_connect(names[highlight - 1], pas);
    end_paswin();
    menu_loading(menuwin, "Connecting...");
  }
  free(pas);
  return rv;
}
