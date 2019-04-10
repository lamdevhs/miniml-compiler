#include <stdlib.h>
///
#include "virtual-machine.h"


//| creation of stacks

Stack *empty_stack() {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->tag = EmptyStack;
  return new_stack;
}

Stack *value_onto_stack(Value *value, Stack *old_stack) {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->tag = HeadIsValue;
  new_stack->as.with_value.head = value;
  new_stack->as.with_value.tail = old_stack;
  return new_stack;
}

Stack *code_onto_stack(Bin *code, Stack *old_stack) {
  Stack *new_stack = malloc(sizeof(Stack));
  new_stack->tag = HeadIsCode;
  new_stack->as.with_code.head = code;
  new_stack->as.with_code.tail = old_stack;
  return new_stack;
}



//| destructuration of stacks

ValueOnStack match_stack_with_value(Stack *stack, enum Status *status) {
  ValueOnStack output = {NULL, NULL};
  if (stack == NULL) {
    *status = MatchNULLStack;
    return output;
  }
  //| else:
  if (stack->tag != HeadIsValue) {
    *status = StackHeadIsNotValue;
    return output;
  }
  //| else:
  output = stack->as.with_value;
  
  free(stack);
  return output;
}

CodeOnStack match_stack_with_code(Stack *stack, enum Status *status) {
  CodeOnStack output = {NULL, NULL};
  if (stack == NULL) {
    *status = MatchNULLStack;
    return output;
  }
  //| else:
  if (stack->tag != HeadIsCode) {
    *status = StackHeadIsNotCode;
    return output;
  }
  //| else:
  output = stack->as.with_code;
  
  free(stack);
  return output;
}