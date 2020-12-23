
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


Coin::Coin() {
	coin_x = 0.0;
	coin_y = 0.0;
	coin_state = 1;
	respawn_timer = 0.0;

	hitbox = new Box(coin_x, coin_y, 50, 50, 1, 1, 1);		//single dynamic box object for the coin object
	if (hitbox == NULL) {
		cout << "\nallocation error of coin hitbox in coin::coin() constructor";
	}
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

Coin::~Coin() {
	if (hitbox != NULL) {
		delete hitbox;
		hitbox == NULL;
	}
}