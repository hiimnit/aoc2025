#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

  size_t cols = 0;
  size_t rows = 0;
  int c;
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      rows += 1;
      break;
    }
    cols += 1;
  }
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      rows += 1;
    }
  }

  rewind(file);

  int **map = malloc((rows + 2) * sizeof(*map));
  int **copy = malloc((rows + 2) * sizeof(*copy));

  map[0] = calloc(cols + 2, sizeof(int));
  copy[0] = calloc(cols + 2, sizeof(int));

  for (size_t row = 1; row <= rows; ++row) {
    map[row] = calloc(cols + 2, sizeof(int));
    copy[row] = calloc(cols + 2, sizeof(int));

    for (size_t col = 1; col <= cols; ++col) {
      c = fgetc(file);
      map[row][col] = c == '@' ? 1 : 0;
      copy[row][col] = c == '@' ? 1 : 0;
    }

    fgetc(file); // read new line
  }

  map[rows + 1] = calloc(cols + 2, sizeof(int));
  copy[rows + 1] = calloc(cols + 2, sizeof(int));

  fclose(file);

  int p1 = 0;
  int p2 = 0;
  int first_loop = 1;

  while (1) {
    int found = 0;

    for (size_t row = 1; row <= rows; ++row) {
      for (size_t col = 1; col <= cols; ++col) {
        if (map[row][col] == 0) {
          continue;
        }

        int total = map[row - 1][col - 1] + map[row - 1][col] +
                    map[row - 1][col + 1] + map[row][col - 1] +
                    map[row][col + 1] + map[row + 1][col - 1] +
                    map[row + 1][col] + map[row + 1][col + 1];

        if (total < 4) {
          if (first_loop) {
            p1 += 1;
          }
          p2 += 1;
          found += 1;

          copy[row][col] = 0;
        }
      }
    }

    first_loop = 0;
    if (found == 0) {
      break;
    }

    for (size_t row = 1; row <= rows; ++row) {
      for (size_t col = 1; col <= cols; ++col) {
        map[row][col] = copy[row][col];
      }
    }
  }

  for (size_t row = 1; row <= rows; ++row) {
    free(map[row]);
    free(copy[row]);
  }
  free(map);
  free(copy);

  printf("part 1: %d\n", p1);
  printf("part 2: %d\n", p2);

  return 0;
}
