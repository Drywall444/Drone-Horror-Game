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
		} // can add more sheets for vehicles
}

void SPRITE_MANAGER::tileCREATE(textureATLAS sheetNUM, natureTYPE_TILE type, SDL_FPoint pos)
{
	entt::entity newTILE_ENT = spriteREGISTER.create();
	TILE newTILE;
	newTILE.pos = pos;
	newTILE.textureSHEET_NUM = NATURE;
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

	 //MOVEMENT
	 auto movingSPRITES = spriteREGISTER.view<MOVING>();
	 for (auto& sprite : movingSPRITES)
	 {
		 auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(sprite);
		 auto& soldierMOVING_INFO = spriteREGISTER.get<MOVING>(sprite);
		 float dist = distanceTO_POINT(getCENTER(soldierSPRITE_INFO.spriteLOCATION.POS, soldierTEXW, soldierTEXH), soldierMOVING_INFO.targetLOC);


		 if (dist < 10.0) // this needs to be fixed sprites will move over their distance sometimes.
		 {
			 //arrived
			 spriteREGISTER.remove<MOVING>(sprite);

		 }
		 else {
			 SDL_FPoint pos = soldierSPRITE_INFO.spriteLOCATION.POS;
			 pos.x += soldierMOVING_INFO.dX * DT;
			 pos.y += soldierMOVING_INFO.dY * DT;
			 soldierSPRITE_INFO.spriteLOCATION.POS = pos;
			 //move sprite according to their speed
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
				 std::cout << "target not dead, shoot\n";
				 soldierSHOOT_AT_TARGET(shootingSOLDIER);
			 }
			 else {
				 std::cout << "Target Died from someone else\n";
				 soldierSPRITE_INFO.enemySOLDIER = entt::null; //set nothing
				 removeTARGET_LIST.push_back(shootingSOLDIER);
			 }
		 }
		 else {
			 std::cout << "remove target, he dead\n";
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
	 std::vector<entt::entity> toDESTROY;

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
	 SDL_FPoint center = getCENTER(soldierSPRITE_INFO.spriteLOCATION.POS, soldierTEXW, soldierTEXH);
	 SDL_FPoint enemyCENTER = getCENTER(enemySPRITE.spriteLOCATION.POS, soldierTEXW, soldierTEXH);
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
			 std::cout << "hit target\n";
			 enemySPRITE.HP -= soldierINFO.weaponDMG;
			 if (enemySPRITE.HP < 0.0)
			 {
				 //stop firing when dead, nolonger have target
				 spriteREGISTER.get<hasTARGET>(soldier).targetDEAD = true;
				 spriteREGISTER.remove<FIRING>(soldier);

				 return;
			 }
		 }
		 fireINFO.TIME_SINCE_LAST_SHOT = 0.0;
	 }
	 fireINFO.TIME_SINCE_LAST_SHOT += DT; //increase by secs

 }

 void SPRITE_MANAGER::ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS)
 {
	 std::cout << "Moving to: " << globalPOS.x << ", " << globalPOS.y << std::endl;

	 auto& selcSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	 auto& selcSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	 SDL_FPoint center = getCENTER(selcSOLDIER.spriteLOCATION.POS, soldierTEXW, soldierTEXH);

	 ROTATION newROT = rotationTO_POINT(center, globalPOS);
	 selcSOLDIER.spriteLOCATION.ROT = newROT;
	 ROTATION dirTOPOINT = directionTO_POINT(center, globalPOS);

	 float dx = dirTOPOINT.cosR * selcSOLDIER_INFO.speed;
	 float dy = dirTOPOINT.sinR * selcSOLDIER_INFO.speed;

	 MOVING newMOVING_ORDER;
	 newMOVING_ORDER.dX = dx;
	 newMOVING_ORDER.dY = dy;
	 newMOVING_ORDER.targetLOC = globalPOS;

	 spriteREGISTER.emplace_or_replace<MOVING>(soldier, newMOVING_ORDER); //if already ordered delete previous and replace, god i love entt
 }

 void SPRITE_MANAGER::checkLOS(entt::entity soldier, bool friendly)
 {

	 float losRANGE = 1500.0;

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

	 //horrible and bad and horrible
 }