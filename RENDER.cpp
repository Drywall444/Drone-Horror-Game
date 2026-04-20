#include "RENDER.h"
#include <SDL3_image/SDL_image.h>

//RENDER

SDL_FPoint rotatePOINT(SDL_FPoint pos, SDL_FPoint centerPOINT, ROTATION rot)
{

	float localX = pos.x - centerPOINT.x;
	float localY = pos.y - centerPOINT.y;

	SDL_FPoint FINAL = { (localX * rot.cosR - localY * rot.sinR) + centerPOINT.x, (localX * rot.sinR + localY * rot.cosR) + centerPOINT.y };

	return FINAL;
}

void RENDER::initializeRENDER()
{
	if (!SDL_CreateWindowAndRenderer("Drone Hunt", IN.WINDOW_W, IN.WINDOW_H, 0, &window, &REND)) {
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

	//Get new offset based off input
	IN.getCAM_OFFSET();

	auto totalSPRITES = spriteREGISTER.view<spriteOBJECT>();

	for (auto& sprite : totalSPRITES)
	{
		auto& curSPRITE = totalSPRITES.get<spriteOBJECT>(sprite);
		if (curSPRITE.textureSHEET_NUM == 0)//USE HUMAN TEXTURE
		{
			float halfW = texW * 0.5f;
			float halfH = texH * 0.5f;

			//Get texture width and height from intizilation and then get top left corner, top right, bottom left, bottom right
			//GET ROTATION AND APPLY TO ALL POINTS

			float x = curSPRITE.spriteLOCATION.POS.x;
			float y = curSPRITE.spriteLOCATION.POS.y;

			SDL_FPoint center = { x + halfW, y + halfH };

			SDL_FPoint Point1 = rotatePOINT({ x, y }, center, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point2 = rotatePOINT({ x + texW, y }, center, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point3 = rotatePOINT({ x + texW, y + texH }, center, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point4 = rotatePOINT({ x, y + texH }, center, curSPRITE.spriteLOCATION.ROT);

			VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {0,0} });
			VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {1,0} });
			VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {1,1} });
			VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {0,1} });
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