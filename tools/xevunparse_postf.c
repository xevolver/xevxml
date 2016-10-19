#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

strobj get_pragma_name(char *line) {
#define INIT_I 8

  int i = INIT_I;
  if (line[i] <= ' ')
    return NULL;

  int j = INIT_I + 1;
  while (line[j] > ' ')
    j ++;

  char s[j-i+3];
  s[0] = '!';
  s[1] = '$';
  while (i < j) {
    s[i - INIT_I +2] = line[i];
    i ++;
  }
  s[i - INIT_I +2] = '\0';

  return get_strobj(s);
}

void postunparse_f(char *srcfile, char *dstfile) {
  char line_body[XEV_LINE_SIZE];

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

  int unbreak = 0;

  while (1) {
    char *line = line_body;
    fgets(line, XEV_LINE_SIZE, fs);

    if (feof(fs))
      break;

    while (*line != '\0' && *line <= ' ')
      line ++;

    if (strstr(line, "!pragma xevparse comment(") == line) {

      char *p = line+25;
      char *s1 = strF2G(p);

      if (unbreak) {
	fprintf(fd, "\n");
	unbreak = 0;
      }

      fprintf(fd, "%s\n", s1);
      free(s1);

    } else if (strstr(line, "!pragma xevparse ecomment(") == line) {

      char *p = line+26;
      char *s1 = strF2G(p);

      fprintf(fd, "%s\n", s1);
      free(s1);

      unbreak = 0;

    } else if (strstr(line, "!pragma xev string_pragma") == line) {

      if (unbreak) {
	fprintf(fd, "\n");
	unbreak = 0;
      }

      char *p = line + strlen(line);
      while (p > line && *p != '\'')
	p --;
      assert(*p == '\'');
      *p = '\0';

      fprintf(fd, "%s\n", line + 27);

    } else if (strstr(line, "!pragma ") == line) {

      if (unbreak) {
	fprintf(fd, "\n");
	unbreak = 0;
      }

      if (strstr(line, "!pragma xev ") == line) {
	int clen;
	strobj orgname = xevparse_pragma_rename(line+12, &clen);
	if (orgname != NULL) {
	  fprintf(fd, "%s%s", orgname, line+12+clen);
	  continue;
	}
      }

      strobj pragma_name = get_pragma_name(line);
      if (!xevparse_matching_pragma(pragma_name)) {
	fputs(line, fd);
	continue;
      }

      line[6] = '!';
      line[7] = '$';

      char s0[strlen(line+6)+1];
      strcpy(s0, line+6);
      char *s = s0;
      while (1) {
	if (*s == '\0')
	  break;

	if (*s == '"' || *s == '\'') {
	  char *t = endstrF(s);
	  while (s <= t)
	    fputc(*(s++), fd);
	  s = t + 1;
	} else if (*s == '`') {
	  s ++;
	  while (*s != '`' && *s != '\0')
	    s ++;
	  assert(*s != '\0');
	  s ++;

	  line = line_body;
	  fgets(line, XEV_LINE_SIZE, fs);
	  while (*line != '\0' && *line <= ' ')
	    line ++;
	  assert(strstr(line, "CALL xevparse_extract_exp") == line);

	  char *t = line + strlen(line);
	  while (*t != ')' && t > line)
	    t --;
	  char *q = strchr(line, '(') + 1;
	  fputc('`', fd);
	  while (q < t)
	    fputc(*(q++), fd);
	  fputc('`', fd);
	} else {
	  fputc(*s, fd);
	  s ++;
	}
      }

    } else {

      if (unbreak) {
	fprintf(fd, "\n");
	unbreak = 0;
      }

      line = line_body;
      int e = strlen(line);
      while (e > 0 && line[e] < ' ')
	e --;
      if (e > 0) {
	line[e+1] = '\0';
	fputs(line, fd);
      }
      unbreak = 1;
    }
  }

  if (unbreak) {
    fprintf(fd, "\n");
    unbreak = 0;
  }

  fclose(fs);
  fclose(fd);
}
