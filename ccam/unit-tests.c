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
  CodeT program[] = { { .instruction = Halt }, };
  assert_execution_went_well(
    "instruction Halt",
    blank_state(program),
    MachineState(NullValue(), program + 1, EmptyStack()),
    Halted
  );
}

void test_Unary()
{
  {
    CodeT program[] = { { .instruction = Unary }, { .data = Fst }, };
    assert_execution_went_aok(
      "instruction Unary(Fst), term = (True, False)",
      MachineState(
        PairValue(BoolValue(True), BoolValue(False)),
        program,
        EmptyStack()
      ),
      MachineState(
        BoolValue(True),
        program + 2,
        EmptyStack()
      )
    );
    assert_execution_crashed(
      "instruction Unary(Fst), term = True",
      MachineState(
        BoolValue(True),
        program,
        EmptyStack()
      ),
      Err__Unary_NotAPair
    );
    assert_execution_crashed(
      "instruction Unary(Fst), term = <NULL>",
      MachineState(
        NULL,
        program,
        EmptyStack()
      ),
      Err__Unary_NotAPair
    );
  }
  {
    CodeT program[] = { { .instruction = Unary }, { .data = Snd }, };
    assert_execution_went_aok(
      "instruction Unary(Snd), term = (True, False)",
      MachineState(
        PairValue(BoolValue(True), BoolValue(False)),
        program,
        EmptyStack()
      ),
      MachineState(
        BoolValue(False),
        program + 2,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = {{.instruction = Unary}, {.data = Head}};
    assert_execution_went_aok(
      "instruction Unary(Head)",
      MachineState(
        ListConsValue(IntValue(3L), NULL),
        program,
        NULL
      ),
      MachineState(
        IntValue(3L),
        program + 2,
        NULL
      )
    );
    assert_execution_went_aok(
      "instruction Unary(Head), 2",
      MachineState(
        ListConsValue(IntValue(3L), EmptyListValue()),
        program,
        CodeOnStack(CodeRef(1L), NULL)
      ),
      MachineState(
        IntValue(3L),
        program + 2,
        CodeOnStack(CodeRef(1L), NULL)
      )
    );
    assert_execution_crashed(
      "instruction Unary(Head), term = []",
      MachineState(
        EmptyListValue(),
        program,
        NULL
      ),
      Err__Unary_Headless
    );
    assert_execution_crashed(
      "instruction Unary(Head), term = ()",
      MachineState(
        NullValue(),
        program,
        NULL
      ),
      Err__Unary_Headless
    );
    assert_execution_crashed(
      "instruction Unary(Head), term = <NULL>",
      MachineState(
        NULL,
        program,
        NULL
      ),
      Err__Unary_Headless
    );
  }
  {
    CodeT program[] = {{.instruction = Unary}, {.data = Tail}};
    assert_execution_went_aok(
      "instruction Unary(Tail)",
      MachineState(
        ListConsValue(IntValue(3L), NULL),
        program,
        NULL
      ),
      MachineState(
        NULL,
        program + 2,
        NULL
      )
    );
    assert_execution_went_aok(
      "instruction Unary(Tail), 2",
      MachineState(
        ListConsValue(IntValue(3L), EmptyListValue()),
        program,
        CodeOnStack(CodeRef(1L), NULL)
      ),
      MachineState(
        EmptyListValue(),
        program + 2,
        CodeOnStack(CodeRef(1L), NULL)
      )
    );
    assert_execution_went_aok(
      "instruction Unary(Tail), 3",
      MachineState(
        ListConsValue(IntValue(3L), ListConsValue(IntValue(1L), NULL)),
        program,
        NULL
      ),
      MachineState(
        ListConsValue(IntValue(1L), NULL),
        program + 2,
        NULL
      )
    );
    assert_execution_crashed(
      "instruction Unary(Tail), term = []",
      MachineState(
        EmptyListValue(),
        program,
        NULL
      ),
      Err__Unary_Headless
    );
    assert_execution_crashed(
      "instruction Unary(Tail), term = 3",
      MachineState(
        IntValue(3L),
        program,
        NULL
      ),
      Err__Unary_Headless
    );
    assert_execution_crashed(
      "instruction Unary(Tail), term = <NULL>",
      MachineState(
        NULL,
        program,
        NULL
      ),
      Err__Unary_Headless
    );
  }
  {
    CodeT program[] = { { .instruction = Unary }, { .data = 42 }, };
    assert_execution_crashed(
      "instruction Unary(42)",
      MachineState(
        PairValue(BoolValue(True), BoolValue(False)),
        program,
        EmptyStack()
      ),
      Err__Unary_Unknown
    );
  }
}

void test_Arith()
{
  {
    CodeT program[] = { { .instruction = Arith }, { .data = Div }, };
    assert_execution_went_aok(
      "instruction Arith(Div), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program,
        EmptyStack()
      ),
      MachineState(
        IntValue(14L),
        program + 2,
        EmptyStack()
      )
    );
    assert_execution_crashed(
      "instruction Arith(Div), term = (42, 0)",
      MachineState(
        PairValue(IntValue(42L), IntValue(0L)),
        program,
        EmptyStack()
      ),
      Err__Arith_DivByZero
    );
  }
  {
    CodeT program[] = { { .instruction = Arith }, { .data = Mod }, };
    assert_execution_crashed(
      "instruction Arith(Mod), term = (42, 0)",
      MachineState(
        PairValue(IntValue(42L), IntValue(0L)),
        program,
        EmptyStack()
      ),
      Err__Arith_DivByZero
    );
  }
  {
    CodeT program[] = { { .instruction = Arith }, { .data = Plus }, };
    assert_execution_crashed(
      "instruction Arith(Plus), term = (42, ())",
      MachineState(
        PairValue(IntValue(42L), NullValue()),
        program,
        EmptyStack()
      ),
      Err__Arith_TypeError
    );
  }
  {
    CodeT program[] = { { .instruction = Arith }, { .data = 123L }, };
    assert_execution_crashed(
      "instruction Arith(123), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program,
        EmptyStack()
      ),
      Err__Arith_Unknown
    );
  }
}

void test_Compare()
{
  {
    CodeT program[] = { { .instruction = Compare }, { .data = Ge }, };
    assert_execution_went_aok(
      "instruction Compare(Geq), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program,
        EmptyStack()
      ),
      MachineState(
        BoolValue(True),
        program + 2,
        EmptyStack()
      )
    );
    assert_execution_went_aok(
      "instruction Compare(Ge), term = (False, True)",
      MachineState(
        PairValue(BoolValue(False), BoolValue(True)),
        program,
        EmptyStack()
      ),
      MachineState(
        BoolValue(False),
        program + 2,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .instruction = Compare }, { .data = Lt }, };
    assert_execution_went_aok(
      "instruction Compare(Lt), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program,
        EmptyStack()
      ),
      MachineState(
        BoolValue(False),
        program + 2,
        EmptyStack()
      )
    );
  }
  {
    CodeT program[] = { { .instruction = Compare }, { .data = Neq }, };
    assert_execution_crashed(
      "instruction Compare(Neq), term = (42, True)",
      MachineState(
        PairValue(IntValue(42L), BoolValue(True)),
        program,
        EmptyStack()
      ),
      Err__Compare_TypeError
    );
    assert_execution_crashed(
      "instruction Compare(Neq), term = (42, ())",
      MachineState(
        PairValue(IntValue(42L), NullValue()),
        program,
        EmptyStack()
      ),
      Err__Compare_TypeError
    );
  }
  {
    CodeT program[] = { { .instruction = Compare }, { .data = 123L }, };
    assert_execution_crashed(
      "instruction Compare(123), term = (42, 3)",
      MachineState(
        PairValue(IntValue(42L), IntValue(3L)),
        program,
        EmptyStack()
      ),
      Err__Compare_Unknown
    );
  }
}

void test_Push()
{
  CodeT program[] = { { .instruction = Push }, };
  assert_execution_went_aok(
    "instruction Push, term = ((), [3, True])",
    MachineState(
      PairValue(
        NullValue(),
        ListConsValue(
          IntValue(3),
          ListConsValue(BoolValue(True), EmptyListValue()))),
      program,
      EmptyStack()),
    MachineState(
      PairValue(
        NullValue(),
        ListConsValue(
          IntValue(3),
          ListConsValue(BoolValue(True), EmptyListValue()))),
      program + 1,
      ValueOnStack(
        PairValue(
          NullValue(),
          ListConsValue(
            IntValue(3),
            ListConsValue(BoolValue(True), EmptyListValue()))),
        EmptyStack())));
}

void test_Cons()
{
  CodeT program[] = { { .instruction = Cons }, };
  assert_execution_went_aok(
    "instruction Cons",
    MachineState(
      PairValue(IntValue(3L), BoolValue(True)),
      program,
      ValueOnStack(IntValue(200L), EmptyStack())
    ),
    MachineState(
      PairValue(IntValue(200L), PairValue(IntValue(3L), BoolValue(True))),
      program + 1,
      EmptyStack()
    )
  );
  assert_execution_crashed(
    "instruction Cons, no value on stack",
    MachineState(
      PairValue(IntValue(3L), BoolValue(True)),
      program,
      CodeOnStack(CodeRef(42L), EmptyStack())
    ),
    Err__Cons_NoValueOnStack
  );
}

void test_QuoteBool()
{
  CodeT program[] = { { .instruction = QuoteBool }, { .data = True }, };
  assert_execution_went_aok(
    "instruction QuoteBool",
    MachineState(
      NULL,
      program,
      ValueOnStack(IntValue(200L), EmptyStack())
    ),
    MachineState(
      BoolValue(True),
      program + 2,
      ValueOnStack(IntValue(200L), EmptyStack())
    )
  );
}

void test_QuoteInt()
{
  CodeT program[] = { { .instruction = QuoteInt }, { .data = 123L }, };
  assert_execution_went_aok(
    "instruction QuoteInt",
    MachineState(
      NULL,
      program,
      ValueOnStack(IntValue(200L), EmptyStack())
    ),
    MachineState(
      IntValue(123L),
      program + 2,
      ValueOnStack(IntValue(200L), EmptyStack())
    )
  );
}

void test_Swap()
{
  CodeT program[] = { { .instruction = Swap }, };
  assert_execution_went_aok(
    "instruction Swap",
    MachineState(
      IntValue(888L),
      program,
      ValueOnStack(PairValue(BoolValue(False), NullValue()), EmptyStack())
    ),
    MachineState(
      PairValue(BoolValue(False), NullValue()),
      program + 1,
      ValueOnStack(IntValue(888L), EmptyStack())
    )
  );
  assert_execution_crashed(
    "instruction Swap, no value on stack",
    MachineState(
      IntValue(888L),
      program,
      CodeOnStack(CodeRef(42L), EmptyStack())
    ),
    Err__Swap_NoValueOnStack
  );
  assert_execution_crashed(
    "instruction Swap, stack is <NULL>",
    MachineState(
      IntValue(888L),
      program,
      NULL
    ),
    Err__Swap_NoValueOnStack
  );
}

void test_Curry()
{
  CodeT program[] = {
    { .instruction = Curry }, { .reference = CodeRef(123456L) },
  };
  assert_execution_went_aok(
    "instruction Curry",
    MachineState(
      IntValue(111L),
      program,
      NULL
    ),
    MachineState(
      ClosureValue(CodeRef(123456L), IntValue(111L)),
      program + 2,
      NULL
    )
  );
  assert_execution_went_aok(
    "instruction Curry, term = <NULL>",
    MachineState(
      NULL,
      program,
      NULL
    ),
    MachineState(
      ClosureValue(CodeRef(123456L), NULL),
      program + 2,
      NULL
    )
  );
}

void test_Apply()
{
  CodeT program[] = { { .instruction = Apply }, };
  assert_execution_went_aok(
    "instruction Apply, term = (Closure(@456789, 100), ())",
    MachineState(
      PairValue(
        ClosureValue(CodeRef(456789L), IntValue(100L)), NullValue()
      ),
      program,
      ValueOnStack(IntValue(3L), EmptyStack())
    ),
    MachineState(
      PairValue(
        IntValue(100L), NullValue()
      ),
      CodeRef(456789L),
      CodeOnStack(
        program + 1, ValueOnStack(IntValue(3L), EmptyStack())
      )
    )
  );
  assert_execution_went_aok(
    "instruction Apply, stack = <NULL>",
    MachineState(
      PairValue(
        ClosureValue(CodeRef(456789L), IntValue(100L)), NullValue()
      ),
      program,
      NULL
    ),
    MachineState(
      PairValue(
        IntValue(100L), NullValue()
      ),
      CodeRef(456789L),
      CodeOnStack(
        program + 1, NULL
      )
    )
  );
  assert_execution_crashed(
    "instruction Apply, term = ((456789, 100), ())",
    MachineState(
      PairValue(
        PairValue(IntValue(456789L), IntValue(100L)), NullValue()
      ),
      program,
      NULL
    ),
    Err__CannotApply
  );
  assert_execution_crashed(
    "instruction Apply, term = (True, ())",
    MachineState(
      PairValue(BoolValue(100L), NullValue()),
      program,
      NULL
    ),
    Err__CannotApply
  );
  assert_execution_crashed(
    "instruction Apply, term = True",
    MachineState(
      BoolValue(100L),
      program,
      NULL
    ),
    Err__CannotApply
  );
}

void test_Return()
{
  CodeT program[] = { { .instruction = Return }, };
  assert_execution_went_aok(
    "instruction Return",
    MachineState(
      NULL,
      program,
      CodeOnStack(CodeRef(42L), ValueOnStack(IntValue(100L), EmptyStack()))
    ),
    MachineState(
      NULL,
      CodeRef(42L),
      ValueOnStack(IntValue(100L), EmptyStack())
    )
  );
  assert_execution_went_aok(
    "instruction Return, 2",
    MachineState(
      IntValue(1000L),
      program,
      CodeOnStack(CodeRef(42L), NULL)
    ),
    MachineState(
      IntValue(1000L),
      CodeRef(42L),
      NULL
    )
  );
  assert_execution_crashed(
    "instruction Return, stack = <NULL>",
    MachineState(
      IntValue(1000L),
      program,
      NULL
    ),
    Err__CannotReturn
  );
  assert_execution_crashed(
    "instruction Return, no code on stack",
    MachineState(
      IntValue(1000L),
      program,
      ValueOnStack(IntValue(1L), NULL)
    ),
    Err__CannotReturn
  );
}

void test_Branch()
{
  CodeT program[] = {
    { .instruction = Branch },
      { .reference = CodeRef(100L) },
      { .reference = CodeRef(200L) },
  };
  assert_execution_went_aok(
    "instruction Branch, term = True",
    MachineState(
      BoolValue(True),
      program,
      ValueOnStack(IntValue(3L), ValueOnStack(IntValue(4L), NULL))
    ),
    MachineState(
      IntValue(3L),
      CodeRef(100L),
      CodeOnStack(program + 3, ValueOnStack(IntValue(4L), NULL))
    )
  );
  assert_execution_went_aok(
    "instruction Branch, term = False",
    MachineState(
      BoolValue(False),
      program,
      ValueOnStack(IntValue(3L), ValueOnStack(IntValue(4L), NULL))
    ),
    MachineState(
      IntValue(3L),
      CodeRef(200L),
      CodeOnStack(program + 3, ValueOnStack(IntValue(4L), NULL))
    )
  );
  assert_execution_crashed(
    "instruction Branch, term not a boolean",
    MachineState(
      NullValue(),
      program,
      ValueOnStack(IntValue(3L), ValueOnStack(IntValue(4L), NULL))
    ),
    Err__Branch_NotABoolean
  );
  assert_execution_crashed(
    "instruction Branch, term = <NULL>",
    MachineState(
      NULL,
      program,
      ValueOnStack(IntValue(3L), ValueOnStack(IntValue(4L), NULL))
    ),
    Err__Branch_NotABoolean
  );
  assert_execution_crashed(
    "instruction Branch, no value on stack",
    MachineState(
      BoolValue(False),
      program,
      CodeOnStack(NULL, NULL)
    ),
    Err__Branch_NoValueOnStack
  );
  assert_execution_crashed(
    "instruction Branch, stack = <NULL>",
    MachineState(
      BoolValue(False),
      program,
      NULL
    ),
    Err__Branch_NoValueOnStack
  );
}

void test_Call()
{
  CodeT program[] = {
    { .instruction = Call }, { .reference = CodeRef(123456L) },
  };
  assert_execution_went_aok(
    "instruction Call",
    MachineState(
      IntValue(1111L),
      program,
      ValueOnStack(BoolValue(True), NULL)
    ),
    MachineState(
      IntValue(1111L),
      CodeRef(123456L),
      CodeOnStack(program + 2, ValueOnStack(BoolValue(True), NULL))
    )
  );
  assert_execution_went_aok(
    "instruction Call, 2",
    MachineState(
      NULL,
      program,
      ValueOnStack(BoolValue(True), NULL)
    ),
    MachineState(
      NULL,
      CodeRef(123456L),
      CodeOnStack(program + 2, ValueOnStack(BoolValue(True), NULL))
    )
  );
  assert_execution_went_aok(
    "instruction Call, 3",
    MachineState(
      NULL,
      program,
      NULL
    ),
    MachineState(
      NULL,
      CodeRef(123456L),
      CodeOnStack(program + 2, NULL)
    )
  );
}

void test_QuoteEmptyList()
{
  CodeT program[] = { { .instruction = QuoteEmptyList } };
  assert_execution_went_aok(
    "instruction QuoteEmptyList",
    MachineState(
      PairValue(NULL, NULL),
      program,
      CodeOnStack(NULL, NULL)
    ),
    MachineState(
      EmptyListValue(),
      program + 1,
      CodeOnStack(NULL, NULL)
    )
  );
}

void test_MakeList()
{
  CodeT program[] = {{ .instruction = MakeList }};
  assert_execution_went_aok(
    "instruction MakeList",
    MachineState(
      EmptyListValue(),
      program,
      ValueOnStack(IntValue(3L), NULL)
    ),
    MachineState(
      ListConsValue(IntValue(3L), EmptyListValue()),
      program + 1,
      NULL
    )
  );
  assert_execution_went_aok(
    "instruction MakeList, 2",
    MachineState(
      ListConsValue(NULL, NULL),
      program,
      ValueOnStack(IntValue(3L), NULL)
    ),
    MachineState(
      ListConsValue(IntValue(3L), ListConsValue(NULL, NULL)),
      program + 1,
      NULL
    )
  );
  assert_execution_crashed(
    "instruction MakeList, term not a list",
    MachineState(
      IntValue(1L),
      program,
      ValueOnStack(IntValue(3L), NULL)
    ),
    Err__MakeList_NotAList
  );
  assert_execution_crashed(
    "instruction MakeList, term = <NULL>",
    MachineState(
      NULL,
      program,
      ValueOnStack(IntValue(3L), NULL)
    ),
    Err__MakeList_NotAList
  );
  assert_execution_crashed(
    "instruction MakeList, no value on stack",
    MachineState(
      EmptyListValue(),
      program,
      CodeOnStack(CodeRef(3L), NULL)
    ),
    Err__MakeList_NoValueOnStack
  );
  assert_execution_crashed(
    "instruction MakeList, stack = <NULL>",
    MachineState(
      EmptyListValue(),
      program,
      NULL
    ),
    Err__MakeList_NoValueOnStack
  );
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
  test_Cons();
  test_QuoteBool();
  test_QuoteInt();
  test_Swap();
  test_Curry();
  test_Apply();
  test_Return();
  test_Branch();
  test_Call();
  test_QuoteEmptyList();
  test_MakeList();
  return 0;
}
