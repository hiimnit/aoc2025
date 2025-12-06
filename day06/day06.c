#include <stdio.h>
#include <stdlib.h>

size_t calculate(int *numbers, size_t length, char operation) {
  if (length == 0) {
    return 0;
  }

  size_t result = numbers[0];
  for (size_t i = 1; i < length; ++i) {
    switch (operation) {
    case '+':
      result += numbers[i];
      break;
    case '*':
      result *= numbers[i];
      break;
    default:
      printf("Unknown operation: %c\n", operation);
      exit(1);
    }
  }
  return result;
}

size_t parse_num(char **lines, size_t row, size_t col, int *number) {
  while (lines[row][col] == ' ') {
    col += 1;
  }
  while (1) {
    char c = lines[row][col];
    if (c == ' ' || c == '\n') {
      break;
    }
    *number = *number * 10 + c - '0';
    col += 1;
  }

  return col;
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

  size_t line_length = 0;
  int c = 0;
  while ((c = fgetc(file)) != '\n') {
    line_length += 1;
  }

  rewind(file);

  printf("line_length: %zu\n", line_length);

  char **lines = malloc(4 * sizeof(char *));
  size_t line_count = 0;
  char *line = malloc(line_length + 2);
  while (fgets(line, line_length + 2, file) != NULL) {
    lines[line_count++] = line;
    line = malloc(line_length + 2);
  }
  free(line);

  printf("read %zu lines\n", line_count);

  fclose(file);

  char operation = ' ';
  int numbers[5];
  size_t col = 0;

  size_t p1 = 0;

  while (col < line_length) {
    operation = ' ';
    for (size_t i = 0; i < 5; ++i) {
      numbers[i] = 0;
    }

    size_t max_length = parse_num(lines, 0, col, &numbers[0]);

    for (size_t row = 1; row < line_count - 1; ++row) {
      size_t length = parse_num(lines, row, col, &numbers[row]);
      max_length = max_length < length ? length : max_length;
    }

    p1 += calculate(numbers, line_count - 1, lines[line_count - 1][col]);
    col = max_length + 1;
  }

  printf("p1: %zu\n", p1);

  int new_block = 1;
  size_t bloc_col = 0;
  col = 0;

  size_t p2 = 0;

  while (col < line_length) {
    if (new_block) {
      operation = ' ';
      bloc_col = 0;
      for (size_t i = 0; i < 5; ++i) {
        numbers[i] = 0;
      }
      new_block = 0;
    }

    size_t empty_fields = 0;
    for (size_t row = 0; row < line_count; ++row) {
      char c = lines[row][col];
      if (c == ' ') {
        empty_fields += 1;
        continue;
      }
      if (row == line_count - 1) {
        operation = c;
        continue;
      }
      numbers[bloc_col] = numbers[bloc_col] * 10 + c - '0';
    }

    if (empty_fields == line_count) {
      new_block = 1;

      p2 += calculate(numbers, bloc_col, operation);
    }

    col += 1;
    bloc_col += 1;
  }

  p2 += calculate(numbers, bloc_col, operation);

  printf("p2: %zu\n", p2);

  for (size_t i = 0; i < line_count; ++i) {
    free(lines[i]);
  }

  return 0;
}
