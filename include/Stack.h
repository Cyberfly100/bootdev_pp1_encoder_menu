// liftet from bootdev and slighly adapted for this project.
#pragma once

#include <stddef.h>
#include <stdlib.h>

typedef enum StackType {
  STACK_TYPE_INT,
  STACK_TYPE_VOID_PTR
} stack_type_t;

typedef struct Stack {
  size_t count;
  size_t original_capacity;
  size_t capacity;
  void **data;
  stack_type_t type;
} int_stack_t;

int_stack_t *stack_new(size_t capacity, stack_type_t type);

void stack_push(int_stack_t *stack, void *item);
void *stack_pop(int_stack_t *stack);

// Int specialization functions
void stack_push_int(int_stack_t *stack, int value);
int stack_pop_int(int_stack_t *stack);

void stack_clear(int_stack_t **stack);

void stack_free(int_stack_t *stack);
void stack_remove_nulls(int_stack_t *stack);