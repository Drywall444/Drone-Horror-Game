#include "SPRITE.h"


//LOS
void SPRITE_MANAGER::checkLOS(entt::entity soldier, TEAM teaminfo) //OVERHAUL
{
	if (spriteREGISTER.all_of<inDUGOUT>(soldier))
	{
		if (spriteREGISTER.all_of<hasTARGET>(soldier))
		{
			spriteREGISTER.remove<hasTARGET>(soldier);
		}
		return;
	}
	auto allSOLDIER = spriteREGISTER.view<soldierOBJECT>(entt::exclude<inDUGOUT>); //exclude those in dugout

	auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& curSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	bool hasTARGET_BOOL = false;
	bool removeTARGET = false;
	//check all targets not friendly, if anything closer replace target with that, if we find nothing remove hasTARGET
	bool noTARGETS_FOUND = true;
	entt::entity foundTARGET;
	float targetDISTANCE;

	for (auto& s : allSOLDIER)
	{
		TEAMS curSOLDIER_TEAM = spriteREGISTER.get<TEAM>(s).soldierTEAM;
		if (s == soldier) { continue; } //self
		if (teaminfo.soldierTEAM == curSOLDIER_TEAM) { continue; } //skip friendlys
		auto& enemySPRITE = spriteREGISTER.get<spriteOBJECT>(s);
		auto& enemySOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(s);
		float losRANGE = 3000.0;

		float dist = distanceTO_POINT_SQ(spriteINFO.spriteLOCATION.POS, enemySPRITE.spriteLOCATION.POS);
		float estimateTILE = (losRANGE / 100.0) * natureTEX_W;
		if (dist > estimateTILE * estimateTILE) { continue; } // early exit


		//NEW SHIT
		auto& collider = spriteREGISTER.get<hasCOLLISION>(soldier);
		auto& enemyCOLLIDER = spriteREGISTER.get<hasCOLLISION>(s);
		std::vector<int> LOS_TILE = bresenLINE(collider.curINDEX, enemyCOLLIDER.curINDEX);
		float losLEFT = losRANGE;
		bool OUT_OF_LOS = false;

		for (int i = 0;i < LOS_TILE.size();i++)
		{
			auto& curTILE = spriteREGISTER.get<TILE>(worldTILES[LOS_TILE[i]]);
			if (curTILE.TYPE == TYPE_WOODS1)
			{
				losLEFT -= 500;
			}
			else {
				losLEFT -= 100;
			}

			if (losLEFT <= 0.0f)
			{
				//out of LOS, return
				OUT_OF_LOS = true;
				break;
			}

		}
		if (OUT_OF_LOS) { continue; }

			if (noTARGETS_FOUND)
			{
				noTARGETS_FOUND = false;
				foundTARGET = s;
				targetDISTANCE = dist;
			}
			else if (targetDISTANCE > dist) //if found closer target
			{
				foundTARGET = s;
				targetDISTANCE = dist;
			}
	}

	if (!noTARGETS_FOUND) //we found a target
	{
		if (!spriteREGISTER.valid(foundTARGET)) { return; } //if target not valid dont add
		spriteREGISTER.emplace_or_replace<hasTARGET>(soldier, foundTARGET, false);
	}
	else if (spriteREGISTER.all_of<hasTARGET>(soldier)) { //if we havent found a target and we have a target remove it
		spriteREGISTER.remove<hasTARGET>(soldier);
	}

}


std::vector<int> SPRITE_MANAGER::bresenLINE(int startTILE, int endTILE)
{
	std::vector<int> LINE;

	if (startTILE > endTILE) //always trace high to low
		std::swap(startTILE, endTILE);

	int x1 = startTILE % MAP_W;
	int x2 = endTILE % MAP_W;
	int y1 = (startTILE - x1) / MAP_W;
	int y2 = (endTILE - x2) / MAP_W;

	int dX = abs(x2 - x1);
	int dY = abs(y2 - y1);

	int x = x1;
	int y = y1;

	int sX = (x2 > x1) ? 1 : -1;
	int sY = (y2 > y1) ? 1 : -1;

	//first point, is starting pos

	if (dX >= dY) //slope less than or equal to 1
	{
		int p = (2 * dY) - dX;
		while (x != x2)
		{
			if (p < 0)
			{
				x += sX; //X step
				LINE.push_back(XY_TO_tileNUM(x, y));
				p = p + 2 * dY;
			}
			else {
				x += sX;
				y += sY;
				LINE.push_back(XY_TO_tileNUM(x, y));
				p = p + 2 * dY - 2 * dX;
			}
		}

	}
	else //greater than or equal to 1
	{
		int p = (2 * dX) - dY;
		while (y != y2)
		{
			if (p < 0)
			{
				y += sY;
				LINE.push_back(XY_TO_tileNUM(x, y));
				p = p + 2 * dX;
			}
			else {
				x += sX;
				y += sY;
				LINE.push_back(XY_TO_tileNUM(x, y));
				p = p + 2 * dX - 2 * dY;
			}
		}
	}

	return LINE;

}

int SPRITE_MANAGER::XY_TO_tileNUM(int x, int y)
{
	int tileNUM = (y * MAP_W) + x;

	return tileNUM;
}