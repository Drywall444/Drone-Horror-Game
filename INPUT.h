#pragma once
#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>


struct CAMERA
{
	float x = 0.0;
	float y = 0.0;
	float zoom = 1;
};

class INPUT
{
public:
	CAMERA C;
	bool RUN = true;
	int fuckASSdegrees = 0;
	float cameraSPEED = 450;
	float deg = 0;
	void handleINPUT(float dt);

private:
};

#endif // !INPUT_H

#pragma once
