#include "virtual-machine.h"

CodeT lambda0[39];
CodeT if_branch2[3];
CodeT else_branch3[40];
CodeT lambda4[19];
CodeT lambda5[3];
CodeT lambda6[3];
CodeT letrec1_go[3];
CodeT lambda7[23];
CodeT lambda8[9];
CodeT lambda9[12];
CodeT main_code[9];

CodeT lambda0[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 2L},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 4L},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 3L},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Return},
};
CodeT if_branch2[] =
{
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Return},
};
CodeT else_branch3[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_go},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Plus},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Return},
};
CodeT lambda4[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Branch},{.reference = if_branch2},{.reference = else_branch3},
    {.instruction = Return},
};
CodeT lambda5[] =
{
    {.instruction = Curry},{.reference = lambda4},
    {.instruction = Return},
};
CodeT lambda6[] =
{
    {.instruction = Curry},{.reference = lambda5},
    {.instruction = Return},
};
CodeT letrec1_go[] =
{
    {.instruction = Curry},{.reference = lambda6},
    {.instruction = Return},
};
CodeT lambda7[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_go},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Return},
};
CodeT lambda8[] =
{
    {.instruction = Push},
    {.instruction = Curry},{.reference = lambda0},
    {.instruction = Swap},
    {.instruction = Curry},{.reference = lambda7},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Return},
};
CodeT lambda9[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Curry},{.reference = lambda8},
    {.instruction = Swap},
    {.instruction = Curry},{.reference = lambda9},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Halt},
};

CodeT *get_main_code()
{
    return main_code;
}
