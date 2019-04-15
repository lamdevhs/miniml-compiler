#include "virtual-machine.h"
#include "generated.h"

CodeT if_branch1[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Plus},
    {.instruction = Return},
};
CodeT if_branch2[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_ack},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Return},
};
CodeT else_branch3[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_ack},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_ack},
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
CodeT else_branch4[] =
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
CodeT lambda5[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Branch},{.reference = if_branch1},{.reference = else_branch4},
    {.instruction = Return},
};
CodeT lambda6[] =
{
    {.instruction = Cur},{.reference = lambda5},
    {.instruction = Return},
};
CodeT letrec0_ack[] =
{
    {.instruction = Cur},{.reference = lambda6},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_ack},
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