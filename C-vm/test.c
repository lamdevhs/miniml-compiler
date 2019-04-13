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
  
  //| -------- testing BoolValue
  {
    long b0 = False;
    ValueT *bv = BoolValue(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("Bool(False)", b1 == b0 && status == AllOk);
  }
  {
    long b0 = True;
    ValueT *bv = BoolValue(b0);
    long b1 = match_value_with_boolean(bv, &status);
    assert("Bool(True)", b1 == b0 && status == AllOk);
  }

  //| -------- testing PairValue
  {
    ValueT *pair = PairValue(BoolValue(0), BoolValue(1));
    PairT p = match_value_with_pair(pair, &status);
    assert("PairValue(b, b)", p.first->as.boolean == 0 && p.second->as.boolean == 1);
  }
  {
    ValueT *pair = PairValue(PairValue(BoolValue(1), BoolValue(0)), BoolValue(0));
    PairT p = match_value_with_pair(pair, &status);
    assert("PairValue(PairValue(b, b), b)",
      p.first->tag == ValueIsPair && p.second->as.boolean == 0 && status == AllOk);
  }

  //| -------- testing NullValue
  {
    ValueT *nullv = NullValue();
    assert("Null()", nullv->tag == ValueIsNull);
  }

  //| -------- testing ClosureValue
  {
    ValueT *p = PairValue(BoolValue(True), BoolValue(False));
    ValueT *closure = ClosureValue((CodeT *)42L, p);
    ClosureT c = match_value_with_closure(closure, &status);
    assert("ClosureValue(42, PairValue(True, False))",
      c.value == p && c.code == (CodeT *)42L && status == AllOk);
  }

  //| -------- testing IntValue
  {
    ValueT *integer = IntValue(43L);
    long i = match_value_with_integer(integer, &status);
    assert("Int(43)", i == 43L && status == AllOk);
  }

  // ValueT *all_values[] = {
  //   NULL, NullValue(), BoolValue(True), IntValue(55L),
  //   PairValue(NullValue(), NullValue()),
  //   ClosureValue((CodeT *)123L, NullValue())
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
    CodeT program[] = { { .data = 42L }, { .instruction = Halt }, { .instruction = Swap } };
    MachineStateT *ms = blank_state(program + 1);
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(NullValue(), program + 2, EmptyStack());
    assert("instruction Halt", status == Halted && equal_states(expected, ms));
  }
  //| -------- testing Unary
  {
    CodeT program[] = { { .data = 42L }, { .instruction = Unary }, { .data = Fst }, { .instruction = Swap } };
    MachineStateT *ms = MachineState(
      PairValue(BoolValue(True), BoolValue(False)),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    MachineStateT *expected = MachineState(BoolValue(True), program + 3, EmptyStack());
    assert("instruction Unary Fst", status == AllOk && equal_states(expected, ms));
  }
  {
    CodeT program[] = { { .data = 42L }, { .instruction = Unary }, { .data = Snd }, { .instruction = Swap } };
    MachineStateT *ms = MachineState(
      PairValue(BoolValue(True), BoolValue(False)),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    MachineStateT *expected = MachineState(BoolValue(False), program + 3, EmptyStack());
    assert("instruction Unary Snd", status == AllOk && equal_states(expected, ms));
  }
  {
    CodeT program[] = {
      { .data = 43L },
      { .instruction = Unary }, { .operation = 42 },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      PairValue(BoolValue(True), BoolValue(False)),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    assert("instruction Unary 42", status == UnknownUnary);
  }
  {
    CodeT program[] = {
      { .data = 43L },
      { .instruction = Unary }, { .operation = Fst },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      BoolValue(True),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    assert("instruction Unary Fst(Bool)", status == ValueIsNotPair);
  }
  {
    CodeT program[] = {
      { .data = 43L },
      { .instruction = Unary }, { .operation = Fst },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      NULL,
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    assert("instruction Unary Fst(NULL)", status == MatchNULLValue);
  }
  
  //| -------- testing Arith
  {
    CodeT program[] = {
      { .data = 42L },
      { .instruction = Arith }, { .operation = Div },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      PairValue(IntValue(42L), IntValue(3L)),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    MachineStateT *expected = MachineState(IntValue(14L), program + 3, EmptyStack());
    assert("instruction Arith Div(42, 3)", status == AllOk && equal_states(expected, ms));
  }
  {
    CodeT program[] = {
      { .data = 42L },
      { .instruction = Arith }, { .operation = Div },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      PairValue(IntValue(42L), IntValue(0L)),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    assert("instruction Arith Div(42, 0)", status == DivZero);
  }
  {
    CodeT program[] = {
      { .data = 43L },
      { .instruction = Arith },
      { .data = 42L },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      PairValue(IntValue(3L), IntValue(4L)),
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    assert("instruction Arith 42", status == UnknownBinary);
  }
  
  
  //| -------- testing Compare
  {
    CodeT program1[] = {
      {.data = 43L},
      {.instruction = Compare},{.operation = Gt},
      {.instruction = Swap},
    };
    CodeT program2[] = {
      {.data = 43L},
      {.instruction = Compare},{.operation = Le},
      {.instruction = Swap},
    };
    CodeT program3[] = {
      {.data = 43L},
      {.instruction = Compare},{.operation = 42},
      {.instruction = Swap},
    };
    
    {
      exec_went_fine(
        "instruction Compare: 3 > -2",
        MachineState(
          PairValue(IntValue(3), IntValue(-2)),
          program1 + 1,
          EmptyStack()
        ),
        MachineState(
          BoolValue(True),
          program1 + 3,
          EmptyStack()
        )
      );
      
      exec_went_fine(
        "instruction Compare: 3 <= 2",
        MachineState(
          PairValue(IntValue(3), IntValue(-2)),
          program2 + 1,
          EmptyStack()
        ),
        MachineState(
          BoolValue(False),
          program2 + 3,
          EmptyStack()
        )
      );
      
      exec_went_fine(
        "instruction Compare: False > True",
        MachineState(
          PairValue(BoolValue(False), BoolValue(True)),
          program2 + 1,
          EmptyStack()
        ),
        MachineState(
          BoolValue(True),
          program2 + 3,
          EmptyStack()
        )
      );
      
      exec_went_fine(
        "instruction Compare: False <= True",
        MachineState(
          PairValue(BoolValue(False), BoolValue(True)),
          program2 + 1,
          EmptyStack()
        ),
        MachineState(
          BoolValue(True),
          program2 + 3,
          EmptyStack()
        )
      );
      
      exec_failed(
        "instruction Compare: False <= 2",
        MachineState(
          PairValue(BoolValue(False), IntValue(2)),
          program2 + 1,
          EmptyStack()
        ),
        MatchFailure
      );
      
      exec_failed(
        "instruction Compare: -2 <= True",
        MachineState(
          PairValue(IntValue(-2), BoolValue(True)),
          program2 + 1,
          EmptyStack()
        ),
        MatchFailure
      );
      
      exec_failed(
        "instruction Compare: False 42 True",
        MachineState(
          PairValue(BoolValue(False), BoolValue(True)),
          program3 + 1,
          EmptyStack()
        ),
        UnknownBinary
      );
    }
  }
  
  
  //| -------- testing Push
  //| (x, Push :: c, st) -> (x, c, Val(x) :: st)
  {
    CodeT program[] = { { .data = 43L }, { .instruction = Push }, { .instruction = Swap } };
    MachineStateT *ms = MachineState(
      PairValue(IntValue(3L), BoolValue(True)),
      program + 1,
      ValueOnStack(IntValue(200L), EmptyStack())
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      PairValue(IntValue(3L), BoolValue(True)),
      program + 2,
      ValueOnStack(
        PairValue(IntValue(3L), BoolValue(True)),
        ValueOnStack(
          IntValue(200L),
          EmptyStack()
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
    CodeT program[] = { { .data = 43L }, { .instruction = Cons }, { .instruction = Swap } };
    {
      MachineStateT *ms = MachineState(
        PairValue(IntValue(3L), BoolValue(True)),
        program + 1,
        ValueOnStack(IntValue(200L), EmptyStack())
      );
      enum Status status = exec(ms);
      
      MachineStateT *expected = MachineState(
        PairValue(
          IntValue(200L),
          PairValue(IntValue(3L), BoolValue(True))
        ),
        program + 2,
        EmptyStack()
      );
      assert("instruction Cons",
        status == AllOk
        && equal_states(expected, ms)
      );
    }
    {
      MachineStateT *ms = MachineState(
        PairValue(IntValue(3L), BoolValue(True)),
        program + 1,
        EmptyStack()
      );
      enum Status status = exec(ms);
      assert("instruction Cons with empty stack",
        status == StackHeadIsNotValue
      );
    }
  }
  
  //| -------- testing QuoteBool, QuoteInt
  {
    CodeT program[] = {
      { .data = 43L },
      { .instruction = QuoteBool }, { .data = True },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      NULL,
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      BoolValue(True),
      program + 3,
      EmptyStack()
    );
    assert("instruction QuoteBool",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  {
    CodeT program[] = {
      { .data = 43L },
      { .instruction = QuoteInt }, { .data = -123L },
      { .instruction = Swap }
    };
    MachineStateT *ms = MachineState(
      NULL,
      program + 1,
      EmptyStack()
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      IntValue(-123L),
      program + 3,
      EmptyStack()
    );
    assert("instruction QuoteInt",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  
  //| -------- testing Swap
  //| (x, Swap :: c, Val(y) :: st) -> (y, c, Val (x) :: st)
  {
    CodeT program[] = { { .data = 43L }, { .instruction = Swap } };
    MachineStateT *ms = MachineState(
      IntValue(-1L),
      program + 1,
      ValueOnStack(BoolValue(False), EmptyStack())
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      BoolValue(False),
      program + 2,
      ValueOnStack(IntValue(-1L), EmptyStack())
    );
    assert("instruction Swap",
      status == AllOk
      && equal_states(expected, ms)
    );
  }
  {
    CodeT program[] = { { .data = 43L }, { .instruction = Swap } };
    MachineStateT *ms = MachineState(
      IntValue(-1L),
      program + 1,
      CodeOnStack((CodeT *) 24L, ValueOnStack(BoolValue(False), EmptyStack()))
    );
    enum Status status = exec(ms);
    
    assert("instruction Swap with code on stack",
      status == StackHeadIsNotValue
    );
  }
  
  //| --------- testing Cur
  //| (x, Cur (closure_code) :: c, st) -> (ClosureV(closure_code, x), c, st)
  {
    CodeT program[] = {
      { .data = -42L },
      { .instruction = Cur }, { .reference = CodeRef(123456L) },
      { .instruction = Push }
    };
    MachineStateT *ms = MachineState(
      IntValue(-1L),
      program + 1,
      NULL
    );
    enum Status status = exec(ms);
    
    MachineStateT *expected = MachineState(
      ClosureValue((CodeT *)123456L, IntValue(-1L)),
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
    CodeT program[] = { { .data = 42L }, { .instruction = App }, { .data = -1L } };
    {
      MachineStateT *ms = MachineState(
        PairValue(
          ClosureValue((CodeT *)456789L, IntValue(-100)),
          NullValue()
        ),
        program + 1,
        EmptyStack()
      );
      enum Status status = exec(ms);
      
      MachineStateT *expected = MachineState(
        PairValue(IntValue(-100), NullValue()),
        (CodeT *)456789L,
        CodeOnStack(program + 2, EmptyStack())
      );
      assert("instruction App",
        status == AllOk
        && equal_states(ms, expected)
      );
    }
    {
      MachineStateT *ms = MachineState(
        PairValue(
          IntValue(-100),
          NullValue()
        ),
        program + 1,
        EmptyStack()
      );
      enum Status status = exec(ms);
      
      assert("instruction App with no closure",
        status == ValueIsNotClosure
      );
    }
  }
  
  //| -------- testing Return
  //| (x, Return :: c, Cod(new_code) :: st) -> (x, new_code, st)
  {
    CodeT program[] = { { .data = 42L }, { .instruction = Return }, { .data = -1L } };
    exec_went_fine(
      "instruction Return",
      MachineState(
        NULL,
        program + 1,
        CodeOnStack((CodeT *)192837L, ValueOnStack(IntValue(4L), EmptyStack()))
      ),
      MachineState(
        NULL,
        (CodeT *) 192837L,
        ValueOnStack(IntValue(4L), EmptyStack())
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L }, { .instruction = Return }, { .data = -1L } };
    exec_failed(
      "instruction Return with no code on stack",
      MachineState(
        NULL,
        program + 1,
        ValueOnStack(IntValue(4L), EmptyStack())
      ),
      StackHeadIsNotCode
    );
  }
  
  //| -------- testing Branch
  //| (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st)
  //| -> (x, (if b then if_then else if_else), Cod(c) :: st)
  {
    CodeT program[] = {
      { .data = 42L },
      { .instruction = Branch },
        { .reference = CodeRef(-123L) },
        { .reference = CodeRef(321L) },
      { .data = -1L }
    };
    
    {
      exec_went_fine(
        "instruction Branch(True)",
        MachineState(
          BoolValue(True),
          program + 1,
          ValueOnStack(IntValue(222L), ValueOnStack(BoolValue(False), EmptyStack()))
        ),
        MachineState(
          IntValue(222L),
          (CodeT *) -123L,
          CodeOnStack(program + 4, ValueOnStack(BoolValue(False), EmptyStack()))
        )
      );
    }
    {
      exec_went_fine(
        "instruction Branch(False)",
        MachineState(
          BoolValue(False),
          program + 1,
          ValueOnStack(IntValue(222L), ValueOnStack(BoolValue(False), EmptyStack()))
        ),
        MachineState(
          IntValue(222L),
          (CodeT *) 321L,
          CodeOnStack(program + 4, ValueOnStack(BoolValue(False), EmptyStack()))
        )
      );
    }
    {
      exec_failed(
        "instruction Branch with no value on stack",
        MachineState(
          BoolValue(False),
          program + 1,
          CodeOnStack(program, ValueOnStack(BoolValue(False), EmptyStack()))
        ),
        StackHeadIsNotValue
      );
    }
    {
      exec_failed(
        "instruction Branch with no boolean term",
        MachineState(
          NULL,
          program + 1,
          ValueOnStack(BoolValue(False), EmptyStack())
        ),
        MatchNULLValue
      );
    }
  }
  
  //| -------- testing Call
  //| (x, Call(ref) :: c, st) -> (x, ref, Cod(c) :: st)
  {
    CodeT program[] = {
      {.instruction = Call},{.reference = CodeRef(123456L)},
      {.instruction = Halt},
    };
    
    {
      exec_went_fine(
        "instruction Call",
        MachineState(
          NULL,
          program,
          ValueOnStack(BoolValue(True), EmptyStack())
        ),
        MachineState(
          NULL,
          CodeRef(123456L),
          CodeOnStack(program + 2, ValueOnStack(BoolValue(True), EmptyStack()))
        )
      );
    }
  }
}


int main() {
  test_value();
  test_exec();
  return 0;
}
