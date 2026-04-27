#include "SPRITE.h"
#include <random>

//DT
void SPRITE_MANAGER::updateDT(float newDT)
{
	DT = newDT;
}

//MATH

ROTATION rotationTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;
	float length = sqrt(dX * dX + dY * dY);
	float cosR = dX / length;
	float sinR = dY / length;
	return { -sinR, cosR }; // 90° CCW baked in
}

ROTATION directionTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;
	float length = sqrt(dX * dX + dY * dY);
	return { dX / length, dY / length }; // no 90° bake, pure direction
}


SDL_FPoint rotatePOINT(SDL_FPoint pos, ROTATION rot)
{

	SDL_FPoint FINAL = { (pos.x * rot.cosR - pos.y * rot.sinR), (pos.x * rot.sinR + pos.y * rot.cosR) };

	return FINAL;
}


float distanceTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB)
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;

	float length = sqrt(dX * dX + dY * dY);
	return length;
}

float dotBETWEEN_ROTS(ROTATION rotA, ROTATION rotB)//RETURNS Dot Product of two rotations
{
	float dot = rotA.cosR * rotB.cosR + rotA.sinR * rotB.sinR;
	//dot product of two angles 
	return dot;
}

bool isPOINT_WITHIN_BOUNDS(SDL_FPoint point, SDL_FPoint spritePOS, ROTATION spriteROT, int texW, int texH)
{

	float localX = point.x - spritePOS.x;//Translate that hoe into local space
	float localY = point.y - spritePOS.y;

	float rotatedX = localX * spriteROT.cosR - localY * spriteROT.sinR;
	float rotatedY = localX * spriteROT.sinR + localY * spriteROT.cosR;

	float halfW = texW / 2.0f;
	float halfH = texH / 2.0f;

	return (rotatedX >= -halfW && rotatedX <= halfW && rotatedY >= -halfH && rotatedY <= halfH);

}

SDL_FPoint getCENTER(SDL_FPoint pos, float texW, float texH)
{
	return { pos.x + texW * 0.5f, pos.y + texH * 0.5f };
}

float randBETWEEN(float min, float max)
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
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

	 //MOVEMENT
	 auto movingSPRITES = spriteREGISTER.view<MOVING>();
	 bool continueMOVING = true;


	 for (auto& sprite : movingSPRITES)
	 {
		 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(sprite);
		 auto& soldierMOVING_INFO = spriteREGISTER.get<MOVING>(sprite);
		 float dist = distanceTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, soldierMOVING_INFO.targetLOC);

		float pX = soldierMOVING_INFO.targetLOC.x - soldierSPRITE_INFO.spriteLOCATION.POS.x;
		float pY = soldierMOVING_INFO.targetLOC.y - soldierSPRITE_INFO.spriteLOCATION.POS.y;

		float DOT = (soldierMOVING_INFO.dX * pX) + (soldierMOVING_INFO.dY * pY);

		if (DOT < 0.0f)
		{
			if (!(soldierSPRITE_INFO.TYPE.uvTYPE == T_VFX_TRACER))
			{
				continueMOVING = false;
				spriteREGISTER.remove<MOVING>(sprite);
			}
			else
			{
				continueMOVING = false;
				toDESTROY.push_back(sprite); //If add to destroy list
			}
		}

		if (continueMOVING)
		{
			SDL_FPoint pos = soldierSPRITE_INFO.spriteLOCATION.POS;
			pos.x += soldierMOVING_INFO.dX * DT;
			pos.y += soldierMOVING_INFO.dY * DT;
			soldierSPRITE_INFO.spriteLOCATION.POS = pos;
		}




		 //ADD: If new enemy is closer switch targets
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
		 auto& soldierSPRITE_INFO = spriteREGISTER.get<hasTARGET>(shootingSOLDIER); //CRASHES GAME: When you remove hasTARGET mid iteration ENTT dosent like that. hasTARGET and isFIRING could be combined
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

	 for (auto corpse : toDESTROY) //cleanup
	 {
		 spriteREGISTER.destroy(corpse);
	 }

 }

 void SPRITE_MANAGER::soldierSHOOT_AT_TARGET(entt::entity soldier)
 {
	 auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 auto& soldiersSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);

	 auto& enemySPRITE = spriteREGISTER.get<spriteOBJECT>(soldiersSHOOTING.enemySOLDIER);

	 if (!spriteREGISTER.all_of<FIRING>(soldier))
	 {
		 FIRING newFIRE;
		 newFIRE.secPER_BULLET = (1.0f / (soldierINFO.weaponRPM / 60.0f));
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
	 if (fireINFO.aimTIME < 0.0 || fireINFO.firingBURST)
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

			 float rangeFACTOR = dist / soldierINFO.weaponEFFECTIVE_RANGE;
			 float baseHIT = 0.95f - (rangeFACTOR * rangeFACTOR) * 0.85f;
			 float finalHIT = baseHIT * soldierINFO.soldierSKILL;
			 finalHIT = std::clamp(finalHIT, 0.01f, 0.99f);
			 //If miss randomize direction

			 float randNUMBER = randBETWEEN(0.0f, 1.0f);

			 if (randNUMBER < finalHIT / 6)
			 {
				 //we hit
				 enemySPRITE.HP -= soldierINFO.weaponDMG;
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
				 SDL_FPoint missPOINT = { enemySPRITE_INFO.spriteLOCATION.POS.x + randBETWEEN(-50.0, 50.0), enemySPRITE_INFO.spriteLOCATION.POS.y + randBETWEEN(-150.0, 250.0) };
				 spawnBULLET(soldier, missPOINT);
			 }

			 fireINFO.TIME_SINCE_LAST_SHOT = 0.0;
		 }
	 }
	 fireINFO.TIME_SINCE_LAST_SHOT += DT; //increase by secs
	 if (!fireINFO.firingBURST)
	 {
		 fireINFO.aimTIME -= DT;
	 }

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

	 entt::entity newBULLET = spriteREGISTER.create();
	 spriteOBJECT newSPRITE_OBJ;
	 ROTATION newROT = soldierINFO.spriteLOCATION.ROT;
	 SDL_FPoint rotatedPOINT = rotatePOINT(offSET_TRACER, newROT);
	 SDL_FPoint bulletPOS = { soldierINFO.spriteLOCATION.POS.x + rotatedPOINT.x,  soldierINFO.spriteLOCATION.POS.y + rotatedPOINT.y };
	 LOCATION newLOC = { bulletPOS, newROT };
	 newSPRITE_OBJ.spriteLOCATION = newLOC;
	 newSPRITE_OBJ.textureSHEET_NUM = HUMAN;
	 newSPRITE_OBJ.texW = 32;
	 newSPRITE_OBJ.texH = 64;
	 newSPRITE_OBJ.TYPE = VFX_BULLET;

	 //EMPLACE MOVING
	 ROTATION dirTOPOINT = directionTO_POINT(bulletPOS, target);
	 MOVING bulletMOVEMENT = newMOVEMENT(bulletSPEED, dirTOPOINT, target);
	 spriteREGISTER.emplace<spriteOBJECT>(newBULLET, newSPRITE_OBJ);
	 spriteREGISTER.emplace<MOVING>(newBULLET, bulletMOVEMENT);

	 //MUZZLE FLASH
	 SDL_FPoint offSET_MUZZLE_FLASH = { 5.0f, -50.0f };
	 SDL_FPoint rotatedPOINT_FLASH = rotatePOINT(offSET_MUZZLE_FLASH, newROT);
	 SDL_FPoint flashPOS = { soldierINFO.spriteLOCATION.POS.x + rotatedPOINT_FLASH.x, soldierINFO.spriteLOCATION.POS.y + rotatedPOINT_FLASH.y };

	 entt::entity newFLASH = spriteREGISTER.create();
	 spriteOBJECT newFLASH_OBJ = newSPRITE_OBJ;
	 float rand = randBETWEEN(0.0, 3.0);
	 if (rand > 0.2){ newFLASH_OBJ.TYPE = VFX_MUZZ_FLASH_1; }
	 else if (rand > 0.1) { newFLASH_OBJ.TYPE = VFX_MUZZ_FLASH_2; }
	 else { newFLASH_OBJ.TYPE = VFX_MUZZ_FLASH_3; }
	 newFLASH_OBJ.spriteLOCATION.POS = flashPOS;
	 newFLASH_OBJ.texW = 32;
	 newFLASH_OBJ.texH = 32;

	 spriteREGISTER.emplace<spriteOBJECT>(newFLASH, newFLASH_OBJ);
	 float randFLASH = randBETWEEN(0.02f, 0.08f);
	 spriteREGISTER.emplace<tempSPRITE>(newFLASH, randFLASH);



	 //CREATE FUNCTION FOR VFX
 }



 //TO DO:

 // - ADD BLOOD
 // - ADD Bullet Casings and mag drops
 // - Soldiers Reload