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
  CodeT lambda0[] = {
    { .instruction = Push },
    { .instruction = Unary }, { .operation = Snd },
    { .instruction = Swap },
    { .instruction = QuoteInt }, { .data = 1L },
    { .instruction = Cons },
    { .instruction = Arith }, { .operation = Plus },
    { .instruction = Return }
  };
  CodeT program[] = {
    { .instruction = Push },
    { .instruction = Cur }, { .reference = lambda0 },
    { .instruction = Swap },
    { .instruction = QuoteInt }, { .data = 2L },
    { .instruction = Cons },
    { .instruction = App },
    { .instruction = Halt },
  };
  printf("!! instruction: *(program[2]) = %d, Push = %d\n",
    program[2].reference[0].instruction, Push
  );
  
  MachineStateT *ms = blank_state(program);
  enum Status status = run_machine(ms, info(verbosity -->) True);
  printf("final status: %d, Halt = %d; NotPair = %d, code = %ld" NL,
    status, Halted, ValueIsNotPair, ms->code - program);
}

void test_unions()
{
  union { long a; ValueT* b; } foo = { .b = PairValue(IntValue(-3), BoolValue(True)) };
  printf("test unions: "); print_value(foo.b); printf(NL);
}

CodeT g1[3];
CodeT g2[3];

CodeT g1[] = { {.data = 1}, {.reference = g2}, {.data = 3}};
CodeT g2[] = { {.data = 4}, {.reference = g1}, {.data = 6}};

void test_local_vars()
{
  printf("g1 = %p, g1[1] = %p, g2 = %p, g2[1] = %p, " NL,
    g1, g1[1].reference, g2, g2[1].reference);
}

int main () {
  printf(" ( \\P\n"); 
  printf(" ( //\n"); 
  test_eval_primop();
  ValueT t1, t2;
  t1.tag = ValueIsInt;
  t1.as.integer = 3;
  t2.tag = ValueIsBool;
  t2.as.boolean = 1;
  t1 = t2;
  printf("=> %d -- %d -- %ld" NL, t1.tag, ValueIsBool, t1.as.boolean);
  
  test_mini_program();
  
  test_unions();
  test_local_vars();
  printf("size of CodeT: %ld, of ptr: %ld" NL, sizeof(CodeT), sizeof(void *));
  
}

