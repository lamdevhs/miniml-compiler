#include "virtual-machine.h"
#include "generated.h"

CodeT if_branch1[] =
{
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Return},
};
CodeT else_branch2[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_factorial},
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
    {.instruction = Arith},{.operation = Mul},
    {.instruction = Return},
};
CodeT lambda3[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Branch},{.reference = if_branch1},{.reference = else_branch2},
    {.instruction = Return},
};
CodeT letrec0_factorial[] =
{
    {.instruction = Cur},{.reference = lambda3},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_factorial},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 15L},
    {.instruction = Cons},
    {.instruction = App},
    {.instruction = Halt},
};
