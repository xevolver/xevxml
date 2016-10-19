#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xevutil.h"

typedef struct MODINFO {
  strobj fullmodname;
  strobj dirpath;
  struct MODINFO *next;
} modlist_t;

static modlist_t *modlist = NULL;

static void add_modinfo(strobj dirpath, strobj fullmodname) {
  assert(dirpath != NULL && fullmodname != NULL);

  modlist_t *l = (modlist_t*) malloc(sizeof(modlist_t));
  assert(l != NULL);

  l->fullmodname = fullmodname;
  l->dirpath = dirpath;
  l->next = modlist;
  modlist = l;
}

static void make_xml(char *drsfile) {

  char command[XEV_COMMAND_LINE_SIZE];
  char xmlfile[XEV_PATH_SIZE];
  strcpy(xmlfile, drsfile);
  set_ext(xmlfile, "xml");

  if (xevtools_dir() != NULL)
    sprintf(command, "%sxevdrsparse %s %s", xevtools_dir(), drsfile, xmlfile);
  else
    sprintf(command, "xevdrsparse %s %s", drsfile, xmlfile);
  int res = system(command);
  if (res != 0) {
    fprintf(stderr, "ERROR at %s\n", command);
    exit(1);
  }

  strcpy(drsfile, xmlfile);
}

static FILE *open_module(strobj dirname, strobj modname, char *path) {

  char t[XEV_PATH_SIZE];

  strcpy(path, dirname);

  sprintf(t, "%s/%s.xml", path, modname);

  FILE *f = fopen(t, "r");
  if (f != NULL)
    return f;

  sprintf(t, "%s/%s.drs", path, modname);
  if (xevutil_file_exist(t)) {

    make_xml(t);

    f = fopen(t, "r");
    assert(f != NULL);
    return f;
  }

  char *s = getenv("XEVTOOLSMODULESDIR");
  if (s != NULL) {
    while (1) {
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

      int pos = strlen(t);
      if (t[pos-1] == '/')
	sprintf(path, "%s%s", t, dirname);
      else
	sprintf(path, "%s/%s", t, dirname);

      sprintf(t, "%s/%s.xml", path, modname);

      if (xevutil_file_exist(t)) {


	f = fopen(t, "r");
	if (f != NULL) {
	  fprintf(stderr, "xevutil_load_module: cannot open %s "
		  "(but exists)\n", t);
	  exit(1);
	}

	return f;
      }

      sprintf(t, "%s/%s.drs", path, modname);

      if (xevutil_file_exist(t)) {

	make_xml(t);

	f = fopen(t, "r");
	assert(f != NULL);
	return f;
      }

      if (s1 == NULL)
	break;
      else
	s = s1 + 1;
    }
  }

  s = getenv("XEVTOOLSDIR");
  if (s != NULL) {
    if (s[strlen(s)-1] == '/')
      sprintf(path, "%smodules/%s", s, dirname);
    else
      sprintf(path, "%s/modules/%s", s, dirname);

    sprintf(t, "%s/%s.xml", path, modname);

    if (xevutil_file_exist(t)) {

      f = fopen(t, "r");
      if (f != NULL) {
	  fprintf(stderr, "xevutil_load_module: cannot open %s "
		  "(but exists)\n", t);
	  exit(1);
	}

	return f;
    }

    sprintf(t, "%s/%s.drs", path, modname);

    if (xevutil_file_exist(t)) {

      make_xml(t);

      f = fopen(t, "r");
      assert(f != NULL);
      return f;
    }
  }

  return NULL;
}

static void inspect_module(xmlcell_t *x, strobj modname, char *dirpath,
			   strobj super) {

  for (; x != NULL; x = xml_get_next(x)) {

    if (xml_get_name(x) == xos._xevdrs_line &&
	xml_get_child(x) != NULL &&
	xml_get_child(xml_get_child(x)) != NULL) {

      xmlcell_t *header = xml_get_cell(x, "cc");
      if (xml_getstr(header, "a=name") == xos._use) {

	if (xml_get_child(header) == NULL) {
	  fprintf(stderr, "xevutil_module: no module in use in module %s\n",
		  modname);
	  exit(1);
	}

	xmlcell_t *c;
	for (c = xml_get_child(header); c != NULL; c = xml_get_next(c)) {
	  if (xml_get_name(c) != xos._xev_literal) {
	    fprintf(stderr, "xevutil_module: module in use must be xev_literal "
		    " but found %s, in module %s\n", xml_get_name(c), modname);
	    exit(1);
	  }

	  strobj submodname = xml_getstr(c, "a=name");
	  if (submodname[0] == '.') /* global */
	    xevutil_load_module(get_strobj(submodname+1), NULL, NULL);
	  else {		/* submodule */
	    char newsuper[XEV_PATH_SIZE];
	    if (super != NULL)
	      sprintf(newsuper, "%s.%s", super, modname);
	    else
	      strcpy(newsuper, modname);
	    xevutil_load_module(submodname, dirpath, get_strobj(newsuper));
	  }
	}
      }
    }
  }
}

static int load_module(xmlcell_t *x, strobj modname, char *dirpath,
		       strobj super) {

  for (; x != NULL; x = xml_get_next(x)) {

    if (xml_get_name(x) == xos._xevdrs_line &&
	xml_get_child(x) != NULL &&
	xml_get_child(xml_get_child(x)) != NULL) {

      xmlcell_t *header = xml_get_cell(x, "cc");
      if (xml_getstr(header, "a=name") == xos._module &&
	  xml_get_next(header) != NULL &&
	  xml_getstr(header, "na=name") == modname) {

	if (xml_get_next(xml_get_next(header)) == NULL ||
	    xml_getstr(header, "nna=name") != xos._dynamic) {
	  fprintf(stderr, "xevutil_module: module %s must be dynamic\n", 
		  modname);
	  exit(1);
	}

	if (xml_get_next(xml_get_child(x)) == NULL ||
	    xml_getstr(x, "cn") != xos._xev_target) {
	  fprintf(stderr, "xevutil_module: module %s format error\n",
		  modname);
	  exit(1);
	}

	inspect_module(xml_get_child(xml_get_cell(x, "cn")),
		       modname, dirpath, super);
	return 1;

      }
    }

    if (xml_get_child(x) != NULL) {
      int cres = load_module(xml_get_child(x), modname, dirpath, super);
      if (cres)
	return 1;
    }
  }

  return 0;
}

void xevutil_load_module(strobj fullmodname, char *parentpath, strobj super) {

  char dirname[strlen(fullmodname) + 1];
  char *modname;
  char dirpath[XEV_PATH_SIZE];
  FILE *f = NULL;

  if (parentpath == NULL) {
    assert(super == NULL);

    strcpy(dirname, fullmodname);

    char *t = strrchr(fullmodname, '.');
    if (t == NULL) {

      modname = fullmodname;

    } else {

      modname = get_strobj(t+1);

      int i;
      for (i=0; dirname[i] != '\0'; i++)
	if (dirname[i] == '.')
	  dirname[i] = '/';

    }

    f = open_module(dirname, modname, dirpath);
    if (f == NULL) {
      fprintf(stderr, "xevutil_module: cannot find module %s\n", modname);
      exit(1);
    }

  } else {			/* parentpath is given */

    char t[XEV_PATH_SIZE];

    assert(strchr(fullmodname, '.') == NULL);
    modname = fullmodname;

    sprintf(dirpath, "%s/%s", parentpath, modname);

    sprintf(t, "%s/%s.xml", dirpath, modname);
    if (xevutil_file_exist(t)) {

      f = fopen(t, "r");
      assert(f != NULL);

    } else {

      sprintf(t, "%s/%s.drs", dirpath, modname);
      if (xevutil_file_exist(t)) {

	make_xml(t);

	f = fopen(t, "r");
	assert(f != NULL);

      } else {

	fprintf(stderr, "xevutil_module: cannot find module %s/%s\n",
		parentpath, modname);
	exit(1);
      }
    }
  }

  if (parentpath == NULL)
    add_modinfo(dirpath, fullmodname);
  else {
    assert(super != NULL);
    char makefullname[XEV_PATH_SIZE];
    sprintf(makefullname, "%s.%s", super, fullmodname);
    add_modinfo(dirpath, get_strobj(makefullname));
  }

  xmlcell_t *x = read_xml(f);
  check_xmlformat(x);

  int res = load_module(x, modname, dirpath, super);
  if (res == 0) {
    fprintf(stderr, "xevutil_module: file %s/%s does not contain module %s\n",
	    dirpath, modname, fullmodname);
    exit(1);
  }

  fclose(f);
}

int xevutil_module_exist(strobj modname) {

  modlist_t *l;
  for (l = modlist; l != NULL; l = l->next)
    if (l->fullmodname == modname) {
      return 1;
    }

  return 0;
}
