//
// BITMAPMAZE.CPP (BITMAZE.CPP)
//
// An animated tour of a 3D bitmapped maze
//
// Written by Christopher Lampton for
// GARDENS OF IMAGINATION (Waite Group Press,1994)
//
#include "lib/retro.h"
#include "lib/retromain.h"

const int VIEW_LIMIT = 4; // Number of squares visible both forward and to the side. (Should not be changed without adding additional bitmaps.)
const int XWINDOW = 110;  // Upper left corner of view window
const int YWINDOW = 0;
const int WWIDTH = 188;
const int WHEIGHT = 120;

// Array of compass faces:
unsigned char *compass_face[4];

// MAP OF THE MAZE
//
// Each element represents a physical square within the maze.
// A value of 0 means the square is empty, a nonzero value
// means that the square is filled with a solid cube. Map
// must be designed so that no square can be viewed over a
// greater distance than that defined by VIEW_LIMIT.
unsigned char maze[16][16] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1 },
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1 },
	{ 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1 },
	{ 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1 },
	{ 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1 },
	{ 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1 },
	{ 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 },
	{ 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1 },
	{ 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

struct xy {
	int x, y;
};

// Directional increments for north,east,south,west respectively:
struct xy forward[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };
struct xy left[4] = { { 0, -1 }, { -1, 0 }, { 0, 1 }, { 1, 0 } };
struct xy right[4] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };

// Viewer's starting position:
struct xy pos = { 3, 7 };
struct xy newpos;
int dir = 0;  // Viewer's current heading in maze, where 0=north,1=east,2=south,3=west

// Horizontal screen positions corresponding to
// visible intersections on the maze grid:
int matrix[VIEW_LIMIT + 1][VIEW_LIMIT * 2 + 2] = {
	{ 187, 187, 187, 187, 143, 47, 0, 0, 0, 0 },
	{ 187, 187, 187, 179, 123, 66, 10, 0, 0, 0 },
	{ 187, 187, 187, 158, 117, 73, 31, 0, 0, 0 },
	{ 187, 187, 177, 144, 111, 78, 45, 12, 0, 0 },
	{ 187, 185, 159, 133, 107, 82, 56, 30, 4, 0 }
};

//
// Displays vertical slice of a WWIDTHxWHEIGHT bitmap from horizontal
// coordinate LEFTX to horizontal coordinate RIGHTX
//
void display_slice(int imagenum, int leftx, int rightx, unsigned char *screen)
{
	unsigned char *image = RETRO_ImageData(imagenum + 1);

	for (int y = 0; y < WHEIGHT; y++) {
		for (int x = leftx; x < (rightx + 1); x++) {
			screen[(YWINDOW + y) * 320 + XWINDOW + x] = image[y * WWIDTH + x];
		}
	}
}

void grab(int x1, int y1, int width, int height, unsigned char *screen, unsigned char *buffer)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			buffer[y * width + x] = screen[(y1 + y) * 320 + x1 + x];
		}
	}
}

void blit(int x1, int y1, int width, int height, unsigned char *screen, unsigned char *sprite)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char byte = sprite[y * width + x];
			screen[(y1 + y) * 320 + x1 + x] = byte;
		}
	}
}

//
// Recursive function to draw left, right and center walls for the
// maze square at POS.X+XSHIFT, POS.Y+YSHIFT, as viewed from POS.X,
// POS.Y, within the screen window bordered by horizontal coordinates
// LVIEW and RVIEW on the left and right respectively. When called
// with XSHIFT and YSHIFT set to 0, draws entire maze as viewed
// from POS.X,POS.Y out to VIEW_LIMIT squares.
//
void drawmaze(int xshift, int yshift, int lview, int rview, unsigned char *screen)
{
	int vleft, vright, vwidth;

	// Return if we've reached the recursive limit:
	if ((abs(xshift) > VIEW_LIMIT) || (abs(yshift) > VIEW_LIMIT)) {
		return;
	}

	// Calculate coordinates of square to left:
	int sx = pos.x + (xshift + 1) * left[dir].x + yshift * forward[dir].x;
	int sy = pos.y + (xshift + 1) * left[dir].y + yshift * forward[dir].y;

	// Get left-right extent of left part of view:
	vleft = lview;
	vright = matrix[yshift][xshift + VIEW_LIMIT + 1];
	if (vright > rview) {
		vright = rview;
	}
	vwidth = vright - vleft;

	if (vwidth > 0) {
		// If the square to the left is occupied,
		// draw right side of cube:
		if (maze[sx][sy]) {
			display_slice(xshift + VIEW_LIMIT + 1, vleft, vright, screen);
		}

		// Else call function recursively to draw the view from
		// next square to the left:
		else {
			drawmaze(xshift + 1, yshift, vleft, vright, screen);
		}
	}

	// Calculate coordinates of square directly ahead:
	sx = pos.x + xshift * left[dir].x + (yshift + 1) * forward[dir].x;
	sy = pos.y + xshift * left[dir].y + (yshift + 1) * forward[dir].y;

	// Get left-right extent of center part of view:
	vleft = matrix[yshift][xshift + VIEW_LIMIT + 1];
	vright = matrix[yshift][xshift + VIEW_LIMIT];
	if (vleft < lview) {
		vleft = lview;
	}
	if (vright > rview) {
		vright = rview;
	}
	vwidth = vright - vleft;

	if (vwidth > 0) {
		// If the square directly ahead is occupied,
		// draw front of cube:
		if (maze[sx][sy]) {
			display_slice(yshift, vleft, vright, screen);
		}

		// Else call function recursively to draw the view from
		// next square forward:
		else {
			drawmaze(xshift, yshift + 1, vleft, vright, screen);
		}
	}

	// Calculate coordinates of square to right:
	sx = pos.x + (xshift - 1) * left[dir].x + yshift * forward[dir].x;
	sy = pos.y + (xshift - 1) * left[dir].y + yshift * forward[dir].y;

	// Get left-right extent of right part of view:
	vleft = matrix[yshift][xshift + VIEW_LIMIT];
	if (vleft < lview) {
		vleft = lview;
	}
	vright = rview;
	vwidth = vright - vleft;

	if (vwidth > 0) {
		// If the square to the right is occupied,
		// draw left side of cube:
		if (maze[sx][sy]) {
			display_slice(VIEW_LIMIT + 1 - xshift, vleft, vright, screen);
		}

		// Else call function recursively to draw the view from
		// next square to the right:
		else {
			drawmaze(xshift - 1, yshift, vleft, vright, screen);
		}
	}
}

void DEMO_Render(double deltatime)
{
	// Check events
	if (RETRO_KeyPressed(SDL_SCANCODE_UP)) {
		newpos.x = pos.x + forward[dir].x;
		newpos.y = pos.y + forward[dir].y;
		if (!maze[newpos.x][newpos.y]) {
			pos.x = newpos.x;
			pos.y = newpos.y;
		}
	}
	if (RETRO_KeyPressed(SDL_SCANCODE_DOWN)) {
		newpos.x = pos.x - forward[dir].x;
		newpos.y = pos.y - forward[dir].y;
		if (!maze[newpos.x][newpos.y]) {
			pos.x = newpos.x;
			pos.y = newpos.y;
		}
	}
	if (RETRO_KeyPressed(SDL_SCANCODE_RIGHT)) {
		dir++;
		if (dir > 3) {
			dir = 0;
		}
	}
	if (RETRO_KeyPressed(SDL_SCANCODE_LEFT)) {
		--dir;
		if (dir < 0) {
			dir = 3;
		}
	}

	unsigned char *buffer = RETRO_FrameBuffer();

	// Draw the maze in screen buffer:
	drawmaze(0, 0, 0, WWIDTH - 1, buffer);

	// Put compass next to maze window:
	blit(20, 35, 42, 41, buffer, compass_face[dir]);

}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/compass.pcx");
	RETRO_LoadImage("assets/front1.pcx", true);
	RETRO_LoadImage("assets/front2.pcx");
	RETRO_LoadImage("assets/front3.pcx");
	RETRO_LoadImage("assets/front4.pcx");
	RETRO_LoadImage("assets/front5.pcx");
	RETRO_LoadImage("assets/side1.pcx");
	RETRO_LoadImage("assets/side2.pcx");
	RETRO_LoadImage("assets/side3.pcx");
	RETRO_LoadImage("assets/side4.pcx");

	// Load compass faces into array:
	for (int num = 0; num < 4; num++) {
		compass_face[num] = new unsigned char[42 * 41];
		grab(17 + num * 50, 18, 42, 41, RETRO_ImageData(0), compass_face[num]);
	}
}
