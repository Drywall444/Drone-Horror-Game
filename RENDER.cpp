#include "RENDER.h"
#include <SDL3_image/SDL_image.h>

//RENDER

std::vector<int> returnINDICIES(int size)
{
	std::vector<int> final;
	final.reserve(size * 6);
	for (int i = 0;i < size; i++)
	{
		int base = i * 4;
		final.push_back(base + 0); // TL
		final.push_back(base + 1); // TR
		final.push_back(base + 3); // BL
		final.push_back(base + 1); // TR
		final.push_back(base + 2); // BR
		final.push_back(base + 3); // BL
	}
	return final;
}

void RENDER::initializeRENDER()
{
	if (!SDL_CreateWindowAndRenderer("Drone Hunt", IN.WINDOW_W, IN.WINDOW_H, 0, &window, &REND)) {
		std::cerr << "Window/Renderer Creation Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	//HUMAN TEXTURE LOADED
	humanTEXTURE = IMG_LoadTexture(REND, "DATA/SPRITE.png");
	if (!humanTEXTURE) { SDL_Log("Failed to load human texture"); }
	SDL_SetTextureBlendMode(humanTEXTURE, SDL_BLENDMODE_BLEND);

	//GROUND TEXTURES
	groundTEXTURE = IMG_LoadTexture(REND, "DATA/GROUND.png");
	if (!groundTEXTURE) { SDL_Log("Failed to load nature texture"); }
	SDL_SetTextureBlendMode(groundTEXTURE, SDL_BLENDMODE_NONE);
	//SDL_GetTextureSize(groundTEXTURE, &natureTEX_W, &natureTEX_H);

	//SDL STUFF

	//PIXEL PERFECT
	SDL_SetTextureScaleMode(groundTEXTURE, SDL_SCALEMODE_NEAREST);
	//SDL_SetTextureScaleMode(humanTEXTURE, SDL_SCALEMODE_NEAREST);

	SDL_SetRenderDrawColor(REND, 255, 255, 255, 255);//White
	SDL_SetWindowFullscreenMode(window, NULL); //WHY TF DOES TWO CALLS TO WINDOW FULL SCREEN WORK, FUCK MY LIFE
	SDL_SetWindowFullscreen(window, true);
	IN.getWINDOWSIZE(window);
	//SDL_SetRenderVSync(REND, 1);


	std::cout << "Renderer Success!/n";
}

void RENDER::renderSPRITES_ON_SCREEN(entt::registry& spriteREGISTER, CAMERA camera)
{
	HUMAN_VERTEXES.clear();
	NATURE_VERTEXES.clear();

	//Get new offset based off input
	IN.getCAM_OFFSET();

	auto totalSPRITES = spriteREGISTER.view<spriteOBJECT>();
	HUMAN_VERTEXES.reserve(totalSPRITES.size() * 4);

	if (totalSPRITES.size() != lastSPRITE_NUM) //only sort if num of sprites has changed
	{
		sorted.assign(totalSPRITES.begin(), totalSPRITES.end());
		std::sort(sorted.begin(), sorted.end(), [&](entt::entity a, entt::entity b) { //Z sort - need to add y tie breaker
			return spriteREGISTER.get<spriteOBJECT>(a).spriteLOCATION.z
				< spriteREGISTER.get<spriteOBJECT>(b).spriteLOCATION.z;
			});

		lastSPRITE_NUM = totalSPRITES.size();
	}

	for (auto& sprite : sorted)
	{
		auto& curSPRITE = totalSPRITES.get<spriteOBJECT>(sprite);
		if (!spriteREGISTER.valid(sprite)) continue;

			float halfW = curSPRITE.texW * 0.5f;
			float halfH = curSPRITE.texH * 0.5f;

			//TRS: Translate, Rotate, Scale

			float x = curSPRITE.spriteLOCATION.POS.x;
			float y = curSPRITE.spriteLOCATION.POS.y;

			if ((x - halfW) * camera.zoom + camera.offSET.x > IN.WINDOW_W || (x + halfW) * camera.zoom + camera.offSET.x < 0.0) { continue; } //Cull Sprites
			if ((y - halfH) * camera.zoom + camera.offSET.y > IN.WINDOW_H || (y + halfH) * camera.zoom + camera.offSET.y < 0.0) { continue; }

			SDL_FPoint Point1 = rotatePOINT({ -halfW, -halfH }, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point2 = rotatePOINT({ halfW, -halfH }, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point3 = rotatePOINT({ halfW,  halfH }, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point4 = rotatePOINT({ -halfW,  halfH }, curSPRITE.spriteLOCATION.ROT);

			Point1 = { Point1.x + x, Point1.y + y };
			Point2 = { Point2.x + x, Point2.y + y };
			Point3 = { Point3.x + x, Point3.y + y };
			Point4 = { Point4.x + x, Point4.y + y };

			SDL_FColor color = { 1, 1, 1, 1 };
			if (curSPRITE.hidden)
			{
				color.a = 0.35;
			}

			if (sprite == IN.curSELECTED_SOLDIER)
			{
				color = { 0.25, 0.25, 1, 1 };
			}

			UV_REGION curTEX_REGION = curSPRITE.TYPE;

			HUMAN_VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMIN, curTEX_REGION.vMIN} });
			HUMAN_VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMAX, curTEX_REGION.vMIN} });
			HUMAN_VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMAX, curTEX_REGION.vMAX} });
			HUMAN_VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMIN, curTEX_REGION.vMAX} });
	}
	auto totalTILE = spriteREGISTER.view<TILE>();
	NATURE_VERTEXES.reserve(totalTILE.size() * 4);

	//TILES
	for (auto& tile : totalTILE)
	{
		auto& curTILE = totalTILE.get<TILE>(tile);

			float halfW = SM.natureTEX_W * 0.5f;
			float halfH = SM.natureTEX_H * 0.5f;

			float x = curTILE.pos.x;
			float y = curTILE.pos.y;

			if ((x - halfW) * camera.zoom + camera.offSET.x > IN.WINDOW_W || (x + halfW) * camera.zoom + camera.offSET.x < 0.0) { continue; }
			if ((y - halfH) * camera.zoom + camera.offSET.y > IN.WINDOW_H || (y + halfH) * camera.zoom + camera.offSET.y < 0.0) { continue; }

			SDL_FPoint Point1 = { -halfW, -halfH }; 
			SDL_FPoint Point2 = { halfW, -halfH };
			SDL_FPoint Point3 = { halfW, halfH };
			SDL_FPoint Point4 = { -halfW,  halfH };

			Point1 = { Point1.x + x, Point1.y + y };
			Point2 = { Point2.x + x, Point2.y + y };
			Point3 = { Point3.x + x, Point3.y + y };
			Point4 = { Point4.x + x, Point4.y + y };

			UV_REGION curTILE_REGION = curTILE.UV_TYPE;

			NATURE_VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMIN,0} });
			NATURE_VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMAX,0} });
			NATURE_VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMAX,1.0} });
			NATURE_VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMIN,1.0} });

	}

	//INDICIES
	int visibleNature = (int)NATURE_VERTEXES.size() / 4;
	int visibleHuman = (int)HUMAN_VERTEXES.size() / 4;

	if (visibleNature != lastNatureSize) {
		NATURE_INDICIES = returnINDICIES(visibleNature);
		lastNatureSize = visibleNature;
	}
	if (visibleHuman != lastHumanSize) {
		HUMAN_INDICIES = returnINDICIES(visibleHuman);
		lastHumanSize = visibleHuman;
	}

	SDL_RenderGeometry(REND, groundTEXTURE, NATURE_VERTEXES.data(), NATURE_VERTEXES.size(),
		NATURE_INDICIES.data(), (NATURE_VERTEXES.size() / 4) * 6);

	SDL_RenderGeometry(REND, humanTEXTURE, HUMAN_VERTEXES.data(), HUMAN_VERTEXES.size(),
		HUMAN_INDICIES.data(), (HUMAN_VERTEXES.size() / 4) * 6);


}