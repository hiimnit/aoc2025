#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
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

  int line_length = 0;
  int c;
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      break;
    }
    line_length += 1;
  }

  rewind(file);

  // line length + new line + \0
  char *str = malloc(line_length + 2);
  int m1 = 0, m2 = 0;
  int m[12] = {0};
  int p1 = 0;
  unsigned long int p2 = 0;

  while (fgets(str, line_length + 2, file) != NULL) {
    m1 = str[line_length - 2] - '0';
    m2 = str[line_length - 1] - '0';

    for (int i = line_length - 3; i >= 0; --i) {
      int n = str[i] - '0';
      if (n >= m1) {
        if (m1 > m2) {
          m2 = m1;
        }
        m1 = n;
      }
    }

    p1 += m1 * 10 + m2;

    for (int i = 0; i < 12; ++i) {
      m[i] = str[line_length - 12 + i] - '0';
    }

    for (int i = line_length - 13; i >= 0; --i) {
      int n = str[i] - '0';
      if (n >= m[0]) {
        int carry = m[0];
        for (int j = 1; j < 12; ++j) {
          if (carry >= m[j]) {
            swap(&m[j], &carry);
          } else {
            break;
          }
        }
        m[0] = n;
      }
    }

    unsigned long int n = 0;
    for (int i = 0; i < 12; ++i) {
      n = n * 10 + m[i];
    }
    p2 += n;
  }

  printf("p1: %d\n", p1);
  printf("p2: %lu\n", p2);

  free(str);
  fclose(file);
  return 0;
}
