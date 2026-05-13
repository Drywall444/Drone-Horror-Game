#include "SPRITE.h"

MOVING SPRITE_MANAGER::newMOVEMENT(float speed, ROTATION dirTO, SDL_FPoint targetLOC)
{
	float dx = dirTO.cosR * speed;
	float dy = dirTO.sinR * speed;

	MOVING newMOVING_ORDER;
	newMOVING_ORDER.dX = dx;
	newMOVING_ORDER.dY = dy;
	newMOVING_ORDER.waypoints.push_back({ targetLOC, {0.0, 0.0} }); //rotation meaingless for now
	newMOVING_ORDER.movementSPEED = speed;

	return newMOVING_ORDER;
}

bool SPRITE_MANAGER::hasARRIVED_AT_POINT(SDL_FPoint spriteCUR_POS, MOVING& soldierMOVING_INFO)
{
	LOCATION curWAYPOINT = soldierMOVING_INFO.waypoints.back(); //Get current waypoint
	float pX = curWAYPOINT.POS.x - spriteCUR_POS.x;
	float pY = curWAYPOINT.POS.y - spriteCUR_POS.y;

	float DOT = (soldierMOVING_INFO.dX * pX) + (soldierMOVING_INFO.dY * pY);
	if (DOT < 0.0) { soldierMOVING_INFO.waypoints.pop_back(); return true; } //delete last waypoint if arrived
	else { return false; }
}

void SPRITE_MANAGER::moveSPRITES()
{
	auto movingSPRITES = spriteREGISTER.view<MOVING>();

	for (auto& sprite : movingSPRITES)
	{
		auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(sprite);
		auto& soldierMOVING_INFO = spriteREGISTER.get<MOVING>(sprite);

		if (hasARRIVED_AT_POINT(spriteINFO.spriteLOCATION.POS, soldierMOVING_INFO)) //If we arrived at 
		{
			if (soldierMOVING_INFO.waypoints.empty()) //Last Waypoint
			{
				if (soldierMOVING_INFO.destroyAT_TARGET)
				{
					toDESTROY.push_back(sprite); //DESTROY AND RETURN;
				}
				if (spriteREGISTER.all_of<ORDER_TO_BUILDING>(sprite)) //If this sprite has been ordered to a building, arrived at the point and the point is the last point
				{
					soldierENTERED_BUILDING(sprite, spriteREGISTER.get<ORDER_TO_BUILDING>(sprite).building);
				}
				else if (spriteREGISTER.all_of<ORDER_TO_POINT>(sprite))
				{
					spriteREGISTER.emplace_or_replace<IDLE>(sprite); //set back to idle
					//do sum here
				}
				spriteREGISTER.remove<MOVING>(sprite);
				continue;
			}
			else //If we have a waypoint stop here for the wait time
			{
				soldierMOVING_INFO.stopped = true;
			}
		} 
		else 
		{
			if (!soldierMOVING_INFO.stopped) //If not stopped we can move
			{
				//MOVE
				SDL_FPoint pos = spriteINFO.spriteLOCATION.POS;
				pos.x += soldierMOVING_INFO.dX * DT;
				pos.y += soldierMOVING_INFO.dY * DT;
				spriteINFO.spriteLOCATION.POS = pos;
			}
			else if (soldierMOVING_INFO.cur_waitTIME_AT_WAYPOINT >= 0.0)
			{
				soldierMOVING_INFO.cur_waitTIME_AT_WAYPOINT -= DT;

			}
			else {
				soldierMOVING_INFO.cur_waitTIME_AT_WAYPOINT = soldierMOVING_INFO.waitTIME_AT_WAYPOINT;
				soldierMOVING_INFO.stopped = false; //start moving again
			}

		}

	}

}

void SPRITE_MANAGER::ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS)
{
	auto& selcSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& selcSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	spriteREGISTER.remove<IDLE>(soldier);

	ROTATION direction = directionTO_POINT(selcSOLDIER.spriteLOCATION.POS, globalPOS); //ALWAYS USE un roated direction
	MOVING newMOVING_ORDER = newMOVEMENT(selcSOLDIER_INFO.speed, direction, globalPOS);
	selcSOLDIER.spriteLOCATION.ROT = rotationTO_POINT(selcSOLDIER.spriteLOCATION.POS, globalPOS);

	spriteREGISTER.emplace_or_replace<MOVING>(soldier, newMOVING_ORDER); //if already ordered delete previous and replace, god i love entt
	if (spriteREGISTER.all_of<hasTARGET>(soldier))
	{
		shootAND_MOVE(soldier);
	}
}

void SPRITE_MANAGER::shootAND_MOVE(entt::entity soldier) //LOOK OVER AND REWRITE
{
	auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& soldierSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);
	auto& soldiersMOVING = spriteREGISTER.get<MOVING>(soldier);

	LOCATION destination = soldiersMOVING.waypoints.back(); // save final target

	float dist = distanceTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, destination.POS);
	if (dist < 450.0) { return; }
	soldiersMOVING.waypoints.clear();
	float wayPOINT_SPACING = 450.0;
	int numOF_WAYPOINTS = dist / wayPOINT_SPACING;
	std::cout << "num of waypoints:" << numOF_WAYPOINTS << std::endl;

	SDL_FPoint dirNorm = {
		soldiersMOVING.dX / soldiersMOVING.movementSPEED,
		soldiersMOVING.dY / soldiersMOVING.movementSPEED
	};

	soldiersMOVING.waypoints.push_back(destination);

	for (int i = numOF_WAYPOINTS; i > 0; i--)
	{
		SDL_FPoint wpPOS = {
			soldierSPRITE_INFO.spriteLOCATION.POS.x + dirNorm.x * (i * wayPOINT_SPACING),
			soldierSPRITE_INFO.spriteLOCATION.POS.y + dirNorm.y * (i * wayPOINT_SPACING)
		};
		soldiersMOVING.waypoints.push_back({ wpPOS, {0.0f, 0.0f} });
	}
}


//THIS NEEDS AN OVERHAUL