
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
	int power_count, power_time;

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
	void power();

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
		in_thrust += 0.70;
	}
	else if (KEY('S'))
	{
		in_thrust -= 0.70;
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

	
	theta = in_roll;
}

void Drone::stability()
{
	
	theta = 0.95 * theta;
	x_dot = 0.90 * x_dot;

	out_thrust = 0.95 * out_thrust + 49.25;
	y_dot = 0.95 * y_dot;

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

class square
{
private:
	double x, y;
	double x_lenght, y_lenght;
	double R[3], G[3], B[3];


public:
	square(int _x, int _y, int _x_lenght, int _y_lenght);

};

square::square(int _x, int _y, int _x_lenght, int _y_lenght)
{

	double R[3] = { 0.0, 0.0, 0.0 }; // red colour components
	double G[3] = { 0.0, 0.0, 0.0 }; // green colour components
	double B[3] = { 0.0, 0.0, 0.0 }; // blue colour components

	x = _x;
	y = _y;
	x_lenght = _x_lenght;
	y_lenght = _y_lenght;

	double xt1[3];
	double yt1[3];

	xt1[0] = x - x_lenght / 2;
	yt1[0] = y + x_lenght / 2;
	xt1[1] = xt1[0];
	yt1[1] = y - x_lenght / 2;
	xt1[2] = x + x_lenght / 2;
	yt1[2] = yt1[0];

	triangle(xt1, yt1, R, G, B);
	
	xt1[1] = x + x_lenght / 2;
	yt1[1] = y + y_lenght / 2;

	triangle(xt1, yt1, R, G, B);
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
		square S1(300, 200, 150, 150);

		D1.set_delta_time();
		D1.inputs();
		D1.calculate();
		D1.stability();
		D1.power();
		//D1.animate();
		//D1.environment();
		//D1.elements();
		draw_sprite(id_drone, D1.get_x(), D1.get_y(), D1.get_theta(), 0.45);

		update();
	}
	

	cout << "\ndone.\n";
	getchar();

	return 0;
}