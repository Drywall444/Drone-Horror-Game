#pragma once
#ifndef RENDER_H
#define RENDER_H
#define FNL_IMPL
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"
#include "INPUT.h"
#include "SPRITE.h"

class RENDER
{
public:
	RENDER(INPUT& I) : IN(I) {}


	//MAP SHIT
	float natureTEX_W = 64.0, natureTEX_H = 64.0;
	int MAP_W = 1000;
	int MAP_H = 100;

	struct TILE_REGION
	{
		float uMIN;
		float uMAX;
		float vMIN;
		float vMAX;
	};
	TILE_REGION GRASS_1 = { 0.0f,  0.25f, 0.0, 1.0};
	TILE_REGION GRASS_2 = { 0.25f,  0.50f, 0.0, 1.0 };
	TILE_REGION WOODS_1 = { 0.50f,  0.75f, 0.0, 1.0 };
	TILE_REGION WOODS_2 = { 0.75f,  1.0f,  0.0, 1.0 };

	TILE_REGION ENEMY_SOLDIER_STANDING = { 0.0f, 0.20f, 0.0, 0.5 };
	TILE_REGION ENEMY_SOLDIER_SHOOTING = { 0.20f, 0.40f, 0.0, 0.5 };
	TILE_REGION ENEMY_SOLDIER_DEAD_1 = { 0.60f, 0.80f, 0.0, 0.5 };
	TILE_REGION ENEMY_SOLDIER_DEAD_2 = { 0.80f, 1.0f, 0.0, 0.5 };

	TILE_REGION SOLDIER_STANDING = { 0.0f, 0.20f, 0.5, 1.0 };
	TILE_REGION SOLDIER_SHOOTING = { 0.20f, 0.40f, 0.5, 1.0 };
	TILE_REGION SOLDIER_DEAD_1 = { 0.60f, 0.80f, 0.5, 1.0 };
	TILE_REGION SOLDIER_DEAD_2 = { 0.80f, 1.0f, 0.5, 1.0 };



	std::vector<int> NATURE_INDICIES;
	std::vector<SDL_Vertex> NATURE_VERTEXES;


	std::vector<int> HUMAN_INDICIES;
	std::vector<SDL_Vertex> HUMAN_VERTEXES;

	float humanTEX_W, humanTEX_H;

	SDL_Window* window = nullptr;
	SDL_Renderer* REND = nullptr;
	SDL_Texture* humanTEXTURE;
	SDL_Texture* groundTEXTURE;

	void renderSPRITES_ON_SCREEN(entt::registry& spriteREGISTER, CAMERA camera);
	void initializeRENDER();


	//OPTIMIZATION
	void emplaceSPRITES_ON_SCREEN();

	//MAP CREATION
	void createMAP(SPRITE_MANAGER& sprites);

private:
	INPUT& IN;

};

#endif // !RENDER_H

