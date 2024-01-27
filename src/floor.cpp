//
// FLOOR.CPP (FLOORCST.CPP)
//
// Calls raycasting function to draw texture-mapped
// view of maze floor.
//
// Written by Christopher Lampton for
// Gardens of Imagination (Waite Group Press,1994)
//
#include "lib/retro.h"
#include "lib/retromain.h"

const int FINE_GRIDSIZE = 64;
const int IMAGE_WIDTH = 64;
const int IMAGE_HEIGHT = 64;
const int WALL_HEIGHT = 64;           // Height of wall in pixels
const int VIEWER_DISTANCE = 128;      // Viewer distance from screen
const int VIEWPORT_LEFT = 0;          // Dimensions of viewport
const int VIEWPORT_RIGHT = 319;
const int VIEWPORT_TOP = 0;
const int VIEWPORT_BOT = 199;
const int VIEWPORT_HEIGHT = VIEWPORT_BOT - VIEWPORT_TOP;
const int VIEWPORT_CENTER = VIEWPORT_TOP + VIEWPORT_HEIGHT / 2;
const int ZOOM = 2;

typedef int map_type[16][16];

const int NUM_IMAGES = 15;

map_type flor = {
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 }
};

map_type ceiling = {
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 }
};

void draw_maze(map_type floor, map_type ceiling, unsigned char *screen,
	int xview, int yview,
	float viewing_angle, int viewer_height,
	unsigned char *textmaps)
{
	int y_unit, x_unit; // Variables for amount of change
	//  in x and y
	int distance, old_distance, xd, yd, sx, sy, offset;

	// Loop through all columns of pixels in viewport:

	for (int column = VIEWPORT_LEFT; column < VIEWPORT_RIGHT; column++) {

		// Calculate horizontal angle of ray relative to
		//  center ray:

		float column_angle = atan((float)((column - 160) / ZOOM)
			/ VIEWER_DISTANCE);

		// Calculate angle of ray relative to maze coordinates

		float radians = viewing_angle + column_angle;

		// Step through floor and ceiling pixels:

		for (int row = VIEWPORT_BOT; row > VIEWPORT_CENTER + 5; --row) {

			// Get ratio of viewer's height to pixel height:

			float ratio = (float)viewer_height / (row - VIEWPORT_CENTER);

			// Get distance to visible pixel:

			distance = ratio * VIEWER_DISTANCE / cos(column_angle);

			// Rotate distance to ray angle:

			int x = -distance * (sin(radians));
			int y = distance * (cos(radians));

			// Translate relative to viewer coordinates:

			x += xview;
			y += yview;

			// Get maze square intersected by ray:

			int xmaze = x / 64;
			int ymaze = y / 64;

			// Find relevant column of texture map:

			int t = ((int)y & 0x3f) * 320 + ((int)x & 0x3f);

			// Which graphics tile are we using?

			int tile = floor[xmaze][ymaze];

			// Find offset of tile and column in bitmap:

			unsigned int tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5)
				* IMAGE_WIDTH + t;

			// Calculate video offset of floor pixel:

			offset = row * 320 + column;

			// Draw pixel:

			screen[offset] = textmaps[tileptr];
		}

		// Step through ceiling pixels:

		for (int row = VIEWPORT_TOP; row < VIEWPORT_CENTER - 5; row++) {

			// Get ratio of viewer's height to pixel height:

			float ratio = (float)(WALL_HEIGHT - viewer_height)
				/ (VIEWPORT_CENTER - row);

			// Get distance to visible pixel:

			distance = ratio * VIEWER_DISTANCE / cos(column_angle);

			// Rotate distance to ray angle:

			int x = -distance * (sin(radians));
			int y = distance * (cos(radians));

			// Translate relative to viewer coordinates:

			x += xview;
			y += yview;

			// Get maze square intersected by ray:

			int xmaze = x / 64;
			int ymaze = y / 64;

			// Find relevant column of texture map:

			int t = ((int)y & 0x3f) * 320 + ((int)x & 0x3f);

			// Which graphics tile are we using?

			int tile = ceiling[xmaze][ymaze];

			// Find offset of tile and column in bitmap:

			unsigned int tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5)
				* IMAGE_WIDTH + t;

			// Calculate video offset of floor pixel:

			offset = row * 320 + column;

			// Draw pixel:

			screen[offset] = textmaps[tileptr];
		}
	}
}

void DEMO_Render(double deltatime)
{
	static float viewing_angle = 0;
	static int viewer_height = 32;
	static float xview = 8 * FINE_GRIDSIZE;
	static float yview = 8 * FINE_GRIDSIZE;

	// Calculate speed
	float walkspeed = deltatime * 200;
	float turnspeed = deltatime * 2;

	// Check events
	if (RETRO_KeyState(SDL_SCANCODE_UP)) {
		xview -= sin(viewing_angle) * walkspeed;
		yview += cos(viewing_angle) * walkspeed;
	}
	if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
		xview += sin(viewing_angle) * walkspeed;
		yview -= cos(viewing_angle) * walkspeed;
	}
	if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		viewing_angle += turnspeed;
	}
	if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
		viewing_angle -= turnspeed;
	}

	unsigned char *buffer = RETRO_FrameBuffer();
	unsigned char *image = RETRO_ImageData();

	// Draw a raycast view of the maze:
	draw_maze(flor, ceiling, buffer, xview, yview, viewing_angle, viewer_height, image);
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/walls.pcx", true);
}
