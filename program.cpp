
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

#include <Windows.h> // also needed for the 2D graphics library

#include "2D_graphics.h" // use the 2D graphics library

#include "game_pad.h"

using namespace std;

#include "timer.h" // use the time / clock reading function

#include <MMSystem.h>



#pragma comment(lib,"winmm.lib") // links a windows library


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
	acceleration = 9.5;
	

	index = 1;
	create_sprite("FrontView.png", id_drone);
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
	acceleration = 9.5;
	GS[9] = 0;

	index = 1;
	create_sprite("FrontView.png", id_drone);
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
	if (y + y_dot >= 800) {										//changed this so drone no longer moves past y pixels: 200-800
		y = 800;
	}
	else if (y + y_dot <= 200) {
		y = 200;
	}
	else y = y + y_dot;


	x_dotdot =  -acceleration * gravity * sin(theta);
	x_dot = x_dot + x_dotdot * delta_time;
	if (x + x_dot >= 1100 ) {									//changed this so drone no longer moves past x pixels: 300-1100
		x = 1100;
	} else if (x + x_dot <= 300) {
		x = 300;
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
	cout << "\n";
	
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

	if (hp >= 5) {
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
private:
	double dist_x;
	double dist_y;
	double radius, radius_limit;
	static int enemy_killcount;
	
public:
	Enemy() { ; }
	Enemy(double _x, double _y, double _theta) : Drone(_x, _y, _theta) {}
	void inputs(double player_x, double player_y);
	void stability();
	void calculate();
	void map_rel(Drone name1);
	double get_aim(Drone player);
	double get_radius() { return radius; }
	void check_health();
	void reset_killcount();
	~Enemy() { ; }

};


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

	x_dotdot = -50 * gravity * sin(theta);
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

void Enemy::check_health()
{
	if (hp <= 0)
	{
		x = 100000;
		y = 100000;
		hp = 100;
		enemy_killcount++;
	}
	cout << enemy_killcount << endl;
}

void Enemy::reset_killcount()
{
	enemy_killcount = 0;
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
	create_sprite("Bullet.png", id_bullet);
	state = 0;
	speed = 4;
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
	double layer_scale;
	double df;			//depth factor, the depth of the layer will influence scroll rate

public:
	map(char layer_file_name[], double depth_factor, double layerX, double layerY, double scale);		//constructor to initialize the layer object, called outside of the infinite draw loop
	void draw_layer(Drone& name1);			//this will contain "draw_sprite()" and manipulate x and y of layer to track drone object properly.Called inside infinite draw loop
	double get_layerX();
	double get_layerY();

};

map::map(char layer_file_name[], double depth_factor, double layerX, double layerY, double scale) {		//takes the file name for this object, to create sprite
	layer_x = layerX;
	layer_y = layerY;
	layer_scale = scale;
	df = depth_factor;
	create_sprite(layer_file_name, id_layer);
}

void map::draw_layer(Drone& name1) {
	//need to adjust x and y of sprite based on drone object, MAKE SURE EQUATIONS ARE GOOD!

	if (name1.get_x() >= 1050) {		//1050 and 350 to start smooth scrolling before drone hits boundary
		layer_x -= name1.get_x_dot()*df;					//***add depth factor***
	}
	else if (name1.get_x() <= 350) layer_x -= name1.get_x_dot() * df;

	if (name1.get_y() >= 500) {		//1050 and 350 to start smooth scrolling before drone hits boundary
		name1.get_y() = 500;
		layer_y -= name1.get_y_dot()*df;				//***add depth factor***
	}
	else if (name1.get_y() <= 250)
	{
		name1.get_y() = 250;
		layer_y -= name1.get_y_dot() * df;
	}
	draw_sprite(id_layer, layer_x, layer_y, 0, layer_scale);
}

double map::get_layerX() {
	return layer_x;
}

double map::get_layerY() {
	return layer_y;
}
//Declare array of coin objects
//map their locations on the map
//their x and y position update with the map
//draw (if state = 1) them to the screen above the layers but below the drone
//After: they can be grabbed and state = 0 to disappear... so only draw if state = 1
//with time, they will return to state 1. 

class Coin {
	double coin_x;
	double coin_y;
	int id_coin1;				//ID for coin sprite
	int id_coin2;
	int id_coin3;
	int id_coin4;
	double coin_rotating;			//no turn, 1, half way is 2, full turn is 3
	bool coin_state;					//when state == 0, coin disappears, coin returns to state == 1 after time passes
	static int collected_coins;			//static variable shared among all objects, they will all increment this value when they are grabbed! (grab_coin function being called)

public:
	Coin();										//constructor for each coin object, all should spawn ie: state == 1
	double& get_x() { return coin_x; };			//access function necessary when mapping coins from a text file
	double& get_y() { return coin_y; };			//access function necessary when mapping coins from a text file
	bool& get_state() { return coin_state; };
	int& get_collected_coins() { return collected_coins; };
	void draw_coin(Drone& name1);			//this is called to draw each coin object, using layer 4 relative displacement	
};

int Coin::collected_coins = 0;				//Can't initialize static member variables in the constructor

Coin::Coin() {
	coin_x = 0.0;
	coin_y = 0.0;
	create_sprite("pirate_coin1.png", id_coin1);
	create_sprite("pirate_coin2.png", id_coin2);
	create_sprite("pirate_coin3.png", id_coin3);
	create_sprite("pirate_coin4.png", id_coin4);
	coin_rotating = 2.0;
	coin_state = 1;
}

void Coin::draw_coin(Drone& name1) {

	//coin position changes with map/drone movement:
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

	//drawing a rotating coin
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
	bool trigger_state;
public:
	Animation(char file_seq[], double _duration);
	void trigger(double _x, double _y);
	void animate();
	bool& return_trig() { return trigger_state; }
};

Animation::Animation(char file_seq[], double _duration)
{
	index = 0;
	delay = 0;
	duration = _duration;
	x = 0;
	y = 0;
	trigger_state = 0;

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
		draw_sprite(sequence[index], x, y, 0, 1);

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

class Sound
{
private:
	int n;
	char* p_buffer, * p_buffer_end;
	short int* p_data;

public:
	Sound(char file_name[]);
	void play();
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
const int nb_enemy = 3;
const int nb_coins = 5;
const int enemy_bullet_limit = 3;
const int player_bullet_limit = 3;
int Enemy::enemy_killcount = 0;

int main()
{
	initialize_graphics();
	Drone D1(0, 0, 0);
	Box D1_Area(0, 0, 120, 40, 1.0, 1.0, 1.0);

	Box Rigid[5];
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

	Animation explosion("Animation/Explosion/animation_sequence.txt", 1);
	Animation collision_animation("Animation/Collision/animation_sequence.txt", 1);

	Sound boom("boom.wav");
	Sound laser("laser.wav");
	Sound collision_sound("collision.wav");

	for (;;)
	{

		bool trigger = 0;
		int shoot_delay;
		int shoot_delay_enemy[nb_enemy];

		D1.reset(400, 300, 0);
		E_Array[0].reset_killcount();
		
		for (int i = 0; i < nb_enemy; i++)
		{
			E_Array[i].reset(1000 + 50*i, 300 + 60*i, 0);
		}
		
		//(1) - Initializing of Layers
		map Layer1("Layer1.png", 0.5, 700.0, 500, 1.8);		//Creating Layer1 object of the map class, attaching the appropriate image and depth factor (df)
		map Layer4("Layer4.png", 0.6, 700.0, 100, 1.65);	//arguments: (image file, its speed relative to other layers (df), x and y position in window relative to other layers, image scaling)
		
		//Initialization of coins on layers
		map_coins("coin_locations.txt", coins, nb_coins);
		//Reset state of coins to 1
		reset_state(coins, nb_coins);

		int id_laser;
		create_sprite("Laser.png", id_laser);

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

		for (;;)
		{
			clear();

			//Box Background(300, 300, 2000, 1000, 0.8, 0.8, 1.0);
			//Background.draw();
			//(1) - Drawing of Layers
			Layer1.draw_layer(D1);				//Updating background according to D1 positioning, absolute background so goes first
			Layer4.draw_layer(D1);
			
			//Drawing of coins
			for (int i = 0; i < nb_coins; i++) {
				coins[i].draw_coin(D1);
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
			
			

			for (int i = 0; i < 1; i++)
			{
				Rigid[i].reset((Layer4.get_layerX() - 500), Layer4.get_layerY()-300, 300, 20, 0.0, 0.0, 0.0);  //-500 in x position relative to Layer4
				Rigid[i].draw();
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

			for (int i = 0; i < nb_enemy; i++)
			{
				for (int j = 0; j < enemy_bullet_limit; j++)
				{

					if (bullet_enemy[j].get_state() == 1)
					{
						bullet_enemy[j].trajectory();
						continue;
					}

					if (shoot_delay_enemy[i] > 40 && bullet_enemy[j].get_state() == 0)
					{
						bullet_enemy[j].set_initial(E_Array[i].get_x(), E_Array[i].get_y(), E_Array[i].get_aim(D1));
						bullet_enemy[j].get_state() = 1;
						shoot_delay_enemy[i] = 0;
						laser.play();
					}
				}
			}

			
			for (int i = 0; i < nb_enemy; i++)
			{
				for (int i = 0; i < player_bullet_limit; i++)
				{
					getting_shot(E_Array[i], E_Area[i], bullet[i], explosion, boom);
				}
			}

			/*
			for (int i = 0; i < enemy_bullet_limit; i++)
			{
				getting_shot(D1, D1_Area, bullet_enemy[i], explosion, boom);
			}
			*/
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

			grab_coin(D1_Area, coins, nb_coins);

			
			D1.controller();
			if (D1.get_gamepad_shoot() == 1)
			{
				shoot_delay++;
			}
			
			
			//Enemy Class
			for (int i = 0; i < nb_enemy; i++)
			{
				E_Array[i].set_delta_time();
				E_Array[i].inputs(D1.get_x(), D1.get_y());
				E_Array[i].calculate();
				E_Array[i].stability();
				E_Array[i].draw();
				E_Array[i].check_health();
				//E_Area[i].draw();
			}



			if (D1.get_hp() == 0) {											//Status check, restart drone simulation if HP = 0

				break;
			}

			//D1.animate();
			//D1.environment();
			//D1.elements();

			//D1_Area.draw();

			if (KEY('R')) break;			//Restart if lost
			
			update();
		}
		
		for(;;) 
		{
			clear();
			double text_x = 255, text_y = 425, text_scale = 1.0;
			restart.reset(450, 400, 400, 200, 0.0, 0.0, 0.0);				//Backdrop for text
			restart.draw();

			text("Press R to restart", text_x, text_y, text_scale);		//Text to restart simulation

			if (KEY('R')) 
			{
				trigger = 1;
				break;
			}

			update();
		}
		trigger = 0;

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
			cout << "\nYou have collected: " << coin_array[i].get_collected_coins() << " so far!";
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