#include "SPRITE.h"

//BUILDING - Create Seperate CPP

entt::entity SPRITE_MANAGER::createBUILDING(SDL_FPoint pos, ROTATION rot, UV_REGION BUILDING_TEX_TYPE)
{
	entt::entity newBUILDING = createSPRITE(pos, rot, 128, 128);
	auto& newBUIDLING = spriteREGISTER.get<spriteOBJECT>(newBUILDING);
	newBUIDLING.TYPE = BUILDING_TEX_TYPE;
	newBUIDLING.spriteLOCATION.z = -10;
	auto& newBUILDING_STRUCT = spriteREGISTER.emplace<BUILDING>(newBUILDING); //use this shit everywhere
	std::vector<SDL_FPoint> newFIRING_POSITIONS = { {newBUIDLING.spriteLOCATION.POS.x, newBUIDLING.spriteLOCATION.POS.y} }; //one position
	newBUILDING_STRUCT.coverVALUE = 0.90;

	return newBUILDING;
}

void SPRITE_MANAGER::soldierMOVE_TO_BUILDING(entt::entity soldier, entt::entity building)
{
	auto& soldierINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& soldierINFO_STRUCT = spriteREGISTER.get<soldierOBJECT>(soldier);
	auto& buildingINFO = spriteREGISTER.get<spriteOBJECT>(building);
	auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(building);
	soldierINFO_STRUCT.curBUILDING = building; //pointer to building
	spriteREGISTER.emplace_or_replace<ORDER_TO_BUILDING>(soldier, building);

	ORDER_soldierMOVE_TO_POINT(soldier, buildingINFO.spriteLOCATION.POS); // can change to waypoint paths for pathing inside buildings in the future
	buildingINFO_STRUCT.soldierINSIDE = soldier;

}

void SPRITE_MANAGER::soldierENTERED_BUILDING(entt::entity soldier, entt::entity building)
{
	auto& buildingINFO = spriteREGISTER.get<BUILDING>(building);
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	soldierINFO.coverVALUE = buildingINFO.coverVALUE;
	buildingINFO.soldierINSIDE = soldier;
	spriteREGISTER.emplace_or_replace<inCOVER>(soldier); //Add shit
	spriteREGISTER.remove<ORDER_TO_BUILDING>(soldier); //remove this shit
}

void SPRITE_MANAGER::soldierMOVE_OUT_BUILDING(entt::entity building, SDL_FPoint globalPOS)
{
	auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(building);
	auto& soldier = buildingINFO_STRUCT.soldierINSIDE;
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	ORDER_soldierMOVE_TO_POINT(soldier, globalPOS);
	buildingINFO_STRUCT.soldierINSIDE = entt::null;
	soldierINFO.coverVALUE = 0.0;
	spriteREGISTER.remove<inCOVER>(soldier); //remove this shit
}