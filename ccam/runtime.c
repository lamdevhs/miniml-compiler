#include "virtual-machine.h"
#include "generated.h"

int main () {
  MachineStateT *ms = blank_state (main_code);
  
#ifdef DEBUGMODE
  int verbosity = True;
#else
  int verbosity = False;
#endif
  
  enum Status status = run_machine (ms, verbosity);
  if (status == Halted) {
#ifdef DEBUGMODE
    printf(NL); print_state(ms);
#else
    print_value (ms->term); printf(NL);
#endif
    return 0;
  }
  else {
    print_status (status);
#ifdef DEBUGMODE
    printf("Final State: "); print_state(ms);
#endif
    return 1;
  }
}