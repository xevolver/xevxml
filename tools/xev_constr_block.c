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
    return 0;

  //  xmlcell_t *cl = xml_get_cell(x, "cc");
  xmlcell_t *cl = xml_get_child(xml_get_child(x));
  while (cl != NULL) {
    if (xml_getstr(cl, "a=name") == xos._begin)
      return 1;

    cl = xml_get_next(cl);
  }

  return 0;
}

#if DETACH_BEGIN_END
static void detach_begin(xmlcell_t *x) {
  assert(x != NULL);
  assert(xml_get_name(x) == xos._xev_pragma ||
	 xml_get_name(x) == xos._xevdrs_line);

  xmlcell_t *c = xml_get_child(x);
  assert(c != NULL);

  if (xml_getstr(c, "ca=name") == xos._begin) {
    xmlcell_t *n = xml_detach_cell(c, "c");
    assert(n != NULL);
    free_xml(n);
  } else {
    xmlcell_t *cl = xml_get_child(c);
    assert(cl != NULL);

    while (xml_getstr(cl, "na=name") != xos._begin)
      cl = xml_get_next(cl);
    assert(cl != NULL);

    xmlcell_t *n = xml_detach_cell(cl, "n");
    assert(n != NULL);
    free_xml(n);
  }
}
#endif

static int directive_end(xmlcell_t *x) {
  if (x == NULL)
    return 0;

  if (xml_get_name(x) != xos._xev_pragma &&
      xml_get_name(x) != xos._xevdrs_line)
    return 0;

  xmlcell_t *cl = xml_get_child(x);
  if (cl != NULL)
    cl = xml_get_child(cl);
  while (cl != NULL) {
    if (xml_getstr(cl, "a=name") == xos._end)
      return 1;

    cl = xml_get_next(cl);
  }

  return 0;
}

static xmlcell_t *find_end(xmlcell_t *x) {
  if (x == NULL) {
    fprintf(stderr, "cannot find begin-end pair\n");
    exit(1);
  }

  if (directive_end(x))
    return x;

  if (directive_begin(x)) {
    xev_constr_block(x);
  }

  if (directive_end(xml_get_next(x))) {
    xmlcell_t *e = xml_cut_link(x, "n");
    return e;
  }

  return find_end(xml_get_next(x));
}

#if DETACH_BEGIN_END
static void detach_end(xmlcell_t *x) {
  assert(x != NULL);
  assert(xml_get_name(x) == xos._xev_pragma ||
	 xml_get_name(x) == xos._xevdrs_line);

  xmlcell_t *c = xml_get_child(x);
  assert(c != NULL);

  if (xml_getstr(c, "ca=name") == xos._end) {
    xmlcell_t *n = xml_detach_cell(c, "c");
    assert(n != NULL);
    free_xml(n);
  } else {
    xmlcell_t *cl = xml_get_child(c);
    assert(cl != NULL);

    while (xml_getstr(cl, "na=name") != xos._end)
      cl = xml_get_next(cl);
    assert(cl != NULL);

    xmlcell_t *n = xml_detach_cell(cl, "n");
    assert(n != NULL);
    free_xml(n);
  }
}
#endif

void make_pair(xmlcell_t *x) {
  xmlcell_t *b = x;
  int fst = 1;

  while (1) {

    xmlcell_t *n = (xml_get_next(b) != NULL ? xml_cut_link(b, "n") : NULL);
    xmlcell_t *e = find_end(n);
    assert(e != NULL);
    xmlcell_t *en = (xml_get_next(e) != NULL ? xml_cut_link(e, "n") : NULL);
  
    int cont = directive_begin(e);

    xmlcell_t *t = xml_make_node(xos._xev_target, xos.spaces[0]);
    if (n != e) {
      xml_append_child(t, n);
      xml_set_posttext(t, xos.spaces[0]);
    }
    xml_append_child(x, t);

    xml_append_child(x, xml_get_child(e));
#if DETACH_BEGIN_END
    detach_end(e);

    if (cont) {
      detach_begin(e);
    }
#endif

    xml_cut_link(e, "c");
    free_xml(e);

    xml_append(x, en);
#if DETACH_BEGIN_END
    if (fst) {
      detach_begin(b);
      fst = 0;
    }
#endif

    if (cont == 0)
      break;
  }
}

void xev_constr_block(xmlcell_t *x) {
  if (x == NULL)
    return;

  if (directive_begin(x))
    make_pair(x);

  xev_constr_block(xml_get_child(x));
  xev_constr_block(xml_get_next(x));
}
