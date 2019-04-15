# TODO

- handle memory via refcounts:
  use wrappers around malloc and free to measure the gain
  
- add a function taking a CodeT* and pretty-printing the next instruction,
  use it to change print_state to sth more friendly
  

- define the runtime() in the generated code, so that the compilation
  process does not depend on how the files are named (does not depend
  on a header either).

# PITFALLS
- /!\ when hitting an error, it may happen that some malloced stuff are in local
  variables of the exec(); more worryingly, the current value of main_term may
  very well have been freed (destructured or even just freed), so if we send it
  back as return value, it can be NULL.
- most of the exec_zzzz that can fail at some point, fail after destructuring some value,
  often leaving the machine in an unsafe, partial state... we might want to fix that.
- using value_one_stack on a NULL stack does not cause an error, and simply uses NULL as its
  bottommost link... maybe we should just use NULL to symbolize the empty stack...?

# IDEAS
- policy for memory leakage:
  - if we hit an error, the vm contains bugs, so it will crash and end,
    we therefore don't need to care about freeing all the memory we used,
    as it'd be then just an additional bug.
  - if we hit halt, we may imagine that the same process could go on doing other things
    after the vm dies, so we should probably free the stack recursively
    (as it's the only place where we put stuff that we don't return).
  so, we would guarantee that, barring compiler bug, the vm doesn't have memory leakage,
  and if the compiler does have a bug, the memory management all goes to shit...
- we could have a status that is different for every single type of errors: 
  one e.g. for "Fst(Bool)", Fst(Null), Fst(NULL), Plus(Bool, Int) etc... though meh
- replace most status errors with "ValueMatchingFailure" and "StackMatchingFailure"

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