#include "SPRITE.h"



void SPRITE_MANAGER::checkCOLLISIONS()
{
	//for each soldier
	//check tile and all neighbour tiles
	auto allCOLLISIONS = spriteREGISTER.view<hasCOLLISION>();
	for (auto& collision : allCOLLISIONS)
	{
		if (spriteREGISTER.all_of<isSTATIC>(collision)) { continue; }
		//if (spriteREGISTER.all_of<inCOVER>(collision)) { continue; }

		auto& cur_collisionINFO = spriteREGISTER.get<hasCOLLISION>(collision);
		auto& cur_spriteINFO = spriteREGISTER.get<spriteOBJECT>(collision);
		//for each collision 
		//find all neighbours
		int curIndex = cur_collisionINFO.curINDEX;
		std::vector<int> tileNUMS;
		tileNUMS.push_back(cur_collisionINFO.curINDEX); //Cur index sprite is in
		if (curIndex - MAP_W >= 0) { tileNUMS.push_back(curIndex - MAP_W); } // up
		if (curIndex + MAP_W < (int)worldTILES.size()) { tileNUMS.push_back(curIndex + MAP_W); } // down
		if (curIndex - 1 >= 0) { tileNUMS.push_back(curIndex - 1); }      // left
		if (curIndex + 1 < (int)worldTILES.size()) { tileNUMS.push_back(curIndex + 1); }      // right
		// diagonals too
		if (curIndex - MAP_W - 1 >= 0) { tileNUMS.push_back(curIndex - MAP_W - 1); }
		if (curIndex - MAP_W + 1 >= 0) { tileNUMS.push_back(curIndex - MAP_W + 1); }
		if (curIndex + MAP_W - 1 < (int)worldTILES.size()) { tileNUMS.push_back(curIndex + MAP_W - 1); }
		if (curIndex + MAP_W + 1 < (int)worldTILES.size()) { tileNUMS.push_back(curIndex + MAP_W + 1); }

		for (int i = 0;i < tileNUMS.size();i++)
		{
			entt::entity tile = worldTILES[tileNUMS[i]];
			auto& curTILE_STRUCT = spriteREGISTER.get<TILE>(tile);
			for (auto& spriteWITHIN_TILE : curTILE_STRUCT.spritesWITHIN)
			{
				if (spriteWITHIN_TILE == collision) { continue; } //skip self
				auto& other_spriteINFO = spriteREGISTER.get<spriteOBJECT>(spriteWITHIN_TILE);
				float dist = distanceTO_POINT(cur_spriteINFO.spriteLOCATION.POS, other_spriteINFO.spriteLOCATION.POS);

				if (dist > 0.0f && dist < 40.0f)
				{
					ROTATION pushDIR = directionTO_POINT(other_spriteINFO.spriteLOCATION.POS, cur_spriteINFO.spriteLOCATION.POS);

					float overlap = 40.0f - dist; //amount we are overlapping with the object
					float pushSTRENGTH = 180.0f;

					cur_spriteINFO.spriteLOCATION.POS.x += pushDIR.cosR * overlap * pushSTRENGTH * DT;
					cur_spriteINFO.spriteLOCATION.POS.y += pushDIR.sinR * overlap * pushSTRENGTH * DT;
				}
			}
		}

	}
}

void SPRITE_MANAGER::assignCOLLISION()
{
	//For each soldier check POS and assign POS to register
	auto allCOLLIDABLES = spriteREGISTER.view<hasCOLLISION>();

	for (auto& collision : allCOLLIDABLES)
	{
		auto& collisionSTRUCT = spriteREGISTER.get<hasCOLLISION>(collision);
		int previousINDEX = collisionSTRUCT.curINDEX;

			//assign new index
			auto& curCOLLISION_SPRITE = spriteREGISTER.get<spriteOBJECT>(collision);
			int tileX = curCOLLISION_SPRITE.spriteLOCATION.POS.x / natureTEX_W;
			int tileY = curCOLLISION_SPRITE.spriteLOCATION.POS.y / natureTEX_H;

			int INDEX = (tileY * MAP_W) + tileX;
			//assign new index
			collisionSTRUCT.curINDEX = INDEX;

		if (INDEX <= 0 || INDEX >= (int)worldTILES.size()) { continue; } //we crash

		if (previousINDEX == INDEX) { continue; } //dont assign newINDEX if in same index

		//assign to tile
		entt::entity tile = worldTILES[collisionSTRUCT.curINDEX];
		auto& curTILE_STRUCT = spriteREGISTER.get<TILE>(tile);
		curTILE_STRUCT.spritesWITHIN.push_back(collision);

		if (previousINDEX == -1) { continue; } //previous not assigned


		if (!spriteREGISTER.all_of<isSTATIC>(collision)) //not static reassign or hasent been assigned
		{
			entt::entity prevTILE = worldTILES[previousINDEX];
			removeCOLLISION_SPRITE_FROM_TILE(collision, prevTILE);
		}
	}

}

void SPRITE_MANAGER::removeCOLLISION_SPRITE_FROM_TILE(entt::entity sprite, entt::entity tile)
{
	auto& prevTILE_STRUCT = spriteREGISTER.get<TILE>(tile);
	int i = 0;
	for (auto& cursprite : prevTILE_STRUCT.spritesWITHIN) //this crashes on start sometimes :/ - still crashes dont know why
	{
		if (cursprite == sprite)
		{
			prevTILE_STRUCT.spritesWITHIN.erase(prevTILE_STRUCT.spritesWITHIN.begin() + i);
			break;
		}
		i++;
	}
}