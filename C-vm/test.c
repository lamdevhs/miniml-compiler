#include <stdio.h>
#include <stdlib.h>
////
#include "virtual-machine.h"


void assert(char *str, int boolean) {
  if (!boolean) {
    printf("[ERROR] %s" NL, str);
    exit(1);
  }
#ifdef VERBOSE
  else printf("[ok] %s" NL, str);
#endif
}


void test_value() {
  enum Status status = AllOk;
  
  //| -------- testing value_Bool
  {
    long b0 = False;
    Value *bv = value_Bool(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("Bool(False)", b1 == b0 && status == AllOk);
  }
  {
    long b0 = True;
    Value *bv = value_Bool(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("Bool(True)", b1 == b0 && status == AllOk);
  }

  //| -------- testing value_Pair
  {
    Value *pair = value_Pair(value_Bool(0), value_Bool(1));
    Pair p = match_value_with_pair(pair, &status);
    assert("Pair(b, b)", p.first->as.boolean == 0 && p.second->as.boolean == 1);
  }
  {
    Value *pair = value_Pair(value_Pair(value_Bool(1), value_Bool(0)), value_Bool(0));
    Pair p = match_value_with_pair(pair, &status);
    assert("Pair(Pair(b, b), b)",
      p.first->tag == PairValue && p.second->as.boolean == 0 && status == AllOk);
  }

  //| -------- testing value_Null
  {
    Value *nullv = value_Null();
    assert("Null()", nullv->tag == NullValue);
  }

  //| -------- testing value_Closure
  {
    Value *p = value_Pair(value_Bool(True), value_Bool(False));
    Value *closure = value_Closure((Bin *)42L, p);
    Closure c = match_value_with_closure(closure, &status);
    assert("Closure(42, Pair(True, False))",
      c.value == p && c.code == (Bin *)42L && status == AllOk);
  }

  //| -------- testing value_Integer
  {
    Value *integer = value_Int(43L);
    long i = match_value_with_integer(integer, &status);
    assert("Int(43)", i == 43L && status == AllOk);
  }

  // Value *all_values[] = {
  //   NULL, value_Null(), value_Bool(True), value_Int(55L),
  //   value_Pair(value_Null(), value_Null()),
  //   value_Closure((Bin *)123L, value_Null())
  // };
  // //| -------- testing match_value_with_boolean
  // {
  //   for (int i = 0; i < 6; i++) {
  //     long b =
}


// void test_exec() {
//   //| -------- testing Halt
//   {
//     Bin *program = { 42L, Halt, Swap };
//     MachineState *ms = blank_state(program + 1);
//     enum Status status = run_machine(ms);
//   }
// }


int main() {
  test_value();
  //test_exec();
  return 0;
}
