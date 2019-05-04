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

enum result match_stacktop_with_value(StackT *stack, ValueOnStackT *output)
{
  if (stack == NULL || stack->tag != StackTopIsValue) return Failure;

  *output = stack->as.with_value;
  free(stack);
  return Success;
}

enum result match_stacktop_with_code(StackT *stack, CodeOnStackT *output)
{
  if (stack == NULL || stack->tag != StackTopIsCode) return Failure;

  *output = stack->as.with_code;
  free(stack);
  return Success;
}

void print_stacktop(StackT *stack)
{
  if (stack == NULL) {
    printf("<NULL Stack>");
  }
  else {
    if (stack->tag == StackIsEmpty) {
      printf("<stack is empty>");
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
