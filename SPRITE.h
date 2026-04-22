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
	TYPE_SOLDIER = 0,
	TYPE_SOLDIER_ENEMY = 1,
	TYPE_DRONE = 2,
	TYPE_GRASS = 3,
	TYPE_DIRT = 4,
	TYPE_TREE = 5
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
	spriteTYPE TYPE;
};

struct soldierOBJECT 
{
	float HP = 100;
};

//TAGS
struct staticSPRITE {};
struct humanSPRITE {};


class SPRITE_MANAGER
{
	public:
		entt::registry spriteREGISTER;
		void spriteCREATE(textureATLAS sheetNUM, spriteTYPE type, SDL_FPoint pos);

		//SOLDIER MANAGER
		void createSOLDIER(spriteTYPE soldierTYPE, SDL_FPoint pos, bool isENEMY); 
		void soldierMOVE_TO_POINT(SDL_FPoint point, entt::entity soldier);

};

//MATH
ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB);
float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
bool isPOINT_WITHIN_BOUNDS(SDL_FPoint point, SDL_FPoint spritePOS, ROTATION spriteROT, int texW, int texH);

#endif // !SPRITE_H

