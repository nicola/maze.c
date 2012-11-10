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
#define SIZEX 4
#define SIZEY 4

#define OUTFILE "MAZE"

// Declaring colors
#define WHITE fprintf(outfile, "%c%c%c", 255,255,255)
#define BLACK fprintf(outfile, "%c%c%c", 0,0,0)
#define RED   fprintf(outfile, "%c%c%c", 0,0,255)

struct cell Maze[SIZEX+1][SIZEY+1];
int currentX, currentY;
long numin = 1;

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
	mazeGenerate_test();

	savebmp(0,0);
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
			//Maze[x][y].visited   = (x == 0 || x == SIZEX || y == 0 || y == SIZEY) ? 1 : 0;
		}
	}
	Maze[0][0].visited = 1;
	return;
}
void mazeGenerate(int *pointerX, int *pointerY) {
	numin = 1;
	printf("\nTotal sum to reach %i\n", (SIZEX+1)*(SIZEY+1));

	do {
		printf("I am at (%i, %i) ", *pointerX, *pointerY);
		mazeBacktrack(pointerX, pointerY);
		printf("B(%i, %i) ", *pointerX, *pointerY);
		mazeGrow(pointerX, pointerY);
		printf("then at (%i, %i)\n", *pointerX, *pointerY);
		numin++;
	} while (numin < (SIZEX+1)*(SIZEY+1)); // TODO check better

	return;
}
void mazeGrow(int *pointerX, int *pointerY) {
	int completed = 0;
	int newX, newY;

	do {
		int newDirection = randomDirection();
		// printf("randomDirection: %i \n", newDirection);
		switch (newDirection) {
			case UP:
				newX = *pointerX;
				newY = *pointerY-1;
			break;
			case DOWN:
				newX = *pointerX;
				newY = *pointerY+1;
			break;
			case LEFT:
				newX = *pointerX-1;
				newY = *pointerY;
			break;
			case RIGHT:
				newX = *pointerX+1;
				newY = *pointerY;
			break;
		}

		// printf("currentX: %i, currentY %i\n", currentX, currentY);
		// printf("newX: %i, newY %i\n", newX, newY);

		// Error Handler
		if (newX < 0 || newY < 0 || newX > SIZEX || newY > SIZEY) continue;
		// printf("\nPass -------------------\n\n");
		// printf("Maze[newX][newY] %d\n", Maze[newX][newY].visited);

		if (!isVisited(newX, newY)) {
			printf(" going %i ", newDirection);
			// printf("\n Pass isVisited false\n\n");
			cellCarvePassage(*pointerX, *pointerY, newDirection);
			Maze[newX][newY].prevX = *pointerX;
			Maze[newX][newY].prevY = *pointerY;

			// equivalent currentY++ or currentY-- depending from direction
			*pointerX = newX;
			*pointerY = newY;
			// printf("newX: %i, newY %i\n", currentX, currentY);
			completed = 1;
		}
	} while (!completed);
	Maze[*pointerX][*pointerY].visited = true;
}
void mazeBacktrack(int *pointerX, int *pointerY) {
	int newX, newY;
	
	// printf("\tareAllNeighborsVisited: %d \n\n", areAllNeighborsVisited(*pointerX, *pointerY));

	printf(" [ ");
	while (areAllNeighborsVisited(*pointerX, *pointerY)) {

		// printf("\tcurrentX %i, currentY %i\n", *pointerX, *pointerY);
		// printf("\tprevX %i, prevY %i\n", Maze[*pointerX][*pointerY].prevX, Maze[*pointerX][*pointerY].prevY);
		newX = Maze[*pointerX][*pointerY].prevX;
		newY = Maze[*pointerX][*pointerY].prevY;
		*pointerX = newX;
		*pointerY = newY;
		printf("> Backtrack (%i,%i) ", newX, newY);

		// printf("\tNew: currentX %i, currentY %i\n", *pointerX, *pointerY);
		// printf("\tareAllNeighborsVisited: %d \n\n", areAllNeighborsVisited(*pointerX, *pointerY));
		
		if (*pointerX == Maze[*pointerX][*pointerY].prevX && *pointerY == Maze[*pointerX][*pointerY].prevY) {

			//printf("\n\n Break! \n\n");

			break; // THIS WILL NEVER HAPPEN, WILL IT?
		}
	}
	printf(" ] ");
}
void mazeReset() {
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

	mazeInitialize();
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
	currentX = 0;
	currentY = 0;
	mazeGenerate(&currentX,&currentY);
}
static void mazeGrow_test() {
	mazeReset();
	currentX = 0;
	currentY = 0;
	mazeGrow(&currentX,&currentY);
	assert(Maze[0][1].visited || Maze[1][0].visited);
	currentX = 2;
	currentY = 2;
	mazeGrow(&currentX,&currentY);
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

void savebmp(int xspecial, int yspecial){
			//save a bitmap file! the xspecial, yspecial pixel is coloured red.
	FILE * outfile;
	int extrabytes, paddedsize;
	int x, y, n;
	int width = (SIZEX)*2-1;
	int height = (SIZEY)*2-1;

	extrabytes  =  (4 - ((width * 3) % 4))%4; 

	char filename[200];

	sprintf(filename, "%s_%ix%i_n%ld.bmp", OUTFILE, SIZEX+1, SIZEY+1, numin);
	paddedsize  =  ((width * 3) + extrabytes) * height;

	unsigned int headers[13]  =  {paddedsize + 54, 0, 54, 40, width, height, 0, 0, paddedsize, 0, 0, 0, 0};

	outfile  =  fopen(filename, "wb");
	fprintf(outfile, "BM");

	for (n  =  0; n <=  5; n++){
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
	}

	fprintf(outfile, "%c", 1);fprintf(outfile, "%c", 0);
	fprintf(outfile, "%c", 24);fprintf(outfile, "%c", 0);

	for (n  =  7; n <=  12; n++){
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
	}

			//Actual writing of data begins here:
	for(y  =  0; y <=  height - 1; y++){
		for(x  =  0; x <=  width - 1; x++){
			if(x%2  ==  1 && y%2  ==  1){
				if(x/2+1  ==  xspecial && y/2+1  ==  yspecial) RED;
				else{
					if(Maze[x/2+1][y/2+1].visited) WHITE; else BLACK;
				}
			}else if(x%2  ==  0 && y%2  ==  0){
				BLACK;
			}else if(x%2  ==  0 && y%2  ==  1){
				if(Maze[x/2+1][y/2+1].left) BLACK; else WHITE;
			}else if(x%2  ==  1 && y%2  ==  0){
				if(Maze[x/2+1][y/2+1].up) BLACK; else WHITE;
			}
		}
		if (extrabytes){     // See above - BMP lines must be of lengths divisible by 4.
			for (n  =  1; n <=  extrabytes; n++){
				fprintf(outfile, "%c", 0);
			}
		}
	}
	printf("file printed: %s\n", filename); 
	fclose(outfile);
	return;
}