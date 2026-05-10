#include "SPRITE.h"

//BUILDING

entt::entity SPRITE_MANAGER::createFOXHOLE(SDL_FPoint pos, ROTATION rot)
{
	std::vector<coverPOS> newFIRING_POSITIONS = { {entt::null, pos} };
	entt::entity newFOXHOLE = createBUILDING(pos, rot, FOXHOLE, newFIRING_POSITIONS, 0.90);
	auto& newBUILDING_STRUCT = spriteREGISTER.get<BUILDING>(newFOXHOLE);

	return newFOXHOLE;
}

entt::entity SPRITE_MANAGER::createDUGOUT(SDL_FPoint pos, ROTATION rot)
{
	SDL_FPoint pos1 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { -32.0f, 32.0 });
	SDL_FPoint pos2 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { 32.0f, 32.0 });
	SDL_FPoint pos3 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { -32.0f, -32.0 });
	SDL_FPoint pos4 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { 32.0f, -32.0 });

	std::vector<coverPOS> newFIRING_POSITIONS = { {entt::null, pos1}, {entt::null, pos2}, {entt::null, pos3}, {entt::null, pos4}, };
	entt::entity newDUGOUT = createBUILDING(pos, rot, DUGOUT, newFIRING_POSITIONS, 1.0f);
	auto& newBUILDING_STRUCT = spriteREGISTER.get<BUILDING>(newDUGOUT);
		auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(newDUGOUT);
		spriteINFO.spriteLOCATION.z = 2.0f;
		spriteINFO.texW = 266.0f;
		spriteINFO.texH = 256.0f;

		return newDUGOUT;
}


entt::entity SPRITE_MANAGER::createBUILDING(SDL_FPoint pos, ROTATION rot, UV_REGION BUILDING_TEX_TYPE, std::vector<coverPOS> firingPOS, float coverVALUE)
{
	//IF DUGOUT soldiers cant shoot out of or be seen

	entt::entity newBUILDING = createSPRITE(pos, rot, 128, 128, -10.0);
	auto& newBUIDLING = spriteREGISTER.get<spriteOBJECT>(newBUILDING);
	newBUIDLING.TYPE = BUILDING_TEX_TYPE;
	auto& newBUILDING_STRUCT = spriteREGISTER.emplace<BUILDING>(newBUILDING); //use this shit everywhere
	newBUILDING_STRUCT.coverVALUE = coverVALUE;
	newBUILDING_STRUCT.soldierINSIDE = firingPOS;
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

	for (int i = 0; i < buildingINFO_STRUCT.soldierINSIDE.size();i++)
	{
		if (buildingINFO_STRUCT.soldierINSIDE[i].soldierIN_POS == entt::null)
		{
			buildingINFO_STRUCT.soldierINSIDE[i].soldierIN_POS = soldier;
			ORDER_soldierMOVE_TO_POINT(soldier, buildingINFO_STRUCT.soldierINSIDE[i].globalPOS); // can change to waypoint paths for pathing inside buildings in the future
			break;
		}
	}
	//maybe remove and add when soldier enters building, but also i need to make sure we cant order two soldiers to the same building thats already full

}

void SPRITE_MANAGER::soldierENTERED_BUILDING(entt::entity soldier, entt::entity building)
{
	auto& buildingINFO = spriteREGISTER.get<BUILDING>(building);
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(soldier);

	//NEED BETTER WAY TO ASSIGN POSITION, also snap soldier to pos when arrive

	soldierINFO.coverVALUE = buildingINFO.coverVALUE;
	//buildingINFO.soldierINSIDE.push_back(soldier);
	spriteREGISTER.emplace_or_replace<inCOVER>(soldier); //Add shit
	spriteREGISTER.remove<ORDER_TO_BUILDING>(soldier); //remove this shit
}

void SPRITE_MANAGER::soldierMOVE_OUT_BUILDING(entt::entity soldier, SDL_FPoint globalPOS, bool movingIN_ANOTHER_BUILDING, entt::entity newBUIDLING)
{
	std::cout << "Move out\n";
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(soldierINFO.curBUILDING);

	for (int i = 0; i < buildingINFO_STRUCT.soldierINSIDE.size();i++)
	{
		if (buildingINFO_STRUCT.soldierINSIDE[i].soldierIN_POS == soldier)
		{
			buildingINFO_STRUCT.soldierINSIDE[i].soldierIN_POS = entt::null;
			break;
		}
	}
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