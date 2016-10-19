#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

#define DEBUG 0

/* unparse pragma */

static int check_pragma(xmlcell_t *x) {
  return xml_chk_struct(x, "=xev_pragma");
}

typedef struct EXC {
  xmlcell_t *x;
  struct EXC *next;
} ex_t;

static ex_t *exlist;
static int num_ex;

static int get_num_ex(xmlcell_t *x) {
  int ret = num_ex ++;

  ex_t *ex = (ex_t*) malloc(sizeof(ex_t));
  assert(ex != NULL);

  ex->x = x;
  ex->next = NULL;

  if (exlist == NULL)
    exlist = ex;
  else {
    ex_t *ee;
    for (ee = exlist; ee->next != NULL; ee = ee->next);
    ee->next = ex;
  }

  return ret;
}

static xmlcell_t* restore_pragma(xmlcell_t *x) {
  assert(x != NULL);

  xmlcell_t *res = xml_make_node(xos._SgPragmaDeclaration, xos.spaces[0]);

  xmlcell_t *c = xml_make_node(xos._SgPragma, xos.spaces[0]);
  xml_insert_child(res, c);

  strobj s = xev_unparse_pragma(xml_get_child(xml_get_child(x)), &get_num_ex);
  xml_add_attr(c, xos._pragma, s);

  return res;
}

static void unparse_pragma(xmlcell_t *x) {
  assert(x != NULL);

  if (xml_get_child(x) != NULL && check_pragma(xml_get_child(x))) {
    xmlcell_t *cold = xml_detach_cell(x, "c");
    xmlcell_t *cnew = restore_pragma(cold);
    xml_insert_child(x, cnew);
    free_xml(cold);
  }

  if (xml_get_child(x) != NULL)
    unparse_pragma(xml_get_child(x));

  if (xml_get_next(x) != NULL && check_pragma(xml_get_next(x))) {
    xmlcell_t *pold = xml_detach_cell(x, "n");
    xmlcell_t *pnew = restore_pragma(pold);
    xml_insert_next(x, pnew);
    free_xml(pold);
  }
  
  if (xml_get_next(x) != NULL)
    unparse_pragma(xml_get_next(x));
}

/* place expression */

static int count_exp(char *s) {
  int c = 0;

  while (1) {

    if (*s == '\0')
      break;

    if (*s == '"' || *s == '\'')
      s = endstrF(s);

    if (*s == '`') {
      c ++;
      s ++;
      while (*s != '`' && *s != '\0')
	s ++;
      assert(*s != '\0');
    }

    s ++;
  }

  return c;
}

static int check_exp(xmlcell_t *x) {

  if (xml_chk_struct(x, "=SgPragmaDeclaration")) {

    strobj ptext = xml_getstr(x, "ca=pragma");
    assert(ptext != NULL);

    return count_exp(ptext);
  }

  return 0;
}

static void place_exp(xmlcell_t *x) {

  int c = check_exp(x);

  /* make reverse list of first c entries of exlist */
  ex_t *l = NULL;
  int i;
  for (i = 0; i< c; i++) {
    ex_t *t = exlist;
    exlist = t->next;
    t->next = l;
    l = t;
  }

  while (c > 0) {
    ex_t *e = l;
    l = l->next;

    strobj newline = xos.spaces[0];
    xmlcell_t *e1 = xml_make_node(xos._SgExprStatement, newline);
    xmlcell_t *e2 = xml_make_node(xos._SgFunctionCallExp, newline);
    xml_insert_child(e1, e2);

    xmlcell_t *e3 = xml_make_node(xos._SgFunctionRefExp, newline);
    xml_insert_child(e2, e3);

    xml_add_attr(e3, xos._name, xos._xevparse_extract_exp);
    xml_add_attr(e3, xos._kind, xos._2);

    xmlcell_t *e4 = xml_make_node(xos._SgExprListExp, newline);
    xml_insert_next(e3, e4);
    if (e->x != NULL)
      xml_insert_child(e4, e->x);

    xml_insert_next(x, e1);

    c --;
  }

  if (xml_get_child(x) != NULL)
    place_exp(xml_get_child(x));
  if (xml_get_next(x) != NULL)
    place_exp(xml_get_next(x));
}

void unparse_f(xmlcell_t *x, char *dstfile) {
  assert(x != NULL);

  /* unparse */

#if DEBUG
  fprintf(stderr, "unparse pragma\n");
#endif

  exlist = NULL;
  num_ex = 0;
  unparse_pragma(x);

#if DEBUG
  fprintf(stderr, "place exp\n");

  ex_t *e;
  for (e = exlist; e != NULL; e = e->next) {
    printf("code_exp found\n");
    fprint_xml(stdout, e->x);
    printf("\n");
  }
#endif

  place_exp(x);

  /* output result */

#if DEBUG
  fprintf(stderr, "output result\n");
#endif

  FILE *fd = fopen(dstfile, "w");
  if (fd == NULL) {
    fprintf(stderr, "cannot open %s\n", dstfile);
    exit(1);
  }

  fprint_xml(fd, x);
  fprintf(fd, "\n");

  fclose(fd);

  free_xml(x);

#if DEBUG
  fprintf(stderr, "unparse done\n");
#endif
}
