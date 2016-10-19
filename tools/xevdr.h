#ifndef __XEVDR_H_INCLUDED__
#define __XEVDR_H_INCLUDED__

typedef struct DRSCOPE *xevdr_scope_t;

#define TYPE_BREAK 1
#define TYPE_RETURN 2

typedef struct {
  int private;
} xevdr_opt_t;

void xevdr_get_opt(xmlcell_t*, xevdr_opt_t*);

void init_root_scopes(void);
xevdr_scope_t xevdrs_root(void);
xevdr_scope_t xevpod_root(void);
int xevdr_scope_index(xevdr_scope_t);
xevdr_scope_t xevdr_lookup_scope(int);
int xevdr_scope_count(xevdr_scope_t);
void xevdr_update_scope_index(xevdr_scope_t);
// get root of a scope
xevdr_scope_t xevdr_get_root(xevdr_scope_t);
xmlcell_t *xevdr_lookup_var(strobj, xevdr_scope_t, int, xevdr_scope_t*);
xmlcell_t *xevdr_lookup_var_sc(strobj, xevdr_scope_t, int, xevdr_scope_t*);
xmlcell_t *xevdr_lookup_public_var_sc(strobj, xevdr_scope_t, int, xevdr_scope_t*);
xmlcell_t *xevdr_probe_var(strobj, xevdr_scope_t, int, xevdr_scope_t*);
xmlcell_t *xevdr_probe_var_sc(strobj, xevdr_scope_t, int, xevdr_scope_t*);
xmlcell_t *xevdr_probe_public_var_sc(strobj, xevdr_scope_t, int, xevdr_scope_t*);
void xevdr_vardefC(strobj, xmlcell_t*, xevdr_scope_t);
void xevdr_vardef_privateC(strobj, xmlcell_t*, xevdr_scope_t);
void xevdr_varsetC(strobj, xmlcell_t*, xevdr_scope_t);
xevdr_scope_t xevdr_new_scope(strobj, xevdr_scope_t, int);
xevdr_scope_t xevdr_del_scope(xevdr_scope_t);
void xevdr_exit_scope(xevdr_scope_t);
void xevdr_dump_scope(xevdr_scope_t);
void xevdr_load_xpg(char*, xevdr_scope_t);

void xevdr_exec_lines(xmlcell_t*, xevdr_scope_t);
void xevdr_exec_aline(xmlcell_t*, xevdr_scope_t);
void xevdr_vardef(xmlcell_t*, xevdr_scope_t);
void xevdr_vardef_private(xmlcell_t*, xevdr_scope_t);
void xevdr_set(xmlcell_t*, xevdr_scope_t);
void xevdr_sethead(xmlcell_t*, xevdr_scope_t);
void xevdr_settail(xmlcell_t*, xevdr_scope_t);
void xevdr_if(xmlcell_t*, xevdr_scope_t);
void xevdr_while(xmlcell_t*, xevdr_scope_t);
void xevdr_funcdef(xmlcell_t*, xevdr_scope_t);
void xevdr_procdef(xmlcell_t*, xevdr_scope_t);
void xevdr_module_def(xmlcell_t*, xevdr_scope_t);
void xevdr_open_module(xmlcell_t*, xevdr_scope_t);
void xevdr_use_module(xmlcell_t*, xevdr_scope_t);
void xevdr_module_ref(xmlcell_t*, xevdr_scope_t);
void xevdr_exit(xmlcell_t*, xevdr_scope_t);
void xevdr_load(xmlcell_t*, xevdr_scope_t);
void xevdr_load_root(xmlcell_t*, xevdr_scope_t);
void xevdr_loadC(char*, xevdr_scope_t);
void xevdr_include(xmlcell_t*, xevdr_scope_t);
void xevdr_print(xmlcell_t*, xevdr_scope_t);
void xevdr_printC(xmlcell_t*);
void xevdr_write(xmlcell_t*, xevdr_scope_t);
void xevdr_drvget(xmlcell_t*, xevdr_scope_t);
void xevdr_drvput(xmlcell_t*, xevdr_scope_t);
void xevdr_exec_command(xmlcell_t*, xevdr_scope_t);
void xevdr_xslt(xmlcell_t*, xevdr_scope_t);
void xevdr_dump_scope(xevdr_scope_t);
void xevdr_dump_scopeC(strobj, xevdr_scope_t);
void xevdr_proc_exec(xmlcell_t*, xevdr_scope_t);
void xevdr_write_xml(xmlcell_t*, xevdr_scope_t);
void xevdr_log(xmlcell_t*x, xevdr_scope_t);
void xevdr_set_fst(xmlcell_t*, xevdr_scope_t);
void xevdr_set_snd(xmlcell_t*, xevdr_scope_t);

xmlcell_t *xevdr_eval(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_func_exec(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_func_list(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_read_xml(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_read(xmlcell_t*, xevdr_scope_t);
#if 0
xmlcell_t *xevdr_tmpfile(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_untmpfile(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_is_tmpfile(xmlcell_t*, xevdr_scope_t);
#else
xmlcell_t *xevdr_is_tmpfile(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_next_tmp(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_next_bra(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_next_ext(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_remove_tmp_suffix(xmlcell_t*, xevdr_scope_t);
#endif
xmlcell_t *xevdr_change_ext(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_searchfile(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_file_mtime(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_file_exist(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_obj_null(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_obj_head(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_obj_tail(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_obj_cons(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_eval_pair(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_make_pair(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_pair_fst(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_pair_snd(xmlcell_t*, xevdr_scope_t);
xmlcell_t *xevdr_is_pair(xmlcell_t*, xevdr_scope_t);

strobj xevdr_get_loading_file(void);
strobj xevdr_get_loading_dir(void);

#endif
