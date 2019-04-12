#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <stdio.h>

#define DBG printf
#define NL "\n"
#define False 0
#define True 1
#define info(x)

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



typedef long CodeT;
//| CodeT* == pointer to an array containing binary code

enum Status {
  AllOk,
  Halted,
  DivZero,
  UnknownInstruction,
  UnknownUnary,
  UnknownArith,
  
  //| pattern-matching errors:
    MatchNULLStack,
    StackHeadIsNotValue,
    StackHeadIsNotCode,
    
    MatchNULLValue,
    ValueIsNotPair,
    ValueIsNotClosure,
    ValueIsNotBool,
    ValueIsNotInt
};



//| datatype ValueT
//|   = BoolValue(long int) | IntValue(long int)
//|   | Pair(ValueT*, ValueT*) | Closure(CodeT*, ValueT*) 

enum ValueTag {
  ValueIsPair,
  ValueIsBool,
  ValueIsInt,
  ValueIsClosure,
  ValueIsNull
};

struct ValueT; //| recursive definitions

typedef struct PairT {
  struct ValueT *first;
  struct ValueT *second;
} PairT;

typedef struct ClosureT {
  CodeT *code;
  struct ValueT *value;
} ClosureT;

typedef struct ValueT {
  enum ValueTag tag;
  union {
    long integer;
    long boolean;
    struct PairT pair;
    struct ClosureT closure;
  } as;
} ValueT;
//| if ValueT.tag == NullValue, then ValueT.as is left undefined/unused




//| datatype StackT
//|   = EmptyStack | ValueOnStack(ValueT*, StackT*) | CodeOnStack(CodeT*, StackT*)

enum StackTag {
  StackTopIsValue,
  StackTopIsCode,
  StackIsEmpty
};

struct StackT; //| recursive definitions

typedef struct ValueOnStackT {
  ValueT *top;
  struct StackT *bottom;
} ValueOnStackT;

typedef struct CodeOnStackT {
  CodeT *top;
  struct StackT *bottom;
} CodeOnStackT;

typedef struct StackT {
  enum StackTag tag;
  union {
    ValueOnStackT with_value;
    CodeOnStackT with_code;
  } as;
} StackT;
//| if StackT.tag == StackIsEmpty, then StackT.as is left undefined/unused



//| datatype MachineStateT = MachineState(ValueT*, CodeT*, StackT*)

typedef struct MachineStateT {
  ValueT *term;
  CodeT *code;
  StackT *stack;
} MachineStateT;


//| value.c
ValueT *value_Pair(ValueT *first, ValueT *second);
ValueT *value_Closure(CodeT *code, ValueT *closure_value);
ValueT *value_Bool(long b);
ValueT *value_Int(long x);
ValueT *value_Null();
  ///
ValueT *deepcopy_value(ValueT *value);
void deepfree_value(ValueT *value);
  ///
PairT match_value_with_pair(ValueT *value, enum Status *status);
ClosureT match_value_with_closure(ValueT *value, enum Status *status);
long match_value_with_boolean(ValueT *value, enum Status *status);
long match_value_with_integer(ValueT *value, enum Status *status);
  ///
void print_value(ValueT *value);
int equal_values(ValueT *a, ValueT *b);

//| stack.c
StackT *empty_stack();
StackT *value_onto_stack(ValueT *value, StackT *old_stack);
StackT *code_onto_stack(CodeT *code, StackT *old_stack);
  ///
ValueOnStackT match_stack_with_value(StackT *stack, enum Status *status);
CodeOnStackT match_stack_with_code(StackT *stack, enum Status *status);
  ///
void print_stack(StackT *stack);
int equal_stacks(StackT *a, StackT *b);


//| machine.c
MachineStateT *MachineState(ValueT *term, CodeT *code, StackT *stack);
MachineStateT *blank_state(CodeT *code);
int equal_states(MachineStateT *a, MachineStateT *b);
enum Status run_machine(MachineStateT *ms, int verbose);
enum Status exec(MachineStateT *ms);
  ///
enum Status exec_Halt(MachineStateT *ms);
  ///
enum Status exec_Unary(MachineStateT *ms);
enum Status exec_Arith(MachineStateT *ms);
  ///
enum Status exec_Push(MachineStateT *ms);
enum Status exec_Cons(MachineStateT *ms);
  ///
enum Status exec_QuoteBool(MachineStateT *ms);
enum Status exec_QuoteInt(MachineStateT *ms);
  ///
enum Status exec_Swap(MachineStateT *ms);
enum Status exec_Cur(MachineStateT *ms);
enum Status exec_App(MachineStateT *ms);
enum Status exec_Return(MachineStateT *ms);
enum Status exec_Branch(MachineStateT *ms);
  ///
long eval_primop(long operation, long a, long b, enum Status *status);
void print_instruction(long instruction);
void print_state(MachineStateT *ms);
void print_status(enum Status status);

#endif
