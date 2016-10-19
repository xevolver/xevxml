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

  xmlcell_t *x = read_xml(fs);
  fclose(fs);
  check_xmlformat(x);
  xml_set_dynamic_all(x);

  if (x == NULL) {
    fprintf(stderr, "xevunparse: %s has no xml\n", argv[1]);
    exit(1);
  }

  strobj fname = xml_getstr(x, "na=file");

  /* remove xevparse info */

  if (xml_getstr(x, "ncc") == xos._xev_pragma &&
      (((xml_getstr(x, "ncccca=name") == xos._xevparse ||
	 xml_getstr(x, "ncccca=name") == xos._xps) &&
	xml_getstr(x, "nccccna=name") == xos._info) ||
       (xml_getstr(x, "nccccna=name") == xos._xevparse &&
	xml_getstr(x, "nccccnna=name") == xos._info))) {

    xmlcell_t *g = xml_get_cell(x, "nc");
    assert(xml_get_name(g) == xos._SgGlobal);
    xmlcell_t *p0 = xml_cut_link(g, "c");
    xmlcell_t *b = xml_cut_link(p0, "n");
    xml_append_child(g, b);
    free_xml(p0);

    while (xml_get_next(xml_get_next(b)) != NULL)
      b = xml_get_next(b);
    xmlcell_t *p1 = xml_cut_link(b, "n");
    free_xml(p1);
  }
  
  /* other processing */

  int ct = code_type(fname);
  if (ct == CODE_TYPE_FORTRAN)
    unparse_f(x, argv[2]);
  else {
    fprintf(stderr, "unknown file type: %s\n", argv[2]);
    exit(1);
  }

  return 0;
}
