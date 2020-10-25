
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

	void set_x_dot();

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

	if (abs(x_dot) < 0.05)
	{
		x_dot = 0.5*x_dot;
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


void Drone::set_x_dot()
{
	x_dot = 0.5 * x_dotdot;
	y_dot = 0.5 * y_dotdot;
	x_dot = -0.5*x_dot;
	y_dot = -0.5*y_dot;

}

class Box
{
private:
	double x, y;
	double x_lenght, y_lenght;
	double R[3], G[3], B[3];
	double x_re[4], y_re[4];


public:
	Box(int _x, int _y, int _x_lenght, int _y_lenght, double _r, double _g, double _b);

	double get_left() { return x_re[0]; }
	double get_right() { return x_re[1]; }
	double get_top() { return y_re[2]; }
	double get_bottom() { return y_re[0]; }
};

Box::Box(int _x, int _y, int _x_lenght, int _y_lenght, double _r, double _g, double _b)
{


	for (int i = 0; i < 3; i++)
	{
		R[i] = _r;
		G[i] = _g;
		B[i] = _b;
	}
	
	x = _x;
	y = _y;
	x_lenght = _x_lenght;
	y_lenght = _y_lenght;

	x_re[0] = x - x_lenght / 2;
	y_re[0] = y - y_lenght / 2;
	x_re[1] = x + x_lenght / 2;
	y_re[1] = y - y_lenght / 2;
	x_re[2] = x + x_lenght / 2;
	y_re[2] = y + y_lenght / 2;
	x_re[3] = x - x_lenght / 2;
	y_re[3] = y + y_lenght / 2;

	double xt[3];
	double yt[3];

	xt[0] = x - x_lenght / 2;
	yt[0] = y - y_lenght / 2;
	xt[1] = x + x_lenght / 2;
	yt[1] = y - y_lenght / 2;
	xt[2] = x + x_lenght / 2;
	yt[2] = y + y_lenght / 2;

	triangle(xt, yt, R, G, B);

	xt[1] = x - x_lenght / 2;
	yt[1] = y + y_lenght / 2;

	triangle(xt, yt, R, G, B);


}


int main()
{
	initialize_graphics();
	
	Drone D1(400, 300, 0);
	

	cout << "\n Meow meow, welcome to the program :)\n";
	//just a random local change

	cout << "\n WE LOVE CATS YESSS :)\n";


	int id_drone;
	create_sprite("FrontView.png", id_drone);
	
	for (;;)
	{
		clear();

		Box s1(200,300,100,100, 0.0, 0.0, 0.0);

		D1.set_delta_time();
		D1.inputs();
		D1.calculate();
		D1.stability();
		D1.power();


		if (D1.get_x() < s1.get_right() && D1.get_x() > s1.get_left() && D1.get_y() < s1.get_top() && D1.get_y() > s1.get_bottom())
		{
			Box s1(200, 300, 90, 90, 1.0, 1.0, 1.0);
			D1.set_x_dot();
		}

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