// #include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3
#define SIZEX 50
#define SIZEY 50

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

struct cell {
	bool visited;
	struct cell *up, *down, *left, *right;
	int prevX, prevY;
	int flag;
};
struct cell Maze[SIZEX][SIZEY];
int currentX, currentY;

unsigned int opposite();
unsigned int randomDirection();
bool isVisited(int x, int y);
bool areAllNeighborsVisited(int x, int y);

void mazeBacktrack(int x, int y);
void mazeGrow(int currentX, int currentY);

void cellCarvePassage(int x, int y, int direction);
void cellSetPrev(int x, int y, int direction);

void mazeReset();

static void opposite_test();
static void randomDirection_test();
static void mazeBacktrack_test();
static void cellCarvePassage_test();
static void mazeGrow_test();
static void isVisited_test();
static void areAllNeighborsVisited_test();
static void cellSetPrev_test();

void mazeGenerate(int currentX, int currentY) {
	long numin = 1;

	do {
		mazeBacktrack(currentX, currentY);
		mazeGrow(currentX, currentY);
		numin++;
	} while (numin < (SIZEX)*(SIZEY));
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


int main() {
	srand((unsigned int)time(NULL));
	
	// Test
	opposite_test();
	randomDirection_test();
	cellCarvePassage_test();
	isVisited_test();
	areAllNeighborsVisited_test();
	mazeGrow_test();
	return 0;
}

void mazeBacktrack(int currentX, int currentY) {
	while (areAllNeighborsVisited(currentX, currentY)) {
		currentX = Maze[currentX][currentY].prevX;
		currentY = Maze[currentX][currentY].prevY;
	}
}

void cellCarvePassage(int x, int y, int direction) {
	switch (direction) {
		case UP:
			if (y == 0) return;
			Maze[x][y].up = &Maze[x][y-1];
			Maze[x][y-1].down = &Maze[x][y];
		break;
		case DOWN:
			if (y == SIZEY) return;
			Maze[x][y].down = &Maze[x][y+1];
			Maze[x][y+1].up = &Maze[x][y];
		break;
		case LEFT:
			if (x == 0) return;
			Maze[x][y].left = &Maze[x-1][y];
			Maze[x-1][y].right = &Maze[x][y];
		break;
		case RIGHT:
			if (x == SIZEX) return;
			Maze[x][y].right = &Maze[x+1][y];
			Maze[x+1][y].left = &Maze[x][y];
		break;
	}
}

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
void mazeReset() {
	int x, y;
	for (x = 0; x <= SIZEX; x++) {
		for (y=0; y <= SIZEY; y++) {
			Maze[x][y].visited = 0;
			Maze[x][y].up = NULL;
			Maze[x][y].down = NULL;
			Maze[x][y].left = NULL;
			Maze[x][y].right = NULL;
			Maze[x][y].prevX = 0;
			Maze[x][y].prevY = 0;
		}
	}
}

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

	assert(!areAllNeighborsVisited(2,2));
}

static void cellSetPrev_test() {
}
static void cellCarvePassage_test() {
	cellCarvePassage(2,3, UP);
	assert(Maze[2][3].up == &Maze[2][2]);
	cellCarvePassage(0,0, UP);
	assert(Maze[0][0].up == NULL);
	cellCarvePassage(0,0, LEFT);
	assert(Maze[0][0].left == NULL);

	cellCarvePassage(SIZEX,SIZEY, DOWN);
	assert(Maze[SIZEX][SIZEY].down == NULL);
	cellCarvePassage(SIZEX,SIZEY, RIGHT);
	assert(Maze[SIZEX][SIZEY].right == NULL);
}

static void mazeGrow_test() {
	mazeReset();
	mazeGrow(0,0);
	assert(Maze[0][1].visited || Maze[1][0].visited);
	mazeGrow(2,2);
	assert(Maze[2][3].visited || Maze[3][2].visited || Maze[2][1].visited || Maze[1][2].visited);
}
static void mazeBacktrack_test() {
	int currentX = 0, currentY = 0;
	Maze[1][0].prevX = 0;
	Maze[1][0].prevY = 0;

	Maze[2][0].prevX = 1;
	Maze[2][0].prevY = 0;

	Maze[3][0].prevX = 2;
	Maze[3][0].prevY = 0;

	mazeBacktrack(currentX, currentY);
}

