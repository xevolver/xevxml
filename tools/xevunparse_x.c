#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"

#define DEBUG 0

static char buf[XEV_LINE_SIZE];
static int bufp;

static void clause_text(xmlcell_t *x, int (*get_num_ex)(xmlcell_t*), 
			int nocomma) {
  int c = 0;

  while (x != NULL) {

    if (x->node->type != XML_TYPE_NODE) {

      fprintf(stderr, "clause_text: unknown type %d\n", x->node->type);
      exit(1);

    }

    strobj node_name = xml_get_name(x);

    if (node_name == xev_integer) {
      c ++;
	
      strobj valuestr = xml_getstr(x, "a=value");
      assert(valuestr != NULL);
      
      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      strcpy(buf + bufp, valuestr);
      bufp += strlen(buf + bufp);

    } else if (node_name == xev_string) {
      c ++;

      strobj value = xml_getstr(x, "a=value");
      assert(value != NULL);

      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      char *t = strG2F(value, '\'');
      strcpy(buf + bufp, t);
      free(t);
      bufp += strlen(buf + bufp);

    } else if (node_name == xos._xev_boolean) {
      c ++;

      strobj valuestr = xml_getstr(x, "a=value");
      assert(valuestr != NULL);

      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      strcpy(buf + bufp, valuestr);
      bufp += strlen(buf + bufp);

    } else if (node_name == xos._xev_literal) {
      c ++;

      strobj name = xml_getstr(x, "a=name");
      assert(name != NULL);

      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      strcpy(buf + bufp, name);
      bufp += strlen(buf + bufp);

    } else if (node_name == xos._xev_pair) {
      c ++;

      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      xmlcell_t *fst = xml_get_child(x);
      xmlcell_t *snd = xml_get_next(fst);
      clause_text(xml_make_temporal_copy(fst), get_num_ex, 1);
      strcpy(buf + bufp, ":"), bufp ++;
      clause_text(snd, get_num_ex, 1);

    } else if (node_name == xos._xevparse_code_exp) {
      c ++;

      //      assert(xml_get_child(x) != NULL);

      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      char exp[10];
      xmlcell_t *ex =
	(xml_get_child(x) == NULL ? NULL : xml_cut_link(x, "c"));
      sprintf(exp, "`%d`", get_num_ex(ex));
      strcpy(buf + bufp, exp);
      bufp += strlen(buf + bufp);

    } else if (node_name == xos._xev_func_call) {
      c ++;

      strobj name = xml_getstr(x, "a=name");
      assert(name != NULL);

      if (c != 1 && nocomma == 0)
	strcpy(buf + bufp, ", "), bufp += 2;

      strcpy(buf + bufp, name);
      bufp += strlen(buf + bufp);

      buf[bufp++] = '(';
      clause_text(xml_get_child(x), get_num_ex, 0);
      buf[bufp++] = ')';

    }

    x = x->next;
  }
}

char* xev_unparse_pragma(xmlcell_t *x, int (*get_num_ex)(xmlcell_t*)) {

  bufp = 0;

  while (x != NULL) {
    if (xml_get_child(x) == NULL) {

      strobj name = xml_getstr(x, "a=name");
      assert(name != NULL);

      if (bufp != 0)
	buf[bufp++] = ' ';
      strcpy(buf + bufp, name);
      bufp += strlen(buf + bufp);

    } else {

      strobj name = xml_getstr(x, "a=name");
      assert(name != NULL);

      if (bufp != 0)
	buf[bufp++] = ' ';
      strcpy(buf + bufp, name);
      bufp += strlen(buf + bufp);

      if (xml_get_child(x) != NULL) {
	buf[bufp++] = '(';
	clause_text(xml_get_child(x), get_num_ex, 0);
	buf[bufp++] = ')';
      }
    }

    x = xml_get_next(x);
  }

  buf[bufp] = '\0';
  return get_strobj(buf);
}
