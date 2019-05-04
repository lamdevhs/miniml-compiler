#ifndef CCAM_TESTING_HEADER
#define CCAM_TESTING_HEADER

#include "ccam.h"

//| testing.c
void assert(char *description, int boolean_value);
void assert_execution_went_well
  (char *description, MachineStateT *ms,
  MachineStateT *expected, enum Status expected_status);
void assert_execution_went_aok
  (char *description, MachineStateT *ms, MachineStateT *expected);
void assert_execution_crashed
  (char *description, MachineStateT *ms, enum error_id expected_error);
  ///
enum boole equal_states(MachineStateT *a, MachineStateT *b);
enum boole equal_stacks(StackT *a, StackT *b);
enum boole equal_values(ValueT *a, ValueT *b);

CodeT *CodeRef(long x);

#endif ///---/// #ifndef CCAM_TESTING_HEADER
