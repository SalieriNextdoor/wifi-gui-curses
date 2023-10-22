/*
 *  This file contains functions dealing with running
 *  other programs, and parsing the output of those commands.
 * */
#include "commands.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

FILE *rawentry = NULL;

/*
 * Runs nmcli to get the list of available wifi networks.
 * Redirects the output to grep, which then redirects to sed,
 * as to further parse the input.
 * Finally, redirects the output to a newly created file located in
 * WIFIFILEPATH.
 * The output contains the fields IN-USE, SSID, RATE and SECURIY of
 * the nmcli command.
 *
 * Returns 0 if everything went correctly.
 * Otherwise returns 1.
 * */
int write_wifilist(void) {
  int pipe1[2];
  int pipe2[2];
  if (pipe(pipe1) == -1) return -1;
  pid_t rc;
  if ((rc = fork()) < 0)
    return -1;
  else if (rc == 0) {
    dup2(pipe1[1], 1);
    close(pipe1[0]);
    close(pipe1[0]);

    execlp("nmcli", "nmcli", "--mode=multiline", "device", "wifi", "list",
           NULL);
    // exec didn't work
    _exit(1);
  }

  if (pipe(pipe2) == -1) return -1;
  if ((rc = fork()) < 0)
    return -1;
  else if (rc == 0) {
    dup2(pipe1[0], 0);
    dup2(pipe2[1], 1);
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execlp("grep", "grep", "-E", "^(IN-USE|SSID|RATE|SECURITY)", NULL);
    // exec didn't work
    _exit(1);
  }

  close(pipe1[0]);
  close(pipe1[1]);
  int fd = creat(WIFIFILEPATH, S_IRWXU);

  if ((rc = fork()) < 0)
    return -1;
  else if (rc == 0) {
    dup2(pipe2[0], 0);
    dup2(fd, 1);
    close(pipe2[0]);
    close(pipe2[1]);
    execlp("sed", "sed", "-E", "s/^(\\w|-)*:[[:space:]]*//g", NULL);
    // exec didn't work
    _exit(1);
  }
  wait(NULL);
  close(pipe2[0]);
  close(pipe2[1]);
  close(fd);

  return 0;
}

/*
 *  Opens the file in WIFIFILEPATH to rawentry.
 *  Must be run after write_wifilist.
 *  Returns -1 if it wasn't able to open the file,
 *  otherwise returns 0.
 * */
int init_wififile(void) {
  sleep(1);
  rawentry = fopen(WIFIFILEPATH, "r");
  if (rawentry == NULL) return -1;
  return 0;
}

/*
 * Rewinds the position within the file back to start.
 * */
void rewind_wififile(void) { rewind(rawentry); }

/*
 *  Closes the file pointer if open.
 * */
void close_wififile(void) {
  if (rawentry != NULL) fclose(rawentry);
  rawentry = NULL;
}

/*
 * Parses the input for the next wifi network
 * in the file pointer and places each field
 * within 'entries'.
 * Returns -1 in case of EOF or getline error.
 * Returns 1 in case of bad malloc.
 * Otherwise returns 0.
 * */
int parse_next_wifi(char *entries[NENTRIES]) {
  char *line = NULL;
  size_t len = 0;
  for (int i = 0; i < NENTRIES; i++) {
    if (getline(&line, &len, rawentry) == -1) {
      if (line != NULL) free(line);
      return -1;
    }
    if (i == 0 && *line != '*')
      entries[i] = strdup("x\n");
    else if (i == 2) {
      line = strsep(&line, " ");
      entries[i] = strdup(line);
    } else if (i == 3 && strncmp(line, "WPA", 3) == 0)
      entries[i] = strdup("WPA");
    else
      entries[i] = strdup(line);
    if (entries[i] == NULL) {
      free(line);
      return 1;
    }
  }
  free(line);
  return 0;
}

/*
 *  Runs the rescan command for nmcli.
 *  Returns -1 in case of errors,
 *  otherwise returns 0.
 * */
int wifi_rescan(void) {
  pid_t rc = fork();
  if (rc < 0)
    return -1;
  else if (rc > 0)
    wait(NULL);
  else {
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("nmcli", "nmcli", "device", "wifi", "rescan", NULL);
    _exit(1);
  }
  return 0;
}

/*
 *  Runs the wifi connect command of nmcli
 *  for network name with password pas.
 *  Returns -1 in case of errors,
 *  otherwise returns 0.
 * */
int wifi_connect(char *name, char *pas) {
  pid_t rc = fork();
  if (rc < 0)
    return -1;
  else if (rc > 0)
    wait(NULL);
  else {
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("nmcli", "nmcli", "device", "wifi", "connect", name, "password", pas,
           NULL);
    _exit(1);
  }
  return 0;
}

/*
 *  Uses the wifi disconnect command for nmcli on network name.
 *  Returns -1 in case of errors,
 *  otherwise returns 0.
 * */
int wifi_disconnect(char *name) {
  pid_t rc = fork();
  if (rc < 0)
    return -1;
  else if (rc > 0)
    wait(NULL);
  else {
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("nmcli", "nmcli", "con", "down", "id", name, NULL);
    _exit(1);
  }
  return 0;
}

/*
 *  Runs write_wifilist and init_wififile.
 *  If rescan is true, runs wifi_rescan.
 *  Returns 1 if any of those functions had an error,
 *  otherwise returns 0.
 * */
int new_search(bool rescan) {
  if (write_wifilist() == -1) {
    return 1;
  }
  if (init_wififile() == -1) {
    return 1;
  }
  if (rescan && wifi_rescan() == -1) {
    return 1;
  }
  return 0;
}

/*
 *  Cleanup function.
 *  Runs close_wififile, and deletes the file in WIFIFILEPATH
 *  if it still exists.
 *  Returns -1 in case of errors,
 *  otherwise returns 0.
 * */
int wifi_cleanup(void) {
  close_wififile();
  pid_t rc = fork();
  if (rc < 0)
    return -1;
  else if (rc > 0)
    wait(NULL);
  else {
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("rm", "rm", "-f", WIFIFILEPATH, NULL);
    _exit(1);
  }
  return 0;
}

