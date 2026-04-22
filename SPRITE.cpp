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

float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;

	float length = sqrt(dX * dX + dY * dY);
	return length;
}

float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB)//RETURNS Dot Product of two rotations
{
	float dot = rotA.cosR * rotB.cosR + rotA.sinR * rotB.sinR;
	//dot product of two angles 
	return dot;
}

bool isPOINT_WITHIN_BOUNDS(SDL_FPoint point, SDL_FPoint spritePOS, ROTATION spriteROT, int texW, int texH)
{
	float centerX = spritePOS.x + texW / 2;
	float centerY = spritePOS.y + texH / 2;

	float localX = point.x - centerX;//Translate that hoe into local space
	float localY = point.y - centerY;

	float rotatedX = localX * spriteROT.cosR - localY * spriteROT.sinR;
	float rotatedY = localX * spriteROT.sinR + localY * spriteROT.cosR;

	float halfW = texW / 2.0f;
	float halfH = texH / 2.0f;

	return (rotatedX >= -halfW && rotatedX <= halfW && rotatedY >= -halfH && rotatedY <= halfH);

}

//SPRITE_MANAGER

void SPRITE_MANAGER::spriteCREATE(textureATLAS sheetNUM, spriteTYPE type, SDL_FPoint pos)
{
		entt::entity newSPRITE = spriteREGISTER.create();
		spriteOBJECT newSPRITE_OBJ;
		ROTATION newROT = { 0.0f, 1.0f };
		LOCATION newLOC = { {pos.x, pos.y}, newROT };
		newSPRITE_OBJ.spriteLOCATION = newLOC;
		newSPRITE_OBJ.textureSHEET_NUM = sheetNUM;
		newSPRITE_OBJ.TYPE = type;
		spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);

		if (sheetNUM == HUMAN)
		{
			spriteREGISTER.emplace<humanSPRITE>(newSPRITE);
			if (type == TYPE_SOLDIER)//check if soldier, emaplce soldierOBJ
			{
				soldierOBJECT thePLAYER = { 100.0 };
				spriteREGISTER.emplace<soldierOBJECT>(newSPRITE, thePLAYER);
			}
		}
		else if (sheetNUM == NATURE)
		{
			spriteREGISTER.emplace<staticSPRITE>(newSPRITE); //emplace static tag for renderer
		}
}

//SOLDIER
 void SPRITE_MANAGER::createSOLDIER(spriteTYPE soldierTYPE, SDL_FPoint pos, bool isENEMY)
{
	 spriteCREATE(HUMAN, TYPE_SOLDIER, pos);
}