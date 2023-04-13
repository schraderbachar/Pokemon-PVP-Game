#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "db_parse.h"
#include "pokemon.h"

typedef struct io_message
{
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head)
  {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *)malloc(sizeof(*tmp))))
  {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof(tmp->msg), format, ap);

  va_end(ap);

  if (!io_head)
  {
    io_head = io_tail = tmp;
  }
  else
  {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head)
  {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head)
    {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character *const *)v1;
  const character *const *c2 = (const character *const *)v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **)malloc(world.cur_map->num_trainers * sizeof(*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
                                           &world.pc)
      {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof(*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      if (world.cur_map->cmap[y][x])
      {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      }
      else
      {
        switch (world.cur_map->map[y][x])
        {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
        }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  mvprintw(24, 1, "PC pokemon: %s %s", (world.pc.p_inventory[0]->is_shiny() ? "SHINY" : ""), world.pc.p_inventory[0]->get_species());
  if ((c = io_nearest_visible_trainer()))
  {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ? 'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ? 'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  }
  else
  {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do
  {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]] ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] == INT_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1)
  {
    for (i = 0; i < 13; i++)
    {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch())
    {
    case KEY_UP:
      if (offset)
      {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13))
      {
        offset++;
      }
      break;
    case 27:
      return;
    }
  }
}

static void io_list_trainers_display(npc **c,
                                     uint32_t count)
{
  uint32_t i;
  char(*s)[40]; /* pointer to array of 40 char */

  s = (char(*)[40])malloc(count * sizeof(*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++)
  {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ? "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ? "West" : "East"));
    if (count <= 13)
    {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13)
  {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  }
  else
  {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **)malloc(world.cur_map->num_trainers * sizeof(*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
                                           &world.pc)
      {
        c[count++] = dynamic_cast<npc *>(world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof(*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  mvprintw(0, 0, "Welcome to the Pokemart.  Could I interest you in some Pokeballs?");
  world.pc.inventory[POTION] = 2;
  world.pc.inventory[REVIVE] = 2;
  world.pc.inventory[POKEBALLS] = 2;
  refresh();
  getch();
}

void io_pokemon_center()
{
  mvprintw(0, 0, "Welcome to the Pokemon Center.  Nurse Joy has healed your Pokemon!");
  for (int i = 0; i < world.pc.p_count; i++)
  {
    world.pc.p_inventory[i]->current_hp = world.pc.p_inventory[i]->get_hp();
  }
  refresh();
  getch();
}

void select_pokemon()
{
  char key = ' ';
  class pokemon *p1;
  class pokemon *p2;
  class pokemon *p3;
  p1 = new class pokemon(1);
  p2 = new class pokemon(1);
  p3 = new class pokemon(1);

  while (key != '1' && key != '2' && key != '3')
  {
    mvprintw(0, 0, "Please select a starter pokemon. 1 for %s %s, 2 for %s %s, or 3 for %s %s ", (p1->is_shiny() ? "SHINY" : ""), p1->get_species(), (p2->is_shiny() ? "SHINY" : ""), p2->get_species(), (p3->is_shiny() ? "SHINY" : ""), p3->get_species());
    key = getch();
  }

  if (key == '1')
  {
    world.pc.p_inventory[0] = p1;
  }
  else if (key == '2')
  {
    world.pc.p_inventory[0] = p2;
  }
  else if (key == '3')
  {
    world.pc.p_inventory[0] = p3;
  }
}

void io_pokemon_encounter()
{
  float range;
  int distance,
      pokemon_level,
      gender,
      shiny,
      hp,
      attack,
      defense,
      speed,
      special_attack,
      special_defense;
  pokemon_db selected_pokemon = pokemon[rand() % 1093];
  pokemon_move_db p_move1, p_move2;
  move_db move1, move2;
  std::vector<pokemon_move_db> possible_moves;

  for (pokemon_move_db move : pokemon_moves)
  {
    if (move.pokemon_id == selected_pokemon.species_id && move.pokemon_move_method_id == 1)
    {
      possible_moves.push_back(move);
    }
  }

  gender = rand() % 100 < 50 ? 0 : 1;

  p_move1 = possible_moves[rand() % possible_moves.size()];
  p_move2 = possible_moves[rand() % possible_moves.size()];

  while (p_move2.move_id == p_move1.move_id)
  {
    p_move2 = possible_moves[rand() % possible_moves.size()];
  }

  for (move_db move : moves)
  {
    if (move.id == p_move1.move_id)
    {
      move1 = move;
    }
    if (move.id == p_move2.move_id)
    {
      move2 = move;
    }
  }

  distance = abs(world.cur_idx[dim_x] - 200) + abs(world.cur_idx[dim_y] - 200);
  if (distance <= 200)
  {
    range = distance / 2 + 1;
  }
  else
  {
    range = (distance - 200) / 2 + 1;
  }
  pokemon_level = rand() % ((int)range) + 1;
  if (pokemon_level > 100)
  {
    pokemon_level = 100;
  }

  shiny = rand() % 3 == 0;

  for (pokemon_stats_db stat : pokemon_stats)
  {
    if (stat.pokemon_id == selected_pokemon.id)
    {
      // IVs
      switch (stat.stat_id)
      {
      case 1:
        hp = ((stat.base_stat + rand() % 15) * 2 * pokemon_level) / 100 + pokemon_level + 10;
        break;
      case 2:
        attack = ((stat.base_stat + rand() % 15) * 2 * pokemon_level) / 100 + 5;
        break;
      case 3:
        defense = ((stat.base_stat + rand() % 15) * 2 * pokemon_level) / 100 + 5;
        break;
      case 4:
        speed = ((stat.base_stat + rand() % 15) * 2 * pokemon_level) / 100 + 5;
        break;
      case 5:
        special_attack = ((stat.base_stat + rand() % 15) * 2 * pokemon_level) / 100 + 5;
        break;
      case 6:
        special_defense = ((stat.base_stat + rand() % 15) * 2 * pokemon_level) / 100 + 5;
        break;
      }
    }
  }

  mvprintw(3, 19, " %-40s ", "");
  mvprintw(4, 19, " %-40s ", "");
  mvprintw(5, 19, " %-40s ", "");
  mvprintw(6, 19, " %-40s ", "");
  mvprintw(7, 19, " %-40s ", "");
  mvprintw(8, 19, " %-40s ", "");
  mvprintw(9, 19, " %-40s ", "");
  mvprintw(10, 19, " %-40s ", "");

  if (shiny)
  {
    mvprintw(4, 19, " A wild SHINY %s (%c) appeared!", selected_pokemon.identifier, gender == 0 ? 'M' : 'F');
  }
  else
  {
    mvprintw(4, 19, " A wild %s (%c) appeared!", selected_pokemon.identifier, gender == 0 ? 'M' : 'F');
  }
  mvprintw(6, 19, " Level: %d", pokemon_level);
  mvprintw(7, 19, " Moves:");
  mvprintw(8, 19, "  - %s", move1.identifier);
  mvprintw(9, 19, "  - %s", move2.identifier);
  mvprintw(6, 38, " HP: %d", hp);
  mvprintw(7, 38, " Attack (S): %d (%d)", attack, special_attack);
  mvprintw(8, 38, " Defense (S): %d (%d)", defense, special_defense);
  mvprintw(9, 38, " Speed: %d", speed);
  refresh();
  getch();
}

void io_encounter_pokemon()
{
  class pokemon *p;
  int current_pokemon = 0;
  int selection;
  int pokemon_selection;
  int in_battle = 1;
  int check_int;
  int item_input;

  int md = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
            abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  int minl, maxl;

  if (md <= 200)
  {
    minl = 1;
    maxl = md / 2;
  }
  else
  {
    minl = (md - 200) / 2;
    maxl = 100;
  }
  if (minl < 1)
  {
    minl = 1;
  }
  if (minl > 100)
  {
    minl = 100;
  }
  if (maxl < 1)
  {
    maxl = 1;
  }
  if (maxl > 100)
  {
    maxl = 100;
  }

  p = new class pokemon(rand() % (maxl - minl + 1) + minl);

  while (in_battle)
  {
    mvprintw(3, 10, " %-60s ", "");
    mvprintw(4, 10, " %-60s ", "");
    mvprintw(5, 10, " %-60s ", "");
    mvprintw(6, 10, " %-60s ", "");
    mvprintw(7, 10, " %-60s ", "");
    mvprintw(8, 10, " %-60s ", "");
    mvprintw(9, 10, " %-60s ", "");
    mvprintw(10, 10, " %-60s ", "");
    mvprintw(11, 10, " %-60s ", "");
    mvprintw(12, 10, " %-60s ", "");
    mvprintw(13, 10, " %-60s ", "");
    mvprintw(14, 10, " %-60s ", "");
    mvprintw(15, 10, " %-60s ", "");

    mvprintw(4, 10, "%s%s%s: HP:%d ATK:%d DEF:%d SPATK:%d SPDEF:%d SPEED:%d %s",
             p->is_shiny() ? "*" : "", p->get_species(),
             p->is_shiny() ? "*" : "", p->get_hp(), p->get_atk(),
             p->get_def(), p->get_spatk(), p->get_spdef(),
             p->get_speed(), p->get_gender_string());
    mvprintw(5, 10, "%s's moves: %s %s", p->get_species(),
             p->get_move(0), p->get_move(1));
    mvprintw(6, 10, " Battle Options:");
    mvprintw(7, 10, " 1. Fight");
    mvprintw(8, 10, " 2. Bag");
    mvprintw(9, 10, " 3. Run");
    mvprintw(10, 10, " 4. Switch Pokemon");
    mvprintw(12, 10, " Current Pokemon: LVL %d %s - %d HP", world.pc.p_inventory[current_pokemon]->level, world.pc.p_inventory[current_pokemon]->get_species(), world.pc.p_inventory[current_pokemon]->current_hp);
    mvprintw(16, 10, " %-60s ", "");

    mvprintw(6, 38, " Moves:");
    mvprintw(7, 38, " 1. %s", world.pc.p_inventory[current_pokemon]->get_move(0));
    if (p->get_move(1) != NULL)
    {
      mvprintw(8, 38, " 2. %s", world.pc.p_inventory[current_pokemon]->get_move(1));
    }
    if (p->get_move(2) != NULL)
    {
      mvprintw(9, 38, " 3. %s", world.pc.p_inventory[current_pokemon]->get_move(2));
    }
    if (p->get_move(3) != NULL)
    {
      mvprintw(10, 38, " 4. %s", world.pc.p_inventory[current_pokemon]->get_move(3));
    }

    mvprintw(13, 10, " Bag Contents:");
    mvprintw(14, 10, " A. Potions:   %d", world.pc.inventory[POTION]);
    mvprintw(15, 10, " B. Revives:   %d", world.pc.inventory[REVIVE]);
    mvprintw(16, 10, " C. Pokeballs: %d", world.pc.inventory[POKEBALLS]);
    mvprintw(13, 38, " Pokemon:");
    if (world.pc.p_inventory[0] != NULL)
    {
      mvprintw(14, 38, " 1. LVL %d %s - %d HP", world.pc.p_inventory[0]->level, world.pc.p_inventory[0]->get_species(), world.pc.p_inventory[0]->current_hp);
    }
    if (world.pc.p_inventory[1] != NULL)
    {
      mvprintw(15, 38, " 2. LVL %d %s - %d HP", world.pc.p_inventory[1]->level, world.pc.p_inventory[1]->get_species(), world.pc.p_inventory[1]->current_hp);
    }
    if (world.pc.p_inventory[2] != NULL)
    {
      mvprintw(16, 38, " 3. LVL %d %s - %d HP", world.pc.p_inventory[2]->level, world.pc.p_inventory[2]->get_species(), world.pc.p_inventory[2]->current_hp);
    }
    if (world.pc.p_inventory[3] != NULL)
    {
      mvprintw(17, 38, " 4. LVL %d %s - %d HP", world.pc.p_inventory[3]->level, world.pc.p_inventory[3]->get_species(), world.pc.p_inventory[3]->current_hp);
      mvprintw(17, 10, " %-60s ", "");
    }
    if (world.pc.p_inventory[4] != NULL)
    {
      mvprintw(18, 38, " 5. LVL %d %s - %d HP", world.pc.p_inventory[4]->level, world.pc.p_inventory[4]->get_species(), world.pc.p_inventory[4]->current_hp);
      mvprintw(18, 10, " %-60s ", "");
    }
    if (world.pc.p_inventory[5] != NULL)
    {
      mvprintw(19, 38, " 6. LVL %d %s - %d HP", world.pc.p_inventory[5]->level, world.pc.p_inventory[5]->get_species(), world.pc.p_inventory[5]->current_hp);
      mvprintw(19, 10, " %-60s ", "");
    }
    mvprintw(17, 10, " %-60s ", "");
    mvprintw(18, 10, " %-60s ", "");
    mvprintw(19, 10, " %-60s ", "");

    mvprintw(18, 10, " Please select what you would like to do.");
    refresh();

    int alive = 0;
    for (int i = 0; i < world.pc.p_count; i++)
    {
      if (world.pc.p_inventory[i]->current_hp > 0)
      {
        alive++;
      }
    }
    if (alive == 0)
    {
      mvprintw(18, 10, " %-60s ", "");
      mvprintw(18, 10, " All of your Pokemon are knocked out. You ran away.");
      refresh();
      getch();
      in_battle = 0;
      delete p;
      break;
    }

    selection = getch();
    switch (selection)
    {
    case '4':
      mvprintw(18, 10, " %-60s ", "");
      mvprintw(18, 10, " Please select a Pokemon to switch with.");
      pokemon_selection = getch();
      switch (pokemon_selection)
      {
      case '1':
        check_int = 0;
        break;
      case '2':
        check_int = 1;
        break;
      case '3':
        check_int = 2;
        break;
      case '4':
        check_int = 3;
        break;
      case '5':
        check_int = 4;
        break;
      case '6':
        check_int = 5;
        break;
      }
      if (current_pokemon == check_int)
      {
        mvprintw(18, 10, " %-60s ", "");
        mvprintw(18, 10, " Already using this Pokemon.");
      }
      else
      {
        current_pokemon = check_int;
      }
      break;
    case '3':
      mvprintw(18, 10, " %-60s ", "");
      mvprintw(18, 10, " You ran away.");
      refresh();
      getch();
      in_battle = 0;
      delete p;
      break;
    case '2':
      mvprintw(18, 10, " %-60s ", "");
      mvprintw(18, 10, " Please select an item to use");
      item_input = getch();
      switch (item_input)
      {
      case 'a':
        if (world.pc.inventory[POTION] > 0)
        {
          mvprintw(18, 10, " %-60s ", "");
          mvprintw(18, 10, " Select pokemon to use a potion on.    ");
          pokemon_selection = getch();
          switch (pokemon_selection)
          {
          case '1':
            if (world.pc.p_inventory[0] != NULL)
            {
              world.pc.inventory[POTION]--;
              if (world.pc.p_inventory[0]->current_hp + 20 > world.pc.p_inventory[0]->get_hp())
              {
                world.pc.p_inventory[0]->current_hp = world.pc.p_inventory[0]->get_hp();
              }
              else
              {
                world.pc.p_inventory[0]->current_hp += 20;
              }
              mvprintw(18, 10, " %-50s ", "");
              mvprintw(18, 10, " Used potion on %s", world.pc.p_inventory[0]->get_species());
            }
            break;
          case '2':
            if (world.pc.p_inventory[1] != NULL)
            {
              world.pc.inventory[POTION]--;
              if (world.pc.p_inventory[1]->current_hp + 20 > world.pc.p_inventory[1]->get_hp())
              {
                world.pc.p_inventory[1]->current_hp = world.pc.p_inventory[1]->get_hp();
              }
              else
              {
                world.pc.p_inventory[1]->current_hp += 20;
              }
              mvprintw(18, 10, " %-50s ", "");
              mvprintw(18, 10, " Used potion on %s", world.pc.p_inventory[1]->get_species());
            }
            break;
          case '3':
            if (world.pc.p_inventory[2] != NULL)
            {
              world.pc.inventory[POTION]--;
              if (world.pc.p_inventory[2]->current_hp + 20 > world.pc.p_inventory[2]->get_hp())
              {
                world.pc.p_inventory[2]->current_hp = world.pc.p_inventory[2]->get_hp();
              }
              else
              {
                world.pc.p_inventory[2]->current_hp += 20;
              }
              mvprintw(18, 10, " %-50s ", "");
              mvprintw(18, 10, " Used potion on %s", world.pc.p_inventory[2]->get_species());
            }
            break;
          case '4':
            if (world.pc.p_inventory[3] != NULL)
            {
              world.pc.inventory[POTION]--;
              if (world.pc.p_inventory[3]->current_hp + 20 > world.pc.p_inventory[3]->get_hp())
              {
                world.pc.p_inventory[3]->current_hp = world.pc.p_inventory[3]->get_hp();
              }
              else
              {
                world.pc.p_inventory[3]->current_hp += 20;
              }
              mvprintw(18, 10, " %-50s ", "");
              mvprintw(18, 10, " Used potion on %s", world.pc.p_inventory[3]->get_species());
            }
            break;
          case '5':
            if (world.pc.p_inventory[4] != NULL)
            {
              world.pc.inventory[POTION]--;
              if (world.pc.p_inventory[4]->current_hp + 20 > world.pc.p_inventory[4]->get_hp())
              {
                world.pc.p_inventory[4]->current_hp = world.pc.p_inventory[4]->get_hp();
              }
              else
              {
                world.pc.p_inventory[4]->current_hp += 20;
              }
              mvprintw(18, 10, " %-50s ", "");
              mvprintw(18, 10, " Used potion on %s", world.pc.p_inventory[4]->get_species());
            }
            break;
          case '6':
            if (world.pc.p_inventory[5] != NULL)
            {
              world.pc.inventory[POTION]--;
              if (world.pc.p_inventory[5]->current_hp + 20 > world.pc.p_inventory[5]->get_hp())
              {
                world.pc.p_inventory[5]->current_hp = world.pc.p_inventory[5]->get_hp();
              }
              else
              {
                world.pc.p_inventory[5]->current_hp += 20;
              }
              mvprintw(18, 10, " %-50s ", "");
              mvprintw(18, 10, " Used potion on %s", world.pc.p_inventory[5]->get_species());
            }
            break;
          default:
            mvprintw(18, 10, " %-50s ", "");
            mvprintw(18, 10, " Invalid Selection");
            break;
          }
        }
        break;
      case 'b':
        if (world.pc.inventory[REVIVE] > 0)
        {
          mvprintw(18, 10, " Select pokemon to use a revive on.    ");
          pokemon_selection = getch();
          switch (pokemon_selection)
          {
          case '1':
            if (world.pc.p_inventory[0] != NULL)
            {
              if (world.pc.p_inventory[0]->current_hp == 0)
              {
                world.pc.p_inventory[0]->current_hp = (int)(world.pc.p_inventory[0]->get_hp() / 2);
                world.pc.inventory[REVIVE]--;
                mvprintw(18, 10, " %-50s ", "");
                mvprintw(18, 10, " Used revive on %s", world.pc.p_inventory[0]->get_species());
              }
            }
            break;
          case '2':
            if (world.pc.p_inventory[1] != NULL)
            {
              if (world.pc.p_inventory[1]->current_hp == 0)
              {
                world.pc.p_inventory[1]->current_hp = (int)(world.pc.p_inventory[1]->get_hp() / 2);
                world.pc.inventory[REVIVE]--;
                mvprintw(18, 10, " %-50s ", "");
                mvprintw(18, 10, " Used revive on %s", world.pc.p_inventory[1]->get_species());
              }
            }
            break;
          case '3':
            if (world.pc.p_inventory[2] != NULL)
            {
              if (world.pc.p_inventory[2]->current_hp == 0)
              {
                world.pc.p_inventory[2]->current_hp = (int)(world.pc.p_inventory[2]->get_hp() / 2);
                world.pc.inventory[REVIVE]--;
                mvprintw(18, 10, " %-50s ", "");
                mvprintw(18, 10, " Used revive on %s", world.pc.p_inventory[2]->get_species());
              }
            }
            break;
          case '4':
            if (world.pc.p_inventory[3] != NULL)
            {
              if (world.pc.p_inventory[3]->current_hp == 0)
              {
                world.pc.p_inventory[3]->current_hp = (int)(world.pc.p_inventory[3]->get_hp() / 2);
                world.pc.inventory[REVIVE]--;
                mvprintw(18, 10, " %-50s ", "");
                mvprintw(18, 10, " Used revive on %s", world.pc.p_inventory[3]->get_species());
              }
            }
            break;
          case '5':
            if (world.pc.p_inventory[4] != NULL)
            {
              if (world.pc.p_inventory[4]->current_hp == 0)
              {
                world.pc.p_inventory[4]->current_hp = (int)(world.pc.p_inventory[4]->get_hp() / 2);
                world.pc.inventory[REVIVE]--;
                mvprintw(18, 10, " %-50s ", "");
                mvprintw(18, 10, " Used revive on %s", world.pc.p_inventory[4]->get_species());
              }
            }
            break;
          case '6':
            if (world.pc.p_inventory[5] != NULL)
            {
              if (world.pc.p_inventory[5]->current_hp == 0)
              {
                world.pc.p_inventory[5]->current_hp = (int)(world.pc.p_inventory[5]->get_hp() / 2);
                world.pc.inventory[REVIVE]--;
                mvprintw(18, 10, " %-50s ", "");
                mvprintw(18, 10, " Used revive on %s", world.pc.p_inventory[5]->get_species());
              }
            }
            break;
          default:
            mvprintw(18, 10, " %-50s ", "");
            mvprintw(18, 10, " Invalid Selection");
            break;
          }
        }
        break;
      case 'c':
        if (world.pc.inventory[POKEBALLS] > 0)
        {
          if (world.pc.p_count < 6)
          {
            mvprintw(18, 10, " %-60s ", "");
            mvprintw(18, 10, " You captured %s", p->get_species());
            getch();
            world.pc.p_inventory[world.pc.p_count] = p;
            world.pc.p_count++;
            in_battle = 0;
            break;
          }
          else
          {
            mvprintw(18, 10, " %-60s ", "");
            mvprintw(18, 10, " %s ran away.", p->get_species());
            refresh();
            getch();
            in_battle = 0;
            delete p;
            break;
          }
        }
        break;
      }
      break;
    case '1':
      if (world.pc.p_inventory[current_pokemon]->current_hp > 0)
      {
        mvprintw(18, 10, " %-60s ", "");
        mvprintw(18, 10, " Select a move to use.");
        int move_selection = getch();
        int move;
        switch (move_selection)
        {
        case '1':
          move = 0;
          break;
        case '2':
          move = 1;
          break;
        case '3':
          move = 2;
          break;
        case '4':
          move = 3;
          break;
        }
        double crit = ((rand() % 255) < (world.pc.p_inventory[current_pokemon]->get_speed() / 2)) ? 1.5 : 1;
        int damage = (int)((((((2 * world.pc.p_inventory[current_pokemon]->level) / 5) + 2) * world.pc.p_inventory[current_pokemon]->get_move_power(move) * world.pc.p_inventory[current_pokemon]->get_atk() / world.pc.p_inventory[current_pokemon]->get_def()) / 50 + 2) * crit * (rand() % 16 + 85));
        if (!((rand() % 100) < world.pc.p_inventory[current_pokemon]->get_move_accuracy(move)))
        {
          mvprintw(18, 10, " %-60s ", "");
          mvprintw(18, 10, " %s missed!", p->get_species());
          getch();
        }
        else
        {
          mvprintw(18, 10, " %-60s ", "");
          mvprintw(18, 10, " %s did %d damage!", world.pc.p_inventory[current_pokemon]->get_species(), damage);
          getch();
          if (p->current_hp - damage <= 0)
          {
            mvprintw(18, 10, " %-60s ", "");
            mvprintw(18, 10, " %s was defeated!", p->get_species());
            getch();
            in_battle = 0;
            break;
          }
          else
          {
            p->current_hp -= damage;
          }
        }
      }
      else
      {
        mvprintw(18, 10, " %-60s ", "");
        mvprintw(18, 10, " Your current Pokemon is knocked out.");
      }
      break;
    }
  }
}

static void io_display_bag()
{
  int item_input, pokemon_selection;

  mvprintw(3, 19, " %-50s ", "");
  mvprintw(4, 19, " %-50s ", "");
  mvprintw(5, 19, " %-50s ", "");
  mvprintw(6, 19, " %-50s ", "");
  mvprintw(7, 19, " %-50s ", "");
  mvprintw(8, 19, " %-50s ", "");
  mvprintw(9, 19, " %-50s ", "");
  mvprintw(10, 19, " %-50s ", "");
  mvprintw(11, 19, " %-50s ", "");
  mvprintw(12, 19, " %-50s ", "");

  mvprintw(4, 19, " Bag Contents:");
  mvprintw(5, 19, " A. Potions:   %d", world.pc.inventory[POTION]);
  mvprintw(6, 19, " B. Revives:   %d", world.pc.inventory[REVIVE]);
  mvprintw(7, 19, " C. Pokeballs: %d", world.pc.inventory[POKEBALLS]);
  mvprintw(4, 38, " Pokemon:");
  if (world.pc.p_inventory[0] != NULL)
  {
    mvprintw(5, 38, " 1. LVL %d %s - %d HP", world.pc.p_inventory[0]->level, world.pc.p_inventory[0]->get_species(), world.pc.p_inventory[0]->current_hp);
  }
  if (world.pc.p_inventory[1] != NULL)
  {
    mvprintw(6, 38, " 2. LVL %d %s - %d HP", world.pc.p_inventory[1]->level, world.pc.p_inventory[1]->get_species(), world.pc.p_inventory[1]->current_hp);
  }
  if (world.pc.p_inventory[2] != NULL)
  {
    mvprintw(7, 38, " 3. LVL %d %s - %d HP", world.pc.p_inventory[2]->level, world.pc.p_inventory[2]->get_species(), world.pc.p_inventory[2]->current_hp);
  }
  if (world.pc.p_inventory[3] != NULL)
  {
    mvprintw(8, 38, " 4. LVL %d %s - %d HP", world.pc.p_inventory[3]->level, world.pc.p_inventory[3]->get_species(), world.pc.p_inventory[3]->current_hp);
  }
  if (world.pc.p_inventory[4] != NULL)
  {
    mvprintw(9, 38, " 5. LVL %d %s - %d HP", world.pc.p_inventory[4]->level, world.pc.p_inventory[4]->get_species(), world.pc.p_inventory[4]->current_hp);
  }
  if (world.pc.p_inventory[5] != NULL)
  {
    mvprintw(10, 38, " 6. LVL %d %s - %d HP", world.pc.p_inventory[5]->level, world.pc.p_inventory[5]->get_species(), world.pc.p_inventory[5]->current_hp);
  }
  mvprintw(11, 19, " To use Potion or Revive, press A or B.");
  refresh();
  item_input = getch();
  switch (item_input)
  {
  case 'a':
    if (world.pc.inventory[POTION] > 0)
    {
      mvprintw(11, 19, " Select pokemon to use a potion on.    ");
      pokemon_selection = getch();
      switch (pokemon_selection)
      {
      case '1':
        if (world.pc.p_inventory[0] != NULL)
        {
          world.pc.inventory[POTION]--;
          if (world.pc.p_inventory[0]->current_hp + 20 > world.pc.p_inventory[0]->get_hp())
          {
            world.pc.p_inventory[0]->current_hp = world.pc.p_inventory[0]->get_hp();
          }
          else
          {
            world.pc.p_inventory[0]->current_hp += 20;
          }
          mvprintw(11, 19, " %-50s ", "");
          mvprintw(11, 19, " Used potion on %s", world.pc.p_inventory[0]->get_species());
        }
        break;
      case '2':
        if (world.pc.p_inventory[1] != NULL)
        {
          world.pc.inventory[POTION]--;
          if (world.pc.p_inventory[1]->current_hp + 20 > world.pc.p_inventory[1]->get_hp())
          {
            world.pc.p_inventory[1]->current_hp = world.pc.p_inventory[1]->get_hp();
          }
          else
          {
            world.pc.p_inventory[1]->current_hp += 20;
          }
          mvprintw(11, 19, " %-50s ", "");
          mvprintw(11, 19, " Used potion on %s", world.pc.p_inventory[1]->get_species());
        }
        break;
      case '3':
        if (world.pc.p_inventory[2] != NULL)
        {
          world.pc.inventory[POTION]--;
          if (world.pc.p_inventory[2]->current_hp + 20 > world.pc.p_inventory[2]->get_hp())
          {
            world.pc.p_inventory[2]->current_hp = world.pc.p_inventory[2]->get_hp();
          }
          else
          {
            world.pc.p_inventory[2]->current_hp += 20;
          }
          mvprintw(11, 19, " %-50s ", "");
          mvprintw(11, 19, " Used potion on %s", world.pc.p_inventory[2]->get_species());
        }
        break;
      case '4':
        if (world.pc.p_inventory[3] != NULL)
        {
          world.pc.inventory[POTION]--;
          if (world.pc.p_inventory[3]->current_hp + 20 > world.pc.p_inventory[3]->get_hp())
          {
            world.pc.p_inventory[3]->current_hp = world.pc.p_inventory[3]->get_hp();
          }
          else
          {
            world.pc.p_inventory[3]->current_hp += 20;
          }
          mvprintw(11, 19, " %-50s ", "");
          mvprintw(11, 19, " Used potion on %s", world.pc.p_inventory[3]->get_species());
        }
        break;
      case '5':
        if (world.pc.p_inventory[4] != NULL)
        {
          world.pc.inventory[POTION]--;
          if (world.pc.p_inventory[4]->current_hp + 20 > world.pc.p_inventory[4]->get_hp())
          {
            world.pc.p_inventory[4]->current_hp = world.pc.p_inventory[4]->get_hp();
          }
          else
          {
            world.pc.p_inventory[4]->current_hp += 20;
          }
          mvprintw(11, 19, " %-50s ", "");
          mvprintw(11, 19, " Used potion on %s", world.pc.p_inventory[4]->get_species());
        }
        break;
      case '6':
        if (world.pc.p_inventory[5] != NULL)
        {
          world.pc.inventory[POTION]--;
          if (world.pc.p_inventory[5]->current_hp + 20 > world.pc.p_inventory[5]->get_hp())
          {
            world.pc.p_inventory[5]->current_hp = world.pc.p_inventory[5]->get_hp();
          }
          else
          {
            world.pc.p_inventory[5]->current_hp += 20;
          }
          mvprintw(11, 19, " %-50s ", "");
          mvprintw(11, 19, " Used potion on %s", world.pc.p_inventory[5]->get_species());
        }
        break;
      default:
        mvprintw(11, 19, " %-50s ", "");
        mvprintw(11, 19, " Invalid Selection");
        break;
      }
    }
    break;
  case 'b':
    if (world.pc.inventory[REVIVE] > 0)
    {
      mvprintw(11, 19, " Select pokemon to use a revive on.    ");
      pokemon_selection = getch();
      switch (pokemon_selection)
      {
      case '1':
        if (world.pc.p_inventory[0] != NULL)
        {
          if (world.pc.p_inventory[0]->current_hp == 0)
          {
            world.pc.p_inventory[0]->current_hp = (int)(world.pc.p_inventory[0]->get_hp() / 2);
            world.pc.inventory[REVIVE]--;
            mvprintw(11, 19, " %-50s ", "");
            mvprintw(11, 19, " Used revive on %s", world.pc.p_inventory[0]->get_species());
          }
        }
        break;
      case '2':
        if (world.pc.p_inventory[1] != NULL)
        {
          if (world.pc.p_inventory[1]->current_hp == 0)
          {
            world.pc.p_inventory[1]->current_hp = (int)(world.pc.p_inventory[1]->get_hp() / 2);
            world.pc.inventory[REVIVE]--;
            mvprintw(11, 19, " %-50s ", "");
            mvprintw(11, 19, " Used revive on %s", world.pc.p_inventory[1]->get_species());
          }
        }
        break;
      case '3':
        if (world.pc.p_inventory[2] != NULL)
        {
          if (world.pc.p_inventory[2]->current_hp == 0)
          {
            world.pc.p_inventory[2]->current_hp = (int)(world.pc.p_inventory[2]->get_hp() / 2);
            world.pc.inventory[REVIVE]--;
            mvprintw(11, 19, " %-50s ", "");
            mvprintw(11, 19, " Used revive on %s", world.pc.p_inventory[2]->get_species());
          }
        }
        break;
      case '4':
        if (world.pc.p_inventory[3] != NULL)
        {
          if (world.pc.p_inventory[3]->current_hp == 0)
          {
            world.pc.p_inventory[3]->current_hp = (int)(world.pc.p_inventory[3]->get_hp() / 2);
            world.pc.inventory[REVIVE]--;
            mvprintw(11, 19, " %-50s ", "");
            mvprintw(11, 19, " Used revive on %s", world.pc.p_inventory[3]->get_species());
          }
        }
        break;
      case '5':
        if (world.pc.p_inventory[4] != NULL)
        {
          if (world.pc.p_inventory[4]->current_hp == 0)
          {
            world.pc.p_inventory[4]->current_hp = (int)(world.pc.p_inventory[4]->get_hp() / 2);
            world.pc.inventory[REVIVE]--;
            mvprintw(11, 19, " %-50s ", "");
            mvprintw(11, 19, " Used revive on %s", world.pc.p_inventory[4]->get_species());
          }
        }
        break;
      case '6':
        if (world.pc.p_inventory[5] != NULL)
        {
          if (world.pc.p_inventory[5]->current_hp == 0)
          {
            world.pc.p_inventory[5]->current_hp = (int)(world.pc.p_inventory[5]->get_hp() / 2);
            world.pc.inventory[REVIVE]--;
            mvprintw(11, 19, " %-50s ", "");
            mvprintw(11, 19, " Used revive on %s", world.pc.p_inventory[5]->get_species());
          }
        }
        break;
      default:
        mvprintw(11, 19, " %-50s ", "");
        mvprintw(11, 19, " Invalid Selection");
        break;
      }
    }
    break;
  }
}

void io_battle(character *aggressor, character *defender)
{
  npc *n = (npc *)((aggressor == &world.pc) ? defender : aggressor);
  int key;
  io_display();
  mvprintw(2, 19, " %-40s ", "");
  mvprintw(3, 19, " %-40s ", "");
  mvprintw(3, 19, " %-40s ", "");
  mvprintw(4, 19, " %-40s ", "");
  mvprintw(5, 19, " %-40s ", "");
  mvprintw(6, 19, " %-40s ", "");
  mvprintw(7, 19, " %-40s ", "");
  mvprintw(8, 19, " %-40s ", "");
  mvprintw(9, 19, " %-40s ", "");
  mvprintw(10, 19, " %-40s ", "");

  mvprintw(0, 0, "Choose a number between 1 and 3 to see that trainers (%c) pokemon: %d\n", n->symbol, sizeof(n->p_inventory));
  switch (key = getch())
  {
  case '1':
    mvprintw(3, 19, "For NPC %c at %d,%d", n->symbol, n->pos[dim_x], n->pos[dim_y]);
    mvprintw(4, 19, "Pokemon 1: %s", n->p_inventory[0]->get_species());
    mvprintw(5, 19, "Level %d", n->p_inventory[0]->level);
    mvprintw(6, 19, "HP: %d", n->p_inventory[0]->get_hp());
    mvprintw(7, 19, " Moves:");
    mvprintw(8, 19, "  - %s", n->p_inventory[0]->get_move(0));
    mvprintw(9, 19, "  - %s", n->p_inventory[0]->get_move(1));
    mvprintw(7, 38, " Attack (S): %d (%d)", n->p_inventory[0]->get_atk(), n->p_inventory[0]->get_spatk());
    mvprintw(8, 38, " Defense (S): %d (%d)", n->p_inventory[0]->get_def(), n->p_inventory[0]->get_spdef());
    mvprintw(9, 38, " Speed: %d", n->p_inventory[0]->get_speed());
    break;
  case '2':
    mvprintw(3, 19, "For NPC %c at %d,%d", n->symbol, n->pos[dim_x], n->pos[dim_y]);
    mvprintw(4, 19, "Pokemon 2: %s", n->p_inventory[1]->get_species());
    mvprintw(5, 19, "Level %d", n->p_inventory[1]->level);
    mvprintw(6, 19, "HP: %d", n->p_inventory[1]->get_hp());
    mvprintw(7, 19, " Moves:");
    mvprintw(8, 19, "  - %s", n->p_inventory[1]->get_move(0));
    mvprintw(9, 19, "  - %s", n->p_inventory[1]->get_move(1));
    mvprintw(7, 38, " Attack (S): %d (%d)", n->p_inventory[1]->get_atk(), n->p_inventory[1]->get_spatk());
    mvprintw(8, 38, " Defense (S): %d (%d)", n->p_inventory[1]->get_def(), n->p_inventory[1]->get_spdef());
    mvprintw(9, 38, " Speed: %d", n->p_inventory[1]->get_speed());
    break;
  case '3':
    mvprintw(3, 19, "For NPC %c at %d,%d", n->symbol, n->pos[dim_x], n->pos[dim_y]);
    mvprintw(4, 19, "Pokemon 3: %s", n->p_inventory[2]->get_species());
    mvprintw(5, 19, "Level %d", n->p_inventory[2]->level);
    mvprintw(6, 19, "HP: %d", n->p_inventory[2]->get_hp());
    mvprintw(7, 19, " Moves:");
    mvprintw(8, 19, "  - %s", n->p_inventory[2]->get_move(0));
    mvprintw(9, 19, "  - %s", n->p_inventory[2]->get_move(1));
    mvprintw(7, 38, " Attack (S): %d (%d)", n->p_inventory[2]->get_atk(), n->p_inventory[2]->get_spatk());
    mvprintw(8, 38, " Defense (S): %d (%d)", n->p_inventory[2]->get_def(), n->p_inventory[2]->get_spdef());
    mvprintw(9, 38, " Speed: %d", n->p_inventory[2]->get_speed());
    break;
  default:
    mvprintw(0, 0, "Didn't recognize input, going to choose the first one\n");
    mvprintw(3, 19, "For NPC: %c", n->symbol);
    mvprintw(4, 19, "Pokemon 1: %s", n->p_inventory[0]->get_species());
    mvprintw(5, 19, "Level %d", n->p_inventory[0]->level);
    mvprintw(6, 19, "HP: %d", n->p_inventory[0]->get_hp());
    mvprintw(7, 19, " Moves:");
    mvprintw(8, 19, "  - %s", n->p_inventory[0]->get_move(0));
    mvprintw(9, 19, "  - %s", n->p_inventory[0]->get_move(1));
    mvprintw(7, 38, " Attack: %d", n->p_inventory[0]->get_atk());
    mvprintw(8, 38, " Defense : %d ", n->p_inventory[0]->get_def());
    mvprintw(9, 38, " Speed: %d", n->p_inventory[0]->get_speed());
  }
  refresh();
  getch();

  n->defeated = 1;
  if (n->ctype == char_hiker || n->ctype == char_rival)
  {
    n->mtype = move_wander;
  }
}
uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input)
  {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input)
  {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart)
    {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center)
    {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]])
  {
    if (dynamic_cast<npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *)world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated)
    {
      // Some kind of greeting here would be nice
      return 1;
    }
    else if ((dynamic_cast<npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]])))
    {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }

  if (world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_grass)
  {
    if (rand() % 100 < 10)
    {
      io_pokemon_encounter();
    }
  }

  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      INT_MAX)
  {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do
  {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, (char *)"%d", &x);
  } while (x < -200 || x > 200);
  do
  {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, (char *)"%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do
  {
    switch (key = getch())
    {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'm':
      io_display_bag();
      break;
    case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}