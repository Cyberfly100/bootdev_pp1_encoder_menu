// liftet from bootdev and slighly adapted for this project.
#pragma once

#include <stddef.h>
#include <stdlib.h>

typedef struct Stack {
  size_t count;
  size_t original_capacity;
  size_t capacity;
  int *data;
} stack_t;

stack_t *stack_new(size_t capacity);

void stack_push(stack_t *stack, int screen_index);
int stack_pop(stack_t *stack);

void stack_clear(stack_t *stack);

void stack_free(stack_t *stack);
void stack_remove_nulls(stack_t *stack);