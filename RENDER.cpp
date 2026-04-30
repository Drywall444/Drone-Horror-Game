#include "RENDER.h"
#include <SDL3_image/SDL_image.h>
#include "FastNoiseLite.h"




//MAP CREATION - DO ON OWN
void RENDER::createMAP(SPRITE_MANAGER& sprites)
{
	// setup noise
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.frequency = 0.03f;  // tweak for region size

	// allocate noise data
	float* noiseData = (float*)malloc(MAP_W * MAP_H * sizeof(float));
	int index = 0;

	for (int y = 0; y < MAP_H; y++)
	{
		for (int x = 0; x < MAP_W; x++)
		{
			noiseData[index++] = fnlGetNoise2D(&noise, x, y);
		}
	}

	// use noise data to place tiles
	index = 0;
	for (int y = 0; y < MAP_H; y++)
	{
		for (int x = 0; x < MAP_W; x++)
		{
			float value = (noiseData[index++] + 1.0f) * 0.5f;

			UV_REGION tileType;
			if (value > 0.8f) 
			{ 
				tileType = SM.GRASS_1;
			}
			else if (value > 0.3f) 
			{ 
				tileType = SM.GRASS_2;
			}
			else { tileType = SM.WOODS_1; }

			sprites.tileCREATE(tileType, { float(x * natureTEX_W), float(y * natureTEX_H) });
		}
	}
}

//RENDER

std::vector<int> returnINDICIES(int size)
{
	std::vector<int> final;

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
	HUMAN_INDICIES.clear();
	NATURE_VERTEXES.clear();
	NATURE_INDICIES.clear();

	//Get new offset based off input
	IN.getCAM_OFFSET();

	auto totalSPRITES = spriteREGISTER.view<spriteOBJECT>();

	// Collect entity handles
	std::vector<entt::entity> sorted(totalSPRITES.begin(), totalSPRITES.end());

	std::sort(sorted.begin(), sorted.end(), [&](entt::entity a, entt::entity b) { //Z sort
		return spriteREGISTER.get<spriteOBJECT>(a).spriteLOCATION.z
			< spriteREGISTER.get<spriteOBJECT>(b).spriteLOCATION.z;
		});

	for (auto& sprite : sorted)
	{
		auto& curSPRITE = totalSPRITES.get<spriteOBJECT>(sprite);

			float halfW = curSPRITE.texW * 0.5f;
			float halfH = curSPRITE.texH * 0.5f;

			//Get texture width and height from intizilation and then get top left corner, top right, bottom left, bottom right
			//GET ROTATION AND APPLY TO ALL POINTS

			//TRS: Translate, Rotate, Scale

			float x = curSPRITE.spriteLOCATION.POS.x;
			float y = curSPRITE.spriteLOCATION.POS.y;

			SDL_FPoint Point1 = rotatePOINT({ -halfW, -halfH }, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point2 = rotatePOINT({ halfW, -halfH }, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point3 = rotatePOINT({ halfW,  halfH }, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point4 = rotatePOINT({ -halfW,  halfH }, curSPRITE.spriteLOCATION.ROT);

			Point1 = { Point1.x + x, Point1.y + y };
			Point2 = { Point2.x + x, Point2.y + y };
			Point3 = { Point3.x + x, Point3.y + y };
			Point4 = { Point4.x + x, Point4.y + y };

			SDL_FColor color = { 1, 1, 1, 1 };

			if (sprite == IN.curSELECTED_SOLDIER)
			{
				color = { 1, 1, 0, 1 };
			}

			UV_REGION curTEX_REGION = curSPRITE.TYPE;

			HUMAN_VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMIN, curTEX_REGION.vMIN} });
			HUMAN_VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMAX, curTEX_REGION.vMIN} });
			HUMAN_VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMAX, curTEX_REGION.vMAX} });
			HUMAN_VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMIN, curTEX_REGION.vMAX} });
	}
	auto totalTILE = spriteREGISTER.view<TILE>();

	//TILES
	for (auto& tile : totalTILE)
	{
		auto& curTILE = totalTILE.get<TILE>(tile);

			float halfW = natureTEX_W * 0.5f;
			float halfH = natureTEX_H * 0.5f;

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

			UV_REGION curTILE_REGION = curTILE.TYPE;

			NATURE_VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMIN,0} });
			NATURE_VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMAX,0} });
			NATURE_VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMAX,1.0} });
			NATURE_VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMIN,1.0} });

	}

	//INDICIES
	auto totalNATURE = spriteREGISTER.view<TILE>();

	HUMAN_INDICIES = returnINDICIES(totalSPRITES.size());
	NATURE_INDICIES = returnINDICIES(totalNATURE.size());

	SDL_RenderGeometry(REND, groundTEXTURE, NATURE_VERTEXES.data(), NATURE_VERTEXES.size(),
		NATURE_INDICIES.data(), (NATURE_VERTEXES.size() / 4) * 6);

	SDL_RenderGeometry(REND, humanTEXTURE, HUMAN_VERTEXES.data(), HUMAN_VERTEXES.size(),
		HUMAN_INDICIES.data(), (HUMAN_VERTEXES.size() / 4) * 6);


}