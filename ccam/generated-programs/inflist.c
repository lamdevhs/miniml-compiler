#include "virtual-machine.h"

CodeT lambda4[8];
CodeT lambda5[8];
CodeT lambda6[25];
CodeT lambda7[3];
CodeT if_branch8[5];
CodeT else_branch9[39];
CodeT lambda10[16];
CodeT lambda11[3];
CodeT letrec0_f[3];
CodeT letrec1_g[3];
CodeT letrec2_h[3];
CodeT letrec3_take[3];
CodeT main_code[73];

CodeT lambda4[] =
{
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Swap},
    {.instruction = Call},{.reference = letrec1_g},
    {.instruction = Cons},
    {.instruction = Return},
};
CodeT lambda5[] =
{
    {.instruction = Push},
    {.instruction = QuoteInt},{.data = 2L},
    {.instruction = Swap},
    {.instruction = Call},{.reference = letrec0_f},
    {.instruction = Cons},
    {.instruction = Return},
};
CodeT lambda6[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec2_h},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Plus},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Return},
};
CodeT lambda7[] =
{
    {.instruction = Curry},{.reference = lambda6},
    {.instruction = Return},
};
CodeT if_branch8[] =
{
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Return},
};
CodeT else_branch9[] =
{
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec3_take},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Arith},{.operation = Sub},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteBool},{.data = False},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Return},
};
CodeT lambda10[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Unary},{.operation = Snd},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 1L},
    {.instruction = Cons},
    {.instruction = Compare},{.operation = Eq},
    {.instruction = Branch},{.reference = if_branch8},{.reference = else_branch9},
    {.instruction = Return},
};
CodeT lambda11[] =
{
    {.instruction = Curry},{.reference = lambda10},
    {.instruction = Return},
};
CodeT letrec0_f[] =
{
    {.instruction = Curry},{.reference = lambda4},
    {.instruction = Return},
};
CodeT letrec1_g[] =
{
    {.instruction = Curry},{.reference = lambda5},
    {.instruction = Return},
};
CodeT letrec2_h[] =
{
    {.instruction = Curry},{.reference = lambda7},
    {.instruction = Return},
};
CodeT letrec3_take[] =
{
    {.instruction = Curry},{.reference = lambda11},
    {.instruction = Return},
};
CodeT main_code[] =
{
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec3_take},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 10L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec0_f},
    {.instruction = Swap},
    {.instruction = QuoteBool},{.data = False},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec3_take},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 10L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec1_g},
    {.instruction = Swap},
    {.instruction = QuoteBool},{.data = False},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec3_take},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 10L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = Push},
    {.instruction = Push},
    {.instruction = Call},{.reference = letrec2_h},
    {.instruction = Swap},
    {.instruction = QuoteInt},{.data = 0L},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Swap},
    {.instruction = QuoteBool},{.data = False},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Apply},
    {.instruction = Cons},
    {.instruction = Cons},
    {.instruction = Halt},
};

CodeT *get_main_code()
{
    return main_code;
}
