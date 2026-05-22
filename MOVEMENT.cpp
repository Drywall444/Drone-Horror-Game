#include "SPRITE.h"

MOVING SPRITE_MANAGER::newMOVEMENT(float speed, ROTATION dirTO, SDL_FPoint targetLOC)
{
	float dx = dirTO.cosR * speed;
	float dy = dirTO.sinR * speed;

	MOVING newMOVING_ORDER;
	newMOVING_ORDER.dX = dx;
	newMOVING_ORDER.dY = dy;
	newMOVING_ORDER.waypoint = targetLOC;//rotation meaingless for now
	newMOVING_ORDER.movementSPEED = speed;

	return newMOVING_ORDER;
}

bool SPRITE_MANAGER::hasARRIVED_AT_POINT(SDL_FPoint spriteCUR_POS, MOVING& soldierMOVING_INFO)
{
	SDL_FPoint curWAYPOINT = soldierMOVING_INFO.waypoint; //Get current waypoint
	float pX = curWAYPOINT.x - spriteCUR_POS.x;
	float pY = curWAYPOINT.y - spriteCUR_POS.y;

	float DOT = (soldierMOVING_INFO.dX * pX) + (soldierMOVING_INFO.dY * pY);
	if (DOT < 0.0) { soldierMOVING_INFO; return true; } //delete last waypoint if arrived
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
		else 
		{
				//MOVE
				SDL_FPoint pos = spriteINFO.spriteLOCATION.POS;
				pos.x += soldierMOVING_INFO.dX * DT;
				pos.y += soldierMOVING_INFO.dY * DT;
				spriteINFO.spriteLOCATION.POS = pos;
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
}