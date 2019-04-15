#include <stdio.h>
#include <stdlib.h>
////
#include "virtual-machine.h"


void test_eval_primop() {
  int res; enum Status status = AllOk;
  
  res = eval_binary_operation(Mul, 3, 7, &status);
  printf("xyz\n" "%d -- status = %d\n", res, status);
  
  res = eval_binary_operation(Mod, 5, 0, &status);
  printf("xyz\n" "%d -- status = %d\n", res, status);
  
  res = eval_binary_operation(4242, 3, 0, &status);
  printf("xyz\n" "%d -- status = %d\n", res, status);
}

#define HOLE 0
#define NL "\n"
void test_mini_program() {
  // [Push; Curry [Push; iSnd; Swap; Quote (IntV 1); Cons;
  //             PrimInstr (BinOp (BArith BAadd)); Return];
  //  Swap; Quote (IntV 2); Cons; Apply]
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
    { .instruction = Curry }, { .reference = lambda0 },
    { .instruction = Swap },
    { .instruction = QuoteInt }, { .data = 2L },
    { .instruction = Cons },
    { .instruction = Apply },
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

CodeT if_branch0[3];
CodeT letrec1_f[3];
CodeT letrec2_g[3];
CodeT letrec3_h[3];
CodeT letrec4_i[3];
CodeT else_branch5[3];
CodeT lambda6[11];
CodeT main_code[9];

CodeT if_branch0[] =
{
    {.instruction = QuoteInt},{.data = 3L},
    {.instruction = Return},
};
CodeT letrec1_f[] =
{
    {.instruction = Call},{.reference = letrec2_g},
    {.instruction = Return},
};
CodeT letrec2_g[] =
{
    {.instruction = QuoteInt},{.data = 123L},
    {.instruction = Return},
};
CodeT letrec3_h[] =
{
    {.instruction = Call},{.reference = letrec4_i},
    {.instruction = Return},
};
CodeT letrec4_i[] =
{
    {.instruction = Call},{.reference = letrec3_h},
    {.instruction = Return},
};
CodeT else_branch5[] =
{
    {.instruction = Call},{.reference = letrec1_f},
    {.instruction = Return},
};
CodeT lambda6[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = Branch},{.reference = if_branch0},{.reference = else_branch5},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Plus},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Curry},{.reference = lambda6},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 2L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Halt},
};
