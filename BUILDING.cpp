#include "SPRITE.h"

//BUILDING

entt::entity SPRITE_MANAGER::createFOXHOLE(SDL_FPoint pos, ROTATION rot)
{
	std::vector<coverPOS> newFIRING_POSITIONS = { {entt::null, pos} };
	entt::entity newFOXHOLE = createBUILDING(pos, rot, 128, 128, -5.0f, FOXHOLE, newFIRING_POSITIONS, 0.90);
	auto& newBUILDING_STRUCT = spriteREGISTER.get<BUILDING>(newFOXHOLE);

	return newFOXHOLE;
}

entt::entity SPRITE_MANAGER::createDUGOUT(SDL_FPoint pos, ROTATION rot)
{
	SDL_FPoint pos1 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { -40.0f, 40.0f });
	SDL_FPoint pos2 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { 40.0f, 40.0f });
	SDL_FPoint pos3 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { -40.0f, -40.0f });
	SDL_FPoint pos4 = rotatePOINT_AND_APPLY_OFFSET(pos, rot, { 40.0f, -40.0f });

	std::vector<coverPOS> newFIRING_POSITIONS = { {entt::null, pos1}, {entt::null, pos2}, {entt::null, pos3}, {entt::null, pos4}, };
	entt::entity newDUGOUT = createBUILDING(pos, rot, 256.0f, 256.0f, 2.0f, DUGOUT, newFIRING_POSITIONS, 1.0f);
	auto& newBUILDING_STRUCT = spriteREGISTER.get<BUILDING>(newDUGOUT);
		auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(newDUGOUT);

		auto& buildingINFO = spriteREGISTER.get<BUILDING>(newDUGOUT);
		buildingINFO.topCOVERED = true;
		buildingINFO.isDUGOUT = true;

	//bottom
		entt::entity newDUGOUT_BASEMENT_SPRITE = createSPRITE(pos, rot, 256.0f, 256.0f, -1.0f);
		auto& basementSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(newDUGOUT_BASEMENT_SPRITE);
		basementSPRITE_INFO.TYPE = DUGOUT_BOTTOM;

		return newDUGOUT;
}


entt::entity SPRITE_MANAGER::createBUILDING(SDL_FPoint pos, ROTATION rot, float w, float h, float z, UV_REGION BUILDING_TEX_TYPE, std::vector<coverPOS> firingPOS, float coverVALUE)
{
	//IF DUGOUT soldiers cant shoot out of or be seen

	entt::entity newBUILDING = createSPRITE(pos, rot, w, h, z);
	auto& newBUIDLING = spriteREGISTER.get<spriteOBJECT>(newBUILDING);
	newBUIDLING.TYPE = BUILDING_TEX_TYPE;
	auto& newBUILDING_STRUCT = spriteREGISTER.emplace<BUILDING>(newBUILDING); //use this shit everywhere
	newBUILDING_STRUCT.coverVALUE = coverVALUE;
	newBUILDING_STRUCT.soldierINSIDE = firingPOS;
	return newBUILDING;
}

void SPRITE_MANAGER::soldierENTERED_BUILDING(entt::entity soldier, entt::entity building)
{
	auto& buildingINFO = spriteREGISTER.get<BUILDING>(building);
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(soldier);

	std::cout << "Soldier Entered Building!\n";
	soldierINFO.curBUILDING = building;

	//NEED BETTER WAY TO ASSIGN POSITION
	if (buildingINFO.topCOVERED)
	{
		//turn opcaity off
		std::cout << "soldier entered building with top cover\n";
		auto& buildingSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldierINFO.curBUILDING);
		buildingSPRITE_INFO.hidden = true;
	}

	if (buildingINFO.isDUGOUT)
	{
		spriteREGISTER.emplace_or_replace<inDUGOUT>(soldier); //Add shit
	}
	spriteREGISTER.emplace_or_replace<inCOVER>(soldier); //In dugout is also in cover

	soldierINFO.coverVALUE = buildingINFO.coverVALUE;
	spriteREGISTER.remove<ORDER_TO_BUILDING>(soldier); //remove this shit
}

void SPRITE_MANAGER::soldierMOVE_OUT_BUILDING(entt::entity soldier, entt::entity building)
{
	std::cout << "Move out\n";
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(soldierINFO.curBUILDING);

	//Check for topcover
	if (buildingINFO_STRUCT.isEMPTY() && buildingINFO_STRUCT.topCOVERED)
	{
		//turn opcaity back on
		auto& buildingSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldierINFO.curBUILDING);
		buildingSPRITE_INFO.hidden = false;
	}

	if (buildingINFO_STRUCT.isDUGOUT)
	{
		spriteREGISTER.remove<inDUGOUT>(soldier); //remove this shit
	}

	spriteREGISTER.remove<inCOVER>(soldier); //remove this shit
	soldierINFO.curBUILDING = entt::null;

}

void SPRITE_MANAGER::checkBUILDING_INSIDES()
{

	auto allBUILDINGS = spriteREGISTER.view<BUILDING>();
	auto allSOLDIERS = spriteREGISTER.view<soldierOBJECT>();
	std::vector<entt::entity> soldierPOS;
	soldierPOS.reserve(allSOLDIERS.size());

	for (auto& soldier : allSOLDIERS)//collect all pos, once
	{
		soldierPOS.push_back(soldier);
	}


	for (auto& building : allBUILDINGS)
	{
		auto& curBUILDING_SPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(building);
		int soldiersIN_BUILDING = 0;


			for(int i = 0; i < soldierPOS.size();i++)
			{
				auto& curSOLDIER_POS = spriteREGISTER.get<spriteOBJECT>(soldierPOS[i]).spriteLOCATION.POS;
				if (distanceTO_POINT(curBUILDING_SPRITE_INFO.spriteLOCATION.POS, curSOLDIER_POS) > 300) { continue; }
				auto& curSOLDIER = spriteREGISTER.get<soldierOBJECT>(soldierPOS[i]);

				if (isPOINT_WITHIN_BOUNDS(curSOLDIER_POS, curBUILDING_SPRITE_INFO.spriteLOCATION.POS, curBUILDING_SPRITE_INFO.spriteLOCATION.ROT, curBUILDING_SPRITE_INFO.texW, curBUILDING_SPRITE_INFO.texH))
				{
					//this soldier is in this building and not already in this building, soldier entered building
					if (curSOLDIER.curBUILDING != building)
					{
						soldierENTERED_BUILDING(soldierPOS[i], building);
					}
					soldiersIN_BUILDING++;
				}
				else if (curSOLDIER.curBUILDING == building) //no longer inside building remove
				{
					soldierMOVE_OUT_BUILDING(soldierPOS[i], building);
					soldiersIN_BUILDING--;
				}

			}

			auto& curBUILDING_INFO = spriteREGISTER.get<BUILDING>(building);
			curBUILDING_INFO.numOF_SOLDIERS_INSIDE = soldiersIN_BUILDING;

	}
}