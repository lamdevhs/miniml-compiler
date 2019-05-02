#include <stdlib.h>
#include <stdio.h>
  ///
#include "ccam.h"

typedef struct foo {
  int x;
  int y;
  char *z;
} Foo;

int main ()
{
  printf("dev!" NL);
  Foo *k = malloc(sizeof(Foo));
  printf("%s", k->z);
  k->z = "blabla" NL;
  printf("%s", k->z);
}
