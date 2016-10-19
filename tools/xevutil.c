#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include "xevutil.h"

char *xevxml_dir(void) {
  return NULL;
}

char *xevtools_dir(void) {
  char *s = getenv("XEVTOOLSDIR");
  if (s == NULL) {
    fprintf(stderr, "ERROR: environment XEVTOOLSDIR is not set\n");
    exit(1);
  }
  if (s[strlen(s)-1] != '/') {
    char t[strlen(s)+2];
    strcpy(t, s);
    strcat(t, "/");
    return dupstr(t);
  } else
    return dupstr(s);
}

char *tmp_dir(void) {
  static char t[XEV_PATH_SIZE];

  FILE *f = fopen("/tmp/xevtools", "r");
  if (f == NULL) {
    system("mkdir /tmp/xevtools");
    system("chmod 777 /tmp/xevtools");
  } else
    fclose(f);

  char *s = getenv("USER");
  if (s == NULL) {
    strcpy(t, "/tmp/xevtools/general_tmp");
    f = fopen(t, "r");
    if (f == NULL) {
      system("mkdir /tmp/xevtools/general_tmp");
      system("chmod 777 /tmp/xevtools/general_tmp");
    } else
      fclose(f);
  } else {
    sprintf(t, "/tmp/xevtools/%s_tmp", s);
    f = fopen(t, "r");
    if (f == NULL) {
      sprintf(t, "mkdir /tmp/xevtools/%s_tmp", s);
      system(t);
      sprintf(t, "chmod 777 /tmp/xevtools/%s_tmp", s);
      system(t);
      sprintf(t, "/tmp/xevtools/%s_tmp", s);
    } else
      fclose(f);
  }

  return t;
}

char *search_file(char *filename) {
  char t[XEV_PATH_SIZE];

  sprintf(t, "%s", filename);

  FILE *f = fopen(t, "r");
  if (f != NULL) {
    fclose(f);
    return dupstr(t);
  }

  if (filename[0] == '/' ||
      (filename[0] == '.' && 
       (filename[1] == '/' ||
	(filename[1] == '.' && filename[2] == '/'))))
    return NULL;

  char *s = getenv("XEVTOOLSMODULESDIR");
  if (s != NULL) {
    char *s0, *s1;
    s0 = s;
    s1 = strchr(s0, ':');

    if (s1 == NULL)
      strcpy(t, s0);
    else {
      int i;
      for (i=0; s0 + i < s1; i++)
	t[i] = s0[i];
      t[i] = '\0';
    }

    if (t[strlen(t)-1] == '/')
      sprintf(t + strlen(t), "%s", filename);
    else
      sprintf(t + strlen(t), "/%s", filename);

    f = fopen(t, "r");
    if (f != NULL) {
      fclose(f);
      return dupstr(t);
    }
  }

  s = getenv("XEVTOOLSDIR");
  if (s != NULL) {
    if (s[strlen(s)-1] == '/')
      sprintf(t, "%s%s", s, filename);
    else
      sprintf(t, "%s/%s", s, filename);

    f = fopen(t, "r");
    if (f != NULL) {
      fclose(f);
      return dupstr(t);
    }

    if (s[strlen(s)-1] == '/')
      sprintf(t, "%smodules/%s", s, filename);
    else
      sprintf(t, "%s/modules/%s", s, filename);

    f = fopen(t, "r");
    if (f != NULL) {
      fclose(f);
      return dupstr(t);
    }
  }

  return NULL;
}

int code_type(char *fname) {
  assert(fname != NULL);

  char *p = strrchr(fname, '.');

  if (p == NULL)
    return CODE_TYPE_OTHER;

  if (p[1] == 'f' || p[1] == 'F')
    return CODE_TYPE_FORTRAN;

  if (p[1] == 'c' || p[1] == 'C')
    return CODE_TYPE_C;

  return CODE_TYPE_OTHER;
}

void get_tmpname(char *q, char *src) {
  assert(q != NULL && src != NULL);

  char *ext = strrchr(src, '.');
  if (ext == NULL) {
    fprintf(stderr, "unknown file type 1: %s\n", src);
    exit(1);
  }

  char *p = strrchr(src, '/');
  if (p != NULL)
    p ++;
  else
    p = src;

  sprintf(q, "%s/%s", tmp_dir(), p);
}

void first_tmp(char *q, char *src) {
  assert(q != NULL && src != NULL);

  char *ext = strrchr(src, '.');
  if (ext == NULL) {
    fprintf(stderr, "unknown file type 2: %s\n", src);
    exit(1);
  }

  char *p = strrchr(src, '/');
  if (p != NULL)
    p ++;
  else
    p = src;

  sprintf(q, "%s/%s", tmp_dir(), p);

  p = strrchr(q, '.');

  sprintf(p, "_0%s", ext);
}

void next_tmp(char *q, char *src) {
  assert(q != NULL && src != NULL);

  strcpy(q, src);

  char *ext = strrchr(src, '.');

  char *p = strrchr(q, '_');
  if (p == NULL) {
    fprintf(stderr, "unknown file type 3: %s\n", src);
    exit(1);
  }

  int n, e;
  e = sscanf(p, "_%d.", &n);
  if (e != 1) {
    fprintf(stderr, "unknown file type 4: %s\n", src);
    exit(1);
  }

  sprintf(p, "_%d%s", n+1, ext);
}

void prev_tmp(char *q, char *src) {
  assert(q != NULL && src != NULL);

  strcpy(q, src);

  char *ext = strrchr(src, '.');

  char *p = strrchr(q, '_');
  if (p == NULL) {
    fprintf(stderr, "unknown file type 5: %s\n", src);
    exit(1);
  }

  int n, e;
  e = sscanf(p, "_%d.", &n);
  assert(e == 1);

  sprintf(p, "_%d%s", n-1, ext);
}

void set_ext(char *q, char *ext) {
  assert(q != NULL && ext != NULL);

  char *p = strrchr(q, '.');
  if (p == NULL) {
    p = q + strlen(q);
    p[0] = '.';
  }

  strcpy(p+1, ext);
}

int remove_tmp(void) {
  return REMOVE_TMP;
}

int xevutil_file_exist(char *path) {

  struct stat st;
  int res = stat(path, &st);
  if (res == -1)
    return 0;

  return 1;
}

void xev_log1(char *s1) {
  char t[XEV_PATH_SIZE];
  sprintf(t, "%s/log", tmp_dir());
  FILE *f = fopen(t, "a");
  if (f != NULL) {
    fprintf(f, "%s\n", s1);
    fclose(f);
  }
}

void xev_log2(char *s1, char *s2) {
  char t[XEV_PATH_SIZE];
  sprintf(t, "%s/log", tmp_dir());
  FILE *f = fopen(t, "a");
  if (f != NULL) {
    fprintf(f, "%s %s\n", s1, s2);
    fclose(f);
  }
}

void xev_log3(char *s1, char *s2, char *s3) {
  char t[XEV_PATH_SIZE];
  sprintf(t, "%s/log", tmp_dir());
  FILE *f = fopen(t, "a");
  if (f != NULL) {
    fprintf(f, "%s %s %s\n", s1, s2, s3);
    fclose(f);
  }
}

void xev_log4(char *s1, char *s2, char *s3, char *s4) {
  char t[XEV_PATH_SIZE];
  sprintf(t, "%s/log", tmp_dir());
  FILE *f = fopen(t, "a");
  if (f != NULL) {
    fprintf(f, "%s %s %s %s\n", s1, s2, s3, s4);
    fclose(f);
  }
}

char* dupstr(char *s) {
  if (s == NULL)
    return NULL;

  char *t = (char*) malloc(sizeof(char) * (strlen(s) + 1));
  assert(t != NULL);

  strcpy(t, s);
  return t;
}

strobj copyrange(char *s, char *t) {
  assert(s != NULL && t != NULL);
  int i = t - s;

  if (i < 0)
    return xos._;

  char p[i+2];
  char *q = p;

  while (s != t)
    *(q++) = *(s++);
  *(q++) = *(s++);

  *q = '\0';

  return get_strobj(p);
}

strobj get_depth_strobj(int d) {
  if (d < XOS_SPACE_MAX)
    return xos.spaces[d];

  char s[2*d+2];

  int i;
  s[0] = '\n';
  for (i = 1; i < 2*d+1; i++)
    s[i] = ' ';
  s[2*d+1] = '\0';

  return get_strobj(s);
}
