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

#include "Drone.h"
#include "Bullet.h"


Bullet::Bullet()
{
	x = 0;
	y = 0;
	theta = 0;
	create_sprite("Images/Bullet.png", id_bullet);
	state = 0;
	speed = 8;
	duration = 0;
}

void Bullet::set_initial(double _x, double _y, double _theta)
{
	x = _x;
	y = _y;
	theta = _theta;
}

bool& Bullet::get_state()
{
	return state;
}

void Bullet::trajectory()
{
	if (state == 1)
	{
		x = x + speed * cos(theta);
		y = y + speed * sin(theta);

		draw_sprite(id_bullet, x, y, theta, 0.25);

		duration++;

		if (duration > 250)
		{
			state = 0;
			duration = 0;
			x = 0;
			y = 0;
			theta = 0;
		}
	}

}

void Bullet::map_rel(Drone name1)
{
	if (name1.get_x() >= 1050) {		//1050 and 350 to start smooth scrolling before drone hits boundary
		x -= name1.get_x_dot() * 0.65;					//***add depth factor***
	}
	else if (name1.get_x() <= 350) x -= name1.get_x_dot() * 0.65;

	if (name1.get_y() >= 500) {		//1050 and 350 to start smooth scrolling before drone hits boundary
		name1.get_y() = 500;
		y -= name1.get_y_dot() * 0.65;				//***add depth factor***
	}
	else if (name1.get_y() <= 250)
	{
		name1.get_y() = 250;
		y -= name1.get_y_dot() * 0.65;
	}
}