#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAP_SIZE 20

#define SEA -1

#define BUOY 0
#define AIRPLANE 1
#define SUBMARINE 2
#define SPY_ONE 3
#define SPY_TWO 4
#define AIRCRAFT_CARRIER 5

#define BUOY_LEN 1
#define AIRPLANE_LEN 4
#define SUBMARINE_LEN 4
#define SPY_ONE_LEN 6
#define SPY_TWO_LEN 6
#define AIRCRAFT_CARRIER_LEN 10

#define BUOY_AMOUNT __DBL_MIN_10_EXP__
#define AIRPLANE_AMOUNT 5
#define SUBMARINE_AMOUNT 5
#define SPY_ONE_AMOUNT 4
#define SPY_TWO_AMOUNT 4
#define AIRCRAFT_CARRIER_AMOUNT 2

int max_hits = (BUOY_AMOUNT * BUOY_LEN) + (AIRPLANE_AMOUNT * AIRPLANE_LEN) +
               (SUBMARINE_AMOUNT * SUBMARINE_LEN) +
               (SPY_ONE_AMOUNT * SPY_ONE_LEN) + (SPY_TWO_AMOUNT * SPY_TWO_LEN) +
               (AIRCRAFT_CARRIER_AMOUNT * AIRCRAFT_CARRIER_LEN);

int parts_format[6][10][2] = {{{0, 0}},
                              {{0, 0}, {1, 0}, {0, 1}, {0, -1}},
                              {{0, 0}, {1, 0}, {2, 0}, {3, 0}},
                              {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {0, 1}, {0, -1}},
                              {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {0, 1}, {0, -1}},
                              {{0, 0},
                               {1, 0},
                               {2, 0},
                               {3, 0},
                               {4, 0},
                               {0, -1},
                               {1, -1},
                               {2, -1},
                               {3, -1},
                               {4, -1}}};

void incializar_mapa(int map[MAP_SIZE][MAP_SIZE]) {
  for (int i = 0; i < MAP_SIZE; i++) {
    for (int j = 0; j < MAP_SIZE; j++) {
      map[i][j] = SEA;
    }
  }
}

void mostrar_mapa(int map[MAP_SIZE][MAP_SIZE]) {
  printf("   0 1 2 3 4 5 6 7 8 9 1 1 1 1 1 1 1 1 1 1\n");
  printf("                       0 1 2 3 4 5 6 7 8 9\n");
  for (int i = 0; i < MAP_SIZE; i++) {
    if (i < 10) {
      printf(" %d ", i);
    } else {
      printf("%d ", i);
    }
    for (int j = 0; j < MAP_SIZE; j++) {
      if (map[i][j] == SEA) {
        printf(". ");
      } else if (map[i][j] == -2) {
        printf("x ");
      } else {
        printf("%d ", map[i][j]);
      }
    }
    printf("\n");
  }
}

int verificar_posicoes(int map[MAP_SIZE][MAP_SIZE], int part_size,
                       int positions[][2]) {
  for (int i = 0; i < part_size; i++) {
    if (map[positions[i][1]][positions[i][0]] != SEA)
      return 0;
    if (positions[i][1] < 0 || positions[i][1] >= MAP_SIZE ||
        positions[i][0] < 0 || positions[i][0] >= MAP_SIZE)
      return 0;
  }

  return 1;
}

void rotacionar_peca(int part_size, int positions[][2], int x, int y) {
  int direction = rand() % 4;
  for (int i = 0; i < part_size; i++) {
    if (direction == 0) {
      positions[i][0] = -(-x + positions[i][0]);
      positions[i][1] = y + positions[i][1];
    } else if (direction == 1) {
      positions[i][0] = x + positions[i][0];
      positions[i][1] = y + positions[i][1];
    } else if (direction == 2) {
      int cache = positions[i][0];
      positions[i][0] = y + positions[i][1];
      positions[i][1] = -(-x + cache);
    } else if (direction == 3) {
      int cache = positions[i][0];
      positions[i][0] = y + positions[i][1];
      positions[i][1] = x + cache;
    }
  }
}

void colocar_peca(int map[MAP_SIZE][MAP_SIZE], int part_number,
                  int part_quantity) {
  int index = 0;
  while (index < part_quantity) {
    int x = rand() % MAP_SIZE;
    int y = rand() % MAP_SIZE;
    int part_size = 10;
    int positions[part_size][2];
    for (int i = 0; i < part_size; i++) {
      positions[i][0] = parts_format[part_number][i][0];
      positions[i][1] = parts_format[part_number][i][1];
    }
    rotacionar_peca(part_size, positions, x, y);
    if (verificar_posicoes(map, part_size, positions)) {
      for (int i = 0; i < part_size; i++) {
        map[positions[i][1]][positions[i][0]] = part_number;
      }
      index++;
    }
  }
}

void move(int plays_made_len, int plays_made[plays_made_len][4],
          int map[MAP_SIZE][MAP_SIZE], int hidden_map[MAP_SIZE][MAP_SIZE],
          int line, int column, int *move_type) {
  int alreadyPlayed = 0;
  for (int i = 0; i < plays_made_len; i++) {
    if (plays_made[i][0] == line && plays_made[i][1] == column) {
      alreadyPlayed = 1;
      break;
    }
  }
  if (!alreadyPlayed) {
    if (map[line][column] == SEA) {
      plays_made[plays_made_len][3] = SEA;
      hidden_map[line][column] = -2;
      *move_type = -1;
    } else {
      plays_made[plays_made_len][3] = map[line][column];
      hidden_map[line][column] = map[line][column];
      *move_type = 1;
    }
  } else {
    *move_type = 0;
  }
}

int is_marked(int plays_made_len, int plays_made[plays_made_len][4], int line,
              int column) {
  int already_marked = 0;

  if (line > 19 || line < 0 || column > 19 || column < 0) {
    return 1;
  }

  for (int i = 0; i < plays_made_len; i++) {
    if (plays_made[i][0] == line && plays_made[i][1] == column) {
      already_marked = 1;
    }
  }

  return already_marked;
}

void last_occurrence(int plays_made_len, int plays_made[plays_made_len][4],
                     int *index) {
  for (int i = plays_made_len; i > 0; i--) {
    if (plays_made[i - 1][3] != SEA && plays_made[i - 1][2] == 1) {
      *index = i - 1;
      break;
    }
  }
}

void random_play(int plays_made_len, int plays_made[plays_made_len][4],
                 int *line, int *column) {
  *line = plays_made[plays_made_len - 1][0];
  *column = plays_made[plays_made_len - 1][1];

  while (1) {
    int range = 10;
    int rand_index = rand() % plays_made_len;
    *line = plays_made[rand_index][0];
    *column = plays_made[rand_index][1];
    int rand_line = rand() % range;
    int rand_column = rand() % range;

    if (!is_marked(plays_made_len, plays_made, *line, *column + rand_column)) {
      *column = *column + rand_column;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line,
                          *column - rand_column)) {
      *column = *column - rand_column;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line + rand_line,
                          *column)) {
      *line = *line + rand_line;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line - rand_line,
                          *column)) {
      *line = *line - rand_line;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line + rand_line,
                          *column + rand_column)) {
      *column = *column + rand_column;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line - rand_line,
                          *column - rand_column)) {
      *column = *column - rand_column;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line + rand_line,
                          *column + rand_column)) {
      *line = *line + rand_line;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line - rand_line,
                          *column - rand_column)) {
      *line = *line - rand_line;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line - rand_line,
                          *column + rand_column)) {
      *line = *line + rand_line;
      break;
    } else if (!is_marked(plays_made_len, plays_made, *line + rand_line,
                          *column - rand_column)) {
      *line = *line - rand_line;
      break;
    }
  }
}

void try_to_shroud(int index, int plays_made_len,
                   int plays_made[plays_made_len][4], int *line, int *column) {
  if (!is_marked(plays_made_len, plays_made, *line, *column + 1)) {
    *column = *column + 1;
  } else if (!is_marked(plays_made_len, plays_made, *line, *column - 1)) {
    *column = *column - 1;
  } else if (!is_marked(plays_made_len, plays_made, *line + 1, *column)) {
    *line = *line + 1;
  } else if (!is_marked(plays_made_len, plays_made, *line - 1, *column)) {
    *line = *line - 1;
  } else {
    int last_index = -1;
    last_occurrence(index, plays_made, &last_index);
    if (last_index == -1) {
      random_play(plays_made_len, plays_made, line, column);
    } else {
      *line = plays_made[last_index][0];
      *column = plays_made[last_index][1];
      try_to_shroud(last_index, plays_made_len, plays_made, &*line, &*column);
    }
  }
}

void resolver(int plays_made_len, int plays_made[plays_made_len][4], int *line,
              int *column) {
  srand(time(NULL));
  if (plays_made_len == 0) {
    *line = rand() % 20;
    *column = rand() % 20;
  } else {
    int index = -1;
    last_occurrence(plays_made_len, plays_made, &index);

    if (index == -1) {
      random_play(plays_made_len, plays_made, line, column);
    } else {
      *line = plays_made[index][0];
      *column = plays_made[index][1];
      try_to_shroud(index, plays_made_len, plays_made, &*line, &*column);
    }
  }
}

void last_move(int plays_made_len, int plays_made[plays_made_len][4]) {
  printf("+----------------------------------------+\n");
  printf("| ÚLTIMA JOGADA                          |\n");
  printf("+----------------------------------------+\n");
  if (plays_made[plays_made_len - 1][3] < 10 ||
      plays_made[plays_made_len - 1][3] < 0)
    printf("| PEÇA: %i                                |\n",
           plays_made[plays_made_len - 1][3]);
  else
    printf("| PEÇA: %i                               |\n",
           plays_made[plays_made_len - 1][3]);
  printf("+----------------------------------------+\n");
  if (plays_made[plays_made_len - 1][0] < 10 &&
      plays_made[plays_made_len - 1][1] < 10)
    printf("| LINHA: %i, COLUNA: %i                    |\n",
           plays_made[plays_made_len - 1][0],
           plays_made[plays_made_len - 1][1]);
  else if (plays_made[plays_made_len - 1][0] < 10 ||
           plays_made[plays_made_len - 1][1] < 10)
    printf("| LINHA: %i, COLUNA: %i                   |\n",
           plays_made[plays_made_len - 1][0],
           plays_made[plays_made_len - 1][1]);
  else
    printf("| LINHA: %i, COLUNA: %i                  |\n",
           plays_made[plays_made_len - 1][0],
           plays_made[plays_made_len - 1][1]);
  printf("+----------------------------------------+\n");
}

void scoreboard(int hits, int mistakes) {
  printf("+----------------------------------------+\n");
  printf("| PLACAR                                 |\n");
  printf("+----------------------------------------+\n");
  if (hits < 10)
    printf("| ACERTOS    | %i                         |\n", hits);
  else
    printf("| ACERTOS    | %i                        |\n", hits);
  printf("+----------------------------------------+\n");
  if (mistakes < 10)
    printf("| ERROS      | %i                         |\n", mistakes);
  else
    printf("| ERROS      | %i                        |\n", mistakes);
  printf("+----------------------------------------+\n");
  printf("\n");
}

void game_over(int isWin, int hidden_map[MAP_SIZE][MAP_SIZE], int hits,
               int mistakes) {
  if (isWin) {
    printf("+----------------------------------------+\n");
    printf("|               VOCÊ VENCEU              |\n");
    printf("+----------------------------------------+\n");

  } else {
    printf("+----------------------------------------+\n");
    printf("|               VOCÊ PERDEU              |\n");
    printf("+----------------------------------------+\n");
  }
  scoreboard(hits, mistakes);
  mostrar_mapa(hidden_map);
}

void play(int plays_made_len, int plays_made[plays_made_len][4],
          int map[MAP_SIZE][MAP_SIZE], int hidden_map[MAP_SIZE][MAP_SIZE],
          int hits, int mistakes, int withResolve) {
  int line, column, move_type;

  scoreboard(hits, mistakes);
  if (plays_made_len > 0)
    last_move(plays_made_len, plays_made);
  mostrar_mapa(hidden_map);
  printf("\n");

  if (withResolve) {
    srand(time(NULL));
    resolver(plays_made_len, plays_made, &line, &column);
  } else {
    printf("Linha -> ");
    scanf("%i", &line);
    printf("Coluna -> ");
    scanf("%i", &column);
  }

  if (line > 19 || line < 0 || column > 19 || column < 0) {
    play(plays_made_len, plays_made, map, hidden_map, hits, mistakes,
         withResolve);
  } else {
    move(plays_made_len, plays_made, map, hidden_map, line, column, &move_type);

    if (move_type == 1) {
      plays_made_len++;
      plays_made[plays_made_len - 1][2] = 1;
      hits++;
    } else if (move_type == -1) {
      plays_made_len++;
      plays_made[plays_made_len - 1][2] = -1;
      mistakes++;
    } else {
      plays_made[plays_made_len - 1][2] = 0;
    }

    plays_made[plays_made_len - 1][0] = line;
    plays_made[plays_made_len - 1][1] = column;

    // sleep(1);

    if (hits == max_hits) {
      system("clear");
      game_over(1, hidden_map, hits, mistakes);
    } else if (mistakes < (400 - max_hits)) {
      system("clear");
      play(plays_made_len, plays_made, map, hidden_map, hits, mistakes,
           withResolve);
    } else {
      system("clear");
      game_over(0, hidden_map, hits, mistakes);
    }
  }
}

void menu_map(int plays_made[2048][4], int map[MAP_SIZE][MAP_SIZE],
              int hidden_map[MAP_SIZE][MAP_SIZE]) {
  int option;

  printf("+----------------------------------------+\n");
  printf("| OPÇÕES                                 |\n");
  printf("+----------------------------------------+\n");
  printf("| 1 | JOGAR                              |\n");
  printf("+----------------------------------------+\n");
  printf("| 2 | VER MAPA                           |\n");
  printf("+----------------------------------------+\n");
  printf("| 3 | RESOLVEDOR                         |\n");
  printf("+----------------------------------------+\n");
  printf("\n");
  printf("-> ");
  scanf("%i", &option);

  if (option == 1) {
    system("clear");
    play(0, plays_made, map, hidden_map, 0, 0, 0);
  } else if (option == 2) {
    system("clear");
    mostrar_mapa(map);
    printf("\n");
    menu_map(plays_made, map, hidden_map);
  } else if (option == 3) {
    system("clear");
    play(0, plays_made, map, hidden_map, 0, 0, 1);
  } else {
    menu_map(plays_made, map, hidden_map);
  }
}

void menu(void) {
  int option;

  printf("+----------------------------------------+\n");
  printf("| OPÇÕES                                 |\n");
  printf("+----------------------------------------+\n");
  printf("| 1 | CRIAR MAPA                         |\n");
  printf("+----------------------------------------+\n");
  printf("| 2 | CARREGAR MAPA                      |\n");
  printf("+----------------------------------------+\n");
  printf("\n");
  printf("-> ");
  scanf("%i", &option);

  if (option == 1) {
    int map[MAP_SIZE][MAP_SIZE];
    int hidden_map[MAP_SIZE][MAP_SIZE];
    int plays_made[2048][4];
    incializar_mapa(map);
    incializar_mapa(hidden_map);
    srand(time(NULL));
    colocar_peca(map, AIRCRAFT_CARRIER, AIRCRAFT_CARRIER_AMOUNT);
    colocar_peca(map, SPY_TWO, SPY_TWO_AMOUNT);
    colocar_peca(map, SPY_ONE, SPY_ONE_AMOUNT);
    colocar_peca(map, SUBMARINE, SUBMARINE_AMOUNT);
    colocar_peca(map, AIRPLANE, AIRPLANE_AMOUNT);
    colocar_peca(map, BUOY, BUOY_AMOUNT);

    system("clear");
    menu_map(plays_made, map, hidden_map);
  } else if (option == 2) {

  } else {
    system("clear");
    menu();
  }
}

int main(void) {
  menu();
  return 0;
}