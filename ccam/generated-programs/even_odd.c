#include "virtual-machine.h"

CodeT if_branch2[3];
CodeT else_branch3[16];
CodeT lambda4[14];
CodeT if_branch5[16];
CodeT else_branch6[3];
CodeT lambda7[14];
CodeT letrec0_even[3];
CodeT letrec1_odd[3];
CodeT main_code[20];

CodeT if_branch2[] =
{
    {.instruction = QuoteBool},{.data = True},
    {.instruction = Return},
};
CodeT else_branch3[] =
{
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_odd},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Return},
};
CodeT lambda4[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Branch},{.reference = if_branch2},{.reference = else_branch3},
    {.instruction = Return},
};
CodeT if_branch5[] =
{
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_even},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Return},
};
CodeT else_branch6[] =
{
    {.instruction = QuoteBool},{.data = False},
    {.instruction = Return},
};
CodeT lambda7[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Neq},
    {.instruction = Branch},{.reference = if_branch5},{.reference = else_branch6},
    {.instruction = Return},
};
CodeT letrec0_even[] =
{
    {.instruction = Curry},{.reference = lambda4},
    {.instruction = Return},
};
CodeT letrec1_odd[] =
{
    {.instruction = Curry},{.reference = lambda7},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_even},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 7L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_odd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 9L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Halt},
};

CodeT *get_main_code()
{
    return main_code;
}