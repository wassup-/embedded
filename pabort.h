#ifndef PABORT_H_
#define PABORT_H_

#include <stdio.h>
#include <stdlib.h>

static void pabort(const char *s) {
  perror(s);
  abort();
}

#endif
