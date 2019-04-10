#include <stdlib.h>
///
#include "virtual-machine.h"

MachineState *blank_machine(Bin *code) {
  MachineState *ms = malloc(sizeof(MachineState));
  ms->term = value_Null();
  ms->code = code;
  ms->stack = empty_stack();
  return ms;
}

enum Status exec(MachineState *ms) {
  long instruction = ms->code[0];
  enum Status status = AllOk;
  switch (instruction) {
    case Halt: status = exec_Halt(ms); break;
    // case Unary: status = exec_Unary(ms); break;
    // case Arith: status = exec_Arith(ms); break;
    // case Compare: status = exec_Compare(ms); break;
    case Cons: status = exec_Cons(ms); break;
    case Push: status = exec_Push(ms); break;
    case Swap: status = exec_Swap(ms); break;
    case App: status = exec_App(ms); break;
    case Return: status = exec_Return(ms); break;
    case QuoteInt: status = exec_QuoteInt(ms); break;
    case QuoteBool: status = exec_QuoteBool(ms); break;
    // case Cur: status = exec_Cur(ms); break;
    case Branch: status = exec_Branch(ms); break;
    // AddDefs
    // RmDefs
    // Call
    default: status = UnknownInstruction; break;
  }
  return status;
}

enum Status run_machine(MachineState *ms) {
  enum Status status = AllOk;
  while (status == AllOk) {
    status = exec(ms);
  }
  return status;
}


enum Status exec_Halt(MachineState *ms) {  
  //| ms->term unchanged
  ms->code += 1; //| kind of an arbitrary choice
  //| ms->stack unchanged
  return Halted; //| <-- <-- <-- !
}

enum Status exec_Push(MachineState *ms) {
  //| (x, Push :: c, st) -> (x, c, Val(x) :: st)
  Value *x = ms->term;
  Stack *stack = ms->stack;
  Value *cloned_x = deepcopy_value(x);
  
  //| ms->term unchanged
  ms->code += 1;
  ms->stack = value_onto_stack(cloned_x, stack);
  return AllOk;
}

enum Status exec_Cons(MachineState *ms) {
  //| (x, Cons :: c, Val(y) :: st) -> (PairV(y, x), c, st)
  enum Status status = AllOk;
  ValueOnStack pattern = match_stack_with_value(ms->stack, &status);
  if (status != AllOk) return status;
  Value *x = ms->term;
  
  ms->term = value_Pair(pattern.head, x);
  ms->code += 1;
  ms->stack = pattern.tail;
  return AllOk;
}

enum Status exec_QuoteBool(MachineState *ms) {
  //| (_, QuoteBool(v) :: c, st) -> (BoolV(v), c, st)
  deepfree_value(ms->term);
  long v = ms->code[1];
  
  ms->term = value_Bool(v);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_QuoteInt(MachineState *ms) {
  //| (_, QuoteInt(v) :: c, st) -> (IntV(v), c, st)
  deepfree_value(ms->term);
  long v = ms->code[1];
  
  ms->term = value_Int(v);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_Swap(MachineState *ms) {
  //| (x, Swap :: c, Val(y) :: st) -> (y, c, Val (x) :: st)
  enum Status status = AllOk;
  ValueOnStack pattern = match_stack_with_value(ms->stack, &status);
  if (status != AllOk) return status;
  Value *x = ms->term;
  
  ms->term = pattern.head;
  ms->code += 1;
  ms->stack = value_onto_stack(x, pattern.tail);
  return AllOk;
}

enum Status exec_Cur(MachineState *ms) {
  //| (x, Cur (closure_code) :: c, st) -> (ClosureV(closure_code, x), c, st)
  Bin *closure_code = (Bin *)ms->code[1];
  Value *x = ms->term;
  
  ms->term = value_Closure(closure_code, x);
  ms->code += 2;
  //| ms-stack unchanged
  return AllOk;
}

enum Status exec_App(MachineState *ms) {
  //| (PairV(ClosureV(new_code, y), z), App :: old_code, st)
  //| -> (PairV(y, z), new_code, Cod(old_code) :: st)
  enum Status status = AllOk;
  Pair pair = match_value_with_pair(ms->term, &status);
  if (status != AllOk) return status;
  Closure closure = match_value_with_closure(pair.first, &status);
  if (status != AllOk) return status;
  
  Value *z = pair.second;
  Value *y = closure.value;
  Bin *new_code = closure.code;
  Stack *st = ms->stack;
  Bin *old_code = ms->code;
  
  ms->term = value_Pair(y, z);
  ms->code = new_code;
  ms->stack = code_onto_stack(old_code, st);
  return AllOk;
}

enum Status exec_Return(MachineState *ms) {
  //| (x, Return :: c, Cod(new_code) :: st) -> (x, new_code, st)
  enum Status status = AllOk;
  CodeOnStack pattern = match_stack_with_code(ms->stack, &status);
  if (status != AllOk) return status;
  
  // ms->term unchanged
  ms->code = pattern.head;
  ms->stack = pattern.tail;
  return AllOk;
}

enum Status exec_Branch(MachineState *ms) {
  //| (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st)
  //| -> (x, (if b then if_then else if_else), Cod(c) :: st)
  enum Status status = AllOk;
  ValueOnStack pattern = match_stack_with_value(ms->stack, &status);
  if (status != AllOk) return status;
  long b = match_value_with_boolean(ms->term, &status);
  if (status != AllOk) return status;
  
  Bin *code = ms->code;
  Bin *if_then = (Bin *)code[1];
  Bin *if_else = (Bin *)code[2];
  Bin *c = code + 3;
  
  ms->term = pattern.head;
  ms->code = (b ? if_then : if_else);
  ms->stack = code_onto_stack(c, pattern.tail);
  return AllOk;
}
