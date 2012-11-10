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
#define SIZEX 2
#define SIZEY 2

struct cell Maze[SIZEX+1][SIZEY+1];
int currentX, currentY;

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
	// mazeGenerate_test();
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
			Maze[x][y].visited   = (x == 0 || x == SIZEX || y == 0 || y == SIZEY) ? 1 : 0;
		}
	}
	return;
}
void mazeGenerate(int currentX, int currentY) {
	long numin = 1;

	do {
		printf("I am at (%i, %i)\n", currentX, currentY);
//		mazeBacktrack(currentX, currentY);
		mazeGrow(currentX, currentY);
		numin++;
	} while (numin < (SIZEX)*(SIZEY)); // TODO check better
}
void mazeGrow(int currentX, int currentY) {
	int completed = 0;
	int newX, newY;

	do {
		int newDirection = randomDirection();
		// printf("randomDirection: %i \n", newDirection);
		switch (newDirection) {
			case UP:
				newX = currentX;
				newY = currentY-1;
			break;
			case DOWN:
				newX = currentX;
				newY = currentY+1;
			break;
			case LEFT:
				newX = currentX-1;
				newY = currentY;
			break;
			case RIGHT:
				newX = currentX+1;
				newY = currentY;
			break;
		}

		// printf("currentX: %i, currentY %i\n", currentX, currentY);
		// printf("newX: %i, newY %i\n", newX, newY);

		// Error Handler
		if (newX < 0 || newY < 0) continue;
		// printf("\nPass -------------------\n\n");
		// printf("Maze[newX][newY] %d\n", Maze[newX][newY].visited);

		if (!isVisited(newX, newY)) {
			// printf("\n Pass isVisited false\n\n");
			cellCarvePassage(currentX, currentY, newDirection);
			Maze[newX][newY].prevX = currentX;
			Maze[newX][newY].prevY = currentY;

			// equivalent currentY++ or currentY-- depending from direction
			currentX = newX;
			currentY = newY;
			// printf("newX: %i, newY %i\n", currentX, currentY);
			completed = 1;
		}
	} while (!completed);
	Maze[currentX][currentY].visited = true;
}
void mazeBacktrack(int *pointerX, int *pointerY) {
	int newX, newY;
	
	// printf("\tareAllNeighborsVisited: %d \n\n", areAllNeighborsVisited(*pointerX, *pointerY));

	while (areAllNeighborsVisited(*pointerX, *pointerY)) {

		// printf("\tcurrentX %i, currentY %i\n", *pointerX, *pointerY);
		// printf("\tprevX %i, prevY %i\n", Maze[*pointerX][*pointerY].prevX, Maze[*pointerX][*pointerY].prevY);

		newX = Maze[*pointerX][*pointerY].prevX;
		newY = Maze[*pointerX][*pointerY].prevY;
		*pointerX = newX;
		*pointerY = newY;

		// printf("\tNew: currentX %i, currentY %i\n", *pointerX, *pointerY);
		// printf("\tareAllNeighborsVisited: %d \n\n", areAllNeighborsVisited(*pointerX, *pointerY));
		
		if (*pointerX == Maze[*pointerX][*pointerY].prevX && *pointerY == Maze[*pointerX][*pointerY].prevY) {

			//printf("\n\n Break! \n\n");

			break; // THIS WILL NEVER HAPPEN, WILL IT?
		}
	}
}
void mazeReset() {
	mazeInitialize();
	int x, y;
	for (x = 0; x <= SIZEX; x++) {
		for (y=0; y <= SIZEY; y++) {
			Maze[x][y].visited = 0;
			Maze[x][y].up = 0;
			Maze[x][y].down = 0;
			Maze[x][y].left = 0;
			Maze[x][y].right = 0;
			Maze[x][y].prevX = 0;
			Maze[x][y].prevY = 0;
		}
	}
	currentX = currentY = 0;
}

/*

  Cell Functions

*/

void cellCarvePassage(int x, int y, int direction) {
	switch (direction) {
		case UP:
			if (y == 0) return;
			Maze[x][y].up = 1;
			Maze[x][y-1].down = 1;
		break;
		case DOWN:
			if (y == SIZEY) return;
			Maze[x][y].down = 1;
			Maze[x][y+1].up = 1;
		break;
		case LEFT:
			if (x == 0) return;
			Maze[x][y].left = 1;
			Maze[x-1][y].right = 1;
		break;
		case RIGHT:
			if (x == SIZEX) return;
			Maze[x][y].right = 1;
			Maze[x+1][y].left = 1;
		break;
	}
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
bool isVisited(int x, int y) {
	return (Maze[x][y].visited);
}
bool areAllNeighborsVisited(int x, int y) {

	bool up = (y == 0) ? 1 : isVisited(x, y-1);
	bool down = (y == SIZEX) ? 1 : isVisited(x, y+1);
	bool left = (x == 0) ? 1 : isVisited(x-1, y);
	bool right = (x == SIZEY) ? 1 : isVisited(x+1, y);

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
	mazeGenerate(0,0);
}
static void mazeGrow_test() {
	mazeReset();
	mazeGrow(0,0);
	assert(Maze[0][1].visited || Maze[1][0].visited);
	mazeGrow(2,2);
	assert(Maze[2][3].visited || Maze[3][2].visited || Maze[2][1].visited || Maze[1][2].visited);
}
static void mazeBacktrack_test() { //TODO
	mazeReset();

//	mazeBacktrack(0,0);
//	assert(!Maze[0][0].prevX && !Maze[0][0].prevY);

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