#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

static int get_ofmt(char *fname) {
  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    fprintf(stderr, "cannot open %s\n", fname);
    exit(1);
  }

  char line[1024];
  fgets(line, 1024, f);
  if (strstr(line, "<?xml") != line) {
    fprintf(stderr, "xevunparse: unknown input file\n");
    exit(1);
  }

  fgets(line, 1024, f);
  char *p = strstr(line, "ofmt=");
  if (p == NULL) {
    fprintf(stderr, "xevunparse: cannot find ofmt\n");
    exit(1);
  }

  int ofmt;
  sscanf(p, "ofmt=\"%d\"", &ofmt);
  return ofmt;
}

int main(int argc, char **argv) {
  char command[XEV_COMMAND_LINE_SIZE];

  if (argc != 3) {
    fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
    exit(1);
  }

  init_xos();

  xev_log3("* xevunparse", argv[1], argv[2]);
  
  char tmp4[XEV_PATH_SIZE];
  get_tmpname(tmp4, argv[1]);
  set_ext(tmp4, "xml3");

  xev_log3("xslt unvardec." XT_FORMATSTR ".xslt", argv[1], tmp4);
  if (xevtools_dir() != NULL)
    sprintf(command, "xsltproc %sunvardec." XT_FORMATSTR ".xslt %s > %s", xevtools_dir(), argv[1], tmp4);
  else
    sprintf(command, "xsltproc unvardec." XT_FORMATSTR ".xslt %s > %s", argv[1], tmp4);
  int res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }
  
  char tmp3[XEV_PATH_SIZE];
  strcpy(tmp3, tmp4);
  set_ext(tmp3, "xml2");

  xev_log3("xevrestruct -b", tmp4, tmp3);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevrestruct -b %s %s", xevtools_dir(), tmp4, tmp3);
  else
    sprintf(command, "xevrestruct -b %s %s", tmp4, tmp3);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }
  
  char tmp1[XEV_PATH_SIZE];
  strcpy(tmp1, tmp3);
  set_ext(tmp1, "xml1");

  xev_log3("xevpreunparse", tmp3, tmp1);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevpreunparse %s %s", xevtools_dir(), tmp3, tmp1);
  else
    sprintf(command, "xevpreunparse %s %s", tmp3, tmp1);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  int ofmt = get_ofmt(argv[1]);

  char tmp2[XEV_PATH_SIZE];
  strcpy(tmp2, tmp1);
  if (ofmt == 1)
    set_ext(tmp2, "f");
  else {
    set_ext(tmp2, "f90");
  }

  xev_log3("xml2src", tmp1, tmp2);
  if (xevxml_dir() != NULL)
    sprintf(command, "%sxml2src %s < %s", xevxml_dir(), tmp2, tmp1);
  else
    sprintf(command, "xml2src %s < %s", tmp2, tmp1);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  char tmp0[XEV_PATH_SIZE];
  strcpy(tmp0, tmp2);
  char *p = tmp0 + strlen(tmp0);
  while (*p != '.')
    p[2] = p[0], p--;
  p[0] = '_';  p[1] = 'x';  p[2] = '.';

  xev_log3("postunparse", tmp2, tmp0);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevpostunparse %s %s", xevtools_dir(), tmp2, tmp0);
  else
    sprintf(command, "xevpostunparse %s %s", tmp2, tmp0);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  xev_log3("continu", tmp0, argv[2]);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevutil_continu d%dd %s %s", xevtools_dir(),
	    ofmt - 1, tmp0, argv[2]);
  else
    sprintf(command, "xevutil_continu d%dd %s %s",
	    ofmt - 1, tmp0, argv[2]);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  xev_log3("* xevunparse done", argv[1], argv[2]);

  return 0;
}
