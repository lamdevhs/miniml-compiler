#include <stdio.h>
///
#include "ccam.h"

int main ()
{
  MachineStateT *ms = blank_state(get_main_code());
  int exit_value;

  #ifdef DEBUGMODE
    int verbosity = True;
  #else
    int verbosity = False;
  #endif

  enum Status status = run_machine(ms, verbosity);
  if (status == Halted) {
    exit_value = 0;

    #ifdef DEBUGMODE
      printf("--------------------"NL);
    #endif

    print_value(ms->term); printf(NL);
  }
  else {
    //| something went wrong, the CAM crashed.
    exit_value = 1;

    #ifdef DEBUGMODE
      printf(NL "[DEBUG] Stopped on instruction: ");
      print_instruction(ms->code); printf(NL);
      printf("[DEBUG] Last state:" NL);
      print_state(ms);
      printf("--------------------"NL);
    #endif

    print_status(status); printf(NL); //| print error message
  }

  #ifdef TRACE_MEMORY
    printf(NL NL);
    deepfree_value(ms->term);
    //| ^ freeing all the values before displaying the memory trace
    memory_value_report();
  #endif

  return exit_value;
}
