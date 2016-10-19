#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

#define TYPE_NAME 1
#define TYPE_HEAD 2
#define GETTYPE(x) ((x)&3)
#define TYPE_STRING 4
#define TYPE_STR_NAME (TYPE_NAME + TYPE_STRING)
#define TYPE_STR_HEAD (TYPE_HEAD + TYPE_STRING)
#define TYPE_EXTEND 8
#define TYPE_EXT_NAME (TYPE_NAME + TYPE_EXTEND)
#define TYPE_EXT_HEAD (TYPE_HEAD + TYPE_EXTEND)

typedef struct NAMECELL {
  int type;
  strobj pragma_name;
  strobj rename;
  struct NAMECELL *next;
} namecell_t;

static namecell_t *namelist = NULL;
static int parse_comment = 0;
static int load_conf = 0;

void xevparse_load_conf(void) {
  assert(load_conf == 0);
  load_conf = 1;

  FILE *f = fopen(search_file("xevparse." XT_FORMATSTR ".conf"), "r");
  if (f == NULL) {
    fprintf(stderr, "xevparse: cannot open xevparse." XT_FORMATSTR ".conf\n");
    exit(1);
  }

  xmlcell_t *x = read_xml(f);
  fclose(f);
  xml_check(x);
  xml_set_dynamic_all(x);

  xmlcell_t *xorg = x;
  while (x != NULL && xml_cell_type(x) != XML_TYPE_NODE)
    x = xml_get_next(x);

  if (x != NULL && xml_get_name(x) != xos._xevparse_conf) {
    fprintf(stderr, "xevparse: config file must have one xevparse_conf tree\n");
    if (x == NULL)
      fprintf(stderr, "x=NULL\n");
    else
      fprintf(stderr, "x=%s\n", xml_get_name(x));
    exit(1);
  }

  x = xml_get_child(x);
  while (x != NULL) {
    if (xml_cell_type(x) == XML_TYPE_NODE) {
      
      strobj name = xml_get_name(x);
      if (name == xos._xevparse_pragma_names ||
	  name == xos._xevparse_string_pragma ||
	  name == xos._xevparse_extend_pragma) {

	xmlcell_t *y = xml_get_child(x);
	while (y != NULL) {

	  if (xml_get_name(y) == xos._name ||
	      xml_get_name(y) == xos._head) {
	   
	    strobj value = xml_getstr(y, "a=value");

	    if (value == NULL) {
	      fprintf(stderr, "xevparse: config pragma_names must have string with value\n");
	      exit(1);
	    }

	    namecell_t *newcell = (namecell_t*) malloc(sizeof(namecell_t));
	    assert(newcell);

	    newcell->type =
	      (xml_get_name(y) == xos._name ? TYPE_NAME : TYPE_HEAD);
	    if (name == xos._xevparse_string_pragma)
	      newcell->type += TYPE_STRING;
	    if (name == xos._xevparse_extend_pragma)
	      newcell->type += TYPE_EXTEND;

	    if (name == xos._xevparse_extend_pragma) {
	      newcell->rename = xml_getstr(y, "a=rename");
	      assert(newcell->rename != NULL);
	    } else
	      newcell->rename = NULL;

	    newcell->pragma_name = value;
	    newcell->next = namelist;
	    namelist = newcell;

	  } else {
	    fprintf(stderr, "xevparse: config pragma_names must have string with value\n");
	    exit(1);
	  }

	  y = xml_get_next(y);
	}

      } else if (name == xos._xevparse_comment) {

	/* not referred */

	strobj value = xml_getstr(x, "a=value");
	if (value == xos._0)
	  parse_comment = 0;
	else if (value == xos._1)
	  parse_comment = 1;
	else {
	  fprintf(stderr, "xevparse: config xevparse_comment must be 0 or 1 (but %s)\n", value);
	  exit(1);
	}

      } else {
	fprintf(stderr, "xevparse: unknown configuration %s\n", name);
	exit(1);
      }
    }

    x = xml_get_next(x);
  }

  free_xml(xorg);
}

int xevparse_matching_pragma(strobj name) {
  assert(load_conf == 1);

  namecell_t *c;

  for (c = namelist; c != NULL; c = c->next)
    if (GETTYPE(c->type) == TYPE_NAME && c->pragma_name == name)
      return c->type;
    else if (GETTYPE(c->type) == TYPE_HEAD && 
	     strstr(name, c->pragma_name) == name)
      return c->type;

  return 0;
}

int xevparse_pragma_line(char *line, strobj *rename) {
  assert(load_conf == 1);
  assert(line != NULL);
  assert(line[0] == '!');

  char *t;
  for (t = line; *t != '\0' && *t <= ' '; t++);

  char *s;
  for (s = t; *s > ' '; s++);

  char q = *s;
  *s = '\0';
  char namestr[strlen(t)+1];
  strcpy(namestr, t);
  *s = q;

  strobj name = get_strobj(namestr);

  namecell_t *c;
  for (c = namelist; c != NULL; c = c->next) {
    if ((GETTYPE(c->type) == TYPE_NAME && c->pragma_name == name) ||
	(GETTYPE(c->type) == TYPE_HEAD && 
	 strstr(name, c->pragma_name) == name)) {

      if (rename != NULL)
	*rename = c->rename;

      return c->type;
    }
  }

  return 0;
}

strobj xevparse_pragma_rename(char *line, int *clen) {
  assert(line != NULL && clen != NULL);

  char *s;
  for (s = line; *s != '\0' && *s > ' '; s++);

  char q = *s;
  *s = '\0';
  char namestr[strlen(line)+1];
  strcpy(namestr, line);
  *s = q;
  strobj name = get_strobj(namestr);

  namecell_t *c;
  for (c = namelist; c != NULL; c = c->next)
    if (c->rename == name) {
      *clen = strlen(name);
      return c->pragma_name;
    }

  return NULL;
}
