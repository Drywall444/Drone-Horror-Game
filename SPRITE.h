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
	TYPE_DRONE = 1
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
};

struct playerOBJECT
{
	float HP = 100;
};

class SPRITE_MANAGER
{
	public:
		entt::registry spriteREGISTER;
		void spriteCREATE(textureATLAS sheetNUM, spriteTYPE type);

};

//MATH
ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB);
float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB);

#endif // !SPRITE_H

