#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

#define DETACH_BEGIN_END 1

static int directive_begin(xmlcell_t *x) {
  if (x == NULL)
    return 0;

  if (xml_get_name(x) != xos._xev_pragma &&
      xml_get_name(x) != xos._xevdrs_line)
    return 0;

  if (xml_chk_struct(x, "cn=xev_target"))
    return 1;

  return 0;
}

#if DETACH_BEGIN_END
static void insert_begin(xmlcell_t *x) {
  assert(x != NULL);
  assert(xml_get_name(x) == xos._xev_pragma ||
	 xml_get_name(x) == xos._xevdrs_line);

  xmlcell_t *b = xml_make_node(xos._xev_clause, xos._);
  xml_add_attr(b, xos._name, xos._begin);

  xml_append_child(xml_get_child(x), b);
}

static void insert_end(xmlcell_t *x) {
  assert(x != NULL);
  assert(xml_get_name(x) == xos._xev_pragma ||
	 xml_get_name(x) == xos._xevdrs_line);

  xmlcell_t *b = xml_make_node(xos._xev_clause, xos._);
  xml_add_attr(b, xos._name, xos._end);

  if (xml_get_name(x) == xos._xev_pragma)
    xml_insert_next(xml_get_child(xml_get_child(x)), b);
  else
    xml_append_child(xml_get_child(x), b);
}
#endif

void xev_destr_block(xmlcell_t *x) {
  if (x == NULL)
    return;

  if (directive_begin(x)) {
    strobj type = xml_get_name(x);

#if DETACH_BEGIN_END
    insert_begin(x);
#endif

    xmlcell_t *en = (xml_get_next(x) != NULL ? xml_cut_link(x, "n") : NULL);
    xmlcell_t *t = xml_cut_link(x, "cn");

    while (t != NULL) {
      xmlcell_t *n = (xml_get_child(t) != NULL ? xml_cut_link(t, "c") : NULL);

      xmlcell_t *l = xml_cut_link(t, "n");
      xmlcell_t *e = xml_make_node(type, xos.spaces[3]);
      xml_append_child(e, l);
#if DETACH_BEGIN_END
      insert_end(e);
#endif
      xml_set_posttext(e, xos.spaces[3]);

      t = (xml_get_next(l) == NULL ? NULL : xml_cut_link(l, "n"));
#if DETACH_BEGIN_END
      if (t != NULL)
	insert_begin(e);
#endif
      xml_append(x, n);
      xml_append(x, e);
    }

    xml_append(x, en);
  }

  xev_destr_block(xml_get_child(x));
  xev_destr_block(xml_get_next(x));
}
