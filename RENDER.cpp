#include "RENDER.h"
#include <SDL3_image/SDL_image.h>
#include "SPRITE.h"

void RENDER::initializeRENDER(int windowW, int windowH)
{
	WINDOW_W = windowW;
	WINDOW_H = windowH;
	WINDOW_CENTER_X = windowW / 2;
	WINDOW_CENTER_Y = windowH / 2;;

	if (!SDL_CreateWindowAndRenderer("Drone Hunt", windowW, windowH, 0, &window, &REND)) {
		std::cerr << "Window/Renderer Creation Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	//HUMAN TEXTURE LOADED
	humanTEXTURE = IMG_LoadTexture(REND, "DATA/PLAYER.png");
	if (!humanTEXTURE) { SDL_Log("Failed to load human texture"); }
	SDL_SetTextureBlendMode(humanTEXTURE, SDL_BLENDMODE_NONE);

	SDL_GetTextureSize(humanTEXTURE, &texW, &texH);

	SDL_SetRenderDrawColor(REND, 255, 255, 255, 255);//White

	std::cout << "Renderer Success!/n";
}

void RENDER::renderSPRITES_ON_SCREEN(entt::registry& spriteREGISTER, CAMERA camera)
{
	VERTEXES.clear();
	INDICIES.clear();

	float camOffX = WINDOW_CENTER_X - camera.x * camera.zoom;
	float camOffY = WINDOW_CENTER_Y - camera.y * camera.zoom;

	auto totalSPRITES = spriteREGISTER.view<spriteOBJECT>();

	for (auto& sprite : totalSPRITES)
	{
		auto& curSPRITE = totalSPRITES.get<spriteOBJECT>(sprite);
		if (curSPRITE.textureSHEET_NUM == 0)//USE HUMAN TEXTURE
		{

			//Get texture width and height from intizilation and then get top left corner, top right, bottom left, bottom right
			//GET ROTATION AND APPLY TO ALL POINTS

			float x = curSPRITE.spriteLOCATION.POS.x;
			float y = curSPRITE.spriteLOCATION.POS.y;

			SDL_FPoint Point1 = { x, y };
			SDL_FPoint Point2 = { x + texW, y };
			SDL_FPoint Point3 = { x + texW, y + texH };
			SDL_FPoint Point4 = { x, y + texH };

			VERTEXES.push_back({ { Point1.x * camera.zoom + camOffX,  Point1.y * camera.zoom + camOffY }, {1,1,1,1}, {0,0} });
			VERTEXES.push_back({ { Point2.x * camera.zoom + camOffX,  Point2.y * camera.zoom + camOffY }, {1,1,1,1}, {1,0} });
			VERTEXES.push_back({ { Point3.x * camera.zoom + camOffX,  Point3.y * camera.zoom + camOffY }, {1,1,1,1}, {1,1} });
			VERTEXES.push_back({ { Point4.x * camera.zoom + camOffX,  Point4.y * camera.zoom + camOffY }, {1,1,1,1}, {0,1} });
		}
	}
	INDICIES.reserve(totalSPRITES.size() * 6);

	for (int i = 0;i < totalSPRITES.size(); i++)
	{
		int base = i * 4;
		INDICIES.push_back(base + 0); // TL
		INDICIES.push_back(base + 1); // TR
		INDICIES.push_back(base + 3); // BL
		INDICIES.push_back(base + 1); // TR
		INDICIES.push_back(base + 2); // BR
		INDICIES.push_back(base + 3); // BL
	}
	SDL_RenderGeometry(REND, humanTEXTURE, VERTEXES.data(), VERTEXES.size(),
		INDICIES.data(), (VERTEXES.size() / 4) * 6);
}