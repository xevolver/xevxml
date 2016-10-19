#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "xevutil.h"

static int depth;

static void indent(xmlcell_t *x) {

  char *t = xml_getstr(x, "s");

  if (t != NULL) {

    char *p;			/* first non-space char */
    for (p = t; *p != '\0'; p++)
      if (!isspace(*p))
	break;

    char *q;			/* last non-space char */
    for (q = t + strlen(t) - 1; q >= t; q --)
      if (!isspace(*q))
	break;
  
    if (p < q) {			/* there is some non-space chars */
#if 1
      int l = q - p + 1 + 3 + 2 * depth;
      char s[l];
      assert(s != NULL);

      s[0] = '\n';

      char *z = s + 1;
      while (p <= q)
	*(z++) = *(p++);

      *(z++) = '\n';
      int i;
      for (i=0; i< 2 * depth; i++)
	*(z++) = ' ';
      *z = '\0';

      assert(z - s == l - 1);

      if (x->node->type != XML_TYPE_DEC)
	xml_reset_pretext(x, get_strobj(s));
#endif
    } else {
      char s[2 + 2 * depth];

      s[0] = '\n';
      int i;
      for (i=0; i< 2 * depth; i++)
	s[1+i] = ' ';
      s[1+i] = '\0';

      if (x->node->type != XML_TYPE_DEC)
	xml_reset_pretext(x, get_strobj(s));
    }
  } else {
    char s[2 + 2 * depth];

    s[0] = '\n';
    int i;
    for (i=0; i< 2 * depth; i++)
      s[1+i] = ' ';
    s[1+i] = '\0';

    if (x->node->type != XML_TYPE_DEC)
      xml_reset_pretext(x, get_strobj(s));
  }

  depth ++;

  if (xml_get_child(x) != NULL)
    indent(xml_get_child(x));
  
  depth --;

  t = xml_getstr(x, "t");

  if (t != NULL) {

    char *p;			/* first non-space char */
    for (p = t; *p != '\0'; p++)
      if (!isspace(*p))
	break;

    char *q;			/* last non-space char */
    for (q = t + strlen(t) - 1; q >= t; q --)
      if (!isspace(*q))
	break;
  
    if (p < q) {			/* there is some non-space chars */
#if 0
      int l = q - p + 1 + 3 + 2 * depth;
      char s[l];
      assert(s != NULL);

      s[0] = '\n';

      char *z = s + 1;
      while (p <= q)
	*(z++) = *(p++);

      *(z++) = '\n';
      int i;
      for (i=0; i< 2 * depth; i++)
	*(z++) = ' ';
      *z = '\0';

      assert(z - s == l - 1);

      xml_reset_posttext(x, get_strobj(s));
#endif
    } else {
      char s[2 + 2 * depth];

      s[0] = '\n';
      int i;
      for (i=0; i< 2 * depth; i++)
	s[1+i] = ' ';
      s[1+i] = '\0';

      xml_reset_posttext(x, get_strobj(s));
    }
  } else {
    char s[2 + 2 * depth];

    s[0] = '\n';
    int i;
    for (i=0; i< 2 * depth; i++)
      s[1+i] = ' ';
    s[1+i] = '\0';

    xml_reset_posttext(x, get_strobj(s));
  }

  if (xml_get_next(x) != NULL)
    indent(xml_get_next(x));
}

int main(int argc, char **argv) {
  if (argc != 3 && argc != 2) {
    fprintf(stderr, "usage: %s infile [oufile]\n", argv[0]);
    exit(1);
  }

  init_xos();

  FILE *f;

  f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "cannot open %s\n", argv[1]);
    exit(1);
  }

  xmlcell_t *x = read_xml(f);
  fclose(f);
  xml_set_dynamic_all(x);

  depth = 0;
  indent(x);

  char *fname;
  if (argc == 2)
    fname = argv[1];
  else
    fname = argv[2];

  f = fopen(fname, "w");
  if (f == NULL) {
    fprintf(stderr, "cannot open %s\n", fname);
    exit(1);
  }

  fprint_xml(f, x);
  fprintf(f, "\n");
  fclose(f);

  free_xml(x);

  return 0;
}
