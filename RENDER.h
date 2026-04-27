#pragma once
#ifndef RENDER_H
#define RENDER_H
#define FNL_IMPL
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"
#include "INPUT.h"

class RENDER
{
public:
	RENDER(INPUT& I, SPRITE_MANAGER& sM) : IN(I),  SM(sM) {}


	//MAP SHIT
	float natureTEX_W = 64.0, natureTEX_H = 64.0;
	int MAP_W = 150;
	int MAP_H = 300;


	std::vector<int> NATURE_INDICIES;
	std::vector<SDL_Vertex> NATURE_VERTEXES;


	std::vector<int> HUMAN_INDICIES;
	std::vector<SDL_Vertex> HUMAN_VERTEXES;

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
	SPRITE_MANAGER& SM;
	INPUT& IN;

};

#endif // !RENDER_H

