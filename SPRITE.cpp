#include "SPRITE.h"

//DT and Helper functions
void SPRITE_MANAGER::updateDT(float newDT)
{
	DT = newDT;
}

void SPRITE_MANAGER::countDOWN_TEMP_SPRITES() //FIX SO THAT ALL TEMP SPRITES ARE IN THE VECTOR
{
	auto tempSPRITES = spriteREGISTER.view<tempSPRITE>();
	for (auto temp : tempSPRITES)
	{
		auto& tempINFO = spriteREGISTER.get<tempSPRITE>(temp);
		

			int totalFRAME_NUM = tempINFO.frames.size();
			float fps = tempINFO.orginalTIME / totalFRAME_NUM;
			int curFRAME = (int)(tempINFO.curTIME / fps);

			UV_REGION curUV = tempINFO.frames[curFRAME];
			auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(temp);
			spriteINFO.TYPE = curUV;

		tempINFO.curTIME += DT;
		if (tempINFO.curTIME >= tempINFO.orginalTIME)
		{
			toDESTROY.push_back(temp);
		}
	}
}

float SPRITE_MANAGER::returnDIST_TO_TARGET(entt::entity soldier, hasTARGET targetINFO)
{
	auto& spriteINFO_POS = spriteREGISTER.get<spriteOBJECT>(soldier).spriteLOCATION.POS;
	auto& ENEMY_spriteINFO_POS = spriteREGISTER.get<spriteOBJECT>(targetINFO.enemySOLDIER).spriteLOCATION.POS;

	float final = distanceTO_POINT(spriteINFO_POS, ENEMY_spriteINFO_POS);
	return final;
}

//Sprite Creation:

entt::entity SPRITE_MANAGER::createSPRITE(SDL_FPoint pos, ROTATION ROT, int texW, int texH, float z)
{
		entt::entity newSPRITE = spriteREGISTER.create();
		spriteOBJECT newSPRITE_OBJ;
		newSPRITE_OBJ.texW = texW;
		newSPRITE_OBJ.texH = texH;
		ROTATION newROT = ROT;
		LOCATION newLOC = { {pos.x, pos.y}, newROT };
		newLOC.z = z;
		newSPRITE_OBJ.spriteLOCATION = newLOC;

		spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);

		return newSPRITE;
}

entt::entity SPRITE_MANAGER::tileCREATE(UV_REGION uv_type, SDL_FPoint pos, natureTYPE_TILE type)
{
	entt::entity newTILE_ENT = spriteREGISTER.create();
	TILE newTILE;
	newTILE.pos = pos;
	newTILE.UV_TYPE = uv_type;
	newTILE.TYPE = type;
	spriteREGISTER.emplace<TILE>(newTILE_ENT, newTILE);
	return newTILE_ENT;
}


 void SPRITE_MANAGER::createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isFRIEND)
{
	 entt::entity newSOLDIER_ENTITY = createSPRITE(pos, rot, 64, 64, 1.0);
	 auto& newSPRITE_OBJ = spriteREGISTER.get<spriteOBJECT>(newSOLDIER_ENTITY);
	 spriteREGISTER.emplace<IDLE>(newSOLDIER_ENTITY);

	 if (isFRIEND)
	 {

		 newSPRITE_OBJ.TYPE = SOLDIER_STANDING;

		 soldierOBJECT newSOLDIER;
		 newSOLDIER.HP = 100.0;
		 spriteREGISTER.emplace<soldierOBJECT>(newSOLDIER_ENTITY, newSOLDIER);
		 spriteREGISTER.emplace<TEAM>(newSOLDIER_ENTITY, BLUFOR);
	 }
	 else //enemy
	 {
		 newSPRITE_OBJ.TYPE = ENEMY_SOLDIER_STANDING;
		 soldierOBJECT newSOLDIER;
		 newSOLDIER.HP = 100.0;
		 spriteREGISTER.emplace<soldierOBJECT>(newSOLDIER_ENTITY, newSOLDIER);
		 spriteREGISTER.emplace<TEAM>(newSOLDIER_ENTITY, OPFOR);
	 }
	 spriteREGISTER.emplace<hasCOLLISION>(newSOLDIER_ENTITY);
}

entt::entity SPRITE_MANAGER::createCORPSE(SDL_FPoint pos, ROTATION rot, TEAM teaminfo)
{
	 std::cout << "Corpse Added.\n";
	 entt::entity newCORPSE_ENTITY = createSPRITE(pos, rot, 64, 128, 0.5);
	 auto& newSPRITE_OBJ = spriteREGISTER.get<spriteOBJECT>(newCORPSE_ENTITY);

	 float rnd = randBETWEEN(0, 1);
	 UV_REGION corpseTYPE;

	 if (teaminfo.soldierTEAM == BLUFOR)//FRIENDLY
	 {
		 if (rnd > 0.5) { corpseTYPE = SOLDIER_DEAD_1; }
		 else { corpseTYPE = SOLDIER_DEAD_2; }

		 newSPRITE_OBJ.TYPE = corpseTYPE;
	 }
	 else
	 {
		 if (rnd > 0.5) { corpseTYPE = ENEMY_SOLDIER_DEAD_1; }
		 else { corpseTYPE = ENEMY_SOLDIER_DEAD_2; }

		 newSPRITE_OBJ.TYPE = corpseTYPE;
	 }
	 
	 return newCORPSE_ENTITY;
}

entt::entity SPRITE_MANAGER::createCORPSE_IN_COVER(SDL_FPoint pos, ROTATION rot, TEAM teaminfo, entt::entity buidling)
{
	auto& buildingINFO = spriteREGISTER.get<BUILDING>(buidling);
	entt::entity corpse = createCORPSE(pos, rot, teaminfo);
	buildingINFO.soldierINSIDE.clear(); //this clears all corpses

	return corpse;
}

void SPRITE_MANAGER::soldierHEALTH(entt::entity soldier)
{
	auto& curS = spriteREGISTER.get<soldierOBJECT>(soldier);
	if (curS.HP < 0.0) //DEAD
	{
		auto& curS_SPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
		auto& teamINFO = spriteREGISTER.get<TEAM>(soldier);

		if (spriteREGISTER.all_of<inCOVER>(soldier)) //If died in cover
		{
			createCORPSE_IN_COVER(curS_SPRITE_INFO.spriteLOCATION.POS, curS_SPRITE_INFO.spriteLOCATION.ROT, teamINFO, curS.curBUILDING);
		}
		else {
			createCORPSE(curS_SPRITE_INFO.spriteLOCATION.POS, curS_SPRITE_INFO.spriteLOCATION.ROT, teamINFO);
		}


		if (spriteREGISTER.all_of<hasCOLLISION>(soldier)) //remove from tile
		{
			auto& curS_COLLISION_INFO = spriteREGISTER.get<hasCOLLISION>(soldier);
			entt::entity curTILE = worldTILES[curS_COLLISION_INFO.curINDEX];
			removeCOLLISION_SPRITE_FROM_TILE(soldier, curTILE);
		}

		toDESTROY.push_back(soldier);
	}
}

void SPRITE_MANAGER::assignCOVER(entt::entity soldier)
{
	auto& collisionSTRUCT = spriteREGISTER.get<hasCOLLISION>(soldier);
	entt::entity tile = worldTILES[collisionSTRUCT.curINDEX];
	auto& curTILE_STRUCT = spriteREGISTER.get<TILE>(tile);
	auto& soldierSTRUCT = spriteREGISTER.get<soldierOBJECT>(soldier);

	if (curTILE_STRUCT.TYPE == TYPE_WOODS1)
	{
		soldierSTRUCT.coverVALUE = 0.50f;
		soldierSTRUCT.concealment = 0.50f;
	}
	else {
		soldierSTRUCT.coverVALUE = 0.0f;
		soldierSTRUCT.concealment = 0.0f;
	}

	if (spriteREGISTER.all_of<FIRING>(soldier))
	{
		soldierSTRUCT.concealment = 0.00f;
	}
}


 void SPRITE_MANAGER::updateGAME()
 {

	 toDESTROY.clear();
	 countDOWN_TEMP_SPRITES(); //Temp sprites timer
	 assignCOLLISION();
	 checkCOLLISIONS();

	 auto allSoldiers = spriteREGISTER.view<soldierOBJECT>(); //STATE CHECK
	 for (auto& S : allSoldiers)
	 {
		 auto& curS = spriteREGISTER.get<soldierOBJECT>(S);
		 soldierHEALTH(S); //Health Check
		 assignCOVER(S);

		 if (curLOS_DELAY < 0.0)
		 {
			 auto& curTEAM_INFO = spriteREGISTER.get<TEAM>(S);
			 //Check LOS
			 checkLOS(S, curTEAM_INFO);
		 }
		 //flags
		 bool movingAND_IDLE = false;
		 bool moving = false;

		 if (spriteREGISTER.all_of<MOVING>(S)) //if moving
		 {
			 auto& moveINFO = spriteREGISTER.get<MOVING>(S);
			 if (moveINFO.stopped) { spriteREGISTER.emplace_or_replace<IDLE>(S); movingAND_IDLE = true; }//we idle baby and set flag
			 else if (spriteREGISTER.all_of<IDLE>(S)) { spriteREGISTER.remove<IDLE>(S); } //if not stopped and still idle remove that hoe!
			 moving = true;
		 }


		 auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(S);
		 auto& teamINFO = spriteREGISTER.get<TEAM>(S); //clean this hoe up

		 if (spriteREGISTER.all_of<throwingGRENADE>(S)) {
			 spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_AIM_GRENADE : ENEMY_SOLDIER_AIM_GRENADE;
		 }
		 else if (moving) //we are moving 
		 {
			 if (spriteREGISTER.all_of<FIRING>(S) && movingAND_IDLE) { //If Moving, but we are waiting we 
				 spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_SHOOTING : ENEMY_SOLDIER_SHOOTING;
				 continue;
			 }
			 spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
			 //can add moving animation here
		 }
		 else {
			 //otherwise we idle
			 if (spriteREGISTER.all_of<FIRING>(S)) {
				 spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_SHOOTING : ENEMY_SOLDIER_SHOOTING;
			 }
			 else {
				 spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
			 }
			 spriteREGISTER.emplace_or_replace<IDLE>(S);
		 }
	 }


	 //MOVEMENT - MOVE INTO SEPERATE FUNCTION
	 moveSPRITES();

	 //GRENADE
	 soldiersAIM_GRENADE();
	 checkEXPLOSIONS();


	 //SHOOTING - Decision Logic (Will Move)

	 auto soldiersSHOOTING = spriteREGISTER.view<hasTARGET>();
	 std::vector<entt::entity> removeTARGET_LIST;
	 for (auto& shootingSOLDIER : soldiersSHOOTING)
	 {
		 auto& soldierTARGET_INFO = spriteREGISTER.get<hasTARGET>(shootingSOLDIER); 
		 if (soldierTARGET_INFO.targetDEAD == false)
		 {
			 if (spriteREGISTER.valid(soldierTARGET_INFO.enemySOLDIER)) //If sprites been removed as a corpse dont use it, we will crash
			 {
				 if (spriteREGISTER.all_of<IDLE>(shootingSOLDIER)){
					 soldierSHOOT_AT_TARGET(shootingSOLDIER);
				 }
			 }
			 else {
				 soldierTARGET_INFO.enemySOLDIER = entt::null; //set nothing
				 removeTARGET_LIST.push_back(shootingSOLDIER);
				 if (spriteREGISTER.all_of<FIRING>(shootingSOLDIER)) { spriteREGISTER.remove<FIRING>(shootingSOLDIER); } //if we still are firing and dont have a target remove firing
			 }
		 }
		 else {
			 removeTARGET_LIST.push_back(shootingSOLDIER);
		 }
	 }


	 //remove all from list
	 for (auto noTARGET : removeTARGET_LIST) //cleanup
	 {
		 spriteREGISTER.remove<hasTARGET>(noTARGET);
	 }

	 for (auto corpse : toDESTROY) //cleanup
	 {
		 spriteREGISTER.destroy(corpse);
	 }

	 if (curLOS_DELAY < 0.0) { curLOS_DELAY = LOS_DELAY; } //reset outside of loop
	 curLOS_DELAY -= DT;
 }

 //TO DO:

 // - ADD Bullet Casings
