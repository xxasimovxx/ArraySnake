#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define BLANK '.'
#define APPLE '?'
#define HEAD '$'
#define SEGMENT '='

typedef struct SG{
  int pos;
  struct SG *ptr;

} SG;

typedef struct map{
  int width;
  int height;
  char *map_pos;
  SG * head;
  int pos_change;

} Map;

void make_map(Map *map);
void print_map(Map *map);
void enable_nonblocking_input();
void disable_nonblocking_input();
void process_input(char ch, int *alive, Map *map);
void move(Map *map);
void apples(Map *map);
int should_be_alive(Map *map);

int main(void){
  char ch;
  int alive = 1;
  int width = 20;
  int height = 10;
  char map_chr[width * height];
  SG test2 = {0, NULL};
  SG test = {0, NULL};
  SG head = {0, NULL};
  Map map = {width, height, map_chr, &head, 0};

  make_map(&map);
  test.pos = map.head->pos -1;
  test2.pos = map.head->pos -2;
  map.head->ptr = &test;
  test.ptr = &test2;
  enable_nonblocking_input();

  while(alive){
    
    print_map(&map);

    ch = getchar();

    process_input(ch, &alive, &map);
    alive = should_be_alive(&map);
    apples(&map);
    move(&map);
    usleep(130000);

  }

  disable_nonblocking_input();

  return 0;

}

void make_map(Map *map){

  int width = map->width;
  int height = map->height;

  srand(time(NULL));
  int head = rand() % (width * height);

  srand(head * rand() % (width * height));
  int apple = rand() % (width * height);

  while(head == apple){
    usleep(15000);

    srand(time(NULL) * rand());
    int apple = rand() % (width * height);

  }

  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x ++){

      map->map_pos[y * width + x] = BLANK;

    }
  }

  map->head->pos = head;

  map->map_pos[head] = HEAD;
  map->map_pos[apple] = APPLE;
}

void print_map(Map *map){

  int width = map->width;
  int height = map->height;

  system("clear");

  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x ++){

      printf("  %c", map->map_pos[y *width + x]);

    }
    puts("\n");
    
  }
}

void enable_nonblocking_input() {

  struct termios t;
    
  tcgetattr(STDIN_FILENO, &t);
    
  t.c_lflag &= ~(ICANON | ECHO);
    
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
    
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void disable_nonblocking_input() {

    struct termios t;
    
    tcgetattr(STDIN_FILENO, &t);
    
    t.c_lflag |= (ICANON | ECHO);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


void process_input(char ch, int *alive, Map *map){
  switch (ch) {
    case 'a': 

      map->pos_change = -1;
      break;

    case 's':

      map->pos_change = map->width;
      break;

    case 'd':

      map->pos_change = 1;
      break;

    case 'w':

      map->pos_change = -map->width;
      break;

    case 'k':

      *alive = 0;
      break;
  }

}


void move(Map *map){
  if((map->head->pos + map->pos_change < map->width * map->height && 
  map->head->pos + map->pos_change >= 0) && map->pos_change != 0
  ){

  SG *current = map->head;
  int last_pos = current->pos;
  int next_pos;
  map->map_pos[current->pos] = BLANK;
  map->head->pos = current->pos + map->pos_change;
  map->map_pos[map->head->pos] = HEAD;
  
  current = current->ptr;

  while(current != NULL){

    next_pos = current->pos; 
    current->pos = last_pos;
    map->map_pos[last_pos] = SEGMENT;
    map->map_pos[next_pos] = BLANK;
    last_pos = next_pos;

    current = current->ptr;
      

    }
  }
}

void apples(Map *map){
  int next_pos = map->head->pos + map->pos_change;
  if(next_pos > 0 || next_pos < map->width * map->height){
  if(map->map_pos[next_pos] == APPLE){
    SG *current = map->head;

    while(current->ptr != NULL){

      current = current->ptr;
    }

    SG *new = (SG*)malloc(sizeof(SG));
    new->pos = current->pos -map->pos_change;
    new->ptr = NULL;
    current->ptr = new;

    srand(time(NULL));
    int apple = rand() % (map->width * map->height);

    while(map->map_pos[apple] != BLANK){

      srand(time(NULL) * rand());
      apple = rand() % (map->width * map->height);

  }

      map->map_pos[apple] = APPLE;

    }
  }
}

int should_be_alive(Map *map){

  int next_pos = map->head->pos + map->pos_change;

  if(next_pos < 0 || next_pos > map->width * map->height){

    puts("boom");

    return 0;

  }else if(map->map_pos[next_pos] == SEGMENT){

    puts("boom");

    return 0;

  }

  return 1;

}











