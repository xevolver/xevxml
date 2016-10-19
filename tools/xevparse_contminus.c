#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

void contminus(xmlcell_t *parent) {

  xmlcell_t *c0 = xml_get_child(parent);
  xmlcell_t *c1 = xml_get_child(c0);
  xmlcell_t *c2 = xml_get_next(c1);

  xmlcell_t *x = xml_get_child(c1);
  xmlcell_t *y = xml_get_child(c2);

  xmlcell_t *p = NULL;
  xmlcell_t *c;
  for (c = x; c != NULL; c = xml_get_next(c))
    p = c;

#if 0
  printf("parent = %s\n", xml_get_name(parent));
  printf("x = %s\n", x == NULL ? "null" : xml_get_name(x));
  printf("p = %s\n", p == NULL ? "null" : xml_get_name(p));
  printf("y = %s\n", y == NULL ? "null" : xml_get_name(y));
#endif

  xml_cut_link(parent, "c");
  xml_insert_child(parent, x);
  if (y != NULL) {
    c = xml_make_node(xos._SgContainsStatement, xos.spaces[1]);
    xml_insert_next(p, c);
    xml_insert_next(c, y);
  }
}

void xevparse_contminus(xmlcell_t *x) {
  while (x != NULL) {

    if (xml_get_name(x) == xos._SgProgramHeaderStatement) {
	
      xmlcell_t *c = xml_get_cell(x, "cnc");
      assert(c != NULL && xml_get_name(c) == xos._SgBasicBlock);
      contminus(c);

    } else if (xml_get_name(x) == xos._SgProcedureHeaderStatement) {
	
      xmlcell_t *c = xml_get_cell(x, "cnnc");
      assert(c != NULL && xml_get_name(c) == xos._SgBasicBlock);
      contminus(c);

    } else if (xml_get_name(x) == xos._SgModuleStatement) {

      xmlcell_t *c = xml_get_child(x);
      assert(c != NULL && xml_get_name(c) == xos._SgClassDefinition);
      contminus(c);
      
    }

    xevparse_contminus(xml_get_child(x));
    x = xml_get_next(x);
  }
}
