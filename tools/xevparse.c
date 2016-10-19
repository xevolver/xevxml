#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"
#include "xevparse.h"

int main(int argc, char **argv) {
  char command[XEV_COMMAND_LINE_SIZE];

  if (argc != 3) {
    fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
    exit(1);
  }

  init_xos();

  xev_log4("* xevparse", argv[1], argv[2], "start");

  char tmp0[XEV_PATH_SIZE];
  get_tmpname(tmp0, argv[1]);

  char *p = strrchr(tmp0, '.');
  int free_form = (strlen(p) >= 3);

  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevutil_continu c%dd %s %s", xevtools_dir(),
	    free_form, argv[1], tmp0);
  else
    sprintf(command, "xevutil_continu c%dd %s %s",
	    free_form, argv[1], tmp0);
  int res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  char tmp1[XEV_PATH_SIZE];
  get_tmpname(tmp1, argv[1]);
  p = tmp1 + strlen(tmp1);
  while (*p != '.')
    p[2] = p[0], p--;
  p[0] = '_'; p[1] = 't'; p[2] = '.';

  xev_log3("xevpreparse", tmp0, tmp1);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevpreparse %s %s", xevtools_dir(), tmp0, tmp1);
  else
    sprintf(command, "xevpreparse %s %s", tmp0, tmp1);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  char tmp2[XEV_PATH_SIZE];
  strcpy(tmp2, tmp1);
  set_ext(tmp2, "xml");

  xev_log3("src2xml", tmp1, tmp2);
  if (xevxml_dir() != NULL)
    sprintf(command, "%ssrc2xml %s > %s", xevxml_dir(), tmp1, tmp2);
  else
    sprintf(command, "src2xml %s > %s", tmp1, tmp2);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  char tmp3[XEV_PATH_SIZE];
  strcpy(tmp3, tmp2);
  set_ext(tmp3, "xml2");

  xev_log3("xevpostparse", tmp2, tmp3);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevpostparse %s %s", xevtools_dir(), tmp2, tmp3);
  else
    sprintf(command, "xevpostparse %s %s", tmp2, tmp3);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  char tmp4[XEV_PATH_SIZE];
  strcpy(tmp4, tmp3);
  set_ext(tmp4, "xml3");

  xev_log3("xevrestruct +b +c", tmp3, tmp4);
  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevrestruct +b %s %s", xevtools_dir(), tmp3, tmp4);
  else
    sprintf(command, "xevrestruct +b %s %s", tmp3, tmp4);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  xev_log3("xsltproc vardec." XT_FORMATSTR ".xslt", tmp4, argv[2]);
  if (xevtools_dir() != NULL)
    sprintf(command, "xsltproc %svardec." XT_FORMATSTR ".xslt %s > %s", xevtools_dir(), tmp4, argv[2]);
  else
    sprintf(command, "xsltproc vardec." XT_FORMATSTR ".xlst %s > %s", tmp4, argv[2]);
  res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  xev_log4("* xevparse", argv[1], argv[2], "finished");

  return 0;
}
