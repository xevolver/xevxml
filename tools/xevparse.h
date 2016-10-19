#ifndef __XEVPARSE_H_INCLUDED__
#define __XEVPARSE_H_INCLUDED__

/* for xevparse_util */

void xevparse_load_conf(void);
int xevparse_matching_pragma(strobj);
int xevparse_pragma_line(char*, strobj*);
strobj xevparse_pragma_rename(char*, int*);
//int xevparse_parse_comment(void);

/* for xevparse */

void preparse_f(char*, char*);
void postparse_f(xmlcell_t *, char*);

xmlcell_t *get_pragma(xmlcell_t*, int depth0, char* parse_code(char*, xmlcell_t**));
xmlcell_t *xev_parse_pragma(char*, int depth0, char* parse_code(char*, xmlcell_t**));
char *xev_unparse_pragma(xmlcell_t*, int (*get_num_ex)(xmlcell_t*));

/* for xevunparse */

void unparse_f(xmlcell_t*, char*);
void postunparse_f(char*, char*);

/* for xevrestruct */

void xev_constr_block(xmlcell_t*);
void xev_destr_block(xmlcell_t*);
void xevparse_contplus(xmlcell_t*);
void xevparse_contminus(xmlcell_t*);
void xev_constr_line(xmlcell_t*);
void xev_destr_line(xmlcell_t*);
void xev_concat_cont(xmlcell_t*);

/** error treatment **/

void xevparse_setfileinfo(char*);
void xevparse_setlineinfo(char*, int);
void xevparse_putlineinfo(FILE*);

#endif
