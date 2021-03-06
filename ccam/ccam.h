#ifndef CCAM_HEADER
#define CCAM_HEADER

#define NL "\n"

enum boole {
  False = 0,
  True = 1,
};

enum result {
  Success,
  Failure,
};

enum instructions {
  Halt,
  Unary,
  Arith,
  Compare,
  Test,
  Cons,
  Push,
  Swap,
  Apply,
  Return,
  QuoteInt,
  QuoteBool,
  Curry,
  Branch,
  //| for let-rec bindings:
  Call,
  //| for lists:
  QuoteEmptyList,
  MakeList,
};

enum unary_operations {
  Fst, Snd,
  Head, Tail,
};

enum test_operations {
  TestIsEmpty,
};

enum arithmetic_operations {
  Plus, Sub, Mul, Div, Mod,
};

enum comparison_operations {
  Eq, Neq,
  Ge, Gt,
  Le, Lt,
};

enum op_report {
  OperationOk,
  InvalidOperands,
  UnknownOp,
};

enum Status {
  AllOk,
  Halted,
  Crashed_UnknownInstruction,
  Crashed_Unary_Unknown,
  Crashed_Unary_NotAPair,
  Crashed_Unary_Headless,
  Crashed_Cons_NoValueOnStack,
  Crashed_Swap_NoValueOnStack,
  Crashed_CannotApply,
  Crashed_CannotReturn,
  Crashed_Arith_TypeError,
  Crashed_Arith_Unknown,
  Crashed_Arith_DivByZero,
  Crashed_Compare_TypeError,
  Crashed_Compare_Unknown,
  Crashed_Branch_NotABoolean,
  Crashed_Branch_NoValueOnStack,
  Crashed_MakeList_NotAList,
  Crashed_MakeList_NoValueOnStack,
  Crashed_Test_Unknown,
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



//| datatype ValueT
//|   = BoolValue(long int) | IntValue(long int)
//|   | Pair(ValueT*, ValueT*) | Closure(CodeT*, ValueT*)

enum ValueTag {
  ValueIsPair,
  ValueIsBool,
  ValueIsInt,
  ValueIsClosure,
  ValueIsNull,
  ValueIsListCons,
  ValueIsEmptyList,
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

typedef struct ListConsT {
  struct ValueT *head;
  struct ValueT *tail;
} ListConsT;

typedef struct ValueT {
  int copy_count;
  enum ValueTag tag;
  union {
    long integer;
    long boolean;
    PairT pair;
    ClosureT closure;
    ListConsT listcons;
  } as;
} ValueT;
//| if ValueT.tag is ValueIsNull or ValueIsEmptyList,
//| then the attribute `ValueT.as` is left undefined/unused
  ///
ValueT the_empty_list; //| we only need one of those in the whole CAM
ValueT the_null_value; //| we only need one of those in the whole CAM
  //| those unique values shall be initialized in value.c




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


//| enums.c
enum Status execute_next_instruction(MachineStateT *ms);
enum op_report eval_arith(int op, long a, long b, long *result);
enum op_report eval_comparison(int op, long a, long b, long *result);
void print_instruction(CodeT *code);
void print_unary(int unary_op);
void print_arith_operation(int operation);
void print_comparison_operation(int operation);
void print_test_operation(int operation);
void print_status(enum Status status);
char *status_message(enum Status status);

//| value.c
ValueT *PairValue(ValueT *first, ValueT *second);
ValueT *ClosureValue(CodeT *code, ValueT *closure_value);
ValueT *BoolValue(long b);
ValueT *IntValue(long x);
ValueT *NullValue();
ValueT *EmptyListValue();
ValueT *ListConsValue(ValueT *head, ValueT *tail);
  ///
ValueT *deepcopy_value(ValueT *value);
void deepincrement_copy_count(ValueT *value);
void deepfree_value(ValueT *value);
  ///
enum result match_value_with_pair(ValueT *value, PairT *output);
enum result match_value_with_closure(ValueT *value, ClosureT *output);
enum result match_value_with_boolean(ValueT *value, long *output);
enum result match_value_with_integer(ValueT *value, long *output);
enum result match_value_with_list_cons(ValueT *value, ListConsT *output);
enum result match_value_with_empty_list(ValueT *value);
enum boole value_is_list(ValueT *value);
  ///
void print_value(ValueT *value);
void print_listcons(ValueT* head, ValueT *tail);
  ///
#ifdef TRACE_MEMORY
int mallocated_values_count;
int freed_values_fake_count;
int freed_values_real_count;
void memory_value_report();
#endif
ValueT *malloc_value();
void free_value(ValueT *value);


//| stack.c
StackT *EmptyStack();
StackT *ValueOnStack(ValueT *value, StackT *old_stack);
StackT *CodeOnStack(CodeT *code, StackT *old_stack);
  ///
enum result match_stacktop_with_value(StackT *stack, ValueOnStackT *output);
enum result match_stacktop_with_code(StackT *stack, CodeOnStackT *output);
  ///
void print_stacktop(StackT *stack);


//| machine.c
MachineStateT *MachineState(ValueT *term, CodeT *code, StackT *stack);
MachineStateT *blank_state(CodeT *code);
enum Status run_machine(MachineStateT *ms, int verbose);
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
enum Status exec_QuoteEmptyList(MachineStateT *ms);
enum Status exec_MakeList(MachineStateT *ms);
  ///
enum Status exec_Test(MachineStateT *ms);
  ///
void print_state(MachineStateT *ms);


#endif ///---/// #ifndef CCAM_HEADER
