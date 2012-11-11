#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/*
  +--------------+--------------+--------------+
  |              |              |              |
  |   x-1, y-1   |     x, y-1   |   x+1, y-1   |
  |              |              |              |
  +--------------+--------------+--------------+
  |              |              |              |
  |   x-1, y     |     x, y     |   x+1, y     |
  |              |              |              |
  +--------------+--------------+--------------+
  |              |              |              |
  |   x-1, y+1   |     x, y+1   |   x+1, y+1   |
  |              |              |              |
  +--------------+--------------+--------------+

*/

#include "maze.h"

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3
#define SIZEX 6
#define SIZEY 6
#define SCALE 20
#define PADDINGTOP 20
#define PADDINGLEFT 20
#define DEBUG

struct cell Maze[SIZEX+1][SIZEY+1];
int currentX, currentY;
long numin = 1;

int main() {
  
  srand((unsigned int)time(NULL));
  mazeInitialize();
  // Test
  opposite_test();
  randomDirection_test();
  cellCarvePassage_test();
  isVisited_test();
  areAllNeighborsVisited_test();
  mazeGrow_test();
  mazeBacktrack_test();
  mazeGenerate_test();
  mazeDraw_test();

  return 0;
}
/*

  Maze Functions

*/
void mazeInitialize() { //TODO TO BE TESTED
  int x, y;
  for(x = 0; x <= SIZEX; x++){
    for(y = 0; y <= SIZEY; y++){
      //All maze cells have all walls existing by default, except the perimeter cells.
      //Maze[x][y].visited   = (x == 0 || x == SIZEX || y == 0 || y == SIZEY) ? 1 : 0;
    }
  }
  Maze[0][0].visited = 1;
  return;
}
void mazeGenerate(int *pointerX, int *pointerY) {
  numin = 1;

  do {
    mazeBacktrack(pointerX, pointerY);
    mazeGrow(pointerX, pointerY);

    numin++;
  } while (numin < (SIZEX+1)*(SIZEY+1)); // TODO check better

  return;
}
void mazeGrow(int *pointerX, int *pointerY) {
  int completed = 0;
  int newX, newY;
  int newDirection;

  do {
    newX = *pointerX;
    newY = *pointerY;
    newDirection = randomDirection();
    switch (newDirection) {
      case UP: newY--; break;
      case DOWN: newY++; break;
      case LEFT: newX--; break;
      case RIGHT: newX++; break;
    }
    // Error Handler
    if (newX < 0 || newY < 0 || newX > SIZEX || newY > SIZEY) continue;

    if (!isVisited(newX, newY)) {
      Maze[newX][newY].visited = true;
      cellCarvePassage(*pointerX, *pointerY, newDirection);
      completed = 1;
    }
  } while (!completed);

  *pointerX = newX;
  *pointerY = newY;
}
void mazeBacktrack(int *pointerX, int *pointerY) {
  int prevX, prevY;

  #ifdef DEBUG
  printf(" [ ");
  #endif
  while (areAllNeighborsVisited(*pointerX, *pointerY)) {

    prevX = Maze[*pointerX][*pointerY].prevX;
    prevY = Maze[*pointerX][*pointerY].prevY;
    *pointerX = prevX;
    *pointerY = prevY;

    #ifdef DEBUG
    printf(">(%i,%i)", *pointerX, *pointerY);
    #endif
    
    if (*pointerX == Maze[*pointerX][*pointerY].prevX && *pointerY == Maze[*pointerX][*pointerY].prevY) {
      #ifdef DEBUG
      printf(" BACK TO THE ORIGINS ");
      #endif
      break; // TODO: THIS WILL NEVER HAPPEN, WILL IT?
    }
  }
  #ifdef DEBUG
  printf(" ] ");
  #endif
}

void mazeSolve(int positionX, int positionY) {
  int completed;

  // maze backtrack with the one with highest depth (hopefully on borders)
}

void drawLine(FILE * outfile, int x1, int x2, int x3, int x4) {
  x1 += PADDINGTOP;
  x3 += PADDINGTOP;
  x2 += PADDINGLEFT;
  x4 += PADDINGLEFT;
  fprintf(outfile, "DL %i %i %i %i\n", x1, x2, x3, x4);
  #ifdef DEBUG
  #else DEBUG
  printf("DL %i %i %i %i\n", x1, x2, x3, x4);
  #endif
}

void mazeDraw(bool solution) {
  // TODO structure for files and fileNames
  int x, y;
  char fileMazeName[200];
  char fileSolutionName[200];
  FILE * fileMaze;
  FILE * fileSolution;
  char * timeStr = timeString();
  
  sprintf(fileMazeName, "%s_%ix%i.maze", timeStr, SIZEX, SIZEY); // TODO MD5
  fileMaze = fopen(fileMazeName, "wt");

  // Loop and find biggest depth
  for (x = 0; x <= SIZEX; x++){
    for (y = 0; y <= SIZEY; y++){
      if (!Maze[x][y].up) {
        drawLine(fileMaze, x*SCALE, y*SCALE, x*(SCALE)+SCALE, y*SCALE);
      }
      if (!Maze[x][y].left) {
        drawLine(fileMaze, x*SCALE, y*SCALE, x*SCALE, y*(SCALE)+SCALE);
      }
    }
  }
  drawLine(fileMaze, 0, (SIZEY+1)*SCALE, (SIZEX+1)*SCALE, (SIZEY+1)*SCALE);
  drawLine(fileMaze, (SIZEX+1)*SCALE, 0, (SIZEX+1)*SCALE, (SIZEY+1)*SCALE);
  fclose(fileMaze);

  sprintf(fileSolutionName, "%s_%ix%i.solution", timeStr, SIZEX, SIZEY); // TODO MD5
  fileSolution = fopen(fileSolutionName, "wt");
  // TODO HERE COPY FILE
  fclose(fileSolution);
  return;
}

void mazeReset() {
  int x, y;
  for (x = 0; x <= SIZEX; x++) {
    for (y=0; y <= SIZEY; y++) {
      Maze[x][y].visited = 0;
      Maze[x][y].depth = 0;
      Maze[x][y].up = 0;
      Maze[x][y].down = 0;
      Maze[x][y].left = 0;
      Maze[x][y].right = 0;
      Maze[x][y].prevX = 0;
      Maze[x][y].prevY = 0;
    }
  }
  currentX = currentY = 0;

  mazeInitialize();
}

/*

  Cell Functions

*/

void cellCarvePassage(int prevX, int prevY, int direction) {
  int x = prevX;
  int y = prevY;


  switch (direction) {
    case UP:
      if (y == 0) return;
      Maze[x][y].up = 1;
      Maze[x][--y].down = 1;
    break;
    case DOWN:
      if (y == SIZEY) return;
      Maze[x][y].down = 1;
      Maze[x][++y].up = 1;
    break;
    case LEFT:
      if (x == 0) return;
      Maze[x][y].left = 1;
      Maze[--x][y].right = 1;
    break;
    case RIGHT:
      if (x == SIZEX) return;
      Maze[x][y].right = 1;
      Maze[++x][y].left = 1;
    break;
  }


  // Set origins
  Maze[x][y].prevX = prevX;
  Maze[x][y].prevY = prevY;
  // Set new depth
  Maze[x][y].depth = Maze[prevX][prevY].depth + 1;
  #ifdef DEBUG
  printf("I am at (%i, %i) depth: %li  [prev:(%i,%i) depth: %li] \n", x, y, Maze[x][y].depth, prevX, prevY, Maze[prevX][prevY].depth);
  #endif
}

/*

  Other Functions

*/

unsigned int opposite(int direction) {
  return (direction + 2) % 4;
}
unsigned int randomDirection() {
  return rand() % 4;
}
char * timeString() {
  time_t timer; //TODO function
  struct tm* timeInfo;
  static char timeStr[16];

  time(&timer);
  timeInfo = localtime(&timer);
  strftime(timeStr, 16, "%Y%d%m%H%M%S", timeInfo);

  char * ptrTimeStr = timeStr;
  return ptrTimeStr;
}

bool isVisited(int x, int y) {
  if (x < 0 || y < 0 || x > SIZEX || y > SIZEY) return false;
  return (Maze[x][y].visited);
}
bool areAllNeighborsVisited(int x, int y) {

  bool up = (y == 0) ? 1 : isVisited(x, y-1);
  bool down = (y == SIZEY) ? 1 : isVisited(x, y+1);
  bool left = (x == 0) ? 1 : isVisited(x-1, y);
  bool right = (x == SIZEX) ? 1 : isVisited(x+1, y);

  return (up && down && left && right);
}

/*

  Tests

*/

static void opposite_test() {
  assert( opposite(UP) == DOWN);
  assert( opposite(DOWN) == UP);
  assert( opposite(LEFT) == RIGHT);
  assert( opposite(RIGHT) == LEFT);
}
static void randomDirection_test() {
  int aDirection = randomDirection();
  assert(aDirection == UP || aDirection == DOWN || aDirection == LEFT || aDirection == RIGHT);
}
static void isVisited_test() {
  Maze[0][0].visited = false;
  assert(!isVisited(0, 0));
  Maze[0][0].visited = true;
  assert(isVisited(0, 0));
}
static void areAllNeighborsVisited_test() {
  mazeReset();
  Maze[1][0].visited = true;
  Maze[0][1].visited = true;
  Maze[2][1].visited = true;
  Maze[1][2].visited = true;
  assert(areAllNeighborsVisited(1,1));

  // TESTING CORNERS
  Maze[0][0].visited = true;
  Maze[1][0].visited = true;
  Maze[0][1].visited = true;
  assert(areAllNeighborsVisited(0,0));

  Maze[SIZEX][0].visited = true;
  Maze[SIZEX-1][0].visited = true;
  Maze[SIZEX][1].visited = true;
  assert(areAllNeighborsVisited(SIZEX,0));

  Maze[0][SIZEY].visited = true;
  Maze[0][SIZEY-1].visited = true;
  Maze[1][SIZEY].visited = true;
  assert(areAllNeighborsVisited(0,SIZEY));

  Maze[SIZEX][SIZEY].visited = true;
  Maze[SIZEX-1][SIZEY].visited = true;
  Maze[SIZEX][SIZEY-1].visited = true;
  assert(areAllNeighborsVisited(SIZEX,0));

  assert(!areAllNeighborsVisited(0,1));
}

static void cellCarvePassage_test() {
  mazeReset();
  cellCarvePassage(2,3, UP);
  assert(Maze[2][3].up == 1);

  mazeReset();
  cellCarvePassage(0,0, UP);
  assert(!Maze[0][0].up);

  mazeReset();
  cellCarvePassage(0,0, LEFT);
  assert(!Maze[0][0].left);

  cellCarvePassage(SIZEX,SIZEY, DOWN);
  assert(!Maze[SIZEX][SIZEY].down);
  cellCarvePassage(SIZEX,SIZEY, RIGHT);
  assert(!Maze[SIZEX][SIZEY].right);
}

static void mazeGenerate_test() {
  mazeReset();
  currentX = 0;
  currentY = 0;
  mazeGenerate(&currentX,&currentY);
}
static void mazeGrow_test() {
  mazeReset();
  currentX = 0;
  currentY = 0;
  mazeGrow(&currentX,&currentY);
  assert(Maze[0][1].visited || Maze[1][0].visited);
  currentX = 2;
  currentY = 2;
  mazeGrow(&currentX,&currentY);
  assert(Maze[2][3].visited || Maze[3][2].visited || Maze[2][1].visited || Maze[1][2].visited);
}
static void mazeBacktrack_test() { //TODO
  mazeReset();

//  mazeBacktrack(0,0);
//  assert(!Maze[0][0].prevX && !Maze[0][0].prevY);

  Maze[2][1].visited = 1;
  Maze[2][1].prevX = 1;
  Maze[2][1].prevY = 1;

  //Neighborhood
  Maze[2][0].visited = 1;
  Maze[1][1].visited = 1;
  Maze[2][2].visited = 1;
  Maze[3][1].visited = 1;
  Maze[0][0].visited = 1;
  Maze[0][1].visited = 1;
  Maze[1][2].visited = 1;

  Maze[1][1].visited = 1;
  Maze[1][1].prevX = 1;
  Maze[1][1].prevY = 0;

  Maze[1][0].visited = 1;
  Maze[1][0].prevX = 0;
  Maze[1][0].prevY = 0;

  currentX = 2;
  currentY = 1;
  // printf("mazeBacktrack loop: started\n");
  // printf("I am at (%i, %i)\n", currentX, currentY); // STAAAAACK OVERFLOOOOOOW
  mazeBacktrack(&currentX, &currentY);
  // printf("I am at (%i, %i)\n", currentX, currentY); // STAAAAACK OVERFLOOOOOOW
  // printf("mazeBacktrack loop: terminated\n\n");
  assert(currentX == 0 && currentY == 0);

  currentX = 1;
  currentY = 2;
  // printf("I am at (%i, %i)\n", currentX, currentY); // STAAAAACK OVERFLOOOOOOW
  mazeBacktrack(&currentX, &currentY);
  // printf("I am at (%i, %i)\n", currentX, currentY); // STAAAAACK OVERFLOOOOOOW
  // printf("mazeBacktrack loop: terminated\n\n");
  assert(currentX == 1 && currentY == 2);
}

static void mazeDraw_test() {
  mazeDraw(false);
}
