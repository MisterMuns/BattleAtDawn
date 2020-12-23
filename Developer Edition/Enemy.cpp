

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
#include "Enemy.h"

void Enemy::reset(double _x, double _y, double _theta)
{
	mass = 5.0;
	x = _x;
	x_dot = 0.0;
	x_dotdot = 0.0;
	y = _y;
	y_dot = 0.0;
	y_dotdot = 0.0;
	theta = _theta;
	in_thrust = 49.25;
	out_thrust = 0.0;
	delta_time = 0.1;
	in_roll = 0.0;
	hp = 100;							//Drone starts at full health (100 health points = 100 pixels)
	aim_angle = 0;
	//acceleration = 9.8;				//Forget about it
	GS[9] = 0;

	index = 1;
	create_sprite("Images/FrontView_E.png", id_drone);
	power_count = 0;
}


void Enemy::inputs(double player_x, double player_y)
{
	dist_x = player_x - x;
	dist_y = player_y - y;
	radius = pow(pow(dist_x, 2) + pow(dist_y, 2), 0.5);
	radius_limit = 400;


	if (radius > radius_limit)
	{
		if (dist_y > 0)
		{
			in_thrust += 5.00;
		}
		else if (dist_y < 0)
		{
			in_thrust -= 5.00;
		}

		if (dist_x < 0)
		{
			in_roll += 0.020;
		}
		if (dist_x > 0)
		{
			in_roll -= 0.020;
		}
	}
	else if (radius < radius_limit)
	{
		in_roll = in_roll * 0.96;
		out_thrust = 0.95 * in_thrust + 49.25;
	}


	out_thrust = in_thrust;

}


void Enemy::stability()
{
	if (in_thrust <= 0) in_thrust = 0;
	if (in_roll > 0.300) in_roll = 0.300;
	if (in_roll < -0.300) in_roll = -0.300;

	theta = in_roll;

	theta = 0.99 * theta;
	x_dot = x_dot / (dist_x + radius_limit);
	out_thrust = 0.95 * in_thrust + 49.25;
	y_dot = y_dot / (dist_y + radius_limit);
}

void Enemy::calculate()
{
	y_dotdot = -((1.0 / mass) * (-out_thrust) + gravity);
	y_dot = y_dot + y_dotdot * delta_time;
	y = y + y_dot;

	x_dotdot = -acceleration * 5 * gravity * sin(theta);
	x_dot = x_dot + x_dotdot * delta_time;
	x = x + x_dot;
}

double Enemy::get_aim(Drone player)
{
	if ((player.get_x() - x) > 0)
	{
		aim_angle = atan((player.get_y() - y) / (player.get_x() - x));
	}
	else if ((player.get_x() - x) < 0)
	{
		aim_angle = 3.14 + atan((player.get_y() - y) / (player.get_x() - x));
	}

	return aim_angle;
}

void Enemy::map_rel(Drone name1)
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

void Enemy::check_health(int& kill_counter)
{
	if (hp <= 0)
	{
		x = 100000;
		y = 100000;
		hp = 100;
		enemy_killcount++;
		kill_counter--;
	}
	//cout << enemy_killcount << endl;
}

void Enemy::reset_killcount()
{
	enemy_killcount = 0;
}

int Enemy::get_killcount() {
	return enemy_killcount;
}
