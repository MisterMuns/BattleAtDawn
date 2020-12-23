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

Drone::Drone(double _x, double _y, double _theta)
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


	index = 1;
	create_sprite("Images/FrontView.png", id_drone);
	power_count = 0;

}

void Drone::reset(double _x, double _y, double _theta)
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
	create_sprite("Images/FrontView.png", id_drone);
	power_count = 0;
}

void Drone::reset2(double _x, double _y, double _theta)
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
	ifstream fin;
	fin.open("acceleration.txt");
	fin >> acceleration;
	fin.close();
	//acceleration = 9.8;
	GS[9] = 0;

	index = 1;
	power_count = 0;
}

double Drone::get_x()
{
	return x;
}

double& Drone::get_y()
{
	return y;
}

double Drone::get_theta()
{
	return theta;
}

double& Drone::get_hp()
{
	return hp;
}

void Drone::calculate()
{
	out_thrust = in_thrust;
	y_dotdot = -((1.0 / mass) * (-out_thrust) + gravity);
	y_dot = y_dot + y_dotdot * delta_time;
	if (y + y_dot >= 550) {										//changed this so drone no longer moves past y pixels: 200-800
		y = 550;
	}
	else if (y + y_dot <= 200) {
		y = 200;
	}
	else y = y + y_dot;


	x_dotdot = -acceleration * gravity * sin(theta);
	x_dot = x_dot + x_dotdot * delta_time;
	if (x + x_dot >= 1100) {									//changed this so drone no longer moves past x pixels: 300-1100
		x = 1100;
	}
	else if (x + x_dot <= 200) {
		x = 200;
	}
	else x = x + x_dot;

}

void Drone::set_delta_time()
{
	double t1 = high_resolution_time();
	double t2;

	while (delta_time < 0.1)
	{
		t2 = high_resolution_time();
		delta_time = t2 - t1;
	}

}


void Drone::inputs()
{
	if (KEY('W'))
	{
		in_thrust += 0.70;
	}
	else if (KEY('S'))
	{
		in_thrust -= 0.70;
	}

	if (KEY('A'))
	{
		in_roll += 0.005;
	}
	else if (KEY('D'))
	{
		in_roll -= 0.005;
	}

}

void Drone::controller()
{

	gamepad_state(GS, index);

	in_thrust += 0.70 * (GS[LEFT_STICK_Y] / 1.0e6);
	in_roll -= 0.005 * (GS[LEFT_STICK_X] / 1.0e6);

	double x = GS[RIGHT_STICK_X] / 1.0e6;
	double y = GS[RIGHT_STICK_Y] / 1.0e6;

	if (x > 0)
	{
		aim_angle = atan(y / x);
	}
	else if (x < 0)
	{
		aim_angle = 3.14 + atan(y / x);
	}



	/*
	for(int i=0;i<N_GS;i++)
	{
	cout << GS[i] << " ";
	}
	*/
	//cout << "\n"; Muneeb you forgot to comment this out so the command screen is just constantly making spaces LMAO

}

bool Drone::get_gamepad_shoot()
{
	gamepad_state(GS, index);
	return GS[9] / (1.0e6);
}

double Drone::get_aim()
{
	return aim_angle;
}

void Drone::stability()
{
	if (in_thrust <= 0) in_thrust = 0;
	if (in_roll > 0.300) in_roll = 0.300;
	if (in_roll < -0.300) in_roll = -0.300;

	theta = in_roll;

	theta = 0.99 * theta;
	x_dot = 0.95 * x_dot;
	out_thrust = 0.95 * in_thrust + 49.25;
	y_dot = 0.95 * y_dot;


	if (abs(x_dot) < 0.05)
	{
		x_dot = 0.5 * x_dot;
	}

	if (abs(x_dot) < 0.0005)
	{
		x_dot = 0.0001 * x_dot;
	}

	if (abs(y_dot) < 0.05)
	{
		y_dot = 0;

	}

}

void Drone::power()
{
	if (KEY('P'))
	{
		power_time += 1;
	}

	if (power_time > 300)
	{
		power_time = 0;
		power_count++;
	}

	if (power_count % 2 == 0)
	{
		out_thrust = 0;
	}
	else
	{
		out_thrust = in_thrust;
	}
}

void Drone::bounce()
{

	double offset = 30;
	double bounce = pow((x_dot * x_dot + y_dot * y_dot), 0.5);

	if (y_dot > 0)
	{
		y_dot = -bounce;
		y = y - offset;

	}
	else if (y_dot < 0)
	{
		y_dot = bounce;
		y = y + offset;
	}

	if (x_dot > 0)
	{
		x_dot = -bounce;
		x = x - offset;

	}
	else if (x_dot < 0)
	{
		x_dot = bounce;
		x = x + offset;

	}
}

void Drone::draw()
{
	draw_sprite(id_drone, x, y, theta, 0.45);
}

void Drone::get_damage()
{
	if (hp >= 5) {
		hp -= 20;			   //Drone health (100 pixels) - take 20 damage when bounch occurs
	}
	else {
		hp = 0;
	}
}