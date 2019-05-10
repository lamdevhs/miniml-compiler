#include <stdlib.h>
#include <stdio.h>
///
#include "ccam.h"

MachineStateT *MachineState(ValueT *term, CodeT *code, StackT *stack) {
  MachineStateT *ms = malloc(sizeof(MachineStateT));
  ms->term = term;
  ms->code = code;
  ms->stack = stack;
  return ms;
}

MachineStateT *blank_state(CodeT *code) {
  return MachineState(NullValue(), code, EmptyStack());
}

enum Status run_machine(MachineStateT *ms, int verbose)
{
  enum Status status = AllOk;
  while (status == AllOk) {
    if (verbose) {
      print_state(ms);
      printf("Next instruction: "); print_instruction(ms->code); printf(NL);
    }
    status = execute_next_instruction(ms);
  }
  return status;
}


//| code for each instruction:

enum Status exec_Halt(MachineStateT *ms)
{
  //| ms->term unchanged
  ms->code += 1; //| arbitrary choice here to make
  //| ms->stack unchanged
  return Halted; //| <-- <-- <-- !
}

enum Status exec_Unary(MachineStateT *ms)
{
  int operation = ms->code[1].operation;

  switch (operation) {
    case Fst:
    case Snd:
    {
      //| (PairV(x, y), PrimInstr (UnOp Fst) :: c, st) -> (x, c, st)
      //| (PairV(x, y), PrimInstr (UnOp Snd) :: c, st) -> (y, c, st)
      ValueT *term = ms->term;

      PairT pair;
      if (match_value_with_pair(term, &pair) == Failure) {
        return Crashed_Unary_NotAPair;
      }
      ValueT *x = pair.first;
      ValueT *y = pair.second;

      if (operation == Fst) {
        ms->term = x;
        deepfree_value(y);
      }
      else if (operation == Snd) {
        ms->term = y;
        deepfree_value(x);
      }
      else {
        printf("exec_Unary: Fatal, impossible, crazy bug!"); exit(1);
      }
      ms->code += 2;
      //| ms->stack unchanged
      return AllOk;
    }
    break;

    case Head:
    case Tail:
    {
      //| (l, PrimInstr (UnOp Head) :: c, st, fds) ->
      //|     match l with
      //|     | ListV EmptyListV -> failwith "RuntimeError: can't take the head of an empty list"
      //|     | ListV (ListConsV (h, t)) -> exec (h, c, st, fds)
      //|     | otherwise -> failwith "TypeError: can't take the head: not a list"
      //| (l, PrimInstr (UnOp Tail) :: c, st, fds) ->
      //|     match l with
      //|     | ListV EmptyListV -> failwith "RuntimeError: can't take the tail of an empty list"
      //|     | ListV (ListConsV (h, t)) -> exec (ListV t, c, st, fds)
      //|     | otherwise -> failwith "TypeError: can't take the head: not a list"
      ValueT *term = ms->term;

      ListConsT pattern;
      if (match_value_with_list_cons(term, &pattern) == Failure) {
        return Crashed_Unary_Headless;
      }
      ValueT *h = pattern.head;
      ValueT *t = pattern.tail;

      if (operation == Head) {
        ms->term = h;
        deepfree_value(t);
      }
      else if (operation == Tail) {
        ms->term = t;
        deepfree_value(h);
      }
      else {
        printf("exec_Unary: Fatal, impossible, crazy bug!"); exit(2);
      }
      ms->code += 2;
      //|ms->stack unchanged
      return AllOk;
    } break;

    default: return Crashed_Unary_Unknown; break;
  } //| end of switch
}

enum Status exec_Arith(MachineStateT *ms)
{
  //| (PairV(IntV x, IntV y), PrimInstr (BinOp (BArith op)) :: c, st)
  //| -> (IntV (eval_arith op x y), c, st)
  ValueT *term = ms->term;

  PairT pair;
  if (match_value_with_pair(term, &pair) == Failure) {
    return Crashed_Arith_TypeError;
  }
  long x;
  if (match_value_with_integer(pair.first, &x) == Failure) {
    //| reset the machine state
    ms->term = PairValue(pair.first, pair.second);
    return Crashed_Arith_TypeError;
  }
  long y;
  if (match_value_with_integer(pair.second, &y) == Failure) {
    //| reset the machine state
    ms->term = PairValue(IntValue(x), pair.second);
    return Crashed_Arith_TypeError;
  }
  int operation = ms->code[1].operation;
  long result;
  enum op_report report = eval_arith(operation, x, y, &result);
  if (report != OperationOk) {
    //| reset the machine state
    ms->term = PairValue(IntValue(x), IntValue(y));
    return (report == InvalidOperands)
      ? Crashed_Arith_DivByZero : Crashed_Arith_Unknown;
  }

  ms->term = IntValue(result);
  ms->code += 2;
  //| ms->stack unchanged
  return AllOk;
}

enum Status exec_Compare(MachineStateT *ms)
{
  //| (PairV(IntV x, IntV y), PrimInstr (BinOp (BCompar op)) :: c, st) ->
  //|  (BoolV (eval_compare op x y), c, st)
  //| (PairV(BoolV x, BoolV y), PrimInstr (BinOp (BCompar op)) :: c, st) ->
  //|  (BoolV (eval_compare op x y), c, st)
  ValueT *term = ms->term;

  PairT pair;
  if (match_value_with_pair(term, &pair) == Failure) {
    return Crashed_Compare_TypeError;
  }
  enum ValueTag tag = pair.first->tag;

  long x, y;
  //| either both operands are IntValues...
  if (tag == ValueIsInt && tag == pair.second->tag) {
    x = pair.first->as.integer;
    y = pair.second->as.integer;
  }
  //| ...or they're both BoolValues...
  else if (tag == ValueIsBool && tag == pair.second->tag) {
    x = pair.first->as.boolean;
    y = pair.second->as.boolean;
  }
  else { //| ... or, error
    //| reset the machine state
    ms->term = PairValue(pair.first, pair.second);
    return Crashed_Compare_TypeError;
  }
  int operation = ms->code[1].operation;
  long result;
  enum op_report report = eval_comparison(operation, x, y, &result);
  if (report != OperationOk) {
    //| reset the machine state
    ms->term = PairValue(pair.first, pair.second);
    return Crashed_Compare_Unknown;
  }

  //| memory management:
  deepfree_value(pair.first);
  deepfree_value(pair.second);

  ms->term = BoolValue(result);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_Push(MachineStateT *ms)
{
  //| (x, Push :: c, st) -> (x, c, Val(x) :: st)
  ValueT *x = ms->term;
  StackT *stack = ms->stack;
  ValueT *cloned_x = deepcopy_value(x);

  //| ms->term unchanged
  ms->code += 1;
  ms->stack = ValueOnStack(cloned_x, stack);
  return AllOk;
}

enum Status exec_Cons(MachineStateT *ms)
{
  //| (x, Cons :: c, Val(y) :: st) -> (PairV(y, x), c, st)
  StackT *stack = ms->stack;

  ValueOnStackT pattern;
  if (match_stacktop_with_value(stack, &pattern) == Failure) {
    return Crashed_Cons_NoValueOnStack;
  }
  ValueT *x = ms->term;

  ms->term = PairValue(pattern.top, x);
  ms->code += 1;
  ms->stack = pattern.bottom;
  return AllOk;
}

enum Status exec_QuoteBool(MachineStateT *ms)
{
  //| (_, QuoteBool(v) :: c, st) -> (BoolV(v), c, st)
  deepfree_value(ms->term);
  long v = ms->code[1].data;

  ms->term = BoolValue(v);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_QuoteInt(MachineStateT *ms)
{
  //| (_, QuoteInt(v) :: c, st) -> (IntV(v), c, st)
  deepfree_value(ms->term);
  long v = ms->code[1].data;

  ms->term = IntValue(v);
  ms->code += 2;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_Swap(MachineStateT *ms)
{
  //| (x, Swap :: c, Val(y) :: st) -> (y, c, Val (x) :: st)
  StackT *stack = ms->stack;

  ValueOnStackT pattern;
  if (match_stacktop_with_value(stack, &pattern) == Failure) {
    return Crashed_Swap_NoValueOnStack;
  }
  ValueT *x = ms->term;

  ms->term = pattern.top;
  ms->code += 1;
  ms->stack = ValueOnStack(x, pattern.bottom);
  return AllOk;
}

enum Status exec_Curry(MachineStateT *ms)
{
  //| (x, Curry (closure_code) :: c, st) -> (ClosureV(closure_code, x), c, st)
  CodeT *closure_code = ms->code[1].reference;
  ValueT *x = ms->term;

  ms->term = ClosureValue(closure_code, x);
  ms->code += 2;
  //| ms-stack unchanged
  return AllOk;
}

enum Status exec_Apply(MachineStateT *ms)
{
  //| (PairV(ClosureV(new_code, y), z), Apply :: old_code, st)
  //| -> (PairV(y, z), new_code, Cod(old_code) :: st)
  ValueT *term = ms->term;

  PairT pair;
  if (match_value_with_pair(term, &pair) == Failure) {
    return Crashed_CannotApply;
  }
  ClosureT closure;
  if (match_value_with_closure(pair.first, &closure) == Failure) {
    //| reset the machine state
    ms->term = PairValue(pair.first, pair.second);
    return Crashed_CannotApply;
  }

  ValueT *z = pair.second;
  ValueT *y = closure.value;
  CodeT *new_code = closure.code;
  StackT *stack = ms->stack;
  CodeT *old_code = ms->code + 1;

  ms->term = PairValue(y, z);
  ms->code = new_code;
  ms->stack = CodeOnStack(old_code, stack);
  return AllOk;
}

enum Status exec_Return(MachineStateT *ms)
{
  //| (x, Return :: c, Cod(new_code) :: st) -> (x, new_code, st)
  StackT *stack = ms->stack;

  CodeOnStackT pattern;
  if (match_stacktop_with_code(stack, &pattern) == Failure) {
    return Crashed_CannotReturn;
  }

  // ms->term unchanged
  ms->code = pattern.top;
  ms->stack = pattern.bottom;
  return AllOk;
}

enum Status exec_Branch(MachineStateT *ms)
{
  //| (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st)
  //| -> (x, (if b then if_then else if_else), Cod(c) :: st)
  ValueT *term = ms->term;
  StackT *stack = ms->stack;

  long b;
  if (match_value_with_boolean(term, &b) == Failure) {
    return Crashed_Branch_NotABoolean;
  }
  ValueOnStackT pattern;
  if (match_stacktop_with_value(stack, &pattern) == Failure) {
    //| reset the machine state
    ms->term = BoolValue(b);
    return Crashed_Branch_NoValueOnStack;
  }

  CodeT *code = ms->code;
  CodeT *if_then = code[1].reference;
  CodeT *if_else = code[2].reference;
  CodeT *c = code + 3;

  ms->term = pattern.top;
  ms->code = (b ? if_then : if_else);
  ms->stack = CodeOnStack(c, pattern.bottom);
  return AllOk;
}

enum Status exec_Call(MachineStateT *ms)
{
  //| (x, Call(ref) :: c, st) -> (x, ref, Cod(c) :: st)
  CodeT *ref = ms->code[1].reference;
  CodeT *c = ms->code + 2;
  StackT *st = ms->stack;

  //| ms->term unchanged
  ms->code = ref;
  ms->stack = CodeOnStack(c, st);
  return AllOk;
}

enum Status exec_QuoteEmptyList(MachineStateT *ms)
{
  //| (_, QuoteEmptyList :: c, st) -> ([], c, st)
  deepfree_value(ms->term);
  long v = ms->code[1].data;

  ms->term = EmptyListValue(v);
  ms->code += 1;
  // ms->stack unchanged
  return AllOk;
}

enum Status exec_MakeList(MachineStateT *ms)
{
  //| (ListV (tail), MakeList :: c, Val(head) :: st, fds)
  //|   -> (ListV(ListConsV (head, tail)), c, st, fds)
  //| (_, MakeList :: c, Val(head) :: st, fds)
  //|   -> failwith "CompilerBug: can't construct list: tail is not a list"
  //| (_, MakeList :: c, st, fds)
  //|   -> failwith "CompilerBug: can't construct list: stacktop is not a value"
  StackT *stack = ms->stack;
  ValueT *tail = ms->term;

  if (!!! value_is_list(tail)) return Crashed_MakeList_NotAList;

  ValueOnStackT pattern;
  if (match_stacktop_with_value(stack, &pattern) == Failure) {
    return Crashed_MakeList_NoValueOnStack;
  }

  ms->term = ListConsValue(pattern.top, tail);
  ms->code += 1;
  ms->stack = pattern.bottom;
  return AllOk;
}

enum Status exec_Test(MachineStateT *ms)
{
  int operation = ms->code[1].operation;

  switch (operation) {
    case TestIsEmpty:
    {
      //| new_term = (term == [])
      ValueT *term = ms->term;
      enum boole is_empty = (match_value_with_empty_list(term) == Success)
        ? True : False;

      if (!!! is_empty) free_value(term);
      //| ^ because the matching does not free term when it fails

      ms->term = BoolValue(is_empty);
      ms->code += 2;
      //| ms->stack unchanged
      return AllOk;
    }
    break;

    default: return Crashed_Test_Unknown; break;
  } //| end of switch
}


//| utilitary functions:

void print_state(MachineStateT *ms)
{
  if (ms == NULL) {
    printf("<NULL MachineState>" NL);
  }
  else {
    printf("# term = "); print_value(ms->term); printf(NL);
    printf("# code = %p" NL, ms->code);
    printf("# stacktop = "); print_stacktop(ms->stack); printf(NL);
  }
}
