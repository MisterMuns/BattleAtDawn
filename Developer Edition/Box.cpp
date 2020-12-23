#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

#include <Windows.h> // also needed for the 2D graphics library

#include "2D_graphics.h" // use the 2D graphics library


#include "game_pad.h"

#include "timer.h" // use the time / clock reading function

#include <MMSystem.h>

#include "ran.h"

#pragma comment(lib,"winmm.lib") // links a windows library

using namespace std;

const double gravity = 9.81;

#include "Box.h"

Box::Box()
{
	x = 0.0;
	y = 0.0;
	x_length = 0.0;
	y_length = 0.0;

}

Box::Box(int _x, int _y, int _x_length, int _y_length, double _r, double _g, double _b)
{
	for (int i = 0; i < 3; i++)
	{
		R[i] = _r;
		G[i] = _g;
		B[i] = _b;
	}

	x = _x;
	y = _y;
	x_length = _x_length;
	y_length = _y_length;

	x_re[0] = x - x_length / 2;
	y_re[0] = y - y_length / 2;
	x_re[1] = x + x_length / 2;
	y_re[1] = y - y_length / 2;
	x_re[2] = x + x_length / 2;
	y_re[2] = y + y_length / 2;
	x_re[3] = x - x_length / 2;
	y_re[3] = y + y_length / 2;

}

void Box::draw()
{
	double xt[3];
	double yt[3];

	xt[0] = x - x_length / 2;
	yt[0] = y - y_length / 2;
	xt[1] = x + x_length / 2;
	yt[1] = y - y_length / 2;
	xt[2] = x + x_length / 2;
	yt[2] = y + y_length / 2;

	triangle(xt, yt, R, G, B);

	xt[1] = x - x_length / 2;
	yt[1] = y + y_length / 2;

	triangle(xt, yt, R, G, B);
}

void Box::reset(int _x, int _y, int _x_length, int _y_length, double _r, double _g, double _b)
{
	for (int i = 0; i < 3; i++)
	{
		R[i] = _r;
		G[i] = _g;
		B[i] = _b;
	}

	x = _x;
	y = _y;
	x_length = _x_length;
	y_length = _y_length;

	x_re[0] = x - x_length / 2;
	y_re[0] = y - y_length / 2;
	x_re[1] = x + x_length / 2;
	y_re[1] = y - y_length / 2;
	x_re[2] = x + x_length / 2;
	y_re[2] = y + y_length / 2;
	x_re[3] = x - x_length / 2;
	y_re[3] = y + y_length / 2;
}
