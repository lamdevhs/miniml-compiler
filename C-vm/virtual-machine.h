#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <stdio.h>

#define DBG printf
#define NL "\n"

enum instructions {
//| there are just int constants,
//| i.e. this enum is not treated as a separate type
  Halt,
  Unary,
  Arith,
  Compare,
  Cons,
  Push,
  Swap,
  App,
  Return,
  QuoteInt,
  QuoteBool,
  Cur,
  Branch,
  AddDefs,
  RmDefs,
  Call
};

enum unary_operations {
  Fst, Snd
};

enum primitive_operations {
//| there are just int constants,
//| i.e. this enum is not treated as a separate type
  Plus, Sub, Mul, Div, Mod,
  Eq, Neq,
  Ge, Gt,
  Le, Lt
};



typedef long Bin;
//| Bin* == pointer to an array containing binary code

enum Status {
  AllOk,
  Halted,
  DivZero,
  UnknownOperation,
  UnknownInstruction,
  
  //| pattern-matching errors:
    MatchNULLStack,
    StackIsEmpty,
    StackHeadIsNotValue,
    StackHeadIsNotCode,
    
    MatchNULLValue,
    ValueIsNotPair,
    ValueIsNotClosure,
    ValueIsNotBool,
    ValueIsNotInt
};



//| datatype Value
//|   = BoolValue(long) | IntValue(long)
//|   | Pair(Value*, Value*) | Closure(Bin*, Value*) 

enum ValueTag {
  PairValue,
  BoolValue,
  IntValue,
  ClosureValue,
  NullValue
};

struct Value; //| recursive definitions

typedef struct Pair {
  struct Value *first;
  struct Value *second;
} Pair;

typedef struct Closure {
  Bin *code;
  struct Value *value;
} Closure;

typedef struct Value {
  enum ValueTag tag;
  union {
    long integer;
    long boolean;
    struct Pair pair;
    struct Closure closure;
  } as;
} Value;
//| if Value.tag == NullValue, then Value.as is left undefined/unused




//| datatype Stack
//|   = EmptyStack | ValueOnStack(Value*, Stack*) | CodeOnStack(Bin*, Stack*)

enum StackTag {
  HeadIsValue,
  HeadIsCode,
  EmptyStack
};

struct Stack; //| recursive definitions

typedef struct ValueOnStack {
  Value *head;
  struct Stack *tail;
} ValueOnStack;

typedef struct CodeOnStack {
  Bin *head;
  struct Stack *tail;
} CodeOnStack;

typedef struct Stack {
  enum StackTag tag;
  union {
    ValueOnStack with_value;
    CodeOnStack with_code;
  } as;
} Stack;
//| if Stack.tag == EmptyStack, then Stack.as is left undefined/unused



//| datatype MachineState = (Value*, Bin*, Stack*)

typedef struct MachineState {
  Value *term;
  Bin *code;
  Stack *stack;
} MachineState;


//| value.c
Value *value_Pair(Value *first, Value *second);
Value *value_Closure(Bin *code, Value *closure_value);
Value *value_Bool(long b);
Value *value_Int(long x);
Value *value_Null();
  ///
Value *deepcopy_value(Value *value);
void deepfree_value(Value *value);
  ///
Pair match_value_with_pair(Value *value, enum Status *status);
Closure match_value_with_closure(Value *value, enum Status *status);
long match_value_with_boolean(Value *value, enum Status *status);
long match_value_with_integer(Value *value, enum Status *status);
  ///
void print_value(Value *value);


//| stack.c
Stack *empty_stack();
Stack *value_onto_stack(Value *value, Stack *old_stack);
Stack *code_onto_stack(Bin *code, Stack *old_stack);
  ///
ValueOnStack match_stack_with_value(Stack *stack, enum Status *status);
CodeOnStack match_stack_with_code(Stack *stack, enum Status *status);


//| machine.c
MachineState *blank_machine(Bin *code);
enum Status run_machine(MachineState *ms);
enum Status exec(MachineState *ms);
  ///
enum Status exec_Halt(MachineState *ms);
  ///
enum Status exec_Unary(MachineState *ms);
enum Status exec_Arith(MachineState *ms);
  ///
enum Status exec_Push(MachineState *ms);
enum Status exec_Cons(MachineState *ms);
  ///
enum Status exec_QuoteBool(MachineState *ms);
enum Status exec_QuoteInt(MachineState *ms);
  ///
enum Status exec_Swap(MachineState *ms);
enum Status exec_Cur(MachineState *ms);
enum Status exec_App(MachineState *ms);
enum Status exec_Return(MachineState *ms);
enum Status exec_Branch(MachineState *ms);
  ///
long eval_primop(long operation, long a, long b, enum Status *status);
void print_instruction(long instruction);

#endif