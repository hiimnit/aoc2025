#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char **lines;
  size_t **splits;
  size_t used;
  size_t size;
} map;

map *map_new(void) {
  map *m = malloc(sizeof(map));
  m->lines = malloc(50 * sizeof(char *));
  m->splits = malloc(50 * sizeof(size_t *));
  m->used = 0;
  m->size = 50;

  return m;
}

void map_push(map *m, char *line, size_t length) {
  if (m->used * 2 + 1 >= m->size) {
    m->size *= 2;
    m->lines = realloc(m->lines, m->size * sizeof(char *));
    m->splits = realloc(m->splits, m->size * sizeof(size_t *));
  }
  m->lines[m->used] = line;
  m->splits[m->used] = calloc(length, sizeof(size_t));

  m->used += 1;
}

void map_free(map *m) {
  for (size_t i = 0; i < m->used; ++i) {
    free(m->lines[i]);
    free(m->splits[i]);
  }
  free(m->lines);
  free(m->splits);
  m->lines = NULL;
  m->splits = NULL;
  m->used = m->size = 0;
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

  map *map = map_new();
  char *line = malloc(line_length + 2);
  while (fgets(line, line_length + 2, file) != NULL) {
    map_push(map, line, line_length);
    line = malloc(line_length + 2);
  }
  free(line);

  printf("read %zu lines\n", map->used);

  fclose(file);

  for (size_t i = 0; i < map->used; ++i) {
    printf("%s", map->lines[i]);
  }

  int p1 = 0;

  for (size_t i = 0; i < map->used - 1; ++i) {
    char *line = map->lines[i];
    size_t *current_splits = map->splits[i];

    char *next_line = map->lines[i + 1];
    size_t *next_splits = map->splits[i + 1];

    for (size_t j = 0; j < line_length; ++j) {
      char c = line[j];
      if (c == '|' || c == 'S') {
        switch (next_line[j]) {
        case '.':
          next_line[j] = c == 'S' ? '|' : c;
          next_splits[j] = c == 'S' ? 1 : current_splits[j];
          break;
        case '^':
          p1 += 1;
          next_line[j - 1] = '|';
          next_line[j + 1] = '|';
          next_splits[j - 1] = next_splits[j - 1] + current_splits[j];
          next_splits[j + 1] = next_splits[j + 1] + current_splits[j];
          break;
        case '|':
          next_splits[j] = next_splits[j] + current_splits[j];
          break;
        }
      }
    }
  }

  for (size_t i = 0; i < map->used; ++i) {
    printf("%s", map->lines[i]);
  }

  printf("p1: %d\n", p1);

  size_t p2 = 0;

  for (size_t j = 0; j < line_length; ++j) {
    p2 += map->splits[map->used - 1][j];
  }

  printf("p2: %zu\n", p2);

  map_free(map);

  return 0;
}
