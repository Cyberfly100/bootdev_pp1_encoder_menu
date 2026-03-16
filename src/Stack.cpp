#include "Stack.h"

void stack_push(int_stack_t *stack, void *item) {
  if (stack->count == stack->capacity) {
    // Double stack capacity to avoid reallocing often
    stack->capacity *= 2;
    stack->data = (void **)realloc(stack->data, stack->capacity * sizeof(void *));
    if (stack->data == NULL) {
      // Unable to realloc, just exit :) get gud
      exit(1);
    }
  }

  stack->data[stack->count] = item;
  stack->count++;

  return;
}

void *stack_pop(int_stack_t *stack) {
  if (stack->count == 0) {
    return NULL;
  }

  stack->count--;
  return stack->data[stack->count];
}

void stack_free(int_stack_t *stack) {
  if (stack == NULL) {
    return;
  }

  if (stack->data != NULL) {
    // Free the allocated contents
    for (size_t i = 0; i < stack->count; i++) {
      if (stack->data[i] != NULL) {
        free(stack->data[i]);
      }
    }
    free(stack->data);
  }

  free(stack);
}

void stack_clear(int_stack_t **stack) {
  int_stack_t *old_stack = *stack;
  *stack = stack_new(old_stack->original_capacity);
  stack_free(old_stack);
}


void stack_remove_nulls(int_stack_t *stack) {
  size_t new_count = 0;

  // Iterate through the stack and compact non-NULL pointers.
  for (size_t i = 0; i < stack->count; ++i) {
    if (stack->data[i] != NULL) {
      stack->data[new_count++] = stack->data[i];
    }
  }

  // Update the count to reflect the new number of elements.
  stack->count = new_count;

  // Optionally, you might want to zero out the remaining slots.
  for (size_t i = new_count; i < stack->capacity; ++i) {
    stack->data[i] = NULL;
  }
}

void stack_push_int(int_stack_t *stack, int value) {
  int *p = (int *)malloc(sizeof(int));
  if (p == NULL) {
    // Unable to allocate, exit
    exit(1);
  }
  *p = value;
  stack_push(stack, p);
}

int stack_pop_int(int_stack_t *stack) {
  void *p = stack_pop(stack);
  if (p == NULL) {
    // Stack was empty, return 0 as error (though usage should prevent this)
    return 0;
  }
  int value = *(int *)p;
  free(p);
  return value;
}

int_stack_t *stack_new(size_t capacity) {
  int_stack_t *stack = (int_stack_t *)malloc(sizeof(int_stack_t));
  if (stack == NULL) {
    return NULL;
  }

  stack->count = 0;
  stack->capacity = capacity;
  stack->original_capacity = capacity;
  stack->data = (void **)malloc(stack->capacity * sizeof(void *));
  if (stack->data == NULL) {
    free(stack);
    return NULL;
  }

  return stack;
}