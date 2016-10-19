#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

static void extract_exp(char *line, FILE *fd) {

  char *p = strchrF(line, '`');

  while (p != NULL) {
    p ++;

    char *q = strchrF(p, '`');
    if (q == NULL) {
      fprintf(stderr, "unknown line: %s", line);
      exit(1);
    }

    int l = q - p;
    char e[l + 1];

    int i;
    for (i=0; i< l; i++)
      e[i] = p[i];
    e[l] = '\0';

    fprintf(fd, "       call xevparse_extract_exp(%s)\n", e);

    p = strchrF(q + 1, '`');
  }
}

#ifdef XFORMART_1_0_0
static char *work_around(char *t, char *l) {

  char *tt = t;
  while (*tt != '\0' && *tt <= ' ')
    tt ++;

  if (strstr(tt, "!$") != tt) {

    char *p = strchrF(t, '!');

    if (p != NULL) {

      char *q;
      /* remove new line characters */
      for (q = p + strlen(p); q > p && *q < ' '; q --)
	*q = '\0';

      /* check comment-only line */
      for (q = t; q < p && *q == ' '; q ++);

      if (q == p) {

	char *s1 = strG2F(p, '\'');
	sprintf(l, "!$xevparse comment(%s)\n", s1);
	free(s1);

	return NULL;
      } else {
	if (p[-1] <= ' ') {
	  p --;
	  while (*p <= ' ')
	    p --;
	  p ++;
	}

	char pp = *p;
	*p = '\0';
	strcpy(l, t);
	*p = pp;
	return p;
      }
    }
  }

  return NULL;
}
#endif

#if 0
void insert_dummy(char *line, FILE *fd) {

  while (*line <= ' ' && *line != '\0')
    line ++;

  //  if (strstr(line, "contains") == line && line[8] <= ' ')
  //    fprintf(fd, "  call xevparse_dummy\n");
}
#endif

void preparse_f(char *srcfile, char *dstfile) {
  char line[XEV_LINE_SIZE];

  char *x = strchr(srcfile, '.');
  int fixed = 0;
  if (strlen(x) < 3)
    fixed = 1;

  FILE *fs = fopen(srcfile, "r");
  if (fs == NULL) {
    fprintf(stderr, "cannot open %s\n", srcfile);
    exit(1);
  }

  FILE *fd = fopen(dstfile, "w");
  if (fd == NULL) {
    fprintf(stderr, "cannot open %s\n", dstfile);
    exit(1);
  }

#if 1
  char *p = strrchr(srcfile, '/');
  if (p == NULL)
    p = srcfile;
  else
    p ++;

#if 0
  fprintf(fd, "!$xevparse info source('%s') version('%s') begin\n", p, XT_FORMATSTR);
#else
  fprintf(fd, "!$xev xevparse info source('%s') version('%s') begin\n", p, XT_FORMATSTR);
#endif
  // end is inserted at xevparse_postf.c
#endif

  while (1) {
    fgets(line, XEV_LINE_SIZE, fs);

    if (feof(fs))
      break;

    //    insert_dummy(line, fd);

#ifdef XFORMAT_1_0_0
    char *t = (char*) malloc(sizeof(char) * (strlen(line) + 1));
    assert(t != NULL);
    strcpy(t, line);

    char *l = work_around(t, line);
#endif

    char *p;
    for (p = line; *p != '\0' && *p <= ' '; p++);
    if (*p == '!') {
      strobj rename;
      int type = xevparse_pragma_line(p, &rename);
      if (type == 5 || type == 6) {
	int i;
	for (i = strlen(line); i >= 0 && line[i] <= ' '; i--)
	  line[i + 23] = line[i];
	line[i + 23] = ')';
	line[i + 22] = '"';
	for (; i >= 0; i--)
	  line[i + 21] = line[i];
	char c = line[0];
	strcpy(line, "!$xev string_pragma(\"");
	line[21] = c;
      } else if (type == 9 || type == 10) {
	assert(rename != NULL);
	int n = 6 + strlen(rename);
	char *pp = p;
	while (*pp != '\0' && *pp > ' ')
	  pp ++;
	int m = pp - p;
	if (m < n) {
	  char *q;
	  for (q = pp + strlen(pp); q > pp; q--)
	    q[n-m] = q[0];
	} else {
	  for (; m > n; m--)
	    *(p++) = ' ';
	}
	sprintf(p, "!$xev %s", rename);
	p[strlen(p)] = ' ';
      }
    }

    fputs(line, fd);

    for (p = line; *p != '\0' && *p <= ' '; p++);
    if (*p == '!' && xevparse_pragma_line(p, NULL))
	extract_exp(line, fd);

#ifdef XFORMAT_1_0_0
    if (l != NULL)
      fprintf(fd, "\n!$xevparse ecomment(%s)\n", strG2F(l, '\''));
    free(t);
#endif

  }

  fprintf(fd, "\n");
  fprintf(fd, "       subroutine xevparse_dummy\n");
  fprintf(fd, "       end subroutine xevparse_dummy\n");

  fclose(fs);
  fclose(fd);
}
