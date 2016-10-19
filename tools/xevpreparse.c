#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
    exit(1);
  }

  init_xos();

  xevparse_load_conf();

  int ct = code_type(argv[1]);
  if (ct == CODE_TYPE_FORTRAN)
    preparse_f(argv[1], argv[2]);
  else {
    fprintf(stderr, "unknown code type: %s\n", argv[1]);
    exit(1);
  }

  return 0;
}
