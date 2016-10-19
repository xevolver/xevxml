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

  if (!xml_chk_struct(x, "cn=xev_target"))
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

void xev_destr_line(xmlcell_t *x) {
  if (x == NULL)
    return;

  if (directive_nobeginend(x)) {
    xmlcell_t *t = xml_cut_link(x, "cn");
    xmlcell_t *n = (xml_get_child(t) != NULL ? xml_cut_link(t, "c") : NULL);
    xmlcell_t *on = (xml_get_next(x) != NULL ? xml_cut_link(x, "n") : NULL);

    if (n != NULL)
      xml_append(x, n);
    if (on != NULL)
      xml_append(x, on);
    free_xml(t);

    xev_destr_line(xml_get_next(x));

  } else {

    xev_destr_line(xml_get_child(x));
    xev_destr_line(xml_get_next(x));

  }
}
