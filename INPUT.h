#pragma once
#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>

struct CAMERA
{
	SDL_FPoint camPOS = { 0.0, 0.0 };
	SDL_FPoint offSET;
	float zoom = 1;
};

struct MOUSE
{
	SDL_FPoint screenMOUSE_POS;
};

class INPUT
{
public:

	int WINDOW_W = 0;
	int WINDOW_H = 0;
	int WINDOW_CENTER_X = 0;
	int WINDOW_CENTER_Y = 0;

	CAMERA C;
	MOUSE M;
	bool RUN = true;
	int fuckASSdegrees = 0;
	float cameraSPEED = 450;
	float deg = 0;
	void handleINPUT(float dt);
	void initializeINPUT(int windowW, int windowH);

	//CAMERA
	void getCAM_OFFSET();
	SDL_FPoint screenTO_WORLD_POS(SDL_FPoint screenPOINT);

private:
};

#endif // !INPUT_H

#pragma once
