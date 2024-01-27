//
// BITMAPMAZE2.CPP (TEXTMAZE.CPP)
//
// An animated tour of a 3D texture-mapped maze
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
const int POLYWIDTH = 96;
const int POLYHEIGHT = 79;

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

typedef struct xy {
	int x, y;
};

// Directional increments for north,east,south,west
// respectively:
struct xy forward[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };
struct xy left[4] = { { 0, -1 }, { -1, 0 }, { 0, 1 }, { 1, 0 } };
struct xy right[4] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };

// Viewer's starting position:
struct xy pos = { 3, 9 };
struct xy newpos;
int dir = 1;  // Viewer's current heading in maze, where 0=north,1=east,2=south,3=west

// Horizontal screen positions corresponding to
// visible intersections on the maze grid:
int matrix[VIEW_LIMIT + 2][VIEW_LIMIT * 2 + 2] = {
	{ 187, 187, 187, 187, 187, 0, 0, 0, 0, 0 },
	{ 187, 187, 187, 238, 143, 47, -49, 0, 0, 0 },
	{ 187, 187, 187, 179, 123, 66, 10, 0, 0, 0 },
	{ 187, 187, 187, 158, 117, 73, 31, 0, 0, 0 },
	{ 187, 187, 177, 144, 111, 78, 45, 12, 0, 0 },
	{ 187, 185, 159, 133, 107, 82, 56, 30, 4, 0 }
};

int top[VIEW_LIMIT + 2] = { -19, 20, 36, 42, 46, 49 };
int bottom[VIEW_LIMIT + 2] = { 138, 99, 83, 77, 73, 70 };

// Structure to hold polygon data:
struct tpolytype {
	int x[4];
	int y[4];
	int wtext;
	int htext;
	unsigned char *textmap;
};

tpolytype tpoly;

void drawbg(int num, unsigned char *screen)
{
	unsigned int offset1 = YWINDOW * 320 + XWINDOW;
	unsigned int offset2 = 0;
	for (int i = 0; i < WHEIGHT; i++) {
		for (int j = 0; j < WWIDTH; j++) {
			screen[offset1++] = RETRO_ImageData(num)[offset2++];
		}
		offset1 += (320 - WWIDTH);
	}
}

void grab(int x1, int y1, int width, int height, int swidth, unsigned char *screen, unsigned char *buffer)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			buffer[y * width + x] = screen[(y1 + y) * swidth + x1 + x];
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

void polytext(tpolytype *tpoly, int leftx, int lefty, int rightx,
	int righty, unsigned char *screen)
{
	float tslope, bslope;
	int vwidth, vy, vheight;

	// Initialize pointer into texture map array:

	unsigned int tptr = 0;

	// Initialize height and width of texture map:

	int theight = tpoly->htext;
	int twidth = tpoly->wtext;
	int swidth = tpoly->x[1] - tpoly->x[0] + 1;

	// If polygon crosses left side of viewport, clip it:

	if (tpoly->x[0] < leftx) {

		// Calculate offset of clip in texture map array:

		int wtemp = tpoly->x[1] - tpoly->x[0] + 1;
		int offtemp = leftx - tpoly->x[0];
		float xratio = (float)tpoly->wtext / wtemp;
		tptr = offtemp * xratio;
		tpoly->wtext -= tptr;

		// Calculate slopes for top and bottom edges:

		tslope = (float)(tpoly->y[1] - tpoly->y[0]) / (tpoly->x[1] - tpoly->x[0]);
		bslope = (float)(tpoly->y[3] - tpoly->y[2]) / (tpoly->x[0] - tpoly->x[1]);

		// Find new endpoints for clipped lines:

		tpoly->y[0] = tpoly->y[0] + tslope * (leftx - tpoly->x[0]);
		tpoly->y[3] = tpoly->y[3] + bslope * (leftx - tpoly->x[0]);
		tpoly->x[0] = leftx;
	}

	// Initialize x,y coordinates for polygon drawing:

	int x = tpoly->x[0];
	int y = tpoly->y[0];

	// Calculate the change in y coordinates for top
	// and bottom edges:

	int topdiff = tpoly->y[1] - tpoly->y[0];
	int botdiff = tpoly->y[2] - tpoly->y[3];

	// Initialize height and width of clipped polygon:

	int height = tpoly->y[3] - tpoly->y[0];
	int width = tpoly->x[1] - tpoly->x[0] + 1;

	// Clip polygon width against right side of viewport:

	if (tpoly->x[1] > rightx)
		vwidth = width - (tpoly->x[1] - rightx);
	else vwidth = width;

	// Initialize top and bottom error terms:

	int toperror = 0;
	int boterror = 0;

	// Initialize horizontal error term for texture map:

	int txerror = 0;

	// Loop across width of polygon:

	for (int w = 0; w < vwidth; w++) {

		// Initialize temporary offset into texture map:

		int t = tptr;

		// If top of current vertical line is outside of
		// viewport, clip it:

		if (y < lefty) {
			vy = lefty;
			vheight = height - (lefty - y);
			float yratio = (float)tpoly->htext / height;
			t += (lefty - y) * yratio;
		} else {
			vy = y;
			vheight = height;
		}
		if ((vy + vheight) > righty)
			vheight = righty - vy;

		// Point to video memory offset for top of line:

		unsigned int ptr = vy * 320 + x;

		// Initialize vertical error term for texture map:

		int tyerror = 0;

		// Loop through the pixels in the current vertical
		// line, advancing PTR to the next row of pixels after
		// each pixel is drawn.

		for (int h = 0; h < vheight; h++) {
			screen[ptr] = tpoly->textmap[t];

			// Is it time to move to next row of pixels in map?

			while (tyerror < theight) {
				tyerror += height;
				t += twidth;
			}
			tyerror -= theight;
			ptr += 320;
		}
		x++;

		// Advance x coordinate to next pixel column:

		toperror += abs(topdiff);

		// If so move it up...

		while (toperror >= width) {

			// If so move it up...

			toperror -= width;
			if (topdiff > 0) {
				y++;
				--height;
			} else {
				--y;
				height++;
			}
		}
		boterror += abs(botdiff);
		while (boterror >= width) {
			boterror -= width;
			if (botdiff > 0) height++;
			else --height;
		}

		//  Is it time to move to next pixel column in map?

		while (txerror < twidth) {
			txerror += swidth;
			tptr++;
		}
		txerror -= twidth;
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

	if ((abs(xshift) > VIEW_LIMIT) || (abs(yshift) > VIEW_LIMIT))
		return;

	// Calculate coordinates of square to left:

	int sx = pos.x + (xshift + 1) * left[dir].x + yshift * forward[dir].x;
	int sy = pos.y + (xshift + 1) * left[dir].y + yshift * forward[dir].y;

	// Get left-right extent of left part of view:

	vleft = lview;
	vright = matrix[yshift + 1][xshift + VIEW_LIMIT + 1];
	if (vright > rview) vright = rview;
	vwidth = vright - vleft;

	if (vwidth > 0) {

		// If the square to the left is occupied,
		// draw right side of cube:

		if (maze[sx][sy]) {

			tpoly.x[0] = matrix[yshift][xshift + VIEW_LIMIT + 1] + XWINDOW;
			tpoly.y[0] = top[yshift] + YWINDOW;
			tpoly.x[1] = matrix[yshift + 1][xshift + VIEW_LIMIT + 1] + XWINDOW;
			tpoly.y[1] = top[yshift + 1] + YWINDOW;
			tpoly.y[2] = bottom[yshift + 1] + YWINDOW;
			tpoly.y[3] = bottom[yshift] + YWINDOW;
			tpoly.wtext = POLYWIDTH;
			tpoly.htext = POLYHEIGHT;
			polytext(&tpoly, vleft + XWINDOW, YWINDOW, vright + XWINDOW,
				YWINDOW + WHEIGHT, screen);
		}

		// Else call function recursively to draw the view from
		// next square to the left:

		else drawmaze(xshift + 1, yshift, vleft, vright, screen);

	}

	// Calculate coordinates of square directly ahead:

	sx = pos.x + xshift * left[dir].x + (yshift + 1) * forward[dir].x;
	sy = pos.y + xshift * left[dir].y + (yshift + 1) * forward[dir].y;


	// Get left-right extent of center part of view:

	vleft = matrix[yshift + 1][xshift + VIEW_LIMIT + 1];
	vright = matrix[yshift + 1][xshift + VIEW_LIMIT];
	if (vleft < lview) vleft = lview;
	if (vright > rview) vright = rview;
	vwidth = vright - vleft;

	if (vwidth > 0) {

		// If the square directly ahead is occupied,
		// draw front of cube:

		if (maze[sx][sy]) {
			tpoly.x[0] = matrix[yshift + 1][xshift + VIEW_LIMIT + 1] + XWINDOW;
			tpoly.y[0] = top[yshift + 1] + YWINDOW;
			tpoly.x[1] = matrix[yshift + 1][xshift + VIEW_LIMIT] + XWINDOW;
			tpoly.y[1] = top[yshift + 1] + YWINDOW;
			tpoly.y[2] = bottom[yshift + 1] + YWINDOW;
			tpoly.y[3] = bottom[yshift + 1] + YWINDOW;
			tpoly.wtext = POLYWIDTH;
			tpoly.htext = POLYHEIGHT;
			polytext(&tpoly, vleft + XWINDOW, YWINDOW, vright + XWINDOW,
				YWINDOW + WHEIGHT, screen);
		}

		// Else call function recursively to draw the view from
		// next square forward:

		else drawmaze(xshift, yshift + 1, vleft, vright, screen);
	}

	// Calculate coordinates of square to right:

	sx = pos.x + (xshift - 1) * left[dir].x + yshift * forward[dir].x;
	sy = pos.y + (xshift - 1) * left[dir].y + yshift * forward[dir].y;

	// Get left-right extent of right part of view:

	vleft = matrix[yshift + 1][xshift + VIEW_LIMIT];
	if (vleft < lview) vleft = lview;
	vright = rview;
	vwidth = vright - vleft;

	if (vwidth > 0) {

		// If the square to the right is occupied,
		// draw left side of cube:

		if (maze[sx][sy]) {
			tpoly.x[0] = matrix[yshift + 1][xshift + VIEW_LIMIT] + XWINDOW;
			tpoly.y[0] = top[yshift + 1] + YWINDOW;
			tpoly.x[1] = matrix[yshift][xshift + VIEW_LIMIT] + XWINDOW;
			tpoly.y[1] = top[yshift] + YWINDOW;
			tpoly.y[2] = bottom[yshift] + YWINDOW;
			tpoly.y[3] = bottom[yshift + 1] + YWINDOW;
			tpoly.wtext = POLYWIDTH;
			tpoly.htext = POLYHEIGHT;
			polytext(&tpoly, vleft + XWINDOW, YWINDOW, vright + XWINDOW,
				YWINDOW + WHEIGHT, screen);
		}

		// Else call function recursively to draw the view from
		// next square to the right:

		else drawmaze(xshift - 1, yshift, vleft, vright, screen);
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
	drawbg(1, buffer);
	drawmaze(0, 0, 0, WWIDTH - 1, buffer);

	// Put compass next to maze window:
	blit(20, 35, 42, 41, buffer, compass_face[dir]);

}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/compass.pcx");
	RETRO_LoadImage("assets/empty.pcx", true);
	RETRO_LoadImage("assets/empty.pcx");
	RETRO_LoadImage("assets/front.pcx");

	// Allocate memory for wall bitmap:
	tpoly.textmap = new unsigned char[POLYWIDTH * POLYHEIGHT];

	// Grab wall image in array:
	grab(47, 20, POLYWIDTH, POLYHEIGHT, 188, RETRO_ImageData(3), tpoly.textmap);

	// Load compass faces into array:
	for (int num = 0; num < 4; num++) {
		compass_face[num] = new unsigned char[42 * 41];
		grab(17 + num * 50, 18, 42, 41, 320, RETRO_ImageData(0), compass_face[num]);
	}
}
