#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

#define DEBUG 0

int main(int argc, char **argv) {
  if (argc < 3 && argc > 5) {
    fprintf(stderr, "usage: %s flags infile outfile\n", argv[0]);
    fprintf(stderr, "flags: +b -b (begin-end block) +c (continuation)\n");
    exit(1);
  }

  init_xos();

  char *infile = argv[argc-2];
  char *outfile = argv[argc-1];

  FILE *fs = fopen(infile, "r");
  if (fs == NULL) {
    fprintf(stderr, "cannot open %s\n", infile);
    exit(1);
  }

  xmlcell_t *x = read_xml(fs);
  fclose(fs);
  //  check_xmlformat(x);
  xml_set_dynamic_all(x);

  if (x == NULL) {
    fprintf(stderr, "xevrestruct: %s has no xml\n", infile);
    exit(1);
  }

  int b_on, b_off, c_on, c_off;
  b_on = b_off = c_on = c_off = 0;
  if (argc > 3) {
    if (strcmp(argv[1], "+b") == 0)
      b_on = 1;
    else if (strcmp(argv[1], "-b") == 0)
      b_off = 1;
    else if (strcmp(argv[1], "+c") == 0)
      c_on = 1;
    else if (strcmp(argv[1], "-c") == 0)
      c_off = 1;
    else {
      fprintf(stderr, "xevrestruct: unknown flag %s\n", argv[1]);
      exit(1);
    }
  }

  if (argc > 4) {
    if (strcmp(argv[1], argv[2]) == 0) {
      fprintf(stderr, "xevrestruct: duplicated flags %s\n", argv[1]);
      exit(1);
    }

    if (strcmp(argv[2], "+b") == 0)
      b_on = 1;
    else if (strcmp(argv[2], "-b") == 0)
      b_off = 1;
    else if (strcmp(argv[2], "+c") == 0)
      c_on = 1;
    else if (strcmp(argv[2], "-c") == 0)
      c_off = 1;
    else {
      fprintf(stderr, "xevrestruct: unknown flag %s\n", argv[2]);
    }

    if ((b_on && b_off) || (c_on && c_off)) {
      fprintf(stderr, "xevrestruct: incompatible flags %s %s\n", argv[1], argv[2]);
      exit(1);
    }
  }

  if (b_on) {
#if DEBUG
    printf("constr_block\n");
#endif
    xev_constr_block(x);
  }
  if (b_off) {
#if DEBUG
    printf("destr_block\n");
#endif
    xev_destr_block(x);
  }
  if (c_on) {
    xevparse_contplus(x);
  }
  if (c_off) {
    xevparse_contminus(x);
  }

  FILE *fd = fopen(outfile, "w");
  if (fd == NULL) {
    fprintf(stderr, "cannot open %s\n", outfile);
    exit(1);
  }

  fprint_xml(fd, x);
  fprintf(fd, "\n");

  fclose(fd);

  free_xml(x);

  return 0;
}
    
