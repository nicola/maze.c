#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

struct cell {
	bool visited;
	bool up, down, left, right;
	int prevX, prevY;
	int flag;
};

unsigned int opposite();
	static void opposite_test();
unsigned int randomDirection();
	static void randomDirection_test();
bool isVisited(int x, int y);
	static void isVisited_test();
bool areAllNeighborsVisited(int x, int y);
	static void areAllNeighborsVisited_test();

void mazeBacktrack(int x, int y);
	static void mazeBacktrack_test();
void mazeGenerate();
	static void mazeGenerate_test();
void mazeGrow(int currentX, int currentY);
	static void mazeGrow_test();
void mazeInitialize();
void mazeReset();

void cellCarvePassage(int x, int y, int direction);
	static void cellCarvePassage_test();