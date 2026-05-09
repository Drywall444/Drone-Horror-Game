#pragma once
#ifndef GAME_MATH_H
#define GAME_MATH_H
#include <random>
#include "SDL3/SDL.h"
#include "entt.hpp"

//MATH

constexpr inline float PI = 3.1415926;

struct ROTATION
{
	float cosR, sinR;
};

struct LOCATION
{
	SDL_FPoint POS;
	ROTATION ROT;
	int z = 0;
};

inline ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;
	float length = sqrt(dX * dX + dY * dY);
	float cosR = dX / length;
	float sinR = dY / length;
	return { -sinR, cosR }; // 90° CCW baked in
}

inline ROTATION directionTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;
	float length = sqrt(dX * dX + dY * dY);
	return { dX / length, dY / length }; // no 90° bake, pure direction
}


inline SDL_FPoint rotatePOINT(SDL_FPoint pos, ROTATION rot)
{

	SDL_FPoint FINAL = { (pos.x * rot.cosR - pos.y * rot.sinR), (pos.x * rot.sinR + pos.y * rot.cosR) };

	return FINAL;
}

inline SDL_FPoint rotatePOINT_AND_APPLY_OFFSET(SDL_FPoint pos, ROTATION rot, SDL_FPoint offSET)
{

	SDL_FPoint rotatedPOINT = rotatePOINT(offSET, rot);
	SDL_FPoint FINAL = { pos.x + rotatedPOINT.x, pos.y + rotatedPOINT.y };

	return FINAL;
}


inline float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;

	float length = sqrt(dX * dX + dY * dY);
	return length;
}

inline float distanceTO_POINT_SQ(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;
	return dX * dX + dY * dY;
}

inline float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB)//RETURNS Dot Product of two rotations
{
	float dot = rotA.cosR * rotB.cosR + rotA.sinR * rotB.sinR;
	//dot product of two angles 
	return dot;
}

inline bool isPOINT_WITHIN_BOUNDS(SDL_FPoint point, SDL_FPoint spritePOS, ROTATION spriteROT, int texW, int texH)
{

	float localX = point.x - spritePOS.x;//Translate that hoe into local space
	float localY = point.y - spritePOS.y;

	float rotatedX = localX * spriteROT.cosR - localY * spriteROT.sinR;
	float rotatedY = localX * spriteROT.sinR + localY * spriteROT.cosR;

	float halfW = texW / 2.0f;
	float halfH = texH / 2.0f;

	return (rotatedX >= -halfW && rotatedX <= halfW && rotatedY >= -halfH && rotatedY <= halfH);

}

inline SDL_FPoint getCENTER(SDL_FPoint pos, float texW, float texH)
{
	return { pos.x + texW * 0.5f, pos.y + texH * 0.5f };
}

inline float randBETWEEN(float min, float max)
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

inline ROTATION randROTATION()
{
	float randDEGREE = randBETWEEN(0.0, 360.0);
	float rad = randDEGREE * PI / 180.0;
	ROTATION final = { cos(rad), sin(rad) };
	return final;
}

inline SDL_FPoint randPOINT_FROM(SDL_FPoint pos, float range)
{
	float randX = randBETWEEN(-range, range);
	float randY = randBETWEEN(-range, range);
	SDL_FPoint finalPOS = { pos.x + randX, pos.y + randY };
	return finalPOS;

}


#endif // !GAME_MATH_H

