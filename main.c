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

#define BUOY_AMOUNT 10
#define AIRPLANE_AMOUNT 5
#define SUBMARINE_AMOUNT 5
#define SPY_ONE_AMOUNT 4
#define SPY_TWO_AMOUNT 4
#define AIRCRAFT_CARRIER_AMOUNT 2

// variavel de quantidade máxima de acertos - 118
int max_hits = (BUOY_AMOUNT * BUOY_LEN) + (AIRPLANE_AMOUNT * AIRPLANE_LEN) +
               (SUBMARINE_AMOUNT * SUBMARINE_LEN) +
               (SPY_ONE_AMOUNT * SPY_ONE_LEN) + (SPY_TWO_AMOUNT * SPY_TWO_LEN) +
               (AIRCRAFT_CARRIER_AMOUNT * AIRCRAFT_CARRIER_LEN);

//Formato das peças
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

//chamndo o arquivo para criar mapa novo
FILE *pFile;

//preenche uma matriz qualquer com -1, desde que seja 20x20 
void incializar_mapa(int map[MAP_SIZE][MAP_SIZE]) {
  for (int i = 0; i < MAP_SIZE; i++) {
    for (int j = 0; j < MAP_SIZE; j++) {
      map[i][j] = SEA;
    }
  }
}

//Mostra o mapa de inteiros formatando *(MAR) e #(ERRO)
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
        printf("* ");
      } else if (map[i][j] == -2) {
        printf("# ");
      } else {
        printf("%d ", map[i][j]);
      }
    }
    printf("\n");
  }
}

//Uma função que verifica multiplas posições, se as posições forem dentro do mapa e for igual a mar então retorna 1 (true)

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

// Recebe uma lista de posições e uma direção(opcional), altera as posições base para a direção desejada ou aleatória4
// Só recebe a direção quando o mapa é carregado

void rotate_part_positions(int part_size, int positions[][2], int x, int y,
                           char *pos, char direction_char) {
  int direction;

  // se nenhuma posição for passada ele aleaoriza um número de 0 a 3 que corresponde a um posição
  if (direction_char == 'E')
    direction = 0;
  else if (direction_char == 'D')
    direction = 1;
  else if (direction_char == 'C')
    direction = 2;
  else if (direction_char == 'B')
    direction = 3;
  else
    direction = rand() % 4;

  for (int i = 0; i < part_size; i++) {
    if (direction == 0) {
      *pos = 'E';
      positions[i][0] = -(-x + positions[i][0]);
      positions[i][1] = y + positions[i][1];
    } else if (direction == 1) {
      *pos = 'D';
      positions[i][0] = x + positions[i][0];
      positions[i][1] = y + positions[i][1];
    } else if (direction == 2) {
      *pos = 'C';
      int cache = positions[i][0];
      positions[i][0] = y + positions[i][1];
      positions[i][1] = -(-x + cache);
    } else if (direction == 3) {
      *pos = 'B';
      int cache = positions[i][0];
      positions[i][0] = y + positions[i][1];
      positions[i][1] = x + cache;
    }
  }
}

//soma a posição passada com o formato da peça
void build_part_shape(int *x, int *y, int line, int column, int part_size,
                      int part_number, int positions[part_size][2]) {
  if (line == -1 || column == -1) {
    *x = rand() % MAP_SIZE;
    *y = rand() % MAP_SIZE;
  } else {
    *x = column;
    *y = line;
  }

  for (int i = 0; i < part_size; i++) {
    positions[i][0] = parts_format[part_number][i][0];
    positions[i][1] = parts_format[part_number][i][1];
  }
}

//Coloca a peça na matriz depois de ter rotacionado e verificado se ela está em um lugar vazio e dentro do mapa, caso contrario ela procura outra posição
void colocar_peca(int map[MAP_SIZE][MAP_SIZE], int part_number,
                  int part_quantity, int isRandom, int line, int column,
                  char direction) {
  int x, y;
  char pos;
  int positions[10][2];
  if (isRandom) {
    int index = 0;
    while (index < part_quantity) {
      build_part_shape(&x, &y, line, column, 10, part_number, positions);
      rotate_part_positions(10, positions, x, y, &pos, '0');
      if (verificar_posicoes(map, 10, positions)) {
        pFile = fopen("./tabuleiro.txt", "a");
        if (pos == 'E' || pos == 'D') {
          fprintf(pFile, "%d %d %c %d\n", y, x, pos, part_number);
        } else {
          fprintf(pFile, "%d %d %c %d\n", x, y, pos, part_number);
        }
        fclose(pFile);

        for (int i = 0; i < 10; i++) {
          map[positions[i][1]][positions[i][0]] = part_number;
        }
        index++;
      }
    }
  } else {
    build_part_shape(&x, &y, line, column, 10, part_number, positions);
    if (direction == 'D' || direction == 'E') {
      rotate_part_positions(10, positions, y, x, &pos, direction);
    } else {
      rotate_part_positions(10, positions, x, y, &pos, direction);
    }
    if (verificar_posicoes(map, 10, positions)) {
      for (int i = 0; i < 10; i++) {
        map[positions[i][1]][positions[i][0]] = part_number;
      }
    }
  }
}

// Executa o movimento passado pelo usuário ou pelo resolvedor em uma nova matriz vazia
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

//Função que verifica se a posição jogada já foi marcada ou não
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

//Mostra a ultima jogada que caiu em uma peça
void last_occurrence(int plays_made_len, int plays_made[plays_made_len][4],
                     int *index) {
  for (int i = plays_made_len; i > 0; i--) {
    if (plays_made[i - 1][3] != SEA && plays_made[i - 1][2] == 1) {
      *index = i - 1;
      break;
    }
  }
}

//Aleatoriza uma jogada perto de outra jogada já feita independente se foi erro ou acerto
void random_play(int plays_made_len, int plays_made[plays_made_len][4],
                 int *line, int *column) {
  int tests[8][2] = {{1, 0}, {-1, 0},  {0, 1},  {0, -1},
                     {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

  *line = plays_made[plays_made_len - 1][0];
  *column = plays_made[plays_made_len - 1][1];

  while (1) {
    int range = 15;
    int rand_index = rand() % plays_made_len;
    *line = plays_made[rand_index][0];
    *column = plays_made[rand_index][1];
    int rand_line = rand() % range;
    int rand_column = rand() % range;

    int is_break_time = 0;
    for (int i = 0; i < 8; i++) {
      if (!is_marked(plays_made_len, plays_made,
                     *line + (rand_line * tests[i][1]),
                     *column + (rand_column * tests[i][0]))) {
        *line = *line + (rand_line * tests[i][1]);
        *column = *column + (rand_column * tests[i][0]);
        is_break_time = 1;
        break;
      }
    }

    if (is_break_time) {
      break;
    }
  }
}

//Testa a posições em volta da ultima jogada bem-sucedida
void try_to_shroud(int index, int plays_made_len,
                   int plays_made[plays_made_len][4], int *line, int *column) {
  int tests[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  int is_random_time = 1;
  for (int i = 0; i < 8; i++) {
    if (!is_marked(plays_made_len, plays_made, *line + tests[i][1],
                   *column + tests[i][0])) {
      *line = *line + tests[i][1];
      *column = *column + tests[i][0];
      is_random_time = 0;
      break;
    }
  }

  if (is_random_time) {
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

// Resolvedor
void resolver(int plays_made_len, int plays_made[plays_made_len][4], int *line,
              int *column) {
  srand(time(NULL));
  if (plays_made_len == 0) {
    *line = rand() % 20;
    *column = rand() % 20;
  } else {
    int index = -1;
    //pega a ultima jogada bem feita
    last_occurrence(plays_made_len, plays_made, &index);

    //se não teve jogada bem feita ele pega uma jogada já feita sendo erro e pega uma posição proxima
    if (index == -1) {
      random_play(plays_made_len, plays_made, line, column);
    } else {
      *line = plays_made[index][0];
      *column = plays_made[index][1];
      try_to_shroud(index, plays_made_len, plays_made, &*line, &*column);
    }

    //Caso exista uma jogada bem sucedida ele testa encima, embaixo, esquerda e direita
  }
}

//Mostra a ultima jogada feita
void last_move(int plays_made_len, int plays_made[plays_made_len][4]) {
  int parts = plays_made[plays_made_len - 1][3];
  int line = plays_made[plays_made_len - 1][0];
  int column = plays_made[plays_made_len - 1][1];

  printf("+----------------------------------------+\n");
  printf("| ÚLTIMA JOGADA                          |\n");
  printf("+----------------------------------------+\n");
  if (parts < 10 || parts < 0)
    printf("| PEÇA: %i                                |\n", parts);
  else
    printf("| PEÇA: %i                               |\n", parts);
  printf("+----------------------------------------+\n");
  if (line < 10 && column < 10)
    printf("| LINHA: %i, COLUNA: %i                    |\n", line, column);
  else if (line < 10 || column < 10)
    printf("| LINHA: %i, COLUNA: %i                   |\n", line, column);
  else
    printf("| LINHA: %i, COLUNA: %i                  |\n", line, column);
  printf("+----------------------------------------+\n");
}

//Mostra os erros e acertos
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

//mostra o menu de fim de jogo
void game_over(int isWin, int hidden_map[MAP_SIZE][MAP_SIZE], int hits,
               int mistakes) {
  printf("+----------------------------------------+\n");
  if (isWin) {
    printf("|               VOCÊ VENCEU              |\n");
  } else {
    printf("|               VOCÊ PERDEU              |\n");
  }
  printf("+----------------------------------------+\n");
  scoreboard(hits, mistakes);
  mostrar_mapa(hidden_map);
}

//executa o jogo
void play(int plays_made_len, int plays_made[plays_made_len][4],
          int map[MAP_SIZE][MAP_SIZE], int hidden_map[MAP_SIZE][MAP_SIZE],
          int hits, int mistakes, int withResolve, int withSleep) {
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
         withResolve, withSleep);
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

    sleep(withSleep);

    //verifica se o jogo acabou ou não
    if (hits == max_hits) {
      system("clear");
      game_over(1, hidden_map, hits, mistakes);
    } else if (mistakes < (400 - max_hits)) {
      system("clear");
      play(plays_made_len, plays_made, map, hidden_map, hits, mistakes,
           withResolve, withSleep);
    } else {
      system("clear");
      game_over(0, hidden_map, hits, mistakes);
    }
  }
}

//Menu de seleção inicial
void menu_map(int plays_made[2048][4], int map[MAP_SIZE][MAP_SIZE],
              int hidden_map[MAP_SIZE][MAP_SIZE]) {
  int option;

  printf("+----------------------------------------+\n");
  printf("| OPÇÕES                                 |\n");
  printf("+----------------------------------------+\n");
  printf("| 1 | PLAY                               |\n");
  printf("+----------------------------------------+\n");
  printf("| 2 | SEE MAP                            |\n");
  printf("+----------------------------------------+\n");
  printf("| 3 | FAST RESOLVER                      |\n");
  printf("+----------------------------------------+\n");
  printf("| 4 | SLOW RESOLVER                      |\n");
  printf("+----------------------------------------+\n");
  printf("\n");
  printf("-> ");
  scanf("%i", &option);

  if (option == 1) {
    system("clear");
    play(0, plays_made, map, hidden_map, 0, 0, 0, 0);
  } else if (option == 2) {
    system("clear");
    mostrar_mapa(map);
    printf("\n");
    menu_map(plays_made, map, hidden_map);
  } else if (option == 3) {
    system("clear");
    play(0, plays_made, map, hidden_map, 0, 0, 1, 0);
  } else if (option == 4) {
    system("clear");
    play(0, plays_made, map, hidden_map, 0, 0, 1, 1);
  } else {
    menu_map(plays_made, map, hidden_map);
  }
}

//Carrega o mapa, e inicia ao final o menu de jogatina
void ler_mapa(int plays_made[2048][4], int map[MAP_SIZE][MAP_SIZE],
              int hidden_map[MAP_SIZE][MAP_SIZE]) {
  FILE *file;
  int line, column, id;
  char direction;
  int temp;

  file = fopen("./tabuleiro.txt", "r");
  if (file == NULL) {
    printf("Você ainda não tem um mapa criado\n\n");
  }

  while ((temp = fscanf(file, "%d %d %c %d", &column, &line, &direction,
                        &id)) != EOF) {
    if (temp != 4) {
      printf("Erro ao ler o arquivo\n");
      exit(0);
    }

    if (id == 0) {
      colocar_peca(map, id, BUOY_LEN, 0, line, column, direction);
    } else if (id == 1) {
      colocar_peca(map, id, AIRPLANE_LEN, 0, line, column, direction);
    } else if (id == 2) {
      colocar_peca(map, id, SUBMARINE_LEN, 0, line, column, direction);
    } else if (id == 3) {
      colocar_peca(map, id, SPY_ONE_LEN, 0, line, column, direction);
    } else if (id == 4) {
      colocar_peca(map, id, SPY_TWO_LEN, 0, line, column, direction);
    } else if (id == 5) {
      colocar_peca(map, id, AIRCRAFT_CARRIER_LEN, 0, line, column, direction);
    }
  }

  menu_map(plays_made, map, hidden_map);
  fclose(file);
}

//Menu de seleção de mapa

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

  //Opção de criação de mapa
  if (option == 1) {
    pFile = fopen("./tabuleiro.txt", "w");
    fclose(pFile);
    int map[MAP_SIZE][MAP_SIZE];
    int hidden_map[MAP_SIZE][MAP_SIZE];
    int plays_made[2048][4];
    incializar_mapa(map);
    incializar_mapa(hidden_map);
    srand(time(NULL));
    colocar_peca(map, AIRCRAFT_CARRIER, AIRCRAFT_CARRIER_AMOUNT, 1, -1, -1,
                 '0');
    colocar_peca(map, SPY_TWO, SPY_TWO_AMOUNT, 1, -1, -1, '0');
    colocar_peca(map, SPY_ONE, SPY_ONE_AMOUNT, 1, -1, -1, '0');
    colocar_peca(map, SUBMARINE, SUBMARINE_AMOUNT, 1, -1, -1, '0');
    colocar_peca(map, AIRPLANE, AIRPLANE_AMOUNT, 1, -1, -1, '0');
    colocar_peca(map, BUOY, BUOY_AMOUNT, 1, -1, -1, '0');
    system("clear");
    menu_map(plays_made, map, hidden_map);
  } else if (option == 2) {
    //Opção de carregamento de mapa
    system("clear");
    int map[MAP_SIZE][MAP_SIZE];
    int hidden_map[MAP_SIZE][MAP_SIZE];
    int plays_made[2048][4];

    incializar_mapa(map);
    incializar_mapa(hidden_map);

    ler_mapa(plays_made, map, hidden_map);
  } else {
    system("clear");
  }
}


//Função main
int main(void) {
  menu();
  return 0;
}
