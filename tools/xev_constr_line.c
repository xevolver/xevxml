#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

static int directive_nobeginend(xmlcell_t *x) {
  if (x == NULL)
    return 0;

  if (xml_get_name(x) != xos._xev_pragma)
    return 0;

  xmlcell_t *cl = xml_get_cell(x, "cc");
  while (cl != NULL) {
    strobj s = xml_getstr(cl, "a=name");
    if (s == xos._begin || s == xos._end)
      return 0;

    cl = cl->next;
  }

  return 1;
}

void xev_constr_line(xmlcell_t *x) {
  if (x == NULL)
    return;

  if (directive_nobeginend(x)) {
    xmlcell_t *n = NULL;
    if (xml_get_next(x) != NULL)
      n = xml_cut_link(x, "n");

    xmlcell_t *t = xml_make_node(xos._xev_target, xos.spaces[0]);
    if (n != NULL)
      xml_append_child(t, n);
    xml_append_child(x, t);

    xev_constr_line(xml_get_child(x));

  } else {

    xev_constr_line(xml_get_child(x));
    xev_constr_line(xml_get_next(x));

  }
}
