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
  Quote, // no need to differentiate bools and ints
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
  ErrEmptyStack,
  ErrWrongStackHead,
  ErrNullTerm,
  ErrCopyNullTerm
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
  TEmpty
};

struct Term;

typedef struct Pair {
  struct Term *first;
  struct Term *second;
} Pair;

typedef struct Closure {
  Binary closure_code;
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

Stack *term_to_stack(Stack *stack, Term *term) {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->head.tag = StackTerm;
  new_stack->head.content.stack_term = term;
  new_stack->tail = stack;
  return new_stack;
}

Term *term_from_stack(Stack **stack, int *err) {
  Stack *original = *stack;
  if (original == NULL) {
    *err = ErrEmptyStack;
    return NULL;
  }
  // else:
  if (original->head.tag != StackTerm) {
    *err = ErrWrongStackHead;
    return NULL;
  }
  // else:
  Term *term = original->head.content.stack_term;
  *stack = original->tail; // modifies the first parameter
  free(original);
  if (term == NULL) {
    *err = ErrNullTerm;
    return NULL;
  }
  // else:
  return term;
}

Stack *code_to_stack(Stack *stack, Binary code) {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->head.tag = StackCode;
  new_stack->head.content.stack_code = code;
  new_stack->tail = stack;
  return new_stack;
}

Stack *free_stack_head(Stack *stack, int *err) {
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

void free_stack(Stack *stack) {
  int err;
  while (stack != NULL) {
    stack = free_stack_head(stack, &err);
  }
}

void free_term(Term *term) {
  if (term != NULL) {
    enum TermTag tag = term->tag;
    if (tag == TPair) {
      free_term(term->content.pair.first);
      free_term(term->content.pair.second);
    }
    else if (tag == TClosure) {
      free_term(term->content.closure.closure_term);
    }
    free(term);
  }
}

Term *new_pair(Term *first, Term *second) {
  Term *term = malloc(sizeof(Term));
  term->tag = TPair;
  term->content.pair.first = first;
  term->content.pair.second = second;
  return term;
}

Term *new_closure(Binary code, Term *term) {
  Term *closure = malloc(sizeof(Term));
  closure->tag = TClosure;
  closure->content.closure.closure_code = code;
  closure->content.closure.closure_term = term;
  return closure;
}

Term *copy_term(Term *term) {
  if (term == NULL) {
    return NULL;
  }
  // else:
  Term *new_term;
  enum TermTag tag = term->tag;
  if (tag == TPair) {
    Term *x = copy_term(term->content.pair.first);
    Term *y = copy_term(term->content.pair.second);
    new_term = new_pair(x, y);
  }
  else if (tag == TClosure) {
    Term *t = copy_term(term->content.closure.closure_term);
    new_term = new_closure(term->content.closure.closure_code, t);
  }
  else {
    Term *new_term = malloc(sizeof(Term));
    *new_term = *term; // value copy
  }
  return new_term;
}
  

Term *exec(Binary code, int *err) {
  // presets
  Term *main_term = malloc(sizeof(Term));
  main_term->tag = TEmpty;
  Stack *main_stack = NULL; // empty stack
  int next_instruction;

  while (*err == AllOk) {
    next_instruction = *code;
    if (next_instruction == Halt) break;
    else if (next_instruction == Push) {
      // term unchanged
      code += 1;
      main_stack = term_to_stack(main_stack, main_term);
    }
    else if (next_instruction == Cons) {
      Term *y = term_from_stack(&main_stack, err);
      if (*err != AllOk) break;
      
      main_term = new_pair(y, main_term);
      code += 1;
      // stack changed automatically
    }
    else if (next_instruction == Push) {
      Term *copy = copy_term(main_term);
      if (*err != AllOk) break;

      // term unchanged
      code += 1;
      main_stack = term_to_stack(main_stack, copy);
    }
  }

  free_stack(main_stack);
  return main_term;
}

int main() {
  test_eval_primop();
  Term t1, t2;
  t1.tag = TInt;
  t1.content.integer = 3;
  t2.tag = TBool;
  t2.content.boolean = 1;
  t1 = t2;
  printf("%d -- %d -- %d\n", t1.tag, TBool, t1.content.boolean);
}

// todo: handle copying smartly
