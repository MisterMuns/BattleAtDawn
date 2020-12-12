
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


class Drone
{
protected:
	double mass;
	double hp;							//Drone health (100 pixels) - used as reference between green bar and damage received
	double aim_angle;
	double x, x_dot, x_dotdot;
	double y, y_dot, y_dotdot;
	double theta;
	double in_thrust, out_thrust;
	double acceleration;
	double in_roll;
	double delta_time;
	int power_count, power_time;
	int index, GS[N_GS];			//Game Pad Controller Variables
	int id_drone;

public:
	Drone() { ; }
	Drone(double _x, double _y, double _theta);
	void calculate();
	void reset(double _x, double _y, double _theta);
	void reset2(double _x, double _y, double _theta);
	double get_x();
	double get_x_dot() { return x_dot; }
	double& get_y();
	double get_y_dot() { return y_dot; }
	double get_theta();
	double& get_hp();					//Used as an access function for hp variable, return reference to get and set variable
	void set_delta_time();
	void inputs();
	void stability();
	void power();
	void controller();
	void bounce();
	double get_aim();
	bool get_gamepad_shoot();
	void draw();
	void get_damage();
	~Drone() { ; }
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


	x_dotdot =  -acceleration * gravity * sin(theta);
	x_dot = x_dot + x_dotdot * delta_time;
	if (x + x_dot >= 1100 ) {									//changed this so drone no longer moves past x pixels: 300-1100
		x = 1100;
	} else if (x + x_dot <= 200) {
		x = 200;
	} else x = x + x_dot;

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
	*/
	//cout << "\n"; Muneeb you forgot to comment this out so the command screen is just constantly making spaces LMAO
	
}

bool Drone::get_gamepad_shoot()
{
	gamepad_state(GS, index);
	return GS[9]/(1.0e6);
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

class Enemy :public Drone
{
private:
	double dist_x;
	double dist_y;
	double radius, radius_limit;
	static int enemy_killcount;
	
public:
	Enemy() { ; }
	Enemy(double _x, double _y, double _theta) : Drone(_x, _y, _theta) {}
	void inputs(double player_x, double player_y);
	void reset(double _x, double _y, double _theta);
	void stability();
	void calculate();
	void map_rel(Drone name1);
	double get_aim(Drone player);
	double get_radius() { return radius; }
	void check_health(int &kill_counter);
	void reset_killcount();
	int get_killcount();
	~Enemy() { ; }

};

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
	radius = pow(pow(dist_x, 2) + pow(dist_y, 2),0.5);
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
	} else if (radius < radius_limit)
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
	x_dot = x_dot/(dist_x + radius_limit);
	out_thrust = 0.95 * in_thrust + 49.25;
	y_dot = y_dot /(dist_y + radius_limit);
}

void Enemy::calculate()
{
	y_dotdot = -((1.0 / mass) * (-out_thrust) + gravity);
	y_dot = y_dot + y_dotdot * delta_time;
	y = y + y_dot;

	x_dotdot = -acceleration*5 * gravity * sin(theta);
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

void Enemy::check_health(int &kill_counter)
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
	void reset(int _x, int _y, int _x_length, int _y_length, double _r, double _g, double _b);
	double get_left() { return x_re[0]; }
	double get_right() { return x_re[1]; }
	double get_top() { return y_re[2]; }
	double get_bottom() { return y_re[0]; }
	~Box() { ; }
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

class Bullet
{
private:
	double x;
	double y;
	double theta;
	int id_bullet;
	bool state;			//Shot or not shot yet? Affects trajectory
	double speed;		//Bullet Speed
	int duration;


public:
	Bullet();
	bool& get_state();
	void set_initial(double _x, double _y, double _theta);	//Initialize starting position of bullet
	void trajectory();
	double get_x() { return x; }
	double get_y() { return y; }
	void map_rel(Drone name1);
	~Bullet() { ; }
};

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

bool &Bullet::get_state()
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

//map class using Parallax technique.
class map { //This class creates the layer objects that the environment is composed of

	int id_layer;		//id of the layer object that is set when constructor is called
	double layer_x;	//layer object's x position
	double layer_y;	//layer object's y position
	double last_x; //start position of layer, used for draw_coin
	double last_y; //start position of layer, used for draw_coin
	double layer_scale;
	double df;			//depth factor, the depth of the layer will influence scroll rate
	double max_pos_x;		//furthest layer will displace right as drone moves LEFT
	double max_neg_x;		//furthest layer will displace left as drone moves RIGHT
	double max_pos_y;		//furthest layer will displace up as drone moves DOWN
	double max_neg_y;		//furthest layer will displace down as drone moves UP

public:
	map(char layer_file_name[], double depth_factor, double layerX, double layerY, double scale, double _max_pos_x, double _max_neg_x, double _max_pos_y, double _max_neg_y);		//constructor to initialize the layer object, called outside of the infinite draw loop
	void reset(double layerX, double layerY);
	void draw_layer(Drone& name1);			//this will contain "draw_sprite()" and manipulate x and y of layer to track drone object properly.Called inside infinite draw loop
	double get_layerX();
	double get_layerY();
	double get_lastX();
	double get_lastY();
	double get_max_pos_x();
	double get_max_pos_y();
	double get_max_neg_x();
	double get_max_neg_y();
	~map() { ; }
};

map::map(char layer_file_name[], double depth_factor, double layerX, double layerY, double scale, double _max_pos_x, double _max_neg_x, double _max_pos_y, double _max_neg_y) {		//takes the file name for this object, to create sprite
	layer_x = layerX;
	layer_y = layerY;
	last_x = layerX;
	last_y = layerY;
	layer_scale = scale;
	df = depth_factor;
	max_pos_x = _max_pos_x;
	max_neg_x = _max_neg_x;
	max_pos_y = _max_pos_y;
	max_neg_y = _max_neg_y;

	create_sprite(layer_file_name, id_layer);
}

void map::reset(double layerX, double layerY) {
	layer_x = layerX;
	layer_y = layerY;
	last_x = layerX;
	last_y = layerY;
}

void map::draw_layer(Drone& name1) {
	//need to adjust x and y of sprite based on drone object, MAKE SURE EQUATIONS ARE GOOD!

	if (name1.get_x() >= 1050) {		//1050 and 250 to start smooth scrolling before drone hits boundary
		if (layer_x > max_neg_x) {
			last_x = layer_x;
			layer_x -= name1.get_x_dot() * df;					
		} 
		else if (layer_x <= max_neg_x) {
			cout << "\n\n\t YOU CANNOT GO ANY FURTHER";
		}
	}
	else if (name1.get_x() <= 250) {
		if (layer_x < max_pos_x) {
			last_x = layer_x;
			layer_x -= name1.get_x_dot() * df;
		} 
		else if (layer_x >= max_pos_x) {
			cout << "\n\n\t YOU CANNOT GO ANY FURTHER";
		}
	}

	if (name1.get_y() >= 500) {		//500 and 250 to start smooth scrolling before drone hits boundary
		//name1.get_y() = 500;
		if (layer_y > max_neg_y) {
			last_y = layer_y;
			layer_y -= name1.get_y_dot() * df;				
		} 
		else if (layer_y <= max_neg_y) {
			cout << "\n\n\t YOU CANNOT GO ANY FURTHER";;
		}
	}
	else if (name1.get_y() <= 250) {
		//name1.get_y() = 250;
		if (layer_y < max_pos_y) {
			last_y = layer_y;
			layer_y -= name1.get_y_dot() * df;
		} 
		else if (layer_y >= max_pos_y) {
			cout << "\n\n\t YOU CANNOT GO ANY FURTHER";
		}
	}
	draw_sprite(id_layer, layer_x, layer_y, 0, layer_scale);
}

double map::get_layerX() {
	return layer_x;
}

double map::get_layerY() {
	return layer_y;
}

double map::get_lastX() {
	return last_x;
}

double map::get_lastY() {
	return last_y;
}

double map::get_max_pos_x() {
	return max_pos_x;
}

double map::get_max_neg_x() {
	return max_neg_x;
}
double map::get_max_pos_y() {
	return max_pos_y;
}
double map::get_max_neg_y() {
	return max_neg_y;
}

class Animation
{
private:
	int sequence[20];
	int size;
	int index;
	double duration;
	int delay;
	int x;
	int y;
	double scale;
	bool trigger_state;
public:
	Animation(char file_seq[], double _duration, double _scale);
	void trigger(double _x, double _y);
	void animate();
	bool& return_trig() { return trigger_state; }
	~Animation() { ; }
};

Animation::Animation(char file_seq[], double _duration, double _scale)
{
	index = 0;
	delay = 0;
	duration = _duration;
	x = 0;
	y = 0;
	trigger_state = 0;
	scale = _scale;

	ifstream fin;
	fin.open(file_seq);

	if (!fin) {
		cout << "\nError opening explosions locations file in animatation function";
		exit(1);
	}

	fin >> size;

	char temp[100];
	
	for (int i = 0; i < size; i++)
	{
		fin >> temp;
		create_sprite(temp, sequence[i]);
	}
	fin.close();

}

void Animation::trigger(double _x, double _y)
{
	trigger_state = 1;
	x = _x;
	y = _y;
}

void Animation::animate()
{

	if (trigger_state == 1)
	{
		draw_sprite(sequence[index], x, y, 0, scale);

		delay++;

		if (delay > duration)
		{
			index++;
			delay = 0;
		}

		if (index == size)
		{
			index = 0;
			trigger_state = 0;
		}
	}
}

/*
Declare array of coin objects. Map each object location x and y position from a map_locations function, updates with moving map. Draw coins (if state = 1)
the to the screen above the layers but below the drone After: they can be grabbed with grab_coin function and state = 0 to disappear... so only draw if state = 1
reset_coin function to return coin object to state 1. 
*/

class Coin {
	double coin_x;
	double coin_y;			
	bool coin_state;					//when state == 0, coin disappears, coin returns to state == 1 after time passes
	static int collected_coins;			//static variable shared among all objects, they will all increment this value when they are grabbed! (grab_coin function being called)

	/*  No longer apart of the coin class:
	int id_coin1;
	int id_coin2;
	int id_coin3;
	int id_coin4;
	double coin_rotating;
	*/

public:
	Coin();										//constructor for each coin object, all should spawn ie: state == 1
	double& get_x() { return coin_x; };			//access function necessary when mapping coins from a text file
	double& get_y() { return coin_y; };			//access function necessary when mapping coins from a text file
	bool& get_state() { return coin_state; };
	int& get_collected_coins() { return collected_coins; };
	void Coin::draw_coin(Drone& name1, map& layer, Animation& coin_animations);	 //this is called to draw each coin object, using layer 4 relative displacement and coin animation
	~Coin() { ; }
};

Coin::Coin() {
	coin_x = 0.0;
	coin_y = 0.0;
	coin_state = 1;

	/*   No longer apart of the coin class:
	create_sprite("pirate_coin1.png", id_coin1);
	create_sprite("pirate_coin2.png", id_coin2);
	create_sprite("pirate_coin3.png", id_coin3);
	create_sprite("pirate_coin4.png", id_coin4);
	coin_rotating = 2.0;
	*/
}

void Coin::draw_coin(Drone& name1, map& layer, Animation& coin_animations) {

	/* 
		OUTDATED: Coins will now use the layer class to track their position

	if (name1.get_x() >= 1050) {		//1050 and 350 to start smooth scrolling before drone hits boundary
		coin_x -= name1.get_x_dot() * 0.65;					//***add depth factor***
	}
	else if (name1.get_x() <= 350) coin_x -= name1.get_x_dot() * 0.65;

	if (name1.get_y() >= 500) {		//1050 and 350 to start smooth scrolling before drone hits boundary
		name1.get_y() = 500;
		coin_y -= name1.get_y_dot() * 0.65;				//***add depth factor***
	}
	else if (name1.get_y() <= 250)
	{
		name1.get_y() = 250;
		coin_y -= name1.get_y_dot() * 0.65;
	}
	*/

	//IF LAYER4 MOVES, COIN OBJECTS MOVE A THE SAME PIXEL RATE ACCORDINGLY
	if (name1.get_x() >= 1050) {		
		if (layer.get_layerX() > layer.get_max_neg_x()) {
			coin_x += (layer.get_layerX() - layer.get_lastX());
		}
	}
	else if (name1.get_x() <= 250) {
		if (layer.get_layerX() < layer.get_max_pos_x()) {
			coin_x += (layer.get_layerX() - layer.get_lastX());
		}
	}

	if (name1.get_y() >= 500) {		
		if (layer.get_layerY() > layer.get_max_neg_y()) {
			coin_y += (layer.get_layerY() - layer.get_lastY());
		}
	}
	else if (name1.get_y() <= 250)
	{
		if (layer.get_layerY() < layer.get_max_pos_y()) {
			coin_y += (layer.get_layerY() - layer.get_lastY());
		}
	}

	// Drawing coin object 
	if (coin_state == 1) {                                //trigger animation located at the coin
		coin_animations.trigger(coin_x, coin_y);            //Each coin animation should occur where the respective coin object resides
		coin_animations.animate();
	}

	/*   
		OUTDATED: coins will now use Animation class to animate the coin
	
	if (coin_rotating >= 2.0 && coin_rotating < 3.0 && coin_state == 1) {
		draw_sprite(id_coin1, coin_x, coin_y, 0, 1);
		coin_rotating += 0.05;
	}
	else if (coin_rotating >= 3.0 && coin_rotating < 4.0 && coin_state == 1) {
		draw_sprite(id_coin2, coin_x, coin_y, 0, 1);
		coin_rotating += 0.05;
	}
	else if (coin_rotating >= 4.0 && coin_rotating < 5.0 && coin_state == 1) {
		draw_sprite(id_coin3, coin_x, coin_y, 0, 1);
		coin_rotating += 0.05;
	}
	else if (coin_rotating >= 5.0 && coin_rotating < 6.0 && coin_state == 1) {
		draw_sprite(id_coin4, coin_x, coin_y, 0, 1);
		coin_rotating += 0.05;
	}
	else if (coin_rotating >= 6.0) {
		coin_rotating = 2.0;
	}
	*/

}


class Sound
{
private:
	int n;
	char* p_buffer, * p_buffer_end;
	short int* p_data;

public:
	Sound(char file_name[]);
	void play();
	~Sound() { ; }
};

Sound::Sound(char file_name[])
{
	ifstream fin;

	fin.open(file_name, ios::binary); // open binary file

	if (!fin) {
		cout << "\nfile open error";
		exit(1);
	}

	fin.seekg(0, ios::end); // move to the end of the file

	n = fin.tellg(); // get the current position -> size of file

	fin.seekg(0, ios::beg); // move to the beginning of the file (rewind)

	// need to dynamically allocate an array of size n for p_buffer
	p_buffer = new char[n];

	if (p_buffer == NULL) {
		cout << "\ndynamic memory allocation error";
		exit(1);
	}

	fin.read(p_buffer, n);

	fin.close();
}

void Sound::play()
{
	PlaySoundA(p_buffer, NULL, SND_MEMORY | SND_ASYNC);
}

void map_coins(char coin_locations[], Coin coin_array[], int nb_coins);
void grab_coin(Box& Drone_Area, Coin coin_array[], int nb_coins);
void reset_state(Coin coin_array[], int nb_coins);

//when coins get picked up, they disappear momentarily. a seperate function called "collecting coins", state goes to 0 when its touched?


//coin_grabbed function to change state of coin


void restore_hp(Drone& name1, Box name2);
void collision(Drone &A, Box Drone, Box Rigid, Animation &_animation, Sound _sound);
void Health_Bar(Enemy enemy, Box black, Box green);
void getting_shot(Drone &Enemy_Drone, Box Enemy_Area, Bullet &bullet, Animation &explosion, Sound sound);
void scoreboard(char scoreboard_file[], char _player_name[], Enemy enemy_array[], Coin coin_array[], char _first[], char _second[], char _third[], double& _firstpnts,
double& _secondpnts, double& _thirdpnts, bool& scoreboard_trigger);
void Spawn(Enemy E_Array[], int &wave, long int rand_s, int &kill_counter);

const int nb_enemy = 10;
const int nb_coins = 10;
const int enemy_bullet_limit = 3;
const int player_bullet_limit = 3;
int Enemy::enemy_killcount = 0;
int Coin::collected_coins = 0;				//Can't initialize static member variables in the constructor

int main()
{
	initialize_graphics();

	char player_name[20];										//Player name array used for scoreboard and end screen cout
	cout << "\n\n\n\tPlease input a name, no spaces or special characters, and press enter!\n\n\n\t\t Name: ";
	cin >> player_name;											//Need to add error check to ensure appropriate name was chosen?
	
	//Char arrays and points to store scoreboard names/points
	char firstplace[20], secondplace[20], thirdplace[20];
	double firstpnts, secondpnts, thirdpnts;

	Drone D1(0, 0, 0);
	Box D1_Area(0, 0, 120, 40, 1.0, 1.0, 1.0);

	//Initializing of Layers
	map Layer1("Images/Layer1.png", 0.5, 700.0, 450, 3.6, 2100, -645, 1416, -115);		//Creating Layer1 object of the map class, (char layer pic, depth factor, double layerX, double layerY, double scale, double _layer_offset, double _max_pos_x, double _max_neg_x
	map Layer4("Images/Layer5.png", 0.6, 700.0, 100, 1.75, 2400, -900, 1199, -645);	//arguments: (image file, its speed relative to other layers (df), x and y position in window relative to other layers, image scaling)

	Box Rigid[2];	//Sky and Grown collision box
	Box HP_zone1(0, 0, 200, 200, 0.0, 0.5, 0.0);
	Bullet bullet[10];
	Bullet bullet_enemy[enemy_bullet_limit];
	Coin coins[nb_coins];						//coin class is added

	Box D1_HPb(D1.get_x(), (D1.get_y() + 40), 106, 16, 0.0, 0.0, 0.0);	//Black outline of the HP bar
	Box D1_HPg(D1.get_x() - ((100 - D1.get_hp()) / 2), D1.get_y() + 40, D1.get_hp(), 10, 0.0, 1.0, 0.0);

	Box D2_HPg[nb_enemy];
	Box D2_HPb[nb_enemy];

	Enemy E_Array[nb_enemy];
	Box E_Area[nb_enemy];

	Box restart(0, 0, 400, 200, 0.0, 0.0, 0.0);

	Animation explosion("Animation/Explosion/animation_sequence.txt", 1, 1);
	Animation collision_animation("Animation/Collision/animation_sequence.txt", 1, 1);
	Animation coin_animation("Animation/Coins/animation_sequence.txt", 100, 0.2);				//coin png scaled down to 0.2

	Sound boom("Sounds/boom.wav");
	Sound laser("Sounds/laser.wav");
	Sound collision_sound("Sounds/collision.wav");

	long int rand_s = 22;

	D1.reset(400, 300, 0);
	for (int i = 0; i < nb_enemy; i++)
	{
		E_Array[i].reset(100000000000000 + 50 * i, 300 + 60 * i, 0);
	}

	int id_laser;
	create_sprite("Images/Laser.png", id_laser);

	bool scoreboard_trigger;						//After restart, reset trigger to 1 so scoreboard will update for next game loop
	int shoot_delay;
	int shoot_delay_enemy[nb_enemy];
	int wave;
	int kill_counter;

	int i_enemy = 0;

		for (;;)		//***_RESTART LOOP_***
		{
			scoreboard_trigger = 1;
			wave = 1;
			kill_counter = 1;

			E_Array[0].reset_killcount();

			D1.reset2(400, 300, 0);
			for (int i = 0; i < nb_enemy; i++)
			{
				E_Array[i].reset2(10000000000 + 50 * i, 300 + 60 * i, 0);
			}

			E_Array[0].reset2(900, 300, 0);

			for (int i = 0; i < player_bullet_limit; i++)
			{ 
				bullet[i].get_state() = 0;
				shoot_delay = 0;
			}

			for (int i = 0; i < enemy_bullet_limit; i++)
			{
				bullet_enemy[i].get_state() = 0;
			}
			for (int i = 0; i < nb_enemy; i++)
			{
				shoot_delay_enemy[i] = 0;
			}

			//Initialization of coins on layers
			map_coins("coin_locations.txt", coins, nb_coins);
			//Reset state of grabbed coins to 1, allowing them to respawn for next game loop and to decrement the collected_coins variable of the objects
			reset_state(coins, nb_coins);

			//Added map class reset function to replace constructor from loop
			Layer1.reset(700.0, 500);
			Layer4.reset(700.0, 100);

		for (;;)		//***_GAME LOOP_***
		{
			
			clear();
			//Drawing of Layers
			Layer1.draw_layer(D1);				//Updating background according to D1 positioning, absolute background so goes first
			Layer4.draw_layer(D1);
			
			Spawn(E_Array, wave, rand_s, kill_counter);

			//Drawing of coins
			for (int i = 0; i < nb_coins; i++) {
				coins[i].draw_coin(D1, Layer4, coin_animation);			//Added Animation class to animate coin
			}

			D1_Area.reset(D1.get_x(), D1.get_y(), 120, 40, 1.0, 1.0, 1.0);
			
			for (int i = 0; i < nb_enemy; i++)
			{
				E_Area[i].reset(E_Array[i].get_x(), E_Array[i].get_y(), 150, 100, 1.0, 1.0, 1.0);
			}
			

			HP_zone1.reset(Layer4.get_layerX(), Layer4.get_layerY(), 200, 200, 0.0, 0.5, 0.0);					//Box object that will be a healing area
			HP_zone1.draw();

			for (int i = 0; i < nb_enemy; i++)
			{
				E_Array[i].map_rel(D1);
			}
			

			D1_HPb.reset(D1.get_x(), (D1.get_y() + 40), 106, 16, 0.0, 0.0, 0.0);	//Black outline of the HP bar
			D1_HPb.draw();

			D1_HPg.reset(D1.get_x() - ((100 - D1.get_hp()) / 2), D1.get_y() + 40, D1.get_hp(), 10, 0.0, 1.0, 0.0);	//This assumes HP is set at 100, not flexible
			D1_HPg.draw();

			//Health_Bar(D1, D2_HPb, D2_HPg);

			for (int i = 0; i < nb_enemy; i++)
			{
				Health_Bar(E_Array[i], D2_HPb[i], D2_HPg[i]);
			}
			
			

			Rigid[0].reset((Layer4.get_layerX() - 0), Layer4.get_layerY() + 1100 , 5000, 60, 0.0, 0.0, 0.0);  //-500 in x position relative to Layer4
			Rigid[1].reset((Layer4.get_layerX() - 0), Layer4.get_layerY() - 1000, 5000, 60, 0.0, 0.0, 0.0);  //-500 in x position relative to Layer4

			for (int i = 0; i < 2; i++)
			{
				//Rigid[i].draw();
				collision(D1, D1_Area, Rigid[i], collision_animation, collision_sound);
			}

			for (int i = 0; i < player_bullet_limit; i++)
			{

				if (bullet[i].get_state() == 1)
				{
					bullet[i].trajectory();
					continue;
				}

				if (shoot_delay > 10 && bullet[i].get_state() == 0)
				{
					bullet[i].set_initial(D1.get_x(), D1.get_y(), D1.get_aim());
					bullet[i].get_state() = 1;
					shoot_delay = 0;
					laser.play();
				}
			}

			if (wave != 1)
			{
				for (int j = 0; j < enemy_bullet_limit; j++)
				{

					if (bullet_enemy[j].get_state() == 1)
					{
						bullet_enemy[j].trajectory();
						continue;
					}

					if (shoot_delay_enemy[i_enemy] > 40 && bullet_enemy[j].get_state() == 0)
					{
						bullet_enemy[j].set_initial(E_Array[i_enemy].get_x(), E_Array[i_enemy].get_y(), E_Array[i_enemy].get_aim(D1));
						bullet_enemy[j].get_state() = 1;
						shoot_delay_enemy[i_enemy] = 0;
						laser.play();
					}
				}
			}
				
				i_enemy++;
				if (i_enemy == wave) i_enemy = 0;
				

			
			for (int i = 0; i < nb_enemy; i++)
			{
				for (int i = 0; i < player_bullet_limit; i++)
				{
					getting_shot(E_Array[i], E_Area[i], bullet[i], explosion, boom);
				}
			}

			
			for (int i = 0; i < enemy_bullet_limit; i++)
			{
				getting_shot(D1, D1_Area, bullet_enemy[i], explosion, boom);
			}
			
			explosion.animate();
			collision_animation.animate();
			
			
			for (int i = 0; i < nb_enemy; i++)
			{
				if (E_Array[i].get_radius() < 500)
				{
					shoot_delay_enemy[i]++;
				}
			}
			


			for (int i = 0; i < player_bullet_limit; i++)
			{
				bullet[i].map_rel(D1);
			}
			for (int i = 0; i < enemy_bullet_limit; i++)
			{
				bullet_enemy[i].map_rel(D1);
			}
			


			if (KEY('O'))
			{
				shoot_delay++;
			}
			
			

			D1.set_delta_time();
			D1.inputs();
			D1.calculate();
			D1.stability();
			D1.power();
			restore_hp(D1, HP_zone1);			//Function setting up Box object 'HP_zone1' as a healing area
			D1.draw();
			draw_sprite(id_laser, D1.get_x(), D1.get_y(), D1.get_aim() + D1.get_theta(), 1.0);
			//cout << D1.get_x() << "      " << D1.get_y() << endl;
			grab_coin(D1_Area, coins, nb_coins);

			/*
			D1.controller();
			if (D1.get_gamepad_shoot() == 1)
			{
				shoot_delay++;
			}
			*/
			
			//Enemy Class
			for (int i = 0; i < wave; i++)
			{
				E_Array[i].set_delta_time();
				E_Array[i].inputs(D1.get_x(), D1.get_y());
				E_Array[i].calculate();
				E_Array[i].stability();
				E_Array[i].draw();
				E_Array[i].check_health(kill_counter);
				//E_Area[i].draw();
			}

			//Update On-screen scores
			text("Kills: ", 10, 710, 0.5);
			text(E_Array[0].get_killcount(), 70, 710, 0.5);
			text("Coins Collected: ", 10, 680, 0.5);
			text(coins[0].get_collected_coins(), 190, 680, 0.5);
			text("Total Score:  ", 10, 650, 0.5);
			text(((coins[0].get_collected_coins())*20.0)+((E_Array[0].get_killcount())*100.0), 150, 650, 0.5);
			text("Wave: ", 10, 620, 0.5);
			text(wave, 90, 620, 0.5);

			if (D1.get_hp() == 0) {											//Status check, restart drone simulation if HP = 0

				break;
			}

			//D1.animate();
			//D1.environment();
			//D1.elements();

			//D1_Area.draw();

			if (KEY('T')) break;			//Restart if lost
			update();
		}
		
		//***SCOREBOARD LOOP***
		for(;;) 
		{
			clear();
			double text_x = 450, text_y = 640, text_scale = 1.0;
			restart.reset(635, 355, 900, 680, 0.0, 0.0, 0.0);				//Backdrop for text
			restart.draw();

			/*File IO should never be in a loop, takes time, so scoreboard input then output occurs once, then use call by reference of arrays to output
			names and points to the screen*/
			if (scoreboard_trigger == 1) {
				scoreboard("scoreboard.txt", player_name, E_Array, coins, firstplace, secondplace, thirdplace, firstpnts, secondpnts, thirdpnts, scoreboard_trigger);
			}

			//Displaying all scoreboard text
			text("Press R to restart", text_x, text_y, text_scale);
			text("Highscores: ", text_x, text_y - 100, text_scale);
			text(firstplace, text_x, text_y - 200, text_scale);
			text(firstpnts, text_x + 400, text_y - 200, text_scale);
			text(secondplace, text_x, text_y - 300, text_scale);
			text(secondpnts, text_x + 400, text_y - 300, text_scale);
			text(thirdplace, text_x, text_y - 400, text_scale);
			text(thirdpnts, text_x + 400, text_y - 400, text_scale);

			if (KEY('R')) 
			{
				break;
			}

			update();
		}

	}

	cout << "\ndone.\n";
	getchar();

	return 0;
}



void collision(Drone &A, Box Drone, Box Rigid, Animation &_animation, Sound _sound)
{

	if (Drone.get_left() < Rigid.get_right() && Drone.get_right() > Rigid.get_left() && Drone.get_bottom() < Rigid.get_top() && Drone.get_top() > Rigid.get_bottom())
	{
		_animation.trigger(A.get_x(), A.get_y());
		_sound.play();
		A.bounce();
	}
}

void getting_shot(Drone &Enemy_Drone,Box Enemy_Area,Bullet &bullet, Animation &explosion, Sound sound)
{
	if (bullet.get_x() < Enemy_Area.get_right() && bullet.get_x() > Enemy_Area.get_left() && bullet.get_y() < Enemy_Area.get_top() && bullet.get_y() > Enemy_Area.get_bottom())
	{
		explosion.trigger(bullet.get_x(), bullet.get_y());
		//explosion.return_trig() = 1;

		bullet.get_state() = 0;
		bullet.set_initial(0, 0, 0);
		Enemy_Drone.bounce();
		Enemy_Drone.get_damage();
		sound.play();
		
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


void Health_Bar(Enemy enemy, Box black, Box green)
{
	black.reset(enemy.get_x(), (enemy.get_y() + 40), 106, 16, 0.0, 0.0, 0.0);	//Black outline of the HP bar
	black.draw();

	green.reset(enemy.get_x() - ((100 - enemy.get_hp()) / 2), enemy.get_y() + 40, enemy.get_hp(), 10, 0.0, 1.0, 0.0);	//This assumes HP is set at 100, not flexible
	green.draw();
}

void map_coins(char coin_locations[], Coin coin_array[], int nb_coins) {      //regular function will map all the coins in one shot, this is regular function 
	int i = 0;													//because we're mapping all the objects in one shot...can't do that in a member 
	double x_value, y_value;									//function of a single object

	ifstream fin;
	fin.open(coin_locations);
	if (!fin) {
		cout << "\nError opening coin locations file in map_coins function";
		exit(1);
	}

	for (i = 0; i < nb_coins; i++) {					//grabbing coordinates from file and initializing coin locations
		fin >> x_value;
		coin_array[i].get_x() = x_value;
		fin >> y_value;
		coin_array[i].get_y() = y_value;
	}

	fin.close();

}

void grab_coin(Box& Drone_Area, Coin coin_array[], int nb_coins) {		//when a coin object gets grabbed, change state from 1 to 0, make collected coins counter go up(and make it respawn?)


	for (int i = 0; i < nb_coins; i++) {
		if (coin_array[i].get_state() == 1 && coin_array[i].get_x() < Drone_Area.get_right() && coin_array[i].get_x() > Drone_Area.get_left() && coin_array[i].get_y() < Drone_Area.get_top() && coin_array[i].get_y() > Drone_Area.get_bottom())
		{
			//coin_array[i].get_state() == 1 means the coin has previously not been grabbed already
			coin_array[i].get_state() = 0;
			coin_array[i].get_collected_coins() += 1;
			cout << "\n\tYou have collected: " << coin_array[i].get_collected_coins() << " so far!";
		}
	}
}

void reset_state(Coin coin_array[], int nb_coins) {
	for (int i = 0; i < nb_coins; i++) {
		if (coin_array[i].get_state() == 0) {				//only if it's 0, revert it back to 1 and decrease collected_coins by 1
			coin_array[i].get_state() = 1;
			coin_array[i].get_collected_coins() -= 1;
		}
	}

}

/*function that takes player_name and coins/drones killed and score board file.
In the function, points for the player will be calculated. File input will occur and grab the data from the scoreboard file. Player will be compared
to the data in the file and sorted. File output will occur and overwrite with the new sorted data to scoreboard file. Names and values will be returned
from the function so it can be drawn to the end screen continuously, below "Press R to restart"
*/
void scoreboard(char scoreboard_file[], char _player_name[], Enemy enemy_array[], Coin coin_array[], char _first[], char _second[], char _third[], double& _firstpnts, double& _secondpnts, double& _thirdpnts, bool& scoreboard_trigger) {
	int i;
	double tempfirstpnts, tempsecondpnts, tempthirdpnts, playerpnts;	//store the scoreboard points
	char temp_first[20], temp_second[20], temp_third[20];				//store the scoreboard names

	playerpnts = ((coin_array[0].get_collected_coins()) * 20.0) + ((enemy_array[0].get_killcount())*100); 	/* All coin objects have the same collected_coins value, I said each coin is 20 points. ***Dead drones must be added***
																These points will be compared to the temp points to see where the player belongs */
																//Start grabbing current scoreboard names/points
	ifstream fin;

	fin.open(scoreboard_file);

	if (!fin) {
		cout << "\nError opening scoreboard file!";
	}

	fin >> temp_first >> tempfirstpnts >> temp_second >> tempsecondpnts >> temp_third >> tempthirdpnts;

	fin.close();

	//***Following Code is in Place to Ensure Scoreboard File is Organized from Highest to Lowest before inputting to arrays***

	if (tempfirstpnts > tempsecondpnts) {
		if (tempfirstpnts > tempthirdpnts) {
			for (i = 0; i < 20; i++) {
				_first[i] = temp_first[i];
			}
			_firstpnts = tempfirstpnts;
			if (tempsecondpnts > tempthirdpnts) {
				for (i = 0; i < 20; i++) {
					_second[i] = temp_second[i];
				}
				_secondpnts = tempsecondpnts;
				for (i = 0; i < 20; i++) {
					_third[i] = temp_third[i];
				}
				_thirdpnts = tempthirdpnts;
			}
			else if (tempsecondpnts < tempthirdpnts) {
				for (i = 0; i < 20; i++) {
					_second[i] = temp_third[i];
				}
				_secondpnts = tempthirdpnts;
				for (i = 0; i < 20; i++) {
					_third[i] = temp_second[i];
				}
				_thirdpnts = tempsecondpnts;
			}
		}
		else if (tempfirstpnts < tempthirdpnts) {
			for (i = 0; i < 20; i++) {
				_first[i] = temp_third[i];
			}
			_firstpnts = tempthirdpnts;
			for (i = 0; i < 20; i++) {
				_second[i] = temp_first[i];
			}
			_secondpnts = tempfirstpnts;
			for (i = 0; i < 20; i++) {
				_third[i] = temp_second[i];
			}
			_thirdpnts = tempsecondpnts;
		}
	}
	else if (tempfirstpnts < tempsecondpnts) {
		if (tempfirstpnts > tempthirdpnts) {
			for (i = 0; i < 20; i++) {
				_first[i] = temp_second[i];
			}
			_firstpnts = tempsecondpnts;
			for (i = 0; i < 20; i++) {
				_second[i] = temp_first[i];
			}
			_secondpnts = tempfirstpnts;
			for (i = 0; i < 20; i++) {
				_third[i] = temp_third[i];
			}
			_thirdpnts = tempthirdpnts;
		}
		else if (tempfirstpnts < tempthirdpnts) {
			for (i = 0; i < 20; i++) {
				_third[i] = temp_first[i];
			}
			_thirdpnts = tempfirstpnts;
			if (tempsecondpnts > tempthirdpnts) {
				for (i = 0; i < 20; i++) {
					_first[i] = temp_second[i];
				}
				_firstpnts = tempsecondpnts;
				for (i = 0; i < 20; i++) {
					_second[i] = temp_third[i];
				}
				_secondpnts = tempthirdpnts;
			}
			else if (tempsecondpnts < tempthirdpnts) {
				for (i = 0; i < 20; i++) {
					_first[i] = temp_third[i];
				}
				_firstpnts = tempthirdpnts;
				for (i = 0; i < 20; i++) {
					_second[i] = temp_second[i];
				}
				_secondpnts = tempsecondpnts;
			}
		}
	}

	/*At this point, the scoreboard names have been input to first/second/third place char arrays, now compare player's point to scoreboard and adjust
	first second and third place arrays accordingly */

	if (playerpnts >= _firstpnts) {				// >= means if same points reached, player steals rank in leaderboard
		for (i = 0; i < 20; i++) {
			_third[i] = _second[i];
		} for (i = 0; i < 20; i++) {
			_second[i] = _first[i];
		} for (i = 0; i < 20; i++) {
			_first[i] = _player_name[i];
		}
		_thirdpnts = _secondpnts;
		_secondpnts = _firstpnts;
		_firstpnts = playerpnts;
	}
	else if (playerpnts >= _secondpnts) {
		for (i = 0; i < 20; i++) {
			_third[i] = _second[i];
		} for (i = 0; i < 20; i++) {
			_second[i] = _player_name[i];
		}
		_thirdpnts = _secondpnts;
		_secondpnts = playerpnts;
	}
	else if (playerpnts >= _thirdpnts) {
		for (i = 0; i < 20; i++) {
			_third[i] = _player_name[i];
		}
		_thirdpnts = playerpnts;
	}
	else if (playerpnts < _thirdpnts) {
		cout << "\n\n\n  Too bad " << _player_name << ". " << playerpnts << " was not high enough to reach the leaderboards! Keep trying, you can beat " << _first << "!";
	}

	//Output current updated scoreboard to the scoreboard file

	ofstream fout;

	fout.open(scoreboard_file);

	if (!fout) {
		cout << "failed opening file output file";
	}

	fout << _first << " " << _firstpnts << "\n";
	fout << _second << " " << _secondpnts << "\n";
	fout << _third << " " << _thirdpnts;

	fout.close();

	/*Now trigger scoreboard boolean so scoreboard function is not called again in loop.Then start drawing to screen the new end screen using the
		new scoreboard names / points (used call by reference, should be able to access scoreboard names/points outside of this function) */

	scoreboard_trigger = 0;
}

void Spawn(Enemy E_Array[], int& wave, long int rand_s, int &kill_counter)
{
	int random = 0;
	


	if (kill_counter == 0)
	{
		wave++;
		kill_counter = wave;

		for (int i = 0; i < wave; i++)
		{
			random = 2000 * ((ran(rand_s) - 0.5) / 0.5) + 1000;

			while (random > 200 && random < 1100)
			{
				random = 2000 * ((ran(rand_s) - 0.5) / 0.5) + 1000;
			}

			E_Array[i].reset2(random, random , 0);
			//E_Array[i].reset2(0, 0, 0);
		}
		
	}
	
}