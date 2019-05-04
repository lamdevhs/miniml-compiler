#include <stdio.h>
#include <stdlib.h>
////
#include "ccam.h"
#include "testing.h"

void assert(char *description, int success)
{
  if (!!! success) {
    printf("[ERROR] %s" NL, description);
    exit(1);
  }
  else {
    #ifdef VERBOSE
      printf("[ok] %s" NL, description);
    #endif
  }
}

void assert_execution_went_well
(char *description, MachineStateT *ms,
MachineStateT *expected, enum Status expected_status)
{
  enum error_id error = NoError;
  CodeT *starting_point = ms->code;
  enum Status status = execute_next_instruction(ms, &error);

  int success = status == expected_status
    && equal_states(ms, expected)
    && error == NoError;

  if (!!! success) {
    print_state(ms); printf(NL);
    print_status(status); printf(NL);
    print_error(error); printf(NL);
    printf("code shift: %ld" NL, ms->code - starting_point);
  }
  assert(description, success);
}

void assert_execution_went_aok
(char *description, MachineStateT *ms, MachineStateT *expected)
{
  assert_execution_went_well(description, ms, expected, AllOk);
}

void assert_execution_crashed
(char *description, MachineStateT *ms, enum error_id expected_error)
{
  enum error_id error = NoError;
  CodeT *starting_point = ms->code;
  enum Status status = execute_next_instruction(ms, &error);

  int success = status == Crashed
    && error == expected_error;

  if (!!! success) {
    print_state(ms); printf(NL);
    print_status(status); printf(NL);
    print_error(error); printf(NL);
    printf("code shift: %ld" NL, ms->code - starting_point);
  }
  assert(description, success);
}



enum boole equal_states(MachineStateT *a, MachineStateT *b) {
  if (a == NULL) return b == NULL;
  if (b == NULL) return False;
  if (a->code != b->code) return False;
  if (!!! equal_values(a->term, b->term)) return False;
  return equal_stacks(a->stack, b->stack);
}

enum boole equal_stacks(StackT *a, StackT *b) {
  if (a == NULL) return b == NULL;
  if (b == NULL) return False;

  enum StackTag tag = a->tag;
  if (tag != b->tag) return False;
  if (tag == StackTopIsValue) {
    return equal_values(a->as.with_value.top, b->as.with_value.top)
      && equal_stacks(a->as.with_value.bottom, b->as.with_value.bottom);
  }
  if (tag == StackTopIsCode) {
    return a->as.with_code.top == b->as.with_code.top
      && equal_stacks(a->as.with_value.bottom, b->as.with_value.bottom);
  }
  else return True; //| StackIsEmpty, or maybe an invalid tag...
}

enum boole equal_values(ValueT *a, ValueT *b) {
  if (a == NULL) return b == NULL;
  if (b == NULL) return False;

  enum ValueTag tag = a->tag;
  if (tag != b->tag) {
    return False;
  }
  else if (tag == ValueIsInt) {
    return a->as.integer == b->as.integer;
  }
  else if (tag == ValueIsBool) {
    return !! (a->as.boolean) == !! (b->as.boolean);
  }
  else if (tag == ValueIsPair) {
    return equal_values(a->as.pair.first, b->as.pair.first)
      && equal_values(a->as.pair.second, b->as.pair.second);
  }
  else if (tag == ValueIsClosure) {
    return a->as.closure.code == b->as.closure.code
      && equal_values(a->as.closure.value, b->as.closure.value);
  }
  else if (tag == ValueIsListCons) {
    return equal_values(a->as.listcons.head, b->as.listcons.head)
      && equal_values(a->as.listcons.tail, b->as.listcons.tail);
  }
  else if (tag >= ValueTagIsInvalid) return False;
  else return True; //| tags are equal and valid and the value contains nothing
}
