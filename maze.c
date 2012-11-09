// #include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

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

unsigned int opposite();
unsigned int randomDirection();
void mazeBacktrack(int x, int y);
void mazeCarvePassage(int x, int y, int direction);
void mazeGrow();
bool isVisited(int x, int y);
bool areAllNeighborsVisited(int x, int y);
static void opposite_test();
static void randomDirection_test();
static void mazeBacktrack_test();
static void mazeCarvePassage_test();
static void mazeGrow_test();
static void isVisited_test();
static void areAllNeighborsVisited_test();

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

void mazeCarvePassage(int x, int y, int direction) {
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


void mazeBacktrack(int currentX, int currentY) {
	while (areAllNeighborsVisited(currentX, currentY)) {
		currentX = Maze[currentX][currentY].prevX;
		currentY = Maze[currentX][currentY].prevY;
	}
}

void mazeGenerate(void) {
	int currentX = 0, currentY = 0;
	int goingTo;
	bool completed;
	long numin=1;

	do {
		mazeBacktrack(currentX, currentY);
//		mazeGrow();
	} while (numin < (SIZEX)*(SIZEY));
}



int main() {
	
	// Test
	opposite_test();
	randomDirection_test();
	mazeCarvePassage_test();
	isVisited_test();
	areAllNeighborsVisited_test();
	return 0;
}

unsigned int opposite(int direction) {
	return (direction + 2) % 4;
}

static void opposite_test() {
	assert( opposite(UP) == DOWN);
	assert( opposite(DOWN) == UP);
	assert( opposite(LEFT) == RIGHT);
	assert( opposite(RIGHT) == LEFT);
}
unsigned int randomDirection() {
	return rand() % 4;
}
static void randomDirection_test() {
	assert(randomDirection() == UP || randomDirection() == DOWN || randomDirection() == LEFT || randomDirection() == RIGHT);
}
static void mazeCarvePassage_test() {
	mazeCarvePassage(2,3, UP);
	assert(Maze[2][3].up == &Maze[2][2]);
	mazeCarvePassage(0,0, UP);
	assert(Maze[0][0].up == NULL);
	mazeCarvePassage(0,0, LEFT);
	assert(Maze[0][0].left == NULL);

	mazeCarvePassage(SIZEX,SIZEY, DOWN);
	assert(Maze[SIZEX][SIZEY].down == NULL);
	mazeCarvePassage(SIZEX,SIZEY, RIGHT);
	assert(Maze[SIZEX][SIZEY].right == NULL);
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