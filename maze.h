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

struct drawing {
  char name[200];
  FILE * file;
};

unsigned int opposite();
  static void opposite_test();
unsigned int randomDirection();
  static void randomDirection_test();
char * timeString();

bool isVisited(int, int);
  static void isVisited_test();
bool areAllNeighborsVisited(int, int);
  static void areAllNeighborsVisited_test();

void mazeBacktrack(int*, int*);
  static void mazeBacktrack_test();
void mazeGenerate();
  static void mazeGenerate_test();
void mazeGrow(int*, int *);
  static void mazeGrow_test();
void mazeInitialize();
void mazeReset();

void cellCarvePassage(int, int, int);
  static void cellCarvePassage_test();

void mazeDraw();
  static void mazeDraw_test();

void drawLine(int, int, int, int);