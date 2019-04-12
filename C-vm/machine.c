#include <stdlib.h>
///
#include "virtual-machine.h"


MachineStateT *MachineState(ValueT *term, CodeT *code, StackT *stack) {
  MachineStateT *ms = malloc(sizeof(MachineStateT));
  ms->term = term;
  ms->code = code;
  ms->stack = stack;
  return ms;
}

MachineStateT *blank_state(CodeT *code) {
  return MachineState(value_Null(), code, empty_stack());
}

int equal_states(MachineStateT *a, MachineStateT *b) {
  if (a == NULL) return b == NULL;
  if (a->code != b->code) return False;
  if (!!! equal_values(a->term, b->term)) return False;
  return equal_stacks(a->stack, b->stack);
}
  

enum Status run_machine(MachineStateT *ms, int verbose) {
  enum Status status = AllOk;
  while (status == AllOk) {
    if (verbose) {
      printf("Term = "); print_value(ms->term); printf(NL);
      printf("Instruction = "); print_instruction(ms->code[0]); printf(NL);
    }
    status = exec(ms);
  }
  return status;
}

enum Status exec(MachineStateT *ms) {
  long instruction = ms->code[0];
  enum Status status = AllOk;
  switch (instruction) {
    case Halt: status = exec_Halt(ms); break;
    case Unary: status = exec_Unary(ms); break;
    case Arith: status = exec_Arith(ms); break;
    // case Compare: status = exec_Compare(ms); break;
    case Cons: status = exec_Cons(ms); break;
    case Push: status = exec_Push(ms); break;
    case Swap: status = exec_Swap(ms); break;
    case App: status = exec_App(ms); break;
    case Return: status = exec_Return(ms); break;
    case QuoteInt: status = exec_QuoteInt(ms); break;
    case QuoteBool: status = exec_QuoteBool(ms); break;
    case Cur: status = exec_Cur(ms); break;
    case Branch: status = exec_Branch(ms); break;
    // AddDefs
    // RmDefs
    // Call
    default: status = UnknownInstruction; break;
  }
  return status;
}



//| individual instructions:

enum Status exec_Halt(MachineStateT *ms) {  
  //| ms->term unchanged
  ms->code += 1; //| kind of an arbitrary choice
  //| ms->stack unchanged
  return Halted; //| <-- <-- <-- !
}

enum Status exec_Arith(MachineStateT *ms) {
  //| (PairV(IntV x, IntV y), PrimInstr (BinOp (BArith op)) :: c, st)
  //| -> (IntV (eval_arith op x y), c, st)
  enum Status status = AllOk;
  PairT pair = match_value_with_pair(ms->term, &status);
  if (status != AllOk) return status;
  long x = match_value_with_integer(pair.first, &status);
  if (status != AllOk) return status;
  long y = match_value_with_integer(pair.second, &status);
  if (status != AllOk) return status;
  long operation = ms->code[1];
  long result = eval_primop(operation, x, y, &status);
  if (status != AllOk) return status;
  
  ms->term = value_Int(result);
  ms->code += 2;
  //| ms->stack unchanged
  return AllOk;
}

enum Status exec_Unary(MachineStateT *ms) {
  //| (PairV(x, y), PrimInstr (UnOp Fst) :: c, st) -> (x, c, st)
  //| (PairV(x, y), PrimInstr (UnOp Snd) :: c, st) -> (y, c, st)
  enum Status status = AllOk;
  PairT pair = match_value_with_pair(ms->term, &status);
  if (status != AllOk) return status;
  ValueT *x = pair.first;
  ValueT *y = pair.second;
  long instruction = ms->code[1];
  
  if (instruction == Fst) {
    ms->term = x;
    deepfree_value(y);
  }
  else if (instruction == Snd) {
    ms->term = y;
    deepfree_value(x);
  }
  else {
    return UnknownUnary;
  }
  ms->code += 2;
  //| ms->stack unchanged
  return AllOk;
}

enum Status exec_Push(MachineStateT *ms) {
  //| (x, Push :: c, st) -> (x, c, Val(x) :: st)
  ValueT *x = ms->term;
  StackT *stack = ms->stack;
  ValueT *cloned_x = deepcopy_value(x);
  
  //| ms->term unchanged
  ms->code += 1;
  ms->stack = value_onto_stack(cloned_x, stack);
  return AllOk;
}

enum Status exec_Cons(MachineStateT *ms) {
  //| (x, Cons :: c, Val(y) :: st) -> (PairV(y, x), c, st)
  enum Status status = AllOk;
  ValueOnStackT pattern = match_stack_with_value(ms->stack, &status);
  if (status != AllOk) return status;
  ValueT *x = ms->term;
  
  ms->term = value_Pair(pattern.top, x);
  ms->code += 1;
  ms->stack = pattern.bottom;
  return AllOk;
}

enum Status exec_QuoteBool(MachineStateT *ms) {
  //| (_, QuoteBool(v) :: c, st) -> (BoolV(v), c, st)
  deepfree_value(ms->term);
  long v = ms->code[1];
  
  ms->term = value_Bool(v);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_QuoteInt(MachineStateT *ms) {
  //| (_, QuoteInt(v) :: c, st) -> (IntV(v), c, st)
  deepfree_value(ms->term);
  long v = ms->code[1];
  
  ms->term = value_Int(v);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_Swap(MachineStateT *ms) {
  //| (x, Swap :: c, Val(y) :: st) -> (y, c, Val (x) :: st)
  enum Status status = AllOk;
  ValueOnStackT pattern = match_stack_with_value(ms->stack, &status);
  if (status != AllOk) return status;
  ValueT *x = ms->term;
  
  ms->term = pattern.top;
  ms->code += 1;
  ms->stack = value_onto_stack(x, pattern.bottom);
  return AllOk;
}

enum Status exec_Cur(MachineStateT *ms) {
  //| (x, Cur (closure_code) :: c, st) -> (ClosureV(closure_code, x), c, st)
  CodeT *closure_code = (CodeT *)ms->code[1];
  ValueT *x = ms->term;
  
  ms->term = value_Closure(closure_code, x);
  ms->code += 2;
  //| ms-stack unchanged
  return AllOk;
}

enum Status exec_App(MachineStateT *ms) {
  //| (PairV(ClosureV(new_code, y), z), App :: old_code, st)
  //| -> (PairV(y, z), new_code, Cod(old_code) :: st)
  enum Status status = AllOk;
  PairT pair = match_value_with_pair(ms->term, &status);
  if (status != AllOk) return status;
  ClosureT closure = match_value_with_closure(pair.first, &status);
  if (status != AllOk) return status;
  
  ValueT *z = pair.second;
  ValueT *y = closure.value;
  CodeT *new_code = closure.code;
  StackT *st = ms->stack;
  CodeT *old_code = ms->code + 1;
  
  ms->term = value_Pair(y, z);
  ms->code = new_code;
  ms->stack = code_onto_stack(old_code, st);
  return AllOk;
}

enum Status exec_Return(MachineStateT *ms) {
  //| (x, Return :: c, Cod(new_code) :: st) -> (x, new_code, st)
  enum Status status = AllOk;
  CodeOnStackT pattern = match_stack_with_code(ms->stack, &status);
  if (status != AllOk) return status;
  
  // ms->term unchanged
  ms->code = pattern.top;
  ms->stack = pattern.bottom;
  return AllOk;
}

enum Status exec_Branch(MachineStateT *ms) {
  //| (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st)
  //| -> (x, (if b then if_then else if_else), Cod(c) :: st)
  enum Status status = AllOk;
  ValueOnStackT pattern = match_stack_with_value(ms->stack, &status);
  if (status != AllOk) return status;
  long b = match_value_with_boolean(ms->term, &status);
  if (status != AllOk) return status;
  
  CodeT *code = ms->code;
  CodeT *if_then = (CodeT *)code[1];
  CodeT *if_else = (CodeT *)code[2];
  CodeT *c = code + 3;
  
  ms->term = pattern.top;
  ms->code = (b ? if_then : if_else);
  ms->stack = code_onto_stack(c, pattern.bottom);
  return AllOk;
}



//| utilitary:

long eval_primop(long operation, long a, long b, enum Status *status) {
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
  case Gt: return a < b;
    break;
  case Le: return a < b;
    break;
  case Lt: return a <= b;
    break;
  default: *status = UnknownArith; return 0;
    break;
  }
}

void print_instruction(long instruction) {
  switch(instruction) {
    case Halt: printf("Halt"); break;
    case Unary: printf("Unary"); break;
    case Arith: printf("Arith"); break;
    case Compare: printf("Compare"); break;
    case Cons: printf("Cons"); break;
    case Push: printf("Push"); break;
    case Swap: printf("Swap"); break;
    case App: printf("App"); break;
    case Return: printf("Return"); break;
    case QuoteInt: printf("QuoteInt"); break;
    case QuoteBool: printf("QuoteBool"); break;
    case Cur: printf("Cur"); break;
    case Branch: printf("Branch"); break;
    default: printf("<Unknown>"); break;
  }
}

void print_state(MachineStateT *ms) {
  if (ms == NULL) {
    printf("<NULL MachineState>" NL);
  }
  else {
    printf("MachineState:" NL);
    printf("  term = "); print_value(ms->term); printf(NL);
    printf("  code = %ld" NL, (long)ms->code);
    printf("  stack = "); print_stack(ms->stack); printf(NL);
  }
}

void print_status(enum Status status) {
  switch (status) {
    case AllOk: printf("AllOk"); break;
    case Halted: printf("Halted"); break;
    case DivZero: printf("DivZero"); break;
    case UnknownInstruction: printf("UnknownInstruction"); break;
    case UnknownUnary: printf("UnknownUnary"); break;
    case UnknownArith: printf("UnknownArith"); break;
    case MatchNULLStack: printf("MatchNULLStack"); break;
    case StackHeadIsNotValue: printf("StackHeadIsNotValue"); break;
    case StackHeadIsNotCode: printf("StackHeadIsNotCode"); break;
    case MatchNULLValue: printf("MatchNULLValue"); break;
    case ValueIsNotPair: printf("ValueIsNotPair"); break;
    case ValueIsNotClosure: printf("ValueIsNotClosure"); break;
    case ValueIsNotBool: printf("ValueIsNotBool"); break;
    case ValueIsNotInt: printf("ValueIsNotInt"); break;
    default: printf("<Unknown>");
  }
}
    