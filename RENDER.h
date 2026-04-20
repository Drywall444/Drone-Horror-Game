#pragma once
#ifndef RENDER_H
#define RENDER_H
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


	std::vector<int> INDICIES;
	std::vector<SDL_Vertex> VERTEXES;

	float texW, texH;

	SDL_Window* window = nullptr;
	SDL_Renderer* REND = nullptr;
	SDL_Texture* humanTEXTURE;

	//TO DO
	void renderSPRITES_ON_SCREEN(entt::registry& spriteREGISTER, CAMERA camera);
	void initializeRENDER();


	//OPTIMIZATION
	void emplaceSPRITES_ON_SCREEN();

private:
	INPUT& IN;

};

#endif // !RENDER_H

