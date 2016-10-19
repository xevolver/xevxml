#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

static char xevparse_filename[XEV_PATH_SIZE];
static char xevparse_line[XEV_LINE_SIZE];
static int xevparse_nline = -1;

void xevparse_setfileinfo(char *fname) {
  strcpy(xevparse_filename, fname);
  xevparse_nline = -1;
}

void xevparse_setlineinfo(char *line, int n) {
  strcpy(xevparse_line, line);
  xevparse_nline = n;
}

void xevparse_endfileinfo(void) {
  strcpy(xevparse_filename, "");
  xevparse_nline = -1;
}

void xevparse_putlineinfo(FILE *f) {
  if (strlen(xevparse_filename) == 0) {
    fprintf(stderr, "xevparse_putlineinfo: unexpected call\n");
    exit(1);
  }

  if (xevparse_nline < 0) {
    fprintf(stderr, "xevparse_putlineinfo: unexpected call for %s\n",
	    xevparse_filename);
    exit(1);
  }

  fprintf(f, "file:%s line:%d\n", xevparse_filename, xevparse_nline);
  fprintf(f, "%s\n", xevparse_line);
}
