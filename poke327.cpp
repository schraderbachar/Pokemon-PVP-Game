#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <sstream>

#include "heap.h"
#include "poke327.h"
#include "character.h"
#include "io.h"
#include "file_parsing.h"

typedef struct queue_node
{
  int x, y;
  struct queue_node *next;
} queue_node_t;

world_t world;

pair_t all_dirs[8] = {
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1},
};

static int32_t path_cmp(const void *key, const void *with)
{
  return ((path_t *)key)->cost - ((path_t *)with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map_t *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint16_t x, y;

  if (!initialized)
  {
    for (y = 0; y < MAP_Y; y++)
    {
      for (x = 0; x < MAP_X; x++)
      {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *)heap_remove_min(&h)))
  {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x])
    {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y])
      {
        mapxy(x, y) = ter_path;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1))))
    {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]]
                                               .hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]))))
    {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
                                           [p->pos[dim_x] - 1]
                                               .hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]))))
    {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
                                           [p->pos[dim_x] + 1]
                                               .hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1))))
    {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]]
                                               .hn);
    }
  }
}

static int build_paths(map_t *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1)
  {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1)
  {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1)
  {
    if (m->s == -1)
    {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }
    else
    {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1)
  {
    if (m->s == -1)
    {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }
    else
    {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1)
  {
    if (m->e == -1)
    {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }
    else
    {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1)
  {
    if (m->e == -1)
    {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }
    else
    {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
    {1, 4, 7, 4, 1},
    {4, 16, 26, 16, 4},
    {7, 26, 41, 26, 7},
    {4, 16, 26, 16, 4},
    {1, 4, 7, 4, 1}};

static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof(height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20)
  {
    do
    {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1)
    {
      head = tail = (queue_node_t *)malloc(sizeof(*tail));
    }
    else
    {
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head)
  {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1])
    {
      height[y - 1][x - 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1])
    {
      height[y][x - 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1])
    {
      height[y + 1][x - 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x])
    {
      height[y - 1][x] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x])
    {
      height[y + 1][x] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1])
    {
      height[y - 1][x + 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1])
    {
      height[y][x + 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1])
    {
      height[y + 1][x + 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      for (s = t = p = 0; p < 5; p++)
      {
        for (q = 0; q < 5; q++)
        {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X)
          {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      for (s = t = p = 0; p < 5; p++)
      {
        for (q = 0; q < 5; q++)
        {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X)
          {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map_t *m, pair_t p)
{
  do
  {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]) == ter_path) &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path)) ||
         ((mapxy(p[dim_x] + 2, p[dim_y]) == ter_path) &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path)) ||
         ((mapxy(p[dim_x], p[dim_y] - 1) == ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path)) ||
         ((mapxy(p[dim_x], p[dim_y] + 2) == ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path))) &&
        (((mapxy(p[dim_x], p[dim_y]) != ter_mart) &&
          (mapxy(p[dim_x], p[dim_y]) != ter_center) &&
          (mapxy(p[dim_x] + 1, p[dim_y]) != ter_mart) &&
          (mapxy(p[dim_x] + 1, p[dim_y]) != ter_center) &&
          (mapxy(p[dim_x], p[dim_y] + 1) != ter_mart) &&
          (mapxy(p[dim_x], p[dim_y] + 1) != ter_center) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x], p[dim_y]) != ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y]) != ter_path) &&
          (mapxy(p[dim_x], p[dim_y] + 1) != ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path))))
    {
      break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x], p[dim_y]) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]) = ter_mart;
  mapxy(p[dim_x], p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x], p[dim_y]) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]) = ter_center;
  mapxy(p[dim_x], p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(map_t *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;

  r = t = 0;

  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1 : MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1 : MAP_X;

  for (q = miny; q < maxy; q++)
  {
    for (p = minx; p < maxx; p++)
    {
      if (q != y || p != x)
      {
        if (m->map[q][p] == ter_mountain ||
            m->map[q][p] == ter_boulder)
        {
          r++;
        }
        else if (m->map[q][p] == ter_forest ||
                 m->map[q][p] == ter_tree)
        {
          t++;
        }
      }
    }
  }

  if (t == r)
  {
    return rand() & 1 ? ter_boulder : ter_tree;
  }
  else if (t > r)
  {
    if (rand() % 10)
    {
      return ter_tree;
    }
    else
    {
      return ter_boulder;
    }
  }
  else
  {
    if (rand() % 10)
    {
      return ter_boulder;
    }
    else
    {
      return ter_tree;
    }
  }
}

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;

  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof(m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++)
  {
    do
    {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0)
    {
      type = ter_grass;
    }
    else if (i == num_grass)
    {
      type = ter_clearing;
    }
    else if (i == num_grass + num_clearing)
    {
      type = ter_mountain;
    }
    else if (i == num_grass + num_clearing + num_mountain)
    {
      type = ter_forest;
    }
    else if (i == num_grass + num_clearing + num_mountain + num_forest)
    {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0)
    {
      head = tail = (queue_node_t *)malloc(sizeof(*tail));
    }
    else
    {
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head)
  {
    x = head->x;
    y = head->y;
    type = m->map[y][x];

    if (x - 1 >= 0 && !m->map[y][x - 1])
    {
      if ((rand() % 100) < 80)
      {
        m->map[y][x - 1] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x])
    {
      if ((rand() % 100) < 20)
      {
        m->map[y - 1][x] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x])
    {
      if ((rand() % 100) < 20)
      {
        m->map[y + 1][x] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1])
    {
      if ((rand() % 100) < 80)
      {
        m->map[y][x + 1] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      }
      else if (!added_current)
      {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1)
      {
        mapxy(x, y) = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1)
  {
    mapxy(n, 0) = ter_gate;
    mapxy(n, 1) = ter_path;
  }
  if (s != -1)
  {
    mapxy(s, MAP_Y - 1) = ter_gate;
    mapxy(s, MAP_Y - 2) = ter_path;
  }
  if (w != -1)
  {
    mapxy(0, w) = ter_gate;
    mapxy(1, w) = ter_path;
  }
  if (e != -1)
  {
    mapxy(MAP_X - 1, e) = ter_gate;
    mapxy(MAP_X - 2, e) = ter_path;
  }

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++)
  {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest && m->map[y][x] != ter_path)
    {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++)
  {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain &&
        m->map[y][x] != ter_path &&
        m->map[y][x] != ter_water)
    {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

void rand_pos(pair_t pos)
{
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker()
{
  pair_t pos;
  npc *c;

  do
  {
    rand_pos(pos);
  } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]] ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4 ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_hiker;
  c->mtype = move_hiker;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 'h';
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;
  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;

  //  printf("Hiker at %d,%d\n", pos[dim_x], pos[dim_y]);
}

void new_rival()
{
  pair_t pos;
  npc *c;

  do
  {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0 ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]] ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4 ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_rival;
  c->mtype = move_rival;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 'r';
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;
  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_swimmer()
{
  pair_t pos;
  npc *c;

  do
  {
    rand_pos(pos);
  } while (world.cur_map->map[pos[dim_y]][pos[dim_x]] != ter_water ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_swimmer;
  c->mtype = move_swim;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = SWIMMER_SYMBOL;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;
  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_char_other()
{
  pair_t pos;
  npc *c;

  do
  {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0 ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]] ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4 ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_other;
  switch (rand() % 4)
  {
  case 0:
    c->mtype = move_pace;
    c->symbol = PACER_SYMBOL;
    break;
  case 1:
    c->mtype = move_wander;
    c->symbol = WANDERER_SYMBOL;
    break;
  case 2:
    c->mtype = move_sentry;
    c->symbol = SENTRY_SYMBOL;
    break;
  case 3:
    c->mtype = move_explore;
    c->symbol = EXPLORER_SYMBOL;
    break;
  }
  rand_dir(c->dir);
  c->defeated = 0;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;
  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void place_characters()
{
  world.cur_map->num_trainers = 3;

  // Always place a hiker and a rival, then place a random number of others
  new_hiker();
  new_rival();
  new_swimmer();
  do
  {
    // higher probability of non- hikers and rivals
    switch (rand() % 10)
    {
    case 0:
      new_hiker();
      break;
    case 1:
      new_rival();
      break;
    case 2:
      new_swimmer();
      break;
    default:
      new_char_other();
      break;
    }
    /* Game attempts to continue to place trainers until the probability *
     * roll fails, but if the map is full (or almost full), it's         *
     * impossible (or very difficult) to continue to add, so we abort if *
     * we've tried MAX_TRAINER_TRIES times.                              */
  } while (++world.cur_map->num_trainers < MIN_TRAINERS ||
           ((rand() % 100) < ADD_TRAINER_PROB));
}

void init_pc()
{
  int x, y;

  do
  {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
  world.pc.symbol = '@';

  world.cur_map->cmap[y][x] = &world.pc;
  world.pc.next_turn = 0;

  heap_insert(&world.cur_map->turn, &world.pc);
}

void place_pc()
{
  character *c;

  if (world.pc.pos[dim_x] == 1)
  {
    world.pc.pos[dim_x] = MAP_X - 2;
  }
  else if (world.pc.pos[dim_x] == MAP_X - 2)
  {
    world.pc.pos[dim_x] = 1;
  }
  else if (world.pc.pos[dim_y] == 1)
  {
    world.pc.pos[dim_y] = MAP_Y - 2;
  }
  else if (world.pc.pos[dim_y] == MAP_Y - 2)
  {
    world.pc.pos[dim_y] = 1;
  }

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;

  if ((c = (character *)heap_peek_min(&world.cur_map->turn)))
  {
    world.pc.next_turn = c->next_turn;
  }
  else
  {
    world.pc.next_turn = 0;
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
int new_map(int teleport)
{
  int d, p;
  int e, w, n, s;
  int x, y;

  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]])
  {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    place_pc();

    return 0;
  }

  world.cur_map =
      world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
          (map_t *)malloc(sizeof(*world.cur_map));

  smooth_height(world.cur_map);

  if (!world.cur_idx[dim_y])
  {
    n = -1;
  }
  else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]])
  {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  }
  else
  {
    n = 3 + rand() % (MAP_X - 6);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1)
  {
    s = -1;
  }
  else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]])
  {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  }
  else
  {
    s = 3 + rand() % (MAP_X - 6);
  }
  if (!world.cur_idx[dim_x])
  {
    w = -1;
  }
  else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1])
  {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  }
  else
  {
    w = 3 + rand() % (MAP_Y - 6);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1)
  {
    e = -1;
  }
  else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1])
  {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  }
  else
  {
    e = 3 + rand() % (MAP_Y - 6);
  }

  map_terrain(world.cur_map, n, s, e, w);

  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d)
  {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d)
  {
    place_center(world.cur_map);
  }

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      world.cur_map->cmap[y][x] = NULL;
    }
  }

  heap_init(&world.cur_map->turn, cmp_char_turns, delete_character);

  if ((world.cur_idx[dim_x] == WORLD_SIZE / 2) &&
      (world.cur_idx[dim_y] == WORLD_SIZE / 2))
  {
    init_pc();
  }
  else
  {
    place_pc();
  }

  pathfind(world.cur_map);
  if (teleport)
  {
    do
    {
      world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;
      world.pc.pos[dim_x] = rand_range(1, MAP_X - 2);
      world.pc.pos[dim_y] = rand_range(1, MAP_Y - 2);
    } while (world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ||
             (move_cost[char_pc][world.cur_map->map[world.pc.pos[dim_y]]
                                                   [world.pc.pos[dim_x]]] ==
              INT_MAX) ||
             world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] < 0);
    world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;
    pathfind(world.cur_map);
  }

  place_characters();

  return 0;
}

// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.quit = 0;
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  world.char_seq_num = 0;
  new_map(0);
}

void delete_world()
{
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++)
  {
    for (x = 0; x < WORLD_SIZE; x++)
    {
      if (world.world[y][x])
      {
        heap_delete(&world.world[y][x]->turn);
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

void print_hiker_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      if (world.hiker_dist[y][x] == INT_MAX)
      {
        printf("   ");
      }
      else
      {
        printf(" %5d", world.hiker_dist[y][x]);
      }
    }
    printf("\n");
  }
}

void print_rival_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      if (world.rival_dist[y][x] == INT_MAX || world.rival_dist[y][x] < 0)
      {
        printf("   ");
      }
      else
      {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void leave_map(pair_t d)
{
  if (d[dim_x] == 0)
  {
    world.cur_idx[dim_x]--;
  }
  else if (d[dim_y] == 0)
  {
    world.cur_idx[dim_y]--;
  }
  else if (d[dim_x] == MAP_X - 1)
  {
    world.cur_idx[dim_x]++;
  }
  else
  {
    world.cur_idx[dim_y]++;
  }
  new_map(0);
}

void game_loop()
{
  character *c;
  npc *n;
  pc *p;
  pair_t d;

  while (!world.quit)
  {
    c = (character *)heap_remove_min(&world.cur_map->turn);
    n = dynamic_cast<npc *>(c);
    p = dynamic_cast<pc *>(c);

    move_func[n ? n->mtype : move_pc](c, d);

    world.cur_map->cmap[c->pos[dim_y]][c->pos[dim_x]] = NULL;
    if (p && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 ||
              d[dim_y] == 0 || d[dim_y] == MAP_Y - 1))
    {
      leave_map(d);
      d[dim_x] = c->pos[dim_x];
      d[dim_y] = c->pos[dim_y];
    }
    world.cur_map->cmap[d[dim_y]][d[dim_x]] = c;

    if (p)
    {
      pathfind(world.cur_map);
    }

    c->next_turn += move_cost[n ? n->ctype : char_pc]
                             [world.cur_map->map[d[dim_y]][d[dim_x]]];

    c->pos[dim_y] = d[dim_y];
    c->pos[dim_x] = d[dim_x];

    heap_insert(&world.cur_map->turn, c);
  }
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s [-s|--seed <seed>]\n", s);

  exit(1);
}

int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    std::string arg = argv[1];
    if (arg == "pokemon" || arg == "moves" || arg == "pokemon_moves" || arg == "pokemon_species" || arg == "experience" || arg == "type_names" || arg == "pokemon_stats" || arg == "stats" || arg == "pokemon_types")
    {
      std::string filename;
      std::string hPath = std::getenv("HOME");
      std::string beginPath;
      std::string line;

      if (std::filesystem::is_directory("/share/cs327/pokedex"))
      {
        beginPath = "/share/cs327/pokedex/pokedex/data/csv/";
        filename = beginPath + arg + ".csv";
      }

      else if (std::filesystem::is_directory(hPath + "/poke327/pokedex"))
      {
        beginPath = "/poke327/pokedex/pokedex/data/csv/";
        filename = hPath + beginPath + arg + ".csv";
        std::cout << filename << std::endl;
      }
      else
      {
        std::cout << "Error: Could not find file" << std::endl;
        return 0;
      }
      std::ifstream input;
      input.open(filename);
      int lineCount = -2;
      while (input)
      {
        std::getline(input, line);
        lineCount++;
      }
      input.close();
      std::cout << lineCount << std::endl;
      int idx = 0;
      if (arg == "pokemon")
      {
        Pokemon *pokemonArray = new Pokemon[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "ID: " << std::endl;
            }
            else
            {
              std::cout << "ID: " + sub << std::endl;
            }
            pokemonArray[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Identifier: " << std::endl;
            }
            else
            {
              std::cout << "Identifier: " + sub << std::endl;
            }
            pokemonArray[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Species ID: " << std::endl;
            }
            else
            {
              std::cout << "Species ID: " + sub << std::endl;
            }
            pokemonArray[idx].species_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Height: " << std::endl;
            }
            else
            {
              std::cout << "Height: " + sub << std::endl;
            }
            pokemonArray[idx].height = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Weight: " << std::endl;
            }
            else
            {
              std::cout << "Weight: " + sub << std::endl;
            }
            pokemonArray[idx].weight = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Base Experience: " << std::endl;
            }
            else
            {
              std::cout << "Base Experience: " + sub << std::endl;
            }
            pokemonArray[idx].base_experience = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Order: " << std::endl;
            }
            else
            {
              std::cout << "Order: " + sub << std::endl;
            }
            pokemonArray[idx].order = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Is Default: \n"
                        << std::endl;
            }
            else
            {
              std::cout << "Is Default: " + sub + "\n"
                        << std::endl;
            }
            pokemonArray[idx].is_default = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "moves")
      {
        Moves *movesArray = new Moves[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "ID: " << std::endl;
            }
            else
            {
              std::cout << "ID: " + sub << std::endl;
            }
            movesArray[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Identifier: " << std::endl;
            }
            else
            {
              std::cout << "Identifier: " + sub << std::endl;
            }
            movesArray[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Generation ID: " << std::endl;
            }
            else
            {
              std::cout << "Generation ID: " + sub << std::endl;
            }
            movesArray[idx].generation_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Type ID " << std::endl;
            }
            else
            {
              std::cout << "Type ID: " + sub << std::endl;
            }
            movesArray[idx].type_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Power: " << std::endl;
            }
            else
            {
              std::cout << "Power: " + sub << std::endl;
            }
            movesArray[idx].power = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "PP: " << std::endl;
            }
            else
            {
              std::cout << "PP: " + sub << std::endl;
            }
            movesArray[idx].pp = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Accuracy: " << std::endl;
            }
            else
            {
              std::cout << "Accuracy: " + sub << std::endl;
            }
            movesArray[idx].accuracy = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Priority: " << std::endl;
            }
            else
            {
              std::cout << "Priority: " + sub << std::endl;
            }
            movesArray[idx].priority = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Target ID: " << std::endl;
            }
            else
            {
              std::cout << "Target ID: " + sub << std::endl;
            }
            movesArray[idx].target_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Damage Class ID: " << std::endl;
            }
            else
            {
              std::cout << "Damage Class ID: " + sub << std::endl;
            }
            movesArray[idx].damage_class_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Effect ID: " << std::endl;
            }
            else
            {
              std::cout << "Effect ID: " + sub << std::endl;
            }
            movesArray[idx].effect_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Effect Chance: " << std::endl;
            }
            else
            {
              std::cout << "Effect Chance: " + sub << std::endl;
            }
            movesArray[idx].effect_chance = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Contest Type ID: " << std::endl;
            }
            else
            {
              std::cout << "Contest Type ID: " + sub << std::endl;
            }
            movesArray[idx].contest_type_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Contest Effect ID: " << std::endl;
            }
            else
            {
              std::cout << "Contest Effect ID: " + sub << std::endl;
            }
            movesArray[idx].contest_effect_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Super Contest Effect ID: \n"
                        << std::endl;
            }
            else
            {
              std::cout << "Super Contest Effect ID: " + sub + "\n"
                        << std::endl;
            }
            movesArray[idx].super_contest_effect_id = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "pokemon_moves")
      {
        Pokemon_moves *pokemonMovesArray = new Pokemon_moves[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Pokemon ID: " << std::endl;
            }
            else
            {
              std::cout << "Pokemon ID: " + sub << std::endl;
            }
            pokemonMovesArray[idx].pokemon_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Version Group ID: " << std::endl;
            }
            else
            {
              std::cout << "Version Group ID: " + sub << std::endl;
            }
            pokemonMovesArray[idx].version_group_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Move ID: " << std::endl;
            }
            else
            {
              std::cout << "Move ID: " + sub << std::endl;
            }
            pokemonMovesArray[idx].move_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Pokemon Move Method ID: " << std::endl;
            }
            else
            {
              std::cout << "Pokemon Move Method ID: " + sub << std::endl;
            }
            pokemonMovesArray[idx].pokemon_move_method_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Level: " << std::endl;
            }
            else
            {
              std::cout << "Level: " + sub << std::endl;
            }
            pokemonMovesArray[idx].level = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Order: \n"
                        << std::endl;
            }
            else
            {
              std::cout << "Order: " + sub + "\n"
                        << std::endl;
            }
            pokemonMovesArray[idx].order = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "pokemon_species")
      {
        Pokemon_species *pokemonSpeciesArray = new Pokemon_species[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "ID: " << std::endl;
            }
            else
            {
              std::cout << "ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Identifier: " << std::endl;
            }
            else
            {
              std::cout << "Identifier: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Generation ID: " << std::endl;
            }
            else
            {
              std::cout << "Generation ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].generation_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Evolves From Species ID: " << std::endl;
            }
            else
            {
              std::cout << "Evolves From Species ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].evolves_from_species_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Evolution Chain ID: " << std::endl;
            }
            else
            {
              std::cout << "Evolution Chain ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].evolution_chain_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Color ID: " << std::endl;
            }
            else
            {
              std::cout << "Color ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].color_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Shape ID: " << std::endl;
            }
            else
            {
              std::cout << "Shape ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].shape_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Habitat ID: " << std::endl;
            }
            else
            {
              std::cout << "Habitat ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].habitat_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Gender Rate: " << std::endl;
            }
            else
            {
              std::cout << "Gender Rate: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].gender_rate = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Capture Rate: " << std::endl;
            }
            else
            {
              std::cout << "Capture Rate: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].capture_rate = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Base Happiness: " << std::endl;
            }
            else
            {
              std::cout << "Base Happiness: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].base_happiness = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Is Baby: " << std::endl;
            }
            else
            {
              std::cout << "Is Baby: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].is_baby = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Hatch Counter: " << std::endl;
            }
            else
            {
              std::cout << "Hatch Counter: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].hatch_counter = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Has Gender Differences: " << std::endl;
            }
            else
            {
              std::cout << "Has Gender Differences: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].has_gender_differences = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Growth Rate ID: " << std::endl;
            }
            else
            {
              std::cout << "Growth Rate ID: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].growth_rate_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Forms Switchable: " << std::endl;
            }
            else
            {
              std::cout << "Forms Switchable: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].forms_switchable = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Is Legendary: " << std::endl;
            }
            else
            {
              std::cout << "Is Legendary: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].is_legendary = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Is Mythical: " << std::endl;
            }
            else
            {
              std::cout << "Is Mythical: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].is_mythical = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Order: " << std::endl;
            }
            else
            {
              std::cout << "Order: " + sub << std::endl;
            }
            pokemonSpeciesArray[idx].order = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Conquest Order: \n"
                        << std::endl;
            }
            else
            {
              std::cout << "Conquest Order: " + sub + "\n"
                        << std::endl;
            }
            pokemonSpeciesArray[idx].conquest_order = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "experience")
      {
        Experience *experienceArray = new Experience[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Growth Rate ID: " << std::endl;
            }
            else
            {
              std::cout << "Growth Rate ID: " + sub << std::endl;
            }
            experienceArray[idx].growth_rate_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Level: " << std::endl;
            }
            else
            {
              std::cout << "Level: " + sub << std::endl;
            }
            experienceArray[idx].level = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              sub = "INT_MAX";
              std::cout << "Experience: \n"
                        << std::endl;
            }
            else
            {
              std::cout << "Experience: " + sub + "\n"
                        << std::endl;
            }
            experienceArray[idx].experience = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "type_names")
      {
        Type_names *typeNamesArray = new Type_names[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            std::string tempTypeId = "INT_MAX";
            if (sub.length() > 0)
            {
              tempTypeId = sub;
            }
            getline(strStream, sub, ',');
            if (sub == "9")
            {
              if (tempTypeId == "INT_MAX")
              {
                std::cout << "Type ID: " << std::endl;
              }
              else
              {
                std::cout << "Type ID: " + tempTypeId << std::endl;
              }
              std::cout << "Local Language ID: 9" << std::endl;
              typeNamesArray[idx].type_id = std::stoi(tempTypeId);
              typeNamesArray[idx].local_language_id = 9;
              getline(strStream, sub, ',');
              if (sub.length() == 0)
              {
                sub = "INT_MAX";
                std::cout << "Name: \n"
                          << std::endl;
              }
              else
              {
                std::cout << "Name: " + sub + "\n"
                          << std::endl;
              }
              typeNamesArray[idx].name = sub;
            }
            idx++;
          }
        }
      }
      else if (arg == "stats")
      {
        Stats *statsArray = new Stats[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');

            std::cout << "ID: " + sub << std::endl;

            statsArray[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
              std::cout << "here" << std::endl;
              sub = "-1";
              std::cout << "Damage Class ID: " << std::endl;
            }
            else
            {
              std::cout << "Damage Class ID: " + sub << std::endl;
            }
            statsArray[idx].damage_class_id = std::stoi(sub);
            getline(strStream, sub, ',');

            std::cout << "Identifier: " + sub << std::endl;

            statsArray[idx].identifier = sub;
            getline(strStream, sub, ',');

            std::cout << "Is Battle Only: " + sub << std::endl;

            statsArray[idx].is_battle_only = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {

              sub = "-1";
              std::cout << "Game Index: \n"
                        << std::endl;
            }
            else
            {
              std::cout << "Game Index: " + sub + "\n"
                        << std::endl;
            }

            statsArray[idx].game_index = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "pokemon_stats")
      {
        Pokemon_stats *pokemonStatsArray = new Pokemon_stats[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');

            std::cout << "Pokemon ID: " + sub << std::endl;

            pokemonStatsArray[idx].pokemon_id = std::stoi(sub);
            getline(strStream, sub, ',');

            std::cout << "Stat ID: " + sub << std::endl;

            pokemonStatsArray[idx].stat_id = std::stoi(sub);
            getline(strStream, sub, ',');

            std::cout << "Base Stat: " + sub << std::endl;

            pokemonStatsArray[idx].base_stat = std::stoi(sub);
            getline(strStream, sub, ',');

            std::cout << "Effort: " + sub + "\n"
                      << std::endl;

            pokemonStatsArray[idx].effort = std::stoi(sub);
            idx++;
          }
        }
      }
      else if (arg == "pokemon_types")
      {
        Pokemon_types *pokemonTypesArray = new Pokemon_types[lineCount]();
        input.open(filename);
        std::getline(input, line);
        while (input)
        {
          std::getline(input, line);
          if (line.length() > 0)
          {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');

            std::cout << "Pokemon ID: " + sub << std::endl;

            pokemonTypesArray[idx].pokemon_id = std::stoi(sub);
            getline(strStream, sub, ',');

            std::cout << "Type ID: " + sub << std::endl;

            pokemonTypesArray[idx].type_id = std::stoi(sub);
            getline(strStream, sub, ',');

            std::cout << "Slot: " + sub + "\n"
                      << std::endl;

            pokemonTypesArray[idx].slot = std::stoi(sub);

            idx++;
          }
        }
      }
    }
    else
    {
      std::cout << "Error: Invalid filename" << std::endl;
    }
  }
  else
  {
    std::cout << "Error: Please enter 1 filename to parse" << std::endl;
  }
  return 0;
  // int i;

  // do_seed = 1;

  // if (argc > 1)
  // {
  //   for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0)
  //   {
  //     if (argv[i][0] == '-')
  //     { /* All switches start with a dash */
  //       if (argv[i][1] == '-')
  //       {
  //         argv[i]++;    /* Make the argument have a single dash so we can */
  //         long_arg = 1; /* handle long and short args at the same place.  */
  //       }
  //       switch (argv[i][1])
  //       {
  //       case 's':
  //         if ((!long_arg && argv[i][2]) ||
  //             (long_arg && strcmp(argv[i], "-seed")) ||
  //             argc < ++i + 1 /* No more arguments */ ||
  //             !sscanf(argv[i], "%u", &seed) /* Argument is not an integer */)
  //         {
  //           usage(argv[0]);
  //         }
  //         do_seed = 0;
  //         break;
  //       default:
  //         usage(argv[0]);
  //       }
  //     }
  //     else
  //     { /* No dash */
  //       usage(argv[0]);
  //     }
  //   }
  // }

  // if (do_seed)
  // {
  //   /* Allows me to start the game more than once *
  //    * per second, as opposed to time().          */
  //   gettimeofday(&tv, NULL);
  //   seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  // }

  // printf("Using seed: %u\n", seed);
  // srand(seed);

  // io_init_terminal();

  // init_world();

  /* print_hiker_dist(); */

  /*
  do {
    print_map();
    printf("Current position is %d%cx%d%c (%d,%d).  "
           "Enter command: ",
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
           world.cur_idx[dim_x] - (WORLD_SIZE / 2),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2));
    scanf(" %c", &c);
    switch (c) {
    case 'n':
      if (world.cur_idx[dim_y]) {
        world.cur_idx[dim_y]--;
        new_map();
      }
      break;
    case 's':
      if (world.cur_idx[dim_y] < WORLD_SIZE - 1) {
        world.cur_idx[dim_y]++;
        new_map();
      }
      break;
    case 'e':
      if (world.cur_idx[dim_x] < WORLD_SIZE - 1) {
        world.cur_idx[dim_x]++;
        new_map();
      }
      break;
    case 'w':
      if (world.cur_idx[dim_x]) {
        world.cur_idx[dim_x]--;
        new_map();
      }
      break;
     case 'q':
      break;
    case 'f':
      scanf(" %d %d", &x, &y);
      if (x >= -(WORLD_SIZE / 2) && x <= WORLD_SIZE / 2 &&
          y >= -(WORLD_SIZE / 2) && y <= WORLD_SIZE / 2) {
        world.cur_idx[dim_x] = x + (WORLD_SIZE / 2);
        world.cur_idx[dim_y] = y + (WORLD_SIZE / 2);
        new_map();
      }
      break;
    case '?':
    case 'h':
      printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
             "Quit with 'q'.  '?' and 'h' print this help message.\n");
      break;
    default:
      fprintf(stderr, "%c: Invalid input.  Enter '?' for help.\n", c);
      break;
    }
  } while (c != 'q');

  */

  // game_loop();

  // delete_world();

  // io_reset_terminal();

  return 0;
}
