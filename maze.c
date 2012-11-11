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
#define SCALE 10
#define PADDINGTOP 20
#define PADDINGLEFT 20
// #define DEBUG

// Game Settings
int sizeX = 20;
int sizeY = 10;
int level = 5;
int game = 1;

// Maze Generation
struct drawing Maze;
struct drawing Solution;
int currentX, currentY;
struct cell Grid[500][500];
struct coords initialPoint;
struct coords finalPoint;

void usage(void)
{
  printf("Usage:\n");
  printf("  -x<int>        Horizontal cells (default: 20)\n");
  printf("  -y<int>        Vertical cells (default: 10)\n");
  printf("  -l<int>        Level from 1 to 10 (default: 10)\n");
  printf("  -g<int>        Game: 1 for Mouse&cheese, 2 for Escape (default: 1)\n");
  printf("  -s             Silent mode, make me shut up\n");
  exit (8);
}

int main(int argc, char *argv[]) {

  // Set randomness
  srand((unsigned int)time(NULL));

  // If no input, suggest to ask for help
  if (!argv[1]) printf("Type \"-h\" for information on available commands.\n");

  // Looping for command line inputs
  while ((argc > 1) && (argv[1][0] == '-')) {
    switch (argv[1][1]) {
      case 'x': sizeX = atoi(&argv[1][2]); break;
      case 'y': sizeY = atoi(&argv[1][2]); break;
      case 'l': level = atoi(&argv[1][2]); break;
      case 'g': game = atoi(&argv[1][2]); break;
      case 'h': usage(); break;
      default:
      printf("\nmaze: *** Command: %s not found.  Stop.\n", argv[1]);
      printf("maze: *** Exiting from the maze, you are at safe\n\n", argv[1]);
      usage();
    }
    ++argv;
    --argc;
  }

  // Validation of command inputs
  if (sizeX < 4 || sizeX > 500) {
    printf("\nmaze: *** -x%i not in range [4-500].  Stop.\n", sizeX);
    printf("\nUsage:\n");
    printf("  -x<int>        Horizontal cells [4-500] (default: 20)\n");
    exit(8);
  }
  if (sizeY < 4 || sizeX > 500) {
    printf("\nmaze: *** -y%i not in range [4-500].  Stop.\n", sizeY);
    printf("\nUsage:\n");
    printf("  -y<int>        Vertical cells [4-500] (default: 20)\n");
    exit(8);
  }
  if (level < 1 || level > 10) {
    printf("\nmaze: *** -l%i not in range [1-10].  Stop.\n", level);
    printf("\nUsage:\n");
    printf("  -l<int>        Level from 1 to 10 (default: 10)\n");
    exit(8);
  }
  
  if (game != 1 && game != 2) {
    printf("\nmaze: *** -g%i not in range.  Stop.\n", game);
    printf("\nUsage:\n");
    printf("  -g<int>        Game: 1 for Mouse&cheese, 2 for Escape (default: 1)\n");
    exit(8);
  }

  if (sizeY > ((sizeX)*3)/2) {
    printf("maze: ** Warning: for better resolution, make -x equal or bigger than -y");
  }

  printf("\nYour settings\n");
  printf("Horizontal cells: %i\n", sizeX);
  printf("Vertical cells: %i\n", sizeY);
  printf("Difficulty: %i\n", level);
  printf("Game: %i\n", game);

  mazeInitialize();
  mazeGenerate(&currentX, &currentY);
  mazeDraw();

  return 0;
}

int mainTest(void) {
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

  /* Prepare .maze and .solution files */
  char * timeStr = timeString();
  sprintf(Maze.name, "%s_%ix%i.maze", timeStr, sizeX, sizeY);
  sprintf(Solution.name, "%s_%ix%i.solution", timeStr, sizeX, sizeY);

  /* Setting starting point */
  initialPoint.x = 0;
  initialPoint.y = 0;
  Grid[initialPoint.x][initialPoint.y].visited = 1;
  Grid[initialPoint.x][initialPoint.y].up = 1;

  /* Initializing final point - (will be updated later) */
  finalPoint.x = 0;
  finalPoint.y = 0;
  finalPoint.depth = &Grid[0][0].depth;

  /* Initialize current position equal to the starting point */
  currentX = initialPoint.x;
  currentY = initialPoint.y;

  return;
}
void mazeGenerate(int *pointerX, int *pointerY) {
  long numin = 1;

  do {
    mazeBacktrack(pointerX, pointerY);
    mazeGrow(pointerX, pointerY);

    numin++;
  } while (numin < (sizeX+1)*(sizeY+1)); // TODO check better

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
    if (newX < 0 || newY < 0 || newX > sizeX || newY > sizeY) continue;

    if (!isVisited(newX, newY)) {
      Grid[newX][newY].visited = true;
      cellCarvePassage(*pointerX, *pointerY, newDirection);
      completed = 1;
    }
  } while (!completed);

  *pointerX = newX;
  *pointerY = newY;

  if (*(finalPoint.depth) < Grid[newX][newY].depth) { //TODO put somewhere else
    finalPoint.x = newX;
    finalPoint.y = newY;
    *finalPoint.depth = Grid[newX][newY].depth;
  }
  //printf("\nMAXDEPTH = (%i,%i) %i\n", finalPoint.x, finalPoint.y, *finalPoint.depth);
}
void mazeBacktrack(int *pointerX, int *pointerY) {
  int prevX, prevY;

  #ifdef DEBUG
  printf("[");
  #endif
  while (areAllNeighborsVisited(*pointerX, *pointerY)) {

    prevX = Grid[*pointerX][*pointerY].prevX;
    prevY = Grid[*pointerX][*pointerY].prevY;
    *pointerX = prevX;
    *pointerY = prevY;

    #ifdef DEBUG
    printf(">(%i,%i)", *pointerX, *pointerY);
    #endif
    
    if (*pointerX == Grid[*pointerX][*pointerY].prevX && *pointerY == Grid[*pointerX][*pointerY].prevY) {
      #ifdef DEBUG
      printf(" BACK TO THE ORIGINS ");
      #endif
      break; // TODO: THIS WILL NEVER HAPPEN, WILL IT?
    }
  }
  #ifdef DEBUG
  printf(" ]\n");
  #endif
}

void mazeSolve(int positionX, int positionY) {
  int completed;

  // maze backtrack with the one with highest depth (hopefully on borders)
}

void drawLine(int x1, int x2, int x3, int x4) {
  x1 += PADDINGTOP;
  x3 += PADDINGTOP;
  x2 += PADDINGLEFT;
  x4 += PADDINGLEFT;
  fprintf(Maze.file, "DL %i %i %i %i\n", x1, x2, x3, x4);
  fprintf(Solution.file, "DL %i %i %i %i\n", x1, x2, x3, x4);
  #ifdef DEBUG
  // printf("DL %i %i %i %i\n", x1, x2, x3, x4);
  #endif
}

void fillRect(int x1, int x2, int x3, int x4)
{
  x1 += PADDINGTOP;
  x2 += PADDINGLEFT;
  fprintf(Solution.file, "FR %i %i %i %i\n", x1, x2, x3, x4);
}


void mazeDraw() { //TODO level and game!
  // TODO MOVE SOMEWHERE ELSE (time)
  int x, y;

  Maze.file = fopen(Maze.name, "wt");
  Solution.file = fopen(Solution.name, "wt");



  // Backtrack the longest path and draw solution

  int tempX = finalPoint.x, tempY = finalPoint.y;
  #ifdef DEBUG
  printf("\nMAXDEPTH = (%i,%i) %i\n", finalPoint.x, finalPoint.y, *finalPoint.depth);
  #endif

fprintf(Solution.file,"SC green\n");
  do {
    #ifdef DEBUG
    printf("FROM (%i,%i):%i", tempX, tempY, Grid[tempX][tempY].depth);
    printf("\tTO (%i,%i):%i\n", Grid[tempX][tempY].prevX, Grid[tempX][tempY].prevY, Grid[Grid[tempX][tempY].prevX][Grid[tempX][tempY].prevY].depth);
    #endif
    int tempX2 = Grid[tempX][tempY].prevX;
    tempY = Grid[tempX][tempY].prevY;
    tempX = tempX2;
    fillRect(tempX*SCALE, tempY*SCALE, SCALE, SCALE);
  } while (!(initialPoint.x == tempX && initialPoint.y == tempY));
fprintf(Solution.file,"SC black\n");


  // Draw the maze grid
  for (x = 0; x <= sizeX; x++){
    for (y = 0; y <= sizeY; y++){
      if (!Grid[x][y].up) {
        drawLine(x*SCALE, y*SCALE, x*(SCALE)+SCALE, y*SCALE);
      }
      if (!Grid[x][y].left) {
        drawLine(x*SCALE, y*SCALE, x*SCALE, y*(SCALE)+SCALE);
      }
    }
  }
  drawLine(0, (sizeY+1)*SCALE, (sizeX+1)*SCALE, (sizeY+1)*SCALE);
  drawLine((sizeX+1)*SCALE, 0, (sizeX+1)*SCALE, (sizeY+1)*SCALE);
  fclose(Maze.file);
  fclose(Solution.file);
  return;
}

void mazeReset() {
  int x, y;
  for (x = 0; x <= sizeX; x++) {
    for (y=0; y <= sizeY; y++) {
      Grid[x][y].visited = 0;
      Grid[x][y].depth = 0;
      Grid[x][y].up = 0;
      Grid[x][y].down = 0;
      Grid[x][y].left = 0;
      Grid[x][y].right = 0;
      Grid[x][y].prevX = 0;
      Grid[x][y].prevY = 0;
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
      Grid[x][y].up = 1;
      Grid[x][--y].down = 1;
    break;
    case DOWN:
      if (y == sizeY) return;
      Grid[x][y].down = 1;
      Grid[x][++y].up = 1;
    break;
    case LEFT:
      if (x == 0) return;
      Grid[x][y].left = 1;
      Grid[--x][y].right = 1;
    break;
    case RIGHT:
      if (x == sizeX) return;
      Grid[x][y].right = 1;
      Grid[++x][y].left = 1;
    break;
  }


  // Set origins
  Grid[x][y].prevX = prevX;
  Grid[x][y].prevY = prevY;
  // Set new depth
  Grid[x][y].depth = Grid[prevX][prevY].depth + 1;
  #ifdef DEBUG
//  printf("I am at (%i, %i) depth: %i  [prev:(%i=%i,%i=%i) depth: %i] \n", x, y, Grid[x][y].depth, prevX, Grid[x][y].prevX, prevY, Grid[x][y].prevY, Grid[prevX][prevY].depth);
  printf("[from(%i, %i):%i]", Grid[x][y].prevX, Grid[x][y].prevY, Grid[Grid[x][y].prevX][Grid[x][y].prevY].depth);
  printf("\tto(%i, %i):%i ", x, y, Grid[x][y].depth);
  printf("\n");
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
  if (x < 0 || y < 0 || x > sizeX || y > sizeY) return false;
  return (Grid[x][y].visited);
}
bool areAllNeighborsVisited(int x, int y) {

  bool up = (y == 0) ? 1 : isVisited(x, y-1);
  bool down = (y == sizeY) ? 1 : isVisited(x, y+1);
  bool left = (x == 0) ? 1 : isVisited(x-1, y);
  bool right = (x == sizeX) ? 1 : isVisited(x+1, y);

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
  Grid[0][0].visited = false;
  assert(!isVisited(0, 0));
  Grid[0][0].visited = true;
  assert(isVisited(0, 0));
}
static void areAllNeighborsVisited_test() {
  mazeReset();
  Grid[1][0].visited = true;
  Grid[0][1].visited = true;
  Grid[2][1].visited = true;
  Grid[1][2].visited = true;
  assert(areAllNeighborsVisited(1,1));

  // TESTING CORNERS
  Grid[0][0].visited = true;
  Grid[1][0].visited = true;
  Grid[0][1].visited = true;
  assert(areAllNeighborsVisited(0,0));

  Grid[sizeX][0].visited = true;
  Grid[sizeX-1][0].visited = true;
  Grid[sizeX][1].visited = true;
  assert(areAllNeighborsVisited(sizeX,0));

  Grid[0][sizeY].visited = true;
  Grid[0][sizeY-1].visited = true;
  Grid[1][sizeY].visited = true;
  assert(areAllNeighborsVisited(0,sizeY));

  Grid[sizeX][sizeY].visited = true;
  Grid[sizeX-1][sizeY].visited = true;
  Grid[sizeX][sizeY-1].visited = true;
  assert(areAllNeighborsVisited(sizeX,0));

  assert(!areAllNeighborsVisited(0,1));
}

static void cellCarvePassage_test() {
  mazeReset();
  cellCarvePassage(2,3, UP);
  assert(Grid[2][3].up == 1);

//  mazeReset();
//  cellCarvePassage(0,0, UP);
//  assert(!Grid[0][0].up);

  mazeReset();
  cellCarvePassage(0,0, LEFT);
  assert(!Grid[0][0].left);

  cellCarvePassage(sizeX,sizeY, DOWN);
  assert(!Grid[sizeX][sizeY].down);
  cellCarvePassage(sizeX,sizeY, RIGHT);
  assert(!Grid[sizeX][sizeY].right);
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
  assert(Grid[0][1].visited || Grid[1][0].visited);
  currentX = 2;
  currentY = 2;
  mazeGrow(&currentX,&currentY);
  assert(Grid[2][3].visited || Grid[3][2].visited || Grid[2][1].visited || Grid[1][2].visited);
}
static void mazeBacktrack_test() { //TODO
  mazeReset();

//  mazeBacktrack(0,0);
//  assert(!Grid[0][0].prevX && !Grid[0][0].prevY);

  Grid[2][1].visited = 1;
  Grid[2][1].prevX = 1;
  Grid[2][1].prevY = 1;

  //Neighborhood
  Grid[2][0].visited = 1;
  Grid[1][1].visited = 1;
  Grid[2][2].visited = 1;
  Grid[3][1].visited = 1;
  Grid[0][0].visited = 1;
  Grid[0][1].visited = 1;
  Grid[1][2].visited = 1;

  Grid[1][1].visited = 1;
  Grid[1][1].prevX = 1;
  Grid[1][1].prevY = 0;

  Grid[1][0].visited = 1;
  Grid[1][0].prevX = 0;
  Grid[1][0].prevY = 0;

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
  mazeDraw();
}
