
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
#include "Box.h"
#include "Bullet.h"
#include "map.h"
#include "Animation.h"
#include "Coin.h"
#include "Sound.h"


void map_coins(char coin_locations[], Coin coin_array[], int nb_coins);
void grab_coin(Box& Drone_Area, Coin coin_array[], int nb_coins, Sound _coin_sound);
void respawn_coin(Coin coin_array[], int nb_coins);
void reset_state(Coin coin_array[], int nb_coins);
void restore_hp(Drone& name1, Box name2);
void collision(Drone& A, Box Drone, Box Rigid, Animation& _animation, Sound _sound);
void Health_Bar(Enemy enemy, Box black, Box green);
void getting_shot(Drone& Enemy_Drone, Box Enemy_Area, Bullet& bullet, Animation& explosion, Sound sound);
void scoreboard(char scoreboard_file[], char _player_name[], Enemy enemy_array[], Coin coin_array[], char _first[], char _second[], char _third[], double& _firstpnts,
	double& _secondpnts, double& _thirdpnts, bool& scoreboard_trigger, Sound _victory_scoreboard, Sound _defeat_scoreboard);
void Spawn(Enemy E_Array[], int& wave, long int rand_s, int& kill_counter);
void detect_controller(bool& controller_state);

const int nb_enemy = 10;
const int nb_coins = 25;
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

	int id_HPzone;
	create_sprite("Images/HealthBoxImage.png", id_HPzone);
	int id_scoreboard;
	create_sprite("Images/scoreboard.png", id_scoreboard);

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
	Animation coin_animation("Animation/Coins/animation_sequence.txt", 200, 0.2);				//coin png scaled down to 0.2

	Sound boom("Sounds/boom.wav");
	Sound laser("Sounds/laser.wav");
	Sound collision_sound("Sounds/collision.wav");
	Sound coin_sound("Sounds/coin.wav");
	Sound start_sound("Sounds/starting.wav");
	Sound victory_scoreboard("Sounds/Victory.wav");		//Victory sound when player achieves a high score
	Sound defeat_scoreboard("Sounds/Defeat.wav");

	long int rand_s = 22;

	D1.reset(400, 300, 0);
	for (int i = 0; i < nb_enemy; i++)
	{
		E_Array[i].reset(100000000000000 + 50 * i, 300 + 60 * i, 0);
	}

	int id_laser;
	create_sprite("Images/Laser.png", id_laser);

	bool controller_state;
	detect_controller(controller_state);

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

		//SOUND TO START GAME
		start_sound.play();

		for (;;)		//***_GAME LOOP_***
		{

			clear();
			//Drawing of Layers
			Layer1.draw_layer(D1);				//Updating background according to D1 positioning, absolute background so goes first
			Layer4.draw_layer(D1);

			HP_zone1.reset(Layer4.get_layerX(), Layer4.get_layerY(), 200, 200, 0.0, 0.5, 0.0);					//Box object that will be a healing area
			HP_zone1.draw();
			draw_sprite(id_HPzone, Layer4.get_layerX(), Layer4.get_layerY(), 0, 0.234);

			Spawn(E_Array, wave, rand_s, kill_counter);

			//Drawing of coins
			for (int i = 0; i < nb_coins; i++) {
				coins[i].draw_coin(D1, Layer4, coin_animation);			//Added Animation class to animate coin
				coins[i].hitbox->reset(coins[i].get_x(), coins[i].get_y(), 50, 50, 1.0, 1.0, 1.0);
			}

			D1_Area.reset(D1.get_x(), D1.get_y(), 120, 40, 1.0, 1.0, 1.0);

			for (int i = 0; i < nb_enemy; i++)
			{
				E_Area[i].reset(E_Array[i].get_x(), E_Array[i].get_y(), 150, 100, 1.0, 1.0, 1.0);
			}


			for (int i = 0; i < nb_enemy; i++)
			{
				E_Array[i].map_rel(D1);
			}



			Rigid[0].reset((Layer4.get_layerX() - 0), Layer4.get_layerY() + 1100, 5000, 60, 0.0, 0.0, 0.0);  //-500 in x position relative to Layer4
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

			grab_coin(D1_Area, coins, nb_coins, coin_sound);			//grab coin if drone hits a coin hitbox
			respawn_coin(coins, nb_coins);								//respawn a coin, respawn timer increments by 0.1, respawns at respawn_timer = 100

			if (controller_state == 1)
			{
				D1.controller();
				if (D1.get_gamepad_shoot() == 1)
				{
					shoot_delay++;
				}
			}

			D1_HPb.reset(D1.get_x(), (D1.get_y() + 40), 106, 16, 0.0, 0.0, 0.0);	//Black outline of the HP bar
			D1_HPb.draw();

			D1_HPg.reset(D1.get_x() - ((100 - D1.get_hp()) / 2), D1.get_y() + 40, D1.get_hp(), 10, 0.0, 1.0, 0.0);	//This assumes HP is set at 100, not flexible
			D1_HPg.draw();

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


			for (int i = 0; i < nb_enemy; i++)
			{
				Health_Bar(E_Array[i], D2_HPb[i], D2_HPg[i]);
			}

			//Update On-screen scores
			text("Kills: ", 10, 710, 0.5);
			text(E_Array[0].get_killcount(), 70, 710, 0.5);
			text("Coins Collected: ", 10, 680, 0.5);
			text(coins[0].get_collected_coins(), 190, 680, 0.5);
			text("Total Score:  ", 10, 650, 0.5);
			text(((coins[0].get_collected_coins()) * 20.0) + ((E_Array[0].get_killcount()) * 100.0), 150, 650, 0.5);
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
		for (;;)
		{
			clear();
			draw_sprite(id_scoreboard, 0, 0, 0, 0);
			double text_x = 375, text_y = 640, text_scale = 1.0;
			restart.reset(635, 355, 900, 680, 0.0, 0.0, 0.0);				//Backdrop for text
			//restart.draw();

			/*File IO should never be in a loop, takes time, so scoreboard input then output occurs once, then use call by reference of arrays to output
			names and points to the screen*/
			if (scoreboard_trigger == 1) {
				scoreboard("scoreboard.txt", player_name, E_Array, coins, firstplace, secondplace, thirdplace, firstpnts, secondpnts, thirdpnts, scoreboard_trigger, victory_scoreboard, defeat_scoreboard);
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



void collision(Drone& A, Box Drone, Box Rigid, Animation& _animation, Sound _sound)
{

	if (Drone.get_left() < Rigid.get_right() && Drone.get_right() > Rigid.get_left() && Drone.get_bottom() < Rigid.get_top() && Drone.get_top() > Rigid.get_bottom())
	{
		_animation.trigger(A.get_x(), A.get_y());
		_sound.play();
		A.bounce();
	}
}

void getting_shot(Drone& Enemy_Drone, Box Enemy_Area, Bullet& bullet, Animation& explosion, Sound sound)
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

		coin_array[i].hitbox->reset(coin_array[i].get_x(), coin_array[i].get_y(), 50, 50, 1.0, 1.0, 1.0); //Re-initialize the hitbox to each coin, they will adjust in position with the coins in main
	}

	fin.close();

}

void grab_coin(Box& Drone_Area, Coin coin_array[], int nb_coins, Sound _coin_sound) {		//when a coin object gets grabbed, change state from 1 to 0, make collected coins counter go up(and make it respawn?)

	for (int i = 0; i < nb_coins; i++) {
		if (coin_array[i].get_state() == 1 && coin_array[i].hitbox->get_left() < Drone_Area.get_right() && coin_array[i].hitbox->get_right() > Drone_Area.get_left() && coin_array[i].hitbox->get_bottom() < Drone_Area.get_top() && coin_array[i].hitbox->get_top() > Drone_Area.get_bottom())
		{
			coin_array[i].get_state() = 0;					//coin_array[i].get_state() == 1 means the coin has previously not been grabbed already
			coin_array[i].get_collected_coins() += 1;
			_coin_sound.play();

			cout << "\n\tYou have collected: " << coin_array[i].get_collected_coins() << " so far!";
		}
	}
}

void respawn_coin(Coin coin_array[], int nb_coins) {

	for (int i = 0; i < nb_coins; i++) {
		if (coin_array[i].get_state() == 0) {							//if coin object state is 0 (it is despawned), increment respawn timer
			coin_array[i].get_respawn_timer() = coin_array[i].get_respawn_timer() + 0.1;
			//cout << "\nrespawn timer is = " << coin_array[i].get_respawn_timer();    Checks time progress

			if (coin_array[i].get_respawn_timer() >= 100.0) {				//once sufficient time has passed based on respawn time, set state back to 1 and reset respawn timer
				coin_array[i].get_state() = 1;
				coin_array[i].get_respawn_timer() = 0.0;
			}
		}
	}
}

void reset_state(Coin coin_array[], int nb_coins) {
	for (int i = 0; i < nb_coins; i++) {
		coin_array[i].get_state() = 1;
		coin_array[i].get_collected_coins() = 0;
	}
}


/*function that takes player_name and coins/drones killed and score board file.
In the function, points for the player will be calculated. File input will occur and grab the data from the scoreboard file. Player will be compared
to the data in the file and sorted. File output will occur and overwrite with the new sorted data to scoreboard file. Names and values will be returned
from the function so it can be drawn to the end screen continuously, below "Press R to restart"
*/
void scoreboard(char scoreboard_file[], char _player_name[], Enemy enemy_array[], Coin coin_array[], char _first[], char _second[], char _third[], double& _firstpnts, double& _secondpnts, double& _thirdpnts, bool& scoreboard_trigger, Sound _victory_scoreboard, Sound _defeat_scoreboard) {
	int i;
	double tempfirstpnts, tempsecondpnts, tempthirdpnts, playerpnts;	//store the scoreboard points
	char temp_first[20], temp_second[20], temp_third[20];				//store the scoreboard names

	playerpnts = ((coin_array[0].get_collected_coins()) * 20.0) + ((enemy_array[0].get_killcount()) * 100); 	/* All coin objects have the same collected_coins value, I said each coin is 20 points. ***Dead drones must be added***
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

	//MUSIC PLAYS BASED ON PLAYERS SCORE!
	if (playerpnts >= _thirdpnts) {
		_victory_scoreboard.play();
	}
	else {
		_defeat_scoreboard.play();
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

void Spawn(Enemy E_Array[], int& wave, long int rand_s, int& kill_counter)
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

			E_Array[i].reset2(random, random, 0);
			//E_Array[i].reset2(0, 0, 0);
		}

	}

}

void detect_controller(bool& controller_state)
{
	ifstream fin;
	fin.open("controller.txt");
	char temp;

	fin >> temp;

	if (temp == 'Y')
	{
		controller_state = 1;
	}
	else if (temp == 'n')
	{
		controller_state = 0;
	}

	fin.close();
}