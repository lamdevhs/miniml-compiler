#include <stdlib.h>
///
#include "virtual-machine.h"


Value *value_Pair(Value *first, Value *second) {
  Value *value = malloc(sizeof(Value));
  value->tag = PairValue;
  value->as.pair.first = first;
  value->as.pair.second = second;
  return value;
}

Value *value_Closure(Bin *code, Value *closure_value) {
  Value *value = malloc(sizeof(Value));
  value->tag = ClosureValue;
  value->as.closure.code = code;
  value->as.closure.value = closure_value;
  return value;
}

Value *value_Bool(long x) {
  Value *value = malloc(sizeof(Value));
  value->tag = BoolValue;
  value->as.boolean = x;
  return value;
}

Value *value_Int(long x) {
  Value *value = malloc(sizeof(Value));
  value->tag = IntValue;
  value->as.integer = x;
  return value;
}

Value *value_Null() {
  Value *value = malloc(sizeof(Value));
  value->tag = NullValue;
  return value;
}

Value *deepcopy_value(Value *value) {
  if (value == NULL) {
    return NULL;
  }
  // else:
  Value *copy;
  enum ValueTag tag = value->tag;
  if (tag == PairValue) {
    Value *x = deepcopy_value(value->as.pair.first);
    Value *y = deepcopy_value(value->as.pair.second);
    copy = value_Pair(x, y);
  }
  else if (tag == ClosureValue) {
    Value *t = deepcopy_value(value->as.closure.value);
    copy = value_Closure(value->as.closure.code, t);
  }
  else {
    copy = malloc(sizeof(Value));
    *copy = *value; //| direct struct copy
  }
  return copy;
}

void deepfree_value(Value *value) {
  if (value != NULL) {
    enum ValueTag tag = value->tag;
    if (tag == PairValue) {
      deepfree_value(value->as.pair.first);
      deepfree_value(value->as.pair.second);
    }
    else if (tag == ClosureValue) {
      deepfree_value(value->as.closure.value);
    }
    free(value);
  }
}

Pair match_value_with_pair(Value *value, enum Status *status) {
  Pair output = {NULL, NULL};
  if (value == NULL) {
    *status = MatchNULLValue;
    return output;
  }
  //| else:
  if (value->tag != PairValue) {
    *status = ValueIsNotPair;
    return output;
  }
  //| else:
  output = value->as.pair;
  
  free(value);
  return output;
}

Closure match_value_with_closure(Value *value, enum Status *status) {
  Closure output = {NULL, NULL};
  if (value == NULL) {
    *status = MatchNULLValue;
    return output;
  }
  //| else:
  if (value->tag != ClosureValue) {
    *status = ValueIsNotClosure;
    return output;
  }
  //| else:
  output = value->as.closure;
  
  free(value);
  return output;
}

long match_value_with_boolean(Value *value, enum Status *status) {
  if (value == NULL) {
    *status = MatchNULLValue;
    return 0;
  }
  //| else:
  if (value->tag != BoolValue) {
    *status = ValueIsNotBool;
    return 0;
  }
  //| else:
  long output = value->as.boolean;
  
  free(value);
  return output;
}

long match_value_with_integer(Value *value, enum Status *status) {
  if (value == NULL) {
    *status = MatchNULLValue;
    return 0;
  }
  //| else:
  if (value->tag != IntValue) {
    *status = ValueIsNotInt;
    return 0;
  }
  //| else:
  long output = value->as.integer;
  
  free(value);
  return output;
}

void print_value(Value *value) {
  if (value == NULL) {
    printf("<NULL Value>");
  }
  else {
    enum ValueTag tag = value->tag;
    if (tag == NullValue) {
      printf("NullValue");
    }
    else if (tag == BoolValue) {
      printf("BoolValue(%c)", value->as.boolean ? 'T' : 'F');
    }
    else if (tag == IntValue) {
      printf("IntValue(%ld)", value->as.integer);
    }
    else if (tag == PairValue) {
      printf("PairValue(");
      print_value(value->as.pair.first);
      printf(", ");
      print_value(value->as.pair.second);
      printf(")");
    }
    else if (tag == ClosureValue) {
      printf("ClosureValue(@%ld, ", (long)value->as.closure.code);
      print_value(value->as.closure.value);
      printf(")");
    }
    else {
      printf("<ERROR Value>");
    }
  }
}
      