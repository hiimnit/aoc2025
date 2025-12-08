#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *x;
  int *y;
  int *z;
  size_t used;
  size_t size;
} map;

map *map_new(void) {
  map *m = malloc(sizeof(map));
  m->x = malloc(50 * sizeof(int));
  m->y = malloc(50 * sizeof(int));
  m->z = malloc(50 * sizeof(int));
  m->used = 0;
  m->size = 50;

  return m;
}

void map_push(map *m, int x, int y, int z) {
  if (m->used >= m->size) {
    m->size *= 2;
    m->x = realloc(m->x, m->size * sizeof(int));
    m->y = realloc(m->y, m->size * sizeof(int));
    m->z = realloc(m->z, m->size * sizeof(int));
  }
  m->x[m->used] = x;
  m->y[m->used] = y;
  m->z[m->used] = z;

  m->used += 1;
}

void map_free(map *m) {
  free(m->x);
  m->x = NULL;
  free(m->y);
  m->y = NULL;
  free(m->z);
  m->z = NULL;
  m->used = m->size = 0;
}

double distance(int fx, int fy, int fz, int tx, int ty, int tz) {
  size_t dx = fx - tx;
  size_t dy = fy - ty;
  size_t dz = fz - tz;

  double xy = sqrt(dx * dx + dy * dy);

  return sqrt(xy * xy + dz * dz);
}

typedef struct {
  double distance;
  size_t from;
  size_t to;
} distance_tuple;

distance_tuple *distance_tuple_new(double distance, size_t from, size_t to) {
  distance_tuple *t = malloc(sizeof(distance_tuple));
  t->distance = distance;
  t->from = from;
  t->to = to;
  return t;
}

int compare_distance_tuples(const void *a, const void *b) {
  const distance_tuple *arg1 = *(const distance_tuple **)a;
  const distance_tuple *arg2 = *(const distance_tuple **)b;

  if (arg1->distance < arg2->distance)
    return -1;
  if (arg1->distance > arg2->distance)
    return 1;
  return 0;
}

typedef struct {
  distance_tuple **array;
  size_t used;
  size_t size;
} distance_tuple_array;

distance_tuple_array *distance_tuple_array_new(void) {
  distance_tuple_array *a = malloc(sizeof(distance_tuple_array));
  a->array = malloc(250 * sizeof(distance_tuple *));
  a->used = 0;
  a->size = 250;

  return a;
}

void distance_tuple_array_push(distance_tuple_array *m,
                               distance_tuple *distance) {
  if (m->used >= m->size) {
    m->size *= 2;
    m->array = realloc(m->array, m->size * sizeof(distance_tuple *));
  }

  m->array[m->used] = distance;
  m->used += 1;
}

typedef struct {
  size_t *array;
  size_t used;
  size_t size;
} circuit;

void circuit_push(circuit *c, size_t i) {
  if (c->used >= c->size) {
    c->size *= 2;
    c->array = realloc(c->array, c->size * sizeof(size_t));
  }

  c->array[c->used] = i;
  c->used += 1;
}

void circuit_free(circuit *c) {
  free(c->array);
  c->array = NULL;
  c->used = c->size = 0;
}

circuit *circuit_new(size_t box) {
  circuit *c = malloc(sizeof(circuit));
  c->array = malloc(50 * sizeof(size_t));
  c->used = 0;
  c->size = 50;

  circuit_push(c, box);

  return c;
}

typedef struct {
  circuit **circuits;
  circuit **boxes;
  size_t size;
} circuits;

circuits *circuits_new(size_t length) {
  circuits *c = malloc(sizeof(circuits));

  c->circuits = malloc(length * sizeof(circuit *));
  c->boxes = malloc(length * sizeof(circuit *));
  for (size_t i = 0; i < length; ++i) {
    circuit *new = circuit_new(i);
    c->circuits[i] = new;
    c->boxes[i] = new;
  }

  c->size = length;

  return c;
}

size_t circuits_connect(circuits *cs, size_t from, size_t to) {
  circuit *from_circuit = cs->circuits[from];
  circuit *to_circuit = cs->circuits[to];

  if (from_circuit == to_circuit) {
    return 0;
  }

  for (size_t i = 0; i < to_circuit->used; ++i) {
    size_t to_box = to_circuit->array[i];
    circuit_push(from_circuit, to_box);
    cs->circuits[to_box] = from_circuit;
    cs->boxes[to_box] = NULL;
  }
  circuit_free(to_circuit);

  return from_circuit->used;
}

void circuits_free(circuits *cs) {
  for (size_t i = 0; i < cs->size; ++i) {
    if (cs->boxes[i] != NULL) {
      free(cs->boxes[i]);
    }
  }

  free(cs->circuits);
  cs->circuits = NULL;
  free(cs->boxes);
  cs->boxes = NULL;
  cs->size = 0;
}

void distance_tuple_array_free(distance_tuple_array *a) {
  for (size_t i = 0; i < a->used; ++i) {
    free(a->array[i]);
  }
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s {input} {p1_length}\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    printf("Could not open file %s\n", argv[1]);
    return 1;
  }

  int p1_length = atoi(argv[2]);

  map *map = map_new();
  int x, y, z;
  while (fscanf(file, "%d,%d,%d\n", &x, &y, &z) == 3) {
    map_push(map, x, y, z);
  }

  fclose(file);

  distance_tuple_array *distances = distance_tuple_array_new();

  for (size_t i = 0; i < map->used; ++i) {
    int fx = map->x[i];
    int fy = map->y[i];
    int fz = map->z[i];

    for (size_t j = i + 1; j < map->used; ++j) {
      int tx = map->x[j];
      int ty = map->y[j];
      int tz = map->z[j];

      distance_tuple_array_push(
          distances,
          distance_tuple_new(distance(fx, fy, fz, tx, ty, tz), i, j));
    }
  }

  qsort(distances->array, distances->used, sizeof(distance_tuple *),
        compare_distance_tuples);

  circuits *cs = circuits_new(map->used);

  for (size_t i = 0; i < p1_length; ++i) {
    circuits_connect(cs, distances->array[i]->from, distances->array[i]->to);
  }

  size_t m1 = 0, m2 = 0, m3 = 0;

  for (size_t i = 0; i < cs->size; ++i) {
    circuit *box = cs->boxes[i];
    if (box == NULL) {
      continue;
    }
    size_t length = box->used;

    if (length >= m1) {
      m3 = m2;
      m2 = m1;
      m1 = length;
    } else if (length >= m2) {
      m3 = m2;
      m2 = length;
    } else if (length >= m3) {
      m3 = length;
    }
  }

  printf("p1: %zu\n", m1 * m2 * m3);

  for (size_t i = p1_length; i < distances->used; ++i) {
    size_t from = distances->array[i]->from;
    size_t to = distances->array[i]->to;
    if (circuits_connect(cs, from, to) == map->used) {
      printf("p2: %zu\n", (size_t)map->x[from] * (size_t)map->x[to]);
      break;
    }
  }

  circuits_free(cs);
  distance_tuple_array_free(distances);
  map_free(map);

  return 0;
}
