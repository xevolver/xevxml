#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

#define DEBUG 0

static int depth;
    
static char* parse_term(char *t, xmlcell_t **xp,
			char* parse_code(char*, xmlcell_t**)) {
  *xp = NULL;

  if (*t == '!') {
    fprintf(stderr, "xevparse: comment where term is expected\n");
    xevparse_putlineinfo(stderr);
    exit(1);
  }

  if (isdigit(*t) || *t == '-') {	/* integer */

    char *p;
    for (p = t + 1; isdigit(*p); p++);

    xmlcell_t *x = xml_make_node(xev_integer, get_depth_strobj(depth));

    strobj s = copyrange(t, p-1);
    xml_add_attr(x, xos._value, s);

    *xp = x;
    return p;
  }

  if (*t == '"' || *t == '\'') {		/* Fortran string */
    char *p = endstrF(t);
    if (p == NULL) {
      fprintf(stderr, "unterminated string (%s)\n", t);
      xevparse_putlineinfo(stderr);
      exit(1);
    }

    xmlcell_t *x = xml_make_node(xev_string, get_depth_strobj(depth));

    strobj s1 = copyrange(t, p);
    strobj s2 = strF2G(s1);
    xml_add_attr(x, xos._value, s2);

    *xp = x;
    return p + 1;
  }

  if (*t == '`') { 		/* inline code */
    if (parse_code == NULL) {
      fprintf(stderr, "xevparse: inline code is not allowed here\n");
      xevparse_putlineinfo(stderr);
      exit(1);
    }
    return parse_code(t, xp);
  }

  if (!isalnum(*t) && *t != '_' && *t != '.') {
    fprintf(stderr, "unknown pragma term: %s\n", t);
    xevparse_putlineinfo(stderr);
    exit(1);
  }

  char *p = t;
  while (isalnum(*p) || *p == '_' || *p == '.')
    p ++;

  strobj name = copyrange(t, p-1);
  if (name == xos._true || name == xos._false) {
    /* boolean */

    xmlcell_t *x = xml_make_node(xev_boolean, get_depth_strobj(depth));
    xml_add_attr(x, xos._value, name == xos._true ? xev_true : xev_false);

    *xp = x;
    return p;
  }

  t = p;
  while (*t != '\0' && *t <= ' ')
    t ++;

  if (*t == '(') {		/* function call */

    xmlcell_t *x = xml_make_node(xos._xev_func_call, get_depth_strobj(depth));
    xml_add_attr(x, xos._name, name);
    xml_set_posttext(x, get_depth_strobj(depth));

    depth ++;

    xmlcell_t *fst = NULL;

    t ++;
    while (1) {

      while (*t != '\0' && *t <= ' ')
	t ++;
      if (*t == ')') {
	t ++;
	break;
      }

      xmlcell_t *tx;
      t = parse_term(t, &tx, parse_code);

      while (*t != '\0' && *t <= ' ')
	t ++;
      if (*t != ')' && *t != ',' && *t != ':') {
	fprintf(stderr, "format error in pragma\n");
	xevparse_putlineinfo(stderr);
	exit(1);
      }

      if (*t == ',' && tx == NULL) {
	fprintf(stderr, "unknown type of pragma 4: %s\n", t);
	xevparse_putlineinfo(stderr);
	exit(1);
      }

      if (*t == ':') {

	if (fst != NULL) {
	  fprintf(stderr, "entangled pairs\n");
	  xevparse_putlineinfo(stderr);
	  exit(1);
	}
	fst = tx;

      } else {
	if (fst != NULL) {
	  if (tx == NULL) {
	    fprintf(stderr, "unmatched pair:\n");
	    xevparse_putlineinfo(stderr);
	    exit(1);
	  }
	  
	  xmlcell_t *pair = xml_make_node(xos._xev_pair,
					  get_depth_strobj(depth));
	  xml_set_posttext(pair, get_depth_strobj(depth));
	  xml_append_child(pair, fst);
	  xml_append_child(pair, tx);
	  fst = NULL;
	  tx = pair;
	}

	if (tx != NULL)
	  xml_append_child(x, tx);
      }

      if (*t == ')') {
	t ++;
	break;
      } else {
	t ++;
	while (*t != '\0' && *t <= ' ')
	  t ++;
      }
    }

    depth --;

    *xp = x;
    return t;
  }

  if (*t != ')' && *t != ',' && *t != ':') {
    fprintf(stderr, "unknown type of pragma 5: %s\n", t);
    xevparse_putlineinfo(stderr);
    exit(1);
  }

  /* literal */

  xmlcell_t *x = xml_make_node(xos._xev_literal, get_depth_strobj(depth));
  xml_add_attr(x, xos._name, name);

  *xp = x;
  return t;
}

xmlcell_t* xev_parse_pragma(char *t, int depth0, 
			    char* parse_code(char*, xmlcell_t**)) {
  depth = depth0;

#if DEBUG
  fprintf(stderr, "xev_parse_pragma %s\n", t);
#endif

  xmlcell_t *res = NULL;

  while (1) {

    while (*t != '\0' && *t <= ' ')
      t ++;

    if (*t == '\0' || *t == '!')
      break;

    if (*t == '&')
      t ++;			/* continuation */

    /* find clause name */
    char *p = t;
    if (*p == '{' || *p == '}')
      p ++;
    else {
      while (isalnum(*p) || *p == '_' || *p == '.')
	p ++;
    }

    if (*p > ' ' && *p != '\0' && *p != '(') {
      fprintf(stderr, "unknown type of pragma 2 (%c:%d): %s\n",
	      *p, *p, t);
      xevparse_putlineinfo(stderr);
      exit(1);
    }

    /* clause head */
    xmlcell_t *s = xml_make_node(xos._xev_clause, get_depth_strobj(depth));
    if (*t == '{')
      xml_add_attr(s, xos._name, xos._begin);
    else if (*t == '}')
      xml_add_attr(s, xos._name, xos._end);
    else
      xml_add_attr(s, xos._name, copyrange(t, p-1));

    if (res == NULL)
      res = s;
    else
      xml_append(res, s);

    depth ++;

    t = p;
    while (*t != '\0' && *t <= ' ')
      t ++;

    if (*t == '(') {		/* parse terms */

      t ++;
      xmlcell_t *fst = NULL;

      while (1) {

	xmlcell_t *tx;
	t = parse_term(t, &tx, parse_code);
	if (*t != ')' && *t != ',' && *t != ':') {
	  fprintf(stderr, "format error in pragma\n");
	  xevparse_putlineinfo(stderr);
	  exit(1);
	}

	if (*t == ',' && tx == NULL) {
	  fprintf(stderr, "unknown type of pragma 3: %s\n", t);
	  xevparse_putlineinfo(stderr);
	  exit(1);
	}

	if (*t == ':') {
	  if (fst != NULL) {
	    fprintf(stderr, "entangled pairs\n");
	    xevparse_putlineinfo(stderr);
	    exit(1);
	  }
	  fst = tx;
	} else {
	  if (fst != NULL) {
	    if (tx == NULL) {
	      fprintf(stderr, "unmatched pair\n");
	      xevparse_putlineinfo(stderr);
	      exit(1);
	    }

	    xmlcell_t *pair = xml_make_node(xos._xev_pair,
					    get_depth_strobj(depth));
	    xml_set_posttext(pair, get_depth_strobj(depth));
	    xml_append_child(pair, fst);
	    xml_append_child(pair, tx);
	    fst = NULL;
	    tx = pair;
	  }

	  if (tx != NULL)
	    xml_append_child(s, tx);
	}

	if (*t == ')') {
	  t ++;
	  break;
	} else {
	  t ++;
	  while (*t != '\0' && *t <= ' ')
	    t ++;
	}
      }
    }

    depth --;

    if (xml_get_child(s) != NULL)
      xml_set_posttext(s, get_depth_strobj(depth));
  }

  return res;
}
