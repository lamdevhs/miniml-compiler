#include <stdio.h>
#include <stdlib.h>
////
#include "virtual-machine.h"


void assert(char *str, int boolean) {
  if (!boolean) {
    printf("[ERROR] %s" NL, str);
    exit(1);
  }
#ifdef VERBOSE
  else printf("[ok] %s" NL, str);
#endif
}


void test_value() {
  enum Status status = AllOk;
  
  //| -------- testing value_Bool
  {
    long b0 = False;
    ValueT *bv = value_Bool(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("Bool(False)", b1 == b0 && status == AllOk);
  }
  {
    long b0 = True;
    ValueT *bv = value_Bool(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("Bool(True)", b1 == b0 && status == AllOk);
  }

  //| -------- testing value_Pair
  {
    ValueT *pair = value_Pair(value_Bool(0), value_Bool(1));
    PairT p = match_value_with_pair(pair, &status);
    assert("PairValue(b, b)", p.first->as.boolean == 0 && p.second->as.boolean == 1);
  }
  {
    ValueT *pair = value_Pair(value_Pair(value_Bool(1), value_Bool(0)), value_Bool(0));
    PairT p = match_value_with_pair(pair, &status);
    assert("PairValue(PairValue(b, b), b)",
      p.first->tag == ValueIsPair && p.second->as.boolean == 0 && status == AllOk);
  }

  //| -------- testing value_Null
  {
    ValueT *nullv = value_Null();
    assert("Null()", nullv->tag == ValueIsNull);
  }

  //| -------- testing value_Closure
  {
    ValueT *p = value_Pair(value_Bool(True), value_Bool(False));
    ValueT *closure = value_Closure((CodeT *)42L, p);
    ClosureT c = match_value_with_closure(closure, &status);
    assert("Closure(42, PairValue(True, False))",
      c.value == p && c.code == (CodeT *)42L && status == AllOk);
  }

  //| -------- testing value_Integer
  {
    ValueT *integer = value_Int(43L);
    long i = match_value_with_integer(integer, &status);
    assert("Int(43)", i == 43L && status == AllOk);
  }

  // ValueT *all_values[] = {
  //   NULL, value_Null(), value_Bool(True), value_Int(55L),
  //   value_Pair(value_Null(), value_Null()),
  //   value_Closure((CodeT *)123L, value_Null())
  // };
  // //| -------- testing match_value_with_boolean
  // {
  //   for (int i = 0; i < 6; i++) {
  //     long b =
}


void exec_went_fine(char *msg, MachineStateT *ms, MachineStateT *expected) {
  enum Status status = exec(ms);
  int condition = status == AllOk && equal_states(ms, expected);
  if (!!! condition) {
    print_state(ms);
    print_status(status); printf(NL);
  }
  assert(msg, condition);
}

void exec_failed(char *msg, MachineStateT *ms, enum Status expected) {
  enum Status status = exec(ms);
  int condition = status == expected;
  if (!!! condition) {
    print_status(status); printf(NL);
  }
  assert(msg, condition);
}

void test_exec() {
  //| -------- testing Halt
  {
    CodeT program[] = { 42L, Halt, Swap };
    MachineStateT *ms = blank_state(program + 1);
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(value_Null(), program + 2, empty_stack());
    assert("instruction Halt", status == Halted && equal_states(expected, ms));
  }
  //| -------- testing Unary
  {
    CodeT program[] = { 42L, Unary, Fst, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Bool(True), value_Bool(False)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    MachineStateT *expected = MachineState(value_Bool(True), program + 3, empty_stack());
    assert("instruction Unary Fst", status == AllOk && equal_states(expected, ms));
  }
  {
    CodeT program[] = { 42L, Unary, Snd, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Bool(True), value_Bool(False)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    MachineStateT *expected = MachineState(value_Bool(False), program + 3, empty_stack());
    assert("instruction Unary Snd", status == AllOk && equal_states(expected, ms));
  }
  {
    CodeT program[] = { 43L, Unary, 42L, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Bool(True), value_Bool(False)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    assert("instruction Unary 42", status == UnknownUnary);
  }
  {
    CodeT program[] = { 43L, Unary, Fst, Swap };
    MachineStateT *ms = MachineState(
      value_Bool(True),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    assert("instruction Unary Fst(Bool)", status == ValueIsNotPair);
  }
  {
    CodeT program[] = { 43L, Unary, Fst, Swap };
    MachineStateT *ms = MachineState(
      NULL,
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    assert("instruction Unary Fst(NULL)", status == MatchNULLValue);
  }
  //| -------- testing Arith
  {
    CodeT program[] = { 42L, Arith, Div, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Int(42L), value_Int(3L)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    MachineStateT *expected = MachineState(value_Int(14L), program + 3, empty_stack());
    assert("instruction Arith Div(42, 3)", status == AllOk && equal_states(expected, ms));
  }
  {
    CodeT program[] = { 42L, Arith, Div, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Int(42L), value_Int(0L)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    assert("instruction Arith Div(42, 0)", status == DivZero);
  }
  {
    CodeT program[] = { 43L, Arith, 42L, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Int(3L), value_Int(4L)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    assert("instruction Arith 42", status == UnknownArith);
  }
  
  //| -------- testing Push
  //| (x, Push :: c, st) -> (x, c, Val(x) :: st)
  {
    CodeT program[] = { 43L, Push, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Int(3L), value_Bool(True)),
      program + 1,
      value_onto_stack(value_Int(200L), empty_stack())
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Pair(value_Int(3L), value_Bool(True)),
      program + 2,
      value_onto_stack(
        value_Pair(value_Int(3L), value_Bool(True)),
        value_onto_stack(
          value_Int(200L),
          empty_stack()
        )
      )
    );
    assert("instruction Push",
      status == AllOk
      && equal_states(expected, ms)
      && (ms->term != ms->stack->as.with_value.top)
    );
  }
  
  //| -------- testing Cons
  //| (x, Cons :: c, Val(y) :: st) -> (PairV(y, x), c, st)
  {
    CodeT program[] = { 43L, Cons, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Int(3L), value_Bool(True)),
      program + 1,
      value_onto_stack(value_Int(200L), empty_stack())
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Pair(
        value_Int(200L),
        value_Pair(value_Int(3L), value_Bool(True))
      ),
      program + 2,
      empty_stack()
    );
    assert("instruction Cons",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  {
    CodeT program[] = { 43L, Cons, Swap };
    MachineStateT *ms = MachineState(
      value_Pair(value_Int(3L), value_Bool(True)),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    assert("instruction Cons with empty stack",
      status == StackHeadIsNotValue
    );
  }
  
  //| -------- testing QuoteBool, QuoteInt
  {
    CodeT program[] = { 43L, QuoteBool, True, Swap };
    MachineStateT *ms = MachineState(
      NULL,
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Bool(True),
      program + 3,
      empty_stack()
    );
    assert("instruction QuoteBool",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  {
    CodeT program[] = { 43L, QuoteInt, -123L, Swap };
    MachineStateT *ms = MachineState(
      NULL,
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Int(-123L),
      program + 3,
      empty_stack()
    );
    assert("instruction QuoteInt",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  
  //| -------- testing Swap
  //| (x, Swap :: c, Val(y) :: st) -> (y, c, Val (x) :: st)
  {
    CodeT program[] = { 43L, Swap };
    MachineStateT *ms = MachineState(
      value_Int(-1L),
      program + 1,
      value_onto_stack(value_Bool(False), empty_stack())
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Bool(False),
      program + 2,
      value_onto_stack(value_Int(-1L), empty_stack())
    );
    assert("instruction Swap",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  {
    CodeT program[] = { 43L, Swap };
    MachineStateT *ms = MachineState(
      value_Int(-1L),
      program + 1,
      code_onto_stack((CodeT *) 24L, value_onto_stack(value_Bool(False), empty_stack()))
    );
    enum Status status = exec(ms);
    
    assert("instruction Swap with code on stack",
      status == StackHeadIsNotValue
    );
  }
  
  //| --------- testing Cur
  //| (x, Cur (closure_code) :: c, st) -> (ClosureV(closure_code, x), c, st)
  {
    CodeT program[] = { -42L, Cur, 123456L, Push };
    MachineStateT *ms = MachineState(
      value_Int(-1L),
      program + 1,
      NULL
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Closure((CodeT *)123456L, value_Int(-1L)),
      program + 3,
      NULL
    );
    assert("instruction Cur",
      status == AllOk
      && equal_states(ms, expected)
    );
  }
  
  //| --------- testing App
  //| (PairV(ClosureV(new_code, y), z), App :: old_code, st)
  //| -> (PairV(y, z), new_code, Cod(old_code) :: st)
  {
    CodeT program[] = { 42L, App, -1L };
    MachineStateT *ms = MachineState(
      value_Pair(
        value_Closure((CodeT *)456789L, value_Int(-100)),
        value_Null()
      ),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      value_Pair(value_Int(-100), value_Null()),
      (CodeT *)456789L,
      code_onto_stack(program + 2, empty_stack())
    );
    assert("instruction App",
      status == AllOk
      && equal_states(ms, expected)
    );
  }
  {
    CodeT program[] = { 42L, App, -1L };
    MachineStateT *ms = MachineState(
      value_Pair(
        value_Int(-100),
        value_Null()
      ),
      program + 1,
      empty_stack()
    );
    enum Status status = exec(ms);
    
    assert("instruction App with no closure",
      status == ValueIsNotClosure
    );
  }
  
  //| -------- testing Return
  //| (x, Return :: c, Cod(new_code) :: st) -> (x, new_code, st)
  {
    CodeT program[] = { 42L, Return, -1L };
    exec_went_fine(
      "instruction Return",
      MachineState(
        NULL,
        program + 1,
        code_onto_stack((CodeT *)192837L, value_onto_stack(value_Int(4L), empty_stack()))
      ),
      MachineState(
        NULL,
        (CodeT *) 192837L,
        value_onto_stack(value_Int(4L), empty_stack())
      )
    );
  }
  {
    CodeT program[] = { 42L, Return, -1L };
    exec_failed(
      "instruction Return with no code on stack",
      MachineState(
        NULL,
        program + 1,
        value_onto_stack(value_Int(4L), empty_stack())
      ),
      StackHeadIsNotCode
    );
  }
  
  //| -------- testing Branch
  //| (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st)
  //| -> (x, (if b then if_then else if_else), Cod(c) :: st)
  {
    CodeT program[] = { 42L, Branch, -123L, 321L, -1L };
    exec_went_fine(
      "instruction Branch(True)",
      MachineState(
        value_Bool(True),
        program + 1,
        value_onto_stack(value_Int(222L), value_onto_stack(value_Bool(False), empty_stack()))
      ),
      MachineState(
        value_Int(222L),
        (CodeT *) -123L,
        code_onto_stack(program + 4, value_onto_stack(value_Bool(False), empty_stack()))
      )
    );
  }
  {
    CodeT program[] = { 42L, Branch, -123L, 321L, -1L };
    exec_went_fine(
      "instruction Branch(False)",
      MachineState(
        value_Bool(False),
        program + 1,
        value_onto_stack(value_Int(222L), value_onto_stack(value_Bool(False), empty_stack()))
      ),
      MachineState(
        value_Int(222L),
        (CodeT *) 321L,
        code_onto_stack(program + 4, value_onto_stack(value_Bool(False), empty_stack()))
      )
    );
  }
  {
    CodeT program[] = { 42L, Branch, -123L, 321L, -1L };
    exec_failed(
      "instruction Branch with no value on stack",
      MachineState(
        value_Bool(False),
        program + 1,
        code_onto_stack(program, value_onto_stack(value_Bool(False), empty_stack()))
      ),
      StackHeadIsNotValue
    );
  }
  {
    CodeT program[] = { 42L, Branch, -123L, 321L, -1L };
    exec_failed(
      "instruction Branch with no boolean term",
      MachineState(
        NULL,
        program + 1,
        value_onto_stack(value_Bool(False), empty_stack())
      ),
      MatchNULLValue
    );
  }
}


int main() {
  test_value();
  test_exec();
  return 0;
}
