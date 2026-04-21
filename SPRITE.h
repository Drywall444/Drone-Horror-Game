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
	TYPE_PLAYER = 0,
	TYPE_DRONE = 1,
	TYPE_GRASS = 2,
	TYPE_DIRT = 3,
	TYPE_TREE = 4
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

struct playerOBJECT //make just tag
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

};

//MATH
ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB);
float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);

#endif // !SPRITE_H

