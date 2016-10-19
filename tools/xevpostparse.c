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

  FILE *fs = fopen(argv[1], "r");
  if (fs == NULL) {
    fprintf(stderr, "cannot open %s\n", argv[1]);
    exit(1);
  }
  xevparse_setfileinfo(argv[1]);

  xmlcell_t *x = read_xml(fs);
  fclose(fs);

  check_xmlformat(x);
  xml_set_dynamic_all(x);

  strobj orgfile = xml_getstr(x, "na=file");
  //  printf("orgfile = %s\n", orgfile);
  int ct = code_type(orgfile);

  if (ct == CODE_TYPE_FORTRAN)
    postparse_f(x, argv[2]);
  else {
    fprintf(stderr, "unknown code type: %s\n", orgfile);
    exit(1);
  }

  free_xml(x);

  return 0;
}
