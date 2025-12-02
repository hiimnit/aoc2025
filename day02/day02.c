#include <stdio.h>
#include <stdlib.h>

void format_int(size_t number, char **str, size_t *length) {
  *length = snprintf(NULL, 0, "%zu", number);
  *str = malloc(*length + 1);
  snprintf(*str, *length + 1, "%zu", number);
}

int is_valid(char *str, size_t length, size_t test) {
  if (length % test != 0) {
    return 1;
  }

  for (size_t t = 0; t < test; ++t) {
    char c = str[t];

    for (size_t i = t + test; i < length; i += test) {
      if (str[i] != c) {
        return 1;
      }
    }
  }

  return 0;
}

int is_valid_p1(char *str, size_t length) {
  return is_valid(str, length, length / 2);
}

int is_valid_p2(char *str, size_t length) {
  for (size_t test = 1; test <= length / 2; ++test) {
    if (!is_valid(str, length, test)) {
      return 0;
    }
  }

  return 1;
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

  char separator = '\0';
  size_t from, to;
  size_t p1 = 0;
  size_t p2 = 0;

  char *str;
  size_t length;

  while (1) {
    if (fscanf(file, "%zu-%zu", &from, &to) != 2) {
      break;
    }

    for (size_t number = from; number <= to; ++number) {
      format_int(number, &str, &length);
      if (!is_valid_p1(str, length)) {
        p1 += number;
      }
      if (!is_valid_p2(str, length)) {
        p2 += number;
      }
      free(str);
    }

    separator = fgetc(file);
    if (separator == EOF || separator == '\n') {
      break;
    }
    if (separator != ',') {
      printf("Unexpected separator >%c<\n", separator);
      return 1;
    }
  }

  printf("part 1: %zu\n", p1);
  printf("part 2: %zu\n", p2);

  return 0;
}
