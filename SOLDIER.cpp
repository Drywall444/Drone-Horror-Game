#include "sprite.h"

//MOVEMENT

MOVING SPRITE_MANAGER::newMOVEMENT(float speed, ROTATION dirTO, SDL_FPoint targetLOC)
{
	float dx = dirTO.cosR * speed;
	float dy = dirTO.sinR * speed;

	MOVING newMOVING_ORDER;
	newMOVING_ORDER.dX = dx;
	newMOVING_ORDER.dY = dy;
	newMOVING_ORDER.waypoints.push_back({ targetLOC, {0.0, 0.0} }); //rotation meaingless for now
	newMOVING_ORDER.movementSPEED = speed;

	return newMOVING_ORDER;
}

void SPRITE_MANAGER::moveSPRITES()
{
	auto movingSPRITES = spriteREGISTER.view<MOVING>();

	for (auto& sprite : movingSPRITES)
	{
		auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(sprite);
		auto& soldierMOVING_INFO = spriteREGISTER.get<MOVING>(sprite);


		if (hasARRIVED_AT_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, soldierMOVING_INFO))
		{
			if (soldierMOVING_INFO.destroyAT_TARGET == false)
			{
				if (soldierMOVING_INFO.waypoints.empty()) //if empty
				{
					std::cout << "Stop moving\n";
					spriteREGISTER.emplace_or_replace<IDLE>(sprite); // Emplace before remove otherwise bad shit happens
					spriteREGISTER.remove<MOVING>(sprite);
				}
				else {
					std::cout << "Set IDLE\n";
					soldierMOVING_INFO.isSTOPPED = true;
					spriteREGISTER.emplace_or_replace<IDLE>(sprite);
				}
				//Building check - Will be moved when movement is overhauled

				auto totalBUIDLINGS = spriteREGISTER.view<BUILDING>();
				for (auto& building : totalBUIDLINGS)
				{
					auto& buildingINFO = spriteREGISTER.get<BUILDING>(building);
					if (buildingINFO.soldierINSIDE == sprite) //if we are moving to building and this is our building
					{
						std::cout << "arrived at building\n";
						auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(sprite);
						soldierINFO.coverVALUE = buildingINFO.coverVALUE;
						break;

					}
				}
			}
			else
			{
				toDESTROY.push_back(sprite); //If add to destroy list
			}
		}
		if (!soldierMOVING_INFO.isSTOPPED) //If not stopped
		{
			SDL_FPoint pos = soldierSPRITE_INFO.spriteLOCATION.POS;
			pos.x += soldierMOVING_INFO.dX * DT;
			pos.y += soldierMOVING_INFO.dY * DT;
			soldierSPRITE_INFO.spriteLOCATION.POS = pos;
		}
		else {
			if (soldierMOVING_INFO.cur_waitTIME_AT_WAYPOINT > 0.0)
			{
				std::cout << "Soldier not moving\n";
				soldierMOVING_INFO.cur_waitTIME_AT_WAYPOINT -= DT;
			}
			else {
				soldierMOVING_INFO.cur_waitTIME_AT_WAYPOINT = soldierMOVING_INFO.waitTIME_AT_WAYPOINT;
				soldierMOVING_INFO.isSTOPPED = false;
				spriteREGISTER.remove<IDLE>(sprite); // no longer idle
			}

		}
	}
}

void SPRITE_MANAGER::ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS)
{
	auto& selcSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& selcSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	spriteREGISTER.remove<IDLE>(soldier);

	ROTATION direction = directionTO_POINT(selcSOLDIER.spriteLOCATION.POS, globalPOS); //ALWAYS USE un roated direction
	MOVING newMOVING_ORDER = newMOVEMENT(selcSOLDIER_INFO.speed, direction, globalPOS);
	selcSOLDIER.spriteLOCATION.ROT = rotationTO_POINT(selcSOLDIER.spriteLOCATION.POS, globalPOS);

	spriteREGISTER.emplace_or_replace<MOVING>(soldier, newMOVING_ORDER); //if already ordered delete previous and replace, god i love entt
	if (spriteREGISTER.all_of<hasTARGET>(soldier))
	{
		shootAND_MOVE(soldier);
	}
}

void SPRITE_MANAGER::shootAND_MOVE(entt::entity soldier) //LOOK OVER AND REWRITE
{
	auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& soldierSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);
	auto& soldiersMOVING = spriteREGISTER.get<MOVING>(soldier);

	LOCATION destination = soldiersMOVING.waypoints.back(); // save final target

	float dist = distanceTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, destination.POS);
	if (dist < 450.0) { return; }
	soldiersMOVING.waypoints.clear();
	float wayPOINT_SPACING = 450.0;
	int numOF_WAYPOINTS = dist / wayPOINT_SPACING;

	SDL_FPoint dirNorm = {
		soldiersMOVING.dX / soldiersMOVING.movementSPEED,
		soldiersMOVING.dY / soldiersMOVING.movementSPEED
	};

	soldiersMOVING.waypoints.push_back(destination);

	for (int i = numOF_WAYPOINTS; i > 0; i--)
	{
		SDL_FPoint wpPOS = {
			soldierSPRITE_INFO.spriteLOCATION.POS.x + dirNorm.x * (i * wayPOINT_SPACING),
			soldierSPRITE_INFO.spriteLOCATION.POS.y + dirNorm.y * (i * wayPOINT_SPACING)
		};
		soldiersMOVING.waypoints.push_back({ wpPOS, {0.0f, 0.0f} });
	}
}

//Seperate movement


//LOS

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

	if (removeTARGET) { spriteREGISTER.remove<hasTARGET>(soldier); }
}

//DAMAGE

void SPRITE_MANAGER::soldierTAKE_DAMAGE(entt::entity soldier, float damage)
{
	//spriteREGISTER.get<soldierOBJECT>(soldier).HP -= damage;
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

void SPRITE_MANAGER::soldierSHOOT_AT_TARGET(entt::entity soldier)
{
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	if (soldierINFO.outOF_AMMO) { return; }
	if (!spriteREGISTER.all_of<IDLE>(soldier))
	{
		spriteREGISTER.remove<FIRING>(soldier);
		return;
	}

	auto& soldiersSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);

	// Idle and not firing yet - set up FIRING
	if (!spriteREGISTER.all_of<FIRING>(soldier))
	{
		FIRING newFIRE;
		newFIRE.secPER_BULLET = (1.0f / (soldierINFO.weapon.weaponRPM / 60.0f));
		newFIRE.TIME_SINCE_LAST_SHOT = newFIRE.secPER_BULLET;
		float curDISTANCE = returnDIST_TO_TARGET(soldier, soldiersSHOOTING);
		newFIRE.aimTIME = curDISTANCE / 1000.0;
		spriteREGISTER.emplace<FIRING>(soldier, newFIRE);
	}

	auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& enemySPRITE = spriteREGISTER.get<spriteOBJECT>(soldiersSHOOTING.enemySOLDIER);
	soldierSPRITE_INFO.spriteLOCATION.ROT = rotationTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, enemySPRITE.spriteLOCATION.POS);
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
		if (soldierINFO.friendly) { spriteINFO.TYPE = SOLDIER_RELOADING; } //set animation to reloading basic as hell will change later
		else { spriteINFO.TYPE = ENEMY_SOLDIER_RELOADING; }

		if (soldierINFO.weapon.curRELOAD_TIME < 0.0)
		{
			float randY = randBETWEEN(-15.0, 15.0);
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

			if (randNUMBER < (finalHIT - (finalHIT * enemySPRITE.coverVALUE)) * soldierSKILL)
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
