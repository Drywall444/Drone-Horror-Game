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

			natureTYPE_TILE tileType;
			if (value > 0.8f) 
			{ 
				tileType = TYPE_GRASS; 
			}
			else if (value > 0.3f) 
			{ 
				tileType = TYPE_GRASS1;
			}
			else { tileType = TYPE_WOODS; }

			sprites.tileCREATE(NATURE, tileType, { float(x * natureTEX_W), float(y * natureTEX_H) });
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
	humanTEXTURE = IMG_LoadTexture(REND, "DATA/SPRITE.png");
	if (!humanTEXTURE) { SDL_Log("Failed to load human texture"); }
	SDL_SetTextureBlendMode(humanTEXTURE, SDL_BLENDMODE_BLEND);
	humanTEX_W = 64;
	humanTEX_H = 128;

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
	SDL_SetRenderVSync(REND, 1);

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

	for (auto& sprite : totalSPRITES)
	{
		auto& curSPRITE = totalSPRITES.get<spriteOBJECT>(sprite);
		if (curSPRITE.textureSHEET_NUM == HUMAN)//USE HUMAN TEXTURE
		{
			float halfW = humanTEX_W * 0.5f;
			float halfH = humanTEX_H * 0.5f;

			//Get texture width and height from intizilation and then get top left corner, top right, bottom left, bottom right
			//GET ROTATION AND APPLY TO ALL POINTS

			float x = curSPRITE.spriteLOCATION.POS.x;
			float y = curSPRITE.spriteLOCATION.POS.y;

			SDL_FPoint center = { x + halfW, y + halfH };

			SDL_FPoint Point1 = rotatePOINT({ x, y }, center, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point2 = rotatePOINT({ x + humanTEX_W, y }, center, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point3 = rotatePOINT({ x + humanTEX_W, y + humanTEX_H }, center, curSPRITE.spriteLOCATION.ROT);
			SDL_FPoint Point4 = rotatePOINT({ x, y + humanTEX_H }, center, curSPRITE.spriteLOCATION.ROT);

			SDL_FColor color = { 1, 1, 1, 1 };

			if (sprite == IN.curSELECTED_SOLDIER)
			{
				color = { 1, 1, 0, 1 };
			}

			TILE_REGION curTEX_REGION;
			if (curSPRITE.TYPE == TYPE_SOLDIER_STANDING) { curTEX_REGION = SOLDIER_STANDING; }
			else if (curSPRITE.TYPE == TYPE_SOLDIER_SHOOTING) { curTEX_REGION = SOLDIER_SHOOTING; }
			else if (curSPRITE.TYPE == TYPE_SOLDIER_ENEMY_STANDING) { curTEX_REGION = SOLDIER_STANDING; }
			else if (curSPRITE.TYPE == DEAD_1) { curTEX_REGION = SOLDIER_DEAD_1; }
			else if (curSPRITE.TYPE == DEAD_2) { curTEX_REGION = SOLDIER_DEAD_2; }

			HUMAN_VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMIN,0} });
			HUMAN_VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMAX,0} });
			HUMAN_VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMAX,1} });
			HUMAN_VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, color, {curTEX_REGION.uMIN,1} });
		}
	}
	auto totalTILE = spriteREGISTER.view<TILE>();

	for (auto& tile : totalTILE)
	{

		auto& curTILE = totalTILE.get<TILE>(tile);

		if (curTILE.textureSHEET_NUM == NATURE)
		{
			float halfW = natureTEX_W * 0.5f;
			float halfH = natureTEX_H * 0.5f;

			float x = curTILE.pos.x;
			float y = curTILE.pos.y;

			SDL_FPoint center = { x + halfW, y + halfH };

			SDL_FPoint Point1 = { x, y };
			SDL_FPoint Point2 = { x + natureTEX_W, y };
			SDL_FPoint Point3 = { x + natureTEX_W, y + natureTEX_H };
			SDL_FPoint Point4 = { x, y + natureTEX_H };

			TILE_REGION curTILE_REGION;
			if (curTILE.TYPE == TYPE_GRASS) { curTILE_REGION = GRASS_1; }
			else if (curTILE.TYPE == TYPE_GRASS1) { curTILE_REGION = GRASS_2; }
			else if (curTILE.TYPE == TYPE_WOODS) { curTILE_REGION = WOODS_1; }

			NATURE_VERTEXES.push_back({ { Point1.x * camera.zoom + camera.offSET.x,  Point1.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMIN,0} });
			NATURE_VERTEXES.push_back({ { Point2.x * camera.zoom + camera.offSET.x,  Point2.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMAX,0} });
			NATURE_VERTEXES.push_back({ { Point3.x * camera.zoom + camera.offSET.x,  Point3.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMAX,1} });
			NATURE_VERTEXES.push_back({ { Point4.x * camera.zoom + camera.offSET.x,  Point4.y * camera.zoom + camera.offSET.y }, {1,1,1,1}, {curTILE_REGION.uMIN,1} });
		}

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