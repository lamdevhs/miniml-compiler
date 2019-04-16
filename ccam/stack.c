#include <stdlib.h>
///
#include "virtual-machine.h"


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

void print_stack(StackT *stack) {
  __print_stack(stack, 0, True);
}
//| where:
  #define STACK_CONS() printf(" :: ")
  void __print_stack(StackT *stack, int code_count, int is_stack_top)
  {
    if (stack == NULL) {
      __print_code_in_stack(code_count);
      printf("<NULL Stack>");
    }
    else {
      if (stack->tag == StackIsEmpty) {
        __print_code_in_stack(code_count);
        printf("[]");
      }
      else if (stack->tag == StackTopIsValue) {
        __print_code_in_stack(code_count);
        print_value(stack->as.with_value.top);
        STACK_CONS();
        __print_stack(stack->as.with_value.bottom, 0, False);
      }
      else if (stack->tag == StackTopIsCode) {
        if (is_stack_top) {
          //| this code is on the top of the stack
          //| so it could be useful to actually print its value
          printf("%p", stack->as.with_code.top);
          STACK_CONS();
          __print_stack(stack->as.with_code.bottom, 0, False);
        }
        else {
          //| this code is not on the top of the stack
          //| so we just add 1 to the count
          __print_stack(stack->as.with_code.bottom, code_count + 1, False);
        } 
      }
      else {
        __print_code_in_stack(code_count);
        printf("<ERROR Stack>");
      }
    }
  }
  void __print_code_in_stack(int code_count)
  {
    if (code_count != 0) printf("{Code x%d} :: ", code_count);
  }