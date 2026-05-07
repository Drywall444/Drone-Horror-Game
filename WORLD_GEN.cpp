#include "RENDER.h"
#include "FastNoiseLite.h"

//MAP CREATION - DO ON OWN
void SPRITE_MANAGER::createMAP(SPRITE_MANAGER& sprites)
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
	natureTYPE_TILE tileTYPE;
	for (int y = 0; y < MAP_H; y++)
	{
		for (int x = 0; x < MAP_W; x++)
		{
			float value = (noiseData[index++] + 1.0f) * 0.5f;

			UV_REGION tileType;
			if (value > 0.45f)
			{
				tileType = GRASS_1;
				tileTYPE = TYPE_GRASS1;
			}
			else if (value > 0.3f)
			{
				tileType = GRASS_2;
				tileTYPE = TYPE_GRASS2;
			}
			else
			{
				tileTYPE = TYPE_WOODS1;
				tileType = WOODS_1;
				float randTREE = randBETWEEN(0.0f, 100.0);
				if (randTREE > 45.0f)
				{
					//create tree
					SDL_FPoint pos = { x * natureTEX_W, y * natureTEX_H };
					spawnTREE(pos);
				}
			}
			SDL_FPoint newPOS = { float(x * natureTEX_W), float(y * natureTEX_H) };

			entt::entity newTILE = tileCREATE(tileType, newPOS, tileTYPE);
			worldTILES.push_back(newTILE);
		}
	}

	free(noiseData);
}

entt::entity SPRITE_MANAGER::spawnTREE(SDL_FPoint pos)
{

	auto allTREE = spriteREGISTER.view<isTREE>();
	for (auto& tree : allTREE)
	{
		auto& curTREE_POS = spriteREGISTER.get<spriteOBJECT>(tree).spriteLOCATION.POS;
		if (distanceTO_POINT(pos, curTREE_POS) < 120)
		{
			std::cout << "Tree Culled\n";
			return entt::null;
		}
	}

	entt::entity treeTOP = createSPRITE(pos, randROTATION(), 128, 128, 10.0);
	spriteREGISTER.emplace_or_replace<isTREE>(treeTOP);
	auto& treeTOP_INFO = spriteREGISTER.get<spriteOBJECT>(treeTOP);
	treeTOP_INFO.TYPE = TREE_TOP;

	spriteREGISTER.emplace<hasCOLLISION>(treeTOP);
	spriteREGISTER.emplace<isSTATIC>(treeTOP);

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