//
// HEIGHTMAP2.CPP (TILECAST.CPP)
//
// Calls raycasting function to draw view of
// height-mapped maze.
//
// Written by Christopher Lampton for
// Gardens of Imagination (Waite Group Press,1994)
//
#include "lib/retro.h"
#include "lib/retromain.h"

const int FINE_GRIDSIZE = 64;
const int NUM_IMAGES = 15;
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
const int GRIDWIDTH = 16;
const int WALLCOLOR = 18;

typedef unsigned char map_type[16][16];

map_type wall = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 28, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 28, 28, 28, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 0, 0 },
	{ 0, 22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 0, 0 },
	{ 0, 17, 17, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 17, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

map_type flor = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 12, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 12, 12, 1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 10, 0, 0 },
	{ 0, 12, 12, 12, 0, 0, 0, 0, 0, 0, 0, 0, 6, 11, 0, 0 },
	{ 0, 12, 12, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 12, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

map_type floorheight = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 10, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 11, 0, 0 },
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

map_type floorbase = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 40, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 40, 40, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 40, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void draw_maze(map_type wall, map_type floor,
	map_type hightile, map_type floorbase,
	unsigned char *screen, int xview, int yview,
	float viewing_angle, int viewer_height,
	unsigned char *textmaps, unsigned char *highmaps)
{
	int y_unit, x_unit; // Variables for amount of change
	//  in x and y
	int distance, real_distance, old_distance, xd, yd, sx, sy;
	unsigned int offset;

	// Loop through all columns of pixels in viewport:

	for (int column = VIEWPORT_LEFT; column < VIEWPORT_RIGHT; column++) {

		// Calculate horizontal angle of ray relative to
		//  center ray:

		float column_angle = atan((float)((column - 160) / ZOOM)
			/ VIEWER_DISTANCE);

		// Fudge column angle:

		if (column_angle == 0.0) column_angle = 0.0001;

		// Calculate angle of ray relative to maze coordinates

		float radians = viewing_angle + column_angle;

		// Which square is the viewer standing on?

		int xmaze = xview / 64;
		int ymaze = yview / 64;

		// Get pointer to floor height map:

		int floortile = hightile[xmaze][ymaze];

		// How high is the floor under the viewer?

		int currentheight = highmaps[(floortile / 5) * 320 * IMAGE_HEIGHT
			+ (floortile % 5) * IMAGE_WIDTH
			+ (yview % IMAGE_HEIGHT) * 320
			+ (xview % IMAGE_WIDTH)];

		// First screen row to draw:

		int row = VIEWPORT_BOT;

		// Cast a ray across the floor:

		for (;;) {

			// Get ratio of viewer's height to pixel height:

			float screen_height = row - VIEWPORT_CENTER;
			if (screen_height == 0.0) screen_height = .00001;
			float ratio = (float)(viewer_height - currentheight) / screen_height;

			// Get distance to pixel:

			real_distance = ratio * VIEWER_DISTANCE;
			if (real_distance > old_distance + 5) distance += 5;
			distance = real_distance / cos(column_angle);

			// Rotate distance to ray angle:

			int x = -distance * (sin(radians));
			int y = distance * (cos(radians));

			// Translate relative to viewer coordinates:

			x += xview;
			y += yview;

			// Get maze square intersected by ray:

			int xmaze = x / 64;
			int ymaze = y / 64;

			// If we've reached the edge of the map, quit:

			if ((xmaze < 0) || (xmaze >= GRIDWIDTH)
				|| (ymaze < 0) || (ymaze >= GRIDWIDTH))
				break;

			// Find relevant column of texture and height maps:

			int t = ((int)y & 0x3f) * 320 + ((int)x & 0x3f);

			// Which height tile are we using?

			int tile = hightile[xmaze][ymaze];

			// Find offset of tile and column in bitmap:

			unsigned int tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5)
				* IMAGE_WIDTH + t;

			// Get height of pixel:

			int newheight = highmaps[tileptr] + floorbase[xmaze][ymaze];

			// Has the floor level gone down?

			if (newheight < currentheight) currentheight = newheight;

			else {

				// Has the floor level gone up?

				if (newheight > currentheight) {

					// If so, set new floor height:

					currentheight = newheight;

					// If so, calculate new screen position:

					ratio = (float)VIEWER_DISTANCE / real_distance;
					int a = ratio * (viewer_height - currentheight);
					int newrow = VIEWPORT_CENTER + a;

					// Draw wall segment:

					offset = row * 320 + column;
					for (int i = row; i > newrow; --i) {
						screen[offset] = wall[xmaze][ymaze];
						offset -= 320;
					}

					// Set screen row to new position:

					row = newrow;
				}

				// If viewer is looking down on floor, draw floor pixel:

				if (row > VIEWPORT_CENTER) {

					// Which graphics tile are we using?

					tile = floor[xmaze][ymaze];

					// Find offset of tile and column in bitmap:

					tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5) * IMAGE_WIDTH + t;

					// Calculate video offset of floor pixel:

					offset = row * 320 + column;

					// Draw pixel:

					screen[offset] = textmaps[tileptr];

					// Advance to next screen line:

					row--;
				}
			}
		}
	}
}

void DEMO_Render(double deltatime)
{
	static float viewing_angle = 0;
	static int viewer_height = 150;
	static float xview = 8 * FINE_GRIDSIZE + 32;
	static float yview = 7 * FINE_GRIDSIZE;

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
	unsigned char *highmap = RETRO_ImageData(1);

	// Draw a raycast view of the maze:
	draw_maze(wall, flor, floorheight, floorbase, buffer, xview, yview, viewing_angle, viewer_height, image, highmap);
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/walls2.pcx", true);
	RETRO_LoadImage("assets/highmaps.pcx");
}
