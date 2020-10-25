
#include <iostream>
#include <cstdio>
#include <cmath>

#include <Windows.h> // also needed for the 2D graphics library

#include "2D_graphics.h" // use the 2D graphics library

#include "timer.h" // use the time / clock reading function

using namespace std;

const double gravity = 9.81;

class Drone
{
private:
	double mass;
	double x, x_dot, x_dotdot;
	double y, y_dot, y_dotdot;
	double theta;
	double in_thrust, out_thrust;
	double in_roll;
	double delta_time;

public:
	
	Drone(double _x, double _y, double _theta);
	void calculate();
	
	//void set_inputs();

	double get_x();
	double get_y();
	double get_theta();
	void set_delta_time();
	void inputs();
	void stability();

};

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

}

double Drone::get_x()
{
	return x;
}

double Drone::get_y()
{
	return y;
}

double Drone::get_theta()
{
	return theta;
}

void Drone::calculate()
{
	y_dotdot = -((1.0 / mass) * (-out_thrust) + gravity);
	y_dot = y_dot + y_dotdot * delta_time;
	y = y + y_dot;

	x_dotdot = -9.81 * gravity * sin(theta);
	x_dot = x_dot + x_dotdot * delta_time;
	x = x + x_dot;
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
		in_thrust += 0.10;
	}
	else if (KEY('S'))
	{
		in_thrust -= 0.10;
	}

	if (in_thrust <= 0) in_thrust = 0;

	if (KEY('A'))
	{
		in_roll += 0.005;
	}
	else if (KEY('D'))
	{
		in_roll -= 0.005;
	}

	out_thrust = in_thrust;
	theta = in_roll;
}

void Drone::stability()
{
	if (theta > 0.0 || theta < 0.0)
	{
		theta = (1.0/1.1)*theta;
		x_dot = (1.0/10.0)*x_dot;
			
	}

	out_thrust = (1.0 / 10.0) * out_thrust + 49.25;
	y_dot = (1.0 / 1.1) * y_dot;

}

int main()
{
	initialize_graphics();
	
	Drone D1(400, 300, 0);
	int id_drone;
	create_sprite("FrontView.png", id_drone);
	
	for (;;)
	{
		clear();

		D1.set_delta_time();
		D1.inputs();
		D1.calculate();
		D1.stability();
		//D1.animate();
		//D1.environment();
		//D1.elements();
		draw_sprite(id_drone, D1.get_x(), D1.get_y(), D1.get_theta(), 0.45);

		update();
	}




	/*
	initialize_graphics();

	for (;;)
	{
		clear();

		draw_sprite(id_drone, 200, 200, 0, 1.0);
		//draw_sprite(id_drone, D1.get_x(), D1.get_y() , D1.get_theta(),  1.0);

		update();
	}

	*/


	/*
	Drone D1(0, 0, 0); //Initialize drone at x=0, y=0, theta=0;
	 
	D1.calculate(input thrust, input pitch); //Input pitch angle, input thrust
					//Inside calculate, maybe include animation based on thrust
	
	
	D1.mapping(Taking x, y information) //Move entire screen for bigger map

	D1.collision(x position, y position, drone area, boxes location array); //If drone touches any other box, game over
	*/

	/*
	initialize_graphics();

	Z Drone(0,0,0);


	double x_move = 0.0;
	double y_move = 0.0;

	int id_cat;
	double theta;


	create_sprite("cat.png", id_cat);


	theta = 0;
	int scale = 1.0;

	double y1 = 0;
	double y2 = 0;
	double y3 = 0;

	clear();

	for (;;)
	{
		clear();


		Drone.calculate();


		draw_sprite(id_cat, 200 + x_move, 200 + Drone.Z_current() , theta, scale);


		update();

		if (KEY('I'))
		{
			Drone.Force = 50.0;

		}
		else
		{
			Drone.Force = 0.0;
		}

		if (KEY('L')) x_move += 5.0; // l key is pressed
		if (KEY('J')) x_move -= 5.0;
		if (KEY('K')) y_move -= 5.0;
		
	}
	*/
	

	cout << "\ndone.\n";
	getchar();

	return 0;
}