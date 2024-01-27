//
// WIREMAZE.CPP (GOMAZE.CPP)
//
// An animated tour of 3D wireframe maze
//
// Written by Christopher Lampton for
// Gardens of Imagination (Waite Group Press,1994)
//
#include "lib/retro.h"
#include "lib/retromain.h"

char maze[16][16] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

typedef struct xy {
	int x, y;
};

struct xy increment[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };
struct xy left[4] = { { 0, -1 }, { -1, 0 }, { 0, 1 }, { 1, 0 } };
struct xy right[4] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };

struct xy pos = { 5, 5 };
struct xy newpos;
int direction = 0;
int visibility = 4;

void linedraw(int x1, int y1, int x2, int y2, int color,
	unsigned char *screen)
{
	int y_unit, x_unit; // Variables for amount of change in x and y

	int offset = y1 * 320 + x1; // Calculate offset into video RAM

	int ydiff = y2 - y1;   // Calculate difference between y coordinates
	if (ydiff < 0) {       // If the line moves in the negative direction
		ydiff = -ydiff;    // ...get absolute value of difference
		y_unit = -320;     // ...and set negative unit in y dimension
	} else y_unit = 320;   // Else set positive unit in y dimension

	int xdiff = x2 - x1;			// Calculate difference between x coordinates
	if (xdiff < 0) {				// If the line moves in the negative direction
		xdiff = -xdiff;				// ...get absolute value of difference
		x_unit = -1;				// ...and set negative unit in x dimension
	} else x_unit = 1;				// Else set positive unit in y dimension

	int error_term = 0;			// Initialize error term
	if (xdiff > ydiff) {		// If difference is bigger in x dimension
		int length = xdiff + 1;	// ...prepare to count off in x direction
		for (int i = 0; i < length; i++) {  // Loop through points in x direction
			screen[offset] = color;	// Set the next pixel in the line to COLOR
			offset += x_unit;				// Move offset to next pixel in x direction
			error_term += ydiff;		// Check to see if move required in y direction
			if (error_term > xdiff) {	// If so...
				error_term -= xdiff;		// ...reset error term
				offset += y_unit;				// ...and move offset to next pixel in y dir.
			}
		}
	} else {								// If difference is bigger in y dimension
		int length = ydiff + 1;	// ...prepare to count off in y direction
		for (int i = 0; i < length; i++) {	// Loop through points in y direction
			screen[offset] = color;	// Set the next pixel in the line to COLOR
			offset += y_unit;				// Move offset to next pixel in y direction
			error_term += xdiff;    // Check to see if move required in x direction
			if (error_term > 0) {		// If so...
				error_term -= ydiff;	// ...reset error term
				offset += x_unit;			// ...and move offset to next pixel in x dir.
			}
		}
	}
}

//
// Draw box around maze window:
//
void drawbox(unsigned char *screen)
{
	linedraw(82, 19, 294, 19, 15, screen);
	linedraw(294, 19, 294, 119, 15, screen);
	linedraw(294, 119, 82, 119, 15, screen);
	linedraw(82, 119, 82, 19, 15, screen);
}

//
// Draw the maze stored in array maze[] into video
// memory at address screen.
//
void drawmaze(unsigned char *screen)
{
	struct xy block, lblock, rblock;
	int oldleft, oldright;

	// Draw the maze at each distance allowed by visibility

	for (int dist = 0; dist < visibility; dist++) {

		// Find current square of maze:

		block.x = pos.x + dist * increment[direction].x;
		block.y = pos.y + dist * increment[direction].y;

		// Find square to the left of current square:

		lblock.x = block.x + left[direction].x;
		lblock.y = block.y + left[direction].y;

		// Find square to the right of current square:

		rblock.x = block.x + right[direction].x;
		rblock.y = block.y + right[direction].y;

		// Draw image of squares according to distance:

		switch (dist) {
		case 0:  // Draw current square

			// Is wall open to the left?
			// If not, draw wall

			if (maze[lblock.x][lblock.y]) {
				linedraw(82, 19, 135, 44, 15, screen);
				linedraw(135, 44, 135, 93, 15, screen);
				linedraw(135, 93, 82, 118, 15, screen);
			} else { // Else draw opening
				if (maze[lblock.x + increment[direction].x]
					[lblock.y + increment[direction].y]) {
					linedraw(82, 44, 135, 44, 15, screen);
					linedraw(135, 44, 135, 93, 15, screen);
					linedraw(135, 93, 82, 93, 15, screen);
				}
			}

			// Is wall open to the right?
			// If not, draw wall

			if (maze[rblock.x][rblock.y]) {
				linedraw(294, 19, 242, 44, 15, screen);
				linedraw(242, 44, 242, 93, 15, screen);
				linedraw(294, 118, 242, 93, 15, screen);
			} else { // Else draw opening
				if (maze[rblock.x + increment[direction].x]
					[rblock.y + increment[direction].y]) {
					linedraw(294, 44, 242, 44, 15, screen);
					linedraw(242, 44, 242, 93, 15, screen);
					linedraw(242, 93, 294, 93, 15, screen);
				}
			}
			break;
		case 1: // Repeat for next square

			// Can we see through the next square?
			// If not, draw wall

			if (maze[block.x][block.y]) {
				linedraw(135, 44, 135, 93, 15, screen);
				linedraw(242, 44, 242, 93, 15, screen);
				linedraw(135, 44, 242, 44, 15, screen);
				linedraw(135, 93, 242, 93, 15, screen);
			} else { // Else draw sides of next square
				if (maze[lblock.x][lblock.y]) {
					linedraw(135, 44, 162, 57, 15, screen);
					linedraw(162, 57, 162, 80, 15, screen);
					linedraw(162, 80, 135, 93, 15, screen);
				} else {
					if (maze[lblock.x + increment[direction].x]
						[lblock.y + increment[direction].y]) {
						linedraw(135, 57, 162, 57, 15, screen);
						linedraw(162, 57, 162, 80, 15, screen);
						linedraw(162, 80, 135, 80, 15, screen);
					}
				}
				if (maze[rblock.x][rblock.y]) {
					linedraw(242, 44, 215, 57, 15, screen);
					linedraw(215, 57, 215, 80, 15, screen);
					linedraw(215, 80, 242, 93, 15, screen);
				} else {
					if (maze[rblock.x + increment[direction].x]
						[rblock.y + increment[direction].y]) {
						linedraw(242, 57, 215, 57, 15, screen);
						linedraw(215, 57, 215, 80, 15, screen);
						linedraw(215, 80, 242, 80, 15, screen);
					}
				}
			}
			break;
		case 2: // Do it again
			if (maze[block.x][block.y]) {
				linedraw(162, 57, 162, 80, 15, screen);
				linedraw(215, 57, 215, 80, 15, screen);
				linedraw(162, 57, 215, 57, 15, screen);
				linedraw(162, 80, 215, 80, 15, screen);
			} else {
				if (maze[lblock.x][lblock.y]) {
					linedraw(162, 57, 175, 63, 15, screen);
					linedraw(175, 63, 175, 74, 15, screen);
					linedraw(175, 74, 162, 80, 15, screen);
				} else {
					if (maze[lblock.x + increment[direction].x]
						[lblock.y + increment[direction].y]) {
						linedraw(162, 63, 175, 63, 15, screen);
						linedraw(175, 63, 175, 74, 15, screen);
						linedraw(175, 74, 162, 74, 15, screen);
					}
				}
				if (maze[rblock.x][rblock.y]) {
					linedraw(215, 57, 202, 63, 15, screen);
					linedraw(202, 63, 202, 74, 15, screen);
					linedraw(202, 74, 215, 80, 15, screen);
				} else {
					if (maze[rblock.x + increment[direction].x]
						[rblock.y + increment[direction].y]) {
						linedraw(215, 63, 202, 63, 15, screen);
						linedraw(202, 63, 202, 74, 15, screen);
						linedraw(202, 74, 215, 74, 15, screen);
					}
				}
			}
			break;
		case 3: // And again
			if (maze[block.x][block.y]) {
				linedraw(175, 63, 175, 74, 15, screen);
				linedraw(202, 63, 202, 74, 15, screen);
				linedraw(175, 63, 202, 63, 15, screen);
				linedraw(175, 74, 202, 74, 15, screen);
			} else {
				if (maze[lblock.x][lblock.y]) {
					linedraw(175, 63, 182, 66, 15, screen);
					linedraw(182, 66, 182, 70, 15, screen);
					linedraw(182, 70, 175, 74, 15, screen);
				} else {
					if (maze[lblock.x + increment[direction].x]
						[lblock.y + increment[direction].y]) {
						linedraw(175, 66, 182, 66, 15, screen);
						linedraw(182, 66, 182, 70, 15, screen);
						linedraw(182, 70, 175, 70, 15, screen);
					}
				}
				if (maze[rblock.x][rblock.y]) {
					linedraw(202, 63, 195, 66, 15, screen);
					linedraw(195, 66, 195, 70, 15, screen);
					linedraw(195, 70, 202, 74, 15, screen);
				} else {
					if (maze[rblock.x + increment[direction].x]
						[rblock.y + increment[direction].y]) {
						linedraw(202, 66, 195, 66, 15, screen);
						linedraw(195, 66, 195, 70, 15, screen);
						linedraw(195, 70, 202, 70, 15, screen);
					}
				}
			}
			break;
		}

		// If view is obscured by wall, stop drawing:

		if (maze[block.x][block.y]) break;
	}
}

void DEMO_Render(double deltatime)
{
	// Check events
	if (RETRO_KeyPressed(SDL_SCANCODE_UP)) {
		newpos.x = pos.x + increment[direction].x;
		newpos.y = pos.y + increment[direction].y;
		if (!maze[newpos.x][newpos.y]) {
			pos.x = newpos.x;
			pos.y = newpos.y;
		}
	}
	if (RETRO_KeyPressed(SDL_SCANCODE_DOWN)) {
		newpos.x = pos.x - increment[direction].x;
		newpos.y = pos.y - increment[direction].y;
		if (!maze[newpos.x][newpos.y]) {
			pos.x = newpos.x;
			pos.y = newpos.y;
		}
	}
	if (RETRO_KeyPressed(SDL_SCANCODE_RIGHT)) {
		direction++;
		if (direction > 3) {
			direction = 0;
		}
	}
	if (RETRO_KeyPressed(SDL_SCANCODE_LEFT)) {
		--direction;
		if (direction < 0) {
			direction = 3;
		}
	}

	unsigned char *buffer = RETRO_FrameBuffer();

	// Draw the maze in screen buffer:
	drawmaze(buffer);
}

void DEMO_Initialize(void)
{
	RETRO_SetColor(15, 255, 255, 255);
}
