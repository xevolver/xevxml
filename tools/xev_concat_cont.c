#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

static int directive_cont(xmlcell_t *x) {
  if (x == NULL)
    return 0;

  if (xml_get_name(x) != xos._xev_pragma)
    return 0;

  xmlcell_t *cc = xml_get_cell(x, "cc");
  if (cc == NULL)
    return 0;

  while (xml_get_next(cc) != NULL)
    cc = xml_get_next(cc);

  if (xml_getstr(cc, "a=name") == xos.ampersand)
    return 1;

  return 0;
}

static void detach_ampersand(xmlcell_t *x) {
  assert(x != NULL);
  assert(xml_get_name(x) == xos._xev_pragma);

  xmlcell_t *cc = xml_get_cell(x, "cc");
  assert(cc != NULL);

  while (xml_get_next(xml_get_next(cc)) != NULL)
    cc = xml_get_next(cc);

  assert(xml_getstr(cc, "na=name") == xos.ampersand);

  xmlcell_t *r = xml_cut_link(cc, "n");
  free_xml(r);
}

static xmlcell_t *find_nextdir(xmlcell_t *x) {
  if (x == NULL)
    return NULL;

  xmlcell_t *n = xml_get_next(x);

  if (n == NULL)
    return NULL;

  if (xml_get_name(n) == xos._xev_pragma) {
    xml_cut_link(x, "n");

    if (xml_get_next(n) != NULL) {
      xmlcell_t *nn = xml_cut_link(n, "n");
      xml_insert_next(x, nn);
    }

    return n;
  }

  return find_nextdir(n);
}

void xev_concat_cont(xmlcell_t *x) {
  if (x == NULL)
    return;

  while (directive_cont(x)) {

    detach_ampersand(x);

    xmlcell_t *n = find_nextdir(x);
    if (n == NULL) {
      fprintf(stderr, "xevpostparse: cannot find next directive for continuation\n");
      exit(1);
    }

    xmlcell_t *c = xml_cut_link(n, "ccn");
    if (c != NULL)
      xml_append_child(xml_get_child(x), c);

    free_xml(n);
  }

  xev_concat_cont(xml_get_child(x));
  xev_concat_cont(xml_get_next(x));
}
