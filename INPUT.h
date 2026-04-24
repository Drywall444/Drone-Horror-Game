#pragma once
#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include "SPRITE.h"

struct CAMERA
{
	SDL_FPoint camPOS = { 0.0, 0.0 };
	SDL_FPoint offSET;
	float zoom = 1;
};

struct MOUSE
{
	SDL_FPoint screenMOUSE_POS;
	bool isHELD_DOWN = false;
};

class INPUT
{
public:
	INPUT(SPRITE_MANAGER& sm) : gameSPRITES(sm) {}
	//PLAYER STUFF
	bool seclectedSOLDIER = false;
	entt::entity curSELECTED_SOLDIER;
	entt::entity nothing;;


	//Camera Movement
	void cameraMOVEMENT();

	//INTERACTION
	void checkLEFTCLICK_RETURN_SPRITE(SDL_FPoint globalPOS);


	int WINDOW_W = 0;
	int WINDOW_H = 0;
	int WINDOW_CENTER_X = 0;
	int WINDOW_CENTER_Y = 0;
	float DT;

	double scrollWHEEL_SENS = 0.1;

	CAMERA C;
	float cameraSPEED = 650;
	MOUSE M;
	const bool* KEYS = nullptr;
	bool RUN = true;
	int fuckASSdegrees = 0;
	float deg = 0;
	void handleINPUT(float dt);
	void initializeINPUT(int windowW, int windowH);

	//CAMERA
	void getCAM_OFFSET();
	SDL_FPoint screenTO_WORLD_POS(SDL_FPoint screenPOINT);
	void getWINDOWSIZE(SDL_Window* window);


private:
	SPRITE_MANAGER& gameSPRITES;
	Uint64 frameSTART = 0;
	Uint64 frameEND = 0;
	double timeINSEC;

};

#endif // !INPUT_H

#pragma once
