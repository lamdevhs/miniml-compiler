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

# Replace CodeT's definition with a union

A value of type CodeT can now be:
- an instruction `(int)`, e.g. `Swap`
- an operation `(int)`, e.g. `Fst`, or `Mul`
- a code reference `(CodeT *)`, e.g. the arguments that follow a
  `Branch` instruction
- data `(long int)`, for literal integers and booleans, to be used
  with the `QuoteInt/Bool` instructions.

Since I didn't add a tag field alongside (mostly to keep the model
low-level), this union does not guarantee any type safety at all.
In other terms, nothing's preventing you from reading a CodeT in an
incorrect manner.

What it does however, is render "safe" the casting between
long/ints/pointers values to/from a CodeT[] array.

# Write flatten_code

Putting aside the implementation details, this new function takes a
list of `instr` and returns a list of `flat_instr`.
The difference between them is that the code structure is not recursive
anymore: code does not contain instructions that could contain code.
Instead, we end up with a list of separated pieces of code, and named
references from one piece of code to another. It fits the C model (with
pointers to code fragments, etc) that `flat_code` will get compiled to
eventually.
This function handles mutually recursive code as defined via the
Fix/AddDefs/Call/RmDefs extensions. Ironically, the "compile" function
that generates `instr` from `mlexp` does not handle it yet; neither
does the parser. But, soon.

# Write code generator from flat_code to C

# C-vm: Add implementation for instruction Compare

I also wrote tests for it, which allowed me to fix big stupid bugs in
eval_binary_operation (previously called eval_primop).

# C-vm: Add implementation for instruction Call

Also wrote corresponding unit tests.

# Add Fix to mlexp pretty printer in miniml.ml

# Now let-rec bindings can be compiled from mlexp

Compiled to generated C code. I tested it on factorial, it
worked like a charm!
Note though that let-rec bindings still can't be *parsed*... but soon
they will be.

# Rewrite parser, Add let-in and let-rec to it

idea: have a runtime function that does not depend on "main_code",
and have the main function in the generated code file, so that we
don't depend on its name.

# Add test-programs/

# Add Call to print_instruction(), Cleanup

And add generated.{h,c} to .gitignore, though soon it probably
won't matter anymore.

# Add debug printing tools for `access`

But the function is not at fault for the problem I encountered the
other day, so I commented out the actual statements meant to print
the trace of what the function does and finds.

# Change parsing of comma to right-associative

Previously was left-associative. This change allows for creating
values akin to lists with a nice syntax: (1, 2, 3, 4) meaning
(1, (2, (3, 4))).

# Write `generate` in interf.ml

That's the function meant to read a source file, parse its content,
compile it and output source files for the CAM (here, a C and C
header files). That function is called via the executable `comp` that
is built by the Makefile located in the root folder of the project.

Also changed a tiny bit the mlexp pretty printer in miniml.ml so that
function applications be more visible in the output.

# Add folder for already generated .c/.h files

So far it contains the code generated for the two ackermann's, for
even_odd and for factorial.
Mind you in the near future I'll probably be changing the format of
the generated code and the corresponding runtime system so they'll 
have to be updated.

# Rename folder /C-vm to /ccam

That's the one containing the source for the CAM (the runtime virtual
machine) that I wrote in C.

# At last update ccam/todo.md

Was long overdue.

# ccam: Rename Cur and App into Curry and Apply

Those are two instructions of the CAM.
Also added factorial to /test-programs/.

# ccam: Simplify print_value()

Essentially, removing the constructors to make it less verbose:
((1,False),Closure(0x000000000000,Null)) instead of
PairValue((IntValue(1),BoolValue(False)),
  ClosureValue(0x000000000000,NullValue))

# Rename ccam/test.c into ccam/unit-tests.c