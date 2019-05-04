#include <stdio.h>
#include <stdlib.h>
////
#include "ccam.h"
#include "testing.h"


void test_BoolValue()
{
  {
    long b0 = False;
    ValueT *v = BoolValue(b0);
    long b1;
    enum result r = match_value_with_boolean(v, &b1);
    assert("Bool(False)", b1 == b0 && r == Success);
  }
  {
    long b0 = True;
    ValueT *v = BoolValue(b0);
    long b1;
    enum result r = match_value_with_boolean(v, &b1);
    assert("Bool(True)", b1 == b0 && r == Success);
  }
}

void test_PairValue()
{
  {
    ValueT *pair = PairValue(BoolValue(0), BoolValue(1));
    PairT p;
    enum result r = match_value_with_pair(pair, &p);
    assert("PairValue(b, b)",
      r == Success && p.first->as.boolean == 0 && p.second->as.boolean == 1);
  }
  {
    ValueT *pair = PairValue(PairValue(BoolValue(1), BoolValue(0)), BoolValue(0));
    PairT p;
    enum result r = match_value_with_pair(pair, &p);
    assert("PairValue(PairValue(b, b), b)",
      r == Success && p.first->tag == ValueIsPair && p.second->as.boolean == 0);
  }
}

void test_NullValue()
{
  ValueT *nullv = NullValue();
  assert("Null()", nullv->tag == ValueIsNull);
}

void test_ClosureValue()
{
  ValueT *p = PairValue(BoolValue(True), BoolValue(False));
  ValueT *closure = ClosureValue((CodeT *)42L, p);
  ClosureT c;
  enum result r = match_value_with_closure(closure, &c);
  assert("ClosureValue(42, PairValue(True, False))",
    r == Success && c.value == p && c.code == (CodeT *)42L);
}

void test_IntValue()
{
  ValueT *integer = IntValue(43L);
  long i;
  enum result r = match_value_with_integer(integer, &i);
  assert("Int(43)", r == Success && i == 43L);
}

void test_EmptyList()
{
  ValueT *empty = EmptyListValue();
  int b = value_is_list(empty)
    && empty == &the_empty_list
    && equal_values(empty, EmptyListValue());
  free_value(empty); //| trying to (pretend-)free it, must not fail
  assert("[]", b);
}


void test_Halt()
{
  CodeT program[] = { { .data = 42L },
    { .instruction = Halt },
    { .instruction = Swap } };
  assert_execution_went_well(
    "instruction Halt",
    blank_state(program + 1),
    MachineState(NullValue(), program + 2, EmptyStack()),
    Halted
  );
}

void test_Unary()
{
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Unary }, { .data = Fst },
      { .instruction = Swap } };
    assert_execution_went_aok(
      "instruction Unary(Fst), term = (True, False)",
      MachineState(
        PairValue(BoolValue(True), BoolValue(False)),
        program + 1,
        EmptyStack()
      ),
      MachineState(
        BoolValue(True),
        program + 3,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Unary }, { .data = Snd },
      { .instruction = Swap } };
    assert_execution_went_aok(
      "instruction Unary(Snd), term = (True, False)",
      MachineState(
        PairValue(BoolValue(True), BoolValue(False)),
        program + 1,
        EmptyStack()
      ),
      MachineState(
        BoolValue(False),
        program + 3,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Unary }, { .data = 42 },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Unary(42)",
      MachineState(
        PairValue(BoolValue(True), BoolValue(False)),
        program + 1,
        EmptyStack()
      ),
      Err__Unary_Unknown
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Unary }, { .data = Fst },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Unary(Fst), term = True",
      MachineState(
        BoolValue(True),
        program + 1,
        EmptyStack()
      ),
      Err__Unary_NotAPair
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Unary }, { .data = Fst },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Unary(Fst), term = <NULL>",
      MachineState(
        NULL,
        program + 1,
        EmptyStack()
      ),
      Err__Unary_NotAPair
    );
  }
}

void test_Arith()
{
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Arith }, { .data = Div },
      { .instruction = Swap } };
    assert_execution_went_aok(
      "instruction Arith(Div), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program + 1,
        EmptyStack()
      ),
      MachineState(
        IntValue(14L),
        program + 3,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Arith }, { .data = Div },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Arith(Div), term = (42, 0)",
      MachineState(
        PairValue(IntValue(42L), IntValue(0L)),
        program + 1,
        EmptyStack()
      ),
      Err__Arith_DivByZero
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Arith }, { .data = Mod },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Arith(Mod), term = (42, 0)",
      MachineState(
        PairValue(IntValue(42L), IntValue(0L)),
        program + 1,
        EmptyStack()
      ),
      Err__Arith_DivByZero
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Arith }, { .data = Plus },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Arith(Plus), term = (42, ())",
      MachineState(
        PairValue(IntValue(42L), NullValue()),
        program + 1,
        EmptyStack()
      ),
      Err__Arith_TypeError
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Arith }, { .data = 123L },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Arith(123), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program + 1,
        EmptyStack()
      ),
      Err__Arith_Unknown
    );
  }
}


void test_Compare()
{
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Compare }, { .data = Ge },
      { .instruction = Swap } };
    assert_execution_went_aok(
      "instruction Compare(Geq), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program + 1,
        EmptyStack()
      ),
      MachineState(
        BoolValue(True),
        program + 3,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Compare }, { .data = Lt },
      { .instruction = Swap } };
    assert_execution_went_aok(
      "instruction Compare(Lt), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program + 1,
        EmptyStack()
      ),
      MachineState(
        BoolValue(False),
        program + 3,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Compare }, { .data = Ge },
      { .instruction = Swap } };
    assert_execution_went_aok(
      "instruction Compare(Ge), term = (False, True)",
      MachineState(
        PairValue(BoolValue(False), BoolValue(True)),
        program + 1,
        EmptyStack()
      ),
      MachineState(
        BoolValue(False),
        program + 3,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Compare }, { .data = Neq },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Compare(Neq), term = (42, True)",
      MachineState(
        PairValue(IntValue(42L), BoolValue(True)),
        program + 1,
        EmptyStack()
      ),
      Err__Compare_TypeError
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Compare }, { .data = Neq },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Compare(Neq), term = (42, ())",
      MachineState(
        PairValue(IntValue(42L), NullValue()),
        program + 1,
        EmptyStack()
      ),
      Err__Compare_TypeError
    );
  }
  {
    CodeT program[] = { { .data = 42L },
      { .instruction = Compare }, { .data = 123L },
      { .instruction = Swap } };
    assert_execution_crashed(
      "instruction Compare(123), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program + 1,
        EmptyStack()
      ),
      Err__Compare_Unknown
    );
  }
}

void test_Push()
{
  CodeT program[] = { { .data = 42L },
    { .instruction = Push },
    { .instruction = Swap } };
  assert_execution_went_aok(
    "instruction Push, term = ((), [3, True])",
    MachineState(
      PairValue(
        NullValue(),
        ListConsValue(
          IntValue(3),
          ListConsValue(BoolValue(True), EmptyListValue()))),
      program + 1,
      EmptyStack()),
    MachineState(
      PairValue(
        NullValue(),
        ListConsValue(
          IntValue(3),
          ListConsValue(BoolValue(True), EmptyListValue()))),
      program + 2,
      ValueOnStack(
        PairValue(
          NullValue(),
          ListConsValue(
            IntValue(3),
            ListConsValue(BoolValue(True), EmptyListValue()))),
        EmptyStack())));
}

int main()
{
  test_BoolValue();
  test_PairValue();
  test_NullValue();
  test_ClosureValue();
  test_IntValue();
  test_EmptyList();

  test_Halt();
  test_Unary();
  test_Arith();
  test_Compare();
  test_Push();
  return 0;
}
