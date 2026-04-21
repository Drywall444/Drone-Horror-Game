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
	};
	TILE_REGION GRASS = { 0.0f,  0.33f };
	TILE_REGION DIRT = { 0.34f, 0.66f };
	TILE_REGION TREE = { 0.66f, 1.0f };

	std::vector<int> NATURE_INDICIES;
	std::vector<SDL_Vertex> NATURE_VERTEXES;


	std::vector<int> HUMAN_INDICIES;
	std::vector<SDL_Vertex> HUMAN_VERTEXES;

	float humanTEX_W, humanTEX_H;

	SDL_Window* window = nullptr;
	SDL_Renderer* REND = nullptr;
	SDL_Texture* humanTEXTURE;
	SDL_Texture* groundTEXTURE;

	//TO DO
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

