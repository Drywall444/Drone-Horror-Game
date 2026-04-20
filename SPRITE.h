#pragma once
#ifndef SPRITE_H
#define SPRITE_H
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"

enum textureATLAS
{
	HUMAN = 0,
	NATURE = 1
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

class SPRITE_MANAGER
{
	public:
		entt::registry spriteREGISTER;
		void spriteCREATE(textureATLAS sheetNUM);

};
#endif // !SPRITE_H

