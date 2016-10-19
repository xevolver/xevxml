#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "xevutil.h"
#include "xevparse.h"

#define MOVE_ALL_INLINE_COMMENT 0

#define MODE_CONCAT 0
#define MODE_DISTR  1
#define FORM_FIXED  0
#define FORM_FREE   1
#define TYPE_DIR    1
#define TYPE_FORT   2
#define TYPE_ALL    3

typedef struct CL {
  char *l;
  struct CL *next;
} cl_t;
static cl_t *cls = NULL;
static char *prep_comment = NULL;

static void put_cl(char *line) {
  cl_t *cl = (cl_t*) malloc(sizeof(cl_t));
  assert(cl != NULL);

  cl->l = (char*) malloc(sizeof(char) * (strlen(line) + 1));
  assert(cl->l != NULL);
  strcpy(cl->l, line);

  cl->next = cls;
  cls = cl;
}

static void put_prep(char *line) {
  assert(prep_comment == NULL);

  prep_comment = (char*) malloc(sizeof(char) * (strlen(line) + 1));
  assert(prep_comment != NULL);

  strcpy(prep_comment, line);
}

static void write_cl_do(cl_t *cl, FILE *outfile) {
  if (cl == NULL)
    return;
  write_cl_do(cl->next, outfile);
  fputs(cl->l, outfile);
  free(cl->l);
  free(cl);
}

static void write_cls(FILE *outfile) {
  write_cl_do(cls, outfile);
  cls = NULL;
}

static void write_prep(FILE *outfile) {
  if (prep_comment != NULL) {
    fputs(prep_comment, outfile);
    free(prep_comment);
    prep_comment = NULL;
  }
}

static void concat(int mode, int form, int type, FILE *infile, FILE *outfile) {

  char line00[140*40], line1[140*40];
  int precon = 0, predir = 0, first = 1;  
  char schar = '\0', *qq = line1;
  line1[0] = '\0';
  char *line0 = line00 +1; // to keep quotation mark

  while (1) {
    line0[0] = '\0';
    fgets(line0, 140*40, infile);

    if (feof(infile) && line0[0] == '\0')
      break;

    int comm, cont, dir, nextcon;
    char *p;			/* will be first char of continuation */
    char *q;			/* char pointer to continue */
    if (form == FORM_FIXED) {

      for (p = line0; *p != '\0' && *p <= ' '; p ++);

      if (line0[0] == 'C' || line0[0] == 'c' || line0[0] == '*' ||
	  *p == '!' || *p == '\0')
	comm = 1;
      else
	comm = 0;

      if (line0[1] == '$' && (line0[0] == 'c' || line0[0] == 'C' ||
			      line0[0] == '*' || line0[0] == '!'))
	dir = 1;
      else
	dir = 0;

      int nocon = 0;
      if (line0[0] == '!' && xevparse_pragma_line(line0, NULL)) {
	nocon = 1;
      }

      if (precon != 0 && ((dir && (type & TYPE_DIR)) ||
			  (!comm && (type & TYPE_FORT)))) {
	if (strlen(line0) < 6 || line0[5] == ' ') {
	  fprintf(stderr, "xevutil_continu: unterminated and uncontinued"
		  " string constant in fixed fortran format:\n %s",
		  line0);
	  exit(1);
	}
	line0[5] = schar;
	q = endstrF(line0+5);

	if (q == NULL) {	/* continuation within string constant */
	  q = line0 + (strlen(line0)-1);
	  while (*q == '\n' || *q == '\r')
	    q --;
	  nextcon = 1;

	  q[1] = '\0';
	  while (qq > line1 && (*qq == '\0' || *qq == '\n' ||
				*qq == '\r' || *qq == '!'))
	    qq --;
	  qq ++;
	  strcpy(qq, line0 + 6);
	  qq += q - (line0 + 6);

	  continue;
	} else {
	  cont = 1;  nextcon = 0;
	  q ++;
	  goto fix_q;
	}
      }

      if (!dir && (type & TYPE_FORT) && 
	  strstr(line0, "     ") == line0 && line0[5] > ' ') {
	cont = 1, p = line0 + 6;
      } else if (dir && (type & TYPE_DIR) && line0[5] > ' ' && nocon == 0) {
	cont = 1, p = line0 + 6;
      } else
	cont = 0, p = line0;

      nextcon = 0;
      q = line0;
      if ((dir && (type & TYPE_DIR)) || (!comm && (type & TYPE_FORT))) {
	q = line0 + 6;
      fix_q:
	while (*q != '\0' && *q != '!') {
	  while (*q != '\0' && *q != '!' && *q != '\'' && *q != '"')
	    q ++;
	  if (*q == '\'' || *q == '"') {
	    schar = *q;
	    q = endstrF(q);
	    if (q == NULL) {	/* continuation within string constant */
	      q = line0 + (strlen(line0)-1);
	      while (*q == '\n' || *q == '\r')
		q --;
	      q ++;
	      nextcon = 1;
	      break;
	    } else
	      q ++;
	  }
	}
      }

      if (!cont && ((dir && (type & TYPE_DIR)) ||
		    (!comm && (type & TYPE_FORT)))) {
#if MOVE_ALL_INLINE_COMMENT
	if (*qq == '!') {
	  if (first)
	    put_prep(qq);
	  else
	    put_cl(qq);
	  qq[0] = '\n';  qq[1] = '\0';
	}
#endif
	write_prep(outfile);
	if (qq != line1)
	  fputs(line1, outfile);
	write_cls(outfile);
	qq = line1;
	line1[0] = '\0';
      }

      if ((comm && !dir) || (dir && (type & TYPE_DIR)==0) ||
	  (!comm && (type & TYPE_FORT)==0))
	put_cl(line0);
      else {
	if (cont) {
	  while (qq > line1 && (*qq == '\0' || *qq == '\n' ||
				*qq == '\r' || *qq == '!'))
	    qq --;
	  qq ++;
	  if (*qq == '!') {
	    if (first)
	      put_prep(qq);
	    else
	      put_cl(qq);
	  }
	  strcpy(qq, line0 + 6);
	  qq += q - line0 - 6;
	  first = 0;
	} else {
	  if (*qq == '!') {
	    if (first)
	      put_prep(qq);
	    else
	      put_cl(qq);
	  }
	  strcpy(qq, line0);
	  qq += q - line0;
	  first = 1;
	}

	precon = nextcon;
	if (precon)
	  predir = dir;
      }

    } else /* FORM_FREE */ {

      for (p = line0; *p != '\0' && *p <= ' '; p++);

      if (p[0] == '!' || *p == '\0')
	comm = 1;
      else
	comm = 0;

      if (p[0] == '!' && p[1] == '$')
	dir = 1;
      else
	dir = 0;

      char *comminline = NULL;

      if (precon && schar != '\0' &&
	  ((dir && (type & TYPE_DIR)) || (!comm && (type & TYPE_FORT)))) {
	if ((predir && !dir) || (!predir && dir)) {
	  fprintf(stderr, "xevutil_continu: fortran and directive continuation"
		  " interleave found: unsolved\n");
	  exit(1);
	}
	q = p;
	if (dir) {
	  while (*q != '\0' && *q != '&' && *q > ' ')
	    q ++;
	}
	while (*q != '\0' && *q <= ' ' && *q != '&')
	  q ++;
	if (*q == '&')
	  p = q;
	else if (!dir)
	  p = line0 - 1;
	char pp = *p;
	*p = schar;
	q = endstrF(p);
	*p = pp;
	goto free_q;
      }

      if ((dir && (type & TYPE_DIR)) || (!comm && (type & TYPE_FORT))) {
	q = (dir ? p+2 : p);
	while (*q != '\0' && *q != '!') {
	  while (*q != '\0' && *q != '!' && *q != '\'' && *q != '"')
	    q ++;
	  if (*q == '\'' || *q == '"') {
	    schar = *q;
	    q = endstrF(q);
	  free_q:
	    if (q == NULL) {	/* continuation within string constant */
	      q = line0 + (strlen(line0)-1);
	      while (q > line0 && *q <= ' ')
		q--;
	      if (*q != '&') {
		fprintf(stderr, "xevutil_continu: unterminated and uncontinued"
			" string in fortran free format:\n %s", line0);
		exit(1);
	      } else
		break;
	    } else {
	      schar = '\0';
	      q ++;
	    }
	  }
	}
	if (*q == '!') {
	  comminline = q;
#if MOVE_ALL_INLINE_COMMENT
	  *q = '\0';
#endif
	}
	if (*q != '&')
	  while (q > line0 && (*q == '!' || *q <= ' '))
	    q--;
	if (*q == '&')
	  nextcon = 1;
	else {
	  q = line0 + strlen(line0);
	  while (q > line0 && (*q == '\0' || *q == '\n' || *q == '\r'))
	    q --;
	  nextcon = 0;
	}
      }

      while (*p != '\0' && *p > ' ' && *p != '&')
	p ++;
      while (*p != '\0' && *p <= ' ')
	p ++;

      if (precon && ((dir && (type & TYPE_DIR)) ||
		     (!comm && (type & TYPE_FORT)))) {
	if ((predir && !dir) || (!predir && dir)) {
	  fprintf(stderr, "xevutil_continu: fortran and directive continuation"
		  " interleave found: unsolved\n");
	  exit(1);
	}
	cont = 1;
	if (dir) {
	  if (*p == '&')
	    p ++;
	  else 
	    p --;
	} else {
	  if (*p == '&')
	    p ++;
	  else
	    p = line0;
	}
      } else {
	p = line0;
	cont = 0;
      }

      if (!cont && ((dir && (type & TYPE_DIR)) ||
		    (!comm && (type & TYPE_FORT)))) {
#if MOVE_ALL_INLINE_COMMENT
	if (*qq == '!') {
	  if (!cont)
	    put_prep(qq);
	  else
	    put_cl(qq);
	  qq[0] = '\n';  qq[1] = '\0';
	}
#endif
	write_prep(outfile);
	if (qq != line1)
	  fputs(line1, outfile);
	write_cls(outfile);
	qq = line1;
	line1[0] = '\0';
      }

      if ((comm && !dir) || (dir && (type & TYPE_DIR) == 0) ||
	  (!comm && (type & TYPE_FORT) == 0)) {
	put_cl(line0);
      } else {
	if (nextcon) {
	  *q = '\0';
	  if (comminline != NULL) {
#if MOVE_ALL_INLINE_COMMENT
	    assert(*comminline == '\0');
	    *comminline = '!';
#endif
	    if (!cont)
	      put_prep(comminline);
	    else
	      put_cl(comminline);
#if MOVE_ALL_INLINE_COMMENT
	    *comminline = '\0';
#endif
	  }
	  comminline = NULL;
	}
	strcpy(qq, p);
	qq += strlen(p);

	precon = nextcon;
	if (precon && ((dir && (type & TYPE_DIR)) ||
		       (!comm && (type & TYPE_FORT))))
	  predir = dir;
      }
      first = !cont;
    }
  }

  if (form == FORM_FIXED) {
#if MOVE_ALL_INLINE_COMMENT
    if (*qq == '!') {
      if (first)
	put_prep(qq);
      else
	put_cl(qq);
      qq[0] = '\n';  qq[1] = '\0';
    }
#endif
    write_prep(outfile);
    if (qq != line1)
      fputs(line1, outfile);
    write_cls(outfile);
  } else {
#if MOVE_ALL_INLINE_COMMENT
    if (*qq == '!') {
      if (first)
	put_prep(qq);
      else
	put_cl(qq);
      qq[0] = '\n';  qq[1] = '\0';
    }
#endif
    write_prep(outfile);
    if (qq != line1)
      fputs(line1, outfile);
    write_cls(outfile);
  }
}

void distribute(int mode, int form, int type, FILE *infile, FILE *outfile) {

  char line0[140*40];

  while (1) {
    line0[0] = '\0';
    fgets(line0, 140*40, infile);

    if (feof(infile) && line0[0] == '\0')
      break;

    int comm, dir;

    if (form == FORM_FIXED) {

      char *p;
      for (p = line0; *p != '\0' && *p <= ' '; p ++);

      if (line0[0] == 'C' || line0[0] == 'c' || line0[0] == '*' ||
	  *p == '!' || *p == '\0')
	comm = 1;
      else
	comm = 0;

      if (line0[1] == '$' && (line0[0] == 'c' || line0[0] == 'C' ||
			      line0[0] == '*' || line0[0] == '!'))
	dir = 1;
      else
	dir = 0;

      char dirname[6];  int i;
      for (i = 0; i< 5; i++)
	dirname[i] = line0[i];
      dirname[5] = '\0';

      if ((dir && (type & TYPE_DIR)) || (!comm && (type & TYPE_FORT))) {
	char *q = line0 + 6;
	while (*q != '\0' && *q != '!') {
	  while (*q != '\0' && *q != '!' && *q != '\'' && *q != '"')
	    q ++;
	  if (*q == '\'' || *q == '"') {
	    q = endstrF(q);
	    if (q == NULL) {
	      fprintf(stderr, "xevutil_continu: unfinished string\n");
	      exit(1);
	    } else
	      q ++;
	  }
	}
	char *eol = q;

	while (eol > line0 && (*eol == '\0' || *eol == '\n' || *eol == '\r'))
	  eol --;
	eol ++;
	if (eol - line0 > 72) {
	  char *p = line0;
	  char q = p[72];
	  p[72] = '\0';
	  fprintf(outfile, "%s\n", p);
	  p[72] = q;
	  p += 72;
	  while (eol - p > 66) {
	    q = p[66];
	    p[66] = '\0';
	    fprintf(outfile, "%s&%s\n", dirname, p);
	    p[66] = q;
	    p += 66;
	  }
	  fprintf(outfile ,"%s&%s", dirname, p);
	} else
	  fputs(line0, outfile);
      } else
	fputs(line0, outfile);

    } else {			/* FORM_FREE */
    
      char *p;
      for (p = line0; *p != '\0' && *p <= ' '; p++);

      if (p[0] == '!' || *p == '\0')
	comm = 1;
      else
	comm = 0;

      if (p[0] == '!' && p[1] == '$')
	dir = 1;
      else
	dir = 0;

      char dirname[100];  int i;
      for (i=0; p[i] > ' '; i++)
	dirname[i] = p[i];
      dirname[i] = '\0';
      int dirlen = i;

      if ((dir && (type & TYPE_DIR)) || (!comm && (type & TYPE_FORT))) {
	char *q = (dir ? p+2 : p);
	while (*q != '\0' && *q != '!') {
	  while (*q != '\0' && *q != '!' && *q != '\'' && *q != '"')
	    q ++;
	  if (*q == '\'' || *q == '"') {
	    q = endstrF(q);
	    if (q == NULL) {	/* continuation within string constant */
	      fprintf(stderr, "xevtuil_continu: unfinished string\n");
	      exit(1);
	    } else {
	      q ++;
	    }
	  }
	}

	char *eol = q;
	while (eol > line0 && (*eol == '\0' || *eol == '\n' || *eol == '\r'))
	  eol --;
	eol ++;
	if (eol - line0 > 132) {
	  char *p = line0;
	  char q = p[131];
	  p[131] = '\0';
	  fprintf(outfile, "%s&\n", p);
	  p[131] = q;
	  p += 131;
	  while (eol - p > 130 - dirlen) {
	    q = p[129-dirlen];  p[129-dirlen] = '\0';
	    fprintf(outfile, "%s &%s&\n", dirname, p);
	    p[129-dirlen] = q;  p += 129-dirlen;
	  }
	  fprintf(outfile, "%s &%s", dirname, p);
	} else
	  fputs(line0, outfile);
      } else
	fputs(line0, outfile);
    }
  }
}

int main(int argc, char **argv) {     
  if (argc != 4) {
  usage:
    fprintf(stderr, "usage: %s mode infile outfile\n", argv[0]);
    fprintf(stderr, "mode first letter: c=concat, d=distribute\n");
    fprintf(stderr, "mode second letter: 0=fixed, 1=free\n");
    fprintf(stderr, "mode third letters: a=all, d=directive, f=fortran\n");
    exit(1);
  }

  init_xos();
  xevparse_load_conf();

  int mode, form, type;
  if (argv[1][0] == 'c')
    mode = MODE_CONCAT;
  else if (argv[1][0] == 'd')
    mode = MODE_DISTR;
  else
    goto usage;

  if (argv[1][1] == '0')
    form = FORM_FIXED;
  else if (argv[1][1] == '1')
    form = FORM_FREE;
  else
    goto usage;

  if (argv[1][2] == 'a')
    type = TYPE_ALL;
  else if (argv[1][2] == 'd')
    type = TYPE_DIR;
  else if (argv[1][2] == 'f')
    type = TYPE_FORT;
  else
    goto usage;

  FILE *infile = fopen(argv[2], "r");
  if (infile == NULL) {
    fprintf(stderr, "xevutil_continu: cannot open %s\n", argv[2]);
    exit(1);
  }

  FILE *outfile = fopen(argv[3], "w");
  if (outfile == NULL) {
    fprintf(stderr, "xevutil_continu: cannot open %s\n", argv[3]);
    exit(1);
  }

  if (mode == MODE_CONCAT)
    concat(mode, form, type, infile, outfile);
  else
    distribute(mode, form, type, infile, outfile);

  fclose(outfile);

  return 0;
}
