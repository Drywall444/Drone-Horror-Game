#pragma once
#ifndef SPRITE_H
#define SPRITE_H
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"

enum textureATLAS //Keep all human types on one sheet and naturage and foliage on another
{
	HUMAN = 0,
	NATURE = 1
};

enum spriteTYPE //Signfiys what you are specfically will an enum for each atlas
{
	TYPE_SOLDIER_STANDING = 0,
	TYPE_SOLDIER_SHOOTING = 1,
	TYPE_SOLDIER_ENEMY_STANDING = 2,
	TYPE_SOLDIER_ENEMY_SHOOTING = 3,
	DEAD_1,
	DEAD_2,
	E_DEAD_1,
	E_DEAD_2
};
enum natureTYPE_TILE
{
	TYPE_GRASS,
	TYPE_GRASS1,
	TYPE_WOODS,
	TYPE_WOODS1,
};

struct ROTATION
{
	float cosR, sinR;
};

struct LOCATION
{
	SDL_FPoint POS;
	ROTATION ROT;
};

struct spriteOBJECT//SHIT THAT CAN MOVE
{
	LOCATION spriteLOCATION;
	textureATLAS textureSHEET_NUM; //ADD DEFAULT
	spriteTYPE TYPE; //what is drawn
};

struct TILE//SHIT THAT CAN MOVE
{
	SDL_FPoint pos;
	textureATLAS textureSHEET_NUM; //ADD DEFAULT
	natureTYPE_TILE TYPE;
};

struct soldierOBJECT 
{
	bool friendly = false;

	float HP = 100.0;
	float speed = 150.0;
	bool isSHOOTING = false;

	//WEAPON INFO
	float weaponEFFECTIVE_RANGE = 3000.0;
	float weaponRPM = 100;
	float weaponDMG = 75.0; 
	int magSIZE = 30;

	//STATS
	float soldierSKILL = 0.8; //default 0.8 increase by 0.1 per day survived
};

//TAGS
struct MOVING
{
	float dX, dY;
	SDL_FPoint targetLOC;
};

struct FIRING
{
	float TIME_SINCE_LAST_SHOT = 0.0;
	float secPER_BULLET = 0.0;
};

struct hasTARGET
{
	entt::entity enemySOLDIER;
	bool targetDEAD = false; //quit firing
};

struct selectedNOTHING {};


class SPRITE_MANAGER
{
	public:
		//GAME LOOP
		float DT = 0.0;
		void updateDT(float newDT);
		void updateGAME();


		int soldierTEXW = 64;
		int soldierTEXH = 128;

		entt::registry spriteREGISTER;
		void spriteCREATE(textureATLAS sheetNUM, spriteTYPE type, SDL_FPoint pos, ROTATION ROT);
		void tileCREATE(textureATLAS sheetNUM, natureTYPE_TILE type, SDL_FPoint pos);

		//SOLDIER MANAGER
		void createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isENEMY); 
		void ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS);

		//SOLDIER ACTIONS
		void checkLOS(entt::entity soldier, bool checkENEMYS);
		void soldierSHOOT_AT_TARGET(entt::entity soldier);
		void fireWEAPON(entt::entity solder, hasTARGET target);
		void soldierRELOAD(entt::entity soldier);

		//VFX
		void spawnBULLET();

};

//MATH
ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB);
float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
bool isPOINT_WITHIN_BOUNDS(SDL_FPoint point, SDL_FPoint spritePOS, ROTATION spriteROT, int texW, int texH);

#endif // !SPRITE_H

