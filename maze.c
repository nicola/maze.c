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
#define WINDOWX 500
#define WINDOWY 300

/*
 * MOUSE_GAME
 * A mouse goes to the cheese (longest path in the maze)
 *
 * ESCAPE_GAME
 * A person escape from one door to another (longest path until a wall in the maze)
 */

#define MOUSE_GAME 1
#define ESCAPE_GAME 2
// #define DEBUG

// Settings - Drawing
int SCALE;
int PADDINGTOP;
int PADDINGLEFT;
struct drawing Maze;
struct drawing Solution;

// Settings - Game
int SIZEX;
int SIZEY;
int LEVEL = 5;
int GAME = MOUSE_GAME;
bool SILENTMODE; // if true, it doesnt launch drawapp automagically


// Settings - Maze
int currentX, currentY;
struct cell Grid[250][250];
struct coords initialPoint;
struct coords finalPoint;


int main(int argc, char *argv[]) {

  // Set randomness
  srand((unsigned int)time(NULL));

  // Taking commands
  evaluateCommandLine(argc, argv);

  // Starting generating maze
  mazeInitialize();
  mazeGenerate(&currentX, &currentY);

  // Draw the maze
  prepareForDrawing();
  mazeDraw();

  if (!SILENTMODE) { 
    char command[60];
    sprintf(command, "cat %s | java -jar drawapp.jar", Maze.name);
    system(command);
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//   Command line and extras

/*
 *  evaluateCommandLine(int, char*)
 *  -------------------------------
 *  Take commands from input and extract information the program needs and evaluate them
 */

void evaluateCommandLine(int argc, char *argv[]) {
  // If no input, suggest to ask for help
   if (!argv[1]) printf("Type \"-h\" for information on available commands.\n");

   // Looping for command line inputs
   while ((argc > 1) && (argv[1][0] == '-')) {
     switch (argv[1][1]) {
       case 'x': saveIfNumeric(&SIZEX, &argv[1][2]); break;
       case 'y': saveIfNumeric(&SIZEY, &argv[1][2]); break;
       case 'l': saveIfNumeric(&LEVEL, &argv[1][2]); break;
       case 'g': saveIfNumeric(&GAME, &argv[1][2]); break;
       case 'h': usage(); break;
       case 's': SILENTMODE = true; break;
       default:
       printf("\nmaze: *** Command: %s not found.  Stop.\n", argv[1]);
       printf("maze: *** Exiting from the maze, you are at safe\n\n", argv[1]);
       usage();
     }
     ++argv;
     --argc;
   }

  // Set given or default options for -x and -y
  if (SIZEX == 0) {
    SIZEX = (SIZEY > 0) ? SIZEY : 20;
  }

  if (SIZEY == 0) {
    SIZEY = SIZEX;
  }

  // Validation of command inputs -x,-y,-l,-g
  if (SIZEX < 4 || SIZEX > 250) {
    printf("\nmaze: *** -x%i not in range [4-250].  Stop.\n", SIZEX);
    printf("\nUsage:\n");
    printf("  -x<int>        Horizontal cells [4-250] (default: 20)\n");
    exit(8);
  }
  if (SIZEY < 4 || SIZEX > 250) {
    printf("\nmaze: *** -y%i not in range [4-250].  Stop.\n", SIZEY);
    printf("\nUsage:\n");
    printf("  -y<int>        Vertical cells [4-250] (default: 20)\n");
    exit(8);
  }

  if (SIZEY > SIZEX) {
    printf("maze: *** -x must be bigger or equal -y.  Stop\n");
    exit(8);
  }

  if (LEVEL < 1 || LEVEL > 10) {
    printf("\nmaze: *** -l%i not in range [1-10].  Stop.\n", LEVEL);
    printf("\nUsage:\n");
    printf("  -l<int>        Level from 1 to 10 (default: 10)\n");
    exit(8);
  }
  
  if (GAME != MOUSE_GAME && GAME != ESCAPE_GAME) {
    printf("\nmaze: *** -g%i not in range.  Stop.\n", GAME);
    printf("\nUsage:\n");
    printf("  -g<int>        Game: 1 for Mouse&cheese, 2 for Escape (default: 1)\n");
    exit(8);
  }  

  printf("\nYour settings\n");
  printf("Horizontal cells: %i\n", SIZEX);
  printf("Vertical cells: %i\n", SIZEY);
  printf("Difficulty: %i\n", LEVEL);
  printf("Game: %i\n", GAME);
}

bool isNumeric (char *ptrChar) {
  if (*ptrChar) {
    char character;
    while ( (character = * ptrChar++) ) {
      if (!isdigit(character)) return 0;
    }
    return 1;
  }
  return 0;
}

/*
 *  saveIfNumeric(int*, char*)
 *  --------------------------
 *  If commandline inputs char, save if its a number otherwise, error.
 */
void saveIfNumeric(int * ptrSetting, char * argInput) {
  if (!isNumeric(argInput)) {
    printf("\nmaze: *** Invalid character, use only int when <int> is required.  Stop.\n\n", SIZEY);
    usage(); // This exits
  }
  *ptrSetting = atoi(argInput);
}

/*
 *  timeString()
 *  ------------
 *  Returns a pointer of timestamp
 */
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
/*
 *  usage()
 *  -------
 *  Returns how to use the command line and exit
 */
void usage() {
  printf("./maze [-x<int>] [-y<int>] [-l<int>] [-g<int>] \n\n");
  printf("Usage:\n");
  printf("  -x<int>        Horizontal cells (default: 20)\n");
  printf("  -y<int>        Vertical cells (default: 10)\n");
  printf("  -l<int>        Level from 1 to 10 (default: 10)\n");
  printf("  -g<int>        Game: 1 for Mouse&cheese, 2 for Escape (default: 1)\n");
  printf("  -s             Silently create maze files without starting drawapp\n");
  exit(8);
}

///////////////////////////////////////////////////////////////////////////////
//   Drawing

/*
 *  drawLine(int, int, int, int)
 *  ----------------------------
 *  Fill a cell with a plain colour
 */
void drawLine(int x1, int x2, int x3, int x4) {
  x1 = x1*SCALE + PADDINGLEFT;
  x2 = x2*SCALE + PADDINGTOP;
  x3 = x3*SCALE + PADDINGLEFT;
  x4 = x4*SCALE + PADDINGTOP;

  fprintf(Maze.file, "DL %i %i %i %i\n", x1, x2, x3, x4);
  fprintf(Solution.file, "DL %i %i %i %i\n", x1, x2, x3, x4);
  #ifdef DEBUG
  // printf("DL %i %i %i %i\n", x1, x2, x3, x4);
  #endif
}

/*
 *  fillCell(bool, int, int, int, int)
 *  ----------------------------------
 *  Fill a cell with a plain colour
 */
void fillCell(bool onlySolution, int x, int y, int width, int height) {
  x = x*SCALE + PADDINGLEFT;
  y = y*SCALE + PADDINGTOP;
  width *= SCALE;
  height *= SCALE;
  fprintf(Solution.file, "FR %i %i %i %i\n", x, y, width, height);
  if (!onlySolution) fprintf(Maze.file, "FR %i %i %i %i\n", x, y, width, height);
}

/*
 *  prepareForDrawing()
 *  -------------------
 *  Compare the scale of x and y (window size) / (size of maze + margin)
 *  Choose the smallest scale, to make it look nicer and set the padding for x and y 
 */
void prepareForDrawing() {
  int scaleX = WINDOWX / (SIZEX+1 +2);
  int scaleY = WINDOWY / (SIZEY+1 +2);
  SCALE = (scaleX <= scaleY) ? scaleX : scaleY;

  PADDINGTOP = (WINDOWY - SCALE*(SIZEY+1))/2;
  PADDINGLEFT = (WINDOWX - SCALE*(SIZEX+1))/2;

  // Level here
  // int depthLevel = (finalPoint.depth / 10)*LEVEL;

  printf("SCALE: %i\n", SCALE);
  printf("PADDINGTOP: %i\n", PADDINGTOP);
  printf("PADDINGLEFT: %i\n", PADDINGLEFT);
}

/*
 *  setColour(FILE*, colour)
 *  ------------------------
 *  Set the colour of drawing
 */
void setColour(FILE * file, colour c) {
  char* colourName;
  switch(c) {
    case black : colourName = "black"; break;
    case blue : colourName = "blue"; break;
    case cyan : colourName = "cyan"; break;
    case darkgray : colourName = "darkgray"; break;
    case gray : colourName = "gray"; break;
    case green : colourName = "green"; break;
    case lightgray : colourName = "lightgray"; break;
    case magenta : colourName = "magenta"; break;
    case orange : colourName = "orange"; break;
    case pink : colourName = "pink"; break;
    case red : colourName = "red"; break;
    case white : colourName = "white"; break;
    case yellow : colourName = "yellow"; break;
  }
  fprintf(file, "SC %s\n", colourName);
}

///////////////////////////////////////////////////////////////////////////////
//   Maze

/*
 *  mazeBacktrack(int, int)
 *  -----------------------
 *  Backtrack from a cell until it finds one that is adjacent to at least one non-visited point
 */
void mazeBacktrack(int *pointerX, int *pointerY) {
  int prevX, prevY;

  #ifdef DEBUG
  printf("[");
  #endif
  while (areNeighborsVisited(*pointerX, *pointerY)) {

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

/*
 *  mazeDraw()
 *  ----------
 *  The drawing strategy consists in drawing .solution and .maze file at the same time.
 *  First solution is written, then 
 */
void mazeDraw() { //TODO level!
  int x, y;

  Maze.file = fopen(Maze.name, "wt");
  Solution.file = fopen(Solution.name, "wt");

  // Creating holes for initial and final
  Grid[initialPoint.x][initialPoint.y].up = 1;
  if (GAME == 2) {
    if (finalPoint.x == 0) Grid[finalPoint.x][finalPoint.y].left = 1;
    if (finalPoint.y == 0) Grid[finalPoint.x][finalPoint.y].up = 1;
  }

  // Backtrack the longest path and draw solution

  int tempX = finalPoint.x, tempY = finalPoint.y;
  #ifdef DEBUG
  printf("\nMAXDEPTH = (%i,%i) %i\n", finalPoint.x, finalPoint.y, *finalPoint.depth);
  #endif

  setColour(Solution.file,green);
  do {
    fillCell(1, tempX, tempY, 1, 1);
    // #ifdef DEBUG
    // printf("FROM (%i,%i):%i", tempX, tempY, Grid[tempX][tempY].depth);
    // printf("\tTO (%i,%i):%i\n", Grid[tempX][tempY].prevX, Grid[tempX][tempY].prevY, Grid[Grid[tempX][tempY].prevX][Grid[tempX][tempY].prevY].depth);
    // #endif
    int tempX2 = Grid[tempX][tempY].prevX;
    tempY = Grid[tempX][tempY].prevY;
    tempX = tempX2;
  } while (!(initialPoint.x == tempX && initialPoint.y == tempY));

  // Draw initial point TODO
  if (initialPoint.x != 0 && initialPoint.y != 0) {
    setColour(Solution.file, blue);
    fillCell(0 || GAME==2, initialPoint.x, initialPoint.y, 1, 1);
  }

  // Draw final point TODO
  setColour(Solution.file, red);
  setColour(Maze.file, magenta);
  fillCell(0 || GAME==2, finalPoint.x, finalPoint.y, 1, 1);
  

  // Draw the maze grid
  setColour(Maze.file, black);
  setColour(Solution.file, black);
  for (x = 0; x <= SIZEX; x++) {
    for (y = 0; y <= SIZEY; y++) {
      if (!Grid[x][y].up)   { drawLine(x, y, x+1,   y); }
      if (!Grid[x][y].left) { drawLine(x, y,   x, y+1); }
    }
  }
  drawLine(0, SIZEY+1, SIZEX+1, SIZEY+1);
  drawLine(SIZEX+1, 0, SIZEX+1, SIZEY+1);


  // If Game == 2 (escape), draw an hole

  fclose(Maze.file);
  fclose(Solution.file);
  return;
}

/*
 *  mazeGenerate(int, int)
 *  ----------------------
 *  Generate a maze by: 1) checking for a non-visited node, grow a maze from it,
 *  until number of cells is equal to horizontal * vertical cells
 */
void mazeGenerate(int *pointerX, int *pointerY) {
  long cellVisited = 1;
  do {
    mazeBacktrack(pointerX, pointerY);
    mazeGrow(pointerX, pointerY);
    cellVisited++;
  } while (cellVisited < (SIZEX+1)*(SIZEY+1)); // TODO check better
  return;
}

/*
 *  mazeGrow(int, int)
 *  ------------------
 *  Grow a maze by looping until a non-visited cell is found
 *  If the distance (depth) from first point is higher than previous, store it
 */
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

    // If it goes outside the borders, start again.
    if (newX < 0 || newY < 0 || newX > SIZEX || newY > SIZEY) continue;

    // If cell not visited, visit and carve a passage
    if (!isVisited(newX, newY)) {
      Grid[newX][newY].visited = true;
      cellCarvePassage(*pointerX, *pointerY, newDirection);
      completed = 1;
    }
  } while (!completed);

  *pointerX = newX;
  *pointerY = newY;

  // Save length of the path (depth) if higher than previous record
  mazeSetLongestPath(newX, newY);
}

/*
 *  mazeInitialize()
 *  ----------------
 *  Creates Maze and Solution files, set the first and the last point.
 */
void mazeInitialize() { //TODO TO BE TESTED

  // Prepare .maze and .solution files
  char * timeStr = timeString();
  sprintf(Maze.name, "%s_%ix%i.maze", timeStr, SIZEX, SIZEY);
  sprintf(Solution.name, "%s_%ix%i.solution", timeStr, SIZEX, SIZEY);

  // Setting starting point
  initialPoint.x = 0;
  initialPoint.y = 0;
  Grid[initialPoint.x][initialPoint.y].visited = 1;
  Grid[initialPoint.x][initialPoint.y].up = 1;

  // Initializing final point at (0, 0) with depth 0 - (will be updated later)
  finalPoint.x = 0;
  finalPoint.y = 0;
  finalPoint.depth = &Grid[0][0].depth;

  // Initialize current position equal to the starting point
  currentX = initialPoint.x;
  currentY = initialPoint.y;

  // Considering arrays starts from 0 and not from 1, SIZEX and SIZEY must be reduced
  SIZEX--;
  SIZEY--;

  return;
}

/*
 *  mazeReset()
 *  -----------
 *  Reset all Grid information and re-initialize the maze
 */
void mazeReset() {
  int x, y;
  for (x = 0; x <= SIZEX; x++) {
    for (y=0; y <= SIZEY; y++) {
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
 *  mazeSetLongestPath(int, int)
 *  ----------------------------
 *  Save length of the path (depth) if higher than previous record.
 */
void mazeSetLongestPath(int newX, int newY) {
  
  // Continues only if the new depth is higher
  if (*(finalPoint.depth) >= Grid[newX][newY].depth) return;
  
  switch (GAME) {
    case MOUSE_GAME:
      // MOUSE_GAME is from initial pointerto the furthest point
      // no rules for mouse game
    break;
    case ESCAPE_GAME:
      // ESCAPE_GAME is from inital point to the furthest point adjacent to the wall
      if (newX != 0 && newY != 0 && newX != SIZEX && newY != SIZEY) return;
    break;
  }

  finalPoint.x = newX;
  finalPoint.y = newY;
  *finalPoint.depth = Grid[newX][newY].depth;

}


///////////////////////////////////////////////////////////////////////////////
//   Cell

/*
 *  mazeSetLongestPath(int, int)
 *  ----------------------------
 *  Save length of the path (depth) if higher than previous record.
 */
bool areNeighborsVisited(int x, int y) {
  bool up = (y == 0) ? 1 : isVisited(x, y-1);
  bool down = (y == SIZEY) ? 1 : isVisited(x, y+1);
  bool left = (x == 0) ? 1 : isVisited(x-1, y);
  bool right = (x == SIZEX) ? 1 : isVisited(x+1, y);

  return (up && down && left && right);
}

/*
 *  cellCarvePassage(int, int, int)
 *  -------------------------------
 *  Save length of the path (depth) if higher than previous record.
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
      if (y == SIZEY) return;
      Grid[x][y].down = 1;
      Grid[x][++y].up = 1;
    break;
    case LEFT:
      if (x == 0) return;
      Grid[x][y].left = 1;
      Grid[--x][y].right = 1;
    break;
    case RIGHT:
      if (x == SIZEX) return;
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
  printf("[from(%i, %i):%i]", Grid[x][y].prevX, Grid[x][y].prevY, Grid[Grid[x][y].prevX][Grid[x][y].prevY].depth);
  printf("\tto(%i, %i):%i ", x, y, Grid[x][y].depth);
  printf("\n");
  #endif
}

/*
 *  isVisited(int, int)
 *  -------------------
 *  Save length of the path (depth) if higher than previous record.
 */
bool isVisited(int x, int y) {
  if (x < 0 || y < 0 || x > SIZEX || y > SIZEY) return false;
  return (Grid[x][y].visited);
}

/*
 *  randomDirection()
 *  -----------------
 *  Returns UP, DOWN, LEFT, RIGHT in a randomly
 */
unsigned int randomDirection() {
  return rand() % 4;
}


///////////////////////////////////////////////////////////////////////////////
//   Unit testing

/*
 *  mainTest()
 *  ----------------
 *  Unit testing
 */
int mainTest(void) {
  randomDirection_test();
  cellCarvePassage_test();
  isVisited_test();
  areNeighborsVisited_test();
  mazeGrow_test();
  mazeBacktrack_test();
  mazeGenerate_test();
  mazeDraw_test();

  mazeReset();
  return 0;
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
static void areNeighborsVisited_test() {
  mazeReset();
  Grid[1][0].visited = true;
  Grid[0][1].visited = true;
  Grid[2][1].visited = true;
  Grid[1][2].visited = true;
  assert(areNeighborsVisited(1,1));
  // TESTING CORNERS
  Grid[0][0].visited = true;
  Grid[1][0].visited = true;
  Grid[0][1].visited = true;
  assert(areNeighborsVisited(0,0));
  Grid[SIZEX][0].visited = true;
  Grid[SIZEX-1][0].visited = true;
  Grid[SIZEX][1].visited = true;
  assert(areNeighborsVisited(SIZEX,0));
  Grid[0][SIZEY].visited = true;
  Grid[0][SIZEY-1].visited = true;
  Grid[1][SIZEY].visited = true;
  assert(areNeighborsVisited(0,SIZEY));
  Grid[SIZEX][SIZEY].visited = true;
  Grid[SIZEX-1][SIZEY].visited = true;
  Grid[SIZEX][SIZEY-1].visited = true;
  assert(areNeighborsVisited(SIZEX,0));
  assert(!areNeighborsVisited(0,1));
}
static void cellCarvePassage_test() {
  mazeReset();
  cellCarvePassage(2,3, UP);
  assert(Grid[2][3].up == 1);

  mazeReset();
  cellCarvePassage(0,0, LEFT);
  assert(!Grid[0][0].left);

  cellCarvePassage(SIZEX,SIZEY, DOWN);
  assert(!Grid[SIZEX][SIZEY].down);
  cellCarvePassage(SIZEX,SIZEY, RIGHT);
  assert(!Grid[SIZEX][SIZEY].right);
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
  mazeBacktrack(&currentX, &currentY);
  assert(currentX == 0 && currentY == 0);

  currentX = 1;
  currentY = 2;
  mazeBacktrack(&currentX, &currentY);
  assert(currentX == 1 && currentY == 2);
}
static void mazeDraw_test() {
  mazeDraw();
}


/* TODO MISSING

DIFFERENT LEVELS
GUI?

PRINT BMP?

RESET A MAXIMUM

FARE I BUCHI NEL MURO IN BASSO E A DESTRA per GAME2

*/

// THE VERY END :)