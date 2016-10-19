#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevtgen.h"

#define DEBUG 0
#define SUPPRESS_WARNING 1
#define CTXFOR 1		/* forward reference of context */

#define TGV_VAR 0
#define TGV_LIST 1

#if 0
#define TGV_CTXT 2
#endif
#define TGV_OP 2
#define TGV_NOREC 4
#define TGV_CALL 8

#define TGV_EXP 16
#define TGV_TERM 32
#define TGV_CLSE 48
#define TGV_STMT 64
#define TGV_NAME 80
#define TGV_VAL  96
#define TGV_ENDS 112
#define TGV_IGNORE 128

#define TGV_SLABEL 0x100
#define TGV_LABEL  0x200
#define TGV_NLABEL 0x400
#define TGV_ELABEL 0x800

#define TGV(X) (X & 0xF0)

static void fprint_type(FILE *f, int type) {
  fprintf(f, "%d", type);

  if (type & TGV_LIST)
    fprintf(f, " list");
  else
    fprintf(f, " var");

#if 0
  if (type & TGV_CTXT)
    fprintf(f, " ctx");
#else
  if (type & TGV_OP)
    fprintf(f, " op");
#endif
  if (type & TGV_NOREC)
    fprintf(f, " norec");

  switch (TGV(type)) {
  case TGV_EXP :
    fprintf(f, " exp");
    break;
  case TGV_TERM :
    fprintf(f, " term");
    break;
  case TGV_CLSE :
    fprintf(f, " clause");
    break;
  case TGV_STMT :
    fprintf(f, " stmt");
    break;
  case TGV_NAME :
    fprintf(f, " name");
    break;
  case TGV_VAL :
    fprintf(f, " value");
    break;
  case TGV_ENDS :
    fprintf(f, " ends");
    break;
  default :
    fprintf(f, " unknown");
    break;
  }
}

typedef struct VARFORM {
  int type;
  xmlcell_t *form;
  struct VARFORM *next;
} varform_t;

typedef struct {
  varform_t *vflist;
} tgenconf_t;

static tgenconf_t *tconf = NULL;

static void load_varformat(strobj type, xmlcell_t *x) {
  assert(tconf != NULL);
  assert(x != NULL);

  varform_t *vt = (varform_t*) malloc(sizeof(varform_t));
  assert(vt != NULL);

  vt->type = (type == xos._name ? TGV_NAME :
	      (type == xos._value ? TGV_VAL :
	       (type == xos._ignore ? TGV_IGNORE :
		(type == xos._term ? TGV_TERM :
		 (type == xos._clause ? TGV_CLSE :
		  (type == xos._exp ? TGV_EXP :
		   (type == xos._stmt ? TGV_STMT :
		    (type == xos._ends ? TGV_ENDS : -1))))))));
  assert(vt->type != -1);

  vt->form = xml_destructive_cellcopy(x);
  vt->next = tconf->vflist;
  tconf->vflist = vt;
}

static int do_match_varformat(xmlcell_t *x, xmlcell_t *f, strobj *nam) {

  if (x == NULL || f == NULL)
    return 1;

  if (xml_get_name(f) == xos._any_list)
    return 1;

  if (xml_get_name(f) != xos._any_node) {

    if (xml_get_name(x) != xml_get_name(f))
      return 0;

#if DEBUG
    printf("matching %s\n", xml_get_name(x));
#endif

    xmlattr_t *xa = x->node->attr;
    xmlattr_t *fa = f->node->attr;

    while (xa != NULL && fa != NULL) {
#if DEBUG
      printf("xa %s %s fa %s %s\n", xa->name, xa->value, fa->name, fa->value);
#endif

      if (xa->name != fa->name)
	return 0;

      if (fa->value == xos._variable_name)
	*nam = xa->value;
      else if (xa->value != fa->value && fa->value != xos._any_value)
	return 0;

      xa = xa->next;
      fa = fa->next;
    }
    if (xa != NULL || fa != NULL)
      return 0;

#if DEBUG
    printf("here\n");
#endif

    xmlcell_t *xx, *ff;
    xx = xml_get_child(x);
    ff = xml_get_child(f);
    if (xx != NULL || ff != NULL) {
      if (xx == NULL || ff == NULL)
	return 0;

      strobj cname = NULL;
      int res = do_match_varformat(xx, ff, &cname);
      if (res == 0)
	return 0;
      if (*nam == NULL)
	*nam = cname;
    }
  }

  xmlcell_t *xx, *ff;
  xx = xml_get_next(x);
  ff = xml_get_next(f);
  if (xx != NULL || ff != NULL) {
    if (xx == NULL || ff == NULL)
      return 0;

    strobj nname = NULL;
    int res = do_match_varformat(xx, ff, &nname);
    if (res == 0)
      return 0;
    if (*nam == NULL)
      *nam = nname;
  }

  return 1;
}

static strobj match_varformat(xmlcell_t *x, xmlcell_t *f) {
  xmlcell_t *xx = xml_make_temporal_copy(x);

#if DEBUG
  if (xml_get_name(xx) == xml_get_name(f)) {
    printf("try to match");
    fprint_xml(stdout, xx);
    printf("\n and");
    fprint_xml(stdout, f);
    printf("\n");
  }
#endif

  strobj name = NULL;
  int res = do_match_varformat(xx, f, &name);

#if DEBUG
  if (res == 1)
    printf("*** matched name=%s\n", name);
#endif

  xml_delete_temporal_copy(xx);

  if (res == 0)
    return NULL;
  return name;
}

static strobj check_varformat(xmlcell_t *x, int *ftype) {
  varform_t *vt;

  for (vt = tconf->vflist; vt != NULL; vt = vt->next)
    if (vt->type == TGV_EXP || vt->type == TGV_TERM ||
	vt->type == TGV_CLSE || vt->type == TGV_STMT ||
	vt->type == TGV_ENDS) {
      strobj name = match_varformat(x, vt->form);

      if (name != NULL) {
	*ftype = vt->type;
	return name;
      }
    }

  return NULL;
}

void xevtgen_init(void) {
  if (tconf != NULL)
    return;

  FILE *f = fopen(search_file("xevtgen." XT_FORMATSTR ".conf"), "r");
  if (f == NULL) {
    fprintf(stderr, "xevtgen: cannot open xevtgen." XT_FORMATSTR ".conf\n");
    exit(1);
  }

  xmlcell_t *x = read_xml(f);
  fclose(f);

  xml_check(x);
  xml_set_static(x);

  tconf = (tgenconf_t*) malloc(sizeof(tgenconf_t));
  assert(tconf != NULL);

  tconf->vflist = NULL;

  xmlcell_t *xorg = x;
  while (x != NULL && xml_cell_type(x) != XML_TYPE_NODE)
    x = xml_get_next(x);

  if (x != NULL && xml_get_name(x) != xos._xevtgen_conf) {
    fprintf(stderr, "xevtgen: config file must have one xevtgen_conf tree\n");
    exit(1);
  }

  x = xml_get_child(x);
  while (x != NULL) {
    if (xml_cell_type(x) == XML_TYPE_NODE) {

      strobj name = xml_get_name(x);
      if (name == xos._xevtgen_varformats) {

	xmlcell_t *y = xml_get_child(x);
	while (y != NULL) {

	  strobj type = xml_get_name(y);
	  if (type != xos._name && type != xos._value && type != xos._ignore &&
	      type != xos._term && type != xos._clause &&
	      type != xos._exp && type != xos._stmt && type != xos._ends) {
	    fprintf(stderr, "xevtgen config: unknown varformat %s\n", type);
	    exit(1);
	  }

	  xmlcell_t *z = xml_get_child(y);
	  while (z != NULL) {
	    load_varformat(type, z);
	    z = xml_get_next(z);
	  }

	  y = y->next;
	}

      } else {
	fprintf(stderr, "xevtgen: unknown configuration %s\n", name);
	exit(1);
      }
    }

    x = x->next;
  }

#if 0
  tgenconf_t *t;
  
  for (t = tconf; t != NULL;) {
    varform_t *v;
    for (v = t->vflist; v != NULL; v = v->next) {
      fprint_type(stdout, v->type);
      fprint_xml(stdout, v->form);
      fprintf(stdout, "\n");
    }
  }
#endif
   
  free_xml(xorg);
}

typedef struct INCOND {
  strobj pat;
  struct INCOND *next;
} incond_t;

typedef struct TGENVAR {
  int type;
  int id;
  int diff;
  strobj name;
  strobj pat;
  strobj callmode;
  xmlcell_t *callparam;
  xmlcell_t *cond;
  incond_t *incond;
  xmlcell_t *operate;
  struct TGENVAR *alias;
  struct TGENVAR *next;
} tgenvar_t;

int num_variables = 0;		/* counted at first path */
tgenvar_t *varlist = NULL;
int var_cnt = 0;		/* counter for second path */

void new_tgenvar(int type, strobj name, strobj callmode, xmlcell_t *callparam, xmlcell_t *cond,
		 strobj alias, xmlcell_t *operate) {
  tgenvar_t *tv = (tgenvar_t*) malloc(sizeof(tgenvar_t));
  assert(tv != NULL);

  if (TGV(type) == 0) {
    fprintf(stderr, "xevtgen: variable %s type unspecified\n", name);
    exit(1);
  }

  if ((type & TGV_LIST) && (TGV(type) == TGV_NAME || TGV(type) == TGV_VAL)) {
    fprintf(stderr, "xevtgen: name or value variable %s cannot be a list\n", name);
    exit(1);
  }

  if (operate != NULL && TGV(type) != TGV_NAME && TGV(type) != TGV_VAL) {
    fprintf(stderr, "xevtgen: operation on non-name non-value variable %s\n", name);
    exit(1);
  }

  tv->name = name;
  if (alias == NULL) {

    tv->type = type;
    tv->id = var_cnt ++;
    tv->diff = -1;
    tv->pat = NULL;
    tv->callmode = callmode;
    tv->callparam = callparam;
    tv->cond = cond;
    tv->alias = NULL;
    tv->operate = operate;

  } else {

    if (cond != NULL || operate != NULL) {
      fprintf(stderr, "xevtgen: alias %s cannot have condition/operate\n", name);
      exit(1);
    }

    tgenvar_t *t = varlist;
    while (t != NULL && t->name != alias)
      t = t->next;

    if (t == NULL) {
      fprintf(stderr, "xevtgen: alias of undefined variable %s\n", alias);
      exit(1);
    }

    if ((t->type | TGV_NOREC | TGV_CALL) !=
	(type | TGV_NOREC | TGV_CALL)) {
      fprintf(stderr, "xevtgen: alias %s type mismatch (original %s", name, alias);
      fprint_type(stderr, t->type);
      fprintf(stderr, ", alias %s", name);
      fprint_type(stderr, type);
      fprintf(stderr, ")\n");
      exit(1);
    }

    tv->alias = t;
    tv->type = type;
    tv->diff = -1;
    tv->id = t->id;
    tv->pat = NULL;
    tv->callmode = callmode;
    tv->callparam = callparam;
    tv->cond = NULL;
    tv->operate = NULL;

  }
  tv->incond = NULL;

  tv->next = varlist;
  varlist = tv;
}

int lookup_tgenvar(strobj name, int *id, strobj *callmode, xmlcell_t **callparam,
		   xmlcell_t **cond, xmlcell_t **operate, int *diff) {
  tgenvar_t *tv;

  for (tv = varlist; tv != NULL; tv = tv->next)
    if (tv->name == name)
      break;

  if (tv == NULL) {
    if (id != NULL)
      *id = -1;
    if (callmode != NULL)
      *callmode = NULL;
    if (callparam != NULL)
      *callparam = NULL;
    if (cond != NULL)
      *cond = NULL;
    if (operate != NULL)
      *operate = NULL;
    if (diff != NULL)
      *diff = -1;
    return 0;
  }

  if (tv->alias != NULL) {
    if (id != NULL)
      *id = tv->alias->id;
    if (cond != NULL)
      *cond = tv->alias->cond;
    if (operate != NULL)
      *operate = tv->alias->operate;
  } else {
    if (id != NULL)
      *id = tv->id;
    if (cond != NULL)
      *cond = tv->cond;
    if (operate != NULL)
      *operate = tv->operate;
  }

  if (callmode != NULL)
    *callmode = tv->callmode;
  if (callparam != NULL)
    *callparam = tv->callparam;
  if (diff != NULL)
    *diff = tv->diff;
  return tv->type;
}

void clear_varpattern(void) {
  tgenvar_t *tv;

#if DEBUG
  printf("clear_varpattern\n");
#endif

  for (tv = varlist; tv != NULL; tv = tv->next) {
    tv->pat = NULL;
    tv->incond = NULL;
  }
}

void register_varpattern(strobj name, strobj pat, int diff) {
  tgenvar_t *tv;

  for (tv = varlist; tv != NULL; tv = tv->next)
    if (tv->name == name)
      break;

  assert(tv != NULL);

  if (tv->alias != NULL)
    tv = tv->alias;
  tv->diff = diff;

#if DEBUG
  printf("register_varpattern %s %s %d\n", name, pat, tv->id);
#endif

  if (tv->pat == NULL)
    tv->pat = pat;
  else {
    fprintf(stderr, "xevtgen WARNING: variable %s has multiple occurance\n", tv->name);
  }
}

void register_varpattern_incond(strobj name, char *s, strobj pat, int diff) {
  tgenvar_t *tv;

  for (tv = varlist; tv != NULL; tv = tv->next)
    if (tv->name == name)
      break;

  assert(tv != NULL);

  if (tv->alias != NULL)
    tv = tv->alias;
  tv->diff = diff;

#if DEBUG
  printf("register_varpattern_incond %s\n", name);
#endif

  incond_t *c = (incond_t*) malloc(sizeof(incond_t));
  assert(c != NULL);

  char res[strlen(s) + strlen(pat) + 12];
  sprintf(res, "%s/self::*/%s", s, pat);
  c->pat = get_strobj(res);
  c->next = NULL;

  if (tv->incond == NULL) {
    tv->incond = c;
  } else {

    incond_t *las = tv->incond;
    while (las->next != NULL)
      las = las->next;
    las->next = c;
  }
}

// mode = 0 : no check of recursive variable
void write_varpattern(FILE *fo, int i, int mode) {

  tgenvar_t *tv;
  for (tv = varlist; tv != NULL; tv = tv->next)
    if (tv->id == i)
      break;

  int type = (tv == NULL ? -1 : tv->type);

  if (tv != NULL && tv->alias != NULL)
    tv = tv->alias;

#if DEBUG
  printf("write_varpattern %d\n", i);
  if (tv == NULL)
    printf("tv == NULL\n");
  else if (tv->pat == NULL)
    printf("tv->pat == NULL\n");
  else
    printf("tv->pat = %s\n", tv->pat);
#endif

  if (tv == NULL || (tv->pat == NULL && tv->incond == NULL)) {
#if !SUPPRESS_WARNING
    if (tv != NULL && (type & TGV_NOREC) == 0 && mode != 0)
      fprintf(stderr, "xevtgen WARNING: variable %s forced norec (context)\n", tv->name);
#endif
    fprintf(fo, "$xevtgenvar%d", i);
  } else if (tv->pat != NULL) {
    fprintf(fo, "%s", tv->pat);
    if (tv->incond != NULL)
      fprintf(stderr, "xevtgen WARNING: variable %s has multiple occurance\n", tv->name);
  } else {
    assert(tv->incond != NULL);
    incond_t *c = tv->incond;
    while (c != NULL) {
      fprintf(fo, "%s", c->pat);
      if (c->next != NULL)
	fprintf(fo, " | ");
      c = c->next;
    }
  }
}

void write_varpattern_params(FILE *fo) {
  int i;

  for (i=0; i< num_variables; i++) {
    fprintf(fo, "          <xsl:with-param name=\"xevtgenvar%d\" select=\"", i);
    write_varpattern(fo, i, 0);
    fprintf(fo, "\"/>\n");
  }
}

void xevtgen_write_varparams(FILE *fo) {
  int i;
  
  for (i=0; i< num_variables; i++)
    fprintf(fo, "    <xsl:param name=\"xevtgenvar%d\" select=\"/..\"/>\n", i);
}

typedef struct CTXDEF {
  int id;
  strobj name;
  struct CTXDEF *next;
} ctxdef_t;

int num_contexts = 0;		/* count at first phase */
ctxdef_t *ctxlist = NULL;
int ctx_count = 0;		/* count at second phase */

int new_context(strobj name) {
  ctxdef_t *ct = (ctxdef_t*) malloc(sizeof(ctxdef_t));
  assert(ct != NULL);

#if CTXFOR
  ct->id = num_contexts ++;
#else
  ct->id = ctx_count;
  ctx_count ++;
#endif

  ct->name = name;

  ct->next = ctxlist;
  ctxlist = ct;

  return ct->id;
}

void del_context(void) {
#if !CTXFOR
  assert(ctxlist != NULL);

  ctxdef_t *ct = ctxlist;
  ctxlist = ct->next;

  free(ct);
#endif
  ctx_count --;
}

int get_ctx_id(void) {
  return ctx_count ++;
}

int next_ctx_id(void) {
  return ctx_count;
}

ctxdef_t *lookup_context(strobj name) {
  ctxdef_t *ct = ctxlist;

  while (ct != NULL && ct->name != name)
    ct = ct->next;

  return ct;
}

void xevtgen_write_context_param(FILE *fo) {
  int i;

  fprintf(fo, "    <xsl:param name=\"xevtgencontext\" select=\"'");
  for (i=0; i< num_contexts; i++)
    fprintf(fo, "f");
  fprintf(fo, "'\"/>\n");

  fprintf(fo, "    <xsl:param name=\"xevtgensibling\" select=\"0\"/>\n");

}


static void write_context_param(FILE *fo, char *ctx_s, int dif) {

  fprintf(fo, "          <xsl:with-param name=\"xevtgencontext\" select=\"");

  if (ctx_s == NULL) {

    fprintf(fo, "$xevtgencontext");

  } else {

    int prev = -1;
    int i;
    for (i=0; i< num_contexts; i++)
      if (ctx_s[i] != 'x') {

	if (num_contexts == 1) {

	  fprintf(fo, "'%c'", ctx_s[i]);

	} else if (prev < 0) {

	  if (i == 0)
	    fprintf(fo, "concat('%c'", ctx_s[i]);
	  else
	    fprintf(fo, "concat(substring($xevtgencontext, 1, %d), '%c'", i, ctx_s[i]);
	  
	} else {

	  if (prev == i-1)
	    fprintf(fo, ", 't'");
	  else
	    fprintf(fo, ", substring($xevtgencontext, %d, %d), '%c'", prev+2, i, ctx_s[i]);

	}

	prev = i;
      }

    if (num_contexts > 1) {
      if (prev == num_contexts-1)
	fprintf(fo, ")");
      else
	fprintf(fo, ", substring($xevtgencontext, %d, %d))", prev+2, num_contexts);
    }
  }

  fprintf(fo, "\"/>\n");

  if (dif == 0 && ctx_s != NULL) {

    int ctx_id = 0;
    if (ctx_s != NULL) {
      while (ctx_s[ctx_id] != 't' && ctx_s[ctx_id] != '\0')
	ctx_id ++;
      assert(ctx_s[ctx_id] != '\0');
    } else {
      assert(0);
      ctx_id = num_contexts + 1;
    }

    fprintf(fo, "          <xsl:with-param name=\"xevtgensibling\" select=\"%d\"/>\n",
	    ctx_id + 1);
  } else
    fprintf(fo, "          <xsl:with-param name=\"xevtgensibling\" select=\"0\"/>\n");
}

static void write_context_test(FILE *fo, xmlcell_t *ctx, int stat) {
  if (ctx == NULL)
    return;

  strobj name = xml_getstr(ctx, "a=name");

  if (name == xos._and) {

    fprintf(fo, " %s (boolean(1)", (stat == 0 ? "and" : "or"));
    write_context_test(fo, xml_get_child(ctx), 0);
    fprintf(fo, ")");

  } else if (name == xos._or) {

    fprintf(fo, " %s (boolean(0)", (stat == 0 ? "and" : "or"));
    write_context_test(fo, xml_get_child(ctx), 1);
    fprintf(fo, ")");

  } else if (name == xos._not) {

    fprintf(fo, " %s not(boolean(1)", (stat == 0 ? "and" : "or"));
    write_context_test(fo, xml_get_child(ctx), 0);
    fprintf(fo, ")");

  } else {

    ctxdef_t *ct = lookup_context(xml_getstr(ctx, "a=name"));
    if (ct == NULL) {
      fprintf(stderr, "context %s is not defined\n", xml_getstr(ctx, "a=name"));
      exit(1);
    }

    fprintf(fo, " %s substring($xevtgencontext, %d, 1) = 't'", 
	    (stat == 0 ? "and" : "or"), ct->id+1);
  }

  write_context_test(fo, xml_get_next(ctx), stat);
}

static int directive_tgen(xmlcell_t *x) {
  if (x == NULL)
    return 0;

  if (xml_get_name(x) != xos._xev_pragma ||
      xml_chk_struct(x, "cca=name=xev") == 0 ||
      xml_chk_struct(x, "ccna=name=tgen") == 0)
    return 0;

  return 1;
}

static strobj is_tgenvar_node(xmlcell_t *x, int *ftype_p) {
  assert(tconf != NULL);
  
  int ftype = 0;
  strobj name = check_varformat(x, &ftype);

#if DEBUG
  if (x != NULL)
    printf("is_tgenvar_node(%s) %s\n", xml_get_name(x), name);
#endif

  int vtype = lookup_tgenvar(name, NULL, NULL, NULL, NULL, NULL, NULL);
  if (vtype == 0)
    return NULL;

  if (TGV(vtype) == TGV_NAME || TGV(vtype) == TGV_VAL)
    return NULL;

  if (TGV(vtype) != TGV(ftype)) {
    fprintf(stderr, "xevtgen: variable %s type mismatch (defined ", name);
    fprint_type(stderr, vtype);
    fprintf(stderr, ", referred ");
    fprint_type(stderr, ftype);
    fprintf(stderr, ")\n");
    exit(1);
  }

  if (ftype_p != NULL)
    *ftype_p = ftype;

  return name;
}

static int is_tgenvar_attr(strobj name) {
  assert(name != NULL);

  varform_t *vt;
  for (vt = tconf->vflist; vt != NULL; vt = vt->next)
    if (xml_getstr(vt->form, "a=name") == name)
      break;

  int retv = 0;

  if (vt != NULL && (vt->type == TGV_NAME || vt->type == TGV_VAL))
    retv = vt->type;

  return retv;
}

static int is_ignore_attr(strobj name) {
  assert(name != NULL);

  varform_t *vt;
  for (vt = tconf->vflist; vt != NULL; vt = vt->next)
    if (xml_get_name(vt->form) == xos._attribute &&
	xml_getstr(vt->form, "a=name") == name)
      break;
  
  if (vt != NULL && vt->type == TGV_IGNORE)
    return vt->type;
  else
    return 0;
}

static int is_ignored_node(strobj name) {
  assert(name != NULL);

  varform_t *vt;
  for (vt = tconf->vflist; vt != NULL; vt = vt->next)
    if (xml_get_name(vt->form) == name &&
	TGV(vt->type) == TGV_IGNORE)
      return vt->type;

  return 0;
}

typedef struct CONDCELL {
  strobj name;
  strobj type;
  int namevalue;
  xmlcell_t *pattern;
  struct CONDCELL *next;
} condcell_t;

condcell_t *condlist = NULL;

static void new_condition(strobj name, strobj type, int namevalue, xmlcell_t *pat) {
  condcell_t *cc = (condcell_t*) malloc(sizeof(condcell_t));
  assert(cc != NULL);

  cc->name = name;
  cc->type = type;
  cc->namevalue = namevalue;
  cc->pattern = pat;
  cc->next = condlist;
  condlist = cc;
}

static condcell_t *lookup_condition(strobj name) {
  condcell_t *cc;
  for (cc = condlist; cc != NULL; cc = cc->next)
    if (cc->name == name)
      break;

  return cc;
}

typedef struct PATCELL {
  int cnt;
  struct PATCELL *next;
} patcell_t;

static patcell_t *push_patstack(patcell_t *patstack, int cnt) {
  patcell_t *pc = (patcell_t*) malloc(sizeof(patcell_t));
  assert(pc != NULL);

  pc->cnt = cnt;
  pc->next = patstack;
  return pc;
}

static patcell_t *pop_patstack(patcell_t *patstack) {
  patcell_t *pc = patstack;
  assert(pc != NULL);

  patstack = pc->next;

  pc->next = NULL;
  free(pc);

  return patstack;
}

static char patbuffer[1000000];
static int patbufp = 0;
static int following_list = 0;

static char condbuffer[1000000];
static int condbufp = 0;

typedef struct CONDP {
  int condbufp;
  struct CONDP *next;
} condp_t;

static condp_t *condp = NULL;

static void push_cond(void) {
  condp_t* c = (condp_t*) malloc(sizeof(condp_t));
  assert(c != NULL);

  c->condbufp = condbufp;
  c->next = condp;
  condp = c;
}

static void pop_cond(void) {
  condp_t* c = condp;
  condp = c->next;

  c->next = NULL;
  free(c);
}

static int cur_condp(void) {
  if (condp == NULL)
    return -1;
  else
    return condp->condbufp;
}

static void write_patstack(patcell_t *ps, FILE *fo, char *buf) {
  assert(ps != NULL);

  if (ps->next == NULL) {

    if (ps->cnt > 1) {

      if (fo == NULL) {
	sprintf(&patbuffer[patbufp], "following-sibling::*[%d]/", ps->cnt-1);
	patbufp += strlen(&patbuffer[patbufp]);
      } else
	fprintf(fo, "following-sibling::*[%d]/", ps->cnt-1);

      if (buf != NULL)
	sprintf(buf, "following-sibling::*[%d]/", ps->cnt-1);

    } else if (ps->cnt < 1) {

      if (following_list) {

	/* skip! */

      } else {

	if (fo == NULL) {
	  sprintf(&patbuffer[patbufp], "preceding-sibling::*[%d]/", 1-ps->cnt);
	  patbufp += strlen(&patbuffer[patbufp]);
	} else
	  fprintf(fo, "preceding-sibling::*[%d]/", 1-ps->cnt);

	if (buf != NULL)
	  sprintf(buf, "preceding-sibling::*[%d]/", 1-ps->cnt);

      }

    }

  } else {

    write_patstack(ps->next, fo, buf);
    if (buf != NULL)
      buf += strlen(buf);

    if (fo == NULL) {
      sprintf(&patbuffer[patbufp], "*[%d]/", ps->cnt);
      patbufp += strlen(&patbuffer[patbufp]);
    } else
      fprintf(fo, "*[%d]/", ps->cnt);

    if (buf != NULL)
      sprintf(buf, "*[%d]/", ps->cnt);

  }
}

/* return -1-c when the last sibling is a list variable */
static int count_sibling(xmlcell_t *x) {
  int c = 0;
  while (x != NULL) {

    strobj name = is_tgenvar_node(x, NULL);
    if (name != NULL && (TGV_LIST & lookup_tgenvar(name, NULL, NULL, NULL, NULL, NULL, NULL)) != 0)
      return -1-c;

    if (!is_ignored_node(xml_get_name(x)))
      c ++;

    x = xml_get_next(x);
  }

  return c;
}

static void write_match_condition(condcell_t *cc, strobj name, FILE *fo);

static void write_match_condition_exp(xmlcell_t *cond, patcell_t *patstack, 
				      int vartype, strobj name, FILE *fo);

// start with [ and end with ], this is the only one
static void write_match_pattern(xmlcell_t *x, FILE *fo, int cnt,
				patcell_t *patstack, int diff) {
  if (x == NULL)
    return;

#if DEBUG
  printf("write_match_pattern, ");
  fprint_xml(stdout, x);
  printf("... GO!\n");
#endif

  if (x->node->type != XML_TYPE_NODE) {
    fprintf(stderr, "xevtgen: nodetype %d not implemented\n", x->node->type);
    exit(1);
  }

  int fin = 0, list = 0;
  int vartype = 0, ftype = 0;
  strobj varname = NULL;
  xmlcell_t *cond = NULL;
  xmlcell_t *operate = NULL;
  xmlattr_t *a = x->node->attr;

  do {

    if (a != NULL) {
      ftype = is_tgenvar_attr(a->name);
      if (ftype != 0)
	varname = a->value;
      else
	varname = NULL;
    } else {
      varname = is_tgenvar_node(x, &ftype);
      fin = 1;
    }

    if (varname != NULL)
      vartype = lookup_tgenvar(varname, NULL, NULL, NULL, &cond, &operate, NULL);
    else
      vartype = 0;

#if DEBUG
    if (vartype != 0) {
      printf("varname %s ftype ", varname);
      fprint_type(stdout, ftype);
      printf(" vartype ");
      fprint_type(stdout, vartype);
      printf("\n");
    }
#endif

    if (vartype != 0 && TGV(vartype) == TGV(ftype)) {
      if (operate != NULL) {
	fprintf(stderr, "xevtgen: variable %s in source should not have operation\n", varname);
	exit(1);
      }

      if (vartype & TGV_LIST)
	list = 1;

      patstack = push_patstack(patstack, cnt);

      int inipat = patbufp;
      write_patstack(patstack, NULL, NULL);
      sprintf(&patbuffer[patbufp], "self::*");
      patbufp += strlen(&patbuffer[patbufp]);

      if ((vartype & TGV_LIST) && cur_condp() == -1) {
	sprintf(&patbuffer[patbufp], " | ");
	patbufp += strlen(&patbuffer[patbufp]);
	write_patstack(patstack, NULL, NULL);
	sprintf(&patbuffer[patbufp], "following-sibling::*");
	patbufp += strlen(&patbuffer[patbufp]);
      }

      if (TGV(vartype) == TGV_NAME || TGV(vartype) == TGV_VAL) {
	assert(a != NULL);
	sprintf(&patbuffer[patbufp], "/@%s", a->name);
	// patbufp?
      }

      strobj pat = get_strobj(&patbuffer[inipat]);

      int i = cur_condp();

      if (i == -1)
	register_varpattern(varname, pat, diff);
      else {
// printf("*** tgenvar %s in condition %s pattern %s\n", varname, condbuffer+i, pat);
	register_varpattern_incond(varname, condbuffer+i, pat, diff);

	if (vartype & TGV_LIST) {
	  sprintf(&patbuffer[patbufp-7], "following-sibling::*");
	  pat = get_strobj(&patbuffer[inipat]);
	  register_varpattern_incond(varname, condbuffer+i, pat, diff);
	}
      }

      patbufp = inipat;

#if 0
      printf("tgenvar %s type %d found: %s\n", varname, vartype, pat);
      fprint_xml(stdout, x);
      printf("\n");
#endif

      patstack = pop_patstack(patstack);

      if (cond != NULL) {
	patstack = push_patstack(patstack, cnt);
	
	fprintf(fo, "[");
	sprintf(&condbuffer[condbufp], "[");
	condbufp ++;
	write_match_condition_exp(cond, patstack, vartype, (a == NULL ? NULL : a->name), fo);
	fprintf(fo, "]");
	sprintf(&condbuffer[condbufp], "]");
	condbufp ++;
	
	patstack = pop_patstack(patstack);
      }
    }

    if (a != NULL)
      a = a->next;
  } while (fin == 0);

  // vartype is of the node
  if (vartype != 0) { // tgen variable

    if ((vartype & TGV_LIST) == 0) {
      patstack = push_patstack(patstack, cnt);

      fprintf(fo, "[");
      sprintf(&condbuffer[condbufp], "[");
      condbufp ++;
      write_patstack(patstack, fo, &condbuffer[condbufp]);
      condbufp += strlen(&condbuffer[condbufp]);
      fprintf(fo, "self::*]");
      sprintf(&condbuffer[condbufp], "self::*]");
      condbufp += strlen(&condbuffer[condbufp]);

      patstack = pop_patstack(patstack);
    }

  } else { // not tgen variable

    patstack = push_patstack(patstack, cnt);

    fprintf(fo, "[");
    sprintf(&condbuffer[condbufp], "[");
    condbufp ++;
    write_patstack(patstack, fo, &condbuffer[condbufp]);
    condbufp += strlen(&condbuffer[condbufp]);
    fprintf(fo, "self::%s]", xml_get_name(x));
    sprintf(&condbuffer[condbufp], "self::%s]", xml_get_name(x));
    condbufp += strlen(&condbuffer[condbufp]);

    int acount = 0;
    xmlattr_t *a = x->node->attr;
    while (a != NULL) {

      if (is_ignore_attr(a->name) || 
#ifndef XFORMAT_1_0_0
	  (a->name == xos._string && xml_get_name(x) == xos._SgIntVal &&
	   lookup_tgenvar(xml_getstr(x, "a=value"), NULL, NULL, NULL, NULL, NULL, NULL) != 0) ||
#endif
	  (is_tgenvar_attr(a->name) &&
	   lookup_tgenvar(a->value, NULL, NULL, NULL, NULL, NULL, NULL) != 0)) {

	if (!is_ignore_attr(a->name)) {
	  fprintf(fo, "[");
	  sprintf(&condbuffer[condbufp], "[");
	  condbufp ++;
	  write_patstack(patstack, fo, &condbuffer[condbufp]);
	  condbufp += strlen(&condbuffer[condbufp]);
	  fprintf(fo, "@%s]", a->name);
	  sprintf(&condbuffer[condbufp], "@%s]", a->name);
	  condbufp += strlen(&condbuffer[condbufp]);
	  acount ++;
	}
	
      } else {

	fprintf(fo, "[");
	sprintf(&condbuffer[condbufp], "[");
	condbufp ++;
	write_patstack(patstack, fo, &condbuffer[condbufp]);
	condbufp += strlen(&condbuffer[condbufp]);
	fprintf(fo, "@%s=&quot;%s&quot;]", a->name, a->value);
	sprintf(&condbuffer[condbufp], "@%s=&quot;%s&quot;]", a->name, a->value);
	condbufp += strlen(&condbuffer[condbufp]);

	acount ++;
      }

      a = a->next;
    }

    fprintf(fo, "[count(");
    sprintf(&condbuffer[condbufp], "[count(");
    condbufp += strlen(&condbuffer[condbufp]);
    write_patstack(patstack, fo, &condbuffer[condbufp]);
    condbufp += strlen(&condbuffer[condbufp]);
    fprintf(fo, "@*)");
    sprintf(&condbuffer[condbufp], "@*)");
    condbufp += strlen(&condbuffer[condbufp]);
    varform_t *vf;
    for (vf = tconf->vflist; vf != NULL; vf = vf->next)
      if (TGV(vf->type) == TGV_IGNORE &&
	  xml_get_name(vf->form) == xos._attribute) {
	strobj aname = xml_getstr(vf->form, "a=name");
	fprintf(fo, "-count(");
	sprintf(&condbuffer[condbufp], "-count(");
	condbufp += strlen(&condbuffer[condbufp]);
	write_patstack(patstack, fo, &condbuffer[condbufp]);
	condbufp += strlen(&condbuffer[condbufp]);
	fprintf(fo, "@%s)", aname);
	sprintf(&condbuffer[condbufp], "@%s)", aname);
	condbufp += strlen(&condbuffer[condbufp]);
      }
    fprintf(fo, "=%d]", acount);
    sprintf(&condbuffer[condbufp], "=%d]", acount);
    condbufp += strlen(&condbuffer[condbufp]);

    int c = count_sibling(xml_get_child(x));

    fprintf(fo, "[count(");
    sprintf(&condbuffer[condbufp], "[count(");
    condbufp += strlen(&condbuffer[condbufp]);
    write_patstack(patstack, fo, &condbuffer[condbufp]);
    condbufp += strlen(&condbuffer[condbufp]);
    fprintf(fo, "*)");
    sprintf(&condbuffer[condbufp], "*)");
    condbufp += strlen(&condbuffer[condbufp]);
    for (vf = tconf->vflist; vf != NULL; vf = vf->next)
      if (TGV(vf->type) == TGV_IGNORE &&
	  xml_get_name(vf->form) != xos._attribute) {
	strobj iname = xml_get_name(vf->form);
	/* currently dont care about attributes */
	fprintf(fo, "-count(");
	sprintf(&condbuffer[condbufp], "-count(");
	condbufp += strlen(&condbuffer[condbufp]);
	write_patstack(patstack, fo, &condbuffer[condbufp]);
	condbufp += strlen(&condbuffer[condbufp]);
	fprintf(fo, "%s)", iname);
	sprintf(&condbuffer[condbufp], "%s)", iname);
	condbufp += strlen(&condbuffer[condbufp]);
      }
    if (c < 0) {
      c = -c-1;
      fprintf(fo, ">=%d]", c);
      sprintf(&condbuffer[condbufp], ">=%d]", c);
      condbufp += strlen(&condbuffer[condbufp]);
    } else {
      fprintf(fo, "=%d]", c);
      sprintf(&condbuffer[condbufp], "=%d]", c);
      condbufp += strlen(&condbuffer[condbufp]);
    }

    write_match_pattern(xml_get_child(x), fo, 1, patstack, diff+1);
    patstack = pop_patstack(patstack);

  }

  if (list == 1 && xml_get_next(x) != NULL) {
    fprintf(stderr, "xevtgen: list variable %s should be the last element\n", varname);
    exit(1);
  }

  write_match_pattern(xml_get_next(x), fo, cnt+1, patstack, diff);
}

static void write_match_condition_exp(xmlcell_t *cond, patcell_t *patstack, 
				      int vartype, strobj name, FILE *fo) {

  strobj condname = xml_getstr(cond, "a=name");

  if (condname == xos._or) {
    fprintf(fo, "(");
    xmlcell_t *c = xml_get_child(cond);
    while(c != NULL) {
      write_match_condition_exp(c, patstack, vartype, name, fo);
      c = xml_get_next(c);
      if (c != NULL)
	fprintf(fo, " or ");
    }
    fprintf(fo, ")");
  } else if (condname == xos._and) {
    fprintf(fo, "(");
    xmlcell_t *c = xml_get_child(cond);
    while (c != NULL) {
      write_match_condition_exp(c, patstack, vartype, name, fo);
      c = xml_get_next(c);
      if (c != NULL)
	fprintf(fo, " and ");
    }
    fprintf(fo, ")");
  } else if (condname == xos._not) {
    fprintf(fo, "not(");
    write_match_condition_exp(xml_get_child(cond), patstack, vartype, name, fo);
    fprintf(fo, ")");
  } else {

    condcell_t *cc = lookup_condition(condname);
    if (cc == NULL) {
      fprintf(stderr, "xevtgen: condition %s is not defined\n", condname);
      exit(1);
    }

    if (vartype & TGV_LIST) {
      push_cond();

      fprintf(fo, "(count(");
      write_patstack(patstack, fo, &condbuffer[condbufp]);
      condbufp += strlen(&condbuffer[condbufp]);
      write_match_condition(cc, name, fo);
      fprintf(fo, ")>0");

      pop_cond();
      push_cond();

      fprintf(fo, " or count(");
      write_patstack(patstack, fo, &condbuffer[condbufp]);
      condbufp += strlen(&condbuffer[condbufp]);
      fprintf(fo, "following-sibling::*/");
      sprintf(&condbuffer[condbufp], "following-sibling::*/");
      condbufp += strlen(&condbuffer[condbufp]);
      write_match_condition(cc, name, fo);
      fprintf(fo, ")>0)");

      pop_cond();
    } else {
      push_cond();

      fprintf(fo, "count(");
      write_patstack(patstack, fo, &condbuffer[condbufp]);
      condbufp += strlen(&condbuffer[condbufp]);
      write_match_condition(cc, name, fo);
      fprintf(fo, ")>0");

      pop_cond();
    }
  }
}

static strobj get_namevalue(xmlcell_t *x) {
  xmlattr_t *a = x->node->attr;
  strobj varname = NULL;

  while (a != NULL) {
    if (is_tgenvar_attr(a->name)) {
      if (varname != NULL) {
	fprintf(stderr, "xevtgen: ambiguous name/value\n");
	exit(1);
      }
      varname = a->value;
    }
    a = a->next;
  }

  return varname;
}

static void write_match_condition(condcell_t *cc, strobj name, FILE *fo) {
#if 0
  printf("cond: type=%s namevalue=%d pattern=", cc->type, cc->namevalue);
  fprint_xml(stdout, cc->pattern);
  printf("\n\n");
#endif

  if (cc->type == xos._contains) {
    fprintf(fo, "descendant-or-self::*");
    sprintf(&condbuffer[condbufp], "descendant-or-self::*");
    condbufp += strlen(&condbuffer[condbufp]);
  } else if (cc->type == xos._is) {
    fprintf(fo, "self::*");
    sprintf(&condbuffer[condbufp], "self::*");
    condbufp += strlen(&condbuffer[condbufp]);
  } else if (cc->type == xos._listwith) {
    fprintf(fo, "self::*[following-sibling::*|self::*]");
    sprintf(&condbuffer[condbufp], "self::*[following-sibling::*|self::*]");
    condbufp += strlen(&condbuffer[condbufp]);
  } else if (cc->type == xos._same) {

    fprintf(stderr, "WARNING: xevtgen same will be deplicated: WARNING\n");

    if (cc->namevalue == 0) {
      fprintf(stderr, "xevtgen: condition same is only for name and value\n");
      exit(1);
    }

    strobj varname = get_namevalue(cc->pattern);

    if (varname == NULL) {
      fprintf(stderr, "xevtgen: condition same needs variable name\n");
      exit(1);
    }
    
    int varid = -1;
    lookup_tgenvar(varname, &varid, NULL, NULL, NULL, NULL, NULL);

    if (cc->namevalue == 1) {
      fprintf(fo, "self::*[@%s = string(", name);
      write_varpattern(fo, varid, 0);
      fprintf(fo, ")]");
    } else {
      fprintf(fo, "self::*[@%s = number(", name);
      write_varpattern(fo, varid, 0);
      fprintf(fo, ")]");
    }

    return;

  } else {
    fprintf(stdout, "xevtgen: condition type %s unimplemented yet\n", cc->type);
    exit(1);
  }

  if (cc->namevalue) {
    strobj varname = get_namevalue(cc->pattern);
    if (varname != NULL) {
      fprintf(fo, "[");
      int c = 0;
      varform_t *vt;
      for (vt = tconf->vflist; vt != NULL; vt = vt->next)
	if ((cc->namevalue == 1 && vt->type == TGV_NAME) ||
	    (cc->namevalue == 2 && vt->type == TGV_VAL)) {
	  if (c ++ > 0)
	    fprintf(fo, " or ");
	  fprintf(fo, "@%s='%s'", xml_getstr(vt->form, "a=name"), varname);
	}
      fprintf(fo, "]");
    } else {
      fprintf(stderr, "xevtgen: cond name not specified\n");
      exit(1);
    }
  } else {
    write_match_pattern(cc->pattern, fo, 1, NULL, 1);
    // maybe the last parameter is not 1
  }
}

static void write_output_pattern(xmlcell_t*, FILE*, int, char*);

static void write_match_context(xmlcell_t *x, FILE *fo, char *ctx_s, xmlcell_t *ext_ctx,
				strobj message) {
#if DEBUG
  printf("write_match_context %s ", ctx_s);
  fprint_xml(stdout, x);
  printf("\n");
#endif

  int cnt = 0;
  xmlcell_t *xx = x;

  while (xx != NULL) {
    clear_varpattern();

    following_list = 0;

    int ftype = 0;
    strobj varname = is_tgenvar_node(xx, &ftype);
    if (varname != NULL) {
      int vartype = lookup_tgenvar(varname, NULL, NULL, NULL, NULL, NULL, NULL);
      if (vartype & (TGV_LIST))
	following_list = 1;
    }

    if (following_list)
      fprintf(fo, "      <xsl:when test=\"count(preceding-sibling::*");
    else
      fprintf(fo, "      <xsl:when test=\"count(self::*");
    write_match_pattern(x, fo, 1 - cnt, NULL, 0);
    if (cnt == 0)
      fprintf(fo, ")>0");
    else {
      int i = 0;
      while (ctx_s[i] != 't' && ctx_s[i] != '\0')
	i ++;
      assert(ctx_s[i] != '\0');
      //      fprintf(fo, ")>0 and $xevtgensibling!=%d", i+1);
      fprintf(fo, ")>0 and ($xevtgensibling = 0 or $xevtgensibling > %d)", i+1);
    }
    if (ext_ctx != NULL)
      write_context_test(fo, ext_ctx, 0);
    fprintf(fo, "\">");

    if (cnt == 0) {
      if (message != NULL)
	fprintf(fo, "<xsl:message>%s</xsl:message>\n", message);
      write_output_pattern(x, fo, 4, ctx_s);
    }
    fprintf(fo, "\n      </xsl:when>\n");
    fprintf(fo, "\n");

    xx = xml_get_next(xx);
    cnt ++;
  }
}

static void write_match_src(xmlcell_t *x, FILE *fo, xmlcell_t *ctx, int ctx_id, strobj message) {
#if DEBUG
  printf("write_match_src");
  fprint_xml(stdout, x);
  printf("\n");
#endif

  int cnt = 1;
  xmlcell_t *xx = xml_get_next(x);

  while (xx != NULL) {

    clear_varpattern();

    following_list = 0;

    int ftype = 0;
    strobj varname = is_tgenvar_node(xx, &ftype);
    if (varname != NULL) {
      int vartype = lookup_tgenvar(varname, NULL, NULL, NULL, NULL, NULL, NULL);
      if (vartype & TGV_LIST)
	following_list = 1;
    }

    if (following_list)
      fprintf(fo, "      <xsl:when test=\"count(preceding-sibling::*");
    else
      fprintf(fo, "      <xsl:when test=\"count(self::*");
    write_match_pattern(x, fo, 1 - cnt, NULL, 0);
    //    fprintf(fo, ")>0 and $xevtgensibling!=%d", ctx_id+1);
    fprintf(fo, ")>0 and ($xevtgensibling = 0 or $xevtgensibling > %d)", ctx_id+1);
    if (ctx != NULL)
      write_context_test(fo, ctx, 0);
    fprintf(fo, "\">");
    fprintf(fo, "\n        </xsl:when>\n");

    xx = xml_get_next(xx);
    cnt ++;
  }

  clear_varpattern();

  fprintf(fo, "        <xsl:when test=\"count(self::*");
  write_match_pattern(x, fo, 1, NULL, 0);
  fprintf(fo, ")>0");

  if (ctx != NULL)
    write_context_test(fo, ctx, 0);
  fprintf(fo, "\">\n");
  if (message != NULL)
    fprintf(fo, "<xsl:message>%s</xsl:message>\n", message);

}

static void write_operate(FILE *fo, xmlcell_t *operate) {
  strobj type = xml_get_name(operate);

  if (type == xos._xev_func_call) {
    strobj name = xml_getstr(operate, "a=name");

    if (name == xos._add) {

      fprintf(fo, "(number(");
      write_operate(fo, xml_get_cell(operate, "c"));
      fprintf(fo, ") + number(");
      write_operate(fo, xml_get_cell(operate, "cn"));
      fprintf(fo, "))");

    } else if (name == xos._sub) {

      fprintf(fo, "(number(");
      write_operate(fo, xml_get_cell(operate, "c"));
      fprintf(fo, ") - number(");
      write_operate(fo, xml_get_cell(operate, "cn"));
      fprintf(fo, "))");

    } else if (name == xos._mul) {

      fprintf(fo, "(number(");
      write_operate(fo, xml_get_cell(operate, "c"));
      fprintf(fo, ") * number(");
      write_operate(fo, xml_get_cell(operate, "cn"));
      fprintf(fo, "))");

    } else if (name == xos._div) {

      fprintf(fo, "(number(");
      write_operate(fo, xml_get_cell(operate, "c"));
      fprintf(fo, ") div number(");
      write_operate(fo, xml_get_cell(operate, "cn"));
      fprintf(fo, "))");

    } else if (name == xos._mod) {

      fprintf(fo, "(number(");
      write_operate(fo, xml_get_cell(operate, "c"));
      fprintf(fo, ") mod number(");
      write_operate(fo, xml_get_cell(operate, "cn"));
      fprintf(fo, "))");

    } else if (name == xos._concat) {

      fprintf(fo, "concat(string(");
      write_operate(fo, xml_get_cell(operate, "c"));
      fprintf(fo, "), string(");
      write_operate(fo, xml_get_cell(operate, "cn"));
      fprintf(fo, "))");

    } else {

      fprintf(stderr, "xevtgen: unkown operation %s\n", name);
      exit(1);

    }

  } else {

    int varid = -1;
    strobj varname = get_namevalue(operate);
    if (varname != NULL)
      lookup_tgenvar(varname, &varid, NULL, NULL, NULL, NULL, NULL);

    if (varid >= 0)
      write_varpattern(fo, varid, 0);
    else {

      if (type == xos._xev_literal || type == xos._SgVarRefExp)
	fprintf(fo, "%s", xml_getstr(operate, "a=name"));
      else if (type == xos._xev_integer || type == xos._SgIntVal)
	fprintf(fo, "%s", xml_getstr(operate, "a=value"));
      else if (type == xos._xev_string || type == xos._SgStringVal)
	fprintf(fo, "&quot;%s&quot;", xml_getstr(operate, "a=value"));
      else {
	fprintf(stderr, "xevtgen: error in operate variable\n");
	fprint_xml(stderr, operate);
	fprintf(stderr, "\n");
	exit(1);
      }
    }

  }
}

static void write_callparam(FILE *fo, xmlcell_t *callparam) {
  if (callparam == NULL)
    return;

  while (callparam != NULL) {
    if (xml_get_name(callparam) != xos._xev_func_call ||
	xml_getstr(callparam, "a=name") != xos._pair) {
      fprintf(stderr, "xevtgen: callparam format needs pair\n");
      exit(1);
    }

    xmlcell_t *e1 = xml_get_child(callparam);
    if (e1 == NULL) {
      fprintf(stderr, "xevtgen: callparam pair 1st element missing\n");
      exit(1);
    }

    xmlcell_t *e2 = xml_get_next(e1);
    if (e2 == NULL) {
      fprintf(stderr, "xevtgen: callparam pair 2nd element missing\n");
      exit(1);
    }

    if (xml_get_name(e1) == xos._xevparse_extract_exp)
      e1 = xml_get_child(e1);

    if (xml_get_name(e1) != xos._xev_string && 
	xml_get_name(e1) != xos._SgStringVal) {
      fprintf(stderr, "xevtgen: callparam pair 1st element must be string\n");
      exit(1);
    }

    if (xml_get_name(e2) == xos._xevparse_extract_exp)
      e2 = xml_get_child(e2);

    fprintf(fo, "<xsl:with-param name=\"%s\" select=\"", xml_getstr(e1, "a=value"));

    int varid = -1;
    strobj varname = is_tgenvar_node(e2, NULL);
    if (varname == NULL)
      varname = get_namevalue(e2);
    if (varname != NULL)
      lookup_tgenvar(varname, &varid, NULL, NULL, NULL, NULL, NULL);

    if (varid >= 0)
      write_varpattern(fo, varid, 0);
    else {

      strobj type = xml_get_name(e2);
      if (type == xos._xev_string || type == xos._SgStringVal)
	fprintf(fo, "'%s'", xml_getstr(e2, "a=value"));
      else if (type == xos._xev_integer || type == xos._SgIntVal)
	fprintf(fo, "'%s'", xml_getstr(e2, "a=value"));
      else if (type == xos._xev_literal || type == xos._SgVarRefExp)
	fprintf(fo, "'%s'", xml_getstr(e2, "a=name"));
      else {
	fprintf(stderr, "xevtgen: modeparam unknown type %s\n", type);
	exit(1);
      }
    }

    fprintf(fo, "\"/>\n");
    
    callparam = xml_get_next(callparam);
  }
}

static void write_output_pattern(xmlcell_t *x, FILE *fo, int depth, char *ctx_s) {
  if (x == NULL)
    return;

  int vartype = 0, ftype = 0;
  int varid = -1;
  strobj callmode = NULL;
  xmlcell_t *callparam = NULL;
  strobj varname = is_tgenvar_node(x, &ftype);
  xmlcell_t *operate = NULL;
  int diff = -1;

  if (varname != NULL) {
    vartype = lookup_tgenvar(varname, &varid, &callmode, &callparam, NULL, &operate, &diff);

#if DEBUG
    printf("varname %s vartype %d varid %d callmode %s diff %d\n",
	   varname, vartype, varid, callmode, diff);
#endif

    if ((vartype & TGV_NOREC) == 0) {
      fprintf(fo, "%s", xos.spaces[depth]);
      if (callmode != NULL)
	fprintf(fo, "<xsl:apply-templates mode=\"%s\" select=\"", callmode);
      else
	fprintf(fo, "<xsl:apply-templates select=\"");
      //write_ctxpattern(fo, varname);
      write_varpattern(fo, varid, 1);
      fprintf(fo, "\">\n");
      if (callmode == NULL) {
	write_context_param(fo, ctx_s, diff);
	write_varpattern_params(fo);
      } else
	write_callparam(fo, callparam);
      fprintf(fo, "%s", xos.spaces[depth]);
      fprintf(fo, "</xsl:apply-templates>"); 
    } else if (TGV(vartype) != TGV_IGNORE) {
      fprintf(fo, "%s", xos.spaces[depth]);
      fprintf(fo, "<xsl:copy-of select=\"");
      write_varpattern(fo, varid, 0);
      fprintf(fo, "\"/>");
    }

  }

  if (vartype == 0) {

    fprintf(fo, "%s", xos.spaces[depth]);
    fprintf(fo, "<xsl:element name=\"%s\">", xml_get_name(x));
  
    xmlattr_t *a = x->node->attr;
    while (a != NULL) {

      fprintf(fo, "%s", xos.spaces[depth+1]);

      int varid = -1;
      xmlcell_t *operate = NULL;
      if (is_tgenvar_attr(a->name) &&
	  lookup_tgenvar(a->value, &varid, NULL, NULL, NULL, &operate, NULL)) {

	fprintf(fo, "<xsl:attribute name=\"%s\"><xsl:value-of select=\"", a->name);
	if (operate != NULL)
	  write_operate(fo, operate);
	else
	  write_varpattern(fo, varid, 0);
	fprintf(fo, "\"/></xsl:attribute>");
#ifndef XFORMAT_1_0_0
      } else if (xml_get_name(x) == xos._SgIntVal &&
		 lookup_tgenvar(xml_getstr(x, "a=value"), &varid, NULL, NULL, NULL, &operate, NULL)) {

	fprintf(fo, "<xsl:attribute name=\"string\"><xsl:value-of select=\"");
	if (operate != NULL)
	  write_operate(fo, operate);
	else
	  write_varpattern(fo, varid, 0);
	fprintf(fo, "\"/></xsl:attribute>");
#endif
      } else {
	fprintf(fo, "<xsl:attribute name=\"%s\">%s</xsl:attribute>", a->name, a->value);
      }
      a = a->next;
    }

    write_output_pattern(xml_get_child(x), fo, depth+1, ctx_s);
    fprintf(fo, "%s", xos.spaces[depth]);
    fprintf(fo, "</xsl:element>");
  }

  write_output_pattern(xml_get_next(x), fo, depth, ctx_s);
}

static void write_output_dst(xmlcell_t *x, FILE *fo, char *ctx_s) {
#if DEBUG
  printf("write_output_dst");
  fprint_xml(stdout, x);
  printf("\n");
#endif

  write_output_pattern(x, fo, 4, ctx_s);
  fprintf(fo, "\n        </xsl:when>\n");
  fprintf(fo, "\n");
}

xmlcell_t *get_context(xmlcell_t *x) {
  while (x != NULL) {

    if (xml_get_name(x) == xos._xev_clause &&
	xml_getstr(x, "a=name") == xos._context)
      return xml_get_child(x);

    x = xml_get_next(x);
  }

  return NULL;
}

xmlcell_t *get_src(xmlcell_t *x) {
  while (x != NULL) {

    if (xml_get_name(x) == xos._xev_clause &&
	xml_getstr(x, "a=name") == xos._src)
      return x;

    x = xml_get_next(x);
  }

  return NULL;
}

xmlcell_t *get_dst(xmlcell_t *x) {
  while (x != NULL) {

    if (xml_get_name(x) == xos._xev_clause &&
	xml_getstr(x, "a=name") == xos._dst)
      return x;

    x = xml_get_next(x);
  }

  return NULL;
}

int xlength(xmlcell_t *x) {
  if (x == NULL)
    return 0;

  return 1 + xlength(xml_get_next(x));
}

static void set_context_string(char *ctx_s, int ctx_id, xmlcell_t *cl) {
  int i;

  for (i=0; i< num_contexts; i++)
    ctx_s[i] = 'x';
  ctx_s[i] = '\0';

  assert(0 <= ctx_id && ctx_id < num_contexts);
  ctx_s[ctx_id] = 't';

  while (cl != NULL) {
    if (xml_chk_struct(cl, "a=name=cancel")) {
      xmlcell_t *l = xml_get_child(cl);
      while (l != NULL) {
	strobj name = xml_getstr(l, "a=name");
	ctxdef_t *ct = lookup_context(name);
	if (ct == NULL) {
	  fprintf(stderr, "xevtgen: canceling unknown context %s\n", name);
	  exit(1);
	}

	assert(0 <= ct->id && ct->id < num_contexts);
	ctx_s[ct->id] = 'f';
	l = xml_get_next(l);
      }
    }
    cl = xml_get_next(cl);
  }
}

strobj get_context_name(xmlcell_t *cl) {
  while (cl != NULL) {
    if (xml_getstr(cl, "a=name") == xos._ctxdef)
      return xml_getstr(cl, "ca=name");
    cl = cl->next;
  }

  return NULL;
}

strobj get_message(xmlcell_t *cl) {
  while (cl != NULL) {
    if (xml_getstr(cl, "a=name") == xos._message)
      return xml_getstr(cl, "ca=value");
    cl = cl->next;
  }

  return  NULL;
}

static int has_exp(xmlcell_t *c) {
  assert(c != NULL);

  for (c = xml_get_child(c); c != NULL; c = xml_get_next(c))
    if (xml_getstr(c, "a=name") == xos._exp)
      return 1;

  return 0;
}

static int clean = 0;

void xevtgen_write_template(xmlcell_t *x, FILE *fo) {
  if (x == NULL)
    return;

  int no_go_child = 0;

  if (directive_tgen(x)) {
    xmlcell_t *cl = xml_get_cell(x, "cc");
#if 0
    printf("found");
    fprint_xml(stdout, cl);
    printf("\n");
#endif

    if (xml_chk_struct(cl, "nna=name") == 0) {
      fprintf(stderr, "xevtgen: tgen without clause\n");
      exit(1);
    }

    strobj k = xml_getstr(cl, "nna=name");
#if DEBUG
    printf("type %s\n", k);
#endif

    if (k == xos._var || k == xos._list) {

      if (xml_chk_struct(cl, "nnn") == 0) {
	fprintf(stderr, "xevtgen: var/list without type\n");
	exit(1);
      }

      no_go_child = 0;

      int type = (k == xos._var ? TGV_VAR : TGV_LIST);
      strobj callmode = NULL;
      xmlcell_t *callparam = NULL;
      xmlcell_t *cond = NULL;
      xmlcell_t *operate = NULL;
      xmlcell_t *tl = xml_get_cell(cl, "nnn");
      strobj alias = NULL;
      while (tl != NULL) {
	strobj s = xml_getstr(tl, "a=name");
	if (s == xos._exp)
	  type |= TGV_EXP;
	else if (s == xos._term)
	  type |= TGV_TERM;
	else if (s == xos._clause)
	  type |= TGV_CLSE;
	else if (s == xos._stmt)
	  type |= TGV_STMT;
	else if (s == xos._name)
	  type |= TGV_NAME;
	else if (s == xos._value)
	  type |= TGV_VAL;
	else if (s == xos._ends)
	  type |= TGV_ENDS;
#if 0
	else if (s == xos._ctx || s == xos._aptr)
	  type |= TGV_CTXT;
#endif
	else if (s == xos._norec)
	  type |= TGV_NOREC;
	else if (s == xos._operate) {
	  type |= TGV_OP;
	  operate = xml_get_child(tl);
	} else if (s == xos._callmode || s == xos._mode) {
	  type |= TGV_CALL;
	  callmode = xml_getstr(tl, "ca=value");
	  xmlcell_t *z = xml_get_cell(tl, "c");
	  callparam = xml_get_next(z);
	} else if (s == xos._cond) {
	  cond = xml_get_child(tl);
	} else if (s == xos._begin)
	  ;			/* just skip */
	else if (s == xos._alias) {
	  alias = xml_getstr(tl, "ca=name");
	  if (alias == NULL)
	    alias = xml_getstr(tl, "ca=value");
	  if (alias == NULL) {
	    fprintf(stderr, "xevtgen: variable alias name not given\n");
	    exit(1);
	  }
	} else {
	  fprintf(stderr, "xevtgen: unknown variable/list type %s\n", s);
	  exit(1);
	}

	tl = xml_get_next(tl);
      }

      xmlcell_t *vl = xml_get_cell(cl, "nnc");
      while (vl != NULL) {
	strobj name = xml_getstr(vl, "a=name");
	if (name == NULL)
	  name = xml_getstr(vl, "a=value");
	if (name == NULL) {
	  fprintf(stderr, "xevtgen: variable name cannot find (%s)\n", xml_get_name(vl));
	  exit(1);
	}

	//	printf("register tgenvar %s %d %s %s\n", name, type, callmode, alias);
	new_tgenvar(type, name, callmode, callparam, cond, alias, operate);
	vl = xml_get_next(vl);
      }

    } else if (k == xos._ctxdef) {

      no_go_child = 1;

#if CTXFOR
      //      ctxdef_t *cdef = lookup_context(xml_getstr(cl, "nnca=name"));
      int ctx_id = get_ctx_id();
      //      assert(cdef->id == ctx_id);
#else
      int ctx_id = new_context(xml_getstr(cl, "nnca=name"));
#endif
      char ctx_s[num_contexts+1];
      set_context_string(ctx_s, ctx_id, cl);
      xmlcell_t *ext_ctx = get_context(cl);

      strobj message = get_message(cl);

      if (xml_chk_struct(x, "cn=xev_target")) {
	xmlcell_t *pat = xml_get_cell(x, "cnc");
	if (has_exp(xml_get_child(x)))
	  pat = xml_get_child(pat);
	write_match_context(pat, fo, ctx_s, ext_ctx, message);
      } else {

	xmlcell_t *c;
	for (c = cl; c != NULL; c = xml_get_next(c)) {
	  strobj n = xml_getstr(c, "a=name");
	  if (n == xos._exp || n == xos._term) {

	    if (xml_get_child(c) == NULL) {
	      fprintf(stderr, "xevtgen: ctxdef expression not in clause\n");
	      exit(1);
	    }

	    if (n == xos._exp && xml_get_name(xml_get_child(c)) != xos._xevparse_code_exp) {
	      fprintf(stderr, "xevtgen: ctxdef exp must given in code expression\n");
	      exit(1);
	    }

	    if (n == xos._exp)
	      write_match_context(xml_get_cell(c, "cc"), fo, ctx_s, ext_ctx, message);
	    else
	      write_match_context(xml_get_child(c), fo, ctx_s, ext_ctx, message);

	    break;

	  } else if (n == xos._clause) {

	    write_match_context(xml_get_next(c), fo, ctx_s, ext_ctx, message);
	    break;

	  }
	}

	if (c == NULL) {
	  fprintf(stderr, "xevtgen: ctxdef expression clause not given\n");
	  exit(1);
	}
      }

    } else if (k == xos._condef) {

      no_go_child = 1;

      strobj type = xml_getstr(cl, "nnna=name");
      if (type != xos._is && type != xos._contains && type != xos._same &&
	  type != xos._listwith) {
	fprintf(stderr, "xevtgen: condef unknown type %s\n", type);
	exit(1);
      }
      
      int namevalue = 0;

      xmlcell_t *pat = NULL;
      if (xml_chk_struct(x, "cn=xev_target")) {
	pat = xml_get_cell(x, "cnc");
	if (has_exp(xml_get_child(x)))
	  pat = xml_get_child(pat);
      } else {
	xmlcell_t *c;
	for (c = cl; c != NULL; c = xml_get_next(c)) {
	  strobj n = xml_getstr(c, "a=name");
	  if (n == xos._exp || n == xos._term) {

	    if (xml_get_child(c) == NULL) {
	      fprintf(stderr, "xevtgen: condef expression not in clause\n");
	      exit(1);
	    }

	    if (n == xos._exp && xml_getstr(c, "c") != xos._xevparse_code_exp) {
	      fprintf(stderr, "xevtgen: condef exp must given in code expression\n");
	      exit(1);
	    }

	    if (n == xos._exp)
	      pat = xml_get_cell(c, "cc");
	    else
	      pat = xml_get_child(c);

	    break;

	  } else if (n == xos._clause) {

	    pat = xml_get_next(c);
	    break;

	  } else if (n == xos._name || n == xos._value) {

	    if (n == xos._name)
	      namevalue = 1;
	    else
	      namevalue = 2;
	    pat = xml_get_child(c);
	    break;

	  }
	}

	if (c == NULL) {
	  fprintf(stderr, "xevtgen: condef expression clause not given\n");
	  exit(1);
	}
      }

      new_condition(xml_getstr(cl, "nnca=name"), type, namevalue, pat);

    } else if (k == xos._trans) {

      no_go_child = 1;

      strobj t = xml_getstr(cl, "nnna=name");

      xmlcell_t *ctx = get_context(cl);
      xmlcell_t *src = get_src(cl);
      xmlcell_t *dst = get_dst(cl);

#if CTXFOR
      //      ctxdef_t *cdef = lookup_context(ctxname);
      int ctx_id = get_ctx_id();
      //      assert(cdef->id == ctx_id);
#else
      strobj ctxname = get_context_name(cl);
      int ctx_id = new_context(ctxname);
#endif
      char ctx_s[num_contexts+1];
      set_context_string(ctx_s, ctx_id, cl);

      strobj message = get_message(cl);

      if (t == xos._exp) {
	if (xml_chk_struct(x, "cn=xev_target") == 0) {

	  if ((src != NULL && xml_getstr(src, "c") != xos._xevparse_code_exp) ||
	      (dst != NULL && xml_getstr(dst, "c") != xos._xevparse_code_exp)) {
	    fprintf(stderr, "xevtgen: trans exp needs code expression in src and dst\n");
	    exit(1);
	  }

	  if (src != NULL)
	    write_match_src(xml_get_cell(src, "cc"), fo, ctx, ctx_id, message);

	  if (dst != NULL)
	    write_output_dst(xml_get_cell(dst, "cc"), fo, ctx_s);

	} else {

	  if ((src != NULL && dst != NULL) ||
	      (src == NULL && dst == NULL)) {
	    fprintf(stderr, "xevtgen: trans exp block must be either src or dst\n");
	    exit(1);
	  }

	  if (src != NULL)
	    write_match_src(xml_get_cell(x, "cncc"), fo, ctx, ctx_id, message);
	  if (dst != NULL)
	    write_output_dst(xml_get_cell(x, "cncc"), fo, ctx_s);

	}
      
      } else if (t == xos._term) {
	if (src == NULL || dst == NULL) {
	  fprintf(stderr, "xevtgen: trans term needs src and dst\n");
	  exit(1);
	}

	write_match_src(xml_get_child(src), fo, ctx, ctx_id, message);
	write_output_dst(xml_get_child(dst), fo, ctx_s);
	
      } else if (t == xos._clause) {

	if (src == NULL && dst == NULL) {
	  fprintf(stderr, "xevtgen: trans clause needs src or dst\n");
	  exit(1);
	}

	int ls = xlength(src);
	int ld = xlength(dst);

	if (ls > ld) {
	  del_context();
	  write_match_src(xml_get_next(src), fo, ctx, ctx_id, message);
	} else
	  write_output_dst(xml_get_next(dst), fo, ctx_s);

      } else if (t == xos._stmt) {

	if ((src == NULL && dst == NULL) ||
	    (src != NULL && dst != NULL)) {
	  fprintf(stderr, "xevtgen: trans stmt needs src xor dst\n");
	  exit(1);
	}

	if (src != NULL) {
	  del_context();
	  write_match_src(xml_get_cell(x, "cnc"), fo, ctx, ctx_id, message);

	  if (xml_get_next(xml_get_cell(x, "cnn")) != NULL)
	    write_output_dst(xml_get_child(xml_get_cell(x, "cnnn")),
			     fo, ctx_s);

	} else
	  write_output_dst(xml_get_cell(x, "cnc"), fo, ctx_s);

      } else {

	fprintf(stderr, "xevtgen: trans unknown type %s\n", t);
	exit(1);

      }

    } else if (k == xos._src) {

      no_go_child = 1;

      xmlcell_t *ctx = get_context(cl);
      strobj message = get_message(cl);

      write_match_src(xml_get_cell(x, "cnc"), fo, ctx, next_ctx_id(), message);

      if (xml_get_next(xml_get_cell(x, "cnn")) != NULL) {
	int ctx_id = get_ctx_id();
	char ctx_s[num_contexts+1];
	set_context_string(ctx_s, ctx_id, xml_get_cell(x, "cnnn"));
	write_output_dst(xml_get_child(xml_get_cell(x, "cnnn")), fo, ctx_s);
      }

    } else if (k == xos._dst) {

      no_go_child = 1;

#if CTXFOR
      //      ctxdef_t *cdef = lookup_context(ctxname);
      int ctx_id = get_ctx_id();
      //      assert(ctx_id == cdef->id);
#else
      strobj ctxname = get_context_name(cl);
      int ctx_id = new_context(ctxname);
#endif
      char ctx_s[num_contexts+1];
      set_context_string(ctx_s, ctx_id, cl);

      strobj message = get_message(cl);
      if (message != NULL)
	fprintf(fo, "<xsl:message>%s</xsl:message>\n", message);

      xmlcell_t *t = xml_get_cell(x, "cn");
      write_output_dst(xml_get_child(t), fo, ctx_s);

    } else if (k == xos._stmt) {

      fprintf(stderr, "statement variable/list(%s) should not appear independently\n",
	      xml_getstr(cl, "nnca=name"));

    } else if (k == xos._include) {

      no_go_child = 1;

    } else if (k == xos._mode) {

      no_go_child = 1;

      if (xevtgen_mode() != xml_getstr(cl, "nnca=value")) {

	xevtgen_write_otherwise(fo);
	fprintf(fo, "    </xsl:choose>\n");
	fprintf(fo, "  </xsl:template>\n");
	fprintf(fo, "\n");

	fprintf(fo, "  <xsl:template mode=\"%s\" match=\"*\">\n",
		xml_getstr(cl, "nnca=value"));
	xevtgen_write_context_param(fo);
	xevtgen_write_varparams(fo);
	fprintf(fo, "    <xsl:choose>\n");
	
      }

    } else if (k == xos._clean) {

      no_go_child = 1;

      xmlcell_t *kk = xml_get_cell(cl, "nn");
      if (xml_get_child(kk) != NULL) {
	if (xml_getstr(kk, "a=name") == xos._true)
	  clean = 1;
	else if (xml_getstr(kk, "a=name") == xos._false)
	  clean = 0;
	else {
	  fprintf(stderr, "xevtgen: unknown option for clean: choose true or false\n");
	  exit(1);
	}
      } else
	clean = 1;

    } else {

      fprintf(stderr, "xevtgen: unknown line type %s\n", k);
      exit(1);

    }    

#if DEBUG
    printf("done\n");
#endif
  }

  if (no_go_child == 0)
    xevtgen_write_template(xml_get_child(x), fo);

  xevtgen_write_template(xml_get_next(x), fo);

}

static strobj mode = NULL;
static int num_trans = 0;

void xevtgen_count_variables(xmlcell_t *x, FILE *fo) {
  if (x == NULL)
    return;

  int no_go_child = 0;

  if (directive_tgen(x)) {
    xmlcell_t *cl = xml_get_cell(x, "cc");
    strobj k = xml_getstr(cl, "nna=name");

    if (k == xos._var || k == xos._list) {

      no_go_child = 0;

      xmlcell_t *vl = xml_get_cell(cl, "nnc");
      while (vl != NULL) {
	num_variables ++;
	vl = xml_get_next(vl);
      }

    } else if (k == xos._ctxdef) {

      no_go_child = 1;
#if CTXFOR
      new_context(xml_getstr(cl, "nnca=name"));
#else
      num_contexts ++;
#endif
      num_trans ++;

    } else if (k == xos._condef) {

      no_go_child = 1;

    } else if (k == xos._trans) {

      no_go_child = 1;

#if CTXFOR
      strobj t = xml_getstr(cl, "nnna=name");
      int new_ctx = 1;

      if (t == xos._clause) {
	xmlcell_t *src = get_src(cl);
	xmlcell_t *dst = get_dst(cl);
	int ls = xlength(src);
	int ld = xlength(dst);
	if (ls > ld)
	  new_ctx = 0;
      } else if (t == xos._stmt) {
	xmlcell_t *src = get_src(cl);
	if (src != NULL) {
	  new_ctx = 0;
	  
	  if (xml_get_next(xml_get_cell(x, "cnn")) != NULL)
	    new_context(get_context_name(xml_get_cell(x, "cnnn")));
	}
      }

      if (new_ctx) {
	new_context(get_context_name(cl));
      }
#else
      num_contexts ++;
#endif
      num_trans ++;

    } else if (k == xos._src) {

      no_go_child = 1;
      num_trans ++;

      if (xml_get_next(xml_get_cell(x, "cnn")) != NULL)
	new_context(get_context_name(xml_get_cell(x, "cnnn")));

    } else if (k == xos._dst) {

      no_go_child = 1;
#if CTXFOR
      new_context(get_context_name(cl));
#else
      num_contexts ++;
#endif

    } else if (k == xos._stmt) {

      fprintf(stderr, "statement variable/list(%s) should not appear independently\n",
	      xml_getstr(cl, "nnca=name"));

    } else if (k == xos._include) {

      no_go_child = 1;

      if (xml_chk_struct(cl, "nnc=xev_string") == 0) {
	fprintf(stderr, "xevtgen: include file name must be string\n");
	exit(1);
      }

      fprintf(fo, "    <xsl:include href=\"%s\"/>\n", xml_getstr(cl, "nnca=value"));

    } else if (k == xos._mode) {

      no_go_child = 1;
      
      if (xml_chk_struct(cl, "nnc=xev_string") == 0) {
	fprintf(stderr, "xevtgen: mode name must be string\n");
	exit(1);
      }

      if (num_trans == 0)
	mode = xml_getstr(cl, "nnca=value");

    } else if (k == xos._clean) {

      no_go_child = 1;

    } else {

      fprintf(stderr, "xevtgen: unknown line type %s\n", k);
      exit(1);

    }    
  }

  if (no_go_child == 0)
    xevtgen_count_variables(xml_get_child(x), fo);

  xevtgen_count_variables(xml_get_next(x), fo);

}

strobj xevtgen_mode(void) {
  return mode;
}

void xevtgen_write_otherwise(FILE *fo) {
  clear_varpattern();

  if (clean != 0) {
    fprintf(fo, "      <xsl:when test=\"count(self::SgSourceFile)+count(self::SgGlobal)+count(self::SgFunctionParameterList[parent::SgProgramHeaderStatement])+count(self::SgFunctionDefinition[parent::SgProgramHeaderStatement])+count(self::SgBasicBlock[parent::SgFunctionDefinition[parent::SgProgramHeaderStatement]])>0\">\n");
    fprintf(fo, "        <xsl:copy>\n");
    fprintf(fo, "          <xsl:copy-of select=\"@*\"/>\n");
    fprintf(fo, "          <xsl:apply-templates>\n");
    write_context_param(fo, NULL, 1);
    write_varpattern_params(fo);
    fprintf(fo, "          </xsl:apply-templates>\n");
    fprintf(fo, "        </xsl:copy>\n");
    fprintf(fo, "      </xsl:when>\n");
    fprintf(fo, "      <xsl:when test=\"count(self::SgProgramHeaderStatement)>0\">\n");
    fprintf(fo, "        <xsl:element name=\"SgProgramHeaderStatement\">\n");
    fprintf(fo, "          <xsl:attribute name=\"name\">rose_fortran_main</xsl:attribute>\n");
    fprintf(fo, "          <xsl:attribute name=\"definition\">1</xsl:attribute>\n");
    fprintf(fo, "          <xsl:attribute name=\"end_name\">0</xsl:attribute>\n");
    fprintf(fo, "          <xsl:attribute name=\"declaration_modifier\">2</xsl:attribute>\n");
    fprintf(fo, "          <xsl:apply-templates>\n");
    write_context_param(fo, NULL, 1);
    write_varpattern_params(fo);
    fprintf(fo, "          </xsl:apply-templates>\n");
    fprintf(fo, "        </xsl:element>\n");
    fprintf(fo, "      </xsl:when>\n");
    fprintf(fo, "\n");
    fprintf(fo, "      <xsl:when test=\"count(self::xevdrs_program)>0\">\n");
    fprintf(fo, "        <xsl:copy>\n");
    fprintf(fo, "          <xsl:copy-of select=\"@*\"/>\n");
    fprintf(fo, "          <xsl:apply-templates>\n");
    write_context_param(fo, NULL, 1);
    write_varpattern_params(fo);
    fprintf(fo, "          </xsl:apply-templates>\n");
    fprintf(fo, "        </xsl:copy>\n");
    fprintf(fo, "      </xsl:when>\n");
  }

  fprintf(fo, "      <xsl:otherwise>\n");

  if (clean == 0) {
    fprintf(fo, "       <xsl:copy>\n");
    fprintf(fo, "        <xsl:copy-of select=\"@*\"/>\n");
  }
   
  if (mode != NULL)
    fprintf(fo, "        <xsl:apply-templates mode=\"%s\">\n", mode);
  else
    fprintf(fo, "        <xsl:apply-templates>\n");
  write_context_param(fo, NULL, 1);
  write_varpattern_params(fo);
  fprintf(fo, "        </xsl:apply-templates>\n");

  if (clean == 0)
    fprintf(fo, "       </xsl:copy>\n");

  fprintf(fo, "      </xsl:otherwise>\n");
}
