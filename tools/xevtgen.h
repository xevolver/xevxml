#ifndef __XEVTGEN_H_INCLUDED__
#define __XEVTGEN_H_INCLUDED__

void xevtgen_init(void);
void xevtgen_count_variables(xmlcell_t*, FILE*);
void xevtgen_write_context_param(FILE*);
void xevtgen_write_varparams(FILE*);
void xevtgen_write_template(xmlcell_t*, FILE*);
void xevtgen_write_otherwise(FILE*);
strobj xevtgen_mode(void);

#endif
