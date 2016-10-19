#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevtgen.h"

#define DEBUG 0

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
    exit(1);
  }

  init_xos();

  xev_log3("* xevtgen", argv[1], argv[2]);
  
  xevtgen_init();

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "cannot open %s\n", argv[1]);
    exit(1);
  }

  xmlcell_t *x = read_xml(f);
  fclose(f);
  check_xmlformat(x);

  xev_log2("output template", argv[2]);

  FILE *fo = fopen(argv[2], "w");
  if (fo == NULL) {
    fprintf(stderr, "cannot open %s\n", argv[2]);
    exit(1);
  }

  fprintf(fo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(fo, "<xsl:stylesheet version=\"1.0\" "
	  "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n");
  fprintf(fo, "\n");

  xevtgen_count_variables(x, fo); // actually output some

  if (xevtgen_mode() != NULL)
    fprintf(fo, "  <xsl:template mode=\"%s\" match=\"*\">\n", xevtgen_mode());
  else
    fprintf(fo, "  <xsl:template match=\"*\">\n");
  xevtgen_write_context_param(fo);
  xevtgen_write_varparams(fo);
  fprintf(fo, "    <xsl:choose>\n");

  xevtgen_write_template(x, fo);
  xevtgen_write_otherwise(fo);

  fprintf(fo, "    </xsl:choose>\n");
  fprintf(fo, "  </xsl:template>\n");
  fprintf(fo, "\n");
  fprintf(fo, "</xsl:stylesheet>\n");

  fclose(fo);

  free_xml(x);

  xev_log4("* xevtgen", argv[1], argv[2], "finished");

  return 0;
}
