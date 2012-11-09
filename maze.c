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
	int flag;
};
struct cell Maze[SIZEX][SIZEY];

unsigned int opposite();
unsigned int randomDirection();
void mazeBacktrack();
void mazeCarvePassage(int x, int y, int direction);
static void opposite_test();
static void randomDirection_test();
static void mazeBacktrack_test();
static void mazeCarvePassage_test();

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


void mazeBacktrack() {
	int currentX, currentY;
	while (Maze[currentX][currentY-1].visited && Maze[currentX][currentY+1].visited && Maze[currentX-1][currentY].visited && Maze[currentX+1][currentY].visited) {
		
	}
}

void mazeGenerate(void) {
	int currentX = 1, currentY = 1;
	int goingTo;
	bool completed;

#ifdef nodeadend
	do {
		success = 0;
		switch (randomDirection()) {
			case UP:
			break;
			case DOWN:
			break;
			case LEFT:
			break;
			case RIGHT:
			break;
		}
		
	} while(!completed);
#endif

#ifdef backtrack
	mazeBacktrack();
#endif

}


int main() {
	
	// Test
	opposite_test();
	randomDirection_test();
	mazeCarvePassage_test();
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