#ifndef __XEVUTIL_H_INCLUDED__
#define __XEVUTIL_H_INCLUDED__
#include <stdio.h>

#if 0
#define malloc(x) debug_malloc(x)
void *debug_malloc(long n);
#define free(x)   debug_free(x)
void debug_free(void *);
#endif

#define XFORMAT_1_1_1 1
#define XEVTOOLSSUBFORMAT 1

#ifdef XFORMAT_1_0_0
#define XFORMATSTR "1.0.0"
#endif

#ifdef XFORMAT_1_1_0
#define XFORMATSTR "1.1.0"
#endif

#ifdef XFORMAT_1_1_1
#define XX_FORMATSTR "1.1.1"
#define XT_FORMATSTR "1.1.1.1"
#endif

typedef char* strobj;

strobj get_strobj(char*);
void dump_strobj(void);
strobj int_strobj(int);
int strobj_int(strobj);

#define xev_integer xos._xev_integer
#define xev_string xos._xev_string
#define xev_boolean xos._xev_boolean
#define xev_true xos._true
#define xev_false xos._false

#define XEV_COMMAND_LINE_SIZE 4096 /* assumed command line size */
#define XEV_PATH_SIZE 4096	   /* assumed path size */
#define XEV_LINE_SIZE 4096	   /* max line size */
#define REMOVE_TMP 0		   /* remove temporary file */

char *xevxml_dir(void);		/* directory of xevxml */
char *xevtools_dir(void);	/* directory of xevtools */
char *tmp_dir(void);		/* directory of temporary files */
char *search_file(char*);	/* search modules */
int xevutil_file_exist(char*);
void get_tmpname(char *fname, char *src);
void first_tmp(char *nxt, char *prv);	/* create first temporary file name */
void next_tmp(char *nxt, char *prv);	/* create next temporary file name */
void prev_tmp(char *prv, char *nxt);	/* look previous temporary file */
void set_ext(char *fname, char *ext);	/* change extension */

int code_type(char*);		/* source code type */

#define CODE_TYPE_FORTRAN 0
#define CODE_TYPE_C 1
#define CODE_TYPE_OTHER 2

int remove_tmp(void);		/* whether remove temporary or not */

void xev_log1(char*);
void xev_log2(char*, char*);
void xev_log3(char*, char*, char*);
void xev_log4(char*, char*, char*, char*);

char *dupstr(char*);
char *copyrange(char *s, char *t);
strobj get_depth_strobj(int);

/* for langstr */

char *endstrF(char*);		/* find end of a fortran string */
char *strchrF(char*, char);	/* strchr considering F string */

char *strF2G(char*);
char *strG2F(char*, char);
char *strC2G(char*);
char *strG2C(char*);
char *strX2G(char*);
char *strG2X(char*, char);

/* for xml */

#define XML_TYPE_NODE 0
#define XML_TYPE_DEC 1
#define XML_TYPE_COMM 2
#define XML_TYPE_DTD 3
#define XML_TYPE_ATTR 4
#define XML_TYPE_LIST 5
#define XML_TYPE_CLOSE 14
#define XML_TYPE_INVALID 15
#define XML_TYPE_CHECKING 16

typedef struct XMLATTR {
  int gcflag;
  strobj name;
  strobj value;
  struct XMLATTR *next;
} xmlattr_t;

typedef struct XMLCELL {
  int gcflag;
  struct XMLNODE *node;
  struct XMLCELL *next;
} xmlcell_t;

typedef struct XMLNODE {
  int gcflag;
  int type;
  strobj name;
  strobj pretext;
  strobj posttext;
  struct XMLATTR *attr;
  struct XMLCELL *child;
  struct XMLNODE *next;		/* only for allocation / GC */
} xmlnode_t;

void check_xmlformat(xmlcell_t*);

int xml_cell_type(xmlcell_t*);
xmlcell_t* xml_make_cell(int, strobj, strobj);
xmlcell_t* xml_make_node(strobj, strobj);
void xml_add_attr(xmlcell_t *x, strobj, strobj);
void xml_reset_attr(xmlcell_t *x, strobj, strobj);

// make a copy of cell, but current cell gets "dummy" node
xmlcell_t* xml_destructive_cellcopy(xmlcell_t*);

// make a temporal copy of cell, without following siblings
xmlcell_t *xml_make_temporal_copy(xmlcell_t*);
void xml_delete_temporal_copy(xmlcell_t*);

// make a deep copy (for quote)
xmlcell_t *xml_make_deep_copy(xmlcell_t*);

void free_xml(xmlcell_t*);

void xml_set_static(xmlcell_t*);
int xml_is_static(xmlcell_t*);
void xml_set_dynamic_all(xmlcell_t*);
void xml_set_dynamic_cell(xmlcell_t*);

void xml_append(xmlcell_t*, xmlcell_t*);
void xml_append_child(xmlcell_t *x, xmlcell_t *c);
void xml_append_attr(xmlattr_t *x, xmlattr_t *y);
void xml_insert_child(xmlcell_t *x, xmlcell_t *c);
void xml_insert_next(xmlcell_t *x, xmlcell_t *n);
void xml_reset_pretext(xmlcell_t *x, strobj s);
void xml_set_posttext(xmlcell_t *x, strobj t);
void xml_reset_posttext(xmlcell_t *x, strobj t);

xmlcell_t *xml_get_next(xmlcell_t*);
xmlcell_t *xml_get_child(xmlcell_t*);
strobj xml_get_name(xmlcell_t*);

/* get operations:
   n: next node
   c: child node
   (end of line): node name
   s: pretext
   t: posttext
   a=name: attribute name
*/
strobj xml_getstr(xmlcell_t *x, char *op);

/* get/detach cell operations:
   n: next node
   c: child node
*/
xmlcell_t *xml_get_cell(xmlcell_t *x, char *op);
xmlcell_t *xml_detach_cell(xmlcell_t *x, char *op);
xmlcell_t *xml_cut_link(xmlcell_t *x, char *op);

/* check structure operations:
   n: next node
   c: child node
   s: pretext
   t: posttext
   =name: node name/pretext/posttext
   a=name: attribute exists
   a=name=value: attribute value
*/
int xml_chk_struct(xmlcell_t *x, char *op);

xmlcell_t *read_xml(FILE *f);
void fprint_xml(FILE *f, xmlcell_t*);
void xml_check(xmlcell_t*);

/* for executable directives */

int xevutil_depend_label(xmlcell_t *lab);
void xevutil_load_depends(char *listname);
xmlcell_t *xevutil_find_depend(xmlcell_t *xev);
int xevutil_depend_exist(strobj filename, xmlcell_t *dep);
void xevutil_mark_executable(xmlcell_t *xev);

void xevutil_load_module(strobj modname, char *parentpath, strobj super);
int xevutil_module_exist(strobj modname);

/* for XOS */

#define XOS_SPACE_MAX 100
#ifndef XEVUTIL_XML_C_DEFINED
extern
#endif
struct {
  strobj ampersand;
  strobj dot;
  strobj _;
  strobj _0;
  strobj _1;
  strobj _2;
  strobj _add;
  strobj _alias;
  strobj _and;
  strobj _any_list;
  strobj _any_name;
  strobj _any_node;
  strobj _any_value;
  strobj _aptr;
  strobj _attribute;
  strobj _begin;
  strobj _break;
  strobj _callmode;
  strobj _candidate;
  strobj _change_ext;
  strobj _clause;
  strobj _clean;
  strobj _comment;
  strobj _concat;
  strobj _cond;
  strobj _condef;
  strobj _contains;
  strobj _contains_body;
  strobj _contains_head;
  strobj _contains_tail;
  strobj _context;
  strobj _ctx;
  strobj _ctxdef;
  //  strobj _ctxref;
  strobj _dep;
  //  strobj _dir;
  strobj _dirset;
  strobj _div;
  strobj _drvdat;
  strobj _drvget;
  strobj _drvput;
  strobj _dst;
  strobj _dump_scope;
  strobj _dynamic;
  strobj _elabel;
  strobj _else;
  strobj _end;
  strobj _ends;
  strobj _eq;
  strobj _exec_command;
  strobj _exit;
  strobj _exp;
  strobj _false;
  strobj _file;
  strobj _filedata;
  strobj _filename;
  strobj _file_exist;
  strobj _file_mtime;
  strobj _format;
  strobj _fortran;
  strobj _funcdef;
  //  strobj _has;
  strobj _head;
  strobj _if;
  strobj _ignore;
  strobj _include;
  strobj _info;
  strobj _is;
  strobj _is_pair;
  strobj _is_tmpfile;
  strobj _kind;
  strobj _label;
  strobj _list;
  strobj _list_nlabels;
  strobj _listwith;
  strobj _lname;
  strobj _load;
  strobj _load_root;
  strobj _log;
  strobj _message;
  strobj _mod;
  strobj _mode;
  strobj _module;
  strobj _module_ref;
  //  strobj _module_def;
  strobj _mul;
  strobj _name;
  strobj _next_bra;
  strobj _next_ext;
  strobj _next_tmp;
  strobj _nlabel;
  strobj _norec;
  strobj _not;
  strobj _null;
  strobj _obj_cons;
  strobj _obj_head;
  strobj _obj_is_null;
  strobj _obj_tail;
  strobj _open_module;
  strobj _operate;
  strobj _or;
  strobj _pair;
  strobj _pair_fst;
  strobj _pair_snd;
  strobj _param;
  strobj _paren;
  strobj _pragma;
  strobj _print;
  strobj _private;
  strobj _procdef;
  //  strobj _prog;
  strobj _quote;
  strobj _read;
  strobj _read_xml;
  strobj _relabel;
  strobj _remove_tmp_suffix;
  strobj _rename;
  strobj _replace;
  strobj _return;
  strobj _same;
  strobj _searchfile;
  strobj _set;
  strobj _set_head;
  strobj _set_tail;
  strobj _set_fst;
  strobj _set_snd;
  strobj _SgBasicBlock;
  strobj _SgBoolValExp;
  strobj _SgContainsStatement;
  strobj _SgClassDefinition;
  strobj _SgExprListExp;
  strobj _SgExprStatement;
  strobj _SgFunctionCallExp;
  strobj _SgFunctionRefExp;
  strobj _SgGlobal;
  strobj _SgIntVal;
  strobj _SgModuleStatement;
  strobj _SgPragma;
  strobj _SgPragmaDeclaration;
  strobj _SgProcedureHeaderStatement;
  strobj _SgProgramHeaderStatement;
  strobj _SgSourceFile;
  strobj _SgStringVal;
  strobj _SgVarRefExp;
  strobj _single;
  strobj _slabel;
  strobj _source;
  strobj _spec_end;
  strobj _src;
  strobj _stmt;
  strobj _stmt_with_end;
  strobj _stmt_with_end_opt;
  strobj _string;
  strobj _sub;
  strobj _term;
  strobj _tgen;
  strobj _tmpdir;
  strobj _tmpfile;
  strobj _trans;
  strobj _true;
  strobj _uname;
  strobj _untmpfile;
  strobj _use;
  strobj _utils;
  strobj _val;
  strobj _var;
  strobj _vardef;
  strobj _vardef_private;
  strobj _variable_name;
  strobj _value;
  strobj _while;
  strobj _write;
  strobj _write_xml;
  strobj _xev;
  strobj _xev_boolean;
  strobj _xev_candidate;
  strobj _xev_clause;
  strobj _xev_clause_list;
  strobj _xev_comment;
  strobj _xev_func_call;
  //  strobj _xev_func_def;
  strobj _xev_integer;
  strobj _xev_list;
  strobj _xev_literal;
  strobj _xev_pair;
  strobj _xev_pragma;
  strobj _xev_single;
  strobj _xev_string;
  strobj _xev_target;
  strobj _xevdmp_root;
  strobj _xevdr_obj_clauses;
  strobj _xevdr_obj_stmts;
  strobj _xevdrs;
  strobj _xevdrs_line;
  strobj _xevdrs_main;
  strobj _xevdrs_program;
  strobj _xevdrs_root;
  strobj _xevparse;
  strobj _xevparse_code_exp;
  strobj _xevparse_comment;
  strobj _xevparse_conf;
  strobj _xevparse_extend_pragma;
  strobj _xevparse_extract_exp;
  strobj _xevparse_pragma_names;
  strobj _xevparse_string_pragma;
  strobj _xevtgen_conf;
  strobj _xevtgen_varformats;
  strobj _xps;
  strobj spaces[XOS_SPACE_MAX];
} xos;
void init_xos(void);

/*********************************************************************
  key-value pair
*********************************************************************/

typedef struct KVCELL {
  strobj key;
  strobj val;
  struct KVCELL *next;
} kvlist_t;

kvlist_t *new_kvcell(strobj key, strobj val, kvlist_t *next);
strobj lookup_kvlist(kvlist_t *list, strobj key);

#define LOGTYPE_LINE 0
#define LOGTYPE_BLOCK 1

#define LOGFLAG_OPEN 1
#define LOGFLAG_CURSOR 2

typedef struct FARG {
  strobj type;
  struct FINFO *fileinfo;
  struct FARG *next;
} filearg_t;

typedef struct LOGENT {
  int flag;
  int level;
  int type;
  kvlist_t *args;
  filearg_t *files;
  struct LOGENT *init;
  struct LOGENT *super;
  struct LOGENT *inner;
  struct LOGENT *next;
} logent_t;

// structured log information

typedef struct LOGLIST {
  logent_t *log;
  struct LOGLIST *next;
} loglist_t;

typedef struct LOGLEVEL {
  int level;
  loglist_t *logs;
  struct LOGLEVEL *next;
} loglevel_t;

typedef struct FDATA {
  strobj name;
  strobj type;
  int indented;
  struct FINFO *finfo;
  struct FDATA *next;
} filedata_t;

typedef struct FINFO {
  filedata_t *filedata;
  loglevel_t *backward;
  loglevel_t *forward;
  loglevel_t *branches;
  loglevel_t *extracts;
  loglevel_t *subfiles;
} fileinfo_t;

char* get_tmpline(void);
int get_offset_log(void);
int num_lines(void);
int num_columns(void);
void print_log_line(char *line);

int implicit_proc(logent_t*);
logent_t *read_log(FILE *f, logent_t *parent);
void xevlog_file(logent_t*);

void print_log(logent_t *e, int level, int clevel, logent_t *parent);
void print_a_log(strobj pre, logent_t *e);
void print_a_file(fileinfo_t*, int mode, int level, int offset, int show);
strobj modpath(strobj);

void _next_tmp(char*);
void next_bra(char*);
void next_ext(char*);

#endif
