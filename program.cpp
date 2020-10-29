
#include <iostream>
#include <cstdio>
#include <cmath>

#include <Windows.h> // also needed for the 2D graphics library

#include "2D_graphics.h" // use the 2D graphics library

#include "game_pad.h"

#include "timer.h" // use the time / clock reading function

using namespace std;

const double gravity = 9.81;

class Drone
{
private:
	double mass;
	double hp;							//Drone health (100 pixels) - used as reference between green bar and damage received
	double aim_angle;
	double x, x_dot, x_dotdot;
	double y, y_dot, y_dotdot;
	double theta;
	double in_thrust, out_thrust;
	double in_roll;
	double delta_time;
	int power_count, power_time;
	int index, GS[N_GS];			//Game Pad Controller Variables
	int id_drone;

public:
	
	Drone(double _x, double _y, double _theta);
	void calculate();
	
	//void set_inputs();

	double get_x();
	double get_y();
	double get_theta();
	double& get_hp();					//Used as an access function for hp variable, return reference to get and set variable
	void set_delta_time();
	void inputs();
	void stability();
	void power();
	void controller();
	void bounce();
	double get_aim();
	void draw();

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
	hp = 100;							//Drone starts at full health (100 health points = 100 pixels)
	aim_angle = 0;

	index = 1;
	create_sprite("FrontView.png", id_drone);

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

double& Drone::get_hp()
{
	return hp;
}

void Drone::calculate()
{
	y_dotdot = -((1.0 / mass) * (-out_thrust) + gravity);
	y_dot = y_dot + y_dotdot * delta_time;
	y = y + y_dot;

	x_dotdot = -9.5*gravity * sin(theta);
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
	
	in_thrust += 0.70*(GS[LEFT_STICK_Y] / 1.0e6);
	in_roll   -= 0.005 * (GS[LEFT_STICK_X] / 1.0e6);
	
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
	cout << "\n";
	*/
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
	x_dot =  0.95*x_dot;
	out_thrust = 0.95 * in_thrust + 49.25;
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

	if (hp >= 20) {
		hp -= 20;			   //Drone health (100 pixels) - take 20 damage when bounch occurs
	}
	else {
		hp = 0;
	}
	

}

void Drone::draw()
{
	draw_sprite(id_drone, x, y, theta, 0.45);
}

class Enemy :public Drone
{
public:
	Enemy(double _x, double _y, double _theta) : Drone(0, 0, 0) {}
};

class Box
{
private:
	double x, y;
	double x_length, y_length;
	double R[3], G[3], B[3];
	double x_re[4], y_re[4];


public:
	Box();
	Box(int _x, int _y, int _x_length, int _y_length, double _r, double _g, double _b);
	void draw();

	double get_left() { return x_re[0]; }
	double get_right() { return x_re[1]; }
	double get_top() { return y_re[2]; }
	double get_bottom() { return y_re[0]; }
};

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

class Bullet
{
private:
	double x;
	double y;
	double theta;
	int id_bullet;
	bool state;			//Shot or not shot yet? Affects trajectory
	double speed;		//Bullet Speed

public:
	Bullet();
	void set_initial(double _x, double _y, double _theta);	//Initialize starting position of bullet
	void trajectory();
	bool& get_state();
};

Bullet::Bullet()
{
	x = 0;
	y = 0;
	theta = 0;
	create_sprite("cat.png", id_bullet);
	state = 0;
	speed = 10;
}

void Bullet::set_initial(double _x, double _y, double _theta)
{
	x = _x;
	y = _y;
	theta = _theta;
}

bool &Bullet::get_state()
{
	return state;
}

void Bullet::trajectory()
{	

	x = x + speed*cos(theta);
	y = y + speed*sin(theta);

	draw_sprite(id_bullet, x, y, theta, 1.0);
}

void restore_hp(Drone& name1, Box name2);
void collision(Drone &A, Box Drone, Box Rigid);

int main()
{
	initialize_graphics();

	for (;;)
	{
		bool trigger = 0;


		Drone D1(400, 300, 0);
		Box Rigid[5];
		Bullet bullet[5];

		Enemy D2(0,0,0);



		int id_laser;
		create_sprite("Laser.png", id_laser);

		for (;;)
		{
			clear();

			Box D1_Area(D1.get_x(), D1.get_y(), 120, 40, 1.0, 1.0, 1.0);

			Box HP_zone1(700, 500, 200, 200, 0.0, 0.5, 0.0);					//Box object that will be a healing area
			HP_zone1.draw();

			Box D1_HPb(D1.get_x(), (D1.get_y() + 40), 106, 16, 0.0, 0.0, 0.0);	//Black outline of the HP bar
			D1_HPb.draw();

			Box D1_HPg(D1.get_x() - ((100 - D1.get_hp()) / 2), D1.get_y() + 40, D1.get_hp(), 10, 0.0, 1.0, 0.0);	//This assumes HP is set at 100, not flexible
			D1_HPg.draw();


			for (int i = 0; i < 5; i++)
			{
				Rigid[i] = Box(200 + i*200, 500, 20, 200, 0.0, 0.0, 0.0);
				Rigid[i].draw();
				collision(D1, D1_Area, Rigid[i]);
			}
			
			if (KEY('O'))
			{
			bullet[0].set_initial(D1.get_x(), D1.get_y(), D1.get_aim());
			bullet[0].get_state() = 1;
			}
			if (bullet[0].get_state() == 1)
			{
				bullet[0].trajectory();
			}

			cout << D1.get_aim() << endl;

			D1.set_delta_time();
			D1.inputs();
			D1.calculate();
			D1.stability();
			D1.power();
			restore_hp(D1, HP_zone1);			//Function setting up Box object 'HP_zone1' as a healing area
			D1.draw();

			//D1.controller();

			


			if (D1.get_hp() == 0) {											//Status check, restart drone simulation if HP = 0

				break;
			}

			//D1.animate();
			//D1.environment();
			//D1.elements();

			//D1_Area.draw();

			

			draw_sprite(id_laser, D1.get_x(), D1.get_y(), D1.get_aim() + D1.get_theta(), 1.0);

			update();
		}

		while (trigger != 1) 
		{
			clear();
			double text_x = 255, text_y = 425, text_scale = 1.0;
			Box restart(450, 400, 400, 200, 0.0, 0.0, 0.0);				//Backdrop for text
			restart.draw();

			text("Press R to restart", text_x, text_y, text_scale);		//Text to restart simulation

			if (KEY('R')) 
			{
				trigger = 1;
			}

			update();
		}
		trigger = 0;
	}




	cout << "\ndone.\n";
	getchar();

	return 0;
}

void collision(Drone &A, Box Drone, Box Rigid)
{

	if (Drone.get_left() < Rigid.get_right() && Drone.get_right() > Rigid.get_left() && Drone.get_bottom() < Rigid.get_top() && Drone.get_top() > Rigid.get_bottom())
	{
		A.bounce();
	}
}

void restore_hp(Drone& name1, Box name2) {
	//IMPROVE: Can add extra variable later that is globally set for all health zones, regen less health at harder difficulty
	//IMPROVE: Can add a cap of how much hp can be regenerated for the given Box &name2

	if (name1.get_x() > name2.get_left() && name1.get_x() < name2.get_right() && name1.get_y() < name2.get_top() && name1.get_y() > name2.get_bottom()) {
		//Currently the origin of drone needs to be in the box zone, can change to the hitbox of the drone later if we want
		if (name1.get_hp() < 100) {
			name1.get_hp() += 1;
		}
	}
}