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
	//divide by length to normalize to 1

	float cosR = dX / length;
	float sinR = dY / length;


	return { -sinR, cosR };
}

ROTATION directionTO_POINT(SDL_FPoint pointA, SDL_FPoint pointB) //normalzied dir
{
	float dX = pointB.x - pointA.x;
	float dY = pointB.y - pointA.y;
	float length = sqrt(dX * dX + dY * dY);
	return { dX / length, dY / length }; // cosR = dx, sinR = dy
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
	float centerX = spritePOS.x + texW / 2;
	float centerY = spritePOS.y + texH / 2;

	float localX = point.x - centerX;//Translate that hoe into local space
	float localY = point.y - centerY;

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

//SPRITE_MANAGER

void SPRITE_MANAGER::spriteCREATE(textureATLAS sheetNUM, spriteTYPE type, SDL_FPoint pos, ROTATION ROT)
{
		entt::entity newSPRITE = spriteREGISTER.create();
		spriteOBJECT newSPRITE_OBJ;
		ROTATION newROT = ROT;
		LOCATION newLOC = { {pos.x, pos.y}, newROT };
		newSPRITE_OBJ.spriteLOCATION = newLOC;
		newSPRITE_OBJ.textureSHEET_NUM = sheetNUM;
		newSPRITE_OBJ.TYPE = type;
		spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);

		if (sheetNUM == HUMAN)
		{
			if (type == TYPE_SOLDIER_STANDING)//FRIENDLY
			{
				soldierOBJECT newSOLDIER;
				newSOLDIER.friendly = true;
				newSOLDIER.HP = 100.0;
				spriteREGISTER.emplace<soldierOBJECT>(newSPRITE, newSOLDIER);
			}
			if (type == TYPE_SOLDIER_ENEMY_STANDING)//ENEMY
			{
				soldierOBJECT newSOLDIER;
				newSOLDIER.friendly = false;
				newSOLDIER.HP = 100.0;
				spriteREGISTER.emplace<soldierOBJECT>(newSPRITE, newSOLDIER);
			}
		} 
		else if (sheetNUM == VFX)
		{
			if (type == VFX_BULLET_TRACER)
			{

			}
		}
}

void SPRITE_MANAGER::tileCREATE(natureTYPE_TILE type, SDL_FPoint pos)
{
	entt::entity newTILE_ENT = spriteREGISTER.create();
	TILE newTILE;
	newTILE.pos = pos;
	newTILE.TYPE = type;
	spriteREGISTER.emplace<TILE>(newTILE_ENT, newTILE);
}

//SOLDIER
 void SPRITE_MANAGER::createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isENEMY)
{
	 if (!isENEMY)
	 {
		 spriteCREATE(HUMAN, TYPE_SOLDIER_STANDING, pos, rot);
	 }
	 else
	 {
		 spriteCREATE(HUMAN, TYPE_SOLDIER_ENEMY_STANDING, pos, rot);
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
		 float dist = distanceTO_POINT(getCENTER(soldierSPRITE_INFO.spriteLOCATION.POS, soldierSPRITE_INFO.texW, soldierSPRITE_INFO.texH), soldierMOVING_INFO.targetLOC);

		SDL_FPoint bulletCENETR = getCENTER(soldierSPRITE_INFO.spriteLOCATION.POS, soldierSPRITE_INFO.texW, soldierSPRITE_INFO.texH);

		float pX = soldierMOVING_INFO.targetLOC.x - bulletCENETR.x;
		float pY = soldierMOVING_INFO.targetLOC.y - bulletCENETR.y;

		float DOT = (soldierMOVING_INFO.dX * pX) + (soldierMOVING_INFO.dY * pY);


		std::cout << "DOT: " << DOT << std::endl;

		if (DOT < 0.0f)
		{
			if (!(soldierSPRITE_INFO.TYPE == VFX_BULLET_TRACER))
			{
				continueMOVING = false;
				spriteREGISTER.remove<MOVING>(sprite);
			}
			else
			{
				continueMOVING = false;
				std::cout << "destroyed bullet\n";
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
			 float rnd = randBETWEEN(0, 1);
			 spriteTYPE corpseTYPE;
			 if (curS.friendly)
			 {
				 //replace w corpse
				 if (rnd > 0.5){ corpseTYPE = DEAD_1;}
				 else { corpseTYPE = DEAD_2; }
				 spriteCREATE(HUMAN, corpseTYPE, curS_SPRITE_INFO.spriteLOCATION.POS, curS_SPRITE_INFO.spriteLOCATION.ROT);
			 }
			 else
			 {
				 if (rnd > 0.5) { corpseTYPE = E_DEAD_1; }
				 else { corpseTYPE = E_DEAD_1; }
				 spriteCREATE(HUMAN, E_DEAD_1, curS_SPRITE_INFO.spriteLOCATION.POS, curS_SPRITE_INFO.spriteLOCATION.ROT);
			 }
			 toDESTROY.push_back(S);
		 }

		 auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(S);

		 if (spriteREGISTER.all_of<hasTARGET>(S)) {
			 spriteINFO.TYPE = curS.friendly ? TYPE_SOLDIER_SHOOTING : TYPE_SOLDIER_ENEMY_SHOOTING;
		 }
		 else if (spriteREGISTER.all_of<MOVING>(S)) {
			 spriteINFO.TYPE = curS.friendly ? TYPE_SOLDIER_STANDING : TYPE_SOLDIER_ENEMY_STANDING;
		 }
		 else {
			 spriteINFO.TYPE = curS.friendly ? TYPE_SOLDIER_STANDING : TYPE_SOLDIER_ENEMY_STANDING;
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
		 spriteREGISTER.emplace<FIRING>(soldier, newFIRE);
	 }
	 //I should really store center in the sprite
	 SDL_FPoint center = getCENTER(soldierSPRITE_INFO.spriteLOCATION.POS, soldierSPRITE_INFO.texW, soldierSPRITE_INFO.texH);
	 SDL_FPoint enemyCENTER = getCENTER(enemySPRITE.spriteLOCATION.POS, enemySPRITE.texW, enemySPRITE.texH);
	 ROTATION newROT = rotationTO_POINT(center, enemyCENTER);
	 soldierSPRITE_INFO.spriteLOCATION.ROT = newROT; //aim at target
	 fireWEAPON(soldier, soldiersSHOOTING);


 }

 void SPRITE_MANAGER::fireWEAPON(entt::entity soldier, hasTARGET target)
 {
	 float baseMISS = 80.0;

	 auto& fireINFO = spriteREGISTER.get<FIRING>(soldier);
	 auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	 if (fireINFO.TIME_SINCE_LAST_SHOT > fireINFO.secPER_BULLET)
	 {

		 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
		 auto& enemySPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(target.enemySOLDIER);
		 auto& enemySPRITE = spriteREGISTER.get<soldierOBJECT>(target.enemySOLDIER);
		 float dist = distanceTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, enemySPRITE_INFO.spriteLOCATION.POS);
		 float soldierSKILL = soldierINFO.soldierSKILL;

		 float rangeFACTOR = dist / soldierINFO.weaponEFFECTIVE_RANGE; // 0=close, 1=effective, 2=double range
		 float baseHIT = 0.95f - (rangeFACTOR * rangeFACTOR) * 0.85f;  // quadratic falloff
		 float finalHIT = baseHIT * soldierINFO.soldierSKILL;           // skill scales it
		 finalHIT = std::clamp(finalHIT, 0.01f, 0.99f);                 // never impossible

		 float randNUMBER = randBETWEEN(0.0f, 1.0f);

		 if (randNUMBER < finalHIT)
		 {
			 //we hit
			 //enemySPRITE.HP -= soldierINFO.weaponDMG;
			 if (enemySPRITE.HP < 0.0)
			 {
				 //stop firing when dead, nolonger have target
				 spriteREGISTER.get<hasTARGET>(soldier).targetDEAD = true;
				 spriteREGISTER.remove<FIRING>(soldier);

				 return;
			 }
		 }

		 //VFX
		 spawnBULLET(soldier, enemySPRITE_INFO.spriteLOCATION.POS);

		 fireINFO.TIME_SINCE_LAST_SHOT = 0.0;
	 }
	 fireINFO.TIME_SINCE_LAST_SHOT += DT; //increase by secs

 }

 void SPRITE_MANAGER::ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS)
 {

	 auto& selcSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	 auto& selcSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	 SDL_FPoint center = getCENTER(selcSOLDIER.spriteLOCATION.POS, selcSOLDIER.texW, selcSOLDIER.texH);

	 ROTATION newROT = rotationTO_POINT(center, globalPOS);
	 selcSOLDIER.spriteLOCATION.ROT = newROT;

	 ROTATION dirTOPOINT = directionTO_POINT(center, globalPOS);

	 MOVING newMOVING_ORDER = newMOVEMENT(selcSOLDIER_INFO.speed, dirTOPOINT, globalPOS);

	 spriteREGISTER.emplace_or_replace<MOVING>(soldier, newMOVING_ORDER); //if already ordered delete previous and replace, god i love entt
 }

 void SPRITE_MANAGER::checkLOS(entt::entity soldier, bool friendly)
 {

	 float losRANGE = 2000.0;

		 auto allSOLDIER = spriteREGISTER.view<soldierOBJECT>();

		 auto& curSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
		 auto& curSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

		 for (auto& enemy : allSOLDIER)
		 {
			 auto& curENEMY = spriteREGISTER.get<spriteOBJECT>(enemy);
			 auto& curENEMY_INFO = spriteREGISTER.get<soldierOBJECT>(enemy);
			 //Skip friendly checks
			 if (friendly == true && curENEMY_INFO.friendly == true) { continue; }
			 if (friendly == false && curENEMY_INFO.friendly == false) { continue; }


			 float dis = distanceTO_POINT(curSOLDIER.spriteLOCATION.POS, curENEMY.spriteLOCATION.POS);

			 if (dis > losRANGE) //WILL Need OBB for tracking rotational squares
			 {
				 continue;
			 }
			 else if (!spriteREGISTER.all_of<hasTARGET>(soldier))
			 {
				 hasTARGET newTARGET = { enemy, false };
				 spriteREGISTER.emplace<hasTARGET>(soldier, newTARGET);
			 }
		 }
 }

 void SPRITE_MANAGER::spawnBULLET(entt::entity soldier, SDL_FPoint target)
 {
	 float bulletSPEED = 2400.0;

	 auto& soldierINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	 SDL_FPoint soldierCENTER = getCENTER(soldierINFO.spriteLOCATION.POS, soldierINFO.texW, soldierINFO.texH);
	 float xOFF = 18.0f;
	 float yOFF = 10.0f;
	 SDL_FPoint bulletPOS = { soldierCENTER.x + (xOFF * soldierINFO.spriteLOCATION.ROT.cosR - yOFF * soldierINFO.spriteLOCATION.ROT.sinR), soldierCENTER.y + (-xOFF * soldierINFO.spriteLOCATION.ROT.sinR + yOFF * soldierINFO.spriteLOCATION.ROT.cosR )};

	 entt::entity newBULLET = spriteREGISTER.create();
	 spriteOBJECT newSPRITE_OBJ;
	 ROTATION newROT = soldierINFO.spriteLOCATION.ROT;
	 LOCATION newLOC = { bulletPOS, newROT };
	 newSPRITE_OBJ.spriteLOCATION = newLOC;
	 newSPRITE_OBJ.textureSHEET_NUM = HUMAN;
	 newSPRITE_OBJ.texW = 32;
	 newSPRITE_OBJ.texH = 64;
	 newSPRITE_OBJ.TYPE = VFX_BULLET_TRACER;

	 ROTATION dirTOPOINT = directionTO_POINT(soldierCENTER, target);

	 MOVING bulletMOVEMENT = newMOVEMENT(bulletSPEED, dirTOPOINT, target);
	 spriteREGISTER.emplace<spriteOBJECT>(newBULLET, newSPRITE_OBJ);
	 spriteREGISTER.emplace<MOVING>(newBULLET, bulletMOVEMENT);

	 //MUZZLE FLASH
	 float yOFF_FLASH = 50.0f;
	 float xOFF_FLASH = 18.0f;
	 SDL_FPoint newFLASH_POS = {
		 soldierCENTER.x + dirTOPOINT.cosR * yOFF_FLASH - dirTOPOINT.sinR * xOFF,
		 soldierCENTER.y + dirTOPOINT.sinR * yOFF_FLASH + dirTOPOINT.cosR * xOFF
	 };
	 entt::entity newFLASH = spriteREGISTER.create();
	 spriteOBJECT newFLASH_OBJ = newSPRITE_OBJ;
	 newFLASH_OBJ.TYPE = VFX_MUZZLE_FLASH_3;
	 newFLASH_OBJ.spriteLOCATION.POS = newFLASH_POS;
	 newFLASH_OBJ.texW = 32;
	 newFLASH_OBJ.texH = 32;

	 spriteREGISTER.emplace<spriteOBJECT>(newFLASH, newFLASH_OBJ);
	 float randFLASH = randBETWEEN(0.02f, 0.08f);
	 spriteREGISTER.emplace<tempSPRITE>(newFLASH, randFLASH);



	 //CREATE FUNCTION FOR VFX
 }