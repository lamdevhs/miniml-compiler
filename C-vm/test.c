#include <stdio.h>
#include <stdlib.h>
////
#include "virtual-machine.h"


void assert(char *str, int boolean) {
  if (!boolean) {
    printf("assertion %s failed" NL, str);
    exit(1);
  }
#ifdef VERBOSE
  else printf("assertion %s succeeded" NL, str);
#endif
}


void test_value_Bool() {
  enum Status status = AllOk;
  {
    long b0 = 0;
    Value *bv = value_Bool(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("boolean False", b1 == b0 && status == AllOk);
  }
  {
    long b0 = 1;
    Value *bv = value_Bool(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("boolean True", b1 == b0 && status == AllOk);
  }
}

void test_value_Pair() {
  enum Status status = AllOk;
  {
    Value *pair = value_Pair(value_Bool(0), value_Bool(1));
    Pair p = match_value_with_pair(pair, &status);
    assert("pair of booleans", p.first->as.boolean == 0 && p.second->as.boolean == 1);
  }
  {
    Value *pair = value_Pair(value_Pair(value_Bool(1), value_Bool(0)), value_Bool(0));
    Pair p = match_value_with_pair(pair, &status);
    assert("pair(pair(b, b), b)",
      p.first->tag == PairValue && p.second->as.boolean == 0);
  }
}

void test_value() {
  test_value_Bool();
  test_value_Pair();
}

int main() {
  test_value();
  return 0;
}