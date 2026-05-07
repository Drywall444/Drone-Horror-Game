#include "RENDER.h"
#include "FastNoiseLite.h"

//MAP CREATION - DO ON OWN
void RENDER::createMAP(SPRITE_MANAGER& sprites)
{
	// setup noise
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.frequency = 0.05f; 

	// allocate noise data
	float* noiseData = (float*)malloc(MAP_W * MAP_H * sizeof(float));
	noise.seed = randBETWEEN(0.0f, 1000.0f);
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
			if (value > 0.45f)
			{
				tileType = SM.GRASS_1;
			}
			else if (value > 0.3f)
			{
				tileType = SM.GRASS_2;
			}
			else
			{
				tileType = SM.WOODS_1;
				float randTREE = randBETWEEN(0.0f, 100.0);
				if (randTREE > 45.0f)
				{
					//create tree
					SDL_FPoint pos = { x * natureTEX_W, y * natureTEX_H };
					spawnTREE(pos);
				}
			}

			sprites.tileCREATE(tileType, { float(x * natureTEX_W), float(y * natureTEX_H) });
		}
	}

	free(noiseData);
}

entt::entity RENDER::spawnTREE(SDL_FPoint pos)
{
	//entt::entity treeTRUNK = SM.createSPRITE(pos, randROTATION(), 64, 64, 1.0);
	//auto& treeTRUNK_INFO = SM.spriteREGISTER.get<spriteOBJECT>(treeTRUNK);
	//treeTRUNK_INFO.TYPE = SM.TREE_TRUNK;

	auto allTREE = SM.spriteREGISTER.view<isTREE>();
	for (auto& tree : allTREE)
	{
		auto& curTREE_POS = SM.spriteREGISTER.get<spriteOBJECT>(tree).spriteLOCATION.POS;
		if (distanceTO_POINT(pos, curTREE_POS) < 120)
		{
			std::cout << "Tree Culled\n";
			return entt::null;
		}
	}

	entt::entity treeTOP = SM.createSPRITE(pos, randROTATION(), 128, 128, 10.0);
	SM.spriteREGISTER.emplace_or_replace<isTREE>(treeTOP);
	auto& treeTOP_INFO = SM.spriteREGISTER.get<spriteOBJECT>(treeTOP);
	treeTOP_INFO.TYPE = SM.TREE_TOP;

	return treeTOP;
}


void SPRITE_MANAGER::toggleTREE_TOPS()
{
	auto allTREE = spriteREGISTER.view<isTREE>();
	for (auto& tree : allTREE)
	{
		auto& curSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(tree);
		auto& curTREE_INFO = spriteREGISTER.get<isTREE>(tree);
		curTREE_INFO.treeTOP = !curTREE_INFO.treeTOP;
		if (curTREE_INFO.treeTOP)
		{
			curSPRITE_INFO.TYPE = TREE_TOP;
			curSPRITE_INFO.texW = 150;
			curSPRITE_INFO.texH = 150;
		}
		else {
			curSPRITE_INFO.TYPE = TREE_TRUNK;
			curSPRITE_INFO.texW = 32;
			curSPRITE_INFO.texH = 32;
		}
	}
}