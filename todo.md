# TODO

simple ones:
- fix comments in machine.c's exec_zzz functions based on the new simulator.ml
- remove CodeRef(long); in machine.c?
- separate equal_... from the runtime side of things
- replace BoolValue() with Bool()/Boolean()?
- test if the parser accepts the empty program
- remove dev comments
- fix generated-all.sh so that it copies the .c files instead

ccam:

caml compiler:
- rename instrs/compile to mlexp_to_code
- replace write_instruction with string_of_instruction?

parser:
  optional stuff:
  - prevent restricted tokens to be used for variables (e.g. head, let, etc)
  - add null value to parser
  - recognize negative numbers (no real need we can always write (0-42))
  - allow underscores and dashes and maybe
    even dots in variable names
  - fix the built_in_test error: "(fst, snd);;"

global:
?

READMEs:
- write miniml's grammar, latex style

final tasks:
- clean up the test-programs list

# PITFALLS
- warning: we must forbid | (x,x) -> patterns.
  same for let rec f = 1 and f = 2 in 3;;

# IDEAS
- we could add a match before even adding custom constructors:
  then it could be "manually" done by user e.g.:
  match x with (0, val) -> ... | (1, val) -> ...
- we'd still need "is_pair", "is_empty", "is_bool", "is_int", "is_null"...

# BAD IDEAS
- fuse Return and Halt, so that Return stops the vm if the stack is empty? or at least if
  it doesn't contain code at its top?... but then we'd risk hiding bugs, as I'm not certain
  the compilation process guarrantees to end on an empty stack... nah, bad idea,
  better not risk continuing forward blindly if there's a bug and the stack isn't naturally
  emptied by the runtime process.

# DONE

Those are in no particular order due to me having been lazy with updating this file.
There's also a bit of redundancy.
  - write match_xxx_with_xxx functions, and their corresponding output types
    e.g.: `match_stack_with_value : Stack* -> enum Err* -> {head : Value*, tail : Stack*}`
  - fix type Stack: inject StackItem directly into Stack
  - fix Quote -> QuoteInt/QuoteBool
  - fix PrimOp -> {Arith, Compare, Fst, Snd}
  - restructure exec():
    - new type: MachineState {term : Term*, code : Binary, stack : Stack*}
    - explode code:
      exec_push : MachineState* -> enum Err* -> Void
      etc
    - next_instruction --> instruction
  - rename
    - Term --> Value
    - TPair, TBool, etc --> PairValue, BoolValue, etc
  - rename Binary --> `(Bin *)`: hiding the pointer is probably a bad idea
  - change Stack's internal structure:
    from `{tag * (code | val) * tail}` to `{tag * (code * tail | val * tail)}`
    so we have a similar behavior to Term/Pair, and we can even use
    StackWithValue etc:
    `{tag * (StackWithValue | StackWithCode)}`
  - use an `union { Bin *coderef, int data }` instead of long for Bin's def (if possible)
  - use PairValue/BoolValue/etc instead of value_Pair, etc, and use PairValueTag instead of PairValue,

These are in chronological order, hopefully:
- rename App --> Apply, Cur --> Curry
- change the C pretty printer of Values to be less verbose (no constructors)
- rename test.c into unit-tests.c (including in makefile)
- meliorate the format of the generated code
- add a function taking a CodeT* and pretty-printing the next instruction,
  use it to change print_state to sth more friendly
- only print the top of the stack when writing the exec state (or just the
  two topmost items maybe)
- handle memory via refcounts:
  use wrappers around malloc and free to measure the gain
- add type list
- make so the final term value of ms is freed before we display memory trace
  also make the memory trace report clearer
- rename virtual-machine to ccam.h
- write script to generate normal and debug versions for each file in
  test-programs
- remove the info() macro from ccam/
- check if we can remove `__print_stack_` stuff
- isolate enums into their own .c file
- simplify unit-tests.c
- meliorate the error handling/messaging system
- remove MEM=y from generate-all.sh
- recognize badly formed programs (let rec etc)
- forbid having the same variable bound twice by the same let rec.
- replace ListCons instr everywhere with MakeList
- fix the shit regarding auto adding .c to generated files by compiler
