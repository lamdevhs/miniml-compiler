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
  Apply,
  Return,
  QuoteInt,
  QuoteBool,
  Curry,
  Branch,
  Call,
};

enum unary_operations {
  Fst, Snd,
};

enum binary_operations {
//| there are just int constants,
//| i.e. this enum is not treated as a separate type
  Plus, Sub, Mul, Div, Mod,
  Eq, Neq,
  Ge, Gt,
  Le, Lt,
};


//| datatype CodeT
//|    = Instruction(int)
//|    | Operation(int)
//|    | Reference(CodeT *)
//|    | Data(long)
union CodeT; typedef union CodeT {
  int instruction;
  int operation;
  union CodeT *reference;
  long data;
} CodeT;
//| A value of type (CodeT *) will represent a pointer to elements
//| of an array containing a program's code: instructions, operations,
//| quoted data, and references to other pieces of code.


enum Status {
  AllOk,
  Halted,
  DivZero,
  UnknownInstruction,
  UnknownUnary,
  UnknownBinary,
  
  //| pattern-matching errors:
    MatchFailure,
    MatchNULLStack,
    StackHeadIsNotValue,
    StackHeadIsNotCode,
    
    MatchNULLValue,
    ValueIsNotPair,
    ValueIsNotClosure,
    ValueIsNotBool,
    ValueIsNotInt,
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
ValueT *PairValue(ValueT *first, ValueT *second);
ValueT *ClosureValue(CodeT *code, ValueT *closure_value);
ValueT *BoolValue(long b);
ValueT *IntValue(long x);
ValueT *NullValue();
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
StackT *EmptyStack();
StackT *ValueOnStack(ValueT *value, StackT *old_stack);
StackT *CodeOnStack(CodeT *code, StackT *old_stack);
  ///
ValueOnStackT match_stacktop_with_value(StackT *stack, enum Status *status);
CodeOnStackT match_stacktop_with_code(StackT *stack, enum Status *status);
  ///
void print_stack(StackT *stack); //| using:
  void __print_stack(StackT *stack, int code_count, int is_stack_top);
  void __print_code_in_stack(int code_count);
void print_stacktop(StackT *stack);
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
enum Status exec_Compare(MachineStateT *ms);
  ///
enum Status exec_Push(MachineStateT *ms);
enum Status exec_Cons(MachineStateT *ms);
  ///
enum Status exec_QuoteBool(MachineStateT *ms);
enum Status exec_QuoteInt(MachineStateT *ms);
  ///
enum Status exec_Swap(MachineStateT *ms);
enum Status exec_Curry(MachineStateT *ms);
enum Status exec_Apply(MachineStateT *ms);
enum Status exec_Return(MachineStateT *ms);
enum Status exec_Branch(MachineStateT *ms);
enum Status exec_Call(MachineStateT *ms);
  ///
long eval_binary_operation(int operation, long a, long b, enum Status *status);
void print_instruction(CodeT *code);
void print_operation(int operation);
void print_state(MachineStateT *ms);
void print_status(enum Status status);
CodeT *CodeRef(long x);

//| runtime requirements:
CodeT *get_main_code(); //| this function is meant to be generated by the compiler

#endif
