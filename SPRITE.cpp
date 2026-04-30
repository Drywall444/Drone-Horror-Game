#include "SPRITE.h"

//DT
void SPRITE_MANAGER::updateDT(float newDT)
{
	DT = newDT;
}

bool hasARRIVED_AT_POINT(SDL_FPoint spriteCUR_POS, MOVING soldierMOVING_INFO)
{
	float pX = soldierMOVING_INFO.targetLOC.x - spriteCUR_POS.x;
	float pY = soldierMOVING_INFO.targetLOC.y - spriteCUR_POS.y;

	float DOT = (soldierMOVING_INFO.dX * pX) + (soldierMOVING_INFO.dY * pY);
	if (DOT < 0.0) { return true; }
	else { return false; }
}

//SPRITE_MANAGER

void SPRITE_MANAGER::spriteCREATE(textureATLAS sheetNUM, SDL_FPoint pos, ROTATION ROT, bool isFRIEND)
{
		entt::entity newSPRITE = spriteREGISTER.create();
		spriteOBJECT newSPRITE_OBJ;
		ROTATION newROT = ROT;
		LOCATION newLOC = { {pos.x, pos.y}, newROT };
		newSPRITE_OBJ.spriteLOCATION = newLOC;
		newSPRITE_OBJ.textureSHEET_NUM = sheetNUM;

		if (sheetNUM == HUMAN)
		{
			if (isFRIEND)//FRIENDLY
			{
				newSPRITE_OBJ.TYPE = SOLDIER_STANDING;

				soldierOBJECT newSOLDIER;
				newSOLDIER.friendly = true;
				newSOLDIER.HP = 100.0;
				spriteREGISTER.emplace<soldierOBJECT>(newSPRITE, newSOLDIER);
			}
			else 
			{
				newSPRITE_OBJ.TYPE = ENEMY_SOLDIER_STANDING;
				soldierOBJECT newSOLDIER;
				newSOLDIER.friendly = false;
				newSOLDIER.HP = 100.0;
				spriteREGISTER.emplace<soldierOBJECT>(newSPRITE, newSOLDIER);
			}
		}
		else if (sheetNUM == CORPSE)
		{
			std::cout << "Corpse Added.\n";
			newSPRITE_OBJ.texH = 128;
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
		}
		spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);
}

void SPRITE_MANAGER::tileCREATE(UV_REGION type, SDL_FPoint pos)
{
	entt::entity newTILE_ENT = spriteREGISTER.create();
	TILE newTILE;
	newTILE.pos = pos;
	newTILE.TYPE = type;
	spriteREGISTER.emplace<TILE>(newTILE_ENT, newTILE);
}

//SOLDIER
 void SPRITE_MANAGER::createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isFRIEND)
{
	 if (isFRIEND)
	 {
		 spriteCREATE(HUMAN,  pos, rot, true);
	 }
	 else
	 {
		 spriteCREATE(HUMAN, pos, rot, false);
	 }
}

 void MOVING::move()
 {

 }


 void SPRITE_MANAGER::updateGAME()
 {
	 std::vector<entt::entity> toDESTROY;

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

			 spriteCREATE(CORPSE, curS_SPRITE_INFO.spriteLOCATION.POS, curS_SPRITE_INFO.spriteLOCATION.ROT, curS.friendly);

			 toDESTROY.push_back(S);
		 }

		 auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(S);

		 if (spriteREGISTER.all_of<hasTARGET>(S)) {
			 spriteINFO.TYPE = curS.friendly ? SOLDIER_SHOOTING : ENEMY_SOLDIER_SHOOTING;
		 }
		 else if (spriteREGISTER.all_of<MOVING>(S)) {
			 spriteINFO.TYPE = curS.friendly ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
		 }
		 else {
			 spriteINFO.TYPE = curS.friendly ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
		 }
	 }

	 //MOVEMENT - MOVE INTO SEPERATE FUNCTION
	 auto movingSPRITES = spriteREGISTER.view<MOVING>();

	 for (auto& sprite : movingSPRITES)
	 {
		 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(sprite);
		 auto& soldierMOVING_INFO = spriteREGISTER.get<MOVING>(sprite);

		 if (hasARRIVED_AT_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, soldierMOVING_INFO))
		 {
			 if (!(soldierSPRITE_INFO.TYPE.uvTYPE == T_VFX_TRACER)) //if bullet delete
			 {
				 spriteREGISTER.remove<MOVING>(sprite);
			 }
			 else
			 {
				 toDESTROY.push_back(sprite); //If add to destroy list
			 }
		 }
		else 
		{
			SDL_FPoint pos = soldierSPRITE_INFO.spriteLOCATION.POS;
			pos.x += soldierMOVING_INFO.dX * DT;
			pos.y += soldierMOVING_INFO.dY * DT;
			soldierSPRITE_INFO.spriteLOCATION.POS = pos;
		}

	 }

	 //CHECK LOS - Friendly
	 auto soldiersFRIENDLY = spriteREGISTER.view<soldierOBJECT>();
	 for (auto& SOLDIER : soldiersFRIENDLY)
	 {
		 bool isFRIEND = false;
		 auto& soldierSPRITE_INFO = spriteREGISTER.get<soldierOBJECT>(SOLDIER);
		 if (soldierSPRITE_INFO.friendly) { isFRIEND = true; }
		 checkLOS(SOLDIER, isFRIEND);

	 }

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

 }

 void SPRITE_MANAGER::soldierSHOOT_AT_TARGET(entt::entity soldier)
 {
	 auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	 if (soldierINFO.outOF_AMMO)
	 {
		 std::cout << "No ammo\n";
		 return;
	 }

	 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 auto& soldiersSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);

	 auto& enemySPRITE = spriteREGISTER.get<spriteOBJECT>(soldiersSHOOTING.enemySOLDIER);

	 if (!spriteREGISTER.all_of<FIRING>(soldier))
	 {
		 FIRING newFIRE;
		 newFIRE.secPER_BULLET = (1.0f / (soldierINFO.weapon.weaponRPM / 60.0f));
		 newFIRE.TIME_SINCE_LAST_SHOT = newFIRE.secPER_BULLET; // ready to fire immediately
		 newFIRE.aimTIME = 2.5;
		 spriteREGISTER.emplace<FIRING>(soldier, newFIRE);
	 }

	 ROTATION newROT = rotationTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, enemySPRITE.spriteLOCATION.POS);
	 soldierSPRITE_INFO.spriteLOCATION.ROT = newROT; //aim at target
	 fireWEAPON(soldier, soldiersSHOOTING);


 }

 void SPRITE_MANAGER::fireWEAPON(entt::entity soldier, hasTARGET target)
 {
	 float baseMISS = 80.0;

	 auto& fireINFO = spriteREGISTER.get<FIRING>(soldier);
	 auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	 auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 if (soldierINFO.weapon.curMAG_SIZE <= 0 && soldierINFO.weapon.reloading == false) //If our mag is on empty and we havent started reloading, reload
	 {
		 soldierINFO.weapon.reloading = true;
		 soldierINFO.weapon.curRELOAD_TIME = soldierINFO.weapon.reloadTIME; //set to full reload time
	 }

	 if (soldierINFO.weapon.reloading == true)
	 {
		 soldierINFO.weapon.curRELOAD_TIME -= DT;
		 if (soldierINFO.friendly){ spriteINFO.TYPE = SOLDIER_RELOADING; } //set animation to reloading basic as hell will change later
		 else{ spriteINFO.TYPE = ENEMY_SOLDIER_RELOADING; }

		 if (soldierINFO.weapon.curRELOAD_TIME < 0.0)
		 {
			 std::cout << "Done Reloading\n";
			 float randY = randBETWEEN( -15.0, 15.0);
			 SDL_FPoint magOFF_SET = { 30.0, randY };
			 SDL_FPoint magPOS = rotatePOINT_AND_APPLY_OFFSET(spriteINFO.spriteLOCATION.POS, spriteINFO.spriteLOCATION.ROT, magOFF_SET);
			 createVFX(magPOS, randROTATION(), VFX_MAG, 16, 16, -1);
			 soldierINFO.weapon.curMAG_SIZE = soldierINFO.weapon.magSIZE;
			 soldierINFO.weapon.reloading = false; //When done reloading set false and then return
			 soldierINFO.curMAGS -= 1;
			 if (soldierINFO.curMAGS <= 0)
			 {
				 soldierINFO.outOF_AMMO = true;
			 }
		 }
		 return;
	 }

	 if (fireINFO.aimTIME < 0.0 || fireINFO.firingBURST && soldierINFO.weapon.reloading == false)
	 {
		 if (fireINFO.TIME_SINCE_LAST_SHOT > fireINFO.secPER_BULLET)
		 {
			 if (fireINFO.firingBURST == false)
			 {
				 fireINFO.firingBURST = true;
			 }
			 else {
				 float rand = randBETWEEN(0.0, 5.0);
				 if (rand > 4.0)
				 {
					 fireINFO.firingBURST = false;
				 }
			 }

			 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
			 auto& enemySPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(target.enemySOLDIER);
			 auto& enemySPRITE = spriteREGISTER.get<soldierOBJECT>(target.enemySOLDIER);

			 float dist = distanceTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, enemySPRITE_INFO.spriteLOCATION.POS);
			 fireINFO.aimTIME = dist / 1000;
			 float soldierSKILL = soldierINFO.soldierSKILL;

			 float rangeFACTOR = dist / soldierINFO.weapon.weaponEFFECTIVE_RANGE;
			 float baseHIT = 0.95f - (rangeFACTOR * rangeFACTOR) * 0.85f;
			 float finalHIT = baseHIT * soldierINFO.soldierSKILL;
			 finalHIT = std::clamp(finalHIT, 0.01f, 0.99f);
			 //If miss randomize direction

			 float randNUMBER = randBETWEEN(0.0f, 1.0f);

			 if (randNUMBER < finalHIT / 5)
			 {
				 //we hit
				 soldierTAKE_DAMAGE(target.enemySOLDIER, soldierINFO.weapon.weaponDMG);
				 //apply damage after bullet hits


				 //VFX
				 spawnBULLET(soldier, enemySPRITE_INFO.spriteLOCATION.POS);
				 if (enemySPRITE.HP < 0.0)
				 {
					 //stop firing when dead, nolonger have target
					 spriteREGISTER.get<hasTARGET>(soldier).targetDEAD = true;
					 spriteREGISTER.remove<FIRING>(soldier);

					 return;
				 }
			 }
			 else {
				 //VFX
				 std::cout << "Missed\n";

				 //create function for this
				 float randX = randBETWEEN(enemySPRITE_INFO.texW / 2, 90.0);
				 float randY = randBETWEEN(enemySPRITE_INFO.texH / 2, 90.0);
				 if (randBETWEEN(0.0, 1.0) > 0.5) { randX = -randX; }
				 if (randBETWEEN(0.0, 1.0) > 0.5) { randY = -randY; }
				 SDL_FPoint missPOINT = { enemySPRITE_INFO.spriteLOCATION.POS.x + randX, enemySPRITE_INFO.spriteLOCATION.POS.y + randY };
				 spawnBULLET(soldier, missPOINT);
			 }
			 soldierINFO.weapon.curMAG_SIZE -= 1;
			 fireINFO.TIME_SINCE_LAST_SHOT = 0.0;
		 }
	 }


	 fireINFO.TIME_SINCE_LAST_SHOT += DT; //increase by secs
	 if (!fireINFO.firingBURST) //If not firing burst reduce aim time while we steady
	 {
		 fireINFO.aimTIME -= DT; //reduce steady time
	 }

 }

 void SPRITE_MANAGER::soldierTAKE_DAMAGE(entt::entity soldier, float damage)
 {
	 spriteREGISTER.get<soldierOBJECT>(soldier).HP -= damage;
	 auto& curSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	 float randX = randBETWEEN(5, 25);
	 float randY = randBETWEEN(5, 25);
	 if (randBETWEEN(0.0, 1.0) > 0.5) { randX = -randX; }
	 if (randBETWEEN(0.0, 1.0) > 0.5) { randY = -randY; }
	 SDL_FPoint bloodPOINT = { curSOLDIER.spriteLOCATION.POS.x + randX, curSOLDIER.spriteLOCATION.POS.y + randY };

	 UV_REGION bloodTEX_TYPE;
	 float randBETWEEN4 = randBETWEEN(0.0, 4.0);
	 if (randBETWEEN4 > 3.0) { bloodTEX_TYPE = VFX_BLOOD_1; }
	 else if (randBETWEEN4 > 2.0) { bloodTEX_TYPE = VFX_BLOOD_2; }
	 else if (randBETWEEN4 > 1.0) { bloodTEX_TYPE = VFX_BLOOD_3; }
	 else { bloodTEX_TYPE = VFX_BLOOD_4; }

	 createVFX(bloodPOINT, randROTATION(), bloodTEX_TYPE, 32, 32, -1);

 }

 //MOVEMENT

 MOVING newMOVEMENT(float speed, ROTATION dirTO, SDL_FPoint targetLOC)
 {
	 float dx = dirTO.cosR * speed;
	 float dy = dirTO.sinR * speed;

	 MOVING newMOVING_ORDER;
	 newMOVING_ORDER.dX = dx;
	 newMOVING_ORDER.dY = dy;
	 newMOVING_ORDER.targetLOC = targetLOC;
	 newMOVING_ORDER.movementSPEED = speed;

	 return newMOVING_ORDER;
 }

 void SPRITE_MANAGER::ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS)
 {

	 auto& selcSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	 auto& selcSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	 ROTATION direction = directionTO_POINT(selcSOLDIER.spriteLOCATION.POS, globalPOS); //ALWAYS USE un roated direction
	 MOVING newMOVING_ORDER = newMOVEMENT(selcSOLDIER_INFO.speed, direction, globalPOS);
	 selcSOLDIER.spriteLOCATION.ROT = rotationTO_POINT(selcSOLDIER.spriteLOCATION.POS, globalPOS);

	 spriteREGISTER.emplace_or_replace<MOVING>(soldier, newMOVING_ORDER); //if already ordered delete previous and replace, god i love entt
 }

 void SPRITE_MANAGER::checkLOS(entt::entity soldier, bool friendly)
 {

	 float losRANGE = 2000.0;

		 auto allSOLDIER = spriteREGISTER.view<soldierOBJECT>();

		 auto& curSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
		 auto& curSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

		 bool hasTARGET_BOOL = false;
		 bool removeTARGET = false;
		 if (spriteREGISTER.all_of<hasTARGET>(soldier))
		 {
			 hasTARGET_BOOL = true;

			 auto& t = spriteREGISTER.get<hasTARGET>(soldier);
			 if (!spriteREGISTER.valid(t.enemySOLDIER))
			 {
				 spriteREGISTER.remove<hasTARGET>(soldier);
				 hasTARGET_BOOL = false;
			 }
		 }

		 for (auto& enemy : allSOLDIER)
		 {
			 auto& curENEMY = spriteREGISTER.get<spriteOBJECT>(enemy);
			 auto& curENEMY_INFO = spriteREGISTER.get<soldierOBJECT>(enemy);
			 //Skip friendly checks
			 if (friendly == true && curENEMY_INFO.friendly == true) { continue; }
			 if (friendly == false && curENEMY_INFO.friendly == false) { continue; }


			 float dis = distanceTO_POINT(curSOLDIER.spriteLOCATION.POS, curENEMY.spriteLOCATION.POS);

			 if (dis > losRANGE) //Skip if to far
			 {
				 if (hasTARGET_BOOL)
				 {
					 // only remove if THIS enemy IS our current target
					 auto& currentTARGET = spriteREGISTER.get<hasTARGET>(soldier);
					 if (currentTARGET.enemySOLDIER == enemy)
					 {
						 removeTARGET = true;
						 hasTARGET_BOOL = false;
					 }
				 }
				 continue;
			 }
			 else if (hasTARGET_BOOL)
			 {
				 auto& targetINFO = spriteREGISTER.get<hasTARGET>(soldier);
				 float curTARGET_DIS = distanceTO_POINT(curSOLDIER.spriteLOCATION.POS, spriteREGISTER.get<spriteOBJECT>(targetINFO.enemySOLDIER).spriteLOCATION.POS);
				 //if new target closer than current target switch
				 if (curTARGET_DIS > dis)
				 {
					 removeTARGET = false;
					 hasTARGET newTARGET = { enemy, false };
					 spriteREGISTER.emplace_or_replace<hasTARGET>(soldier, newTARGET);

				 }
			 }
			 else {
				 removeTARGET = false;
				 hasTARGET newTARGET = { enemy, false };
				 spriteREGISTER.emplace_or_replace<hasTARGET>(soldier, newTARGET);
			 }
		 }

		 if (removeTARGET){ spriteREGISTER.remove<hasTARGET>(soldier); }
 }

 //VFX

 void SPRITE_MANAGER::spawnBULLET(entt::entity soldier, SDL_FPoint target)
 {
	 float bulletSPEED = 3500.0;

	 auto& soldierINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 SDL_FPoint offSET_TRACER = { 7.5f, -50.0f };

	 ROTATION newROT = soldierINFO.spriteLOCATION.ROT;
	 SDL_FPoint bulletPOS = rotatePOINT_AND_APPLY_OFFSET(soldierINFO.spriteLOCATION.POS, newROT, offSET_TRACER);
	 entt::entity newBULLET = createVFX(bulletPOS, newROT, VFX_BULLET, 32, 64, 0.5);

	 //EMPLACE MOVING
	 ROTATION dirTOPOINT = directionTO_POINT(bulletPOS, target);
	 MOVING bulletMOVEMENT = newMOVEMENT(bulletSPEED, dirTOPOINT, target);
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
	 newSPRITE_OBJ.textureSHEET_NUM = HUMAN;
	 newSPRITE_OBJ.texW = w;
	 newSPRITE_OBJ.texH = h;
	 newSPRITE_OBJ.TYPE = MAG_TEX_TYPE;
	 newSPRITE_OBJ.spriteLOCATION.z = z;
	 spriteREGISTER.emplace<spriteOBJECT>(newVFX, newSPRITE_OBJ);

	 return newVFX;
 }

 //COMBINE VFX INTO ONE FUNCTION


 //TO DO:

 // - ADD Bullet Casings
