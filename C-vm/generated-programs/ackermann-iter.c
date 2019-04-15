#include "virtual-machine.h"
#include "generated.h"

CodeT lambda3[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Plus},
    {.instruction = Return},
};
CodeT if_branch4[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Return},
};
CodeT else_branch5[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_iter},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Return},
};
CodeT lambda6[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Branch},{.reference = if_branch4},{.reference = else_branch5},
    {.instruction = Return},
};
CodeT lambda7[] =
{
    {.instruction = Cur},{.reference = lambda6},
    {.instruction = Return},
};
CodeT if_branch8[] =
{
    {.instruction = Call},{.reference = letrec0_succ},
    {.instruction = Return},
};
CodeT else_branch9[] =
{
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_iter},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec2_ack},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Return},
};
CodeT lambda10[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Branch},{.reference = if_branch8},{.reference = else_branch9},
    {.instruction = Return},
};
CodeT letrec0_succ[] =
{
    {.instruction = Cur},{.reference = lambda3},
    {.instruction = Return},
};
CodeT letrec1_iter[] =
{
    {.instruction = Cur},{.reference = lambda7},
    {.instruction = Return},
};
CodeT letrec2_ack[] =
{
    {.instruction = Cur},{.reference = lambda10},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec2_ack},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 3L},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 2L},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Halt},
};