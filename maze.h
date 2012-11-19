///////////////////////////////////////////////////////////////////////////////
// Definitions

// Definitions - drawings
struct drawing {
  char name[35];
  FILE * file;
};
enum colour {black,blue,cyan,darkgray,gray,green,lightgray,magenta,orange,pink,red,white,yellow};
typedef enum colour colour;

struct colourscheme {
  colour bgWindow;
  colour bgMaze;
  colour lines;
  colour solution;
  colour finalMarker;
};

// Definitions - cells
struct cell {
  bool visited;
  bool up, down, left, right;
  int prevX, prevY;
  int depth;
};
struct coords {
  int x;
  int y;
  int * depth;
};


///////////////////////////////////////////////////////////////////////////////
// Command line and extra

// Common - functions
void evaluateCommandLine(int, char *arg[]);
bool isNumeric(char*);
void saveIfNumeric(int*, char*);
char * timeString();
void usage();

///////////////////////////////////////////////////////////////////////////////
// Drawing

// Drawing - functions
void drawGrid (struct coords *, struct coords *);
void drawLine(int, int, int, int);
void drawMarkers(struct coords *, struct coords *);
void drawSolution(struct coords *, struct coords *);
void fillCell(bool, int, int);
void loadColourScheme();
void loadLevelSize();
void prepareForDrawing();
void setColour(FILE*, colour);

///////////////////////////////////////////////////////////////////////////////
// Maze

// Maze - functions
void mazeBacktrack(int*, int*);
void mazeDraw();
void mazeGenerate();
void mazeGrow(int*, int *);
void mazeInitialize();
void mazeReset();
void mazeSetLongestPath(int, int);


// Maze - tests
static void mazeBacktrack_test();
static void mazeDraw_test();
static void mazeGenerate_test();
static void mazeGrow_test();

///////////////////////////////////////////////////////////////////////////////
// Cells

// Cells - functions
bool areNeighborsVisited(int, int);
void cellCarvePassage(int, int, int);
bool isVisited(int, int);
unsigned int randomDirection();

// Cells - tests
static void areNeighborsVisited_test();
static void cellCarvePassage_test();
static void isVisited_test();
static void randomDirection_test();

// THE VERY END :)