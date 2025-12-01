#include <stdio.h>

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

  char direction = '\0';
  int steps;
  int p1 = 0;
  int p2 = 0;
  int position = 50;

  while (1) {
    if (fscanf(file, "%c%d\n", &direction, &steps) != 2) {
      break;
    }

    switch (direction) {
    case 'L':
      if (position - steps <= 0 && position != 0) {
        p2 += 1;
        steps -= position;
        position = 0;
      }
      position = (position - steps) % 100;
      break;
    case 'R':
      if (position + steps >= 100 && position != 0) {
        p2 += 1;
        steps -= 100 - position;
        position = 0;
      }
      position = (position + steps) % 100;
      break;
    default:
      printf("Unknown direction %c\n", direction);
      return 1;
    }
    if (position < 0) {
      position += 100;
    }
    if (position == 0) {
      p1 += 1;
    }
    if (steps >= 100) {
      p2 += steps / 100;
    }
  }

  printf("part 1: %d\n", p1);
  printf("part 2: %d\n", p2);

  return 0;
}
