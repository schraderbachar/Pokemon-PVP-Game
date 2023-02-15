#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "heap.h"
#include "heap.c"
#define tallgrass ':'
#define clearing '.'
#define boulder '%'
#define tree '^'
#define road '#'
#define water '~'
#define player '@'

#define MAP_X 80
#define MAP_Y 21

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef enum dim
{
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef struct path
{
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
  int32_t traveltime;
} path_t;

typedef uint8_t pair_t[num_dims];

struct map
{
  char map[80][21];
  int left, top, right, bottom, x, y;
  pair_t pc;
};

static int32_t path_cmp(const void *key, const void *with);
static void dijkstra_path(struct map *m, pair_t from, bool ishiker);
void init_pc(struct map *mapptr);
void printmap(char map[80][21]);
void pathwe(char (*map)[21], int w, int e);
void pathns(char (*map)[21], int top, int bottom);
void biom(char (*map)[21]);
void terrainscatter(char (*map)[21]);
void pokecenter(char (*map)[21]);
void pokemart(char (*map)[21]);
void switchmap(struct map *world[401][401], int x, int y);
void init_exits(struct map *mapptr);
struct map *buildmap(int top, int bottom, int left, int right, int dist);

int main(int argc, char *argv[])
{
  srand(time(NULL));
  struct map *world[401][401];
  char input = '0';
  int x = 200, y = 200;

  world[200][200] = buildmap(0, 0, 0, 0, 0);
  printmap(world[200][200]->map);
  printf("(%d, %d)\n", x - 200, y - 200);

  while (input != 'q')
  {
    scanf("%c", &input);
    if (input == 's')
    {
      y++;
      if (x > 400 || x < 0 || y > 400 || y < 0)
      {
        printf("invalid input\nx=0 y=0");
        x = 200;
        y = 200;
      }
      else
      {
        switchmap(world, x, y);
      }
    }
    else if (input == 'n')
    {
      y--;
      if (x > 400 || x < 0 || y > 400 || y < 0)
      {
        printf("invalid input\nx=0 y=0");
        x = 200;
        y = 200;
      }
      else
      {
        switchmap(world, x, y);
      }
    }
    else if (input == 'e')
    {
      x++;
      if (x > 400 || x < 0 || y > 400 || y < 0)
      {
        printf("invalid input\nx=0 y=0");
        x = 200;
        y = 200;
      }
      else
      {
        switchmap(world, x, y);
      }
    }
    else if (input == 'w')
    {
      x--;
      if (x > 400 || x < 0 || y > 400 || y < 0)
      {
        printf("invalid input\nx=0 y=0");
        x = 200;
        y = 200;
      }
      else
      {
        switchmap(world, x, y);
      }
    }
    else if (input == 'f')
    {
      printf("enter x y \n");
      scanf("%d %d", &x, &y);
      x += 200;
      y += 200;
      if (x > 400 || x < 0 || y > 400 || y < 0)
      {
        printf("invalid input\nx=0 y=0");
        x = 200;
        y = 200;
      }
      else
      {
        switchmap(world, x, y);
      }
    }
  }
  return 0;
}

static int32_t path_cmp(const void *key, const void *with)
{
  return ((path_t *)key)->cost - ((path_t *)with)->cost;
}

static void dijkstra_path(struct map *m, pair_t from, bool ishiker)
{
  static path_t path[MAP_X][MAP_Y], *p;
  static uint32_t initialized = 0;
  heap_t h;
  int x, y;
  //   printf("0X%d  1Y%d\n",from[0],from[1]);

  if (!initialized)
  {
    for (y = 0; y < MAP_Y; y++)
    {
      for (x = 0; x < MAP_X; x++)
      {
        path[x][y].pos[dim_y] = y;
        path[x][y].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < 21; y++)
  {
    for (x = 0; x < 80; x++)
    {
      path[x][y].cost = INT_MAX;

      if (m->map[x][y] == road)
      {
        path[x][y].traveltime = 10;
      }
      else if (m->map[x][y] == 'M')
      {
        path[x][y].traveltime = 50;
      }
      else if (m->map[x][y] == 'C')
      {
        path[x][y].traveltime = 50;
      }
      else if (m->map[x][y] == tallgrass && !ishiker)
      {
        path[x][y].traveltime = 20;
      }
      else if (m->map[x][y] == tallgrass && ishiker)
      {
        path[x][y].traveltime = 15;
      }
      else if (m->map[x][y] == water)
      {
        path[x][y].cost = INT_MAX;
      }
      else if (m->map[x][y] == clearing)
      {
        path[x][y].traveltime = 10;
      }
      else if (m->map[x][y] == tree)
      {
        path[x][y].cost = INT_MAX;
      }
      else if (m->map[x][y] == boulder)
      {
        path[x][y].cost = INT_MAX;
      }
    }
  }

  path[from[0]][from[1]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      if (path[x][y].traveltime != INT_MAX)
      {
        path[x][y].hn = heap_insert(&h, &path[x][y]);
      }
    }
  }

  while ((p = heap_remove_min(&h)))
  {
    p->hn = NULL;

    /*check above*/
    if (path[p->pos[0] - 1][p->pos[1]].hn && (path[p->pos[0] - 1][p->pos[1]].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0] - 1][p->pos[1]].cost = p->cost + p->traveltime;
      // path[p->pos[0]-1][p->pos[1]].from[0] = p->pos[0];
      // path[p->pos[0]-1][p->pos[1]].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0] - 1][p->pos[1]].hn);
      // printf("1st\n");
    }
    /*check below*/
    if (path[p->pos[0] + 1][p->pos[1]].hn && (path[p->pos[0] + 1][p->pos[1]].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0] + 1][p->pos[1]].cost = p->cost + p->traveltime;
      // path[p->pos[0]+1][p->pos[1]].from[0] = p->pos[0];
      // path[p->pos[0]+1][p->pos[1]].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0] + 1][p->pos[1]].hn);
      // printf("2st\n");
    }
    /*check right*/
    if (path[p->pos[0]][p->pos[1] + 1].hn && (path[p->pos[0]][p->pos[1] + 1].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0]][p->pos[1] + 1].cost = p->cost + p->traveltime;
      // path[p->pos[0]][p->pos[1] + 1].from[0] = p->pos[0];
      // path[p->pos[0]][p->pos[1] + 1].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0]][p->pos[1] + 1].hn);
      // printf("3st\n");
    }
    /*check left*/
    if (path[p->pos[0]][p->pos[1] - 1].hn && (path[p->pos[0]][p->pos[1] - 1].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0]][p->pos[1] - 1].cost = p->cost + p->traveltime;
      // path[p->pos[0]][p->pos[1]-1].from[0] = p->pos[0];
      // path[p->pos[0]][p->pos[1]-1].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0]][p->pos[1] - 1].hn);
      // printf("4st\n");
    }
    /*check top right*/
    if (path[p->pos[0] - 1][p->pos[1] + 1].hn && (path[p->pos[0] - 1][p->pos[1] + 1].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0] - 1][p->pos[1] + 1].cost = p->cost + p->traveltime;
      // path[p->pos[0]-1][p->pos[1]+1].from[0] = p->pos[0];
      // path[p->pos[0]-1][p->pos[1]+1].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0] - 1][p->pos[1] + 1].hn);
      // printf("5st\n");
    }
    /*check bottom right*/
    if (path[p->pos[0] + 1][p->pos[1] + 1].hn && (path[p->pos[0] + 1][p->pos[1] + 1].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0] + 1][p->pos[1] + 1].cost = p->cost + p->traveltime;
      // path[p->pos[0]+1][p->pos[1]+1].from[0] = p->pos[0];
      // path[p->pos[0]+1][p->pos[1]+1].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0] + 1][p->pos[1] + 1].hn);
      // printf("6st\n");
    }
    /*check top left*/
    if (path[p->pos[0] - 1][p->pos[1] - 1].hn && (path[p->pos[0] - 1][p->pos[1] - 1].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0] - 1][p->pos[1] - 1].cost = p->cost + p->traveltime;
      // path[p->pos[0]-1][p->pos[1]-1].from[0] = p->pos[0];
      // path[p->pos[0]-1][p->pos[1]-1].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0] - 1][p->pos[1] - 1].hn);
      // printf("7st\n");
    }
    /*check bottom left*/
    if (path[p->pos[0] + 1][p->pos[1] - 1].hn && (path[p->pos[0] + 1][p->pos[1] - 1].cost > (p->cost + p->traveltime)))
    {

      path[p->pos[0] + 1][p->pos[1] - 1].cost = p->cost + p->traveltime;
      // path[p->pos[0]+1][p->pos[1]-1].from[0] = p->pos[0];
      // path[p->pos[0]+1][p->pos[1]-1].from[1] = p->pos[1];
      heap_decrease_key_no_replace(&h, path[p->pos[0] + 1][p->pos[1] - 1].hn);
      // printf("8st\n");
    }
  }

  for (int i = 0; i < 21; i++)
  {
    for (int j = 0; j < 80; j++)
    {
      if (m->map[j][i] == '%' || m->map[j][i] == '^' || path[j][i].cost == INT_MAX)
      {
        printf("   ");
      }
      else if (i == from[1] && j == from[0])
      {
        printf("@  ");
      }
      else if ((path[i][j].cost % 100) >= 0)
      {
        printf("%02d ", path[j][i].cost % 100);
      }
    }
    printf("\n");
  }
  printf("\n\n");
}

void switchmap(struct map *world[401][401], int x, int y)
{
  int n = 0, s = 0, e = 0, w = 0, dist = abs(x - 200) + abs(y - 200);
  if (world[x][y] == NULL)
  {
    world[x][y] = malloc(sizeof(*world[x][y]));
    if (world[x + 1][y] != NULL && x + 1 < 401)
    {
      e = world[x + 1][y]->left;
    }
    if (world[x - 1][y] != NULL && x - 1 > 0)
    {
      w = world[x - 1][y]->right;
    }
    if (world[x][y - 1] != NULL && y - 1 > 0)
    {
      n = world[x][y - 1]->bottom;
    }
    if (world[x][y + 1] != NULL && y + 1 < 401)
    {
      s = world[x][y + 1]->top;
    }
    world[x][y] = buildmap(n, s, w, e, dist);
  }
  if (x == 400)
  {
    for (int i = 0; i < 21; i++)
    {
      if (world[x][y]->map[79][i] == road)
      {
        world[x][y]->map[79][i] = boulder;
      }
    }
  }
  if (x == 0)
  {
    for (int i = 0; i < 21; i++)
    {
      if (world[x][y]->map[0][i] == road)
      {
        world[x][y]->map[0][i] = boulder;
      }
    }
  }
  if (y == 400)
  {
    for (int i = 0; i < 80; i++)
    {
      if (world[x][y]->map[i][20] == road)
      {
        world[x][y]->map[i][20] = boulder;
      }
    }
  }
  if (y == 0)
  {
    for (int i = 0; i < 80; i++)
    {
      if (world[x][y]->map[i][0] == road)
      {
        world[x][y]->map[i][0] = boulder;
      }
    }
  }
  printmap(world[x][y]->map);
  printf("(%d, %d)\n", x - 200, y - 200);
}

struct map *buildmap(int n, int s, int w, int e, int dist)
{
  struct map *m, *ptr;

  m = malloc(sizeof(*m));
  ptr = m;

  for (int i = 0; i < 21; i++)
  {
    for (int j = 0; j < 80; j++)
    {
      ptr->map[j][i] = '.';
    }
  }

  biom(ptr->map);
  biom(ptr->map);
  biom(ptr->map);
  biom(ptr->map);
  biom(ptr->map);
  pathwe(ptr->map, w, e);
  pathns(ptr->map, n, s);
  if (dist == 0)
  {
    pokemart(ptr->map);
    pokecenter(ptr->map);
  }
  else if (dist < 25)
  {
    if (rand() % 2 == 0)
    {
      pokemart(ptr->map);
      pokecenter(ptr->map);
    }
  }
  else if (dist < 50)
  {
    if (rand() % 3 == 0)
    {
      pokemart(ptr->map);
      pokecenter(ptr->map);
    }
  }
  else if (dist < 100)
  {
    if (rand() % 5 == 0)
    {
      pokemart(ptr->map);
    }
    if (rand() % 5 == 0)
    {
      pokecenter(ptr->map);
    }
  }
  else
  {
    if (rand() % 20 == 0)
    {
      pokemart(ptr->map);
    }
    if (rand() % 20 == 0)
    {
      pokecenter(ptr->map);
    }
  }
  init_pc(ptr);
  terrainscatter(ptr->map);
  init_exits(ptr);
  dijkstra_path(ptr, ptr->pc, true);
  dijkstra_path(ptr, ptr->pc, false);

  return m;
}

// k is pointer to map struct
void init_exits(struct map *k)
{
  for (int i = 0; i < 80; i++)
  {
    if (k->map[i][0] == road)
      k->top = i;
  }
  for (int i = 0; i < 80; i++)
  {
    if (k->map[i][20] == road)
      k->bottom = i;
  }
  for (int i = 0; i < 21; i++)
  {
    if (k->map[79][i] == road)
      k->right = i;
  }
  for (int i = 0; i < 21; i++)
  {
    if (k->map[0][i] == road)
      k->left = i;
  }
}

void pathwe(char (*map)[21], int left, int right)
{
  srand(time(NULL));
  int num21left = 3, num80turn = 3, num21right = 3, i = 0;
  num80turn = rand() % 75 + 2;
  num21left = rand() % 19 + 1;
  num21right = rand() % 19 + 1;

  if (left != 0)
  {
    num21left = left;
  }
  map[i][num21left] = road;
  while (i < num80turn)
  {
    i++;
    map[i][num21left] = road;
  }
  if (right != 0)
  {
    num21right = right;
  }
  while (num21left != num21right)
  {
    if (num21left < num21right)
    {
      num21left++;
      map[i][num21left] = road;
    }
    else if (num21left > num21right)
    {
      num21left--;
      map[i][num21left] = road;
    }
  }
  while (i < 80)
  {
    map[i][num21left] = road;
    i++;
  }
}

void pathns(char (*map)[21], int top, int bottom)
{
  int num21turn, num80top, num80bottom, i = 0;
  num80top = rand() % 78 + 1;
  num80bottom = rand() % 78 + 1;
  num21turn = rand() % 19 + 1;

  if (top != 0)
  {
    num80top = top;
  }
  map[num80top][i] = road;
  while (i < num21turn)
  {
    i++;
    map[num80top][i] = road;
  }
  if (bottom != 0)
  {
    num80bottom = bottom;
  }
  while (num80top != num80bottom)
  {
    if (num80top < num80bottom)
    {
      num80top++;
      map[num80top][i] = road;
    }
    else if (num80top > num80bottom)
    {
      num80top--;
      map[num80top][i] = road;
    }
  }
  while (i < 21)
  {
    map[num80top][i] = road;
    i++;
  }
}

void biom(char (*map)[21])
{
  char terrain = '0';

  int randx = rand() % 78 + 1;
  int randy = rand() % 19 + 1;
  char randterrain = rand() % 5;

  if (randterrain == 0 || randterrain == 1 || randterrain == 2)
  {
    terrain = tree;
  }
  else if (randterrain == 2)
  {
    terrain = water;
  }
  else
  {
    terrain = tallgrass;
  }

  for (int y = randy - 3; y <= randy + 3; y++)
  {
    for (int x = randx - 4; x <= randx + 4; x++)
    {
      if (map[x][y] == clearing)
      {
        map[x][y] = terrain;
      }
    }
  }
  for (int y = randy - 1; y <= randy + 1; y++)
  {
    for (int x = randx - 8; x <= randx + 8; x++)
    {
      if (map[x][y] == clearing)
      {
        map[x][y] = terrain;
      }
    }
  }
  for (int y = randy - 2; y <= randy + 2; y++)
  {
    for (int x = randx - 7; x <= randx + 7; x++)
    {
      if (map[x][y] == clearing)
      {
        map[x][y] = terrain;
      }
    }
  }
}

void terrainscatter(char (*map)[21])
{
  int randx, randy, randterrain;
  int total;
  total = rand() % 250 + 75;

  for (int i = 0; i < total; i++)
  {
    randx = rand() % 78 + 1;
    randy = rand() % 19 + 1;
    randterrain = rand() % 7;
    if (map[randx][randy] != road && map[randx][randy] != boulder && map[randx][randy] != 'M' && map[randx][randy] != 'C' && map[randx][randy] != '@')
    {
      randterrain = rand() % 7;
      if (randterrain == 0)
      {
        map[randx][randy] = boulder;
      }
      else if (randterrain == 1 || randterrain == 2 || randterrain == 3)
      {
        map[randx][randy] = tallgrass;
      }
      else if (randterrain == 4 || randterrain == 5)
      {
        map[randy][randx] = water;
      }
      else
      {
        map[randx][randy] = tree;
      }
    }
  }
}

void pokemart(char (*map)[21])
{
  int randx, randy, i = 1;
  bool ismart = false;
  randx = rand() % 78 + 1;
  randy = rand() % 19 + 1;
  while (i < 79)
  {
    if (map[i + 1][randy] == road && map[i][randy] != road && map[i][randy] != 'C')
    {
      map[i][randy] = 'M';
      ismart = true;
      break;
    }
    i++;
  }
  i = 0;
  while (i < 20 && !ismart)
  {
    if (map[randx][i + 1] == road && map[randx][i] != road && map[randx][i] != 'C')
    {
      map[randx][i] = 'M';
      break;
    }
    i++;
  }
}

void pokecenter(char (*map)[21])
{
  int randx, randy, i = 1;
  bool ismart = false;
  randx = rand() % 78 + 1;
  randy = rand() % 19 + 1;
  while (i < 79)
  {
    if (map[i + 1][randy] == road && map[i][randy] != road && map[i][randy] != 'M')
    {
      map[i][randy] = 'C';
      ismart = true;
      break;
    }
    i++;
  }
  i = 0;
  while (i < 20 && !ismart)
  {
    if (map[randx][i + 1] == road && map[randx][i] != road && map[randx][i] != 'M')
    {
      map[randx][i] = 'C';
      break;
    }
    i++;
  }
}

void init_pc(struct map *k)
{
  int randx, randy, i = 1;
  bool ispc = false;
  randx = rand() % 78 + 1;
  randy = rand() % 19 + 1;
  while (i < 79)
  {
    if (k->map[i][randy] == road)
    {
      k->map[i][randy] = player;
      k->pc[0] = i;
      k->pc[1] = randy;
      ispc = true;
      break;
    }
    i++;
  }
  i = 0;
  while (i < 20 && !ispc)
  {
    if (k->map[randx][i] == road)
    {
      k->map[randx][i] = player;
      k->pc[0] = randx;
      k->pc[1] = i;
      break;
    }
    i++;
  }
}

void printmap(char map[80][21])
{
  for (int i = 0; i < 80; i++)
  {
    if (map[i][20] != road)
    {
      map[i][20] = boulder;
    }
    if (map[i][0] != road)
    {
      map[i][0] = boulder;
    }
  }
  for (int i = 0; i < 21; i++)
  {
    if (map[0][i] != road)
    {
      map[0][i] = boulder;
    }
    if (map[79][i] != road)
    {
      map[79][i] = boulder;
    }
  }

  for (int i = 0; i < 21; i++)
  {
    for (int j = 0; j < 80; j++)
    {
      printf("%c", map[j][i]);
    }
    printf("\n");
  }
}
