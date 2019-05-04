#include <stdlib.h>
#include <stdio.h>
  ///
#include "ccam.h"

enum Status execute_next_instruction(MachineStateT *ms)
{
  int instruction = ms->code[0].instruction;
  enum Status status = AllOk;
  switch (instruction) {
    case Halt: status = exec_Halt(ms); break;
    case Unary: status = exec_Unary(ms); break;
    case Arith: status = exec_Arith(ms); break;
    case Compare: status = exec_Compare(ms); break;
    case Cons: status = exec_Cons(ms); break;
    case Push: status = exec_Push(ms); break;
    case Swap: status = exec_Swap(ms); break;
    case Apply: status = exec_Apply(ms); break;
    case Return: status = exec_Return(ms); break;
    case QuoteInt: status = exec_QuoteInt(ms); break;
    case QuoteBool: status = exec_QuoteBool(ms); break;
    case Curry: status = exec_Curry(ms); break;
    case Branch: status = exec_Branch(ms); break;
    case Call: status = exec_Call(ms); break;
    case QuoteEmptyList: status = exec_QuoteEmptyList(ms); break;
    case MakeList: status = exec_MakeList(ms); break;
    default:
    {
      status = Crashed_UnknownInstruction;
    } break;
  }
  return status;
}

enum op_report eval_arith(int op, long a, long b, long *result)
{
  switch (op)
  {
    case Plus: *result = a + b; break;
    case Sub: *result = a - b; break;
    case Mul: *result = a * b; break;
    case Div:
      if (b == 0) { return InvalidOperands; }
      else { *result = a / b; }
      break;
    case Mod:
      if (b == 0) { return InvalidOperands; }
      else { *result = a % b; }
      break;
    default: return UnknownOp; break;
  }
  return OperationOk;
}

enum op_report eval_comparison(int op, long a, long b, long *result)
{
  switch (op)
  {
    case Eq: *result = a == b; break;
    case Neq: *result = a != b; break;
    case Ge: *result = a >= b; break;
    case Gt: *result = a > b; break;
    case Le: *result = a <= b; break;
    case Lt: *result = a < b; break;
    default: return UnknownOp; break;
  }
  return OperationOk;
}

void print_instruction(CodeT *code)
{
  int instruction = code[0].instruction;
  switch(instruction) {
    case Halt: printf("Halt"); break;
    case Unary:
      printf("Unary("); print_unary(code[1].operation); printf(")");
      break;
    case Arith:
      printf("Arith(");
      print_arith_operation(code[1].operation);
      printf(")");
      break;
    case Compare:
      printf("Compare(");
      print_comparison_operation(code[1].operation);
      printf(")");
      break;
    case Cons: printf("Cons"); break;
    case Push: printf("Push"); break;
    case Swap: printf("Swap"); break;
    case Apply: printf("Apply"); break;
    case Return: printf("Return"); break;
    case QuoteInt: printf("QuoteInt(%ld)", code[1].data); break;
    case QuoteBool:
      printf("QuoteBool(%s)", code[1].data ? "True" : "False");
      break;
    case Curry: printf("Curry(%p)", code[1].reference); break;
    case Branch:
      printf("Branch(%p,%p)", code[1].reference, code[2].reference);
      break;
    case Call: printf("Call(%p)", code[1].reference); break;
    default: printf("<Unknown Instruction>"); break;
  }
}

void print_unary(int unary_op)
{
  switch (unary_op)
  {
    case Fst: printf("Fst"); break;
    case Snd: printf("Snd"); break;
    case Head: printf("Head"); break;
    case Tail: printf("Tail"); break;
  }
}

void print_arith_operation(int operation)
{
  switch (operation)
  {
    case Plus: printf("Plus"); break;
    case Sub: printf("Sub"); break;
    case Mul: printf("Mul"); break;
    case Div: printf("Div"); break;
    case Mod: printf("Mod"); break;
    default: printf("<Unknown Arith Operation>"); break;
  }
}

void print_comparison_operation(int operation)
{
  switch (operation)
  {
    case Eq: printf("Eq"); break;
    case Neq: printf("Neq"); break;
    case Ge: printf("Ge"); break;
    case Gt: printf("Gt"); break;
    case Le: printf("Le"); break;
    case Lt: printf("Lt"); break;
    default: printf("<Unknown Comparison Operation>"); break;
  }
}

void print_status(enum Status status)
{
  printf("%s", status_message(status));
}

char *status_message(enum Status status)
{
  switch (status) {
    case AllOk:
    return "AllOk, no errors to report"; break;
    case Halted:
    return "halted, no errors to report"; break;
    case Crashed_UnknownInstruction:
    return "MachineFailure: unknown instruction"; break;
    case Crashed_Unary_Unknown:
    return "MachineFailure: unknown unary operator"; break;
    case Crashed_Unary_NotAPair:
    return "TypeError: can't get first/second: not a pair"; break;
    case Crashed_Unary_Headless:
    return "TypeError: can't get head/tail: empty list or not a list"; break;
    case Crashed_Cons_NoValueOnStack:
    return "MachineFailure: can't build pair: no value on stack"; break;
    case Crashed_Swap_NoValueOnStack:
    return "MachineFailure: can't swap: no value on stack"; break;
    case Crashed_CannotApply:
    return "MachineFailure: can't apply: invalid term"; break;
    case Crashed_CannotReturn:
    return "MachineFailure: can't return: no code reference on stack"; break;
    case Crashed_Arith_TypeError:
    return "TypeError: can't do arithmetic operation: "
           "operands missing or not integers"; break;
    case Crashed_Arith_Unknown:
    return "MachineFailure: unknown arithmetic operation"; break;
    case Crashed_Arith_DivByZero:
    return "RuntimeException: division by zero"; break;
    case Crashed_Compare_TypeError:
    return "TypeError: can't compare: operands either missing "
           "or neither two integers nor two booleans"; break;
    case Crashed_Compare_Unknown:
    return "MatchFailure: unknown comparison operation"; break;
    case Crashed_Branch_NotABoolean:
    return "TypeError: in conditional branch: condition is not a boolean"; break;
    case Crashed_Branch_NoValueOnStack:
    return "MachineFailure: in conditional branch: no value on stack"; break;
    case Crashed_MakeList_NotAList:
    return "TypeError: can't build list: tail is not a list"; break;
    case Crashed_MakeList_NoValueOnStack:
    return "MachineFailure: can't build list: no value on stack"; break;

    default: return "<unknown error id>"; break;
  }
}
