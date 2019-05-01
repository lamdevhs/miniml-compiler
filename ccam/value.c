#include <stdlib.h>
///
#include "virtual-machine.h"

//| static, unique values:
ValueT the_empty_list = { .copy_count = 0, .tag = ValueIsEmptyList } ;
ValueT the_null_value = { .copy_count = 0, .tag = ValueIsNull } ;

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
  the_null_value.copy_count += 1;
  return &the_null_value;
}

ValueT *EmptyListValue()
{
  the_empty_list.copy_count += 1;
  return &the_empty_list;
}

ValueT *ListConsValue(ValueT *head, ValueT *tail)
{
  ValueT *value = malloc_value();
  value->tag = ValueIsListCons;
  value->as.list.head = head;
  value->as.list.tail = tail;
  return value;
}

ValueT *deepcopy_value(ValueT *value) {
  if (value == NULL) return NULL;

  deepincrement_copy_count(value);
  //| now we can safely return the input
  //| as if we'd actually copied the structure
  //| recursively
  return value;
}

void deepincrement_copy_count(ValueT *value)
{
  if (value != NULL) {
    enum ValueTag tag = value->tag;
    if (tag == ValueIsPair) {
      deepincrement_copy_count(value->as.pair.first);
      deepincrement_copy_count(value->as.pair.second);
    }
    else if (tag == ValueIsListCons) {
      deepincrement_copy_count(value->as.list.head);
      deepincrement_copy_count(value->as.list.tail);
    }
    else if (tag == ValueIsClosure) {
      deepincrement_copy_count(value->as.closure.value);
    }
    else {} //| `value` does not contain any other value
    value->copy_count += 1; //| <---
  }
}

void deepfree_value(ValueT *value) {
  if (value != NULL) {
    enum ValueTag tag = value->tag;
    if (tag == ValueIsPair) {
      deepfree_value(value->as.pair.first);
      deepfree_value(value->as.pair.second);
    }
    if (tag == ValueIsListCons) {
      deepfree_value(value->as.list.head);
      deepfree_value(value->as.list.tail);
    }
    else if (tag == ValueIsClosure) {
      deepfree_value(value->as.closure.value);
    }
    else {} //| `value` does not contain any other value
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

int value_is_list(ValueT *value)
{
  if (value == NULL) return False;
  enum ValueTag tag = value->tag;
  return (tag == ValueIsEmptyList || tag == ValueIsListCons);
}

ListConsT match_value_with_list_cons(ValueT *value, enum Status *status)
{
  ListConsT output = {NULL, NULL};
  if (value == NULL) {
    *status = MatchNULLValue;
    return output;
  }
  if (!!! value_is_list(value)) {
    *status = ValueIsNotAList;
    return output;
  }
  if (value->tag == ValueIsEmptyList) {
    *status = ValueIsHeadless;
    return output;
  }
  //| else
  output = value->as.list;

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
    else if (tag == ValueIsEmptyList) {
      printf("[]");
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
    else if (tag == ValueIsListCons) {
      printf("[");
      print_listcons(value->as.list.head, value->as.list.tail);
      printf("]");
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

void print_listcons(ValueT* head, ValueT *tail)
{
  if (head == NULL) return; //| print nothing
  print_value(head);

  if (!!! value_is_list(tail)) {
    printf("<ERROR tail is not a list>"); return;
    //| ^ should really never happen
  }
  if (tail->tag == ValueIsEmptyList) return; //| we printed the head, we're done
  else {
    printf("; ");
    print_listcons(tail->as.list.head, tail->as.list.tail);
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
  else if (tag == ValueIsListCons) {
    return equal_values(a->as.list.head, b->as.list.head)
      && equal_values(a->as.list.tail, b->as.list.tail);
  }
  else if (tag >= ValueTagIsInvalid) return False;
  else return True; //| tags are equal and valid and the value contains nothing
}

#ifdef TRACE_MEMORY
int mallocated_values_count = 0;
int freed_values_fake_count = 0;
int freed_values_real_count = 0;

void memory_value_report()
{
  printf(
    "[ValueT MEMORY USAGE REPORT] " NL
    "- mallocated values: %d " NL
    "- freed values: %d" NL
    "- reference counting prevented the allocation of" NL
    "  %d unncecessary copies of values" NL,
    mallocated_values_count,
    freed_values_real_count,
    freed_values_fake_count - freed_values_real_count
  );
}
#endif

ValueT *malloc_value()
{
#ifdef TRACE_MEMORY
  mallocated_values_count += 1;
#endif
  ValueT *value = malloc(sizeof(ValueT));
  value->copy_count = 1;
  return value;
}

void free_value(ValueT *value)
{
  value->copy_count -= 1;
  if (value->copy_count == 0
    && value != &the_empty_list
    && value != &the_null_value)
  {
    //| this value has no existing copy left,
    //| so we can genuinely free it for real
    //| without risk of creating dangling pointers.
    //| we also checked that it was not one of
    //| the two static, unique values: the_empty_list and the_null_value.
#ifdef TRACE_MEMORY
    freed_values_real_count += 1;
#endif
    free(value);
  }
  else {
#ifdef TRACE_MEMORY
    freed_values_fake_count += 1;
#endif
  }
}
