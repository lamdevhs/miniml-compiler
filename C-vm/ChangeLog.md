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