#include <stdlib.h>
///
#include "virtual-machine.h"


ValueT *PairValue(ValueT *first, ValueT *second) {
  ValueT *value = malloc_value();
  value->tag = ValueIsPair;
  value->as.pair.first = first;
  value->as.pair.second = second;
  return value;
}

ValueT *ClosureValue(CodeT *code, ValueT *closure_value) {
  ValueT *value = malloc_value();
  value->tag = ValueIsClosure;
  value->as.closure.code = code;
  value->as.closure.value = closure_value;
  return value;
}

ValueT *BoolValue(long x) {
  ValueT *value = malloc_value();
  value->tag = ValueIsBool;
  value->as.boolean = !! x;
  return value;
}

ValueT *IntValue(long x) {
  ValueT *value = malloc_value();
  value->tag = ValueIsInt;
  value->as.integer = x;
  return value;
}

ValueT *NullValue() {
  ValueT *value = malloc_value();
  value->tag = ValueIsNull;
  return value;
}

ValueT *deepcopy_value(ValueT *value) {
  if (value == NULL) {
    return NULL;
  }
  // else:
  ValueT *copy;
  enum ValueTag tag = value->tag;
  if (tag == ValueIsPair) {
    ValueT *x = deepcopy_value(value->as.pair.first);
    ValueT *y = deepcopy_value(value->as.pair.second);
    copy = PairValue(x, y);
  }
  else if (tag == ValueIsClosure) {
    ValueT *t = deepcopy_value(value->as.closure.value);
    copy = ClosureValue(value->as.closure.code, t);
  }
  else {
    copy = malloc_value();
    *copy = *value; //| direct struct copy
  }
  return copy;
}

void deepfree_value(ValueT *value) {
  if (value != NULL) {
    enum ValueTag tag = value->tag;
    if (tag == ValueIsPair) {
      deepfree_value(value->as.pair.first);
      deepfree_value(value->as.pair.second);
    }
    else if (tag == ValueIsClosure) {
      deepfree_value(value->as.closure.value);
    }
    free_value(value);
  }
}

PairT match_value_with_pair(ValueT *value, enum Status *status) {
  PairT output = {NULL, NULL};
  if (value == NULL) {
    *status = MatchNULLValue;
    return output;
  }
  //| else:
  if (value->tag != ValueIsPair) {
    *status = ValueIsNotPair;
    return output;
  }
  //| else:
  output = value->as.pair;
  
  free_value(value);
  return output;
}

ClosureT match_value_with_closure(ValueT *value, enum Status *status) {
  ClosureT output = {NULL, NULL};
  if (value == NULL) {
    *status = MatchNULLValue;
    return output;
  }
  //| else:
  if (value->tag != ValueIsClosure) {
    *status = ValueIsNotClosure;
    return output;
  }
  //| else:
  output = value->as.closure;
  
  free_value(value);
  return output;
}

long match_value_with_boolean(ValueT *value, enum Status *status) {
  if (value == NULL) {
    *status = MatchNULLValue;
    return 0;
  }
  //| else:
  if (value->tag != ValueIsBool) {
    *status = ValueIsNotBool;
    return 0;
  }
  //| else:
  long output = value->as.boolean;
  
  free_value(value);
  return output;
}

long match_value_with_integer(ValueT *value, enum Status *status) {
  if (value == NULL) {
    *status = MatchNULLValue;
    return 0;
  }
  //| else:
  if (value->tag != ValueIsInt) {
    *status = ValueIsNotInt;
    return 0;
  }
  //| else:
  long output = value->as.integer;
  
  free_value(value);
  return output;
}

void print_value(ValueT *value) {
  if (value == NULL) {
    printf("<NULL Value>");
  }
  else {
    enum ValueTag tag = value->tag;
    if (tag == ValueIsNull) {
      printf("Null");
    }
    else if (tag == ValueIsBool) {
      printf("%s", value->as.boolean ? "True" : "False");
    }
    else if (tag == ValueIsInt) {
      printf("%ld", value->as.integer);
    }
    else if (tag == ValueIsPair) {
      printf("(");
      print_value(value->as.pair.first);
      printf(", ");
      print_value(value->as.pair.second);
      printf(")");
    }
    else if (tag == ValueIsClosure) {
      printf("Closure(%p, ", value->as.closure.code);
      print_value(value->as.closure.value);
      printf(")");
    }
    else {
      printf("<ERROR Value>");
    }
  }
}

int equal_values(ValueT *a, ValueT *b) {
  if (a == NULL) {
    return b == NULL;
  }
  // else:
  enum ValueTag tag = a->tag;
  if (tag != b->tag) {
    return False;
  }
  else if (tag == ValueIsInt) {
    return a->as.integer == b->as.integer;
  }
  else if (tag == ValueIsBool) {
    return !! (a->as.boolean) == !! (b->as.boolean);
  }
  else if (tag == ValueIsPair) {
    return equal_values(a->as.pair.first, b->as.pair.first)
      && equal_values(a->as.pair.second, b->as.pair.second);
  }
  else if (tag == ValueIsClosure) {
    return a->as.closure.code == b->as.closure.code
      && equal_values(a->as.closure.value, b->as.closure.value);
  }
  else return True; //| tag == ValueIsNull, or is invalid...
}

#ifdef TRACE_MEMORY
int mallocated_values_count = 0;
int freed_values_count = 0;

void memory_value_report()
{
  printf("[VALUE MEMORY USAGE]: malloced: %d, freed: %d" NL,
    mallocated_values_count,
    freed_values_count);
}
#endif

ValueT *malloc_value()
{
#ifdef TRACE_MEMORY
  mallocated_values_count += 1;
#endif
  ValueT *value = malloc(sizeof(ValueT));
  return value;
}

void free_value(ValueT *value)
{
#ifdef TRACE_MEMORY
  freed_values_count += 1;
#endif
  free(value);
}