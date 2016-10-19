#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

void contplus(xmlcell_t *parent) {

  xmlcell_t *x = xml_get_child(parent);
  xmlcell_t *p = NULL;
  xmlcell_t *c = x;
  while (c != NULL && xml_get_name(c) != xos._SgContainsStatement) {
    p = c;
    c = xml_get_next(c);
  }
  xmlcell_t *y = (c == NULL ? NULL : xml_get_next(c));

#if DEBUG
  printf("parent = %s\n", xml_get_name(parent));
  printf("x = %s\n", x == NULL ? "null" : xml_get_name(x));
  printf("p = %s\n", p == NULL ? "null" : xml_get_name(p));
  printf("c = %s\n", c == NULL ? "null" : xml_get_name(c));
  printf("y = %s\n", y == NULL ? "null" : xml_get_name(y));
#endif

  if (x != NULL)
    xml_cut_link(parent, "c");
  if (c != NULL)
    xml_cut_link(p, "n");

  xmlcell_t *c0 = xml_make_node(xos._contains_body, xos.spaces[0]);
  xml_set_posttext(c0, xos.spaces[0]);
  xmlcell_t *c1 = xml_make_node(xos._contains_head, xos.spaces[1]);
  if (x != NULL)
    xml_set_posttext(c1, xos.spaces[1]);
  xmlcell_t *c2 = xml_make_node(xos._contains_tail, xos.spaces[1]);
  if (y != NULL)
    xml_set_posttext(c2, xos.spaces[1]);

  xml_append_child(c0, c1);
  xml_append_child(c0, c2);
  xml_append_child(c1, x);
  xml_append_child(c2, y);
  xml_append_child(parent, c0);
}

void xevparse_contplus(xmlcell_t *x) {
  while (x != NULL) {

    if (xml_get_name(x) == xos._SgProgramHeaderStatement) {

      xmlcell_t *c = xml_get_cell(x, "cnc");
      assert(c != NULL && xml_get_name(c) == xos._SgBasicBlock);
      contplus(c);

    } else if (xml_get_name(x) == xos._SgProcedureHeaderStatement) {

      xmlcell_t *c = xml_get_cell(x, "cnnc");
      assert(c != NULL && xml_get_name(c) == xos._SgBasicBlock);
      contplus(c);

    } else if (xml_get_name(x) == xos._SgModuleStatement) {

      xmlcell_t *c = xml_get_child(x);
      assert(c != NULL && xml_get_name(c) == xos._SgClassDefinition);
      contplus(c);
      
    }

    xevparse_contplus(xml_get_child(x));
    x = xml_get_next(x);
  }
}
