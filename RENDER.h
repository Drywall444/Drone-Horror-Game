#pragma once
#ifndef RENDER_H
#define RENDER_H
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"
#include "INPUT.h"

class RENDER
{
public:
	std::vector<int> INDICIES;
	std::vector<SDL_Vertex> VERTEXES;

	int WINDOW_W = 0;
	int WINDOW_H = 0;
	int WINDOW_CENTER_X = 0;
	int WINDOW_CENTER_Y = 0;

	float texW, texH;

	SDL_Window* window = nullptr;
	SDL_Renderer* REND = nullptr;
	SDL_Texture* humanTEXTURE;

	//TO DO
	void renderSPRITES_ON_SCREEN(entt::registry& spriteREGISTER, CAMERA camera);
	void initializeRENDER(int windowH, int windowW);


	//OPTIMIZATION
	void emplaceSPRITES_ON_SCREEN();

};

#endif // !RENDER_H

