#pragma once
#ifndef SPRITE_H
#define SPRITE_H
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"
#include "GAME_MATH.h"

enum textureATLAS //Keep all human types on one sheet and naturage and foliage on another
{
	HUMAN,
	CORPSE
};


enum uvTYPE
{
	T_GRASS1,
	T_GRASS2,
	T_WOODS1,
	T_WOODS2,
	T_SOLDIER_STANDING,
	T_SOLDIER_SHOOTING,
	T_SOLDIER_DEAD_1,
	T_SOLDIER_DEAD_2,
	T_ENEMY_SOLDIER_STANDING,
	T_ENEMY_SOLDIER_SHOOTING,
	T_ENEMY_DEAD_1,
	T_ENEMY_DEAD_2,
	T_VFX_TRACER,
	T_VFX_MUZZLE_FLASH_1,
	T_VFX_MUZZLE_FLASH_2,
	T_VFX_MUZZLE_FLASH_3,
	T_BLOOD_1,
	T_BLOOD_2,
	T_BLOOD_3,
	T_BLOOD_4,
	T_MAG

};

struct UV_REGION
{
	uvTYPE uvTYPE;
	float uMIN;
	float uMAX;
	float vMIN;
	float vMAX;
};

enum natureTYPE_TILE
{
	TYPE_GRASS,
	TYPE_GRASS1,
	TYPE_WOODS,
	TYPE_WOODS1,
};

struct GUN
{
	std::string name = "AK-74";
	float weaponEFFECTIVE_RANGE = 3000.0;
	float weaponRPM = 600;
	float weaponDMG = 45.0;
	int magSIZE = 30;
	int curMAG_SIZE = 30; //start at a full mag
	float reloadTIME = 2.5;
	float curRELOAD_TIME = 2.5; //hits zero when reloaded
	bool reloading = false;
};

struct spriteOBJECT//SHIT THAT CAN MOVE
{
	LOCATION spriteLOCATION;
	textureATLAS textureSHEET_NUM; //ADD DEFAULT
	UV_REGION TYPE; //what is drawn
	int texW = 64;
	int texH = 128;
};

struct TILE//SHIT THAT CAN MOVE
{
	SDL_FPoint pos;
	UV_REGION TYPE;
};

struct soldierOBJECT 
{
	bool friendly = false;

	float HP = 100.0;
	float speed = 150.0;
	bool isSHOOTING = false;

	//WEAPON INFO
	GUN weapon;

	//STATS
	float soldierSKILL = 0.5; //default 0.8 increase by 0.1 per day survived
};

//STATE TAGS
struct MOVING
{
	float dX, dY;
	SDL_FPoint targetLOC;
	float movementSPEED = 150.0;
	void move(); //call to advance object
};

struct FIRING
{
	float aimTIME = 0.0;
	float TIME_SINCE_LAST_SHOT = 0.0;
	float secPER_BULLET = 0.0;
	bool firingBURST = false; //ignore aimtime and fire multiple bullets in seccesion
};

struct hasTARGET
{
	entt::entity enemySOLDIER;
	bool targetDEAD = false; //quit firing
};

struct IDLE{};

struct tempSPRITE
{
	float timeLEFT;
};

struct selectedNOTHING {};


class SPRITE_MANAGER
{
	public:
		//SPRITE INFO
		UV_REGION GRASS_1 = { T_GRASS1, 0.0f,  0.25f, 0.0, 1.0 };
		UV_REGION GRASS_2 = { T_GRASS2, 0.25f,  0.50f, 0.0, 1.0 };
		UV_REGION WOODS_1 = { T_WOODS1, 0.50f,  0.75f, 0.0, 1.0 };
		UV_REGION WOODS_2 = { T_WOODS2, 0.75f,  1.0f,  0.0, 1.0 };

		//SPRITES

		UV_REGION SOLDIER_STANDING = { T_SOLDIER_STANDING, 0.0f, 0.20f, 0.4f, 0.8f };
		UV_REGION SOLDIER_SHOOTING = { T_SOLDIER_SHOOTING, 0.20f, 0.40f, 0.40f, 0.8f };
		UV_REGION SOLDIER_DEAD_1 = { T_SOLDIER_DEAD_1, 0.60f,  0.80f, 0.4f, 0.8f };
		UV_REGION SOLDIER_DEAD_2 = { T_SOLDIER_DEAD_2, 0.80f, 1.0f, 0.4f, 0.8f };

		UV_REGION ENEMY_SOLDIER_STANDING = { T_ENEMY_SOLDIER_STANDING, 0.0f, 0.20f, 0.0f, 0.4f };
		UV_REGION ENEMY_SOLDIER_SHOOTING = { T_ENEMY_SOLDIER_SHOOTING, 0.20f, 0.40f, 0.0f, 0.4f };
		UV_REGION ENEMY_SOLDIER_DEAD_1 = { T_ENEMY_DEAD_1, 0.60f, 0.80f, 0.0f, 0.4f };
		UV_REGION ENEMY_SOLDIER_DEAD_2 = { T_ENEMY_DEAD_2, 0.80f, 1.0f, 0.0f, 0.4f };

		UV_REGION VFX_BULLET = { T_VFX_TRACER, 0.15f, 0.2f, 0.8f, 0.85f };
		UV_REGION VFX_MUZZ_FLASH_3 = { T_VFX_MUZZLE_FLASH_1, 0.10f, 0.15f, 0.8f, 0.85f };
		UV_REGION VFX_MUZZ_FLASH_2 = { T_VFX_MUZZLE_FLASH_2, 0.05f, 0.10f, 0.8f, 0.85f };
		UV_REGION VFX_MUZZ_FLASH_1 = { T_VFX_MUZZLE_FLASH_3, 0.0f, 0.05f, 0.8f, 0.85f };

		UV_REGION VFX_BLOOD_1 = { T_BLOOD_1, 0.15f, 0.2f, 0.85f, 0.90f };
		UV_REGION VFX_BLOOD_2 = { T_BLOOD_2, 0.10f, 0.15f, 0.85f, 0.90f };
		UV_REGION VFX_BLOOD_3 = { T_BLOOD_3, 0.05f, 0.10f, 0.85f, 0.90f };
		UV_REGION VFX_BLOOD_4 = { T_BLOOD_4, 0.0f, 0.05f, 0.85f, 0.90f };

		UV_REGION VFX_MAG = { T_MAG, 0.05f, 0.10f, 0.90f, 0.95f };

		//GAME LOOP
		float DT = 0.0;
		void updateDT(float newDT);
		void updateGAME();

		entt::registry spriteREGISTER;
		void spriteCREATE(textureATLAS sheetNUM, SDL_FPoint pos, ROTATION ROT, bool isFRIEND);
		void tileCREATE(UV_REGION type, SDL_FPoint pos);

		//SOLDIER MANAGER
		void createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isFRIEND);
		void ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS);

		//SOLDIER ACTIONS
		void checkLOS(entt::entity soldier, bool checkENEMYS);
		void soldierSHOOT_AT_TARGET(entt::entity soldier);
		void fireWEAPON(entt::entity solder, hasTARGET target);
		void soldierRELOAD(entt::entity soldier);
		void soldierTAKE_DAMAGE(entt::entity soldier, float damage);

		//VFX - Include sound here
		void spawnBULLET(entt::entity soldier, SDL_FPoint target);
		void spawnBLOOD(SDL_FPoint pos, ROTATION rot, UV_REGION bloodTEX_TYPE);
		void spawnMAG(SDL_FPoint pos, ROTATION rot, UV_REGION MAG_TEX_TYPE);

};

////MATH
//ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
//float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB);
//float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
//bool isPOINT_WITHIN_BOUNDS(SDL_FPoint point, SDL_FPoint spritePOS, ROTATION spriteROT, int texW, int texH);
//SDL_FPoint rotatePOINT(SDL_FPoint pos, ROTATION rot);

#endif // !SPRITE_H

