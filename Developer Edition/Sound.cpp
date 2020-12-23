
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

#include "Sound.h"

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