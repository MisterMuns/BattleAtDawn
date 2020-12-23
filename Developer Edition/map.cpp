
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
#include "map.h"

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
			//cout << "\n\n\t YOU CANNOT GO ANY FURTHER";   spams command window
		}
	}
	else if (name1.get_x() <= 250) {
		if (layer_x < max_pos_x) {
			last_x = layer_x;
			layer_x -= name1.get_x_dot() * df;
		}
		else if (layer_x >= max_pos_x) {
			//cout << "\n\n\t YOU CANNOT GO ANY FURTHER";    spams command window
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