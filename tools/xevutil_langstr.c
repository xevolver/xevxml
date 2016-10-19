#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "xevutil.h"

/* fortran

   character starts from either " or '
   starting ", " can be inserted by ""
   starting ', ' can be inserted by ''

 */

char *strF2G(char *s) {
  assert(s != NULL);

  int l = 1, c = 0;

  if (s[0] != '"' && s[0] != '\'') {
    fprintf(stderr, "strF2G: Fortran string must start with \" or '\n");
    exit(1);
  }

  while (1) {
    if (s[l] == s[0] && s[l+1] == s[0])
      l += 2, c ++;
    else if (s[l] == s[0])
      break;
    else
      l ++;
  }

  /* s[l] end of string, c: number of "" or '' */

  int len = l - c;
  char *t = (char*) malloc(sizeof(char) * len);
  assert(t != NULL);

  l = 1, c = 0;
  while (1) {
    if (s[l] == s[0] && s[l+1] == s[0])
      l += 2, t[c++] = s[0];
    else if (s[l] == s[0]) {
      t[c] = '\0';
      break;
    } else
      t[c++] = s[l++];
  }

  assert(strlen(t) == len - 1);  
  return t;
}

char *strG2F(char *s, char initc) {
  assert(s != NULL);

  int l, c;

  l = 0, c = 0;
  while (1) {
    if (s[l] == initc)
      c ++, l ++;
    else if (s[l] == '\0')
      break;
    else
      l ++;
  }

  int len = l + 3 + c;
  char *t = (char*) malloc(sizeof(char) * len);
  assert(t != NULL);

  l = c = 0;
  t[c++] = initc;
  while (1) {
    if (s[l] == initc) {
      t[c++] = initc;
      t[c++] = initc;
      l ++;
    } else if (s[l] == '\0') {
      t[c++] = initc;
      t[c++] = '\0';
      break;
    } else
      t[c++] = s[l++];
  }

  assert(strlen(t) == len - 1);
  return t;
}

char *endstrF(char *s) {
  assert(s != NULL);

  char c = s[0];
  char *t = s + 1;

  while (*t != '\0') {
    if (t[0] == c && t[1] == c)
      t ++;
    else if (t[0] == c)
      return t;

    t ++;
  }

  return NULL;
}

char *strchrF(char *s, char c) {
  assert(s != NULL);

  while (*s != '\0' && *s != c) {

    if (*s == '\'' || *s == '"') {
      s = endstrF(s);
      if (s == NULL)
	return NULL;
    }

    s ++;
  }

  if (*s == '\0')
    return NULL;
  else
    return s;
}

/* C language

   string starts with "
   backslash introduces special characters
   \o \oo \ooo \xh \xhh

 */

char *strC2G(char *s) {
  assert(s != NULL);
  int l, c;

  if (s[0] != '"') {
    fprintf(stderr, "strC2G: C string must start with \"\n");
    exit(1);
  }

  l = 1, c = 0;
  while (1) {
    if (s[l] == '\\') {
      if (s[l+1] == 'x') {	/* hexadecimal */
	if (!isxdigit(s[l+2]))
	  fprintf(stderr, "Warning: error in C string hexadecimal?\n");
	if (isxdigit(s[l+3]))	/* \xhh */
	  c += 3, l += 4;
	else			/* \xh */
	  c += 2, l += 3;
      } else if ('0' <= s[l+1] && s[l+1] <= '7') { /* octal */
	if ('0' <= s[l+2] && s[l+2] <= '7') {
	  if ('0' <= s[l+3] && s[l+3] <= '7') /* \ooo */
	    c += 3, l += 4;
	  else			/* \oo */
	    c += 2, l += 3;
	} else			/* \o */
	  c += 1, l += 2;
      } else			/* other \z */
	c ++, l += 2;
    } else if (s[l] == '"')
      break;
    else
      l ++;
  }

  int len = l - c;
  char *t = (char*) malloc(sizeof(char) * len);
  assert(t != NULL);

  l = 1, c = 0;
  while (1) {
    if (s[l] == '\\') {
      if (s[l+1] == 'x') {	/* hexadecimal */
	int v;
	if (!isxdigit(s[l+2]))
	  fprintf(stderr, "Warning: error in C string hexadecimal?\n");
	if ('0' <= s[l+2] && s[l+2] <= '9')
	  v = s[l+2] - '0';
	else if ('a' <= s[l+2] && s[l+2] <= 'z')
	  v = s[l+2] - 'a';
	else
	  v = s[l+2] - 'A';
	if (isxdigit(s[l+3])) {	/* \xhh */
	  if ('0' <= s[l+3] && s[l+3] <= '9')
	    v = v * 16 + s[l+3] - '0';
	  else if ('a' <= s[l+3] && s[l+3] <= 'z')
	    v = v * 16 + s[l+3] - 'a';
	  else
	    v = v * 16 + s[l+3] - 'A';
	  t[c++] = v, l += 4;
	} else			/* \xh */
	  t[c++] = v, l += 3;
      } else if ('0' <= s[l+1] && s[l+1] <= '7') { /* octal */
	int v = s[l+1] - '0';
	if ('0' <= s[l+2] && s[l+2] <= '7') {
	  v = v * 8 + s[l+2] - '0';
	  if ('0' <= s[l+3] && s[l+3] <= '7') { /* \ooo */
	    v = v * 8 + s[l+2] - '0';
	    t[c++] = v, l += 4;
	  } else			/* \oo */
	    t[c++] = v, l += 3;
	} else			/* \o */
	  t[c++] = v, l += 2;
      } else {			/* other \z */
	if (s[l+1] == 'a')
	  t[c++] = '\a';
	else if (s[l+1] == 'b')
	  t[c++] = '\b';
	else if (s[l+1] == 'f')
	  t[c++] = '\f';
	else if (s[l+1] == 'n')
	  t[c++] = '\n';
	else if (s[l+1] == 'r')
	  t[c++] = '\r';
	else if (s[l+1] == 't')
	  t[c++] = '\t';
	else if (s[l+1] == 'v')
	  t[c++] = '\v';
	else if (s[l+1] == '\\')
	  t[c++] = '\\';
	else if (s[l+1] == '?')
	  t[c++] = '\?';
	else if (s[l+1] == '\'')
	  t[c++] = '\'';
	else if (s[l+1] == '\"')
	  t[c++] = '\"';
	else
	  t[c++] = s[l+1];
	l += 2;
      }
    } else if (s[l] == '"') {
      t[c++] = '\0';
      break;
    } else
      t[c++] = s[l++];
  }

  assert(strlen(t) == len - 1);
  return t;
}

char *strG2C(char *s) {
  assert(s != NULL);
  int l, c;

  l = c = 0;
  while (1) {
    if (s[l] == '\0')
      break;
    else if (s[l] == '"')
      c += 1;
    else if (s[l] < '\x20' || s[l] >= '\x7f')
      c += 3;
    l ++;
  }

  int len = l + 3 + c;
  char *t = (char*) malloc(sizeof(char) * len);
  assert(t != NULL);

  l = c = 0;
  t[c++] = '"';
  while (1) {
    if (s[l] == '\0') {
      t[c++] = '"';
      t[c++] = '\0';
      break;
    } else if (s[l] == '"') {
      t[c++] = '\\';
      t[c++] = '"';
    } else if (s[l] < '\x20' || s[l] >= '\x7f') {
      t[c++] = '\\';
      t[c++] = 'x';
      sprintf(t+c, "%02x", s[l]);
      c += 2;
    } else
      t[c++] = s[l];
    l ++;
  }

  assert(strlen(t) == len - 1);
  return t;
}

/* XML string

   string can start either by " or '
   entity reference:
   < &lt;
   > &gt;
   & &amp;
   " &quot;
   ' &apos;

 */

char* strX2G(char *s) {
  assert(s != NULL);
  int l, c;

  if (s[0] != '"' && s[0] != '\'') {
    fprintf(stderr, "strX2G: XML string must start with \" or '\n");
    exit(1);
  }

  l = 1, c = 0;
  while (1) {
    if (s[l] == '&') {
      if (strstr(&s[l], "&lt;") == &s[l])
	c += 3, l += 4;
      else if (strstr(&s[l], "&gt;") == &s[l])
	c += 3, l += 4;
      else if (strstr(&s[l], "&amp;") == &s[l])
	c += 4, l += 5;
      else if (strstr(&s[l], "&quot;") == &s[l])
	c += 5, l += 6;
      else if (strstr(&s[l], "&apos;") == &s[l])
	c += 5, l += 6;
      else
	c ++, l ++;
    } else if (s[l] == s[0])
      break;
    else
      l ++;
  }

  int len = l - c;
  char *t = (char*) malloc(sizeof(char) * len);
  assert(t != NULL);

  l = 1, c = 0;
  while (1) {
    if (s[l] == '&') {
      if (strstr(&s[l], "&lt;") == &s[l])
	t[c++] = '<', l += 4;
      else if (strstr(&s[l], "&gt;") == &s[l])
	t[c++] = '>', l += 4;
      else if (strstr(&s[l], "&amp;") == &s[l])
	t[c++] = '&', l += 5;
      else if (strstr(&s[l], "&quot;") == &s[l])
	t[c++] = '"', l += 6;
      else if (strstr(&s[l], "&apos;") == &s[l])
	t[c++] = '\'', l += 6;
      else
	t[c++] = '&', l ++;
    } else if (s[l] == s[0]) {
      t[c++] = '\0';
      break;
    } else
      t[c++] = s[l++];
  }

  assert(strlen(t) == len - 1);
  return t;
}

char* strG2X(char *s, char initc) {
  assert(s != NULL);
  int l, c;

  l = c = 0;
  while (1) {
    if (s[l] == '<')
      c += 3;
    else if (s[l] == '>')
      c += 3;
    else if (s[l] == '&')
      c += 4;
    else if (s[l] == '"')
      c += 5;
    else if (s[l] == '\'')
      c += 5;
    else if (s[l] == '\0')
      break;
    l ++;
  }

  int len = l + 2 + 1 + c;
  char *t = (char*) malloc(sizeof(char) * len);
  assert(t != NULL);

  l = c = 0;
  t[c++] = initc;
  while (1) {
    if (s[l] == '<') {
      strcpy(&t[c], "&lt;");
      c += 4;
    } else if (s[l] == '>') {
      strcpy(&t[c], "&gt;");
      c += 4;
    } else if (s[l] == '&') {
      strcpy(&t[c], "&amp;");
      c += 5;
    } else if (s[l] == '"') {
      strcpy(&t[c], "&quot;");
      c += 6;
    } else if (s[l] == '\'') {
      strcpy(&t[c], "&apos;");
      c += 6;
    } else if (s[l] == '\0') {
      t[c++] = initc;
      t[c++] = '\0';
      break;
    } else
      t[c++] = s[l];
    l ++;
  }

  assert(strlen(t) == len - 1);
  return t;
}

#if 0
int main(void) {

  init_xos();

  printf("testing string conversions...\n");

  char *s = "cx<'&\"\x1\x02\3\04\005";

  char *s1 = strG2C(s);
  printf("C: %s\n", s1);
  char *s2 = strC2G(s1);
  char *s3 = strG2C(s2);

  if (strcmp(s, s2) != 0)
    printf("G2C + C2G error\n");
  if (strcmp(s1, s3) != 0)
    printf("C2G + G2C error\n");

  s = "cx<'&\"";
  s1 = strG2F(s, '"');
  printf("F: %s\n", s1);
  s2 = strF2G(s1);
  s3 = strG2F(s2, '"');

  if (strcmp(s, s2) != 0)
    printf("G2F + F2G error\n");
  if (strcmp(s1, s3) != 0)
    printf("F2G + G2F error\n");

  s = "cx<'&\"";
  s1 = strG2F(s, '\'');
  printf("F: %s\n", s1);
  s2 = strF2G(s1);
  s3 = strG2F(s2, '\'');

  if (strcmp(s, s2) != 0)
    printf("G2F + F2G error\n");
  if (strcmp(s1, s3) != 0)
    printf("F2G + G2F error\n");

  s = "cx<'&\">";
  s1 = strG2X(s, '"');
  printf("X: %s\n", s1);
  s2 = strX2G(s1);
  s3 = strG2X(s2, '"');

  if (strcmp(s, s2) != 0)
    printf("G2X + X2G error\n");
  if (strcmp(s1, s3) != 0)
    printf("X2G + G2X error\n");

  s = "cx<'&\">";
  s1 = strG2X(s, '\'');
  printf("X: %s\n", s1);
  s2 = strX2G(s1);
  s3 = strG2X(s2, '\'');

  if (strcmp(s, s2) != 0)
    printf("G2X + X2G error\n");
  if (strcmp(s1, s3) != 0)
    printf("X2G + G2X error\n");

  return 0;
}
#endif
