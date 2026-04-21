#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "INPUT.h"
#include "SPRITE.h"

class PLAYER
{
public:
	PLAYER(SPRITE_MANAGER& sm, INPUT& I) : gameSPRITES(sm), IN(I){}//Get created sprite class
	entt::entity thePLAYER;
	float playerTEX_W = 64.0;
	float playerTEX_H = 64.0;

	//Movement
	float playerSPEED_WALK = 250.0;

	void createPLAYER();

	//Player Movement
	void playerROTATE_TO_MOUSE();
	void playerMOVEMENT();

	//CAMERA
	void cameraFOLLOW_PLAYER();


private:
	SPRITE_MANAGER& gameSPRITES;
	INPUT& IN;
};


#endif PLAYER_H
