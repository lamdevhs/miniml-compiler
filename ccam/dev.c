#include <stdlib.h>
#include <stdio.h>
  ///
#include "ccam.h"

typedef struct foo {
  int x;
  int y;
  char *z;
} Foo;

char *test_str_ptr()
{
  return "yaha" NL;
}

int main ()
{
  printf("dev!" NL);
  Foo *k = malloc(sizeof(Foo));
  printf("%s", k->z); printf(NL);
  k->z = test_str_ptr();
  printf("%s%p %p"NL, k->z, k->z, test_str_ptr());
}
