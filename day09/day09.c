#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
  int row;
  int col;
} tile;

tile *tile_new(void) { return malloc(sizeof(tile)); }

int compare_tile_row(const void *a, const void *b) {
  tile arg1 = *(const tile *)a;
  tile arg2 = *(const tile *)b;

  if (arg1.row < arg2.row)
    return -1;
  if (arg1.row > arg2.row)
    return 1;
  return 0;
}
int compare_tile_col(const void *a, const void *b) {
  tile arg1 = *(const tile *)a;
  tile arg2 = *(const tile *)b;

  if (arg1.col < arg2.col)
    return -1;
  if (arg1.col > arg2.col)
    return 1;
  return 0;
}

typedef struct {
  tile *tiles;
  size_t used;
  size_t size;
} tiles;

tiles *tiles_new(void) {
  tiles *m = malloc(sizeof(tiles));

  m->tiles = malloc(50 * sizeof(tile));
  m->used = 0;
  m->size = 50;

  return m;
}

void tiles_push(tiles *m, int row, int col) {
  if (m->used >= m->size) {
    m->size *= 2;
    m->tiles = realloc(m->tiles, m->size * sizeof(tile));
  }
  m->tiles[m->used].row = row;
  m->tiles[m->used].col = col;

  m->used += 1;
}

void tiles_pop(tiles *m) {
  if (m->used == 0) {
    return;
  }
  m->used -= 1;
}

void tiles_free(tiles *m) {
  free(m->tiles);
  m->tiles = NULL;
  m->used = m->size = 0;
}

void fill(char **map) {
  char *result = strstr(map[1], ".X.");
  if (result == NULL) {
    exit(1);
  }
  int position = result - map[1];

  tiles *queue = tiles_new();
  tiles_push(queue, 1, position + 2);

  while (queue->used > 0) {
    int row = queue->tiles[queue->used - 1].row;
    int col = queue->tiles[queue->used - 1].col;

    tiles_pop(queue);

    map[row][col] = 'X';

    if (map[row + 1][col] == '.') {
      tiles_push(queue, row + 1, col);
    }
    if (map[row - 1][col] == '.') {
      tiles_push(queue, row - 1, col);
    }
    if (map[row][col + 1] == '.') {
      tiles_push(queue, row, col + 1);
    }
    if (map[row][col - 1] == '.') {
      tiles_push(queue, row, col - 1);
    }
  }
}

int is_valid(char **map, int *row_mapping, int *col_mapping, tile t1, tile t2) {
  int t1cr = row_mapping[t1.row];
  int t1cc = row_mapping[t1.col];
  int t2cr = row_mapping[t2.row];
  int t2cc = row_mapping[t2.col];

  for (int i = MIN(t1cr, t2cr); i <= MAX(t1cr, t2cr); ++i) {
    for (int j = MIN(t1cc, t2cc); j <= MAX(t1cc, t2cc); ++j) {
      if (map[i][j] == '.') {
        return 0;
      }
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

  tiles *t = tiles_new();

  int row = 0, col = 0;
  while (fscanf(file, "%d,%d\n", &row, &col) == 2) {
    tiles_push(t, row, col);
  }

  fclose(file);

  tiles *sorted_tiles = malloc(sizeof(tiles));
  sorted_tiles->size = t->used;
  sorted_tiles->used = t->used;
  sorted_tiles->tiles = malloc(sorted_tiles->used * sizeof(tile));

  memcpy(sorted_tiles->tiles, t->tiles, sorted_tiles->used * sizeof(tile));

  qsort(sorted_tiles->tiles, sorted_tiles->used, sizeof(tile),
        compare_tile_row);

  int *row_mapping =
      calloc(sorted_tiles->tiles[sorted_tiles->used - 1].row + 1, sizeof(int));

  int index = 0;
  int previous_row = -1;
  for (size_t i = 0; i < sorted_tiles->used; ++i) {
    int row = sorted_tiles->tiles[i].row;

    if (previous_row == -1) {
      row_mapping[row] = index;
      previous_row = row;
      continue;
    }

    if (previous_row == row) {
      continue;
    }

    if (row - previous_row == 1) {
      index += 1;
      row_mapping[row] = index;
      previous_row = row;
      continue;
    }

    index += 2;
    row_mapping[row] = index;
    previous_row = row;
  }

  int compressed_rows = index + 1;

  qsort(sorted_tiles->tiles, sorted_tiles->used, sizeof(tile),
        compare_tile_col);

  int *col_mapping =
      calloc(sorted_tiles->tiles[sorted_tiles->used - 1].col + 1, sizeof(int));

  index = 0;
  int previous_col = -1;
  for (size_t i = 0; i < sorted_tiles->used; ++i) {
    int col = sorted_tiles->tiles[i].col;

    if (previous_col == -1) {
      row_mapping[col] = index;
      previous_col = col;
      continue;
    }

    if (previous_col == col) {
      continue;
    }

    if (col - previous_col == 1) {
      index += 1;
      row_mapping[col] = index;
      previous_col = col;
      continue;
    }

    index += 2;
    row_mapping[col] = index;
    previous_col = col;
  }

  tiles_free(sorted_tiles);
  sorted_tiles = NULL;

  int compressed_cols = index + 1;

  char **map = malloc(compressed_rows * sizeof(char *));
  for (size_t i = 0; i < compressed_rows; ++i) {
    map[i] = malloc((compressed_cols + 1) * sizeof(char));
    for (size_t j = 0; j < compressed_cols; ++j) {
      map[i][j] = '.';
    }
    map[i][compressed_cols] = '\0';
  }

  tile previous_tile = t->tiles[t->used - 1];
  int prow = row_mapping[previous_tile.row];
  int pcol = row_mapping[previous_tile.col];
  for (size_t i = 0; i < t->used; ++i) {
    tile tile = t->tiles[i];

    int crow = row_mapping[tile.row];
    int ccol = row_mapping[tile.col];

    map[crow][ccol] = '#';

    if (prow == crow) {
      for (int i = MIN(pcol, ccol) + 1; i < MAX(pcol, ccol); ++i) {
        map[crow][i] = 'X';
      }
    } else if (pcol == ccol) {
      for (int i = MIN(prow, crow) + 1; i < MAX(prow, crow); ++i) {
        map[i][ccol] = 'X';
      }
    } else {
      printf("Invalid input\n");
      return 1;
    }

    prow = crow;
    pcol = ccol;
  }

  fill(map);

  for (size_t i = 0; i < compressed_rows; ++i) {
    printf("%s\n", map[i]);
  }

  size_t p1 = 0, p2 = 0;

  for (size_t i = 0; i < t->used; ++i) {
    tile tile = t->tiles[i];
    for (size_t j = i + 1; j < t->used; ++j) {
      size_t area = (size_t)(abs(tile.row - t->tiles[j].row) + 1) *
                    (size_t)(abs(tile.col - t->tiles[j].col) + 1);
      if (area > p1) {
        p1 = area;
      }
      if (area > p2 &&
          is_valid(map, row_mapping, col_mapping, tile, t->tiles[j])) {
        p2 = area;
      }
    }
  }

  printf("p1: %zu\n", p1);
  printf("p2: %zu\n", p2);

  free(row_mapping);
  free(col_mapping);
  tiles_free(t);
  for (size_t i = 0; i < compressed_rows; ++i) {
    free(map[i]);
  }
  free(map);

  return 0;
}
