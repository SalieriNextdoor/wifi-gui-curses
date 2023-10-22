/*
 * This file contains macro definitions that may be used
 * for any other file.
 * */
#ifndef WIFI_COMMONS_H
#define WIFI_COMMONS_H
#include <stdlib.h>
#define FREEPTRARR(_nulltermarr)                               \
  for (int _iterv = 0; _nulltermarr[_iterv] != NULL; _iterv++) \
  free(_nulltermarr[_iterv])

#define FREEARR(_ptrarr, _delimm) \
  for (int _iterv = 0; _iterv < _delimm; _iterv++) free(_ptrarr[_iterv])

#endif
