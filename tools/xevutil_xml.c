#ifndef XEVUTIL_XML_C_DEFINED
#define XEVUTIL_XML_C_DEFINED
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"

#define KIND_FREE -1
#define KIND_INIT 0
#define KIND_STATIC 1
#define KIND_DYNAMIC 2

#define DEBUG 0

static xmlnode_t dummynode ={ 0, 0, "dummy", "\n", "\n", NULL, NULL, NULL };

/* check xevxml format */

void check_xmlformat(xmlcell_t *x) {
  assert(x != NULL);
  x = xml_get_next(x);
  assert(x != NULL);

  if (xml_get_name(x) == xos._SgSourceFile) {

    strobj format = xml_getstr(x, "a=format");
    if (format != get_strobj(XX_FORMATSTR)) {
      fprintf(stderr, "xevxml format mismatch: %s assumed, %s input\n",
	      XX_FORMATSTR, format);
      exit(1);
    }

  } else if (xml_get_name(x) == xos._xevdrs_program) {

    strobj format = xml_getstr(x, "a=format");
    if (format != get_strobj(XT_FORMATSTR)) {
      fprintf(stderr, "xevdrsparse format mismatch: %s assumed, %s input\n",
	      XT_FORMATSTR, format);
      exit(1);
    }

  } else {
    fprintf(stderr, "check_xmlformat: unknown type %s\n", xml_get_name(x));
    exit(1);
  }
}

/*********************************************************************
  allocation data structure
*********************************************************************/

#define INITALLOCSIZE 10240

typedef struct XMLATTR_ALLOC {
  int n_alloc;
  xmlattr_t *array;
  struct XMLATTR_ALLOC *next;
} xmlattr_alloc_t;

static xmlattr_alloc_t *xmlattr_alloc = NULL;
static xmlattr_t *xmlattr_freelist = NULL;

static void free_xmlattr_cell(xmlattr_t *cell) {
  assert(cell != NULL);

  cell->gcflag = -1;
  cell->name = NULL;
  cell->value = NULL;
  cell->next = xmlattr_freelist;
  xmlattr_freelist = cell;
}

static xmlattr_t* get_xmlattr_cell(void) {
  if (xmlattr_freelist == NULL) {
    int alloc_size = (xmlattr_alloc == NULL) ?
      INITALLOCSIZE : xmlattr_alloc->n_alloc * 2;

    xmlattr_alloc_t *alloc = (xmlattr_alloc_t*) malloc(sizeof(xmlattr_alloc_t));
    assert(alloc != NULL);

    alloc->n_alloc = alloc_size;
    alloc->array = (xmlattr_t*) malloc(sizeof(xmlattr_t) * alloc_size);
    assert(alloc->array != NULL);
    alloc->next = xmlattr_alloc;
    xmlattr_alloc = alloc;

    int i;
    for (i = alloc_size - 1; i >= 0; i--)
      free_xmlattr_cell(&alloc->array[i]);
  }

  assert(xmlattr_freelist != NULL);
  xmlattr_t *cell = xmlattr_freelist;
  xmlattr_freelist = cell->next;
  cell->gcflag = 0;
  return cell;
}

typedef struct XMLCELL_ALLOC {
  int n_alloc;
  xmlcell_t *array;
  struct XMLCELL_ALLOC *next;
} xmlcell_alloc_t;

static xmlcell_alloc_t *xmlcell_alloc = NULL;
static xmlcell_t *xmlcell_freelist = NULL;

static void free_xmlcell_cell(xmlcell_t *cell) {
  assert(cell != NULL);

  cell->gcflag = -1;
  cell->node = NULL;
  cell->next = xmlcell_freelist;
  xmlcell_freelist = cell;
}

static xmlcell_t* get_xmlcell_cell(void) {
  if (xmlcell_freelist == NULL) {
    int alloc_size = (xmlcell_alloc == NULL) ?
      INITALLOCSIZE : xmlcell_alloc->n_alloc * 2;

    xmlcell_alloc_t *alloc = (xmlcell_alloc_t*) malloc(sizeof(xmlcell_alloc_t));
    assert(alloc != NULL);

    alloc->n_alloc = alloc_size;
    alloc->array = (xmlcell_t*) malloc(sizeof(xmlcell_t) * alloc_size);
    assert(alloc->array != NULL);
    alloc->next = xmlcell_alloc;
    xmlcell_alloc = alloc;

    int i;
    for (i = alloc_size - 1; i >= 0; i--)
      free_xmlcell_cell(&alloc->array[i]);
  }

  assert(xmlcell_freelist != NULL);
  xmlcell_t *cell = xmlcell_freelist;
  xmlcell_freelist = cell->next;
  cell->gcflag = 0;
  return cell;
}

typedef struct XMLNODE_ALLOC {
  int n_alloc;
  xmlnode_t *array;
  struct XMLNODE_ALLOC *next;
} xmlnode_alloc_t;

static xmlnode_alloc_t *xmlnode_alloc = NULL;
static xmlnode_t *xmlnode_freelist = NULL;

static void free_xmlnode_cell(xmlnode_t *cell) {
  assert(cell != NULL);
  if (cell == &dummynode)
    return;

  cell->gcflag = -1;
  cell->type = -1;
  cell->name = NULL;
  cell->pretext = NULL;
  cell->posttext = NULL;
  cell->attr = NULL;
  cell->child = NULL;
  cell->next = xmlnode_freelist;
  xmlnode_freelist = cell;
}

static xmlnode_t* get_xmlnode_cell(void) {
  if (xmlnode_freelist == NULL) {
    int alloc_size = (xmlnode_alloc == NULL) ?
      INITALLOCSIZE : xmlnode_alloc->n_alloc * 2;

    xmlnode_alloc_t *alloc = (xmlnode_alloc_t*) malloc(sizeof(xmlnode_alloc_t));
    assert(alloc != NULL);

    alloc->n_alloc = alloc_size;
    alloc->array = (xmlnode_t*) malloc(sizeof(xmlnode_t) * alloc_size);
    assert(alloc->array != NULL);
    alloc->next = xmlnode_alloc;
    xmlnode_alloc = alloc;

    int i;
    for (i = alloc_size - 1; i >= 0; i--)
      free_xmlnode_cell(&alloc->array[i]);
  }

  assert(xmlnode_freelist != NULL);
  xmlnode_t *cell = xmlnode_freelist;
  xmlnode_freelist = cell->next;
  cell->gcflag = 0;
  return cell;
}

static void free_xmlattr(xmlattr_t *attr) {
  if (attr->next != NULL)
    free_xmlattr(attr->next);
  free_xmlattr_cell(attr);
}

static void free_xmlcell(xmlcell_t*);

static void free_xmlnode(xmlnode_t *node) {
  if (node->attr != NULL)
    free_xmlattr(node->attr);
  if (node->child != NULL)
    free_xmlcell(node->child);
  free_xmlnode_cell(node);
}

static void free_xmlcell(xmlcell_t *cell) {
  if (cell->node != NULL)
    free_xmlnode(cell->node);
  if (cell->next != NULL)
    free_xmlcell(cell->next);
  free_xmlcell_cell(cell);
}

void free_xml(xmlcell_t *cell) {
#if 0
  printf("freeing ...\n");
  fprint_xml(stdout, cell);
  printf("...freeing\n");
  free_xmlcell(cell);
  printf("done\n");
#endif
}

void xmlgc_mark(void) {
  xmlattr_alloc_t *a;
  for (a = xmlattr_alloc; a != NULL; a = a->next) {
    int i;
    for (i=0; i< a->n_alloc; i++)
      a->array[i].gcflag = 0;
  }

  xmlattr_t *af;
  for (af = xmlattr_freelist; af != NULL; af = af->next)
    af->gcflag = -1;

  xmlcell_alloc_t *c;
  for (c = xmlcell_alloc; c != NULL; c = c->next) {
    int i;
    for (i=0; i< c->n_alloc; i++)
      c->array[i].gcflag = 0;
  }

  xmlcell_t *cf;
  for (cf = xmlcell_freelist; cf != NULL; cf = cf->next)
    cf->gcflag = -1;

  xmlnode_alloc_t *n;
  for (n = xmlnode_alloc; n != NULL; n = n->next) {
    int i;
    for (i=0; i< n->n_alloc; i++)
      n->array[i].gcflag = 0;
  }

  xmlnode_t *nf;
  for (nf = xmlnode_freelist; nf != NULL; nf = nf->next)
    nf->gcflag = -1;
}

void xmlgc_collect(void) {
  xmlattr_alloc_t *a;
  for (a = xmlattr_alloc; a != NULL; a = a->next) {
    int i;
    for (i=0; i< a->n_alloc; i++)
      if (a->array[i].gcflag == 0)
	free_xmlattr_cell(&a->array[i]);
  }

  xmlcell_alloc_t *c;
  for (c = xmlcell_alloc; c != NULL; c = c->next) {
    int i;
    for (i=0; i< c->n_alloc; i++)
      if (c->array[i].gcflag == 0)
	free_xmlcell_cell(&c->array[i]);
  }

  xmlnode_alloc_t *n;
  for (n = xmlnode_alloc; n != NULL; n = n->next) {
    int i;
    for (i=0; i< n->n_alloc; i++)
      if (n->array[i].gcflag == 0)
	free_xmlnode_cell(&n->array[i]);
  }
}

/*********************************************************************
  making xml
*********************************************************************/

static xmlcell_t *xml_newcell(xmlnode_t *node) {
  //  xmlcell_t *x = (xmlcell_t*) malloc(sizeof(xmlcell_t));
  xmlcell_t *x = get_xmlcell_cell();
  assert(x != NULL);
  
  x->node = node;
  x->next = NULL;

  return x;
}

static xmlnode_t *xml_newnode(int type, strobj name, strobj pretext) {
  //  xmlnode_t *node = (xmlnode_t*) malloc(sizeof(xmlnode_t));
  xmlnode_t *node = get_xmlnode_cell();
  assert(node != NULL);
  
  node->type = type;
  node->name = name;
  node->pretext = pretext;
  node->posttext = NULL;
  node->attr = NULL;
  node->child = NULL;
  
  return node;
}

static xmlattr_t *xml_newattr(strobj name, strobj value) {
  //  xmlattr_t *a = (xmlattr_t*) malloc(sizeof(xmlattr_t));
  xmlattr_t *a = get_xmlattr_cell();
  assert(a != NULL);

  a->name = name;
  a->value = value;
  a->next = NULL;

  return a;
}

xmlcell_t *xml_make_cell(int type, strobj name, strobj pretext) {
  return xml_newcell(xml_newnode(type, name, pretext));
}

xmlcell_t *xml_make_node(strobj name, strobj pretext) {
  return xml_make_cell(XML_TYPE_NODE, name, pretext);
}

xmlcell_t *xml_destructive_cellcopy(xmlcell_t *cell) {
  //  xmlcell_t *newcell = (xmlcell_t*) malloc(sizeof(xmlcell_t));
  xmlcell_t *newcell = get_xmlcell_cell();
  assert(newcell != NULL);

  newcell->node = cell->node;
  newcell->next = NULL;

  cell->node = &dummynode;

  return newcell;
}

xmlcell_t *xml_make_temporal_copy(xmlcell_t *cell) {
  //  xmlcell_t *newcell = (xmlcell_t*) malloc(sizeof(xmlcell_t));
  xmlcell_t *newcell = get_xmlcell_cell();
  assert(newcell != NULL);

  newcell->node = cell->node;
  newcell->next = NULL;

  xml_set_dynamic_cell(newcell);

  return newcell;
}

/* for quote */
static xmlattr_t *deep_copy_attr(xmlattr_t *a) {
  if (a == NULL)
    return NULL;

  xmlattr_t *n = deep_copy_attr(a->next);

  xmlattr_t *newattr = get_xmlattr_cell();
  assert(newattr != NULL);
  newattr->gcflag = KIND_STATIC;
  newattr->name = a->name;
  newattr->value = a->value;
  newattr->next = n;

  return newattr;
}

xmlcell_t *xml_make_deep_copy(xmlcell_t *cell) {
  if (cell == NULL)
    return NULL;

  xmlcell_t *c = NULL, *n = NULL;
  if (xml_get_child(cell) != NULL)
    c = xml_make_deep_copy(xml_get_child(cell));

  if (xml_get_next(cell) != NULL)
    n = xml_make_deep_copy(xml_get_next(cell));

  xmlnode_t *newnode = get_xmlnode_cell();
  assert(newnode != NULL);
  newnode->gcflag = KIND_DYNAMIC;
  newnode->type = XML_TYPE_NODE;
  newnode->name = cell->node->name;
  newnode->pretext = cell->node->pretext;
  newnode->posttext = cell->node->posttext;
  newnode->attr = deep_copy_attr(cell->node->attr);
  newnode->child = c;

  xmlcell_t *newcell = get_xmlcell_cell();
  assert(newcell != NULL);
  newcell->gcflag = KIND_DYNAMIC;
  newcell->node = newnode;
  newcell->next = n;

  return newcell;
}
  
void xml_delete_temporal_copy(xmlcell_t *cell) {
  cell->node = NULL;
  free_xml(cell);
}

void xml_add_attr(xmlcell_t *x, strobj name, strobj value) {
  assert(x != NULL);
  assert(x->node != NULL);
  assert(x->gcflag != KIND_STATIC);

  xmlattr_t *a = xml_newattr(name, value);

  if (x->node->attr == NULL)
    x->node->attr = a;
  else
    xml_append_attr(x->node->attr, a);
}

void xml_reset_attr(xmlcell_t *x, strobj name, strobj value) {
  assert(x != NULL);
  assert(x->gcflag != KIND_STATIC);

  xmlattr_t *a;
  for (a = x->node->attr; a != NULL; a = a->next)
    if (a->name == name) {
      a->value = value;
      return;
    }

  fprintf(stderr, "xml_reset_attr: cannot find attribute %s\n", name);
  exit(1);
}

/*********************************************************************
  constructing xml
*********************************************************************/

void xml_append(xmlcell_t *x, xmlcell_t *y) {
  assert(x != NULL);
  assert(x->gcflag != KIND_STATIC);

  xmlcell_t *xx;

  for (xx = x; xx->next != NULL; xx = xx->next);

  assert(xx->next == NULL);

  xx->next = y;
}

void xml_append_child(xmlcell_t *x, xmlcell_t *c) {
  assert(x != NULL);
  assert(x->node != NULL);
  assert(x->gcflag != KIND_STATIC);

  if (x->node->child == NULL)
    x->node->child = c;
  else
    xml_append(x->node->child, c);
}

void xml_append_attr(xmlattr_t *x, xmlattr_t *y) {
  assert(x != NULL);
  assert(x->gcflag != KIND_STATIC);

  xmlattr_t *xx;

  for (xx = x; xx->next != NULL; xx = xx->next);

  assert(xx->next == NULL);

  xx->next = y;
}

void xml_insert_child(xmlcell_t *x, xmlcell_t *c) {
  assert(x != NULL);
  assert(x->node != NULL);
  assert(c != NULL);
  assert(c->node != NULL);
  assert(x->gcflag != KIND_STATIC);

  xmlcell_t *cc = c;
  while (cc->next != NULL)
    cc = cc->next;

  cc->next = x->node->child;
  x->node->child = c;
}

void xml_insert_next(xmlcell_t *x, xmlcell_t *n) {
  assert(x != NULL);
  assert(n != NULL);
  assert(x->gcflag != KIND_STATIC);

  xmlcell_t *nn = n;
  while (nn->next != NULL)
    nn = nn->next;

  nn->next = x->next;
  x->next = n;
}

void xml_reset_pretext(xmlcell_t *x, strobj t) {
  assert(x != NULL);
  assert(x->node != NULL);
  assert(x->gcflag != KIND_STATIC);

  x->node->pretext = t;
}

void xml_set_posttext(xmlcell_t *x, strobj t) {
  assert(x != NULL);
  assert(x->node != NULL);
  assert(x->gcflag != KIND_STATIC);

  if (x->node->posttext != NULL) {
    fprintf(stderr, "xml_set_posttext: non-null posttext: %s\n",
	    x->node->posttext);
    exit(1);
  }

  x->node->posttext = t;
}

void xml_reset_posttext(xmlcell_t *x, strobj t) {
  assert(x != NULL);
  assert(x->node != NULL);
  assert(x->gcflag != KIND_STATIC);

  x->node->posttext = t;
}

void xml_set_static(xmlcell_t *x) {
  while (x != NULL) {
    assert(x->gcflag == KIND_INIT || x->gcflag == KIND_STATIC);
    x->gcflag = KIND_STATIC;

    assert(x->node->gcflag == KIND_INIT || x->node->gcflag == KIND_STATIC);
    x->node->gcflag = KIND_STATIC;

    xmlattr_t *a;
    for (a = x->node->attr; a != NULL; a = a->next) {
      assert(a->gcflag == KIND_INIT || a->gcflag == KIND_STATIC);
      a->gcflag = KIND_STATIC;
    }

    if (xml_get_child(x) != NULL)
      xml_set_static(xml_get_child(x));

    x = xml_get_next(x);
  }
}

int xml_is_static(xmlcell_t *x) {
  return x->gcflag == KIND_STATIC;
}

void xml_set_dynamic_all(xmlcell_t *x) {
  while (x != NULL) {
    x->gcflag = KIND_DYNAMIC;
    x->node->gcflag = KIND_DYNAMIC;

    xmlattr_t *a;
    for (a = x->node->attr; a != NULL; a = a->next)
      a->gcflag = KIND_DYNAMIC;

    if (xml_get_child(x) != NULL)
      xml_set_dynamic_all(xml_get_child(x));

    x = xml_get_next(x);
  }
}

void xml_set_dynamic_cell(xmlcell_t *x) {
  x->gcflag = KIND_DYNAMIC;
}

/*********************************************************************
  analyzing xml
*********************************************************************/

int xml_cell_type(xmlcell_t *x) {
  assert(x != NULL);
  assert(x->node != NULL);
  return x->node->type;
}

xmlcell_t *xml_get_next(xmlcell_t *x) {
  assert(x != NULL);
  return x->next;
}

xmlcell_t *xml_get_child(xmlcell_t *x) {
  assert(x != NULL);
  assert(x->node != NULL);
  return x->node->child;
}

strobj xml_get_name(xmlcell_t *x) {
  assert(x != NULL);
  assert(x->node != NULL);
  return x->node->name;
}

/* get operations:
   n: next node
   c: child node
   (end of line): node name
   s: pretext
   t: posttext
   a=name: attribute name
*/

strobj xml_getstr(xmlcell_t *x, char *op) {
  if (op == NULL) {
    fprintf(stderr, "xml_getstr: null operation\n");
    exit(1);
  }

  if (x == NULL) {
    fprintf(stderr, "xml_getstr(%s): null xml\n", op);
    exit(1);
  }

  char *op_org = op;

  while (*op != '\0') {

    assert(x != NULL);
    assert(x->node != NULL);

    if (*op == 'n') {

      x = x->next;
      if (x == NULL)
	goto ERR;
      op ++;

    } else if (*op == 'c') {

      x = x->node->child;
      if (x == NULL)
	goto ERR;
      op ++;

    } else if (*op == 's') {

      return x->node->pretext;

    } else if (*op == 't') {

      return x->node->posttext;

    } else if (*op == 'a') {

      if (op[1] != '=') {
	fprintf(stderr, "xml_getstr(%s): = must follow a\n", op_org);
	exit(1);
      }

      int l = strlen(op+2);
      char name[l+1];
      strcpy(name, op+2);
      strobj aname = get_strobj(name);

      xmlattr_t *a = x->node->attr;
      while (a != NULL) {
	if (a->name == aname)
	  break;
	a = a->next;
      }

      if (a != NULL)
	return a->value;
      else
	return NULL;

    } else {

      fprintf(stderr, "xml_getstr(%s): unknown character %c\n", op_org, *op);
      exit(1);

    }
  }

  assert(x != NULL);
  assert(x->node != NULL);
  return x->node->name;

 ERR:
  fprintf(stderr, "xml_getstr(%s): mismatching xml structure\n", op_org);
  exit(1);
  return NULL;
}

static int cmp_str(char *s, strobj t) {
  return (get_strobj(s) == t ? 1 : 0);
}

int xml_chk_struct(xmlcell_t *x, char *op) {
  if (op == NULL) {
    fprintf(stderr, "xml_chk_struct: null operation\n");
    exit(1);
  }

  if (x == NULL)
    return 0;

  char *op_org = op;

  while (*op != '\0') {

    assert(x != NULL);
    assert(x->node != NULL);

    if (*op == 'n') {

      x = x->next;
      if (x == NULL)
	return 0;
      op ++;

    } else if (*op == 'c') {

      x = x->node->child;
      if (x == NULL)
	return 0;
      op ++;

    } else if (*op == '=') {

      return cmp_str(op+1, x->node->name);

    } else if (*op == 's') {

      return cmp_str(op+2, x->node->pretext);
      
    } else if (*op == 't') {

      return cmp_str(op+2, x->node->posttext);

    } else if (*op == 'a') {
      
      strobj aname = NULL;
      strobj value = NULL;

      int l = strlen(op+2);
      char name[l+1];
      strcpy(name, op+2);
      char *v = strchr(name, '=');
      if (v == NULL)
	aname = get_strobj(name);
      else {
	*v = '\0';  // end of name
	v ++;  // v is now value
	aname = get_strobj(name);
	value = get_strobj(v);
      }

      xmlattr_t *a = x->node->attr;
      while (a != NULL) {
	if (a->name == aname)
	  break;
	a = a->next;
      }

      if (a != NULL)
	return (value == NULL || value == a->value) ? 1 : 0;
      else
	return 0;

    } else {
      fprintf(stderr, "xml_chk_struct(%s): unknown op %c\n", op_org, *op);
      exit(1);
    }
  }

  return 1;
}

xmlcell_t *xml_get_cell(xmlcell_t *x, char *op) {
  if (op == NULL) {
    fprintf(stderr, "xml_get_cell: null operation\n");
    exit(1);
  }

  if (x == NULL) {
    fprintf(stderr, "xml_get_cell: null xml\n");
    exit(1);
  }

  char *op_org = op;

  while (*op != '\0') {

    if (x == NULL)
      goto ERR;

    if (*op == 'n') {

      x = x->next;
      op ++;

    } else if (*op == 'c') {

      x = x->node->child;
      op ++;

    } else {

      fprintf(stderr, "xml_get_cell(%s): unknown op %c\n", op_org, *op);
      exit(1);

    }
  }

  return x;

 ERR:
  fprintf(stderr, "xml_get_cell(%s): mismatched structure\n", op_org);
  exit(1);
}

xmlcell_t *xml_detach_cell(xmlcell_t *x, char *op) {
  if (op == NULL) {
    fprintf(stderr, "xml_detach_cell: null operation\n");
    exit(1);
  }

  if (x == NULL) {
    fprintf(stderr, "xml_detach_cell: null xml\n");
    exit(1);
  }
  assert(x->gcflag != KIND_STATIC);

  char *op_org = op;
  xmlcell_t *pre = NULL;

  while (*op != '\0') {

    assert(x != NULL);
    assert(x->node != NULL);

    if (*op == 'n') {

      pre = x;
      x = x->next;
      if (x == NULL)
	goto ERR;
      op ++;

    } else if (*op == 'c') {

      pre = x;
      x = x->node->child;
      if (x == NULL)
	goto ERR;
      op ++;

    } else {

      fprintf(stderr, "xml_detach_node(%s): unknown op %c\n", op_org, *op);
      exit(1);

    }
  }

  if (pre == NULL) {
    fprintf(stderr, "xml_detach_cell(%s): node must next or child\n", op_org);
    exit(1);
  }

  assert(x != NULL);

  if (x == pre->next) {

    pre->next = x->next;
    x->next = NULL;
    return x;

  } else if (x == pre->node->child) {

    pre->node->child = x->next;
    x->next = NULL;
    return x;

  } else {
    assert(1);
  }

 ERR:
  fprintf(stderr, "xml_detach_node(%s): mismatched structure\n", op_org);
  exit(1);
}

xmlcell_t *xml_cut_link(xmlcell_t *x, char *op) {
  if (op == NULL) {
    fprintf(stderr, "xml_cut_link: null operation\n");
    exit(1);
  }

  if (x == NULL) {
    fprintf(stderr, "xml_cut_link: null xml\n");
    exit(1);
  }
  assert(x->gcflag != KIND_STATIC);

  char *op_org = op;
  xmlcell_t *pre = NULL;

  while (*op != '\0') {

    assert(x != NULL);
    assert(x->node != NULL);

    if (*op == 'n') {

      pre = x;
      x = x->next;
      if (x == NULL)
	goto ERR;
      op ++;

    } else if (*op == 'c') {

      pre = x;
      x = x->node->child;
      if (x == NULL)
	goto ERR;
      op ++;

    } else {

      fprintf(stderr, "xml_cut_link(%s): unknown op %c\n", op_org, *op);
      exit(1);

    }
  }

  if (pre == NULL) {
    fprintf(stderr, "xml_cut_link(%s): node must next or child\n", op_org);
    exit(1);
  }

  assert(x != NULL);

  if (x == pre->next) {

    pre->next = NULL;
    return x;

  } else if (x == pre->node->child) {

    pre->node->child = NULL;
    return x;

  } else {
    assert(1);
  }

 ERR:
  fprintf(stderr, "xml_cut_link(%s): mismatched structure\n", op_org);
  exit(1);
}

/*********************************************************************
  read xml from text
*********************************************************************/

#define STRBUFSIZE 1048576

static char strbuf[STRBUFSIZE];
static int strbufp = 0;

static strobj copybuf(void) {
  if (strbufp == 0) {		/* empty string */
    strbufp = 0;
    return xos._;
  }

  strobj s = get_strobj(strbuf);
  assert(s != NULL);
  strbufp = 0;
  return s;
}

static strobj copybufxml(void) {
  if (strbufp == 0) {		/* empty string */
    strbufp = 0;
    return xos._;
  }

  int n_dq = 0, i, j;
  for (i=0; i< strbufp; i++)
    if (strbuf[i] == '"')
      n_dq ++;

  int len = strbufp + 3 + 5 * n_dq;
  char t[len];
  t[0] = '"';
  for (i=0, j=1; i< strbufp; i++, j++)
    if (strbuf[i] == '"')
      strcpy(t+j, "&quot;"), j+= 5;
    else
      t[j] = strbuf[i];
  t[j] = '"';  t[j+1] = '\0';
  assert(j+2 == len);
  strbufp = 0;

  char *s = strX2G(t);
  assert(s != NULL);  

  strobj r = get_strobj(s);
  assert(r != NULL);
  free(s);
  
  return r;
}

static char loaduntil(FILE *f, char *s) {

  while (!feof(f)) {
    char c = fgetc(f);

    int i;
    for (i=0; s[i] != '\0'; i++)
      if (c == s[i]) {
	strbuf[strbufp] = '\0';
#if DEBUG
	fprintf(stderr, "[%s%c]", strbuf, c);
#endif
	return c;
      }

    if (strbufp >= STRBUFSIZE) {
      strbuf[100] = '\0';
      fprintf(stderr, "too long xml data: %s\n", strbuf);
      exit(1);
    }
    strbuf[strbufp ++] = c;
  }

  return '\0';
}

static strobj loadxmlstring(FILE *f) {

  char c = fgetc(f);
  while (c == ' ')
    c = fgetc(f);

  if (c != '"' && c != '\'') {
    fprintf(stderr, "error at reading attribute text (start with %c)\n", c);
    exit(1);
  }

  char inichar = c;
  strbufp = 0;

  while (1) {
    c = fgetc(f);

    if (feof(f)) {
      fprintf(stderr, "error at reading attribute text (end of file)\n");
      exit(1);
    }

    if (c == inichar)
      break;

    assert(strbufp < STRBUFSIZE);
    strbuf[strbufp++] = c;
  }
   
  strbuf[strbufp] = '\0';
#if DEBUG
  fprintf(stderr, "{%s}", strbuf);
#endif
  return copybufxml();
}

static xmlcell_t *xxx = NULL;

static xmlcell_t *get_xml(FILE *f) { /* now on or before '<' */

#if DEBUG
  printf("get_xml 0\n");
#endif

  if (loaduntil(f, "<") != '<')
    return NULL;

#if DEBUG
  printf("get_xml 1\n");
#endif

  /* xml found */
  strobj pretext = copybufxml();
  //  xmlcell_t *x = xml_make_cell(XML_TYPE_INVALID, NULL, pretext);

#if DEBUG
  printf("get_xml 2\n");
#endif

  char c = loaduntil(f, " />");

  if (c == '/' && strbufp == 0) { /* closing */
    loaduntil(f, ">");

    xmlcell_t *x = xml_make_cell(XML_TYPE_CLOSE, copybuf(), pretext);
#if DEBUG
    fprintf(stderr, "</%s>\n", x->node->name);
#endif
    return x;
  }

  if (c == '\0') {
    fprintf(stderr, "unstructured XML\n");
    exit(1);
  }

#if DEBUG
  printf("get_xml 3\n");
#endif

  if (strbuf[0] == '?') {	/* xml declaration */
    assert(strbufp < STRBUFSIZE);
    strbuf[strbufp++] = c;
    c = loaduntil(f, ">");
    if (c == '\0') {
      strbuf[20] = '\0';
      fprintf(stderr, "error at XML dec: %s\n", strbuf);
      exit(1);
    }

    xmlcell_t *x = xml_make_cell(XML_TYPE_DEC, copybuf(), pretext);
#if DEBUG
    fprintf(stderr, "<%s>\n", x->node->name);
#endif
    return x;
  }

#if DEBUG
  printf("get_xml 4\n");
#endif

  if (strcmp(strbuf, "!--") == 0) { /* comment */
    while (1) {
      char c = loaduntil(f, ">");
      if (c == '\0') {
	strbuf[20] = '\0';
	fprintf(stderr, "error at XML comment: %s\n", strbuf);
	exit(1);
      }

      if (strbufp >= 3 && strbuf[strbufp-2] == '-' 
	  && strbuf[strbufp-3] == '-')
	break;
    }

    xmlcell_t *x = xml_make_cell(XML_TYPE_COMM, copybuf(), pretext);
#if DEBUG
    fprintf(stderr, "<%s>\n", x->node->name);
#endif
    return x;
  }

#if DEBUG
  printf("get_xml 4\n");
#endif

  xmlcell_t *x = xml_make_node(copybuf(), pretext);
  if (c == '/') {		/* single without attr */
    loaduntil(f, ">");
#if DEBUG
    fprintf(stderr, "<%s/>\n", x->node->name);
#endif
    return x;
  }

#if DEBUG
  printf("get_xml 5\n");
#endif

#if DEBUG
  fprintf(stderr, "<%s \n", x->node->name);
#endif

  if (c == ' ') {		/* search for attributes */

    while(1) {

#if DEBUG
  printf("get_xml 6\n");
#endif

      c = loaduntil(f, " =/>");

#if DEBUG
  printf("get_xml 7\n");
#endif

      if (c == '=') {
	strobj name = copybuf();
	strobj value = loadxmlstring(f);

#if DEBUG
	fprintf(stderr, " %s=?\n", name);
#endif

	xml_add_attr(x, name, value);

      } else if (c == '/' || c == '>')
	break;

#if DEBUG
  printf("get_xml 8\n");
#endif

      if (c == '\0') {
	fprintf(stderr, "error within attribute\n");
	exit(1);
      }
    }

#if DEBUG
  printf("get_xml 8\n");
#endif

    if (c == '/') {		/* single with attr */
      loaduntil(f, ">");
#if DEBUG
      fprintf(stderr, "/>\n");
#endif
      return x;
    }
  }

#if DEBUG
  printf("get_xml 9\n");
#endif

#if DEBUG
  fprintf(stderr, ">\n");
#endif

  /* search for children */
  while (1) {
    xmlcell_t *y = get_xml(f);

#if DEBUG
  printf("get_xml 10\n");
#endif

    if (y == NULL)
      break;
    
    if (y->node->type == XML_TYPE_CLOSE) {
      x->node->posttext = y->node->pretext;
      free_xml(y);
      break;
    }

#if DEBUG
  printf("get_xml 10\n");
#endif

#if 0					    /* regarding xev_list */
    if (strcmp(x->name, "xev_list") == 0) { /* special treatment for list */

      xml_t *cell = make_xml(XML_TYPE_LIST, "", "");
      cell->child = y;

      if (x->child == NULL)
	x->child = cell;
      else
	xml_append(x->child, cell);

    } else {

      if (x->child == NULL)
	x->child = y;
      else
	xml_append(x->child, y);

    }
#else
    xml_append_child(x, y);
#endif
  }

#if DEBUG
  printf("get_xml 11\n");
#endif

  return x;
}

xmlcell_t *read_xml(FILE *f) {

  strbufp = 0;

  xmlcell_t *x = get_xml(f);

  xxx = x;

  while (!feof(f)) {
    xmlcell_t *y = get_xml(f);

    if (y == NULL)
      break;

    assert(x != NULL);
    xml_append(x, y);
  }

  xml_set_static(x);
  
  return x;
}

/*********************************************************************
  write xml to text
*********************************************************************/

static int all_spaces(strobj s) {
  if (s == NULL)
    return 1;

  int i;
  for (i=0; s[i] != '\0' && s[i] <= ' '; i++);

  return (s[i] == '\0');
}

void fprint_xml(FILE *f, xmlcell_t *x) {
  if (x == NULL)
    return;

  assert(x->node != NULL);
  
  if (x->node->pretext != NULL) {
    char *t = strG2X(x->node->pretext, '"');
    t[strlen(t)-1] = '\0';	/* remove last " */
    fputs(t+1, f);
    free(t);
  }

  if (x->node->type == XML_TYPE_COMM || x->node->type == XML_TYPE_DEC) {
    assert(x->node != NULL);
    assert(x->node->name != NULL);
    fprintf(f, "<%s>", x->node->name);
  } else if (x->node->type == XML_TYPE_NODE ||
	     x->node->type == XML_TYPE_CLOSE) {
    assert(x->node != NULL);
    assert(x->node->name != NULL);
    if (x->node->type == XML_TYPE_CLOSE)
      fprintf(f, "CLOSING:");
    fprintf(f, "<%s", x->node->name);

    xmlattr_t *a;
    for (a = x->node->attr; a != NULL; a = a->next) {
      assert(a->name != NULL);
      if (a->value == NULL)
	fprintf(f, " %s=\"\"", a->name);
      else {
	char *t = strG2X(a->value, '"');
	fprintf(f, " %s=%s", a->name, t);
		free(t);
      }
    }
    
    if (x->node->child != NULL || !all_spaces(x->node->posttext)) {
      fprintf(f, ">");

      if (x->node->child != NULL)
	fprint_xml(f, x->node->child);

      if (x->node->posttext != NULL) {
	char *t = strG2X(x->node->posttext, '"');
	t[strlen(t)-1] = '\0';	/* remove last " */
	fputs(t+1, f);
	free(t);
      }

      assert(x->node->name != NULL);
      fprintf(f, "</%s>", x->node->name);
    } else
      fprintf(f, "/>");
#if 0    
  } else if (x->type == XML_TYPE_LIST) {

    fprint_xml(f, x->child);

  } else if (x->type == XML_TYPE_ATTR) {

    fprintf(stderr, "Warning: orphan attribute %s=%s\n", x->name, x->text);
    exit(1);
    fprintf(f, "[attr %s=%s]\n", x->name, x->text);
#endif
  } else {
    fprintf(stderr, "fprint_xml: unknown type %d\n", x->node->type);
    exit(1);
  }

  fprint_xml(f, x->next);
}

/*********************************************************************
  check consistency of xml
*********************************************************************/

static void check_attr(xmlattr_t *a) {
  if (a == NULL)
    return;

  assert(get_strobj(a->name) == a->name);
  assert(get_strobj(a->value) == a->value);

  check_attr(a->next);
}

static void do_check(xmlcell_t*x) {
  if (x == NULL)
    return;
  
  assert(x->node != NULL);

  assert(XML_TYPE_NODE <= x->node->type &&
	 x->node->type <= XML_TYPE_DTD);

  x->node->type |= XML_TYPE_CHECKING;

  assert(get_strobj(x->node->name) == x->node->name);
  assert(get_strobj(x->node->pretext) == x->node->pretext);
  assert(get_strobj(x->node->posttext) == x->node->posttext);

  check_attr(x->node->attr);

  if (x->node->child != NULL) {
    do_check(x->node->child);
  }
  if (x->next != NULL)
    do_check(x->next);
}

static void clear_flag(xmlcell_t *x) {

  assert((x->node->type & XML_TYPE_CHECKING) != 0);

  x->node->type -= XML_TYPE_CHECKING;

  if (x->node->child != NULL)
    clear_flag(x->node->child);
  if (x->next != NULL)
    clear_flag(x->next);
}

void xml_check(xmlcell_t *x) {
  do_check(x);
  clear_flag(x);
}

void init_xos(void) {
  xos.ampersand = get_strobj("&");
  xos.dot = get_strobj(".");
  xos._ = get_strobj("");
  xos._0 = get_strobj("0");
  xos._1 = get_strobj("1");
  xos._2 = get_strobj("2");
  xos._add = get_strobj("add");
  xos._alias = get_strobj("alias");
  xos._and = get_strobj("and");  
  xos._any_list = get_strobj("any_list");
  xos._any_name = get_strobj("any_name");
  xos._any_node = get_strobj("any_node");
  xos._any_value = get_strobj("any_value");
  xos._aptr = get_strobj("aptr");
  xos._attribute = get_strobj("attribute");
  xos._begin = get_strobj("begin");
  xos._break = get_strobj("break");
  xos._callmode = get_strobj("callmode");
  xos._change_ext = get_strobj("change_ext");
  xos._candidate = get_strobj("candidate");
  xos._clause = get_strobj("clause");
  xos._clean = get_strobj("clean");
  xos._comment = get_strobj("comment");
  xos._concat = get_strobj("concat");
  xos._cond = get_strobj("cond");
  xos._condef = get_strobj("condef");
  xos._contains = get_strobj("contains");
  xos._contains_body = get_strobj("contains_body");
  xos._contains_head = get_strobj("contains_head");
  xos._contains_tail = get_strobj("contains_tail");
  xos._context = get_strobj("context");
  xos._ctx = get_strobj("ctx");
  xos._ctxdef = get_strobj("ctxdef");
  //  xos._ctxref = get_strobj("ctxref");
  xos._dep = get_strobj("dep");
  //  xos._dir = get_strobj("dir");
  xos._dirset = get_strobj("dirset");
  xos._div = get_strobj("div");
  xos._drvdat = get_strobj("drvdat");
  xos._drvget = get_strobj("drvget");
  xos._drvput = get_strobj("drvput");
  xos._dst = get_strobj("dst");
  xos._dump_scope = get_strobj("dump_scope");
  xos._dynamic = get_strobj("dynamic");
  xos._elabel = get_strobj("elabel");
  xos._else = get_strobj("else");
  xos._end = get_strobj("end");
  xos._ends = get_strobj("ends");
  xos._eq = get_strobj("eq");
  xos._exec_command = get_strobj("exec_command");
  xos._exit = get_strobj("exit");
  xos._exp = get_strobj("exp");
  xos._false = get_strobj("false");
  xos._file = get_strobj("file");
  xos._filedata = get_strobj("filedata");
  xos._filename = get_strobj("filename");
  xos._file_exist = get_strobj("file_exist");
  xos._file_mtime = get_strobj("file_mtime");
  xos._format = get_strobj("format");
  xos._fortran = get_strobj("fortran");
  xos._funcdef = get_strobj("funcdef");
  //  xos._has = get_strobj("has");
  xos._head = get_strobj("head");
  xos._if = get_strobj("if");
  xos._ignore = get_strobj("ignore");
  xos._include = get_strobj("include");
  xos._info = get_strobj("info");
  xos._is = get_strobj("is");
  xos._is_pair = get_strobj("is_pair");
  xos._is_tmpfile = get_strobj("is_tmpfile");
  xos._head = get_strobj("head");
  xos._kind = get_strobj("kind");
  xos._label = get_strobj("label");
  xos._list = get_strobj("list");
  xos._list_nlabels = get_strobj("list_nlabels");
  xos._listwith = get_strobj("listwith");
  xos._lname = get_strobj("lname");
  xos._load = get_strobj("load");
  xos._load_root = get_strobj("load_root");
  xos._log = get_strobj("log");
  xos._message = get_strobj("message");
  xos._mod = get_strobj("mod");
  xos._mode = get_strobj("mode");
  xos._module = get_strobj("module");
  xos._module_ref = get_strobj("module_ref");
  //  xos._module_def = get_strobj("module_def");
  xos._mul = get_strobj("mul");
  xos._next_bra = get_strobj("next_bra");
  xos._next_ext = get_strobj("next_ext");
  xos._next_tmp = get_strobj("next_tmp");
  xos._name = get_strobj("name");
  xos._nlabel = get_strobj("nlabel");
  xos._norec = get_strobj("norec");
  xos._not = get_strobj("not");
  xos._null = get_strobj("null");
  xos._obj_cons = get_strobj("obj_cons");
  xos._obj_head = get_strobj("obj_head");
  xos._obj_is_null = get_strobj("obj_is_null");
  xos._obj_tail = get_strobj("obj_tail");
  xos._open_module = get_strobj("open_module");
  xos._operate = get_strobj("operate");
  xos._or = get_strobj("or");
  xos._pair = get_strobj("pair");
  xos._pair_fst = get_strobj("pair_fst");  
  xos._pair_snd = get_strobj("pair_snd");  
  xos._param = get_strobj("param");
  xos._paren = get_strobj("paren");
  xos._pragma = get_strobj("pragma");
  xos._print = get_strobj("print");
  xos._private = get_strobj("private");
  xos._procdef = get_strobj("procdef");
  // xos._prog = get_strobj("prog");
  xos._quote = get_strobj("quote");
  xos._read = get_strobj("read");
  xos._read_xml = get_strobj("read_xml");
  xos._relabel = get_strobj("relabel");
  xos._remove_tmp_suffix = get_strobj("remove_tmp_suffix");
  xos._rename = get_strobj("rename");
  xos._replace = get_strobj("replace");
  xos._return = get_strobj("return");
  xos._same = get_strobj("same");
  xos._searchfile = get_strobj("searchfile");
  xos._set = get_strobj("set");
  xos._set_head = get_strobj("set_head");
  xos._set_tail = get_strobj("set_tail");  
  xos._set_fst = get_strobj("set_fst");  
  xos._set_snd = get_strobj("set_snd");  
  xos._SgBasicBlock = get_strobj("SgBasicBlock");
  xos._SgBoolValExp = get_strobj("SgBoolValExp");
  xos._SgClassDefinition = get_strobj("SgClassDefinition");
  xos._SgContainsStatement = get_strobj("SgContainsStatement");
  xos._SgExprListExp = get_strobj("SgExprListExp");
  xos._SgExprStatement = get_strobj("SgExprStatement");
  xos._SgFunctionCallExp = get_strobj("SgFunctionCallExp");
  xos._SgFunctionRefExp = get_strobj("SgFunctionRefExp");
  xos._SgGlobal = get_strobj("SgGlobal");
  xos._SgIntVal = get_strobj("SgIntVal");
  xos._SgModuleStatement = get_strobj("SgModuleStatement");
  xos._SgPragma = get_strobj("SgPragma");
  xos._SgPragmaDeclaration = get_strobj("SgPragmaDeclaration");
  xos._SgProcedureHeaderStatement = get_strobj("SgProcedureHeaderStatement");
  xos._SgProgramHeaderStatement = get_strobj("SgProgramHeaderStatement");
  xos._SgSourceFile = get_strobj("SgSourceFile");
  xos._SgStringVal = get_strobj("SgStringVal");
  xos._SgVarRefExp = get_strobj("SgVarRefExp");
  xos._single = get_strobj("single");
  xos._slabel = get_strobj("slabel");
  xos._source = get_strobj("source");
  xos._spec_end = get_strobj("spec_end");
  xos._src = get_strobj("src");
  xos._stmt = get_strobj("stmt");
  xos._stmt_with_end = get_strobj("stmt_with_end");
  xos._stmt_with_end_opt = get_strobj("stmt_with_end_opt");
  xos._string = get_strobj("string");
  xos._sub = get_strobj("sub");
  xos._term = get_strobj("term");
  xos._tgen = get_strobj("tgen");
  xos._tmpdir = get_strobj("tmpdir");
  xos._tmpfile = get_strobj("tmpfile");
  xos._trans = get_strobj("trans");
  xos._true = get_strobj("true");
  xos._while = get_strobj("while");
  xos._write = get_strobj("write");
  xos._write_xml = get_strobj("write_xml");
  xos._uname = get_strobj("uname");
  xos._untmpfile = get_strobj("untmpfile");
  xos._use = get_strobj("use");
  xos._utils = get_strobj("utils");
  xos._val = get_strobj("val");
  xos._var = get_strobj("var");
  xos._vardef = get_strobj("vardef");
  xos._vardef_private = get_strobj("vardef_private");
  xos._variable_name = get_strobj("variable_name");
  xos._value = get_strobj("value");
  xos._xev = get_strobj("xev");
  xos._xev_boolean = get_strobj("xev_boolean");
  xos._xev_candidate = get_strobj("xev_candidate");
  xos._xev_clause = get_strobj("xev_clause");
  xos._xev_clause_list = get_strobj("xev_clause_list");
  xos._xev_comment = get_strobj("xev_comment");
  xos._xev_func_call = get_strobj("xev_func_call");
  //  xos._xev_func_def = get_strobj("xev_func_def");
  xos._xev_integer = get_strobj("xev_integer");
  xos._xev_list = get_strobj("xev_list");
  xos._xev_literal = get_strobj("xev_literal");
  xos._xev_pair = get_strobj("xev_pair");
  xos._xev_pragma = get_strobj("xev_pragma");
  xos._xev_single = get_strobj("xev_single");
  xos._xev_string = get_strobj("xev_string");
  xos._xev_target = get_strobj("xev_target");
  xos._xevdmp_root = get_strobj("xevdmp_root");
  xos._xevdr_obj_clauses = get_strobj("xevdr_obj_clauses");
  xos._xevdr_obj_stmts = get_strobj("xevdr_obj_stmts");
  xos._xevdrs = get_strobj("xevdrs");
  xos._xevdrs_line = get_strobj("xevdrs_line");
  xos._xevdrs_main = get_strobj("xevdrs_main");
  xos._xevdrs_program = get_strobj("xevdrs_program");
  xos._xevdrs_root = get_strobj("xevdrs_root");
  xos._xevparse = get_strobj("xevparse");
  xos._xevparse_code_exp = get_strobj("xevparse_code_exp");
  xos._xevparse_comment = get_strobj("xevparse_comment");
  xos._xevparse_conf = get_strobj("xevparse_conf");
  xos._xevparse_extend_pragma = get_strobj("xevparse_extend_pragma");
  xos._xevparse_extract_exp = get_strobj("xevparse_extract_exp");
  xos._xevparse_pragma_names = get_strobj("xevparse_pragma_names");
  xos._xevparse_string_pragma = get_strobj("xevparse_string_pragma");
  xos._xevtgen_conf = get_strobj("xevtgen_conf");
  xos._xevtgen_varformats = get_strobj("xevtgen_varformats");
  xos._xps = get_strobj("xps");
  int i;
  char s[2*XOS_SPACE_MAX+2];
  s[0] = '\n';
  s[1] = '\0';
  for (i=0; i< XOS_SPACE_MAX; i++) {
    xos.spaces[i] = get_strobj(s);
    s[2*i+1] = s[2*i+2] = ' ';
    s[2*i+3] = '\0';
  }
}

/*********************************************************************
  test code
*********************************************************************/

#if MAIN
int main(int argc, char **argv) {
  assert(argc == 2);
  
  init_xos();

  FILE *f = fopen(argv[1], "r");
  assert(f != NULL);

  xmlcell_t *x = read_xml(f);

  xml_check(x);
  
  fprint_xml(stdout, x);

  char line[1024];
  while (1) {
    fgets(line, 1024, stdin);

    /* remove newline */
    char *e = line + strlen(line);
    while (e >= line && *e < ' ')
      *(e--) = '\0';

    if (e == line)
      break;

    strobj s = xml_getstr(x, line);
    printf("%s\n", s);
  }

  printf("\n *** checking xml_chk_struct ***\n");

  while (1) {
    fgets(line, 1024, stdin);

    /* remove newline */
    char *e = line + strlen(line);
    while (e >= line && *e < ' ')
      *(e--) = '\0';

    if (e == line)
      break;

    int chk = xml_chk_struct(x, line);
    printf("%d\n", chk);
  }

  free_xml(x);

  return 0;
}
#endif
