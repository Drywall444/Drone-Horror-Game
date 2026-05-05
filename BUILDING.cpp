#include "SPRITE.h"

//BUILDING - Create Seperate CPP

entt::entity SPRITE_MANAGER::createBUILDING(SDL_FPoint pos, ROTATION rot, UV_REGION BUILDING_TEX_TYPE)
{
	entt::entity newBUILDING = createSPRITE(pos, rot, 128, 128, -10.0);
	auto& newBUIDLING = spriteREGISTER.get<spriteOBJECT>(newBUILDING);
	newBUIDLING.TYPE = BUILDING_TEX_TYPE;
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
	if (buildingINFO_STRUCT.isOCCUPIED())
	{
		std::cout << "Building Full!\n";
		return;
	}

	soldierINFO_STRUCT.curBUILDING = building; //pointer to building
	spriteREGISTER.emplace_or_replace<ORDER_TO_BUILDING>(soldier, building);

	ORDER_soldierMOVE_TO_POINT(soldier, buildingINFO.spriteLOCATION.POS); // can change to waypoint paths for pathing inside buildings in the future
	buildingINFO_STRUCT.soldierINSIDE.push_back(soldier);
	//maybe remove and add when soldier enters building, but also i need to make sure we cant order two soldiers to the same building thats already full

}

void SPRITE_MANAGER::soldierENTERED_BUILDING(entt::entity soldier, entt::entity building)
{
	auto& buildingINFO = spriteREGISTER.get<BUILDING>(building);
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	soldierINFO.coverVALUE = buildingINFO.coverVALUE;
	//buildingINFO.soldierINSIDE.push_back(soldier);
	spriteREGISTER.emplace_or_replace<inCOVER>(soldier); //Add shit
	spriteREGISTER.remove<ORDER_TO_BUILDING>(soldier); //remove this shit
}

void SPRITE_MANAGER::soldierMOVE_OUT_BUILDING(entt::entity building, SDL_FPoint globalPOS, bool movingIN_ANOTHER_BUILDING, entt::entity newBUIDLING)
{
	std::cout << "Move out\n";
	auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(building);
	auto& soldier = buildingINFO_STRUCT.soldierINSIDE.back(); //last entered soldier
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	buildingINFO_STRUCT.soldierINSIDE.pop_back(); //remove last added
	soldierINFO.coverVALUE = 0.0;
	spriteREGISTER.remove<inCOVER>(soldier); //remove this shit

	if (movingIN_ANOTHER_BUILDING)
	{
		soldierMOVE_TO_BUILDING(soldier, newBUIDLING);
		soldierINFO.curBUILDING = newBUIDLING;
	}
	else {
		ORDER_soldierMOVE_TO_POINT(soldier, globalPOS);
		soldierINFO.curBUILDING = entt::null;
	}
}