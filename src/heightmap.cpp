//
// HEIGHTMAP.CPP (BLOKCAST.CPP)
//
// Calls raycasting function to draw view of
// block-aligned height fields.
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
const int VIEWER_DISTANCE = 192;      // Viewer distance from screen
const int VIEWPORT_LEFT = 0;          // Dimensions of viewport
const int VIEWPORT_RIGHT = 319;
const int VIEWPORT_TOP = 0;
const int VIEWPORT_BOT = 199;
const int VIEWPORT_HEIGHT = VIEWPORT_BOT - VIEWPORT_TOP;
const int VIEWPORT_CENTER = VIEWPORT_TOP + VIEWPORT_HEIGHT / 2;
const int GRIDWIDTH = 16;

typedef unsigned char map_type[16][16];

map_type wall = {
	{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 1, 1, 1, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7 },
	{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 }
};

map_type flor = {
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 10, 10, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 },
	{ 2, 10, 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0 },
	{ 2, 10, 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }
};

map_type floorheight = {
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 0, 0, 20, 20, 20, 20, 20, 20, 20, 40, 60, 80, 100, 120, 120 },
	{ 20, 0, 0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 120, 120 },
	{ 20, 0, 0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 120, 120 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 40, 60, 80, 100, 120, 120 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 60, 60, 60, 60, 60, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 60, 100, 100, 100, 60, 20, 20, 20, 20, 40, 20, 20 },
	{ 20, 20, 20, 20, 60, 100, 150, 100, 60, 20, 20, 20, 40, 40, 20, 20 },
	{ 20, 20, 20, 20, 60, 100, 100, 100, 60, 20, 20, 40, 40, 40, 20, 20 },
	{ 20, 20, 20, 20, 60, 60, 60, 60, 60, 20, 20, 20, 20, 20, 20, 20 },
	{ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 }
};

//
// Draws a raycast image in the viewport of the maze represented
// in array MAP[], as seen from position XVIEW, YVIEW by a
// viewer looking at angle VIEWING_ANGLE where angle 0 is due
// north. (Angles are measured in radians.)
//
void draw_maze(map_type wall, map_type floor,
	map_type floorheight, unsigned char *screen,
	int xview, int yview,
	float viewing_angle, int viewer_height,
	unsigned char *textmaps)
{
	// Variable declarations:

	int sy, offset;       // Pixel y position and offset
	float xd, yd;         // Distance to next wall in x and y
	int grid_x, grid_y;   // Coordinates of x and y grid lines
	float xcross_x, xcross_y; // Ray intersection coordinates
	float ycross_x, ycross_y;
	unsigned int xdist, ydist; // Distance to x and y grid lines
	//  int xmaze,ymaze;     // Map location of ray collision
	int distance, realdistance;  // Distance to wall along ray
	int tmcolumn;        // Column in texture map
	float yratio;
	int currentheight; // Height of current floor block
	int newheight, wallheight;
	int heightchange;
	int top, bot, lasttop;
	float x, y;

	// Loop through all columns of pixels in viewport:

	for (int column = VIEWPORT_LEFT; column < VIEWPORT_RIGHT; column++) {

		// Get viewer's initial coordinates:

		int xmaze = xview / 64;
		int ymaze = yview / 64;

		// Get height of floor under viewer:

		currentheight = floorheight[xmaze][ymaze];

		// Calculate height of viewer above base level:

		int vheight = viewer_height + currentheight;

		// Calculate horizontal angle of ray relative to
		//  center ray:

		float column_angle = atan((float)(column - 160)
			/ VIEWER_DISTANCE);

		// Calculate angle of ray relative to maze coordinates

		float radians = viewing_angle + column_angle;

		// Rotate endpoint of ray to viewing angle:

		int x2 = -1024 * (sin(radians));
		int y2 = 1024 * (cos(radians));

		// Translate relative to viewer's position:

		x2 += xview;
		y2 += yview;

		// Initialize ray at viewer's position:

		x = xview;
		y = yview;

		// Find difference in x,y coordinates along ray:

		int xdiff = x2 - xview;
		int ydiff = y2 - yview;

		// Cheat to avoid divide-by-zero error:

		if (xdiff == 0) xdiff = 1;

		// Get slope of ray:

		float slope = (float)ydiff / xdiff;

		// Cheat (again) to avoid divide-by-zero error:

		if (slope == 0.0) slope = .0001;

		lasttop = VIEWPORT_BOT;

		// Cast ray from grid line to grid line:

		while ((xmaze >= 0) && (xmaze < GRIDWIDTH) && (ymaze >= 0)
			&& (ymaze < GRIDWIDTH)) {

			// Repeat casting loop until edge of maze is found:

			while ((xmaze >= 0) && (xmaze < GRIDWIDTH) && (ymaze >= 0)
				&& (ymaze < GRIDWIDTH)) {

				// If ray direction positive in x, get next x grid line:

				if (xdiff > 0) grid_x = ((int)x & 0xffc0) + 64;

				// If ray direction negative in x, get last x grid line:

				else grid_x = ((int)x & 0xffc0) - 1;

				// If ray direction positive in y, get next y grid line:

				if (ydiff > 0) grid_y = ((int)y & 0xffc0) + 64;

				// If ray direction negative in y, get last y grid line:

				else grid_y = ((int)y & 0xffc0) - 1;

				// Get x,y coordinates where ray crosses x grid line:

				xcross_x = grid_x;
				xcross_y = y + slope * (grid_x - x);

				// Get x,y coordinates where ray crosses y grid line:

				ycross_x = x + (grid_y - y) / slope;
				ycross_y = grid_y;

				// Get distance to x grid line:

				xd = xcross_x - x;
				yd = xcross_y - y;
				xdist = sqrt(xd * xd + yd * yd);

				// Get distance to y grid line:

				xd = ycross_x - x;
				yd = ycross_y - y;
				ydist = sqrt(xd * xd + yd * yd);

				// If x grid line is closer...

				if (xdist < ydist) {

					// Calculate maze grid coordinates of square:

					xmaze = xcross_x / 64;
					ymaze = xcross_y / 64;

					// Set x and y to point of ray intersection:

					x = xcross_x;
					y = xcross_y;

					// Find relevant column of texture map:

					tmcolumn = (int)y & 0x3f;

					// Is there a maze cube here? If so, stop looping:

					if (floorheight[xmaze][ymaze] != currentheight) break;
				} else { // If y grid line is closer:

					// Calculate maze grid coordinates of square:

					xmaze = ycross_x / 64;
					ymaze = ycross_y / 64;

					// Set x and y to point of ray intersection:

					x = ycross_x;
					y = ycross_y;

					// Find relevant column of texture map:

					tmcolumn = (int)x & 0x3f;

					// Is there a maze cube here? If so, stop looping:

					if (floorheight[xmaze][ymaze] != currentheight) break;
				}

			} // End of inner casting loop

			// Get distance from viewer to intersection point:

			xd = x - xview;
			yd = y - yview;
			realdistance = (long)sqrt(xd * xd + yd * yd);
			distance = (long)realdistance * cos(column_angle);
			if (distance == 0) distance = 1;

			// Calculate bottom of wall on screen:

			float ratio = (float)VIEWER_DISTANCE / distance;
			int screendepth = ratio * (vheight - currentheight);
			bot = VIEWPORT_CENTER + screendepth;

			if ((xmaze >= 0) && (xmaze < GRIDWIDTH) &&
				(ymaze >= 0) && (ymaze < GRIDWIDTH)) {

				newheight = floorheight[xmaze][ymaze];
				wallheight = newheight - currentheight;

				if (wallheight <= 0) top = bot;
				else {

					// Calculate visible height of wall:

					int visheight = (float)VIEWER_DISTANCE * wallheight / distance;

					// Calculate top of wall on screen:

					top = bot - visheight;

					// Initialize temporary offset into texture map:

					int t = tmcolumn;

					// If top of current vertical line is outside of
					// viewport, clip it:

					int dheight = visheight;
					int iheight = wallheight;
					yratio = (float)wallheight / visheight;
					if (top < VIEWPORT_TOP) {
						dheight -= (VIEWPORT_TOP - top);
						t += (int)((VIEWPORT_TOP - top) * yratio) * 320;
						iheight -= ((VIEWPORT_TOP - top) * yratio);
						top = VIEWPORT_TOP;
					}
					if (bot > lasttop) {
						dheight -= (bot - lasttop);
						iheight -= (bot - lasttop) * yratio;
						bot = lasttop;
					}

					// Point to video memory offset for top of line:

					offset = top * 320 + column;

					// Initialize vertical error term for texture map:

					int tyerror = iheight;

					// Which graphics tile are we using?

					int tile = wall[xmaze][ymaze] - 1;

					// Find offset of tile and column in bitmap:

					unsigned int tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5)
						* IMAGE_WIDTH + t;

					// Loop through the pixels in the current vertical
					// line, advancing OFFSET to the next row of pixels
					// after each pixel is drawn.

					for (int h = 0; h < iheight; h++) {

						// Are we ready to draw a pixel?

						while (tyerror >= iheight) {

							// If so, draw it:

							screen[offset] = textmaps[tileptr];

							// Reset error term:

							tyerror -= iheight;

							// And advance OFFSET to next screen line:

							offset += 320;
						}

						// Incremental division:

						tyerror += dheight;

						// Advance TILEPTR to next line of bitmap:

						tileptr += 320;

					} // End of for() loop

				} // End of if

			} // End of if

			if (bot > VIEWPORT_CENTER) {

				// Step through floor pixels:

				for (int row = bot + 1; row < lasttop; row++) {

					// Get ratio of viewer's height to pixel height:

					float ratio = (float)(vheight - currentheight)
						/ (row - VIEWPORT_CENTER);

					// Get distance to visible pixel:

					distance = ratio * VIEWER_DISTANCE / cos(column_angle);

					// Rotate distance to ray angle:

					int px = -distance * (sin(radians));
					int py = distance * (cos(radians));

					// Translate relative to viewer coordinates:

					px += xview;
					py += yview;

					// Get maze square intersected by ray:

					int xmazes = px / 64;
					int ymazes = py / 64;

					// Find relevant column of texture map:

					int t = ((int)py & 0x3f) * 320 + ((int)px & 0x3f);

					// Which graphics tile are we using?

					int tile = floor[xmazes][ymazes];

					// Find offset of tile and column in bitmap:

					unsigned int tileptr = (tile / 5) * 320 * IMAGE_HEIGHT + (tile % 5)
						* IMAGE_WIDTH + t;

					// Calculate video offset of floor pixel:

					offset = row * 320 + column;

					// Draw pixel:

					screen[offset] = textmaps[tileptr];

				} // End of floor loop

			} // End of if()

			currentheight = newheight;
			if (top < lasttop) lasttop = top;

		}  // End of outer casting while() loop

	} // End of column for() loop
}

void DEMO_Render(double deltatime)
{
	static float viewing_angle = 0;
	static int viewer_height = 32;
	static float xview = 6 * FINE_GRIDSIZE + 48;
	static float yview = 6 * FINE_GRIDSIZE;

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
	draw_maze(wall, flor, floorheight, buffer, xview, yview, viewing_angle, viewer_height, image);
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/walls.pcx", true);
}
