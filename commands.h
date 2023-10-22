/*
 *  This file contains constants and functions dealing with running
 *  other programs, and parsing the output of those commands.
 * */
#ifndef WIFICMDS_H
#define WIFICMDS_H
#include <stdbool.h>

#define NENTRIES 4
#define WIFIFILEPATH "/tmp/wifiutiltmpfile"

int write_wifilist(void);
int init_wififile(void);
void rewind_wififile(void);
void close_wififile(void);
int parse_next_wifi(char *entries[NENTRIES]);
int wifi_rescan(void);
int wifi_connect(char *name, char *pas);
int wifi_disconnect(char *name);
int new_search(bool rescan);
int wifi_cleanup(void);

#endif
