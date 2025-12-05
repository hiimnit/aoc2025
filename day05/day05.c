#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned long int *array;
  size_t used;
  size_t size;
} int_range_array;

void range_array_init(int_range_array *array) {
  array->array = malloc(50 * sizeof(unsigned long int));
  array->used = 0;
  array->size = 50;
}

void range_array_push(int_range_array *array, unsigned long int from,
                      unsigned long int to) {
  if (array->used * 2 + 1 >= array->size) {
    array->size *= 2;
    array->array =
        realloc(array->array, array->size * sizeof(unsigned long int));
  }
  array->array[array->used * 2] = from;
  array->array[array->used * 2 + 1] = to;
  ++array->used;
}

void range_array_insert(int_range_array *array, size_t i,
                        unsigned long int from, unsigned long int to) {
  if (array->used * 2 + 1 >= array->size) {
    array->size *= 2;
    array->array =
        realloc(array->array, array->size * sizeof(unsigned long int));
  }

  memmove(array->array + (i * 2 + 2), array->array + i * 2,
          (array->used - i) * 2 * sizeof(unsigned long int));
  ++array->used;

  array->array[i * 2] = from;
  array->array[i * 2 + 1] = to;
}

void range_array_pop_many(int_range_array *array, size_t from_i, size_t to_i) {
  memmove(array->array + from_i * 2, array->array + to_i * 2,
          (array->used - to_i) * 2 * sizeof(unsigned long int));

  array->used -= to_i - from_i;
}

void range_array_get(int_range_array *array, size_t i, unsigned long int *from,
                     unsigned long int *to) {
  *from = array->array[i * 2];
  *to = array->array[i * 2 + 1];
}

void range_array_free(int_range_array *array) {
  free(array->array);
  array->array = NULL;
  array->used = array->size = 0;
}

void merge_ranges(int_range_array *array, unsigned long int from,
                  unsigned long int to) {
  size_t merge_from = -1;
  unsigned long int i_from, i_to;
  for (size_t i = 0; i < array->used; ++i) {
    range_array_get(array, i, &i_from, &i_to);

    if (to < i_from) {
      range_array_insert(array, i, from, to);
      return;
    }

    if (from > i_to) {
      continue;
    }

    if (from >= i_from && to <= i_to) {
      // range is fully contained, do nothing
      return;
    }

    merge_from = i;
    break;
  }

  if (merge_from == -1) {
    range_array_push(array, from, to);
    return;
  }

  size_t merge_to = merge_from;
  for (size_t i = merge_from + 1; i < array->used; ++i) {
    range_array_get(array, i, &i_from, &i_to);

    if (to < i_from) {
      break;
    }

    merge_to = i;
  }

  range_array_get(array, merge_from, &i_from, &i_to);
  unsigned long int min_from = i_from < from ? i_from : from;

  range_array_get(array, merge_to, &i_from, &i_to);
  unsigned long int max_to = i_to > to ? i_to : to;

  range_array_pop_many(array, merge_from, merge_to + 1);
  range_array_insert(array, merge_from, min_from, max_to);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s {input}\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    printf("Could not open file %s\n", argv[1]);
    return 1;
  }

  int_range_array range_array;
  range_array_init(&range_array);

  unsigned long int from, to;
  while (fscanf(file, "%ld-%ld\n", &from, &to) == 2) {
    merge_ranges(&range_array, from, to);
  }

  int p1 = 0;
  unsigned long int n;
  while (fscanf(file, "%ld", &n) == 1) {
    for (size_t j = 0; j < range_array.used; ++j) {
      range_array_get(&range_array, j, &from, &to);
      if (n >= from && n <= to) {
        p1 += 1;
        break;
      }
    }

    if (fgetc(file) == EOF) {
      break;
    }
  }
  printf("p1: %d\n", p1);

  fclose(file);

  unsigned long int p2 = 0;
  for (size_t j = 0; j < range_array.used; ++j) {
    range_array_get(&range_array, j, &from, &to);
    p2 += to - from + 1;
  }
  printf("p2: %ld\n", p2);

  range_array_free(&range_array);
  return 0;
}
