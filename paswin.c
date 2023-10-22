/*
 *  This file includes functions related to the
 *  menu for password input.
 * */
#include <curses.h>

WINDOW *paswin;
WINDOW *typewin;

/*
 *  Handles the password input in the password input.
 *  Returns 1 if the user pressed <F3> (force exit),
 *  otherwise returns 0.
 * */
int get_pas(char *pas) {
  int c;
  char *pasbgn = pas;
  wmove(typewin, 1, 1);
  while ((c = wgetch(typewin)) != KEY_F(3) && c != '\n') {
    if (c == KEY_BACKSPACE && pas != pasbgn) {
      pas--;
      mvwaddch(typewin, 1, pas - pasbgn + 1, ' ');
      wmove(typewin, 1, pas - pasbgn + 1);
    } else if (c >= ' ' && c <= '~') {
      *pas++ = c;
      waddch(typewin, '*');
    }
  }
  *pas = '\0';
  if (c == KEY_F(3)) return 1;
  int y __attribute__((unused)), x;
  getmaxyx(paswin, y, x);
  mvwaddstr(paswin, 4, (x - 13) / 2, "Connecting...");
  wrefresh(paswin);
  return 0;
}

/*
 *  Draws the password menu.
 * */
void draw_paswin(void) {
  int y __attribute__((unused)), x;
  getmaxyx(paswin, y, x);
  box(paswin, 0, 0);
  wattron(paswin, A_BOLD);
  mvwaddstr(paswin, 3, (x - 19) / 2, "Enter the password:");
  wattroff(paswin, A_BOLD);
  box(typewin, 0, 0);
  wrefresh(paswin);
  wrefresh(typewin);
}

/*
 *  Initializes the password menu, with the windows
 *  paswin and typewin (input field).
 *  Also changes cursor state to 1.
 * */
int init_paswin(void) {
  paswin = newwin(LINES / 2, COLS * 3 / 4, LINES / 4, COLS / 8);
  typewin = newwin(3, COLS / 2, LINES / 2, COLS / 4);
  if (paswin == NULL || typewin == NULL) return 1;
  wtimeout(typewin, -1);
  keypad(typewin, TRUE);
  draw_paswin();
  curs_set(1);
  return 0;
}

/*
 *  Cleanup function.
 *  Clears and deletes created windows.
 *  Also returns cursor to 0 state.
 * */
void end_paswin(void) {
  curs_set(0);
  wclear(paswin);
  wclear(typewin);
  wrefresh(paswin);
  wrefresh(typewin);
  delwin(paswin);
  delwin(paswin);
}
