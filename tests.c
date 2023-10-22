/*
 *  This file contains unit tests for the backend (commands.c).
 * */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "commands.h"

extern FILE *rawentry;

void commands_write_wifilist(void) { assert(!write_wifilist()); }

void commands_init_wififile(void) {
  char *line = NULL;
  size_t len = 0;
  assert(!init_wififile());
  assert(getline(&line, &len, rawentry) != -1);
  if (line != NULL) free(line);
}

void commands_rewind_wififile(void) {
  rewind_wififile();
  assert(ftell(rawentry) == 0);
}

void commands_parse_next_wifi(void) {
  char *entries[NENTRIES];
  assert(!parse_next_wifi(entries));
  for (int i = 0; i < NENTRIES; i++) {
    assert(entries[i] != NULL);
    if (i != 0) assert(*entries[i] != '\n');
    free(entries[i]);
  }
}

void commands_wifi_rescan(void) { assert(!wifi_rescan()); }

void commands_wifi_connect(void) { assert(!wifi_connect("test", "test")); }

void commands_wifi_disconnect(void) { assert(!wifi_disconnect("test")); }

void commands_new_search(void) {
  assert(!new_search(false));
  close_wififile();
  assert(!new_search(true));
}

void commands_close_wififile(void) {
  close_wififile();
  assert(rawentry == NULL);
}

void commands_wifi_cleanup(void) {
  init_wififile();
  assert(!wifi_cleanup());
  sleep(1);
  assert(access(WIFIFILEPATH, F_OK));
}

void test_commands(void) {
  commands_write_wifilist();
  commands_init_wififile();
  commands_rewind_wififile();
  commands_parse_next_wifi();
  commands_wifi_rescan();
  commands_wifi_connect();
  commands_wifi_disconnect();
  commands_new_search();
  commands_close_wififile();
  commands_wifi_cleanup();
}

int main() {
  test_commands();
  return 0;
}
