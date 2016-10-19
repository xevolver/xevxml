#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

#define DEBUG 0

/* remove ppinfo */

static int check_ppinfo(xmlcell_t *x) {
  assert(x != NULL);
  assert(x->node != NULL);
  
  if (xml_chk_struct(x, "=PreprocessingInfo")) {

    assert(x->node->posttext != NULL);
    char *p = strchr(x->node->posttext, '!');
    assert(p != NULL);

    if (xevparse_pragma_line(p, NULL)/*strstr(p, "!$xev") == p*/) {
      return 1;
    }
  }

  return 0;
}

static void remove_ppinfo(xmlcell_t *x) {
  assert(x != NULL);

  if (xml_get_child(x) != NULL) {
    remove_ppinfo(xml_get_child(x));

    if (check_ppinfo(xml_get_child(x))) {
      xmlcell_t *y = xml_detach_cell(x, "c");
      free_xml(y);
    }
  }

  if (xml_get_next(x) != NULL) {
    remove_ppinfo(xml_get_next(x));

    if (check_ppinfo(xml_get_next(x))) {
      xmlcell_t *y = xml_detach_cell(x, "n");
      free_xml(y);
    }
  }
}

/* collect extract exp */

typedef struct EX {
  xmlcell_t *x;
  struct EX *next;
} ex_t;

static ex_t *ex_list = NULL;

static int check_exp(xmlcell_t *x) {
  assert(x != NULL);

  if (xml_chk_struct(x, "=SgExprStatement") &&
      xml_chk_struct(x, "c=SgFunctionCallExp") &&
      xml_chk_struct(x, "cc=SgFunctionRefExp") &&
      xml_chk_struct(x, "cca=name=xevparse_extract_exp")) {

    ex_t *ex = (ex_t*) malloc(sizeof(ex_t));
    assert(ex != NULL);

    //    ex->x = xml_detach_cell(x, "ccnc");
    xmlcell_t *pre = xml_get_cell(x, "ccn");
    if (xml_get_child(pre) != NULL)
      ex->x = xml_cut_link(x, "ccnc");
    else
      ex->x = NULL;
    ex->next = NULL;

    /* add to list */
    if (ex_list == NULL)
      ex_list = ex;
    else {
      ex_t *ex1;
      for (ex1 = ex_list; ex1->next != NULL; ex1 = ex1->next);
      ex1->next = ex;
    }

    return 1;
  }

  return 0;
}

static void collect_exp(xmlcell_t *x) {
  assert(x != NULL);

  while (xml_get_child(x) != NULL) {
    if (check_exp(xml_get_child(x))) {
      xmlcell_t *y = xml_detach_cell(x, "c");
      free_xml(y);
    } else
      break;
  }

  if (xml_get_child(x) != NULL)
    collect_exp(xml_get_child(x));

  while (xml_get_next(x) != NULL) {
    if (check_exp(xml_get_next(x))) {
      xmlcell_t *y = xml_detach_cell(x, "n");
      free_xml(y);
    } else
      break;
  }

  if (xml_get_next(x) != NULL)
    collect_exp(xml_get_next(x));
}

/* parse xev pragma */

static int check_pragma(xmlcell_t *x) {
  return xml_chk_struct(x, "=SgPragmaDeclaration");
}

static int depth;

static char* parse_code(char *t, xmlcell_t **xp) {
  assert(ex_list != NULL);
  assert(t != NULL);

  char *p = strchrF(t+1, '`');
  if (p == NULL) {
    fprintf(stderr, "incomplete fortran code: %s\n", t);
    xevparse_putlineinfo(stderr);
    exit(1);
  }
  t = p + 1;

  xmlcell_t *x = xml_make_node(xos._xevparse_code_exp, get_depth_strobj(depth));
  assert(x != NULL);
  if (ex_list->x != NULL)
    xml_insert_child(x, ex_list->x);
  xml_set_posttext(x, get_depth_strobj(depth));

  ex_t *ex = ex_list;
  ex_list = ex_list->next;
  free(ex);

  *xp = x;
  return t;
}

static int n_pragma = 0;

xmlcell_t *get_pragma_(xmlcell_t *x) {
  assert(x != NULL);
  assert(xml_chk_struct(x, "=SgPragmaDeclaration"));
  assert(xml_chk_struct(x, "c=SgPragma"));

  strobj t = xml_getstr(x, "ca=pragma");
  n_pragma ++;

  xevparse_setlineinfo(t, n_pragma);

  if (!isalnum(*t) && *t != '_' && *t != '.') {
    fprintf(stderr, "unknown type of pragma 1\n");
    xevparse_putlineinfo(stderr);
    exit(1);
  }

  xmlcell_t *r = xml_make_node(xos._xev_pragma, get_depth_strobj(depth));
  assert(r != NULL);
  xml_set_posttext(r, get_depth_strobj(depth));

  depth ++;

  xmlcell_t *c = xml_make_node(xos._xev_clause_list, get_depth_strobj(depth));
  assert(c != NULL);
  xml_set_posttext(c, get_depth_strobj(depth));
  xml_insert_child(r, c);

  depth ++;

  xmlcell_t *p = xev_parse_pragma(t, depth, parse_code);
  xml_insert_child(c, p);

  depth -= 2;

  return r;
}

static void parse_pragma(xmlcell_t *x) {
  assert(x != NULL);

  depth ++;

  if (xml_get_child(x) != NULL && 
      check_pragma(xml_get_child(x))) {
    xmlcell_t *y = xml_detach_cell(x, "c");
    xmlcell_t *z = get_pragma_(y);
    xml_insert_child(x, z);
    free_xml(y);
  }

  if (xml_get_child(x) != NULL)
    parse_pragma(xml_get_child(x));

  depth --;
  
  if (xml_get_next(x) != NULL && 
      check_pragma(xml_get_next(x))) {
    xmlcell_t *y = xml_detach_cell(x, "n");
    xmlcell_t *z = get_pragma_(y);
    xml_insert_next(x, z);
    free_xml(y);
  }

  if (xml_get_next(x) != NULL)
    parse_pragma(xml_get_next(x));
}

void arrange_info_source(xmlcell_t *x) {
  xmlcell_t *e1 = xml_make_node(xos._SgPragmaDeclaration, xos.spaces[1]);
  xmlcell_t *e2 = xml_make_node(xos._SgPragma, xos.spaces[2]);

#if 0
  xml_add_attr(e2, xos._pragma, get_strobj("xevparse end info"));
#else
  xml_add_attr(e2, xos._pragma, get_strobj("xev xevparse end info"));
#endif
  xml_append_child(e1, e2);
  //  xml_append_child(xml_get_next(x), e1);
  xml_append_child(xml_get_child(xml_get_next(x)), e1);
}

static void modify_source_info(xmlcell_t *x) {
  assert(x != NULL);

  xmlcell_t *xs = xml_get_next(x);
  assert(xs != NULL && xml_get_name(xs) == xos._SgSourceFile);

  xmlcell_t *ps = xml_get_cell(x, "ncc");
  assert(ps != NULL && xml_get_name(ps) == xos._SgPragmaDeclaration);

  xmlcell_t *ss = xml_get_child(ps);
  assert(ss != NULL && xml_get_name(ss) == xos._SgPragma);

  char s[XEV_LINE_SIZE];
  strcpy(s, xml_getstr(ss, "a=pragma"));
 
#if 0
  assert(strstr(s, "xevparse info source") == s);
#else
  assert(strstr(s, "xev xevparse info source") == s);
#endif
  char *t = strchr(s + 26, '\'');
  assert(t != NULL);
  *t = '\0';

  xml_reset_attr(xs, xos._file, get_strobj(s + 26));
}

void postparse_f(xmlcell_t *x, char *dstfile) {
  assert(dstfile != NULL);

  /* modify source file name */

  modify_source_info(x);

  /* parse */

  arrange_info_source(x);

  remove_ppinfo(x);		/* remove PreprocessingInfo for xev */

#if DEBUG
  printf("collect_exp\n");
#endif
 
  ex_list = NULL;
  collect_exp(x);		/* collect xev_extract_exp and remove */

#if DEBUG
  {
    ex_t *ex;
    for (ex = ex_list; ex != NULL; ex = ex->next) {
      fprintf(stderr, "extracted exp:\n");
      fprint_xml(stderr, ex->x);
      fprintf(stderr, "\n");
    }
  }
#endif

#if DEBUG
  fprintf(stderr, "parse_pragma\n");
#endif

  depth = 0;
  n_pragma = 0;
  parse_pragma(x);		/* parse xev pragma */

  /* write result */

#if DEBUG
  fprintf(stderr, "write parsed xml\n");
#endif

  FILE *fd = fopen(dstfile, "w");
  if (fd == NULL) {
    fprintf(stderr, "cannot open %s\n", dstfile);
    exit(1);
  }

  fprint_xml(fd, x);
  fprintf(fd, "\n");

#if DEBUG
  fprintf(stderr, "close file\n");
#endif

  fclose(fd);

#if DEBUG
  fprintf(stderr, "parse done\n");
#endif
}
