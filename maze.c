#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/*
  
  # Install
      make

  # Use it
      ./maze -h

  # Grid
  
  My grid is a matrix that respect as follows:
  
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
// #define DEBUG

/*
 *  MOUSE_GAME
 *  A mouse goes to the cheese (longest path in the maze)
 *
 *  ESCAPE_GAME
 *  A person escape from one door to another (longest path until a wall in the maze)
 */
#define MOUSE_GAME 1
#define ESCAPE_GAME 2

// Settings - Drawing
unsigned int SCALE;
unsigned int PADDINGTOP;
unsigned int PADDINGLEFT;
unsigned int COLOURSCHEME;
struct drawing Maze;
struct drawing Solution;
struct colourscheme Scheme; // current-colour-Scheme

// Settings - Game
unsigned int SIZEX;
unsigned int SIZEY;
unsigned int LEVEL;
unsigned int GAME = MOUSE_GAME;
bool SILENTMODE; // if true, it doesnt launch drawapp automagically
bool DELETEFILES; // if true, it deletes all your *.solution *.maze


// Settings - Maze
unsigned int currentX, currentY;
struct cell Grid[250][150];
struct coords initialPoint;
struct coords finalPoint;

// Debug
#ifdef DEBUG
FILE * debugFile;
char debugFileName[] = "logfile.debug";
#endif


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

    sprintf(command, "cat %s | java -jar drawapp.jar", Solution.name);
    system(command);
  }

  #ifdef DEBUG
  // This will perform unit tests, they will print output in logfile.debug
  // If a test fail, the program will exit with an error
  debugFile = fopen(debugFileName, "wt");
  mainTest();
  fclose(debugFile);
  #endif

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
      case 'c': saveIfNumeric(&COLOURSCHEME, &argv[1][2]); break;
      case 'h': usage(); break;
      case 's': SILENTMODE = true; break;
      case 'd': DELETEFILES = true; break;
      default:
      printf("\nmaze: *** Command: %s not found.  Stop.\n", argv[1]);
      printf("maze: *** Exiting from the maze, you are at safe\n\n", argv[1]);
      usage();
    }
    ++argv;
    --argc;
  }

  // This delete all the previous files

  if (DELETEFILES) {
    #if defined(__APPLE__) || defined(__unix__) || defined(__linux__) || __CYGWIN__ || __FreeBSD__
    system("rm -rf *.maze *.solution");
    exit(8);
    #else
    printf("\nmaze: *** Command: -d cannot be executed on non-Unix operating systems.  Stop.\n", argv[1]);
    exit(8);
    #endif
  }


  
  if (LEVEL > 0 && LEVEL < 6) {
    if (SIZEX == 0 && SIZEY == 0) {
      loadLevelSize();
    } else {
      printf("\nmaze: ** You cannot use -l with -x or -y, this is exclusive.  Warning.\n");
      printf("maze: ** Level -l will be ignored.  Continue.\n");
    }
  }

  // Set given or default options for -x and -y
  if (SIZEX == 0) {
    SIZEX = (SIZEY > 0) ? SIZEY : 10;
  }

  if (SIZEY == 0) {
    SIZEY = SIZEX;
    if (SIZEY > 149) SIZEY = 149;
  }

  // Validation of command inputs -x,-y,-l,-g
  if (SIZEX < 4 || SIZEX > 249) {
    printf("\nmaze: *** -x%i not in range [4-249].  Stop.\n", SIZEX);
    printf("\nUsage:\n");
    printf("  -x<int>        Horizontal cells [4-249] (default: 10)\n");
    exit(8);
  }
  if (SIZEY < 4 || SIZEY > 149) {
    printf("\nmaze: *** -y%i not in range [4-149].  Stop.\n", SIZEY);
    printf("\nUsage:\n");
    printf("  -y<int>        Vertical cells [4-149] (default: 10)\n");
    exit(8);
  }

  if (SIZEY > SIZEX) {
    printf("maze: *** -x must be bigger or equal -y.  Stop\n");
    exit(8);
  }

  if (LEVEL < 0 || LEVEL > 5) {
    printf("\nmaze: *** -l%i not in range [1-5].  Stop.\n", LEVEL);
    printf("\nUsage:\n");
    printf("  -l<int>        Level from 1 to 5 (default: 2)\n");
    exit(8);
  }

  if (COLOURSCHEME > 4) {
    printf("\nmaze: *** -c%i not in range [1-3].  Stop.\n", LEVEL);
    printf("\nUsage:\n");
    printf("  -c<int>        Colourscheme: 1 for Default, 2 for Star Wars, 3 for Funky, 4 for B/W\n");
    exit(8);
  }
  
  if (GAME != MOUSE_GAME && GAME != ESCAPE_GAME) {
    printf("\nmaze: *** -g%i not in range.  Stop.\n", GAME);
    printf("\nUsage:\n");
    printf("  -g<int>        Game: 1 for Mouse&cheese, 2 for Escape (default: 1)\n");
    exit(8);
  }
}

/*
 *  isNumeric(char*)
 *  ----------------
 *  Check if a char represents a number
 */
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
  time_t timer;
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
  printf("  -x<int>        Horizontal cells (default: 10)\n");
  printf("  -y<int>        Vertical cells (default: 10)\n");
  printf("  -l<int>        Level from 1 to 5 (default: 2)\n");
  printf("  -g<int>        Game: 1 for Mouse&cheese, 2 for Escape (default: 1)\n");
  printf("  -c<int>        Colourscheme: 1 for Default, 2 for Star Wars, 3 for Funky, 4 for B/W\n");
  printf("  -s             Silently create maze files without starting drawapp\n");
  printf("  -d             Delete all the previous *.maze and *.solution\n");
  exit(8);
}

///////////////////////////////////////////////////////////////////////////////
//   Drawing

/*
 *  drawGrid(struct coords, struct coords)
 *  --------------------------------------
 *  Draw grid (including holes) Strategy is that lines are drawn if .up == 1 || .left == 1
 */
void drawGrid (struct coords * ptrInitial, struct coords * ptrFinal) {
  int x, y;

  // Creating holes for initial and final
  Grid[ptrInitial->x][ptrInitial->y].up = 1;
  if (GAME == ESCAPE_GAME) {
    if (ptrFinal->x == 0) Grid[ptrFinal->x][ptrFinal->y].left = 1;
    if (ptrFinal->x == SIZEX) Grid[ptrFinal->x][ptrFinal->y].right = 1;
    if (ptrFinal->y == 0) Grid[ptrFinal->x][ptrFinal->y].up = 1;
    if (ptrFinal->y == SIZEY) Grid[ptrFinal->x][ptrFinal->y].down = 1;
  }

  // Draw the maze grid
  setColour(Maze.file, Scheme.lines);
  setColour(Solution.file, Scheme.lines);
  for (x = 0; x <= SIZEX; x++) {
    for (y = 0; y <= SIZEY; y++) {
      if (!Grid[x][y].up)   { drawLine(x, y, x+1,   y); }
      if (!Grid[x][y].left) { drawLine(x, y,   x, y+1); }
    }
  }
  
  // Draw bottom line and right line (since strategy is up/left)
  drawLine(0, SIZEY+1, SIZEX+1, SIZEY+1);
  drawLine(SIZEX+1, 0, SIZEX+1, SIZEY+1);
  
  // Draw holes on bottom or right if GAME is ESCAPE
  if (GAME == ESCAPE_GAME) {
    setColour(Maze.file, Scheme.bgMaze);
    setColour(Solution.file, Scheme.bgMaze);
    if (ptrFinal->x == SIZEX) drawLine(ptrFinal->x+1, ptrFinal->y, ptrFinal->x+1, ptrFinal->y+1);
    // X is preferred over Y
    if (ptrFinal->y == SIZEY && ptrFinal->x != SIZEX) drawLine(ptrFinal->x, ptrFinal->y+1, ptrFinal->x+1, ptrFinal->y+1);
  }
}

/*
 *  drawLine(int, int, int, int)
 *  ----------------------------
 *  Draw a line from one point to another
 */
void drawLine(int x1, int x2, int x3, int x4) {
  x1 = x1*SCALE + PADDINGLEFT;
  x2 = x2*SCALE + PADDINGTOP;
  x3 = x3*SCALE + PADDINGLEFT;
  x4 = x4*SCALE + PADDINGTOP;

  fprintf(Maze.file, "DL %i %i %i %i\n", x1, x2, x3, x4);
  fprintf(Solution.file, "DL %i %i %i %i\n", x1, x2, x3, x4);
}

/*
 *  drawMarkers(struct coords, struct coords)
 *  -----------------------------------------
 *  Put a colored square (marker) at the initial and final point
 */
void drawMarkers(struct coords * ptrInitial, struct coords * ptrFinal) {

  // Draw initial point
  // Although this will happen only with Col.Sch.= 4, this may be extended in future
  if (COLOURSCHEME == 4) {
    setColour(Maze.file, Scheme.finalMarker);
    setColour(Solution.file, Scheme.finalMarker);
    fillCell(0, ptrInitial->x, ptrInitial->y);
  }

  // Draw final point
  setColour(Solution.file, Scheme.finalMarker);
  setColour(Maze.file, Scheme.finalMarker);
  // GAME != ESCAPE_GAME means that it will fill the finalPoint when playing ESCAPE_GAME
  // In addition, always shows with the dark colourscheme 4
  if (GAME != ESCAPE_GAME || COLOURSCHEME == 4) fillCell(0, ptrFinal->x, ptrFinal->y);
}

/*
 *  drawSolution(coords*, coords*)
 *  ------------------------------
 *  Starts from fromCoords and draw a solution to toCoords, backtracking
 *  First solution is written, then 
 */
void drawSolution(struct coords * toCoords, struct coords * fromCoords) {
  int tempX = fromCoords->x, tempY = fromCoords->y;
  setColour(Solution.file, Scheme.solution);

  // Start filling from last cell
  do {
    fillCell(1, tempX, tempY);
    int tempX2 = Grid[tempX][tempY].prevX;
    tempY = Grid[tempX][tempY].prevY;
    tempX = tempX2;
  } while (!(toCoords->x == tempX && toCoords->y == tempY));
  
  // Finally fill initial cell
  fillCell(1, toCoords->x, toCoords->y);
}

/*
 *  fillCell(bool, int, int)
 *  ----------------------------------
 *  Fill a cell with a plain colour
 */
void fillCell(bool onlySolution, int x, int y) {
  x = x*SCALE + PADDINGLEFT;
  y = y*SCALE + PADDINGTOP;
  fprintf(Solution.file, "FR %i %i %i %i\n", x, y, SCALE, SCALE);
  if (!onlySolution) fprintf(Maze.file, "FR %i %i %i %i\n", x, y, SCALE, SCALE);
}

/*
 *  fillMaze(bool, int, int)
 *  ----------------------------------
 *  Fill a the maze with a plain colour
 */
void fillMaze() {
  setColour(Maze.file, Scheme.bgMaze);
  setColour(Solution.file, Scheme.bgMaze);
  fprintf(Maze.file, "FR %i %i %i %i\n", PADDINGLEFT, PADDINGTOP, SCALE*(SIZEX+1), SCALE*(SIZEY+1));
  fprintf(Solution.file, "FR %i %i %i %i\n", PADDINGLEFT, PADDINGTOP, SCALE*(SIZEX+1), SCALE*(SIZEY+1));
}

/*
 *  fillWindow(bool, int, int)
 *  ----------------------------------
 *  Fill a the window with a plain colour
 */
void fillWindow() {
  setColour(Maze.file, Scheme.bgWindow);
  setColour(Solution.file, Scheme.bgWindow);
  fprintf(Maze.file, "FR %i %i %i %i\n", 0, 0, WINDOWX, WINDOWY);
  fprintf(Solution.file, "FR %i %i %i %i\n", 0, 0, WINDOWX, WINDOWY);
  setColour(Maze.file, Scheme.bgMaze);
  setColour(Solution.file, Scheme.bgMaze);
}

/*
 *  loadColourScheme()
 *  ------------------
 *  Load template colourscheme saved in in COLOURSCHEME
 */
void loadColourScheme() {
  switch(COLOURSCHEME) {
    // Default
    case 1:
    Scheme.bgWindow = white;
    Scheme.bgMaze = white;
    Scheme.lines = black;
    Scheme.solution = green;
    Scheme.finalMarker = magenta;
    break;
    // Star wars
    case 2:
    Scheme.bgWindow = darkgray;
    Scheme.bgMaze = black;
    Scheme.lines = yellow;
    Scheme.solution = blue;
    Scheme.finalMarker = cyan;
    break;
    // Funky
    case 3:
    Scheme.bgWindow = orange;
    Scheme.bgMaze = pink;
    Scheme.lines = cyan;
    Scheme.solution = yellow;
    Scheme.finalMarker = red;
    break;
    // B/W
    case 4:
    Scheme.bgWindow = black;
    Scheme.bgMaze = white;
    Scheme.lines = black;
    Scheme.solution = yellow;
    Scheme.finalMarker = orange;
    break;
  }
}

/*
 *  loadLevelSize()
 *  ------------------
 *  Load level (pre-fixed sizes of the maze)
 */
void loadLevelSize() {
  switch(LEVEL) {
    // Default
    case 1:
    SIZEX = 5;
    SIZEY = 5;
    break;
    case 2:
    SIZEX = 10;
    SIZEY = 10;
    break;
    case 3:
    SIZEX = 30;
    SIZEY = 15;
    break;
    case 4:
    SIZEX = 40;
    SIZEY = 40;
    break;
    case 5:
    SIZEX = 100;
    SIZEY = 80;
    break;
  }
}

/*
 *  prepareForDrawing()
 *  -------------------
 *  Compare the scale of x and y (window size) / (size of maze + margin)
 *  Choose the smallest scale, to make it look nicer and set the padding for x and y 
 */
void prepareForDrawing() {
  int scaleX = WINDOWX / (SIZEX+1 +2); // +2 is the top/left padding
  int scaleY = WINDOWY / (SIZEY+1 +2);
  SCALE = (scaleX <= scaleY) ? scaleX : scaleY;
  
  // When scale is too small, it's because of integer calculations
  if (SCALE == 1) SCALE = 2;

  PADDINGTOP = (WINDOWY - SCALE*(SIZEY+1))/2;
  PADDINGLEFT = (WINDOWX - SCALE*(SIZEX+1))/2;

  // Level here
  // int depthLevel = (finalPoint.depth / 10)*LEVEL;
  
  if (COLOURSCHEME == 0) COLOURSCHEME = (rand() % 4)+1;
  loadColourScheme();
  
  // Display all settings
  printf("\nYour settings:\n");
  printf("-x, Horizontal cells: %i\n", SIZEX);
  printf("-y, Vertical cells: %i\n", SIZEY);
  printf("-l Level: %i\n", LEVEL);
  printf("-g Game: %i\n", GAME);
  printf("-c Colourscheme: %i\n", COLOURSCHEME);
  printf("-s Silent mode: %i\n", SILENTMODE);
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

  while (areNeighborsVisited(*pointerX, *pointerY)) {
    prevX = Grid[*pointerX][*pointerY].prevX;
    prevY = Grid[*pointerX][*pointerY].prevY;

    // Set points for next loop
    *pointerX = prevX;
    *pointerY = prevY;

    #ifdef DEBUG
    if (debugFile != (FILE *) 0)
      fprintf(debugFile, ">(%i,%i)", *pointerX, *pointerY);
    #endif

    
    // Nothing can be previous of himself except initial point
    if (*pointerX == Grid[*pointerX][*pointerY].prevX && *pointerY == Grid[*pointerX][*pointerY].prevY)
      break;
  }

}

/*
 *  mazeDraw()
 *  ----------
 *  The drawing strategy consists in drawing .solution and .maze file at the same time.
 *  First solution is written, then 
 */
void mazeDraw() { //TODO level!

  Maze.file = fopen(Maze.name, "wt");
  Solution.file = fopen(Solution.name, "wt");

  // Creating holes for initial and final
  Grid[initialPoint.x][initialPoint.y].up = 1;
  if (GAME == 2) {
    if (finalPoint.x == 0) Grid[finalPoint.x][finalPoint.y].left = 1;
    if (finalPoint.y == 0) Grid[finalPoint.x][finalPoint.y].up = 1;
  }

  // First layer
  fillWindow();
  // Second layer
  fillMaze();
  // Third layer
  drawSolution(&initialPoint, &finalPoint);
  // Fourh layer
  drawMarkers(&initialPoint, &finalPoint);
  // Fifth layer
  drawGrid(&initialPoint, &finalPoint);

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
  } while (cellVisited < (SIZEX+1)*(SIZEY+1));
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
void mazeInitialize() {

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
 *  areNeighborsVisited(int, int)
 *  ----------------------------
 *  Check if neighboors of the cell are visited
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
 *  Set the Grid[][] so that there will be an hole from prevX, prevY to a certain direction
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
  if (debugFile != (FILE *) 0) {
    fprintf(debugFile, "[from(%i, %i):%i]", Grid[x][y].prevX, Grid[x][y].prevY, Grid[Grid[x][y].prevX][Grid[x][y].prevY].depth);
    fprintf(debugFile, "\tto(%i, %i):%i ", x, y, Grid[x][y].depth);
    fprintf(debugFile, "\n");
  }
  #endif
}

/*
 *  isVisited(int, int)
 *  -------------------
 *  Check if cell is visited
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
static void mazeBacktrack_test() {
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

TORRETTA
OPEN WITH maze_opener

*/

// THE VERY END :)