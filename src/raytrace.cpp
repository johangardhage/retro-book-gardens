//
// RAYTRACE.CPP (RTDEMO.CPP)
//
// Simple demonstration of ray tracing (in grey scale)
//
// Written by Christopher Lampton
// for GARDENS OF IMAGINATION (Waite Group Press,1994)
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include <values.h>

const int RECTANGLE = 1;
const int INFINITE_PLANE = 2;
const int SPHERE = 3;
const int NUMBER_OF_OBJECTS = 2;
const int BACKGROUND_COLOR = 6;
const double AMBIENT_INTENSITY = 0.14;
const int VIEWER_X = 160;
const int VIEWER_Y = 120;
const int VIEWER_DISTANCE = 100;

struct vector_type {
	double x, y, z;
};

struct plane_type {
	vector_type surface_normal;
	double distance;
};

struct sphere_type {
	double l, m, n; // Center of sphere
	double r;     // Radius of sphere
};

union object_union {
	plane_type *plane;
	sphere_type *sphere;
};

struct object_type {
	int type_of_object;
	int color;
	object_union obj;
};

const vector_type LIGHT_SOURCE = { 500, -200, 150 };
struct plane_type plane = { { 0, -1, 0 }, 500 };
struct sphere_type sphere = { 0, 40, 300, 100 };
struct sphere_type sphere2 = { -60, 200, 300, 40 };
struct sphere_type sphere3 = { -30, -150, 300, 70 };
struct sphere_type sphere4 = { 0, -30, 200, 150 };

struct object_type objects[NUMBER_OF_OBJECTS] = {
  { SPHERE, 56, (plane_type *)&sphere },
  { INFINITE_PLANE, 48, (plane_type *)&plane }
};

int lightsource(int color, vector_type loc, vector_type norm)
{
	vector_type lightvec;
	lightvec.x = LIGHT_SOURCE.x - loc.x;
	lightvec.y = LIGHT_SOURCE.y - loc.y;
	lightvec.z = LIGHT_SOURCE.z - loc.z;
	long distance = sqrt(lightvec.x * lightvec.x + lightvec.y * lightvec.y + lightvec.z * lightvec.z);
	if (distance == 0) {
		distance = 1;
	}

	double xlight = (double)lightvec.x / distance;
	double ylight = (double)lightvec.y / distance;
	double zlight = (double)lightvec.z / distance;

	double intensity = norm.x * xlight + norm.y * ylight + norm.z * zlight;
	if (intensity < 0.0) {
		intensity = 0;
	}
	intensity += AMBIENT_INTENSITY;
	if (intensity > 1.0) {
		intensity = 1.0;
	}
	double newcolor = color * intensity;

	double integral;
	if (modf(newcolor, &integral) * 10000 > (rand() % 10000)) {
		newcolor = ceil(newcolor);
	} else {
		newcolor = floor(newcolor);
	}
	if (newcolor > 63) {
		newcolor = 63;
	}
	return (int)newcolor;
}

int trace_ray(vector_type rayvec, vector_type raystart)
{
	double t, prev_t = MAXDOUBLE;
	vector_type norm, loc;

	int color = BACKGROUND_COLOR;
	for (int ob = 0; ob < NUMBER_OF_OBJECTS; ob++) {
		if (objects[ob].type_of_object == INFINITE_PLANE) {
			norm = objects[ob].obj.plane->surface_normal;
			double a = norm.x;
			double b = norm.y;
			double c = norm.z;
			double d = objects[ob].obj.plane->distance;
			double X0 = raystart.x;
			double Y0 = raystart.y;
			double Z0 = raystart.z;
			double Xd = rayvec.x;
			double Yd = rayvec.y;
			double Zd = rayvec.z;
			double vd = a * Xd + b * Yd + c * Zd;

			if (vd != 0) {
				double v0 = -(a * X0 + b * Y0 + c * Z0 + d);
				t = v0 / vd;
				if ((t < prev_t) && (t >= 0)) {
					prev_t = t;
					loc.x = X0 + Xd * t;
					loc.y = Y0 + Yd * t;
					loc.z = Z0 + Zd * t;
					color = lightsource(objects[ob].color, loc, norm);
				}
			}
		} else if (objects[ob].type_of_object == SPHERE) {
			double Xc = objects[ob].obj.sphere->l;
			double Yc = objects[ob].obj.sphere->m;
			double Zc = objects[ob].obj.sphere->n;
			double Sr = objects[ob].obj.sphere->r;
			double X0 = raystart.x;
			double Y0 = raystart.y;
			double Z0 = raystart.z;
			double Xd = rayvec.x;
			double Yd = rayvec.y;
			double Zd = rayvec.z;
			double b = 2.0 * (Xd * (X0 - Xc) + Yd * (Y0 - Yc) + Zd * (Z0 - Zc));
			double c = (X0 - Xc) * (X0 - Xc) + (Y0 - Yc) * (Y0 - Yc) + (Z0 - Zc) * (Z0 - Zc) - Sr * Sr;
			double discriminant = b * b - 4 * c;

			if (discriminant >= 0) {
				double sd = sqrt(discriminant);
				double t0 = (-b - sd) / 2;
				double t1 = (-b + sd) / 2;
				if ((t0 > 0) || (t1 > 0)) {
					if (t0 > 0) {
						t = t0;
					}
					if ((t1 < t0) && (t1 >= 0)) {
						t = t1;
					}
					if (t < prev_t) {
						prev_t = t;
						loc.x = X0 + Xd * t;
						loc.y = Y0 + Yd * t;
						loc.z = Z0 + Zd * t;
						norm.x = (loc.x - Xc) / Sr;
						norm.y = (loc.y - Yc) / Sr;
						norm.z = (loc.z - Zc) / Sr;
						color = lightsource(objects[ob].color, loc, norm);
					}
				}
			}
		}
	}
	return color;
}

void DEMO_Render(double deltatime)
{
	vector_type rayvec, raystart;

	for (int yscreen = 0; yscreen < RETRO_HEIGHT; yscreen++) {
		for (int xscreen = 0; xscreen < RETRO_WIDTH; xscreen++) {
			raystart.x = xscreen - VIEWER_X;
			raystart.y = (yscreen - VIEWER_Y) * 1.2;
			raystart.z = VIEWER_DISTANCE;
			double distance = sqrt(raystart.x * raystart.x + raystart.y * raystart.y + raystart.z * raystart.z);
			rayvec.x = raystart.x / distance;
			rayvec.y = raystart.y / distance;
			rayvec.z = raystart.z / distance;
			RETRO_PutPixel(xscreen, yscreen, trace_ray(rayvec, raystart));
		}
	}
}

void DEMO_Initialize(void)
{
	for (int i = 0; i < 64; i++) {
		RETRO_SetColor(i, i * 4, i * 4, i * 4);
	}
}
