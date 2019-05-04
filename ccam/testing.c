#include <stdio.h>
#include <stdlib.h>
////
#include "ccam.h"

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
