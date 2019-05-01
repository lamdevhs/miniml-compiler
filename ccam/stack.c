#include <stdlib.h>
#include <stdio.h>
///
#include "ccam.h"


//| creation of stacks

StackT *EmptyStack() {
  StackT *new_stack = malloc(sizeof(StackT));
  new_stack->tag = StackIsEmpty;
  return new_stack;
}

StackT *ValueOnStack(ValueT *value, StackT *old_stack) {
  StackT *new_stack = malloc(sizeof(StackT));
  new_stack->tag = StackTopIsValue;
  new_stack->as.with_value.top = value;
  new_stack->as.with_value.bottom = old_stack;
  return new_stack;
}

StackT *CodeOnStack(CodeT *code, StackT *old_stack) {
  StackT *new_stack = malloc(sizeof(StackT));
  new_stack->tag = StackTopIsCode;
  new_stack->as.with_code.top = code;
  new_stack->as.with_code.bottom = old_stack;
  return new_stack;
}



//| destructuration of stacks

ValueOnStackT match_stacktop_with_value(StackT *stack, enum Status *status) {
  ValueOnStackT output = {NULL, NULL};
  if (stack == NULL) {
    *status = MatchNULLStack;
    return output;
  }
  //| else:
  if (stack->tag != StackTopIsValue) {
    *status = StackHeadIsNotValue;
    return output;
  }
  //| else:
  output = stack->as.with_value;

  free(stack);
  return output;
}

CodeOnStackT match_stacktop_with_code(StackT *stack, enum Status *status) {
  CodeOnStackT output = {NULL, NULL};
  if (stack == NULL) {
    *status = MatchNULLStack;
    return output;
  }
  //| else:
  if (stack->tag != StackTopIsCode) {
    *status = StackHeadIsNotCode;
    return output;
  }
  //| else:
  output = stack->as.with_code;

  free(stack);
  return output;
}

int equal_stacks(StackT *a, StackT *b) {
  if (a == NULL) return b == NULL;
  // else:
  enum StackTag tag = a->tag;
  if (tag != b->tag) return False;
  if (tag == StackTopIsValue) {
    return equal_values(a->as.with_value.top, b->as.with_value.top)
      && equal_stacks(a->as.with_value.bottom, b->as.with_value.bottom);
  }
  if (tag == StackTopIsCode) {
    return a->as.with_code.top == b->as.with_code.top
      && equal_stacks(a->as.with_value.bottom, b->as.with_value.bottom);
  }
  else return True; //| StackIsEmpty, or maybe an invalid tag...
}

void print_stacktop(StackT *stack)
{
  if (stack == NULL) {
    printf("<NULL Stack>");
  }
  else {
    if (stack->tag == StackIsEmpty) {
      printf("[]");
    }
    else if (stack->tag == StackTopIsValue) {
      print_value(stack->as.with_value.top);
    }
    else if (stack->tag == StackTopIsCode) {
      printf("%p", stack->as.with_code.top);
    }
    else {
      printf("<ERROR Stack>");
    }
  }
}
