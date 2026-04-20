#include "SPRITE.h"

//MATH

ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;

	float length = sqrt(dX * dX + dY * dY);
	//divide by length to normalize to 1

	float cosR = dX / length;
	float sinR = dY / length;

	return { -sinR, cosR };
}

float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB)//RETURNS Dot Product of two rotations
{
	float dot = rotA.cosR * rotB.cosR + rotA.sinR * rotB.sinR;
	//dot product of two angles 
	return dot;
}

//SPRITE_MANAGER

void SPRITE_MANAGER::spriteCREATE(textureATLAS sheetNUM, spriteTYPE type)
{

	entt::entity newSPRITE = spriteREGISTER.create();
	spriteOBJECT newSPRITE_OBJ;
	ROTATION newROT = { 0.0f, -1.0f };
	LOCATION newLOC = { {0.0, 0.0}, newROT };
	newSPRITE_OBJ.spriteLOCATION = newLOC;
	newSPRITE_OBJ.textureSHEET_NUM = sheetNUM;
	spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);
	if (type == TYPE_PLAYER)//check if player, emaplce playerOBJ
	{
		playerOBJECT thePLAYER = { 100.0 };
		spriteREGISTER.emplace<playerOBJECT>(newSPRITE, thePLAYER);
	}
}