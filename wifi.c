/*
 *  Creates a Linux curses terminal GUI for nmcli.
 *  Up and down arrows are used to browse between networks,
 *  enter to select a network and R to rescan for networks
 *  (five second delay between scans).
 * */
#include <curses.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "commands.h"
#include "commons.h"
#include "mainmenu.h"
#include "paswin.h"

extern bool forced_exit;
extern char *names[100];
extern int namecnt;

/*
 *  Initialization function for curses.
 * */
void init_curses(void) {
  initscr();
  clear();
  cbreak();
  noecho();
  curs_set(0);
  start_color();
  setlocale(LC_ALL, "");
}

/*
 *  Cleanup function.
 *  Clears stdscr and the menu window and deletes it.
 *  Also calls wifi_cleanup.
 * */
void cleanup(WINDOW *menuwin) {
  clear();
  refresh();
  wclear(menuwin);
  wrefresh(menuwin);
  delwin(menuwin);
  wifi_cleanup();
}

int main() {
  WINDOW *menuwin;
  int c;

  init_curses();

  addstr("<ENTER> to select\n<R>     to rescan\n<F3>    to exit");
  refresh();

  int y = 0, x __attribute__((unused));
  if ((menuwin = init_menuwin()) != NULL) getmaxyx(menuwin, y, x);

  if (new_search(false)) forced_exit = true;

  int rc;
  int linecnt = 1;
  int cnt = 1;
  names[0] = NULL;
  char *entries[NENTRIES];
  char *connected = NULL;
  bool draw = true;
  bool search = true;
  int highlight = 1;
  int skip = -1;
  time_t last_refresh = 0;
  time_t current;
  while (!forced_exit && (c = wgetch(menuwin)) != KEY_F(3)) {
    while (linecnt < y && draw && (rc = parse_next_wifi(entries)) != -1 &&
           rc != 1) {
      // creates a scrolling functionality
      if ((skip < 0 && cnt > y - 2) || skip > 0) {
        FREEARR(entries, NENTRIES);
        skip--;
        cnt++;
        continue;
      }
      draw_inner_menu(menuwin, linecnt, cnt, entries, &connected, search,
                      &highlight);
      linecnt++;
      cnt++;
    }
    if (rc == 1) {
      forced_exit = true;
      FREEARR(entries, NENTRIES);
      break;
    }
    names[namecnt] = NULL;
    if (draw) {
      draw = search = false;
      rewind_wififile();
    }
    switch (c) {
      case KEY_UP:
        draw = true;
        if (highlight == 1)
          highlight = cnt - 1;
        else
          highlight--;
        break;
      case KEY_DOWN:
        draw = true;
        if (highlight == cnt - 1)
          highlight = 1;
        else
          highlight++;
        break;
      case 'r':
      case 'R':
        if ((current = time(NULL)) - last_refresh > 5) {
          last_refresh = current;
          draw = search = true;
        }
        break;
      case '\n':
        if (handle_connection(menuwin, &connected, highlight)) {
          forced_exit = true;
          break;
        }
        sleep(2);
        draw = search = true;
        break;
    }
    if (draw) {
      werase(menuwin);
      skip = highlight - (y - 2);
      linecnt = cnt = 1;
      box(menuwin, 0, 0);
    }
    if (search) {
      menu_loading(menuwin, "Searching...");

      close_wififile();
      if (new_search(true)) forced_exit = true;

      FREEPTRARR(names);
      names[0] = NULL;
      highlight = 1;
    }
  }

  FREEPTRARR(names);
  cleanup(menuwin);
  endwin();
  return forced_exit ? 1 : 0;
}
