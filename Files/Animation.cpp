
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

#include "Animation.h"

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
