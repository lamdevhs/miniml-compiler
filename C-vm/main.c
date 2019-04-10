#include <stdio.h>
#include <stdlib.h>
////
#include "virtual-machine.h"


// long eval_primop(int op, long a, long b, enum Status *status) {
//   switch (op) {
//   case Plus: return a + b;
//     break;
//   case Sub: return a - b;
//     break;
//   case Mul: return a * b;
//     break;
//   case Div:
//     if (b == 0) { *status = DivZero; return 0; }
//     else { return a / b; }
//     break;
//   case Mod:
//     if (b == 0) { *status = DivZero; return 0; }
//     else { return a % b; }
//     break;
//   case Eq: return a == b;
//     break;
//   case Neq: return a != b;
//     break;
//   case Ge: return a >= b;
//     break;
//   case Gt: return a < b;
//     break;
//   case Le: return a < b;
//     break;
//   case Lt: return a <= b;
//     break;
//   default: *status = UnknownOperation; return 0;
//     break;
//   }
// }


void test_eval_primop() {
  int res; enum Status status = AllOk;
  
  res = eval_primop(Mul, 3, 7, &status);
  printf("xyz\n" "%d -- status = %d\n", res, status);
  
  res = eval_primop(Mod, 5, 0, &status);
  printf("xyz\n" "%d -- status = %d\n", res, status);
  
  res = eval_primop(4242, 3, 0, &status);
  printf("xyz\n" "%d -- status = %d\n", res, status);
}

#define HOLE 0
#define NL "\n"
void test_mini_program() {
  // [Push; Cur [Push; iSnd; Swap; Quote (IntV 1); Cons;
  //             PrimInstr (BinOp (BArith BAadd)); Return];
  //  Swap; Quote (IntV 2); Cons; App]
  Bin program[] = {
     Push, Cur, HOLE, Swap, QuoteInt, 2L, Cons, App, Halt,
     Push, Unary, Snd, Swap, QuoteInt, 1L, Cons, Arith, Plus, Return
  };
  program[2] = (long)(program + 9);
  printf("instruction: *program[2] = %ld, Push = %d\n",
    ((Bin *)program[2])[0], Push);
  
  MachineState *ms = blank_machine(program);
  enum Status status = run_machine(ms);
  printf("final status: %d, Halt = %d; NotPair = %d, code = %ld" NL,
    status, Halted, ValueIsNotPair, (long)(ms->code - program));
}

int main () {
  printf(" ( \\P\n"); 
  printf(" ( //\n"); 
  printf("nothing to see\n");
  test_eval_primop();
  Value t1, t2;
  t1.tag = IntValue;
  t1.as.integer = 3;
  t2.tag = BoolValue;
  t2.as.boolean = 1;
  t1 = t2;
  printf("=> %d -- %d -- %ld\n", t1.tag, BoolValue, t1.as.boolean);
  
  test_mini_program();
}

