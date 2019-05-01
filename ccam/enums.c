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
    default: status = UnknownInstruction; break;
  }
  return status;
}

long eval_binary_operation(int operation, long a, long b, enum Status *status)
{
  switch (operation) {
  case Plus: return a + b;
    break;
  case Sub: return a - b;
    break;
  case Mul: return a * b;
    break;
  case Div:
    if (b == 0) { *status = DivZero; return 0; }
    else { return a / b; }
    break;
  case Mod:
    if (b == 0) { *status = DivZero; return 0; }
    else { return a % b; }
    break;
  case Eq: return a == b;
    break;
  case Neq: return a != b;
    break;
  case Ge: return a >= b;
    break;
  case Gt: return a > b;
    break;
  case Le: return a <= b;
    break;
  case Lt: return a < b;
    break;
  default: *status = UnknownBinary; return 0;
    break;
  }
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
      print_operation(code[1].operation);
      printf(")");
      break;
    case Compare:
      printf("Compare(");
      print_operation(code[1].operation);
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

void print_operation(int operation)
{
  switch (operation)
  {
    case Plus: printf("Plus"); break;
    case Sub: printf("Sub"); break;
    case Mul: printf("Mul"); break;
    case Div: printf("Div"); break;
    case Mod: printf("Mod"); break;

    case Eq: printf("Eq"); break;
    case Neq: printf("Neq"); break;
    case Ge: printf("Ge"); break;
    case Gt: printf("Gt"); break;
    case Le: printf("Le"); break;
    case Lt: printf("Lt"); break;

    default: printf("<Unknown Operation>"); break;
  }
}

void print_status(enum Status status) {
  switch (status) {
    case AllOk: printf("AllOk"); break;
    case Halted: printf("Halted"); break;
    case DivZero: printf("DivZero"); break;
    case UnknownInstruction: printf("UnknownInstruction"); break;
    case UnknownUnary: printf("UnknownUnary"); break;
    case UnknownBinary: printf("UnknownBinary"); break;
    case MatchFailure: printf("MatchFailure"); break;
    case MatchNULLStack: printf("MatchNULLStack"); break;
    case StackHeadIsNotValue: printf("StackHeadIsNotValue"); break;
    case StackHeadIsNotCode: printf("StackHeadIsNotCode"); break;
    case MatchNULLValue: printf("MatchNULLValue"); break;
    case ValueIsNotPair: printf("ValueIsNotPair"); break;
    case ValueIsNotClosure: printf("ValueIsNotClosure"); break;
    case ValueIsNotBool: printf("ValueIsNotBool"); break;
    case ValueIsNotInt: printf("ValueIsNotInt"); break;
    case ValueIsNotAList: printf("ValueIsNotAList"); break;
    default: printf("<Unknown>");
  }
}
