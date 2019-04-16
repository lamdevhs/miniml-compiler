#include "virtual-machine.h"

int main () {
  MachineStateT *ms = blank_state (get_main_code());
  
#ifdef DEBUGMODE
  int verbosity = True;
#else
  int verbosity = False;
#endif
  
  enum Status status = run_machine (ms, verbosity);
  if (status == Halted) {
#ifdef DEBUGMODE
    printf(NL "[DEBUG] Final state: " NL);
    print_state (ms) ;
#else
    print_value (ms->term) ; printf(NL);
#endif
    return 0;
  }
  else {
    printf(NL NL "CAM crashed! Status = "); print_status (status); printf(NL);
#ifdef DEBUGMODE
    printf(NL "[DEBUG] Stopped on instruction: "); print_instruction(ms->code); printf(NL);
    printf("[DEBUG] Last state:" NL); print_state(ms);
#endif
    return 1;
  }
}
