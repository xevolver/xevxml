#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "xevutil.h"

#define strobjHashBit 10
#define strobjHashSize (1<<(strobjHashBit))

#define strobjMinSize 8

typedef struct STROBJCELL {
  struct STROBJCELL *next;
  char s[strobjMinSize];
} strobjCell_t;

static strobjCell_t **strobjTable = NULL;

static int strobjHash(char *s) {
  if (s == NULL)
    return 0;

  int res = 0;
  int coef = 123;
  
  while (*s != '\0') {
    res += *s * coef;
    res &= strobjHashSize - 1;
    coef += 2;
    s ++;
  }

  assert(0 <= res && res < strobjHashSize);
  
  return res;
}

static strobjCell_t *newStrobjCell(char *s) {

  int len = strlen(s) + 1;
  int cellSize;
  if (len <= strobjMinSize)
    cellSize = sizeof(strobjCell_t);
  else
    cellSize = sizeof(strobjCell_t) + sizeof(char) * (len - strobjMinSize);
  
  strobjCell_t *cell = (strobjCell_t*) malloc(cellSize);
  assert(cell != NULL);
  strcpy(cell->s, s);
  cell->next = NULL;

  return cell;
}

strobj get_strobj(char *s) {
  if (s == NULL)
    return (strobj) NULL;

  if (strobjTable == NULL) {
    strobjTable = (strobjCell_t**) malloc(sizeof(strobjCell_t*)
					  * strobjHashSize);
    assert(strobjTable != NULL);

    int i;
    for (i = 0; i < strobjHashSize; i++)
      strobjTable[i] = NULL;
  }
  
  int p = strobjHash(s);
  strobjCell_t *cell;
  
  if (strobjTable[p] == NULL) {
    
    cell = newStrobjCell(s);
    strobjTable[p] = cell;

  } else if (strcmp(strobjTable[p]->s, s) == 0) {

    cell = strobjTable[p];

  } else {

    for (cell = strobjTable[p];
	 cell->next != NULL && strcmp(cell->next->s, s) != 0;
	 cell = cell->next);

    if (cell->next == NULL)
      cell->next = newStrobjCell(s);

    cell = cell->next;
  }
  
  return (strobj) cell->s;
}

void dump_strobj(void) {
  int i;
  for (i=0; i< strobjHashSize; i++) {
    strobjCell_t *cell = strobjTable[i];

    if (cell != NULL)
      printf("%6d ", i);
    
    while (cell != NULL) {
      assert(get_strobj(cell->s) == cell->s);
      printf("[%s] %lx\n", cell->s, (long)(cell->s));
      cell = cell->next;
      if (cell != NULL)
	printf("    -> ");
    }
  }
}

strobj int_strobj(int n) {
  int nn = n, c = 1;
  while (nn != 0) {
    nn /= 10;
    c ++;
  }

  char val[c+2];
  sprintf(val, "%d", n);
  return get_strobj(val);
}

int strobj_int(strobj s) {
  int n = 0;

  if (sscanf(s, "%d", &n) != 1) {
    fprintf(stderr, "strobj_int(%s) failed\n", s);
    exit(1);
  }
  return n;
}

#if 0

int main(void) {
  char line[1024];

  while (1) {
    fgets(line, 1024, stdin);

    strobj p = get_strobj(line);
    printf("%lx\n", (long)p);

    dump_strobj();
  }
  
  return 0;
}

#endif
