#include "SPRITE.h"

//DT and Helper functions
void SPRITE_MANAGER::updateDT(float newDT)
{
	DT = newDT;
}

bool SPRITE_MANAGER::hasARRIVED_AT_POINT(SDL_FPoint spriteCUR_POS, MOVING& soldierMOVING_INFO)
{
	LOCATION curWAYPOINT = soldierMOVING_INFO.waypoints.back();
	float pX = curWAYPOINT.POS.x - spriteCUR_POS.x;
	float pY = curWAYPOINT.POS.y - spriteCUR_POS.y;

	float DOT = (soldierMOVING_INFO.dX * pX) + (soldierMOVING_INFO.dY * pY);
	if (DOT < 0.0) { soldierMOVING_INFO.waypoints.pop_back(); return true; }
	else { return false; }
}

float SPRITE_MANAGER::returnDIST_TO_TARGET(entt::entity soldier, hasTARGET targetINFO)
{
	auto& spriteINFO_POS = spriteREGISTER.get<spriteOBJECT>(soldier).spriteLOCATION.POS;
	auto& ENEMY_spriteINFO_POS = spriteREGISTER.get<spriteOBJECT>(targetINFO.enemySOLDIER).spriteLOCATION.POS;

	float final = distanceTO_POINT(spriteINFO_POS, ENEMY_spriteINFO_POS);
	return final;
}

//Sprite Creation:

entt::entity SPRITE_MANAGER::createSPRITE(SDL_FPoint pos, ROTATION ROT, int texW, int texH)
{
		entt::entity newSPRITE = spriteREGISTER.create();
		spriteOBJECT newSPRITE_OBJ;
		newSPRITE_OBJ.texW = texW;
		newSPRITE_OBJ.texH = texH;
		ROTATION newROT = ROT;
		LOCATION newLOC = { {pos.x, pos.y}, newROT };
		newSPRITE_OBJ.spriteLOCATION = newLOC;

		spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);

		return newSPRITE;
}

void SPRITE_MANAGER::tileCREATE(UV_REGION type, SDL_FPoint pos)
{
	entt::entity newTILE_ENT = spriteREGISTER.create();
	TILE newTILE;
	newTILE.pos = pos;
	newTILE.TYPE = type;
	spriteREGISTER.emplace<TILE>(newTILE_ENT, newTILE);
}


 void SPRITE_MANAGER::createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isFRIEND)
{
	 entt::entity newSOLDIER_ENTITY = createSPRITE(pos, rot, 64, 64);
	 auto& newSPRITE_OBJ = spriteREGISTER.get<spriteOBJECT>(newSOLDIER_ENTITY);
	 spriteREGISTER.emplace<IDLE>(newSOLDIER_ENTITY);

	 if (isFRIEND)
	 {

		 newSPRITE_OBJ.TYPE = SOLDIER_STANDING;

		 soldierOBJECT newSOLDIER;
		 newSOLDIER.friendly = true;
		 newSOLDIER.HP = 100.0;
		 spriteREGISTER.emplace<soldierOBJECT>(newSOLDIER_ENTITY, newSOLDIER);
	 }
	 else //enemy
	 {
		 newSPRITE_OBJ.TYPE = ENEMY_SOLDIER_STANDING;
		 soldierOBJECT newSOLDIER;
		 newSOLDIER.friendly = false;
		 newSOLDIER.HP = 100.0;
		 spriteREGISTER.emplace<soldierOBJECT>(newSOLDIER_ENTITY, newSOLDIER);
	 }
}

entt::entity SPRITE_MANAGER::createCORPSE(SDL_FPoint pos, ROTATION rot, bool isFRIEND)
{
	 std::cout << "Corpse Added.\n";
	 entt::entity newCORPSE_ENTITY = createSPRITE(pos, rot, 64, 128);
	 auto& newSPRITE_OBJ = spriteREGISTER.get<spriteOBJECT>(newCORPSE_ENTITY);
	 newSPRITE_OBJ.spriteLOCATION.z = -0.5;

	 float rnd = randBETWEEN(0, 1);
	 UV_REGION corpseTYPE;

	 if (isFRIEND)//FRIENDLY
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


 void SPRITE_MANAGER::updateGAME()
 {
	 toDESTROY.clear();
	 //TEMP SPRITES
	 auto tempSPRITES = spriteREGISTER.view<tempSPRITE>();
	 for (auto temp : tempSPRITES)
	 {
		 auto& tempINFO = spriteREGISTER.get<tempSPRITE>(temp);
		 if (tempINFO.timeLEFT < 0.0)
		 {
			 toDESTROY.push_back(temp);
		 }
		 else {
			 tempINFO.timeLEFT -= DT;
		 }
	 }

	 //HEALTH - STATE
	 auto allSOLDIER = spriteREGISTER.view<soldierOBJECT>();

	 for (auto& S : allSOLDIER)
	 {
		 auto& curS = spriteREGISTER.get<soldierOBJECT>(S);
		 if (curS.HP < 0.0) //DEAD
		 {
			 auto& curS_SPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(S);
			 createCORPSE(curS_SPRITE_INFO.spriteLOCATION.POS, curS_SPRITE_INFO.spriteLOCATION.ROT, curS.friendly);
			 toDESTROY.push_back(S);
		 }

		 auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(S);


		 if (curLOS_DELAY < 0.0)
		 {
			 //Check LOS
			 checkLOS(S, curS.friendly);
		 }

		 if (spriteREGISTER.all_of<FIRING>(S)) {
			 spriteINFO.TYPE = curS.friendly ? SOLDIER_SHOOTING : ENEMY_SOLDIER_SHOOTING;
		 }
		 else if (spriteREGISTER.all_of<IDLE>(S)) {
			 spriteINFO.TYPE = curS.friendly ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
		 }
		 else {
			 spriteINFO.TYPE = curS.friendly ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
		 }
	 }

	 //MOVEMENT - MOVE INTO SEPERATE FUNCTION
	 moveSPRITES();

	 //SHOOTING - HAS TARGET

	 auto soldiersSHOOTING = spriteREGISTER.view<hasTARGET>();
	 std::vector<entt::entity> removeTARGET_LIST;
	 for (auto& shootingSOLDIER : soldiersSHOOTING)
	 {
		 auto& soldierSPRITE_INFO = spriteREGISTER.get<hasTARGET>(shootingSOLDIER); 
		 if (soldierSPRITE_INFO.targetDEAD == false)
		 {
			 if (spriteREGISTER.valid(soldierSPRITE_INFO.enemySOLDIER)) //If sprites been removed as a corpse dont use it, we will crash
			 {
				 soldierSHOOT_AT_TARGET(shootingSOLDIER);
			 }
			 else {
				 soldierSPRITE_INFO.enemySOLDIER = entt::null; //set nothing
				 removeTARGET_LIST.push_back(shootingSOLDIER);
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
		 auto& solOBJ = spriteREGISTER.get<soldierOBJECT>(noTARGET);
	 }

	 for (auto corpse : toDESTROY) //cleanup
	 {
		 spriteREGISTER.destroy(corpse);
	 }

	 if (curLOS_DELAY < 0.0) { curLOS_DELAY = LOS_DELAY; } //reset outside of loop
	 curLOS_DELAY -= DT;
 }

 //VFX

 void SPRITE_MANAGER::spawnBULLET(entt::entity soldier, SDL_FPoint target)
 {
	 float bulletSPEED = 3500.0;

	 auto& soldierINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 SDL_FPoint offSET_TRACER = { 7.5f, -50.0f };

	 ROTATION newROT = soldierINFO.spriteLOCATION.ROT;
	 SDL_FPoint bulletPOS = rotatePOINT_AND_APPLY_OFFSET(soldierINFO.spriteLOCATION.POS, newROT, offSET_TRACER);
	 entt::entity newBULLET = createVFX(bulletPOS, newROT, VFX_BULLET, 32, 64, 0);

	 //EMPLACE MOVING
	 ROTATION dirTOPOINT = directionTO_POINT(bulletPOS, target);
	 MOVING bulletMOVEMENT = newMOVEMENT(bulletSPEED, dirTOPOINT, target);
	 bulletMOVEMENT.destroyAT_TARGET = true; //destroy once at target
	 spriteREGISTER.emplace<MOVING>(newBULLET, bulletMOVEMENT);

	 //MUZZLE FLASH
	 SDL_FPoint offSET_MUZZLE_FLASH = { 6.5f, -50.0f };
	 SDL_FPoint flashPOS = rotatePOINT_AND_APPLY_OFFSET(soldierINFO.spriteLOCATION.POS, newROT, offSET_MUZZLE_FLASH);
	 UV_REGION randFLASH_UV;
	 float rand = randBETWEEN(0.0, 3.0);
	 if (rand > 0.2) { randFLASH_UV = VFX_MUZZ_FLASH_1; }
	 else if (rand > 0.1) { randFLASH_UV = VFX_MUZZ_FLASH_2; }
	 else { randFLASH_UV = VFX_MUZZ_FLASH_3; }
	 entt::entity newFLASH = createVFX(flashPOS, newROT, randFLASH_UV, 32, 32, 0.5);

	 float randFLASH = randBETWEEN(0.02f, 0.08f);
	 spriteREGISTER.emplace<tempSPRITE>(newFLASH, randFLASH);

 }

 entt::entity SPRITE_MANAGER::createVFX(SDL_FPoint pos, ROTATION rot, UV_REGION MAG_TEX_TYPE, int w, int h, int z)
 {
	 entt::entity newVFX = spriteREGISTER.create();
	 spriteOBJECT newSPRITE_OBJ;
	 ROTATION newROT = rot;
	 LOCATION newLOC = { pos, newROT };
	 newSPRITE_OBJ.spriteLOCATION = newLOC;
	 newSPRITE_OBJ.texW = w;
	 newSPRITE_OBJ.texH = h;
	 newSPRITE_OBJ.TYPE = MAG_TEX_TYPE;
	 newSPRITE_OBJ.spriteLOCATION.z = z;
	 spriteREGISTER.emplace<spriteOBJECT>(newVFX, newSPRITE_OBJ);

	 return newVFX;
 }



 //BUILDING - Create Seperate CPP

 entt::entity SPRITE_MANAGER::createBUILDING(SDL_FPoint pos, ROTATION rot, UV_REGION BUILDING_TEX_TYPE)
 {
	 entt::entity newBUILDING = createSPRITE(pos, rot, 128, 128);
	 auto& newBUIDLING = spriteREGISTER.get<spriteOBJECT>(newBUILDING);
	 newBUIDLING.TYPE = BUILDING_TEX_TYPE;
	 newBUIDLING.spriteLOCATION.z = -10;
	 auto& newBUILDING_STRUCT = spriteREGISTER.emplace<BUILDING>(newBUILDING); //use this shit everywhere
	 std::vector<SDL_FPoint> newFIRING_POSITIONS = { {newBUIDLING.spriteLOCATION.POS.x, newBUIDLING.spriteLOCATION.POS.y} }; //one position
	 newBUILDING_STRUCT.coverVALUE = 0.85;

	 return newBUILDING;
 }

 void SPRITE_MANAGER::soldierMOVE_INSIDE_BUILDING(entt::entity soldier, entt::entity building)
 {
	 auto& soldierINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 auto& soldierINFO_STRUCT = spriteREGISTER.get<soldierOBJECT>(soldier);
	 auto& buildingINFO = spriteREGISTER.get<spriteOBJECT>(building);
	 auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(building);
	 soldierINFO_STRUCT.curBUILDING = building; //pointer to building

	 std::cout << "Moving to Occupy building\n";
	 ORDER_soldierMOVE_TO_POINT(soldier, buildingINFO.spriteLOCATION.POS);
	 buildingINFO_STRUCT.soldierINSIDE = soldier;

 }

 void SPRITE_MANAGER::soldierMOVE_OUT_BUILDING(entt::entity building, SDL_FPoint globalPOS)
 {
	 auto& buildingINFO_STRUCT = spriteREGISTER.get<BUILDING>(building);
	 auto& soldier = buildingINFO_STRUCT.soldierINSIDE;
	 auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	 ORDER_soldierMOVE_TO_POINT(soldier, globalPOS);
	 buildingINFO_STRUCT.soldierINSIDE = entt::null;
	 soldierINFO.coverVALUE = 0.0;
 }

 //TO DO:

 // - ADD Bullet Casings
