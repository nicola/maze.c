///////////////////////////////////////////////////////////////////////////////
// Command line and extra

// Common - functions
void evaluateCommandLine(int, char *arg[]);
char * timeString();
void usage();

///////////////////////////////////////////////////////////////////////////////
// Drawing

// Drawing - definitions
struct drawing {
  char name[35];
  FILE * file;
};
enum colour {black,blue,cyan,darkgray,gray,green,lightgray,magenta,orange,pink,red,white,yellow};
typedef enum colour colour;

// Drawing - functions
void drawLine(int, int, int, int);
void fillCell(bool, int, int, int, int);
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

// Cells - definitions
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