# Add exec for Unary, Arith and Cur

I also tried to run a mini program but some error occurred, so the
code is obviously buggy. I'm going to write unit tests for each of
the functions that I've written so far, so I can test their
behavior and uncover all the bugs I've missed.

# Start writing unit tests for C-vm

# Add /comp to .gitignore

# Add pretty printers for instructions and Values

# instrs.ml: Replace Quote with QuoteI/B

# Fix a bug in exec_App

Instead of storing the address of the next instruction on the stack,
exec_App used to store the address of the current instruction (App).
With this fix, the mini program works as expected! :)

# Add functions to test equality

Equality between Values, Stacks, MachineStates.
Also renamed blank_machine to the more appropriate blank_state.

# Add some tests for type Value

# Add function new_state(), Fix exec_Unary

To create a MachineState with specific field values.

Changed exec_Unary a bit so it raises an error if the operation
is not recognized (instead of silently assuming it's Snd).

Also renamed UnknownOperation into UnknownArith.

# Add verbosity argument for run_machine()

# Add printers for MachineState, Stack, enum Status

# Add tests for exec_... functions

For all the instructions that I have implemented so far.

# Rename lots of things

Mostly:
- Datatypes now sport a capital T at the end, as in StackT, ValueT,
  and so on. The enums were left untouched as they exist in a
  separate namespace.
- Stack's "head/tail" have become "top/bottom", closer to the stack
  metaphor.
- Bin was renamed to CodeT.
- Tags now all contain an "Is" (reminiscent of boolean values),
  e.g. ValueIsNull, StackIsEmpty, and so on.

Most of the above is in anticipation of renaming the constructors
value_Pair, empty_stack, etc., to PairValue, EmptyStack, etc.

# Rename constructors

Replaced value_Pair with PairValue, code_onto_stack with CodeOnStack,
and so on. Also replaced match_stack_with_zzz with
match_stacktop_with_zzz.

# Add cours.pdf

# Fix exec_zzz to avoid incoherent states

Before this commit, if an instruction terminated with a non-AllOk
status, there was a real possibility that part of the machine state
(ms->term and/or ms->stack) had been free()'d. Also, some
malloc()'ed objects could have been lost in a leak.

So, I modified the implementations of the various instructions to
make sure to reset the machine state to its previous values in case
of error -- also plugging the memory leaks at the same time.