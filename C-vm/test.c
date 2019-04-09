#include <stdio.h>
#include <stdlib.h>


typedef int* Binary;

enum instruction {
  Halt,
  Prim,
  Cons,
  Push,
  Swap,
  App,
  Return,
  Quote, // ^ no need to differentiate bools and ints
  Cur,
  Branch,
  AddDefs,
  RmDefs
};

enum error {
  AllOk,
  ErrDivZero,
  ErrUnknownOp,
  ErrUnknownInstruction,
  ErrEmptyStack
};

enum primop {
  Fst,
  Snd,
  Plus,
  Sub,
  Mul,
  Div,
  Mod,
  Eq,
  Neq,
  Ge,
  Gt,
  Le,
  Lt
};

enum TermTag {
  TPair,
  TBool,
  TInt,
  TClosure,
  TNull
};

struct Term;

typedef struct Pair {
  struct Term *first;
  struct Term *second;
} Pair;

typedef struct Closure {
  Binary *closure_code;
  struct Term *closure_term;
} Closure;

typedef struct Term {
  enum TermTag tag;
  union {
    int integer;
    int boolean;
    struct Pair pair;
    struct Closure closure;
  } content;
} Term;

enum StackItemTag {
  StackTerm,
  StackCode
};

typedef struct StackItem {
  enum StackItemTag tag;
  union {
    Term *stack_term;
    Binary stack_code;
  } content;
} StackItem;

typedef struct Stack {
  struct StackItem head;
  struct Stack *tail;
} Stack;

int eval_primop(enum primop op, int a, int b, int *err) {
  switch (op) {
  case Fst: return a;
    break;
  case Snd: return b;
    break;
  case Plus: return a + b;
    break;
  case Sub: return a - b;
    break;
  case Mul: return a * b;
    break;
  case Div:
    if (b == 0) { *err = ErrDivZero; return 0; }
    else { return a / b; }
    break;
  case Mod:
    if (b == 0) { *err = ErrDivZero; return 0; }
    else { return a % b; }
    break;
  case Eq: return a == b;
    break;
  case Neq: return a != b;
    break;
  case Ge: return a >= b;
    break;
  case Gt: return a < b;
    break;
  case Le: return a < b;
    break;
  case Lt: return a <= b;
    break;
  default: *err = ErrUnknownOp; return 0;
    break;
  }
}

int test_eval_primop() {
  int res; int err = AllOk;
  
  res = eval_primop(Mul, 3, 7, &err);
  printf("xyz\n" "%d -- err = %d\n", res, err);
  
  res = eval_primop(Mod, 5, 0, &err);
  printf("xyz\n" "%d -- err = %d\n", res, err);
  
  res = eval_primop(4242, 3, 0, &err);
  printf("xyz\n" "%d -- err = %d\n", res, err);
  return 0;
}

Stack *term_to_stack(Stack *stack, enum StackItemTag type, Term *term) {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->head.tag = StackTerm;
  new_stack->head.content.stack_term = term;
  new_stack->tail = stack;
  return new_stack;
}

Stack *code_to_stack(Stack *stack, enum StackItemTag type, Binary code) {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->head.tag = StackCode;
  new_stack->head.content.stack_code = code;
  new_stack->tail = stack;
  return new_stack;
}

Stack *pop_stack(Stack *stack, int *err) {
  if (stack == NULL) {
    // stack is empty
    *err = ErrEmptyStack;
    return NULL;
  }
  // else:
  Stack *new_stack = stack->tail;
  if (stack->head.tag == StackTerm) {
    free(stack->head.content.stack_term);
  }
  free(stack);
  return new_stack;
}

void exec(Binary code) {
  // presets
  Term main_term;
  main_term.tag = TNull;
  
  Stack *main_stack = NULL; // empty stack
  
  
}

int main() {
  test_eval_primop();
}